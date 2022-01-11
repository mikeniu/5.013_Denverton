/** @file
  The PCH Init PEIM implements the PCH PEI Init PPI.

@copyright
  Copyright (c) 2004 - 2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/

#include "PchInitPei.h"
#include "PchSmbus.h"
#include "PchReset.h"
#include "PchUsb.h"
#include "PchSpi.h"
#include "Wdt.h"
#include <MeChipset.h>
#include <IncludePrivate/PchHsio.h>
#include <Library/HeciMsglib.h>
#include <SimicsDebug.h>
#include <IeHeciRegs.h>

//Begin: nCPM WA for A0/A1 only (HSD 1404743896). Remove from B0
#define PUGLSR0_PMC_PCI_MMR_REG  0x000005C0
#define B_AGT10_PWRUNGATE_LTCY   BIT20
#define N_AGT10_PWRUNGATE_LTCY   (BIT21 | BIT20)
//End: nCPM WA for A0/A1 only (HSD 1404743896). Remove from B0

/**
  Check some PCH policies are valid for debugging unexpected problem if these
  values are not initialized or assigned incorrect resource.

  @param[in] PchPolicy    The PCH Policy PPI instance

**/
VOID
PchValidatePolicy (
  IN  PCH_POLICY_PPI *PchPolicy
  )
{
  ASSERT (PchPolicy->Revision == PCH_POLICY_REVISION);
  ASSERT (PchPolicy->AcpiBase != 0);
}

/**
  Build Memory Mapped IO Resource which is used to build E820 Table in LegacyBios,
  the resource range should be preserved in ACPI as well.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
PchPreservedMmioResource (
  VOID
  )
{
  //
  // This function builds a HOB that describes a chunk of system memory.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PCH_PRESERVED_BASE_ADDRESS,
    PCH_PRESERVED_MMIO_SIZE
    );
  //
  // This function builds a HOB for the memory allocation.
  //
  BuildMemoryAllocationHob (
    PCH_PRESERVED_BASE_ADDRESS,
    PCH_PRESERVED_MMIO_SIZE,
    EfiMemoryMappedIO
    );

  return EFI_SUCCESS;
}

/**
  Perform DCI configuration.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
  @param[in] PchPwrmBase                PCH Power Management Base address of this PCH device
**/
VOID
PchDciConfiguration (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi,
  IN  UINT32                            PchPwrmBase
  )
{
  UINT32                                Data32;
  BOOLEAN                               DciEnabled;
  BOOLEAN                               DciDetected;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  DciEnabled  = FALSE;
  DciDetected = FALSE;

  #ifdef PCH_PO_FLAG
  if (PchPolicyPpi->PwrOnSafeMode.Bit.DciSafeRegister) {
    ///
    /// Leave at default value
    ///
  } else {
  #endif // PCH_PO_FLAG
  ///
  /// Set PCR[DCI] + 30h bit[5] to 1, and clear 30h [2,1,0] = 0
  ///
  PchPcrAndThenOr32 (
    PID_DCI, R_PCH_PCR_DCI_PCE,
    (UINT32)~(B_PCH_PCR_DCI_PCE_D3HE | B_PCH_PCR_DCI_PCE_I3E),
    0
    );
  #ifdef PCH_PO_FLAG
  }
  #endif // PCH_PO_FLAG

  ///
  /// BWG 13.6 DCI support
  /// DCI enable configuration
  ///
  /// If (DCI mode enable) {
  ///   Enable flow
  /// } else {
  ///   If (auto mode enabled) {
  ///     Auto flow
  ///   } else {
  ///    Disable flow
  ///   }
  /// }
  ///

  DciEnabled = PchPolicyPpi->DciConfig.DciEn == TRUE;
  ///
  /// If DCI is not eanbled, and DCI AUTO MODE is enable, then detects DCI connected.
  ///
  if ((DciEnabled == FALSE) && (PchPolicyPpi->DciConfig.DciAutoDetect == TRUE)) {
    ///
    /// Detect DCI being avaiable (PCR[DCI] + 0x4[9] == 1 or PCR[DCI] + 0x4[10] == 1)
    ///
    PchPcrRead32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, &Data32);
    DciDetected = (Data32 & (BIT9 | BIT10)) != 0;
  }

  if (DciEnabled || DciDetected) {
    ///
    /// Disable SLP S0 and ModPHY sus power gating feature
    /// Handled in PchPm.c
    ///

    if (DciEnabled) {
      ///
      /// Set PCR[DCI] + 4h bit[4] to 1 if DCI is enabled.
      ///
      PchPcrAndThenOr32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, (UINT32)~0, (B_PCH_PCR_DCI_ECTRL_HDCIEN));
    }
    ///
    /// Set HSWPGCR1, PWRMBASE offset 0x5D0 to 0x80000000.
    /// This step has higher priority than the TraceHub mode disable HSWPGCR1 step.
    ///
    MmioWrite32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1, B_PCH_PWRM_SW_PG_CTRL_LOCK);
  } else {
    ///
    /// Set PCR[DCI] + 4h bit[4] to 0
    ///
    PchPcrAndThenOr32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, (UINT32)~(B_PCH_PCR_DCI_ECTRL_HDCIEN), 0);
  }
  
