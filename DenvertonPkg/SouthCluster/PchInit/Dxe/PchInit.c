/** @file
  This is the Common driver that initializes the Intel PCH.

@copyright
  Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
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
#include "PchInit.h"

#include <Cpu/Include/CpuRegs.h>
#include <PcieIpRegs.h>

//
// Module variables
//
extern UINT16                                          mPcieIoTrapAddress;

GLOBAL_REMOVE_IF_UNREFERENCED PCH_POLICY_HOB           *mPchPolicyHob;
GLOBAL_REMOVE_IF_UNREFERENCED SI_POLICY_HOB            *mSiPolicyHob;

//
// EFI_EVENT
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_EVENT mHeciEvent;



VOID
ConfigureWakeEvents()
{
	UINT16	AcpiBase;
	UINT32	PchPwrmBase;
	UINTN   PciPmcRegBase;
	PciPmcRegBase = MmPciBase (
			DEFAULT_PCI_BUS_NUMBER_PCH,
			PCI_DEVICE_NUMBER_PCH_PMC,
			PCI_FUNCTION_NUMBER_PCH_PMC
	);

	DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
	DEBUG ((DEBUG_INFO, "Enable RTC wake\n"));

  PchAcpiBaseGet(&AcpiBase);
  PchPwrmBaseGet(&PchPwrmBase);

  DEBUG ((EFI_D_INFO, "\tGPE0_STS_127_96 = 0x%08x\tValue = 0x%08x\n",
    (AcpiBase + R_PCH_ACPI_GPE0_STS_127_96),
    IoRead32 (AcpiBase + R_PCH_ACPI_GPE0_STS_127_96)));

	//
	//Set RTC_EN bit in PM1_EN to wake up from the alarm
	//
    // Denverton AptioV Override Start - EIP#302008
    // Remove setting of RTC_EN bit. Setting this causes and unhandled SMI to
    // occur with a failed PXE boot causing to system to hang
    //IoOr16 (AcpiBase + R_PCH_ACPI_PM1_EN, B_PCH_ACPI_PM1_EN_RTC);
    //DEBUG ((EFI_D_INFO, "\tPM1_EN_STS = 0x%08x\tValue = 0x%08x\n",
      //AcpiBase + R_PCH_ACPI_PM1_STS,
      //(IoRead16 (AcpiBase + R_PCH_ACPI_PM1_EN) << 16) | IoRead16 (AcpiBase + R_PCH_ACPI_PM1_STS)));
    // Denverton AptioV Override End - EIP#302008
	
  //
	//PMC ROW Enable
  //

	//Send message to PMC to determine if ROW is enabled
	MmioWrite32(PchPwrmBase + R_PCH_PMC_MTPMC_OFFSET, (UINT32)0x3);

	//Wait until PMC_MSG_FULL_STS is cleared. Polling PMC_MSG_FULL_STS
	while(MmioRead32(PchPwrmBase + R_PCH_PM_STS_OFFSET) & BIT24){
		MicroSecondDelay (0x01);
	}

	//Determine if PMC ROW is enabled
	if(MmioRead32(PchPwrmBase + R_PCH_PMC_MFPMC_OFFSET) == (UINT32)0x1) {
    MmioOr32 ((UINTN) (PchPwrmBase + R_PCH_PMC_GPE0_EN_OFFSET), BIT16);
		
    //Signal PMC to enable ROW capability
		MmioWrite32(PchPwrmBase + R_PCH_PMC_MTPMC_OFFSET, (UINT32)0x4); 

		//Wait until PMC_MSG_FULL_STS is cleared. Polling PMC_MSG_FULL_STS
		while(MmioRead32(PchPwrmBase + R_PCH_PM_STS_OFFSET) & BIT24){
			MicroSecondDelay (0x01);
		}
		DEBUG ((DEBUG_INFO, "\tROW: ROW was enabled\n"));
	}
	else{
		DEBUG ((DEBUG_WARN, "\tROW: ROW was not enabled\n"));
	}

	//
	//Wake on LAN
	//
	MmioAnd32((UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B), (UINT32)~B_PCH_PMC_GEN_PMCON_B_PME_B0_S5_DIS);
	IoOr32((UINTN) (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96), (UINT32)B_PCH_ACPI_GPE0_EN_127_96_LAN_WAKE);

	//
	//Wake on PME
	//
  IoOr32 ((UINTN) (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96), (UINT32)B_PCH_ACPI_GPE0_EN_127_96_PME_B0);

	//
  //Wake on PME EN
	//
  IoOr32 ((UINTN) (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96), (UINT32)B_PCH_ACPI_GPE0_EN_127_96_PME);
  DEBUG ((EFI_D_INFO, "\tGPE0_EN_127_96 = 0x%08x\tValue = 0x%08x\n",
    (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96),
    IoRead16 (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96)));

	//
	//Wake on USB
	//
	MmioOr32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_CFG2), (UINT32)B_PCH_PWRM_CFG2_USB_DSX_PER_PORT_EN);
	DEBUG ((EFI_D_INFO, "\tPM_CFG2 = 0x%08x\tValue = 0x%08x\n",
    (PchPwrmBase + R_PCH_PWRM_CFG2),
    MmioRead32 ((UINTN)(PchPwrmBase + R_PCH_PWRM_CFG2))));

	DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}
/**
  <b>PchInit DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
      The PchInit module is a DXE driver that initializes the Intel Platform Controller Hub
      following the PCH BIOS specification and EDS requirements and recommendations. It consumes
      the PCH_POLICY_HOB SI_POLICY_HOB for expected configurations per policy.
      This is the standard EFI driver point that detects whether there is an supported PCH in
      the system and if so, initializes the chipset.

  - <b>Details</b>\n
    This module is required for initializing the Intel Platform Controller Hub to
    follow the PCH BIOS specification and EDS.
    This includes some initialization sequences, enabling and disabling PCH devices,
    configuring clock gating, RST PCIe Storage Remapping, SATA controller, ASPM of PCIE devices. Right before end of DXE,
    it's responsible to lock down registers for security requirement.

  - @pre
    - PCH PCR base address configured
    - EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.

  - @result
    - Publishes the @link _PCH_INFO_PROTOCOL PCH_INFO_PROTOCOL @endlink
    - Publishes the @link _PCH_EMMC_TUNING_PROTOCOL PCH_EMMC_TUNING_PROTOCOL @endlink

  - <b>References</b>\n
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink.
    - @link _SI_POLICY_STRUCT SI_POLICY_HOB @endlink.

  - <b>Integration Checklists</b>\n
    - Verify prerequisites are met. Porting Recommendations.
    - No modification of this module should be necessary
    - Any modification of this module should follow the PCH BIOS Specification and EDS


**/
VOID
PchInitEntryPoint (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS  HobPtr;

  DEBUG ((DEBUG_INFO, "Common PchInitEntryPoint() Start\n"));

  //
  // Get PCH Policy HOB.
  //
  HobPtr.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mPchPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);

  //
  // Get Silicon Policy data HOB
  //
  HobPtr.Guid   = GetFirstGuidHob (&gSiPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mSiPolicyHob  = GET_GUID_HOB_DATA (HobPtr.Guid);

  ConfigureWakeEvents ();
  DEBUG ((DEBUG_INFO, "Common PchInitEntryPoint() End\n"));

  return;
}

