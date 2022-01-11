/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PpmInitialization.c

Abstract:

  Platform power management initialization code.  This code determines current
  user configuration and modifies and loads ASL as well as initializing chipset
  and processor features to enable the proper power management.

  Acronyms:
    PPM   Platform Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology

--*/

//
// Statements that include other files
//
//#include <Ppm.h>
#include "../Library/Goldmont/GoldmontPpmDefines.h"
#include "SimicsDebug.h"
#include <PowerManagement.h>
#include <PchAccess.h>
#include <CpuRegs.h>
#include <IndustryStandard/Acpi30.h>
#include <Library/AslUpdateLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <CpuPpmLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MmPciLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/BootScriptSave.h>
#include <Protocol/PpmPlatformPolicy.h>
#include <Guid/PowerManagementAcpiTableStorage.h>

//
// This is not a standard protocol, as it is never published.
// It is more of a dynamic internal library.
//
#include <Protocol/PpmProcessorSupport2.h>

#ifndef EFI_DEADLOOP 
 #define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); }
#endif

//
// Global variables
//

//
// PPM policy configurations
//
extern PPM_PLATFORM_POLICY_PROTOCOL mPpmPlatformPolicyProtocol;

//
// FVID Table Information
//
extern UINT16 mNumberOfStates;
extern FVID_TABLE *mFvidPointer;

//
// Power management ACPI base address
//
UINT32 AcpiBase;

//
// Global NVS area (communication buffer between SMM and ASL code)
// This area is special because it is in ACPI NVS memory and should
// not be relocated by the OS.  It is accessed in BS, SMM, and by ASL.
//
extern EFI_GLOBAL_NVS_AREA *mGlobalNvsAreaPtr;

//
// iFSB Frequency
//
UINT16 miFSBFrequency;

//
// Last requested GV state
//
UINT16 mRequestedState;

//
// Default FVID table
// One header field plus states
//
FVID_TABLE mEmptyFvidTable[FVID_MAX_STATES + 1];
FVID_TABLE *mFvidPointer = &mEmptyFvidTable[0];

//
// Timer global data
//
UINT8                          mPpmCstTmrFlags;
UINTN                          mPpmTscCorrFactor;
UINTN                          mPpmTscCorrFactorRem;
UINTN                          mPpmCstTscCorrRem;
UINT64                         mPpmCstTscTicks;

//
// Globals to support updating ACPI Tables
//

EFI_ACPI_TABLE_PROTOCOL       *mAcpiTableProtocol = NULL;

EFI_ACPI_DESCRIPTION_HEADER     *mCpu0IstTable  = NULL;
EFI_ACPI_DESCRIPTION_HEADER     *mApIstTable    = NULL;
EFI_ACPI_DESCRIPTION_HEADER     *mCpu0CstTable  = NULL;
EFI_ACPI_DESCRIPTION_HEADER     *mApCstTable    = NULL;
EFI_ACPI_DESCRIPTION_HEADER     *mCpuPmTable    = NULL;
EFI_ACPI_DESCRIPTION_HEADER     *mCpu0TstTable  = NULL;
EFI_ACPI_DESCRIPTION_HEADER     *mApTstTable  = NULL;

//
// Function prototypes
//
STATIC
UINT32
FindStateFrequency (
  IN  UINT16 RatioSetting
  );

EFI_STATUS
SendInfoCoreToPUnit (
  UINT8 CoreBGF
  );

