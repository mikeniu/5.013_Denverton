/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


Module Name:

  SaInit.c

Abstract:

  This is the driver that initializes the Intel Denverton SA.

--*/
#include "SaInit.h"
#ifdef FSP_FLAG
//
// Notify Descriptor
//
STATIC
EFI_PEI_NOTIFY_DESCRIPTOR  mSaInitNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEventReadyToBootGuid,
    SaEndOfDxeCallback
  }
};
#endif // FSP_FLAG

VOID
SaDxePolicyDump (
  IN SA_DATA_HOB *SaDataHob
  )
{
  DEBUG ((DEBUG_INFO, "\n------------------------ DXE SA Platform Policy Dump Start -----------------\n"));
  DEBUG ((DEBUG_INFO, "\n------------------------ SA PLATFORM CONFIGURATION Begin---------------\n"));
  DEBUG ((DEBUG_INFO, "SA Clock Gating           : %x\n", SaDataHob->SaPlatform.EnableSsaClockGating));
  DEBUG ((DEBUG_INFO, "MSI Redirection Algorithm : %x\n", SaDataHob->SaPlatform.MsiRedirAlgorithm));
  DEBUG ((DEBUG_INFO, "\n------------------------ SA PLATFORM CONFIGURATION End---------------\n"));

  DEBUG ((DEBUG_INFO, "\n------------------------ VTD PLATFORM CONFIGURATION Begin---------------\n"));
  DEBUG ((DEBUG_INFO, "Vtd Enable                : %x\n", SaDataHob->VtdData.VtdEnable));
  DEBUG ((DEBUG_INFO, "Vtd BaseAddress           : %x\n", SaDataHob->VtdData.BaseAddress[0]));
  DEBUG ((DEBUG_INFO, "Vtd RmrrUsbBaseAddress    : %x\n", SaDataHob->VtdData.RmrrUsbBaseAddress));
  DEBUG ((DEBUG_INFO, "Vtd RmrrUsbLimitAddress   : %x\n", SaDataHob->VtdData.RmrrUsbLimitAddress));
  DEBUG ((DEBUG_INFO, "Vtd Interrupt remapping   : %x\n", SaDataHob->VtdData.InterruptRemappingSupport));
  DEBUG ((DEBUG_INFO, "\n------------------------ VTD PLATFORM CONFIGURATION End---------------\n"));

  DEBUG ((DEBUG_INFO, "\n------------------------ DXE SA Platform Policy Dump End -----------------\n"));
}

EFI_STATUS
EFIAPI
#ifndef FSP_FLAG
SaInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
#else
SaInitEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FfsHeader,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
#endif
/*++

Routine Description:

  This is the standard EFI driver point that detects
  whether there is an ICH southbridge in the system
  and if so, initializes the chip.

Arguments:

  ImageHandle             Handle for the image of this driver
  SystemTable             Pointer to the EFI System Table

Returns:

  EFI_SUCCESS             The function completed successfully

--*/
{
  EFI_STATUS                        Status;

  SA_DATA_HOB               *SaDataHob;
#ifndef FSP_FLAG
  EFI_EVENT                 EndOfDxeEvent;
#endif

#ifdef ECP_FLAG
  INITIALIZE_SCRIPT (ImageHandle, SystemTable);
#endif

  DEBUG ((DEBUG_INFO, "SaInitDxe Start\n"));

  ///
  /// Get the platform setup policy.
  ///
  //
  // Get SaDataHob
  //
  SaDataHob = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  ///
  /// Dump SA Platform Policy
  ///
  SaDxePolicyDump (SaDataHob);

#ifndef FSP_FLAG
  ///
  /// LegacyRegion Driver
  ///
  DEBUG ((EFI_D_INFO, "Initializing Legacy Region\n"));
  LegacyRegionInstall (ImageHandle, SystemTable);
  ///
  /// Vtd Initialization
  ///
  DEBUG ((DEBUG_INFO, "Initializing VT-d\n"));
  VtdInit (SaDataHob);

  DEBUG ((DEBUG_INFO, "Initializing SA Miscellaneous Registers\n"));
  SaMiscellaneousRegisters (ImageHandle, SaDataHob);
#endif

  ///
  /// Register callback on EndOfDxe event
  ///
#ifndef FSP_FLAG
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SaEndOfDxeCallback,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR(Status);
#else
  //
  // Install call-back
  //
  Status = (**PeiServices).NotifyPpi (PeiServices, &mSaInitNotifyList[0]);
  ASSERT_EFI_ERROR (Status);
#endif

  DEBUG ((DEBUG_INFO, "SaInitDxe End\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaEndOfDxeCallback (
#ifndef FSP
  IN EFI_EVENT    Event,
  IN VOID         *Context
#else
  IN EFI_PEI_SERVICES                  **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN VOID                              *Ppi
#endif
  )
/**
  This function gets registered as a callback to perform SA configuration security lock

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS      - When event completes successfully.

  **/
{
  UINT64                  Data;
  UINT64                  MchBar;
  UINTN                   McD0BaseAddress;
  UINT8                   IMRCount;

  DEBUG ((EFI_D_INFO, "SaEndOfDxeCallback() Start\n"));
  McD0BaseAddress = MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN);
  MchBar = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_LO) & ~BIT0;

#ifndef SA_PO_FLAG
  UINT32                  IMRBase;
  UINT32                  IMRMask;
  SA_IMR_REGIONS          IMRRegions;
  EFI_STATUS              Status;

#ifndef FSP_FLAG
  MchBar   += (UINT64)LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_HI), 32);