// Denverton AptioV Override Start: EIP#318724
    ///
    /// Set PCR[DCI] + 4h bit[0] to 1 To Lock the Register.
    ///
    PchPcrAndThenOr32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, (UINT32)~0, BIT0);
// Denverton AptioV Override End:  EIP#318724

  PchPcrRead32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, &Data32);
  DEBUG ((EFI_D_INFO, "\tECTRL = 0x%08x\n",Data32));
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
  Internal function performing miscellaneous init needed in early PEI phase

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchMiscInit (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{
  UINTN                 PciSataRegBase;
  UINTN                 PciPmcRegBase;
  PCH_SERIES            PchSeries;
  UINTN                 SmbusBaseAddress;

  PchSeries        = GetPchSeries ();
  SmbusBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_SMBUS,
                       PCI_FUNCTION_NUMBER_PCH_SMBUS
                       );
  PciPmcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_PMC,
                      PCI_FUNCTION_NUMBER_PCH_PMC
                      );
  PciSataRegBase  = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA
                      );
  ///
  /// Set PMC PCI offset ACh[20] = 0 at early boot
  ///
  MmioAnd32 (PciPmcRegBase + R_PCH_PMC_ETR3, (UINT32)~(B_PCH_PMC_ETR3_CF9GR));


  #ifdef PCH_PO_FLAG
  if (PchPolicyPpi->PwrOnSafeMode.Bit.LpcSafeRegister) {
    ///
    /// Leave at default value
    ///
  } else {
  #endif // PCH_PO_FLAG

  ///
  /// BIOS need to set LPC PCR 0x341C[13,3:0] to all 1's and [8,9.10] = 0's
  ///
  PchPcrAndThenOr32 (PID_LPC, R_PCH_PCR_LPC_PRC, (UINT32)~(BIT10 | BIT9), 0);
  #ifdef PCH_PO_FLAG
  }
  #endif // PCH_PO_FLAG

  ///
  /// BIOS need to set FIA PCR offset 0[17:15] to 111b and offset 0x20[31] = 1b
  ///
  #ifdef PCH_PO_FLAG
  if (PchPolicyPpi->PwrOnSafeMode.Bit.FiaSafeRegister) {
  ///
  /// Leave at default value
  ///
  } else {
  #endif // PCH_PO_FLAG
  ///
  /// Set FIA PCR offset 0x20[31] = 1b
  //
  PchPcrAndThenOr32 (PID_FIA, R_PCH_PCR_FIA_PLLCTL, (UINT32)~0, (BIT31));

  #ifdef PCH_PO_FLAG
  }
  #endif // PCH_PO_FLAG
  ///
  /// Clear PCR PSF_1_PSF_PORT_CONFIG_PG1_PORT7[5] = 0b
  ///
  ///
  /// For SPT-H set SMBus PCI 0x64 = 0x0F060000
  ///
  if (PchSeries == PchH) {
     MmioWrite32 (SmbusBaseAddress + R_PCH_SMBUS_64, 0x0F060000);
  }
  return EFI_SUCCESS;
}