VOID
InitializePpm (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Initialize the processor power management based on hardware capabilities
  and user configuration settings.

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/
{
  UINT32                      PpmFlagsMask;       // User configuration mask
  // Denverton AptioV Override Start- EIP#312471
  EFI_STATUS                  Status;
  EFI_MP_SERVICES_PROTOCOL    *gMpServices;
  // Denverton AptioV Override End- EIP#312471
  
  //
  // Set PPM initialization global variables
  //
  PpmPreInit (This);

  //
  // Initialize flags for the current processor
  //
  This->SetPpmFlags (This);

  //
  // Save the hardware features.
  //
  This->PpmFeatureFlags = This->PpmFlags;

  //
  // Determine current user configuration
  //
  SetUserConfigurationPpmFlagsMask (&PpmFlagsMask);

  //
  // Modify PpmFlags based on user configuration
  //
  This->PpmFlags &= PpmFlagsMask;
  DEBUG ((EFI_D_ERROR , "SetUserConfigurationPpmFlagsMask\n"));

  //
  // Initialize P states
  //
  if ((This->PpmFlags & PPM_GV3) && (This->InitEist != NULL) ) {
    This->InitEist (This, &mPpmPlatformPolicyProtocol, mFvidPointer);
    mNumberOfStates = mFvidPointer[0].FvidHeader.EistStates;
  }

  //
  // Test for EMTTM supported and requested and initialize if true.
  //
  if ((This->PpmFlags & (PPM_EMTTM | PPM_GV3)) == (PPM_EMTTM | PPM_GV3)) {
    if (This->EnableEmttm != NULL) {
      This->EnableEmttm (This, mFvidPointer);
    }
  }

  //
  // Configure C States setting and Enable C States if C-State setup option is enabled
  //
  This->EnableCStates (This);

  //
  // Initialize the TSC update
  //
  if ((This->PpmFlags & PPM_TSC) && (This->EnableTsc != NULL)) {
    This->EnableTsc (This, mFvidPointer, miFSBFrequency, &mPpmCstTmrFlags, &mPpmTscCorrFactor, &mPpmTscCorrFactorRem);
    mPpmCstTscCorrRem = 0;
  }

  //
  // Initialize thermal features
  //
  This->InitThermal (This, &mPpmPlatformPolicyProtocol);

  //
  // Initialize TM1 before TM2 because some processors (Dothan)
  // only support one at a time, so enabling TM2 later results in TM2
  // instead of TM1.
  //
  if (This->PpmFlags & PPM_TM) {
    This->EnableTm (This);
  }
//  if (This->PpmFlags & PPM_TM2) {
//    This->EnableTm2 (This, mFvidPointer);
//  }
  if (This->PpmFlags & PPM_PROC_HOT) {
    This->EnableProcHot (This);
    
    // Denverton AptioV Override Start- EIP#312471
    // To solve THERM_INTERRUPT MSR value inconsistency in APs
    Status = gBS->LocateProtocol( &gEfiMpServiceProtocolGuid, NULL, &gMpServices );
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR (Status))
        gMpServices->StartupAllAPs (gMpServices, This->EnableProcHot, TRUE, NULL, 0, NULL, NULL);
    // Denverton AptioV Override End- EIP#312471
  }

  //
  // Initialize PPM ASL code
  //
  mGlobalNvsAreaPtr->PpmFlags = This->PpmFlags;
  InitializePpmAcpiTable (This);

  //
  // Send Info Core to PUnit through a mailbox
  //
  SendInfoCoreToPUnit(mPpmPlatformPolicyProtocol.CoreBGF);
 
  //
  // Complete initialization
  //
  PpmPostInit (This);
}

EFI_STATUS
SendInfoCoreToPUnit (
  UINT8 CoreBGF
  )
{
  B2P_MAILBOX_INTERFACE    B2PMailbox;
  UINT16                   Counter = 500;
  UINT16                   PunitRamAddress = 0x1;
  UINT64                   PtrMCHBAR = 0;
  UINT32                   PunitDataMailbox;
  UINT32                   PunitInterfaceMailbox;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  if((CoreBGF != 8) && (CoreBGF != 4) && (CoreBGF != 2)) {
    DEBUG ((DEBUG_INFO, "\tCore-Uncore BGF pointer separation value is invalid or not supported\n"));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Get MCHBAR
  PtrMCHBAR = PciRead32 (PCI_LIB_ADDRESS (MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_HI));
  PtrMCHBAR = LShiftU64 (PtrMCHBAR, MCHBAR64_SHIFT_HI);
  PtrMCHBAR |= (UINTN) PciRead32 (PCI_LIB_ADDRESS (MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_LO));
  // Clear the less significative bit, because only indicate that this register is actived
  PtrMCHBAR &=(UINTN)(~BIT0);
 
  PunitDataMailbox = (UINT32)(PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR);
  PunitInterfaceMailbox = (UINT32)(PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR);

  if (CoreBGF == 8) {
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "\tBIOS_MAILBOX_INTERFACE_MCHBAR = 0x%08x \tValue = 0x%08x\n",BIOS_MAILBOX_INTERFACE_MCHBAR, MmioRead32 (PunitInterfaceMailbox))); 
  
  do {
    B2PMailbox.Data = (UINTN)MmioRead32 (PunitInterfaceMailbox);
    MicroSecondDelay(1000000);
    Counter--;
  } while (B2PMailbox.Bits.RunBusy && (Counter));

  B2PMailbox.Data = CoreBGF;
  MmioWrite32 ((UINTN)(PunitDataMailbox), B2PMailbox.Data);
  DEBUG ((EFI_D_INFO, "\tBIOS_MAILBOX_DATA_MCHBAR = 0x%08x \tValue = 0x%08x\n",BIOS_MAILBOX_DATA_MCHBAR, MmioRead32 (PunitDataMailbox))); 
  B2PMailbox.Data = 0;

  B2PMailbox.Bits.Command = 49;
  B2PMailbox.Bits.Address = PunitRamAddress;
  MmioWrite32 ((UINTN)(PunitInterfaceMailbox), B2PMailbox.Data);
  DEBUG ((EFI_D_INFO, "\tBIOS_MAILBOX_INTERFACE_MCHBAR = 0x%08x \tValue = 0x%08x\n",BIOS_MAILBOX_INTERFACE_MCHBAR, MmioRead32 (PunitInterfaceMailbox)));

  B2PMailbox.Bits.RunBusy = 1;
  MmioWrite32 ((UINTN)(PunitInterfaceMailbox), B2PMailbox.Data);
  DEBUG ((DEBUG_INFO, "\tSetting RUN_BUSY\n"));

  do {
    B2PMailbox.Data = (UINTN)MmioRead32(PunitInterfaceMailbox);
    MicroSecondDelay(10);
    Counter--;
  } while (B2PMailbox.Bits.RunBusy && (Counter));

  if (B2PMailbox.Bits.RunBusy == 0 ) {
    DEBUG ((EFI_D_INFO, "\tSuccessful\n"));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return EFI_SUCCESS;
  }
  
  DEBUG ((EFI_D_INFO, "\tNo Successful\n"));
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
  return EFI_D_ERROR;
  
} 