#endif

  //Setup variable for IMR overlap tests
  SaGetImrRegions(&IMRRegions, (UINTN)MchBar);

  //
  // Lock unused IMRs and test for overlap
  //
  for (IMRCount = 0; IMRCount < N_SA_MCHBAR_BIMR_NUM_OF_IMRS; IMRCount++) {
      IMRBase = MmioRead32 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0BASE + (IMRCount * N_SA_MCHBAR_BIMR_OFFSET)));
      IMRMask = MmioRead32 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0MASK + (IMRCount * N_SA_MCHBAR_BIMR_OFFSET)));
      if ( ((IMRBase & B_SA_MCHBAR_BIMR_ADDRESS_MASK) == B_SA_MCHBAR_BIMR_DEFAULT_BASE) &&
           ((IMRMask & B_SA_MCHBAR_BIMR_ADDRESS_MASK) == B_SA_MCHBAR_BIMR_DEFAULT_MASK)) {

          DEBUG ((DEBUG_INFO, "IMR%02d is unused.  Locking...\n", IMRCount));

          //Set WAC and RAC to LOCK_VALUE, followed by CP to LOCK_VALUE to lock-out further changes to all agents
          //CP *must* be set after WAC and RAC
          MmioWrite64 (((UINTN)(MchBar + R_SA_MCHBAR_BIMR0WAC + (IMRCount * N_SA_MCHBAR_BIMR_OFFSET))), N_SA_MCHBAR_BIMR_LOCK_VALUE);
          MmioWrite64 (((UINTN)(MchBar + R_SA_MCHBAR_BIMR0RAC + (IMRCount * N_SA_MCHBAR_BIMR_OFFSET))), N_SA_MCHBAR_BIMR_LOCK_VALUE);
          MmioWrite64 (((UINTN)(MchBar + R_SA_MCHBAR_BIMR0CP + (IMRCount * N_SA_MCHBAR_BIMR_OFFSET))), N_SA_MCHBAR_BIMR_LOCK_VALUE);

      } else {

          //Test for overlap
          Status = SaCheckImrForOverlap(
                     &IMRRegions,
                     IMRCount,
                     (IMRBase & B_SA_MCHBAR_BIMR_ADDRESS_MASK) << N_SA_MCHBAR_BIMR_ADDRESS_LSHIFT,
                     LENGTH_FROM_MASK( (IMRMask & B_SA_MCHBAR_BIMR_ADDRESS_MASK) << N_SA_MCHBAR_BIMR_ADDRESS_LSHIFT,
                                       (IMRBase & B_SA_MCHBAR_BIMR_ADDRESS_MASK) << N_SA_MCHBAR_BIMR_ADDRESS_LSHIFT,
                                       N_SA_MCHBAR_BIMR_ADDRESS) );
          if (EFI_ERROR(Status)) {
              DEBUG ((EFI_D_ERROR, "An error was detected in security checking of IMRs.\n"));
              DEBUG ((EFI_D_ERROR, "See serial log output above to determine the error.\n"));
              CpuDeadLoop();
          }
      }
  }