/**
  Internal function performing HPET initin early PEI phase

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
PchHpetInit (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  PCH_HPET_CONFIG                       *HpetConfig;
  UINTN                                 P2sbBase;
  UINT16                                Data16;
  UINT32                                HpetBase;

  HpetConfig      = &PchPolicyPpi->HpetConfig;
  P2sbBase        = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_P2SB,
                      PCI_FUNCTION_NUMBER_PCH_P2SB
                      );

  //
  // Program this field accordingly if unique bus:device:function number is required for the
  // corresponding HPET
  //
  if (HpetConfig->BdfValid) {
    Data16 = ((UINT16) (HpetConfig->BusNumber) << 8) & B_PCH_P2SB_HBDF_BUF;
    Data16 |= ((UINT16) (HpetConfig->DeviceNumber) << 3) & B_PCH_P2SB_HBDF_DEV;
    Data16 |= (UINT16) (HpetConfig->FunctionNumber) & B_PCH_P2SB_HBDF_FUNC;
    MmioWrite16 ((UINTN) (P2sbBase + R_PCH_P2SB_HBDF), Data16);
  }
  //
  // Initial and enable HPET High Precision Timer memory address for basic usage
  // If HPET base is not set, the default would be 0xFED00000.
  //
  HpetBase = HpetConfig->Base;
  if (HpetBase == 0) {
    HpetBase = V_PCH_HPET_BASE0;
  }
  MmioAndThenOr8 (
    P2sbBase + R_PCH_P2SB_HPTC,
    (UINT8)~B_PCH_P2SB_HPTC_AS,
    (UINT8)(((HpetBase >> N_PCH_HPET_ADDR_ASEL) & B_PCH_P2SB_HPTC_AS) | B_PCH_P2SB_HPTC_AE)
    );
  //
  // Read back for posted write to take effect
  //
  MmioRead8 (P2sbBase + R_PCH_P2SB_HPTC);
  //
  // Set HPET Timer enable to start counter spinning
  //
  if (HpetConfig->Enable == TRUE) {
    MmioOr32 (HpetBase + 0x10, 0x1);
  }
  //
  // Build the resource descriptor hob for HPET address resource.
  // HPET only claims 0x400 in size, but the minimal size to reserve memory
  // is one page 0x1000.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    HpetBase,
    0x1000
    );
  BuildMemoryAllocationHob (
    HpetBase,
    0x1000,
    EfiMemoryMappedIO
    );
}

//
// SVID / SID init table entry
//
typedef struct {
  UINT8 DeviceNumber;
  UINT8 FunctionNumber;
  UINT8 SvidRegOffset;
  UINT32 SvidSidValue;
} PCH_SVID_SID_INIT_ENTRY;



STATIC PCH_SVID_SID_INIT_ENTRY  SvidSidInitTable[] = {
  { PCI_DEVICE_NUMBER_PCH_LPC,                  PCI_FUNCTION_NUMBER_PCH_LPC,                 PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_P2SB,                 PCI_FUNCTION_NUMBER_PCH_P2SB,                PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_PMC,                  PCI_FUNCTION_NUMBER_PCH_PMC,                 PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_SMBUS,                PCI_FUNCTION_NUMBER_PCH_SMBUS,               PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_SATA,                 PCI_FUNCTION_NUMBER_PCH_SATA,                PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_SATA2,                PCI_FUNCTION_NUMBER_PCH_SATA2,               PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_SPI,                  PCI_FUNCTION_NUMBER_PCH_SPI,                 PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_TRACE_HUB,            PCI_FUNCTION_NUMBER_PCH_TRACE_HUB,           PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_SCC_EMMC,             PCI_FUNCTION_NUMBER_PCH_SCC_EMMC,            PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { PCI_DEVICE_NUMBER_PCH_XHCI,                 PCI_FUNCTION_NUMBER_PCH_XHCI,                PCI_SVID_OFFSET, V_USE_POLICY_SVID_DID},
  { ME_DEVICE_NUMBER,                           HECI_FUNCTION_NUMBER,                        PCI_SVID_OFFSET, V_ME_HECI1_SVID_DID},
  { ME_DEVICE_NUMBER,                           HECI2_FUNCTION_NUMBER,                       PCI_SVID_OFFSET, V_ME_HECI2_SVID_DID},
  { ME_DEVICE_NUMBER,                           IDER_FUNCTION_NUMBER,                        PCI_SVID_OFFSET, V_ME_MEKT_SVID_DID},
  { ME_DEVICE_NUMBER,                           SOL_FUNCTION_NUMBER,                         PCI_SVID_OFFSET, V_ME_HECI3_SVID_DID},
  { IE_DEVICE_NUMBER,                           IE_HECI1_FUN,                                PCI_SVID_OFFSET, V_IE_HECI1_SVID_DID},
  { IE_DEVICE_NUMBER,                           IE_HECI2_FUN,                                PCI_SVID_OFFSET, V_IE_HECI2_SVID_DID},
  { IE_DEVICE_NUMBER,                           IE_IDER_FUN,                                 PCI_SVID_OFFSET, V_IE_MEKT_SVID_DID},
  { IE_DEVICE_NUMBER,                           IE_KT_FUN,                                   PCI_SVID_OFFSET, V_IE_HECI3_SVID_DID}
};

/**
  Program Pch devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID).

  @param[in] PchPolicy    The PCH Policy PPI instance

  @retval EFI_SUCCESS     The function completed successfully
**/
EFI_STATUS
PchProgramSvidSid (
  IN PCH_POLICY_PPI      *PchPolicy
  )
{
  UINT8                           Index;
  UINT8                           BusNumber;
  UINTN                           PciEAddressBase;
  UINT8                           DeviceNumber;
  UINT8                           FunctionNumber;
  UINT8                           SvidRegOffset;
  UINTN                           SvidSidInitTableSize;
  PCH_SERIES                      PchSeries;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  PchSeries            = GetPchSeries ();
  BusNumber            = DEFAULT_PCI_BUS_NUMBER_PCH;
  SvidSidInitTableSize = sizeof (SvidSidInitTable) / sizeof (PCH_SVID_SID_INIT_ENTRY);

  if ((PchPolicy->PchConfig.SubSystemVendorId != 0) ||
      (PchPolicy->PchConfig.SubSystemId != 0))
  {
    for (Index = 0; Index < SvidSidInitTableSize; Index++) {
      DeviceNumber    = SvidSidInitTable[Index].DeviceNumber;
      FunctionNumber  = SvidSidInitTable[Index].FunctionNumber;
      SvidRegOffset   = SvidSidInitTable[Index].SvidRegOffset;
      PciEAddressBase = MmPciBase (
                          BusNumber,
                          DeviceNumber,
                          FunctionNumber
                          );
      //
      // Skip if the device is disabled
      //
      if (MmioRead16 (PciEAddressBase) != V_PCH_INTEL_VENDOR_ID) {
        continue;
      }
      //
      // Program Pch devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID)
      //
      if(SvidSidInitTable[Index].SvidSidValue != V_USE_POLICY_SVID_DID) {
          MmioWrite32 (
            (UINTN) (PciEAddressBase + SvidRegOffset),
            (UINT32) SvidSidInitTable[Index].SvidSidValue
            );
      } else {
          MmioWrite32 (
            (UINTN) (PciEAddressBase + SvidRegOffset),
            (UINT32) (PchPolicy->PchConfig.SubSystemVendorId |
                     (PchPolicy->PchConfig.SubSystemId << 16))
            );
      }
    }
  }

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}