VOID
PpmPreInit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Initialize global variables used during PPM init
    miFSBFrequency - iFSB frequency.
    mFvidPointer - Set FVID states
    mAcpiTableProtocol - ACPI Table protocol instance

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/
{
  UINTN                   PmcBaseAddress;
  EFI_STATUS                    Status;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Get the ACPI Base Address
  //
  PmcBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_PMC,
                       PCI_FUNCTION_NUMBER_PCH_PMC
                       );
  AcpiBase = MmioRead16 (PmcBaseAddress + R_PCH_PMC_ACPI_BASE) & B_PCH_PMC_ACPI_BASE_BAR;

  //
  // Determine the processor core iFSB frequency
  //
  miFSBFrequency = 100;

  //
  // If specified, create a custom the FVID table.
  // (The settings populating the FVID table may not be correct for the specific processor,
  // and it is up to the user to specify settings applicable to the processor being used.)
  //
  SetMem (mFvidPointer, sizeof (mEmptyFvidTable), 0);
  if (mPpmPlatformPolicyProtocol.CustomVidTable.VidNumber >= 2) {
    CreateCustomFvidTable (This, mFvidPointer);
  }

  //
  // Locate ACPI Table protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &mAcpiTableProtocol);

  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

VOID
SetUserConfigurationPpmFlagsMask (
  IN OUT UINT32 *PpmFlagsMask
  )