#endif //SA_PO_FLAG

  //Permission revocation for BIOS and SMM to modify IMRs
  for (IMRCount = 0; IMRCount < N_SA_MCHBAR_BIMR_NUM_OF_IMRS; IMRCount++) {
	//BIMR1CP is skipped. Permission revocation for this IMR is executed by a callback after ExitBootServicesEvent
	if (IMRCount == 1){
		IMRCount++;
	}
    Data = MmioRead64 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0CP + (N_SA_MCHBAR_BIMR_OFFSET * IMRCount)));
    Data &= (~(V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED | V_SA_MCHBAR_BSMR_SMM_OPEN_FOR_IA));
    MmioWrite64 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0CP + (N_SA_MCHBAR_BIMR_OFFSET * IMRCount)), Data);
  }
  // Denverton AptioV Override Start #EIP319702
  MmioOr32 ((UINTN) (McD0BaseAddress + R_SA_TSEG), (UINT32) V_SA_TSEGMB_LOCK_DEFAULT);
  MmioOr32 ((UINTN) (McD0BaseAddress + R_SA_BGSM), (UINT32) V_SA_BGSM_LOCK_DEFAULT);
  // Denverton AptioV Override End #EIP319702
  DEBUG ((EFI_D_INFO, "SaEndOfDxeCallback() End\n"));

  return EFI_SUCCESS;
}

VOID
SaGetImrRegions (
  IN OUT SA_IMR_REGIONS    *IMRRegions,
  IN     UINTN             MchBar
  )
