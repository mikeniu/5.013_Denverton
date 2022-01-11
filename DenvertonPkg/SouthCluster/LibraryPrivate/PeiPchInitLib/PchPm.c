/** @file
  Initializes PCH power management controller.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PchAccess.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchHsioLib.h>
#include <Library/MmPciLib.h>
#include <Ppi/PchPolicy.h>
#include <IncludePrivate/PchHsio.h>
#include <IncludePrivate/Ppi/PchPmcXramOffsetData.h>


/**
  Configure miscellaneous power management settings

  @param[in] PchPolicy          The PCH Policy instance

  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
ConfigureMiscPm (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{

  UINTN                                 PciPmcRegBase;
  UINT32                                PmConB;

  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );

  ///
  /// Handle wake policy
  ///
  PmConB = MmioRead32 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B);

  PmConB &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_B_PME_B0_S5_DIS);

  if (PchPolicy->PmConfig.WakeConfig.PmeB0S5Dis) {
    PmConB |= B_PCH_PMC_GEN_PMCON_B_PME_B0_S5_DIS;
  }


  MmioWrite32 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B, PmConB);



  return EFI_SUCCESS;
}


/**
  Configure PSF power management.
  Must be called after all PSF configuration is completed.
**/
VOID
ConfigurePsfPm (
  VOID
  )
{

  PchPcrAndThenOr32 (PID_PSF0,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSF1,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSF2,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSF3,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSFS0,    R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSFS1,    R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_MEPSF,    R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_IEPSF,    R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
}

/**
  Configure SPI/eSPI power management
**/
VOID ConfigureSpiPm (
  VOID
  )
{
  //
  // Set SPI PCR 0xC004 bits 29, 10-8, 5-0 to 1b, 111b, 11111b
  // Clear SPI PCR 0xC008 bit 2,1,0
  PchPcrAndThenOr32 (PID_ESPISPI, R_PCH_PCR_SPI_CLK_CTL, ~0u, 0x2000073F);
  PchPcrAndThenOr32 (PID_ESPISPI, R_PCH_PCR_SPI_PWR_CTL, (UINT32)~(BIT2 | BIT1 | BIT0), 0x00);
  //Begin: SPI_CORE_CLK WA for A0/A1 & B0 (HSD 1404767110). It won't be fixed by HW
  //
  // [ 8] Need to be 0 so clock gating is disabled
  //
  PchPcrAndThenOr32 (PID_ESPISPI, R_PCH_PCR_SPI_CLK_CTL, (UINT32)~(BIT9 | BIT8 | BIT5 | BIT4 | BIT2 | BIT1), BIT2);
  //End: SPI_CORE_CLK WA for A0/A1 & B0 (HSD 1404767110). It won't be fixed by HW
}

/**
  Configure LPC power management.

  @param[in] PchPolicy                  The PCH Policy instance
**/
VOID
ConfigureLpcPm (
  IN  PCH_POLICY_PPI                    *PchPolicy
  )
{
  UINTN                                 PciLpcRegBase;

  PciLpcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC
                      );

  ///
  /// System BIOS is also required to set following bit.
  /// PCI CLKRUN# Enable" bit (LPC PCI offset E0h[0]) = 1b
  ///
  #ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.LpcSafeRegister) {
    ///
    /// Leave at default value
    ///
  } else {
  #endif // PCH_PO_FLAG
  if (PchPolicy->PmConfig.PciClockRun == FALSE) {
    MmioAnd16 ((UINTN) (PciLpcRegBase + R_PCH_LPC_PCC), (UINT16) (~B_PCH_LPC_PCC_CLKRUN_EN));
  } else {
    MmioOr16 ((UINTN) (PciLpcRegBase + R_PCH_LPC_PCC), (UINT16) (B_PCH_LPC_PCC_CLKRUN_EN));
  }
  #ifdef PCH_PO_FLAG
  }
  #endif // PCH_PO_FLAG
}