/*++

Routine Description:

  Set the PPM flags based on current user configuration

Arguments:

  PpmFlagsMask  Mask of feature options to be enabled as specified by the policy

Returns:

  None

--*/
{
  UINT32 Ecx = 0;
  EFI_CPUID_REGISTER mCpuid0B = { 0, 0, 0, 0 };

  //
  // Initialize flags
  //
  *PpmFlagsMask = 0;
  *PpmFlagsMask |= PPM_TSC;
  *PpmFlagsMask |= PPM_MWAIT_EXT;

  // Retrieve Processor Topology.
  Ecx = 1;
  AsmCpuidEx (EFI_CPUID_CORE_TOPOLOGY, Ecx, &mCpuid0B.RegEax, &mCpuid0B.RegEbx, &mCpuid0B.RegEcx, &mCpuid0B.RegEdx);

  //
  // Configure based on setup values
  //
  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableGv) {
    *PpmFlagsMask |= PPM_GV3;
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableCx) {
    *PpmFlagsMask |= PPM_C1;
    DEBUG ((EFI_D_INFO, "EnableCx = TRUE \n"));
    if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableCxe) {
      *PpmFlagsMask |= PPM_C1E;
      DEBUG ((EFI_D_INFO, "EnableCxe = TRUE \n"));
    }
    if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableC3) {
      *PpmFlagsMask |= PPM_C3;
      DEBUG ((EFI_D_INFO, "EnableC3 = TRUE \n"));
    }
    if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableC6) {
      *PpmFlagsMask |= (PPM_C6|PPM_C3);
      DEBUG ((EFI_D_INFO, "EnableC6 = TRUE \n"));
    }
    if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableC7) {
      *PpmFlagsMask |= (PPM_C7|PPM_C6|PPM_C3);
      DEBUG ((EFI_D_INFO, "EnableC7 = TRUE \n"));
    }
    if (mPpmPlatformPolicyProtocol.FunctionEnables.S0ixSupport) {
      if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableC8) {
        *PpmFlagsMask |= (PPM_C8|PPM_C7|PPM_C6|PPM_C3);
        DEBUG ((EFI_D_INFO, "EnableC8 = TRUE \n"));
      }
      if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableC9) {
        *PpmFlagsMask |= (PPM_C9|PPM_C8|PPM_C7|PPM_C6|PPM_C3);
        DEBUG ((EFI_D_INFO, "EnableC9 = TRUE \n"));
      }
      if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableC10) {
        *PpmFlagsMask |= (PPM_C10|PPM_C9|PPM_C8|PPM_C7|PPM_C6|PPM_C3);
        DEBUG ((EFI_D_INFO, "EnableC10 = TRUE \n"));
      }
    } else { 
      *PpmFlagsMask |= (PPM_C7|PPM_C6|PPM_C3);
      DEBUG ((EFI_D_INFO, "EnableC7 = TRUE \n"));
    }
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableTm) {
    *PpmFlagsMask |= PPM_TM;
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableProcHot) {
    *PpmFlagsMask |= PPM_PROC_HOT;
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableCMP) {
    //if dual core present
    if ((mCpuid0B.RegEbx  & 0xFF) > 1) {
      *PpmFlagsMask |= (PPM_CMP);
      DEBUG ((EFI_D_INFO, "PPM_CMP = TRUE \n"));
    }
    //if Quad core present
    if ((mCpuid0B.RegEbx  & 0xFF) > 2) {
      *PpmFlagsMask |= (PPM_QUAD);
      DEBUG ((EFI_D_INFO, "PPM_QUAD = TRUE \n"));
    }
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableTurboMode) {
    *PpmFlagsMask |= PPM_TURBO;
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableEmttm) {
    *PpmFlagsMask |= PPM_EMTTM;
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.TStatesEnable) {
    *PpmFlagsMask |= PPM_TSTATES;
  }

  if (mPpmPlatformPolicyProtocol.BootInLfm == PPM_DISABLE) {
    *PpmFlagsMask |= PPM_BOOT_P_ST_HFM;
  }

  if (mPpmPlatformPolicyProtocol.FunctionEnables.S0ixSupport) {
    *PpmFlagsMask |= PPM_S0ix;
  }
}