/**
  This function sets up various IMR regions in SA_IMR_REGIONS
  structure.

  @param[in/out] IMRRegions  - Pointer to SA_IMR_REGIONS struct.
  @param[in]     MchBar      - Base of MCH.

  **/
{
  UINTN                   McD0BaseAddress;
  UINT64                  Msr64;
  UINT32                  Reg32;

  ASSERT(IMRRegions != NULL);
  if (IMRRegions == NULL) {
	DEBUG ((EFI_D_ERROR, "SaGetImrRegions() - IMRRegions pointer is null!\n"));
    return;
  }
  //Set GenericIMRCount to 0 to indicate empty list
  IMRRegions->GenericIMRCount = 0;

  ASSERT(MchBar != (UINTN)NULL);
  if (MchBar == (UINTN)NULL) {
    DEBUG ((EFI_D_ERROR, "SaGetImrRegions() - MchBar is null!\n"));
    return;
  }

  McD0BaseAddress = MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN);

  IMRRegions->Prmrr.Base = AsmReadMsr64(EFI_MSR_PRMRR_BASE) & B_EFI_MSR_PRMRR_BASE_ADDR_MASK;
  Msr64 = AsmReadMsr64(EFI_MSR_PRMRR_MASK);
  IMRRegions->Prmrr.Length = LENGTH_FROM_MASK(
                                 Msr64 & B_EFI_MSR_PRMRR_MASK_ADDR_MASK,
                                 IMRRegions->Prmrr.Base,
                                 N_EFI_MSR_PRMRR_MASK);
  if ((Msr64 & B_EFI_MSR_PRMRR_MASK_VLD) == B_EFI_MSR_PRMRR_MASK_VLD) {
    IMRRegions->Prmrr.Enabled = TRUE;
  } else {
    IMRRegions->Prmrr.Enabled = FALSE;
  }
  DEBUG((DEBUG_INFO, "PRMRR Range: 0x%016lx - 0x%016lx | %s\n", IMRRegions->Prmrr.Base, IMRRegions->Prmrr.Base + IMRRegions->Prmrr.Length - 1,
          (IMRRegions->Prmrr.Enabled ? L"Enabled" : L"Disabled")));

  IMRRegions->Bgsm.Base =   (UINT64) (MmioRead32(McD0BaseAddress + R_SA_BGSM) & B_SA_BGSM_BGSM_MASK);
  IMRRegions->Bgsm.Length = (UINT64) (MmioRead32(McD0BaseAddress + R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK) - IMRRegions->Bgsm.Base;
  if (IMRRegions->Bgsm.Length != 0) {
    IMRRegions->Bgsm.Enabled = TRUE;
    DEBUG ((DEBUG_INFO, "BGSM  Range: 0x%016lx - 0x%016lx | %s\n", IMRRegions->Bgsm.Base, IMRRegions->Bgsm.Base + IMRRegions->Bgsm.Length - 1, L"Enabled"));
  } else {
    IMRRegions->Bgsm.Enabled = FALSE;
    DEBUG ((DEBUG_INFO, "BGSM  Range: 0x%016lx                      | %s\n", IMRRegions->Bgsm.Base, L"Disabled"));
  }

  IMRRegions->Tseg.Base   = (UINT64) (MmioRead32(McD0BaseAddress + R_SA_TSEG) & B_SA_TSEGMB_TSEGMB_MASK);
  IMRRegions->Tseg.Length = IMRRegions->Bgsm.Base - IMRRegions->Tseg.Base;
  if (IMRRegions->Tseg.Length != 0) {
    IMRRegions->Tseg.Enabled = TRUE;
    DEBUG ((DEBUG_INFO, "TSEG  Range: 0x%016lx - 0x%016lx | %s\n", IMRRegions->Tseg.Base, IMRRegions->Tseg.Base + IMRRegions->Tseg.Length - 1, L"Enabled"));
  } else {
    IMRRegions->Tseg.Enabled = FALSE;
    DEBUG ((DEBUG_INFO, "TSEG  Range: 0x%016lx           | %s\n", IMRRegions->Tseg.Base, L"Disabled"));
  }

  Reg32 = MmioRead32(MchBar + R_SA_MCHBAR_MOT_OUT_BASE);
  if ((Reg32 & B_SA_MCHBAR_MOT_OUT_BASE_EN) == B_SA_MCHBAR_MOT_OUT_BASE_EN) {
    IMRRegions->Mot.Enabled = TRUE;
  } else {
    IMRRegions->Mot.Enabled = FALSE;
  }
  IMRRegions->Mot.Base =   ((UINT64)Reg32 & B_SA_MCHBAR_MOT_OUT_ADDR_MASK) << N_SA_MCHBAR_MOT_OUT_ADDR_LSHIFT;
  IMRRegions->Mot.Length = LENGTH_FROM_MASK(
         MultU64x32((UINT64)MmioRead32(MchBar + R_SA_MCHBAR_MOT_OUT_MASK) & B_SA_MCHBAR_MOT_OUT_ADDR_MASK, 2^N_SA_MCHBAR_MOT_OUT_ADDR_LSHIFT),
         IMRRegions->Mot.Base,
         N_SA_MCHBAR_MOT_OUT_ADDR);
  DEBUG ((DEBUG_INFO, "MOT   Range: 0x%016lx - 0x%016lx | %s\n", IMRRegions->Mot.Base, IMRRegions->Mot.Base + IMRRegions->Mot.Length - 1,
          (IMRRegions->Mot.Enabled ? L"Enabled" : L"Disabled")));

  return;
}

EFI_STATUS
SaCheckImrForOverlap (
  IN OUT SA_IMR_REGIONS    *IMRRegions,
  IN     UINT8              IMRNumber,
  IN     UINT32             IMRBase,
  IN     UINT32             IMRLength
  )