/**
  This function performs basic initialization for PCH in PEI phase after Policy produced.
  If any of the base address arguments is zero, this function will disable the corresponding
  decoding, otherwise this function will enable the decoding.
  This function locks down the AcpiBase.

  @param[in] SiPolicyPpi   The Silicon Policy PPI instance
  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
EFIAPI
PchOnPolicyInstalled (
  IN  SI_POLICY_PPI   *SiPolicyPpi,
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  )
{
  EFI_STATUS              Status;
  PCH_SERIES              PchSeries;
  UINTN                   LpcBaseAddress;
  UINTN                   PmcBaseAddress;
  UINTN                   SpiBaseAddress;
  UINT8                   HsioMatch;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  HsioMatch = FALSE;

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SPI,
                     PCI_FUNCTION_NUMBER_PCH_SPI
                     );

  ///
  /// Set ACPI Base in PMC device
  ///
  Status = PchAcpiBaseSet (PchPolicyPpi->AcpiBase);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Lock down the ACPI Base
  ///
  MmioOr8 (
    PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B + 2,
    (UINT8) (B_PCH_PMC_GEN_PMCON_B_ACPI_BASE_LOCK >> 16)
    );

  ///
  /// Set PWRM Base in PMC device
  ///
  Status = PchPwrmBaseSet (PCH_PWRM_BASE_ADDRESS);
  ASSERT_EFI_ERROR (Status);

  //
  // Check to disable and lock WDT according to Policy.
  //
  WdtOnPolicyInstalled ((BOOLEAN)PchPolicyPpi->WdtConfig.DisableAndLock);

  ///
  /// Tune the USB 2.0 high-speed signals quality.
  /// This is done before USB Initialization because the PMC XRAM has
  /// to be filled up with USB2 AFE settings followed by HSIO settings
  /// before it is locked
  ///
  Usb2AfeProgramming (&PchPolicyPpi->UsbConfig, SiPolicyPpi->TempMemBaseAddr, PmcBaseAddress);

  //
  // Configure BIOS HSIO if ChipsetInit HSIO Settings in CSME are right
  // The HSIO PPI MUST be ready before this function.
  // The HSIO programming should be done before any IP configuration.
  //
#ifdef ME_SUPPORT_FLAG
  if ( !PEI_IN_SIMICS ) {
    if (PeiHeciInitialize() == EFI_SUCCESS) {
      if (PchHsioChipsetInitProg (FALSE) == EFI_SUCCESS) {
        Status = PchHsioBiosProg (PchPolicyPpi);
        HsioMatch = TRUE;
        ASSERT_EFI_ERROR (Status);
      }
#endif //ME_SUPPORT_FLAG
#ifdef ME_SUPPORT_FLAG
    } else {
      DEBUG ((DEBUG_INFO, "***** PchOnPolicyInstalled -> PeiHeciInitialize ELSE NOT ******\n" ));
      Status = PchHsioBiosProg (PchPolicyPpi);
      HsioMatch = TRUE;
      ASSERT_EFI_ERROR (Status);
    }
  } else
#endif //ME_SUPPORT_FLAG
  {
    Status = PchHsioBiosProg (PchPolicyPpi);
    HsioMatch = TRUE;
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Clear SMM_EISS (SPI PCI offset DCh[5])
  // Clear LPC/eSPI EISS (LPC/eSPI PCI offset DCh[5])
  // Since the HW default is 1, need to clear it when disabled in policy
  //
  MmioAnd8 (SpiBaseAddress + R_PCH_SPI_BC, (UINT8)~B_PCH_SPI_BC_EISS);
  MmioAnd8 (LpcBaseAddress + R_PCH_LPC_BC, (UINT8)~B_PCH_LPC_BC_EISS);

  PchDciConfiguration (PchPolicyPpi, PCH_PWRM_BASE_ADDRESS);
  TraceHubInitialize (PchPolicyPpi);
  TraceHubDisablePunitMgs ();
  //
  // The following function should be executed after PchDciConfiguration()
  //
  TraceHubManagePowerGateControl();

  //
  // Configure SATA controller
  //
  PchSeries                   = GetPchSeries ();
  if(HsioMatch)
  {
    ConfigurePchHSata(
                 #ifdef PCH_PO_FLAG
                 PchPolicyPpi->PwrOnSafeMode.Bit.SataSafeRegister,
                 #endif // PCH_PO_FLAG
                 &PchPolicyPpi->SataConfig[0],
                 PCI_DEVICE_NUMBER_PCH_SATA,
                 FALSE,
                 SiPolicyPpi->TempMemBaseAddr);
    ConfigurePchHSata(
                 #ifdef PCH_PO_FLAG
                 PchPolicyPpi->PwrOnSafeMode.Bit.SataSafeRegister,
                 #endif // PCH_PO_FLAG
                 &PchPolicyPpi->SataConfig[1],
                 PCI_DEVICE_NUMBER_PCH_SATA2,
                 FALSE,
                 SiPolicyPpi->TempMemBaseAddr);
  }

  Status = InitializePchSmbus (PchPolicyPpi);
  
  // Denverton AptioV override start - EIP#292289
  // These values will be programmed later in SbPei.c by using the values from "SB_PCI_DEVICES_SSID_TABLE" SDL Token
  //
  // Program SVID and SID of PCH devices.
  // Program SVID and SID before most PCH device init since some device might be locked after init.
  //
  // Status = PchProgramSvidSid (PchPolicyPpi);
  // ASSERT_EFI_ERROR (Status);
  // Denverton AptioV override End - EIP#292289
  
  Status = PchMiscInit (PchPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  PchHpetInit (PchPolicyPpi);

  if(HsioMatch)
  {
    //
    // Configure PCH xHCI, pre-mem phase
    //
    Status = ConfigureXhciPreMem (PchPolicyPpi, SiPolicyPpi->TempMemBaseAddr);
  }

  ConfigureLpcOnPolicy (PchPolicyPpi);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
 Early init P2SB configuration
 For GPIO and ITSS that support sideband posted write, they can support
 back to back write after their correspoing bit sunder P2SB PCI Config
 80h-9fh are set.
**/
VOID
PchP2sbEarlyConfig (
  VOID
  )
{
  UINTN                   P2sbBase;

  P2sbBase  = MmPciBase (
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_P2SB,
                PCI_FUNCTION_NUMBER_PCH_P2SB
                );
  ///
  /// For GPIO and ITSS that support sideband posted write, they can support
  /// back to back write after their correspoing bit sunder P2SB PCI Config
  /// 80h-9fh are set.
  /// For SKL PCH-LP and SKL PCH-H, program the following at early PCH BIOS init
  /// 1. Set P2SB PCI offset 80h to 0
  /// 2. Set P2SB PCI offset 84h to 0
  /// 3. Set P2SB PCI offset 88h to 0
  /// 4. Set P2SB PCI offset 8Ch to 0
  /// 5. Set P2SB PCI offset 90h to 0
  /// 6. Set P2SB PCI offset 94h to 0000F000h
  /// 7. Set P2SB PCI offset 98h to 00000010h
  /// 8. Set P2SB PCI offset 9Ch to 0
  ///
  MmioWrite32 (P2sbBase + R_PCH_P2SB_80, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_84, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_88, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_8C, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_90, 0);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_94, 0x0000F000);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_98, 0x00000010);
  MmioWrite32 (P2sbBase + R_PCH_P2SB_9C, 0);
  ///
  /// Set P2SB PCI offset 0xF4[0] = 1
  ///
  MmioOr8 (P2sbBase + R_PCH_P2SB_F4, BIT0);
}