VOID
AcpiPatchSpssNpss (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Patch the SMI _PSS (SPSS) package with performance values and the native _PSS (NPSS) package with the GV3 values

  Uses ratio/VID values from the FVID table to fix up the control values in the NPSS and SPSS.

  (1) Find NPSS or SPSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the NPSS or SPSS AML name object.
  (2) Resize the NPSS and SPSS package.
  (3) Fix up the NPSS and SPSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.
  (6) Renames PSS packages for ACPI utility evaluation

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/
{
  UINT8                           *CurrPtr;
  UINT8                           *EndOfTable;
  UINT8                           i;
  UINT16                          NewPackageLength;
  UINT16                          MaxPackageLength;
  UINT16                          Temp;
  UINT16                          *PackageLength;
  UINT16                          *ScopePackageLengthPtr = NULL;
  UINT32                          *Signature;
  PSS_PACKAGE_LAYOUT              *PssPackage = NULL;

  //
  // Calculate new package length
  //
  NewPackageLength = Temp = (UINT16) (mNumberOfStates * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  MaxPackageLength = (UINT16) (FVID_MAX_STATES * sizeof (PSS_PACKAGE_LAYOUT) + 3);

  mNumberOfStates = mFvidPointer[0].FvidHeader.EistStates;

  //
  // Locate the SSDT package in the IST table
  //
  CurrPtr = (UINT8*) mCpu0IstTable;
  EndOfTable =(UINT8*) (CurrPtr + mCpu0IstTable->Length);
  for (; CurrPtr <= EndOfTable; CurrPtr++) {
    Signature = (UINT32*) (CurrPtr + 1);

    //
    // If we found the the _PR_CPU0 scope, save a pointer to the package length
    //
    if ((*CurrPtr == AML_SCOPE_OP) && (*(Signature + 1) == SIGNATURE_32 ('_', 'P', 'R', '_')) && (*(Signature + 2) == SIGNATURE_32 ('C', 'P', 'U', '0'))) {
      ScopePackageLengthPtr = (UINT16*) (CurrPtr + 1);
    }
    //
    // Patch the native _PSS (NPSS or SPSS) package with the GV3 values
    //
    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('S', 'P', 'S', 'S'))) {
      DEBUG ((EFI_D_ERROR, "S P S S start here\n"));
      //
      // Check table dimensions
      // SPSS and NPSS packages reserve space for FVID_MAX_STATES number of P-states so check if the
      // current number of P- states is more than FVID_MAX_STATES. Also need to update the SSDT contents
      // if the current number of P-states is less than FVID_MAX_STATES.
      //
      NewPackageLength = Temp = (UINT16) ((mNumberOfStates + 1) * sizeof (PSS_PACKAGE_LAYOUT) + 3);
      ASSERT (mNumberOfStates <= FVID_MAX_STATES);
      if (mNumberOfStates < FVID_MAX_STATES) {
        *(CurrPtr + 8) = (UINT8) (mNumberOfStates + 1);
        PackageLength = (UINT16*) (CurrPtr + 6);

        //
        // Update the Package length in AML package length format
        //
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);

        //
        // Move SSDT contents
        //
        CopyMem ((CurrPtr + NewPackageLength), (CurrPtr + MaxPackageLength), EndOfTable - (CurrPtr + MaxPackageLength));

        //
        // Save the new end of the SSDT
        //
        EndOfTable = EndOfTable - (MaxPackageLength - NewPackageLength);
      }

      PssPackage = (PSS_PACKAGE_LAYOUT*) (CurrPtr + 9);
      DEBUG ((EFI_D_ERROR, "mNumberOfStates = %x\n", mNumberOfStates));

      for (i = 1; i <= (mNumberOfStates + 1); i++) {
        //
        // If Turbo mode is supported, add one to the HFM frequency
        //
        PssPackage->Control       = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
        PssPackage->Status        = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
        PssPackage->Power         = (UINT32) mFvidPointer[i].FvidState.Power;

        if ((This->PpmFlags & PPM_TURBO) && (i == 1)) {
          PssPackage->CoreFrequency  = FindStateFrequency (mFvidPointer[i + 1].FvidState.BusRatio);
          PssPackage->CoreFrequency++;
          PssPackage->Control       = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
          PssPackage->Status        = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
        } else if (This->PpmFlags & PPM_TURBO) {
          PssPackage->CoreFrequency  = FindStateFrequency (mFvidPointer[i].FvidState.BusRatio);
          PssPackage->Control       = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
          PssPackage->Status        = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
          PssPackage->Power         = (UINT32) mFvidPointer[i].FvidState.Power;
        } else {
          PssPackage->CoreFrequency  = FindStateFrequency (mFvidPointer[i].FvidState.BusRatio);
        }
          PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;

        DEBUG ((EFI_D_ERROR, "PssPackage->CoreFrequency = %x\n", PssPackage->CoreFrequency));
        DEBUG ((EFI_D_ERROR, "PssPackage->Control = %x\n", PssPackage->Control));
        DEBUG ((EFI_D_ERROR, "PssPackage->Power = %x\n", PssPackage->Power));

        PssPackage->BMLatency = PSTATE_BM_LATENCY;
        PssPackage++;
        }
      }

    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('N', 'P', 'S', 'S'))) {
      DEBUG ((EFI_D_ERROR, "N P S S start here\n"));
      //
      // Check table dimensions
      // SPSS and NPSS packages reserve space for FVID_MAX_STATES number of P-states so check if the
      // current number of P- states is more than FVID_MAX_STATES. Also need to update the SSDT contents
      // if the current number of P-states is less than FVID_MAX_STATES.
      //
      NewPackageLength = Temp = (UINT16) (mNumberOfStates * sizeof (PSS_PACKAGE_LAYOUT) + 3);
      ASSERT (mNumberOfStates <= FVID_MAX_STATES);
      if (mNumberOfStates < FVID_MAX_STATES) {
        *(CurrPtr + 8) = (UINT8) mNumberOfStates;
        PackageLength = (UINT16*) (CurrPtr + 6);

        //
        // Update the Package length in AML package length format
        //
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);

        //
        // Move SSDT contents
        //
        CopyMem ((CurrPtr + NewPackageLength), (CurrPtr + MaxPackageLength), EndOfTable - (CurrPtr + MaxPackageLength));

        //
        // Save the new end of the SSDT
        //
        EndOfTable = EndOfTable - (MaxPackageLength - NewPackageLength);
      }

      PssPackage = (PSS_PACKAGE_LAYOUT*) (CurrPtr + 9);

      for (i = 1; i <= mNumberOfStates; i++) {
        //
        // Update the NPSS table
        //
        if (This->PpmFlags & PPM_TURBO && i == 1) {
          //
          // If Turbo mode is supported, apply the following calculation: P0 = P1 + 1
          //
          PssPackage->CoreFrequency  = FindStateFrequency (mFvidPointer[i + 1].FvidState.BusRatio);
          PssPackage->CoreFrequency += 1;
        } else {
          PssPackage->CoreFrequency  = FindStateFrequency (mFvidPointer[i].FvidState.BusRatio);
        }
        PssPackage->Power = (UINT32) mFvidPointer[i].FvidState.Power;

        PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
        PssPackage->Control       = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));
        PssPackage->Status        = (UINT32) ((mFvidPointer[i].FvidState.BusRatio << 8) | (mFvidPointer[i].FvidState.Vid));

        PssPackage->BMLatency = PSTATE_BM_LATENCY;
        DEBUG ((EFI_D_ERROR, "PssPackage->CoreFrequency = %x\n", PssPackage->CoreFrequency));
        DEBUG ((EFI_D_ERROR, "PssPackage->Control = %x\n", PssPackage->Control));
        DEBUG ((EFI_D_ERROR, "PssPackage->Power = %x\n", PssPackage->Power));
        PssPackage++;
      }
    }
  }
  //
  // Update the Package length in AML package length format
  //
  ASSERT (ScopePackageLengthPtr != NULL);

  CurrPtr                 = (UINT8*) ScopePackageLengthPtr;
  NewPackageLength        = Temp = (UINT16) (EndOfTable - CurrPtr);
  if (ScopePackageLengthPtr != NULL) {
    *ScopePackageLengthPtr  = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
  }
  mCpu0IstTable->Length   = (UINT32) (EndOfTable - (UINT8*) mCpu0IstTable);
}