/**
  Process all the lock downs

  @retval     EFI_SUCCESS        All lock downs successful
  @retval     Others             Not successful
**/
EFI_STATUS
ProcessAllLocks (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT8       Data8;
  UINT16      Data16And;
  UINT16      Data16Or;
  volatile UINT32      Data32;
  UINT32      Data32And;
  UINT32      Data32Or;
  UINT32      DlockValue;
  UINTN       PciLpcRegBase;
  UINTN       PciSpiRegBase;
  UINTN       PciPmcRegBase;
  UINT16      ABase;
  UINT16      TcoBase;
  UINT32      PchPwrmBase;
  UINTN       PchSpiBar0;
  PCH_SERIES  PchSeries;
  UINT64      WaitCount;
  UINTN       PciP2sbRegBase;
  UINT16      DisableIOSFEndPointsIDs[] = {
                                           PID_GLM0,
                                           PID_GLM1,
                                           PID_GLM2,
                                           PID_GLM3,
                                           PID_GLM4,
                                           PID_GLM5,
                                           PID_GLM6,
                                           PID_GLM7,
                                           PID_PML,
                                           PID_THERMGLM0,
                                           PID_THERMGLM1,
                                           PID_THERMGLM2,
                                           PID_THERMGLM3,
                                           PID_THERMGLM4,
                                           PID_THERMGLM5,
                                           PID_THERMGLM6,
                                           PID_THERMGLM7,
                                           PID_THERMDDR0,
                                           PID_THERMDDR1,
                                           PID_THERMWM8,
                                           PID_THERMWM16,
                                           PID_THERMGPIO,
                                           PID_FIA                                           
                                          };
  UINTN       DisableIOSFEndPointsIDsEntries = 0;
  UINTN       i;

  Status = EFI_SUCCESS;

  PciLpcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC
                      );
  PciPmcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_PMC,
                      PCI_FUNCTION_NUMBER_PCH_PMC
                      );
  PciSpiRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SPI,
                      PCI_FUNCTION_NUMBER_PCH_SPI
                      );
  MmioOr8 (PciSpiRegBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE );
  MmioWrite32 (PciSpiRegBase + R_PCH_SPI_BAR0, PCH_SPI_BASE_ADDRESS);
  PchSpiBar0      = MmioRead32 (PciSpiRegBase + R_PCH_SPI_BAR0) &~(B_PCH_SPI_BAR0_MASK);

  PchAcpiBaseGet (&ABase);
  PchTcoBaseGet (&TcoBase);

  PchSeries = GetPchSeries ();