/**
  Function disable type 0 controller in PSF
**/
VOID
PsfT0FunctionDisable (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            ShdwRegBase
  )
{
  PchPcrAndThenOr32 (
    Pid,
    ShdwRegBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
    ~0u,
    B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
    );
}

/**
  Function disable type 1 controller in PSF
**/
VOID
PsfT1FunctionDisable (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            ShdwRegBase
  )
{
  PchPcrAndThenOr32 (
    Pid,
    ShdwRegBase + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
    ~0u,
    B_PCH_PCR_PSFX_T1_SHDW_PCIEN_FUNDIS
    );
}


/**
  Internal function performing miscellaneous init needed in very early PEI phase

  @param[in] PchPwrmBase       PCH Power Management Base address of this PCH device

**/
VOID
PchEarlyInit (
  IN  UINT32                      PchPwrmBase
  )
{
  UINTN                   PciPmcRegBase;
  UINT8                   Data8;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));



  ///
  /// Before any interrupt is enabled, set ITSS.MMC, PCR[ITSS] + 3334h[0] = 1b.
  ///
  PchPcrWrite16 (PID_ITSS, R_PCH_PCR_ITSS_MMC, (UINT16)B_PCH_PCR_ITSS_MMC_MSTRMSG_EN);


  ///
  /// For GPIO and ITSS that support sideband posted write, they can support
  /// back to back write after their correspoing bit sunder P2SB PCI Config
  /// 80h-9fh are set.
  ///
  PchP2sbEarlyConfig ();

  ///
  /// PCH BIOS Spec Section 19.3 Power Failure Considerations
  /// RTC_PWR_STS bit, GEN_PMCON_B (PMC PCI offset A4h[2])
  /// When the RTC_PWR_STS bit is set, it indicates that the RTCRST# signal went low.
  /// Software should clear this bit. For example, changing the RTC battery sets this bit.
  /// System BIOS should reset CMOS to default values if the RTC_PWR_STS bit is set.
  /// The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set
  /// before memory initialization. This will ensure that the RTC state machine has been
  /// initialized.
  /// 1. If the RTC_PWR_STS bit is set which indicates a new coin-cell battery insertion or a
  ///    battery failure, steps 2 through 5 should be executed.
  /// 2. Set RTC Register 0Ah[6:4] to 110b or 111b
  /// 3. Set RTC Register 0Bh[7].
  /// 4. Set RTC Register 0Ah[6:4] to 010b
  /// 5. Clear RTC Register 0Bh[7].
  ///
  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );

  if ((MmioRead8 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B) &
      (UINT8) B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    ///
    /// 2. Set RTC Register 0Ah[6:4] to 110b or 111b
    ///
    IoWrite8 (R_PCH_RTC_INDEX, (UINT8) R_PCH_RTC_REGA);
    Data8 = IoRead8 (R_PCH_RTC_INDEX) & (UINT8) ~(BIT6 | BIT5 | BIT4);
    Data8 |= (UINT8) (BIT6 | BIT5);
    IoWrite8 (R_PCH_RTC_TARGET, Data8);
    ///
    /// 3. Set RTC Register 0Bh[7].
    ///
    IoWrite8 (R_PCH_RTC_INDEX, (UINT8) R_PCH_RTC_REGB);
    IoOr8 (R_PCH_RTC_TARGET, (UINT8) B_PCH_RTC_REGB_SET);
    ///
    /// 4. Set RTC Register 0Ah[6:4] to 010b
    ///
    IoWrite8 (R_PCH_RTC_INDEX, (UINT8) R_PCH_RTC_REGA);
    Data8 = IoRead8 (R_PCH_RTC_TARGET) & (UINT8) ~(BIT6 | BIT5 | BIT4);
    Data8 |= (UINT8) (BIT5);
    IoWrite8 (R_PCH_RTC_TARGET, Data8);
    ///
    /// 5. Clear RTC Register 0Bh[7].
    ///
    IoWrite8 (R_PCH_RTC_INDEX, (UINT8) R_PCH_RTC_REGB);
    IoAnd8 (R_PCH_RTC_TARGET, (UINT8) ~B_PCH_RTC_REGB_SET);
  }
  ///
  /// PCH BIOS Spec Section 19.1 Handling Status Registers
  /// System BIOS must set 1b to clear the following registers during power-on
  /// and resuming from Sx sleep state.
  /// - PWRMBASE + Offset 10h[0] = 1b
  ///   Done in ConfigureMiscPm ()
  /// - PWRMBASE + Offset 10h[4] = 1b, needs to be done as early as possible during PEI
  /// - PWRMBASE + Offset 10h[5] = 1b
  ///   Done in ConfigureMiscPm ()
  ///
  MmioWrite32 (
    (UINTN) (PchPwrmBase + R_PCH_PWRM_PRSTS),
    (UINT32) (B_PCH_PWRM_PRSTS_FIELD_1 | B_PCH_PWRM_PRSTS_ME_WD_TMR_STS)
    );
  DEBUG ((EFI_D_INFO, "\tPRSTS = 0x%08x\tValue = 0x%08x\n",PchPwrmBase + R_PCH_PWRM_PRSTS,MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_PRSTS))));

  //
  // Clear CF9GR if it's set in previous boot.
  //
  MmioAnd32 (PciPmcRegBase + R_PCH_PMC_ETR3, (UINT32)~B_PCH_PMC_ETR3_CF9GR);
  DEBUG ((EFI_D_INFO, "\tETR3 = 0x%08x\tValue = 0x%08x\n",PciPmcRegBase + R_PCH_PMC_ETR3,MmioRead32 (PciPmcRegBase + R_PCH_PMC_ETR3)));

  ConfigureLpcOnEarlyPei ();

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return;
}