VOID
PpmPostInit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Completes processor power management initialization
    (1) Initializes the TSC update variables.
    (2) Initializes the GV state for processors.
    (3) Saves MSR state for S3
    (4) Adds a callback (SMI) in S3 resume script to restore the MSR

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/
{
  //
  // Configure Freq Limits
  //
  This->ConfigureFreqLimit (This, &mPpmPlatformPolicyProtocol);

  //
  // Configure Rapl Limits
  //
  This->ConfigureRaplLimit (This, &mPpmPlatformPolicyProtocol);

  //
  // Set Boot P-state based on Policy.
  //
  This->SetBootPState(This);

  //
  // Get Crash Dump If Available
  //
  if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableDumpCrashLog)
  {
    This->GetCrashLogDump(This);
  }
}

VOID
CreateCustomFvidTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer
  )
/*++

Routine Description:

  Create a custom FVID table based on setup options.
  Caller is responsible for providing a large enough table.

Arguments:

  This          Pointer to the protocol instance
  FvidPointer   Table to update, must be initialized.

Returns:

  None

--*/
{
  UINT16 Index;

  //
  // Fill in the table header
  //
  FvidPointer[0].FvidHeader.Stepping     = mPpmPlatformPolicyProtocol.CustomVidTable.VidCpuid;
  FvidPointer[0].FvidHeader.MaxVid       = mPpmPlatformPolicyProtocol.CustomVidTable.VidMaxVid;
  FvidPointer[0].FvidHeader.MaxBusRatio  = mPpmPlatformPolicyProtocol.CustomVidTable.VidMaxRatio;
  FvidPointer[0].FvidHeader.EistStates    = mPpmPlatformPolicyProtocol.CustomVidTable.VidNumber;


  //
  // Fill in the state data
  //
  for (Index = 0; Index < mPpmPlatformPolicyProtocol.CustomVidTable.VidNumber; Index++) {
    FvidPointer[Index + 1].FvidState.State     = Index;
    FvidPointer[Index + 1].FvidState.Vid       = mPpmPlatformPolicyProtocol.CustomVidTable.StateVid[Index];
    FvidPointer[Index + 1].FvidState.BusRatio  = mPpmPlatformPolicyProtocol.CustomVidTable.StateRatio[Index];
  }
}

EFI_STATUS
PatchCpuPmTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Update the SSDT table pointers and config DWORD CFGD with the PpmFlags current configuration value

Arguments:

  This                Pointer to the protocol instance