/**
  This function tests IMRs for overlap with each other and other
  reserved memory regions.

  @param[in/out] IMRRegions  - Pointer to SA_IMR_REGIONS struct.
  @param[in]     IMRNumber   - IMR number being tested.
  @param[in]     IMRBase     - Base of IMR to test.
  @param[in]     IMRLength   - Length of IMR to test.

  @retval EFI_SUCCESS      - When no overlaps are detected.
   EFI_INVALID_PARAMETER   - When a parameter is invalid.
   EFI_SECURITY_VIOLATION  - When an overlap is detected.

  **/
{
  UINT8          IMRCount;

  if (IMRNumber >= N_SA_MCHBAR_BIMR_NUM_OF_IMRS) {
    DEBUG ((DEBUG_ERROR, "IMR%02d passed into SaCheckImrForOverlap() which exceeds total IMRs expected!\n", IMRNumber));
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }

  if (IMRRegions->GenericIMRCount >= N_SA_MCHBAR_BIMR_NUM_OF_IMRS) {
    DEBUG ((DEBUG_ERROR, "Function SaCheckImrForOverlap() called to check more IMRs than expected!\n"));
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "IMR%02d is used | 0x%016x - 0x%016x\n", IMRNumber, IMRBase, IMRBase + IMRLength - 1));

  //Test against all enabled reserved regions in IMRRegions
  if (IMRRegions->Prmrr.Enabled) {
    if (DO_NUMBERS_OVERLAP(IMRBase, IMRBase + IMRLength - 1, IMRRegions->Prmrr.Base, IMRRegions->Prmrr.Base + IMRRegions->Prmrr.Length - 1)) {
      DEBUG ((DEBUG_ERROR, "IMR%02d Overlaps with PRMRR Region!", IMRNumber));
      ASSERT(FALSE);
      return EFI_SECURITY_VIOLATION;
    }
  }

  if (IMRRegions->Bgsm.Enabled) {
    if (DO_NUMBERS_OVERLAP(IMRBase, IMRBase + IMRLength - 1, IMRRegions->Bgsm.Base, IMRRegions->Bgsm.Base + IMRRegions->Bgsm.Length - 1)) {
      DEBUG ((DEBUG_ERROR, "IMR%02d Overlaps with BGSM Region!", IMRNumber));
      ASSERT(FALSE);
      return EFI_SECURITY_VIOLATION;
    }
  }

  if (IMRRegions->Mot.Enabled) {
    if (DO_NUMBERS_OVERLAP(IMRBase, IMRBase + IMRLength - 1, IMRRegions->Mot.Base, IMRRegions->Mot.Base + IMRRegions->Mot.Length - 1)) {
      DEBUG ((DEBUG_ERROR, "IMR%02d Overlaps with MOT Region!", IMRNumber));
      ASSERT(FALSE);
      return EFI_SECURITY_VIOLATION;
    }
  }

  if (IMRRegions->Tseg.Enabled) {
    if (DO_NUMBERS_OVERLAP(IMRBase, IMRBase + IMRLength - 1, IMRRegions->Tseg.Base, IMRRegions->Tseg.Base + IMRRegions->Tseg.Length - 1)) {
      DEBUG ((DEBUG_ERROR, "IMR%02d Overlaps with TSEG Region!", IMRNumber));
      ASSERT(FALSE);
      return EFI_SECURITY_VIOLATION;
    }
  }

  //Test against previously examined IMRs
  for (IMRCount = 0; IMRCount < IMRRegions->GenericIMRCount; IMRCount++) {
    if (DO_NUMBERS_OVERLAP(IMRBase, IMRBase + IMRLength - 1,
                           IMRRegions->GenericIMR[IMRCount].Base,
                           IMRRegions->GenericIMR[IMRCount].Base + IMRRegions->GenericIMR[IMRCount].Length - 1)) {
      DEBUG ((DEBUG_ERROR, "IMR%02d Overlaps with IMR%02d!", IMRNumber, IMRRegions->GenericIMR[IMRCount].IMRNumber));
      ASSERT(FALSE);
      return EFI_SECURITY_VIOLATION;
    }
  }

  //If no overlaps, add current IMR to the list for future tests
  IMRRegions->GenericIMR[IMRRegions->GenericIMRCount].Base = IMRBase;
  IMRRegions->GenericIMR[IMRRegions->GenericIMRCount].Length = IMRLength;
  IMRRegions->GenericIMR[IMRRegions->GenericIMRCount].IMRNumber = IMRNumber;
  IMRRegions->GenericIMRCount++;

  return EFI_SUCCESS;
}