/**
  Performing Pch early init before PchPlatfromPolicy PPI produced

  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
PchInitPreMem (
  VOID
  )
{
  EFI_STATUS              Status;
  UINT32                  PchPwrmBase;
  UINTN                   P2sbBase;
  UINT32                  PcrBase;
  UINT32                  Puglsr0;
  UINT32                  StPgFuseSsDis;
  UINT32                  StPgFdisPmc1;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Check if Pch is supported
  //
  if (!IsPchSupported ()) {
    DEBUG ((DEBUG_ERROR, "PCH SKU is not supported due to no proper PCH LPC found!\n"));
    ASSERT (FALSE);
  }

  ///
  /// Check if SBREG has been set.
  /// If not, program it.
  ///
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );
  PcrBase  = MmioRead32 (P2sbBase + R_PCH_P2SB_SBREG_BAR);
  if ((PcrBase & B_PCH_P2SB_SBREG_RBA) == 0) {
    DEBUG ((DEBUG_INFO, "SBREG should be programmed before here\n"));
    //
    // Set SBREG base address.
    //
    MmioWrite32 (P2sbBase + R_PCH_P2SB_SBREG_BAR, PCH_PCR_BASE_ADDRESS);
    //
    // Enable the MSE bit for MMIO decode.
    //
    MmioOr8 (P2sbBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  }

  ///
  /// Check if PCH PWRM Base has been set
  ///
  Status = PchPwrmBaseGet (&PchPwrmBase);
  DEBUG ((DEBUG_INFO, "PCH PWRM Base needs to be programmed before here\n"));
  ASSERT (PchPwrmBase != 0);

  if ( !PEI_IN_SIMICS ) {
    if ( PchStepping() == PchHA0 || PchStepping() == PchHA1 ) {
      StPgFuseSsDis = MmioRead32 (PchPwrmBase + R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2);
      StPgFdisPmc1  = MmioRead32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1);

      // Check if nCPM is not disabled through fuse or soft strap (STPG_FUSE_SS_DIS_RD_2[6]) &
      // Check if nCPM is not statically power gated (ST_PG_FDIS_PMC_1[6])
      if ( !(StPgFuseSsDis & B_PCH_PWRM_NCPM_FUSE_SS_DIS) && !(StPgFdisPmc1 & B_PCH_PWRM_NCPM_FDIS_PMC) ) {
        DEBUG ((DEBUG_INFO, "nCPM WA Start\n"));
        DEBUG ((EFI_D_INFO, "\tPchPwrmBase = 0x%08x\n",PchPwrmBase));
        DEBUG ((EFI_D_INFO, "\tSTPG_FUSE_SS_DIS_RD_2 = 0x%08x\tValue = 0x%08x\n",R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2,MmioRead32 (PchPwrmBase + R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2)));
        DEBUG ((EFI_D_INFO, "\tST_PG_FDIS_PMC_1 = 0x%08x\tValue = 0x%08x\n",R_PCH_PWRM_ST_PG_FDIS_PMC_1,MmioRead32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1)));
        DEBUG ((EFI_D_INFO, "\tPUGLSR0 = 0x%08x\tValue = 0x%08x\n",PUGLSR0_PMC_PCI_MMR_REG,MmioRead32 (PchPwrmBase + PUGLSR0_PMC_PCI_MMR_REG)));
        Puglsr0 = MmioRead32 (PchPwrmBase + PUGLSR0_PMC_PCI_MMR_REG);
        if ( (Puglsr0 & (UINT32)N_AGT10_PWRUNGATE_LTCY) != (UINT32)N_AGT10_PWRUNGATE_LTCY) {
          DEBUG ((DEBUG_INFO, "\tApplying WA for nCPM...\n"));
          MmioAndThenOr32(PchPwrmBase + PUGLSR0_PMC_PCI_MMR_REG,(UINT32)~N_AGT10_PWRUNGATE_LTCY,(UINT32)N_AGT10_PWRUNGATE_LTCY);
          DEBUG ((EFI_D_INFO, "\tPUGLSR0 = 0x%08x\tValue = 0x%08x\n",PUGLSR0_PMC_PCI_MMR_REG,MmioRead32 (PchPwrmBase + PUGLSR0_PMC_PCI_MMR_REG)));
          DEBUG ((DEBUG_INFO, "\tRequesting Cold Reset...\n"));
          IoWrite8 ((UINTN) R_PCH_RST_CNT, 0xE);
            // If cold reset for some reason wasn't performed  properly, sends an assert.
            ASSERT (0);
          }
        DEBUG ((DEBUG_INFO, "nCPM WA End\n"));
      }
    }
  }

  ///
  /// Perform miscellaneous init needed in very early PEI phase
  ///
  PchEarlyInit (PchPwrmBase);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return Status;
}

/**
  Show PCH related information
**/
VOID
PchShowInfomation (
  VOID
  )
{
DEBUG_CODE_BEGIN();
  CHAR8           Buffer[32];
  UINT32          BufferSize;
  UINTN           LpcBaseAddress;

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  BufferSize = sizeof (Buffer);
  PchGetSeriesStr (GetPchSeries (), Buffer, &BufferSize);
  DEBUG ((DEBUG_INFO, "PCH Series   : %a\n", Buffer));
  BufferSize = sizeof (Buffer);
  PchGetSteppingStr (PchStepping (), Buffer, &BufferSize);
  DEBUG ((DEBUG_INFO, "PCH Stepping : %a\n", Buffer));
  BufferSize = sizeof (Buffer);
  PchGetSkuStr (MmioRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET), Buffer, &BufferSize);
  DEBUG ((DEBUG_INFO, "PCH SKU      : %a\n", Buffer));
DEBUG_CODE_END();
}

/**
  PCH init pre-memory entry point
**/
VOID
EFIAPI
PchInitPrePolicy (
  VOID
  )
{
  EFI_STATUS Status;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Dump PCH information
  //
  DEBUG_CODE (
  PchShowInfomation ();
  );

  //
  // Build Memory Mapped IO Resource which is used to build E820 Table in LegacyBios.
  // the resource range should be preserved in ACPI as well.
  //
  // Denverton AptioV override Start
  // Resource allocation Done using RSVCHIPSET
  // Status = PchPreservedMmioResource (); 
  // Denverton AptioV override End

  //
  // Initialize WDT and install WDT PPI
  //
  Status = WdtPeiEntryPoint ();

  //
  // Install PCH RESET PPI
  //
  Status = InstallPchReset ();

  //
  // Installs PCH SPI PPI
  //
  Status = InstallPchSpi ();

  //
  // Perform PCH early init
  //
  Status = PchInitPreMem ();

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}