Returns:

  EFI_SUCCESS         The function completed successfully

--*/
{
  UINT8         *CurrPtr;
  UINT32        *Signature;
  SSDT_LAYOUT   *SsdtPackage = NULL;
  CFGD_LAYOUT   *CfgdPackage = NULL;

  //
  // Locate the SSDT package
  //
  CurrPtr = (UINT8*) mCpuPmTable;
  for (; CurrPtr <= ((UINT8*) mCpuPmTable + mCpuPmTable->Length); CurrPtr++) {
    Signature = (UINT32*) (CurrPtr + 1);
    if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('S', 'S', 'D', 'T')) {
      //
      // Update the SSDT table pointers for dynamically loaded tables
      //
      SsdtPackage = (SSDT_LAYOUT*) CurrPtr;

      //
      // Set the P-State SSDT table information
      //
      SsdtPackage->Cpu0IstAddr  = (UINT32) (UINTN) mCpu0IstTable;
      SsdtPackage->Cpu0IstLen   = mCpu0IstTable->Length;
      SsdtPackage->ApIstAddr    = (UINT32) (UINTN) mApIstTable;
      SsdtPackage->ApIstLen     = mApIstTable->Length;

      //
      // Set the C-State SSDT table information
      //
      SsdtPackage->Cpu0CstAddr  = (UINT32) (UINTN) mCpu0CstTable;
      SsdtPackage->Cpu0CstLen   = mCpu0CstTable->Length;
      SsdtPackage->ApCstAddr    = (UINT32) (UINTN) mApCstTable;
      SsdtPackage->ApCstLen     = mApCstTable->Length;
    }
    //
    // Update the config DWORD CFGD with the PpmFlags current configuration value
    //
    if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('C', 'F', 'G', 'D')) {
      CfgdPackage = (CFGD_LAYOUT*) CurrPtr;
      CfgdPackage->Value = This->PpmFlags;
      break;
    }
  }

  //
  // Assert if we didn't update the PM table
  //
  ASSERT (SsdtPackage != NULL);
  ASSERT (CfgdPackage != NULL);

  return EFI_SUCCESS;
}