VOID
SaMiscellaneousRegisters (
#ifndef FSP_FLAG
  IN EFI_HANDLE      ImageHandle,
#endif
  IN SA_DATA_HOB     *SaDataHob
  )
/**
  Configure SA Registers: A-Unit, B-Unit and T-Unit.

  @param[in] DxePlatformSaPolicy            SA DxePlatformPolicy protocol

**/
{
  UINT64                    MchBar;
  UINTN                     McD0BaseAddress;
  UINT8                     MsiRedirAlgorithmNewValue;
  UINT32                    Bisocwt;
  UINT32                    ReadOrderingCfg;
  UINT32                    WriteOrderingCfg;
  UINT32					Bisoc;
  UINT32					BestEffortMaxLatency;
  UINT32					BCoscat;
  UINT32					Index;

  McD0BaseAddress = MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN);

  MchBar = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_LO) & ~BIT0;
#ifndef FSP_FLAG
  MchBar   += (UINT64)LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_HI), 32);
#endif

  MsiRedirAlgorithmNewValue = 0;
  Bisocwt = 0;
  ReadOrderingCfg = 0;
  WriteOrderingCfg = 0;
  Bisoc				=0;
  BestEffortMaxLatency = 0;
  BCoscat = 0;
  Index = 0;

  //
  // MSI redirection algorithm options
  //
  MsiRedirAlgorithmNewValue = MmioRead8((UINTN)(MchBar + (MCHBAR_RANGE_TUNIT + TUNIT_INTR_REDIR_CTL)));
  MsiRedirAlgorithmNewValue &= V_MASK_INTR_REDIR_CTL_MCHBAR_FIXED;	//set 0

  if (SaDataHob->SaPlatform.MsiRedirAlgorithm == TUNIT_MSI_ALGORITH_ROUNDROBIN) {
    MsiRedirAlgorithmNewValue |= V_MASK_INTR_REDIR_CTL_MCHBAR_RR;	//set 1
  }
  DEBUG ((EFI_D_INFO, "MSI  %X %X\n", MsiRedirAlgorithmNewValue, SaDataHob->SaPlatform.MsiRedirAlgorithm ));
  MmioWrite8((UINTN)(MchBar + (MCHBAR_RANGE_TUNIT + TUNIT_INTR_REDIR_CTL)), MsiRedirAlgorithmNewValue);

  //Configuring Scheduler Latency
  Bisoc = MmioRead32(PCH_PCR_ADDRESS (PID_BUNIT0, R_BUNIT_BISOC ));
  Bisoc &= ~B_BUNIT_BISOC_SCHEDULER_LATENCY;
  Bisoc |= ((UINT32)SaDataHob->SaPlatform.SchedulerLatency) << N_BUNIT_BISOC_SHEDULER_LATENCY_B;

  MmioWrite32(PCH_PCR_ADDRESS (PID_BUNIT0, R_BUNIT_BISOC ), Bisoc);
  DEBUG ((EFI_D_INFO, "Scheduler Latency new value:  0x%X\n", SaDataHob->SaPlatform.SchedulerLatency));

  //Configuring Best Effort Max Latency
  BestEffortMaxLatency  = MmioRead32(PCH_PCR_ADDRESS (PID_BUNIT0, R_BUNIT_BSCHCTRL0));
  BestEffortMaxLatency &= ~B_BUNIT_BSCHCTRL0_BEST_EFFORT_MAX_LAT;
  BestEffortMaxLatency |= (SaDataHob->SaPlatform.BestEffortMaxLat) << N_BUNIT_BSCHCTRL0_BEST_EFFORT_MAX_LAT_B;

  DEBUG ((EFI_D_INFO, "Best effort max latency new value:  0x%X\n", SaDataHob->SaPlatform.BestEffortMaxLat));
  MmioWrite32(PCH_PCR_ADDRESS (PID_BUNIT0, R_BUNIT_BSCHCTRL0), BestEffortMaxLatency);

  //Configuring Cos Cat Agent N
  for(Index = 0; Index < COS_CAT_AGENT_COUNT ; Index++) {
	  BCoscat|= SaDataHob->SaPlatform.CosCatAgent[Index] << (BUNIT_BCOSCAT_WIDTH * Index);
	  DEBUG ((EFI_D_INFO, "COS Cat Agent#%d new value:  0x%X\n", Index, SaDataHob->SaPlatform.CosCatAgent[Index]));
  }
  MmioWrite32(PCH_PCR_ADDRESS(PID_BUNIT0, R_BUNIT_BCOSCAT), BCoscat);

 if (!DXE_IN_SIMICS) {
	//
	// Program channel ordering
	// Vc0a is in-order and Vc0mmu is out-of-order
	//
	ReadOrderingCfg = MmioRead32((UINTN)(MchBar + (MCHBAR_RANGE_TUNIT + TUNIT_VC_READ_ORDERING)));
	ReadOrderingCfg |= V_UPSTREAM_VC0A_IN_ORDER;
	ReadOrderingCfg &= ~V_UPSTREAM_VC0MMU_IN_ORDER;
	MmioWrite32((UINTN)(MchBar + (MCHBAR_RANGE_TUNIT + TUNIT_VC_READ_ORDERING)), ReadOrderingCfg);

	WriteOrderingCfg = MmioRead32((UINTN)(MchBar + (MCHBAR_RANGE_TUNIT + TUNIT_VC_WRITE_ORDERING)));
	WriteOrderingCfg |= V_UPSTREAM_VC0A_IN_ORDER;
	WriteOrderingCfg &= ~V_UPSTREAM_VC0MMU_IN_ORDER;
	MmioWrite32((UINTN)(MchBar + (MCHBAR_RANGE_TUNIT + TUNIT_VC_WRITE_ORDERING)), WriteOrderingCfg);

	//
	// Disable Isoc Weights
	//
	Bisocwt = MmioRead32((UINTN)(MchBar + (MCHBAR_RANGE_BUNIT + BUNIT_BISOCWT)));
	Bisocwt &= ~V_BISOCWT_ENABLE_ISOC_WEIGHTS;
	MmioWrite32((UINTN)(MchBar + (MCHBAR_RANGE_BUNIT + BUNIT_BISOCWT)), Bisocwt);
  }