#ifndef FSP_FLAG
///
/// Set PWRMBASE + 620h [31] to lock the ST and NST PG register fields.
///

#endif //!FSP_FLAG


  ///
  /// Program the Flash Protection Range Register based on policy
  ///
  DlockValue = MmioRead32 (PchSpiBar0 + R_PCH_SPI_DLOCK);
  for (Index = 0; Index < PCH_FLASH_PROTECTED_RANGES; ++Index) {
    if ((mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].WriteProtectionEnable ||
         mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ReadProtectionEnable) != TRUE) {
      continue;
    }

    ///
    /// Proceed to program the register after ensure it is enabled
    ///
    Data32 = 0;
    Data32 |= (mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].WriteProtectionEnable == TRUE) ? B_PCH_SPI_PRX_WPE : 0;
    Data32 |= (mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ReadProtectionEnable == TRUE) ? B_PCH_SPI_PRX_RPE : 0;
    Data32 |= ((UINT32) mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ProtectedRangeLimit << N_PCH_SPI_PRX_PRL) & B_PCH_SPI_PRX_PRL_MASK;
    Data32 |= ((UINT32) mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ProtectedRangeBase << N_PCH_SPI_PRX_PRB) & B_PCH_SPI_PRX_PRB_MASK;
    DEBUG ((DEBUG_INFO, "Protected range %d: 0x%08x \n", Index, Data32));

    DlockValue |= (UINT32) (B_PCH_SPI_DLOCK_PR0LOCKDN << Index);
    MmioWrite32 ((UINTN) (PchSpiBar0 + (R_PCH_SPI_PR0 + (Index * S_PCH_SPI_PRX))), Data32);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (PchSpiBar0 + (R_PCH_SPI_PR0 + (Index * S_PCH_SPI_PRX))),
      1,
      (VOID *) (UINTN) (PchSpiBar0 + (R_PCH_SPI_PR0 + (Index * S_PCH_SPI_PRX)))
      );
  }

  //
  //Disable soft sequence.
  //OPMENU0             = 0
  //OPMENU1           = 0
  //PREOP_TYPE          = 0
  //B_PCH_SPI_DLOCK_SSEQLOCKDN   = 1
  //B_PCH_SPI_DLOCK_SBMRAGLOCKDN = 1
  //B_PCH_SPI_DLOCK_SBMWAGLOCKDN = 1
  //B_PCH_SPI_DLOCK_BMRAGLOCKDN  = 1
  //B_PCH_SPI_DLOCK_BMWAGLOCKDN  = 1
  //Note that PREOP_OPTYPE, OPMENU0 and OPMENU1 are 0 by default.
  //
  DlockValue |= B_PCH_SPI_DLOCK_SSEQLOCKDN;
  DlockValue |= B_PCH_SPI_DLOCK_SBMRAGLOCKDN;
  DlockValue |= B_PCH_SPI_DLOCK_SBMWAGLOCKDN;
  DlockValue |= B_PCH_SPI_DLOCK_BMRAGLOCKDN;
  DlockValue |= B_PCH_SPI_DLOCK_BMWAGLOCKDN;

  //
  // Program DLOCK register
  //
  MmioWrite32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_DLOCK), DlockValue);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PchSpiBar0 + R_PCH_SPI_DLOCK),
    1,
    (VOID *) (UINTN) (PchSpiBar0 + R_PCH_SPI_DLOCK)
    );

  //
  // Programming WRSDIS logic
  //
  WaitCount = WAIT_TIME / WAIT_PERIOD; // Time out value
  // Ensure SPI cycle not in progress before programming register
  Data32 = MmioRead32 (PchSpiBar0 + R_PCH_SPI_HSFSC);
  while((Data32 & B_PCH_SPI_HSFSC_SCIP) && WaitCount--) {
    Data32 = MmioRead32 (PchSpiBar0 + R_PCH_SPI_HSFSC);
    MicroSecondDelay (WAIT_PERIOD);
  }

  if( WaitCount != 0) {
	// Clear Status report bits
	MmioOr32((UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC), B_PCH_SPI_HSFSC_FDONE | B_PCH_SPI_HSFSC_FCERR | B_PCH_SPI_HSFSC_AEL);
	// Ensure FCYCLE is not write status command (7) before writing WRSDIS
    MmioAnd32((UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC), (UINT32)~(B_PCH_SPI_HSFSC_CYCLE_MASK | B_PCH_SPI_HSFSC_CYCLE_FGO));
    // Program WRSDIS - write status command (0x7) is not allowed as hardware sequencing operation
    MmioOr32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC), (UINT32) (B_PCH_SPI_HSFSC_WRSDIS));
    } else {
      DEBUG ((DEBUG_ERROR, "SPI Cycle Timeout\n"));
    }

  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// In PCH SPI controller the BIOS should set the Flash Configuration Lock-Down bit
  /// (SPI_BAR0 + 04[15]) at end of post.  When set to 1, those Flash Program Registers
  /// that are locked down by this FLOCKDN bit cannot be written.
  /// Please refer to the EDS for which program registers are impacted.
  ///
  MmioOr32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC), (UINT32) (B_PCH_SPI_HSFSC_FLOCKDN));
  MmioOr32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC), (UINT32) (B_PCH_SPI_HSFSC_PRR34_LOCKDN));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC),
    1,
    (VOID *) (UINTN) (PchSpiBar0 + R_PCH_SPI_HSFSC)
    );

  ///
  /// SPI Flash Programming Guide Section 5.5.2 Vendor Component Lock
  /// It is strongly recommended that BIOS sets the Vendor Component Lock (VCL) bits. VCL applies
  /// the lock to both VSCC0 and VSCC1 even if VSCC0 is not used. Without the VCL bits set, it is
  /// possible to make Host/GbE VSCC register(s) changes in that can cause undesired host and
  /// integrated GbE Serial Flash functionality.
  ///
  MmioOr32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SFDP0_VSCC0), B_PCH_SPI_SFDP0_VSCC0_VCL);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PchSpiBar0 + R_PCH_SPI_SFDP0_VSCC0),
    1,
    (VOID *) (UINTN) (PchSpiBar0 + R_PCH_SPI_SFDP0_VSCC0)
    );

  ///
  /// Additional Power Management Programming
  /// Step 3
  /// Set GEN_PMCON_LOCK register, PMC PCI offset A6h = 06h, after stretch and ACPI base programming completed.
  ///
  MmioOr8 (
    (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B + 2),
    (UINT8) ((B_PCH_PMC_GEN_PMCON_B_SLPSX_STR_POL_LOCK | B_PCH_PMC_GEN_PMCON_B_ACPI_BASE_LOCK) >> 16)
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B + 2),
    1,
    (VOID *) (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B + 2)
    );

  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS needs to enable the BIOS Lock Enable (BLE) feature of the PCH by setting
  /// SPI/eSPI/LPC PCI offset DCh[1] = 1b.
  /// When this bit is set, attempts to write the BIOS Write Enable (BIOSWE) bit
  /// in PCH will cause a SMI which will allow the BIOS to verify that the write is
  /// from a valid source.
  /// Remember that BIOS needs to set SPI/LPC/eSPI PCI Offset DC [0] = 0b to enable
  /// BIOS region protection before exiting the SMI handler.
  /// Also, TCO_EN bit needs to be set (SMI_EN Register, ABASE + 30h[13] = 1b) to keep
  /// BLE feature enabled after booting to the OS.
  /// Intel requires that BIOS enables the Lock Enable (LE) feature of the PCH to
  /// ensure SMM protection of flash.
  /// Left to platform code to register a callback function to handle BiosWp SMI
  ///
  if (mPchPolicyHob->LockDownConfig.SpiEiss == TRUE) {
    //
    // eSPI/LPC
    //
    if (! (MmioRead8 (PciLpcRegBase + R_PCH_LPC_BC) & B_PCH_LPC_BC_EISS)) {
      DEBUG ((DEBUG_INFO, "Set LPC EISS\n"));
      MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), B_PCH_LPC_BC_EISS);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
        1,
        (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
        );
    }
    //
    // SPI
    //
    if (! (MmioRead8 (PciSpiRegBase + R_PCH_SPI_BC) & B_PCH_SPI_BC_EISS)) {
      DEBUG ((DEBUG_INFO, "Set SPI EISS\n"));
      MmioOr8 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC), (UINT8) B_PCH_SPI_BC_EISS);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
        1,
        (VOID *) (UINTN) (PciSpiRegBase + R_PCH_SPI_BC)
        );
    }
    AsmWriteMsr64 (EFI_MSR_POWER_MISC, AsmReadMsr64(EFI_MSR_POWER_MISC) | B_EFI_MSR_POWER_MISC_ENABLE_IA_UNTRUSTED_MODE);
  }
  if (mPchPolicyHob->LockDownConfig.BiosLock == TRUE) {
    //
    // eSPI/LPC
    //
    if (! (MmioRead8 (PciLpcRegBase + R_PCH_LPC_BC) & B_PCH_LPC_BC_LE)) {
      DEBUG ((DEBUG_INFO, "Set LPC bios lock\n"));
      MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), B_PCH_LPC_BC_LE);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
        1,
        (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
        );
    }
    //
    // SPI
    //
    if (! (MmioRead8 (PciSpiRegBase + R_PCH_SPI_BC) & B_PCH_SPI_BC_LE)) {
      DEBUG ((DEBUG_INFO, "Set SPI bios lock\n"));
      MmioOr8 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC), (UINT8) B_PCH_SPI_BC_LE);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
        1,
        (VOID *) (UINTN) (PciSpiRegBase + R_PCH_SPI_BC)
        );
    }
  }
  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS needs to enables SMI_LOCK (PMC PCI offset A0h[4] = 1b) which prevent writes
  /// to the Global SMI Enable bit (GLB_SMI_EN ABASE + 30h[0]). Enabling this bit will
  /// mitigate malicious software attempts to gain system management mode privileges.
  ///
  if (mPchPolicyHob->LockDownConfig.GlobalSmi == TRUE) {
    ///
    /// Save Global SMI Enable bit setting before BIOS enables SMI_LOCK during S3 resume
    ///
    Data32Or = IoRead32 ((UINTN) (ABase + R_PCH_SMI_EN));
    if ((Data32Or & B_PCH_SMI_EN_GBL_SMI) != 0) {
      Data32And = 0xFFFFFFFF;
      Data32Or |= B_PCH_SMI_EN_GBL_SMI;
      S3BootScriptSaveIoReadWrite (
        S3BootScriptWidthUint32,
        (UINTN) (ABase + R_PCH_SMI_EN),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
    }
    MmioOr8 ((UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A), B_PCH_PMC_GEN_PMCON_A_SMI_LOCK);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint8,
      (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A),
      1,
      (VOID *) (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A)
      );
  }

  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS also needs to set the BIOS Interface Lock Down bit in multiple locations
  /// (PCR[RTC] + 3400h[31], LPC/eSPI PCI offset DCh[7] and SPI PCI offset DCh[7]).
  /// Setting these bits will prevent writes to the Top Swap bit (under their respective locations)
  /// and the Boot BIOS Straps. Enabling this bit will mitigate malicious software
  /// attempts to replace the system BIOS option ROM with its own code.
  ///
  if (mPchPolicyHob->LockDownConfig.BiosInterface == TRUE) {
    ///
    /// LPC
    ///
    MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), (UINT32) B_PCH_LPC_BC_BILD);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint8,
      (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
      1,
      (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
      );

    ///
    /// Reads back for posted write to take effect
    ///
    Data8 = MmioRead8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC));
    S3BootScriptSaveMemPoll  (
      S3BootScriptWidthUint8,
      (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
      &Data8,     // BitMask
      &Data8,     // BitValue
      1,          // Duration
      1           // LoopTimes
      );

    ///
    /// SPI
    ///
    MmioOr32 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC), (UINT32) B_PCH_SPI_BC_BILD);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
      1,
      (VOID *) (UINTN) (PciSpiRegBase + R_PCH_SPI_BC)
      );

    ///
    /// Reads back for posted write to take effect
    ///
    Data32Or = MmioRead32 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC));
    S3BootScriptSaveMemPoll  (
      S3BootScriptWidthUint32,
      (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
      &Data32Or,  // BitMask
      &Data32Or,  // BitValue
      1,          // Duration
      1           // LoopTimes
      );

    ///
    /// Set PCR[RTC] + 3400[31] = 1b
    ///
    PchPcrAndThenOr32 (PID_RTC, R_PCH_PCR_RTC_CONF, (UINT32)(~0), B_PCH_PCR_RTC_BILD);

  }

  ///
  /// PCH BIOS Spec Section 5.13 BIOS guide on using RTC RAM
  /// For Data integrity protection, set RTC Memory locks (Upper 128 Byte Lock and
  /// Lower 128 Byte Lock) at SBCR[RTC] + 3400h[4] and SBCR[RTC] + 3400h[3]. Note once locked
  /// bytes 0x38 - 0x3F in each of the Upper and Lower Byte blocks, respectively,
  /// cannot be unlocked until next reset.
  ///
  if (mPchPolicyHob->LockDownConfig.RtcLock == TRUE) {
    Data32And = 0xFFFFFFFF;
    Data32Or  = (B_PCH_PCR_RTC_CONF_UCMOS_LOCK | B_PCH_PCR_RTC_CONF_LCMOS_LOCK | B_PCH_PCR_RTC_CONF_RESERVED);
    PchPcrAndThenOr32 (
      PID_RTC, R_PCH_PCR_RTC_CONF,
      Data32And,
      Data32Or
      );
    PCH_PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      PID_RTC, R_PCH_PCR_RTC_CONF,
      &Data32Or,
      &Data32And
      );
  }

  ///
  /// Lock Down TCO
  ///
  Data16And = 0xFFFF;
  Data16Or  = B_PCH_TCO_CNT_LOCK;
  IoOr16 (TcoBase + R_PCH_TCO1_CNT, Data16Or);
  S3BootScriptSaveIoReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (TcoBase + R_PCH_TCO1_CNT),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );

  ///
  /// PCH BIOS Spec Section 5.15.1 Additional Chipset Initialization
  /// Step 1
  /// Set SPIBAR0 + F0h [0] to 1b
  ///
  MmioOr8 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSML), B_PCH_SPI_SSML_SSL);

  PciP2sbRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_P2SB,PCI_FUNCTION_NUMBER_PCH_P2SB);

  ///
  /// Assume P2SB already hidden, reveal first
  ///
  PchRevealP2sb (PciP2sbRegBase, NULL);

  ///
  /// Lock Down SB ports
  ///
  DisableIOSFEndPointsIDsEntries = sizeof(DisableIOSFEndPointsIDs)/sizeof(UINT16);
  for(i=0; i < DisableIOSFEndPointsIDsEntries;i++) {
    MmioOr32( PciP2sbRegBase + P2SB_PORT_DISABLE_REG(DisableIOSFEndPointsIDs[i]),
               P2SB_PORT_DISABLE_VAL(DisableIOSFEndPointsIDs[i])
             );
  }

  ///
  /// Enable Parity Checking on P2SBC
  ///
  MmioOr32 ( PciP2sbRegBase + R_PCH_P2SB_P2SBC,
             (
               B_PCH_P2SB_P2SBC_DPPEE |
               B_PCH_P2SB_P2SBC_CPEE  |
               B_PCH_P2SB_P2SBC_DPEE
             )
            );

  ///
  /// Lock down P2SBC MASKLOCK
  ///
  MmioOr32 ( PciP2sbRegBase + R_PCH_P2SB_P2SBC,
             B_PCH_P2SB_P2SBC_MASKLOCK
            );

  ///
  /// Hide P2SB
  ///
  if ( mPchPolicyHob->PchHideP2sbEnable == TRUE) {
	  PchHideP2sb (PciP2sbRegBase);
  }

  //
  // Lock Down PMC
  // Set PWRM + 0x18 [27, 22] prior to OS.
  //
  PchPwrmBaseGet (&PchPwrmBase);
  Data32Or = BIT27;
  if (mPchPolicyHob->PmConfig.PmcReadDisable) {
    Data32Or |= BIT22;
  }
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, Data32Or) ;
  DEBUG ((EFI_D_INFO, "\tPM_CFG = 0x%08x\tValue = 0x%08x\n",PchPwrmBase + R_PCH_PWRM_CFG,MmioRead32 (PchPwrmBase + R_PCH_PWRM_CFG)));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PchPwrmBase + R_PCH_PWRM_CFG),
    1,
    (VOID *) (UINTN) (PchPwrmBase + R_PCH_PWRM_CFG)
    );

  return Status;
}