/**
  Perform power management initialization

  @param[in] PchPolicy                  The PCH Policy PPI instance

  @retval EFI_SUCCESS                   Succeeds.
**/
EFI_STATUS
PchPmInit (
  IN  PCH_POLICY_PPI                    *PchPolicy
  )
{
  UINTN                                 PciPmcRegBase;
  PCH_STEPPING                          PchStep;
  UINT32                                PchPwrmBase;
  UINT32                                Data32;
  UINT32                                Data32And;
  UINT32                                Data32Or;
  PCH_SERIES                            PchSeries;
  BOOLEAN                               DciEnabled;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );
  PchPwrmBaseGet (&PchPwrmBase);

  PchStep    = PchStepping ();
  PchSeries  = GetPchSeries ();
  Data32And  = 0;
  Data32Or   = 0;

  DciEnabled = PchPolicy->DciConfig.DciEn == TRUE;
  if ((DciEnabled == FALSE) && (PchPolicy->DciConfig.DciAutoDetect == TRUE)) {
    ///
    /// Detect DCI being avaiable (PCR[DCI] + 0x4[9] == 1 or PCR[DCI] + 0x4[10] == 1)
    ///
    PchPcrRead32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, &Data32);
    DciEnabled = (Data32 & (BIT9 | BIT10)) != 0;
  }

  //
  // Perform PM recommendation settings
  //

  ///
  /// PWRMBASE + 0x14 [5,4,2,1,0] = all 1s
  ///
  MmioOr32 (
    PchPwrmBase + R_PCH_PWRM_14,
    BIT5 | BIT4 | BIT2 | BIT1 | BIT0
    );

  ///
  /// PM_CFG (0x018h[29]) = 1, Allow 24MHz Crystal Oscillator Shutdown (ALLOW_24_OSC_SD)
  /// PM_CFG (0x018h[2]) = 1
  /// PM_CFG (0x018h[24]) = 1  Must perform this setting as a separate write
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, BIT2 | B_PCH_PWRM_CFG_ALLOW_24_OSC_SD);

  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, BIT24);


  ///
  /// PM_CFG (0x018h[23]) = 1  Must perform this setting as a separate write
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, BIT23);



  ///
  /// PM_CFG (0x018h[3]) = 1, Apply timeout to the S1/S3/S4/S5 entry flow
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, B_PCH_PWRM_CFG_SX_ENT_TO_EN);
  DEBUG ((EFI_D_INFO, "\tPM_CFG = 0x%08x\tValue = 0x%08x\n",PchPwrmBase + R_PCH_PWRM_CFG,MmioRead32 (PchPwrmBase + R_PCH_PWRM_CFG)));

  ///
  /// PWRMBASE + 0x3C[31:29]
  /// For PCH power button override period, 0=4sec, 011b=10sec
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG2, ((PchPolicy->PmConfig.PwrBtnOverridePeriod)<<N_PCH_PWRM_CFG2_PBOP));

  ///
  /// PWRMBASE + 0xE0 = 0x01040000
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_CFG3, V_PCH_PWRM_CFG3);

  ///
  /// ONLY FOR A0 STEPPING
  /// MODPHY_PM_CFG1 (0x200h[31:16]) = 0x0000, ModPHY Lane S0 SUS Well Power Gating Policy (MLS0SWPGP)
  /// MODPHY_PM_CFG1 (0x200h[15:0])  = 0x0000, ModPHY Lane Sx SUS Well Power Gating Policy (MLSXSWPGP)
  ///
  if (PchStep == PchHA0) {
    MmioWrite32 (PchPwrmBase + R_PCH_PWRM_MODPHY_PM_CFG1, V_PCH_PWRM_MODPHY_PM_CFG1);
  }

  ///
  /// MODPHY_PM_CFG2 (0x204h[29])    = 1,     Enable ModPHY FET Control (EMFC)
  /// MODPHY_PM_CFG2 (0x204h[28:24]) = 01010, External FET Ramp Time (EFRT)
  /// MODPHY_PM_CFG2 (0x204h[3:0])   = 0000,  ModPHY Lane Sx SUS Well Power Gating Policy (MLSXSWPGP)
  ///
  /// If DCI is enabled, skip 0x204 [29] [28:24] programming. Disable ModPHY sus power gating feature
  ///
  Data32And = (UINT32) ~(B_PCH_PWRM_MODPHY_PM_CFG2_EMFC | B_PCH_PWRM_MODPHY_PM_CFG2_EFRT | B_PCH_PWRM_MODPHY_PM_CFG2_MLSXSWPGP);
  if (PchStep == PchHA0) {
	  Data32Or = (UINT32) V_PCH_PWRM_MODPHY_PM_CFG2_MLSXSWPGP;  // Set to 0s for A0 stepping
  } else {
	  Data32Or = (UINT32) B_PCH_PWRM_MODPHY_PM_CFG2_MLSXSWPGP;  // Set to 1s for >A0 steppings
  }
  if (DciEnabled == FALSE) {
    Data32Or  |= (UINT32) (B_PCH_PWRM_MODPHY_PM_CFG2_EMFC | (V_PCH_PWRM_MODPHY_PM_CFG2_EFRT_200US << N_PCH_PWRM_MODPHY_PM_CFG2_EFRT));
  }

  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_MODPHY_PM_CFG2,
    Data32And,
    Data32Or
    );