VOID
InitializePpmAcpiTable(
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Locate the PPM ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         i;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol = NULL;
  INTN                          Instance;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER   *TempTable;
  UINTN                         TableKey;
  

  //
  // Locate protocol.
  // There is little chance we can't find an FV protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Looking for FV with ACPI storage file
  //
  for (i = 0; i < NumberOfHandles; i++) {

    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gPowerManagementAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //
  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol != NULL);
  if( FwVol == NULL ) {
    return;
  }
  //
  // By default, a table belongs in all ACPI table versions published.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  //
  // Read tables from the storage file.
  //
  Instance = 0;
  CurrentTable = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gPowerManagementAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **)&CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      //
      // Check the table ID to modify the table
      //
      switch (((EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable)->OemTableId) {

        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)):
          mCpu0IstTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          if (mPpmPlatformPolicyProtocol.FunctionEnables.EnableGv) {
            if ((This->PpmFlags & PPM_GV3 )||(This->PpmFlags & PPM_BOOT_P_ST_HFM)) {
              //
              // Patch the SMI _PSS (SPSS) package with performance values and
              // the native _PSS (NPSS) package with the GV3 values
              //
              AcpiPatchSpssNpss (This);
              //
            }
          }
          break;

        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'C', 's', 't', 0)):
          mCpu0CstTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          break;

        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'T', 's', 't', 0)):
          mCpu0TstTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          break;

        case (SIGNATURE_64 ('A', 'p', 'I', 's', 't', 0, 0, 0)):
          mApIstTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          break;

        case (SIGNATURE_64 ('A', 'p', 'C', 's', 't', 0, 0, 0)):
          mApCstTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          break;

        case (SIGNATURE_64 ('A', 'p', 'T', 's', 't', 0, 0, 0)):
          mApTstTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          break;

        case (SIGNATURE_64 ('C', 'p', 'u', 'P', 'm', 0, 0, 0)):
          mCpuPmTable = (EFI_ACPI_DESCRIPTION_HEADER*) CurrentTable;
          break;

        default:
          break;
      }

      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  //Fixed the BSOD "ACPI_BIOS_USE_OS_MEMORY" when Disable CMP function
  Status = gBS->AllocatePool (EfiReservedMemoryType, mApIstTable->Length, (VOID **) &TempTable);
  ASSERT_EFI_ERROR (Status);
  CopyMem (TempTable, mApIstTable, mApIstTable->Length);
  gBS->FreePool (mApIstTable);
  mApIstTable = TempTable;
  AcpiChecksum (mApIstTable, mApIstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));

  Status = gBS->AllocatePool (EfiReservedMemoryType, mApCstTable->Length, (VOID **) &TempTable);
  ASSERT_EFI_ERROR (Status);
  CopyMem (TempTable, mApCstTable, mApCstTable->Length);
  gBS->FreePool (mApCstTable);
  mApCstTable = TempTable;
  AcpiChecksum (mApCstTable, mApCstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));

  //
  // If we are CMP, then the PPM tables are dynamically loaded:
  //   We need to publish the CpuPm table to the ACPI tables, and move the IST and CST
  //   tables that are dynamically loaded to a separate location so that we can fix the
  //   addresses in the CpuPm table.
  // Otherwise (non-CMP)
  //   We need to publish CPU 0 tables only, and IST and CST tables only if IST and CST are enabled
  //
  if (This->PpmFlags & PPM_CMP) {
    //
    // Copy tables to our own location and checksum them
    //

    Status = gBS->AllocatePool (EfiReservedMemoryType, mCpu0IstTable->Length, (VOID **) &TempTable);
    ASSERT_EFI_ERROR (Status);
    CopyMem (TempTable, mCpu0IstTable, mCpu0IstTable->Length);
    gBS->FreePool (mCpu0IstTable);
    mCpu0IstTable = TempTable;
    AcpiChecksum (mCpu0IstTable, mCpu0IstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));

    Status = gBS->AllocatePool (EfiReservedMemoryType, mCpu0CstTable->Length, (VOID **) &TempTable);
    ASSERT_EFI_ERROR (Status);
    CopyMem (TempTable, mCpu0CstTable, mCpu0CstTable->Length);
    gBS->FreePool (mCpu0CstTable);
    mCpu0CstTable = TempTable;
    AcpiChecksum (mCpu0CstTable, mCpu0CstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));

  } else {
    //
    // CMP disabled, so statically load the tables
    //

    //
    // Add IST SSDT if GV3 enabled
    //
    if (This->PpmFlags & PPM_GV3) {
      TableKey = 0;
      Status = mAcpiTableProtocol->InstallAcpiTable (mAcpiTableProtocol, mCpu0IstTable, mCpu0IstTable->Length, &TableKey);
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Add CST SSDT if C states are enabled
    //
    if (This->PpmFlags & PPM_C_STATES) {
      TableKey = 0;
      Status = mAcpiTableProtocol->InstallAcpiTable (mAcpiTableProtocol, mCpu0CstTable, mCpu0CstTable->Length, &TableKey);
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Since we are UP, there is no need for the CPU 1 tables
    //

    //
    // At this time, we can free all tables, since they have been copied into ACPI tables by ACPI support protocol
    //
    gBS->FreePool (mCpu0IstTable);
    gBS->FreePool (mCpu0CstTable);
  }

  //
  // Update the CpuPm SSDT table in the ACPI tables.
  //
  PatchCpuPmTable (This);

  TableKey = 0;
  Status = mAcpiTableProtocol->InstallAcpiTable (mAcpiTableProtocol, mCpuPmTable, mCpuPmTable->Length, &TableKey);
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mCpuPmTable);

  if (This->PpmFlags & PPM_TSTATES) {
    //
    // Load the Cpu0Tst SSDT table in the ACPI tables
    //
    TableKey = 0;
    Status = mAcpiTableProtocol->InstallAcpiTable (mAcpiTableProtocol, mCpu0TstTable, mCpu0TstTable->Length, &TableKey);
    ASSERT_EFI_ERROR (Status);
    gBS->FreePool (mCpu0TstTable);

    //
    // If the CMP is enabled then load the ApTst SSDT table in the ACPI tables
    //
    if (This->PpmFlags & PPM_CMP) {
      TableKey = 0;
      Status = mAcpiTableProtocol->InstallAcpiTable (mAcpiTableProtocol, mApTstTable, mApTstTable->Length, &TableKey);
      ASSERT_EFI_ERROR (Status);
    }
  }
  gBS->FreePool (mApTstTable);

}

STATIC
UINT32
FindStateFrequency (
  IN  UINT16 RatioSetting
  )
/*++

Routine Description:

  Returns the frequency (MHz) of a given state based on the ratio

Arguments:

  RatioSetting  The ratio settings for the state

Returns:

  None

--*/
{
  UINT32  Frequency;

  Frequency = (UINT32)(RatioSetting * miFSBFrequency);

  return Frequency;
}