VOID
PrintSataSpeed
(UINT8 SataDeviceNumber)
{
  UINTN  PciSataRegBase;
  UINTN  AhciBarAddress;
  UINT32 SpeedValue;
  UINT32 SataPiRegister;
  UINT8  PortIndex;
  UINT16 Data16;

  DEBUG ((EFI_D_INFO,"SATA: Controller speed: 0x%x\n",SataDeviceNumber));

  PciSataRegBase  = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, SataDeviceNumber, PCI_FUNCTION_NUMBER_PCH_SATA);
  Data16          = MmioRead16 (PciSataRegBase + PCI_DEVICE_ID_OFFSET);

  if(!IS_PCH_H_SATA_DEVICE_ID(Data16))
  {
      DEBUG ((EFI_D_INFO,"SATA: Controller not found\n"));
      return;
  }

  AhciBarAddress  = MmioRead32(PciSataRegBase + R_PCH_SATA_AHCI_BAR);

  if(((AhciBarAddress & 0xFFFFFFFF) == 0xFFFFFFFF) || (AhciBarAddress == 0))
  {
      DEBUG ((EFI_D_INFO,"SATA: Wrong bar set\n"));
      return;
  }

  SataPiRegister = MmioRead32(AhciBarAddress + R_PCH_SATA_AHCI_PI);

  for (PortIndex = 0; PortIndex < PCH_MAX_SATA_PORTS; PortIndex ++)
  {
      if(SataPiRegister & (BIT0<<PortIndex))
      {
        SpeedValue = MmioRead32(AhciBarAddress + R_PCH_SATA_AHCI_P0SSTS + (PortIndex*80));
        SpeedValue = (SpeedValue & B_PCH_SATA_AHCI_PXSSTS_SPD_MASK) >> N_PCH_SATA_AHCI_PXSSTS_SPD;
        DEBUG ((EFI_D_INFO,"\tSATA SPD register port %x value: 0x%x\n",PortIndex,SpeedValue));
      }
  }
}