#ifndef FSP_FLAG
  //
  // If SSA Clock Gating Setup Option is enabled, then configure Clock Gating registers as enabled,
  // if setup option is'nt enabled or code is not compiled, all Clock Gating registers are disabled
  // by default within the Architecture, so there is no necessary any action.
  //

  if (SaDataHob->SaPlatform.EnableSsaClockGating) {

    PchPcrAndThenOr32 (
	  PID_AUNIT0,                              // A Unit clock gating Port Id
	  R_AUNIT_PGCB_CLKGATE_DIS,                // A Unit clock gating Register
	  V_AUNIT_PGCB_CLKGATE_DIS_MASK,           // A Unit clock gating Mask
	  V_AUNIT_PGCB_CLKGATE_DIS_EN              // A Unit clock gating enable value
	  );

    PchPcrAndThenOr32 (
	  PID_BUNIT0,                              // B Unit clock gating Port Id
	  R_BUNIT_BCTRL,                           // B Unit clock gating Register
	  V_BUNIT_BCTRL_MASK,                      // B Unit clock gating Mask
	  V_BUNIT_BCTRL_EN                         // B Unit clock gating enable value
	  );

	PchPcrAndThenOr32 (
	  PID_CUNIT,                               // C Unit clock gating Port Id
	  R_CUNIT_LOCAL_CONTROL_MODE,              // C Unit clock gating Register
	  V_CUNIT_LOCAL_CONTROL_MODE_MASK,         // C Unit clock gating Mask
	  V_CUNIT_LOCAL_CONTROL_MODE_EN            // C Unit clock gating enable value
	  );

	PchPcrAndThenOr32 (
	  PID_TUNIT2,                              // T Unit clock gating Port Id
	  R_TUNIT_CR_CLKGATE_CTL,                  // T Unit clock gating Register
	  V_TUNIT_CR_CLKGATE_CTL_MASK,             // T Unit clock gating Mask
	  V_TUNIT_CR_CLKGATE_CTL_EN                // T Unit clock gating enable value
	  );

  }

#endif // FSP_FLAG
}