#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.PsfSafeRegister == FALSE) {
#endif
  ConfigurePsfPm ();
#ifdef PCH_PO_FLAG
  }
#endif

#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.SpiSafeRegister == FALSE) {
#endif
  ConfigureSpiPm ();
#ifdef PCH_PO_FLAG
  }
#endif


  ConfigureLpcPm (PchPolicy);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

ConfigureMiscPm (PchPolicy);
  return EFI_SUCCESS;
}

/**
  Get PMC Data BAR Address

  @param[in]   TempMemBaseAddr          Temporary memory base address for PMC device
  @param[in]   PmcBaseAddress           PMC Base Address
  @param[out]  PmcDataBar               PMC Data BAR Address

  @retval FALSE                         No cleanup temporary MMIO resource to PM_DATA_BAR required
  @retval TRUE                          Cleanup of temporary MMIO resource to PM_DATA_BAR required
**/
BOOLEAN
PchPmcGetDataBar (
  IN  UINT32                          TempMemBaseAddr,
  IN  UINTN                           PmcBaseAddress,
  OUT UINTN                           *PmcDataBar
  )
{
  BOOLEAN       ResetPmcBar;

  ResetPmcBar = FALSE;
  if ((MmioRead32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR) & B_PCH_PMC_PM_DATA_BAR) == 0x0 ||
    (MmioRead32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR) & B_PCH_PMC_PM_DATA_BAR) == B_PCH_PMC_PM_DATA_BAR) {
    ResetPmcBar = TRUE;
    //
    // Disable memory space decoding in command register
    //
    MmioAnd16 (PmcBaseAddress + PCI_COMMAND_OFFSET, (UINT16) ~(EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
    //
    // Program temporary memory base address
    //
    MmioWrite32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR, TempMemBaseAddr);
    //
    // Enable memory space decoding in command register
    //
    MmioOr16 (PmcBaseAddress + PCI_COMMAND_OFFSET, (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
  }
  *PmcDataBar = MmioRead32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR) & B_PCH_PMC_PM_DATA_BAR;

  return ResetPmcBar;
}

/**
  Reset PMC Data BAR
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcResetDataBar (
  IN  UINTN                           PmcBaseAddress
)
{
  //
  // Disable memory space decoding in command register
  //
  MmioAnd16 (PmcBaseAddress + PCI_COMMAND_OFFSET, (UINT16) ~(EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
  ///
  /// Clean up Temporary MMIO resource to PM_DATA_BAR
  ///
  MmioWrite32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR, 0);
}



/**
  Calculate PMC XRAM Header offset based on group data

  @param[in]  Pid                       Port ID
  @param[in]  RowOffset                 Rows of data already present
  @param[in]  EndTableMarkers           Total bytes required for end table marker
  @param[in]  Delay                     Number of delays introduced

  @retval Header Data
**/
UINT16
SurvPhyGrpTblEntry (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT8                             RowOffset,
  IN  UINT8                             EndTableMarkers,
  IN  UINT8                             Delay
  )
{
  UINT8 Dwords;
  if ((Pid == 0x00) && (RowOffset == 0x0F)) {
    return (UINT16) 0x0F00;
  } else {
    DEBUG ((DEBUG_INFO, "Pid = %d\n", Pid));
    DEBUG ((DEBUG_INFO, "RowOffset = %d\n", RowOffset));
    DEBUG ((DEBUG_INFO, "EndTableMarkers = %d\n", EndTableMarkers));
    DEBUG ((DEBUG_INFO, "Delay = %d\n", Delay));
    Dwords = (RowOffset * 3) + EndTableMarkers + Delay;
    DEBUG ((DEBUG_INFO, "Dwords = %d\n", Dwords));
    return (UINT16) (((Dwords & 0x0F) << 12) | ((Pid & 0x0F) << 8) | (Dwords >> 4));
  }
}