/**
  This function process P2SB configuration when End Of Dxe callback
  This MUST be executed in very end of End Of Dxe
**/
VOID
ConfigureP2sbOnEndOfDxe (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "ConfigureP2sbOnEndOfDxe() Start\n"));

  ///
  /// Lock down the P2sb SBI before going into OS.
  ///
  ConfigureP2sbSbiLock (&mPchPolicyHob->P2sbConfig);
  DEBUG ((DEBUG_INFO, "ConfigureP2sbOnEndOfDxe() End\n"));
}

/**
  Common PCH initialization before Boot Sript Table is closed

**/
VOID
PchOnEndOfDxe (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Data32And;
  UINT32                                    Data32Or;
#ifndef FSP_FLAG
#endif //!FSP_FLAG

  DEBUG ((DEBUG_INFO, "Common PchOnEndOfDxe() Start\n"));

  ///
  /// Perform remaining configuration for PCH SATA on End of DXE
  ///
  ConfigurePchSataOnEndOfDxe (&mPchPolicyHob->SataConfig[0], PCI_DEVICE_NUMBER_PCH_SATA);
  ConfigurePchSataOnEndOfDxe (&mPchPolicyHob->SataConfig[1], PCI_DEVICE_NUMBER_PCH_SATA2);

  ///
  /// Check to disable Smbus controller
  ///
  if (mPchPolicyHob->SmbusConfig.Enable == FALSE) {
    Data32And = 0xFFFFFFFF;
    Data32Or  = B_PCH_PCR_SMBUS_GC_FD;
    PchPcrAndThenOr32 (
      PID_SMB, R_PCH_PCR_SMBUS_GC,
      Data32And,
      Data32Or
      );
    PCH_PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      PID_SMB, R_PCH_PCR_SMBUS_GC,
      &Data32Or,
      &Data32And
      );
    //
    // @todo: Add PSF programming when disabling SMBUS
    //
  }

  Status = ProcessAllLocks ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error when processing locks in PchOnEndOfDxe \n"));
    ASSERT (FALSE);
  }

  //
  // This function process P2SB configuration when End Of Dxe callback
  // This MUST be executed in very end of End Of Dxe
  // All PSF access after this function will not acceptable
  //
  ConfigureP2sbOnEndOfDxe ();

  //Clear DEFEATURE.CPERR and DEFEATURE.DPERR for SMBGEN
  MmioAnd16(MmPciBase(PCI_BUS_NUMBER_PCH_SMBUS_HOST, PCI_DEVICE_NUMBER_PCH_SMBUS_HOST, PCI_FUNCTION_NUMBER_PCH_SMBUS_HOST) + R_PCH_SMBUS_DEFEATURE, (UINT16)~(B_PCH_SMBUS_DEFEATURE_CPERR | B_PCH_SMBUS_DEFEATURE_DPERR));
  //Set PLKCTL.CL for SMBGEN
  MmioOr16(MmPciBase(PCI_BUS_NUMBER_PCH_SMBUS_HOST, PCI_DEVICE_NUMBER_PCH_SMBUS_HOST, PCI_FUNCTION_NUMBER_PCH_SMBUS_HOST) + R_PCH_SMBUS_PLKCTL, B_PCH_SMBUS_PLKCTL_CL);
#ifdef FSP_FLAG
  if (MmioRead16(MmPciBase(PCI_BUS_NUMBER_PCH_XHCI, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI)) == 0xFFFF) {
    DEBUG ((DEBUG_ERROR, "XHCI disabled \n"));
  } else {
#endif
  //Set PLKCTL.CL for USBx
  MmioOr32(MmPciBase(PCI_BUS_NUMBER_PCH_XHCI, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI) + R_PCH_XHCI_XHCC1, B_PCH_XHCI_XHCC1_ACCTRL);
#ifdef FSP_FLAG
  }
#endif
  DEBUG ((DEBUG_INFO, "Common PchOnEndOfDxe() End\n"));

  return;
}
