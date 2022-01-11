/** @file
  Library to initializes IQAT Device

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved This
 software and associated documentation (if any) is furnished
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
#include <Library/HobLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Guid/IqatIpReservedBase.h>
#include <Library/MmPciLib.h>
#include <SaRegs.h>
#include <IqatIpInitLib.h>
#include <IqatIpRegs.h>

/**
  Check whether IQAT controller is present in this chipset SKU.
  
  @retval TRUE    IQAT is supported in this chipset
  @retval FALSE   IQAT is not supported
**/

BOOLEAN
EFIAPI
IqatIpIsIqatPresent (
  VOID
  )
{
  UINT32  PwrmBase;
  UINT32  FuseDis2State;
  PchPwrmBaseGet (&PwrmBase);
  FuseDis2State = MmioRead32 ((UINTN) (PwrmBase + R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2));
  return (FuseDis2State & B_PCH_PWRM_NCPM_FUSE_SS_DIS) == 0;
}

/**
  Check whether IQAT controller is enabled (in terms of
  static power gating).

  @retval TRUE    IQAT is enabled
  @retval FALSE   IQAT is statically power gated
**/
BOOLEAN
IqatIpIsIqatEnabled (
  VOID
)
{
  UINT32  PwrmBase;
  UINT32  STPGFDisState;
  PchPwrmBaseGet (&PwrmBase);
  STPGFDisState = MmioRead32 ((UINTN) (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
  return (STPGFDisState & B_PCH_PWRM_NCPM_FDIS_PMC) == 0;
}

STATIC
VOID
SetupImrRegion (
  IN  UINT32                      Start,
  IN  UINT32                      Length
)
/*++
Routine Description:
  Setup an IMR entry for the region specified

Arguments:
  Start  - Starting address of region.
  Length - Length of region in bytes.

--*/
{
  UINT32 Low;
  UINT32 High;
  UINT32 MchBar;
  UINT64 Temp64;

  DEBUG ( (EFI_D_INFO, "SetupImrRegion called..(0x%x, 0x%x) \n",Start, Length) );

  Low = High = 0;
  // Setup IMR
  Low = (Start >> IMR_BASE_SHIFT) & BUNIT_BIMRL_LOWER_BOUND_MASK;
  High = (~((Length >> IMR_BASE_SHIFT) - 1));
  High &= B_SA_MCHBAR_BIMR_ADDRESS_MASK;

  DEBUG ( (EFI_D_INFO, "SetupImrRegion Low = 0x%x High = 0x%x..\n",Low,High) );

  MchBar = MmioRead32 (MmPciBase(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_MCHBAR) & ~BIT0; //B0:D0:F0  offset: 0x48
  DEBUG ((DEBUG_INFO, "MchBar: 0x%x\n", MchBar));
  DEBUG ((DEBUG_INFO, "MchBar: LOW read [0x%x] = 0x%x\n", (MchBar + IMR_LOW), MmioRead32 (MchBar + IMR_LOW)));
  DEBUG ((DEBUG_INFO, "MchBar: LOW write [0x%x] = 0x%x\n", (MchBar + IMR_LOW), Low | 0x80000000));
  MmioWrite32 (MchBar + IMR_LOW, Low | 0x80000000);
  DEBUG ((DEBUG_INFO, "MchBar: LOW read [0x%x] = 0x%x\n", (MchBar + IMR_LOW), MmioRead32 (MchBar + IMR_LOW)));

  DEBUG ((DEBUG_INFO, "MchBar: HIGH read [0x%x] = 0x%x\n", (MchBar + IMR_HIGH), MmioRead32 (MchBar + IMR_HIGH)));
  DEBUG ((DEBUG_INFO, "MchBar: HIGH write [0x%x] = 0x%x\n", (MchBar + IMR_HIGH), High));
  MmioWrite32 (MchBar + IMR_HIGH, High);
  DEBUG ((DEBUG_INFO, "MchBar: HIGH read [0x%x] = 0x%x\n", (MchBar + IMR_HIGH), MmioRead32 (MchBar + IMR_HIGH)));

  /* Allow nCPM only access */
  DEBUG ((DEBUG_INFO, "Allowing nCPM to READ / WRITE only\n"));
  Temp64 = MmioRead64 (MchBar + IMR_RAC);
  DEBUG ((DEBUG_INFO, "MchBar: RAC read [0x%x] = 0x%lx\n", (MchBar + IMR_RAC), Temp64));
  Temp64 = IMR_IQAT_ACCESS; /* Set bit 26 for nCPM */
  DEBUG ((DEBUG_INFO, "MchBar: RAC write [0x%x] = 0x%lx\n", (MchBar + IMR_RAC), Temp64));
  MmioWrite64 (MchBar + IMR_RAC, Temp64);
  DEBUG ((DEBUG_INFO, "MchBar: RAC read [0x%x] = 0x%lx\n", (MchBar + IMR_RAC), MmioRead64 (MchBar + IMR_RAC)));

  Temp64 = MmioRead64 (MchBar + IMR_WAC);
  DEBUG ((DEBUG_INFO, "MchBar: WAC read [0x%x] = 0x%lx\n", (MchBar + IMR_WAC), Temp64));
  Temp64 = IMR_IQAT_ACCESS; /* Set bit 26 for nCPM */
  DEBUG ((DEBUG_INFO, "MchBar: WAC write [0x%x] = 0x%lx\n", (MchBar + IMR_WAC), Temp64));
  MmioWrite64 (MchBar + IMR_WAC, Temp64);
  DEBUG ((DEBUG_INFO, "MchBar: RAC read [0x%x] = 0x%lx\n", (MchBar + IMR_WAC), MmioRead64 (MchBar + IMR_WAC)));

}
/**
  Configures IQat Secure RAM

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureIqatSecureRam ()
{
  EFI_HOB_GUID_TYPE     *GuidHob;
  EFI_PHYSICAL_ADDRESS  MemBaseAddress;
  UINTN                 Vrp2MemBase;
  UINTN                 QatMemBase;
  UINT16                Vrp2PciCmd;
  UINT16                QatPciCmd;
  UINT32                QatPmiscBar;
  UINT32                QatBusNum;


  DEBUG ((DEBUG_INFO, "ConfigureIqatSecureRam() Start\n"));

  ///
  /// Allocate 512k of RESERVED memory
  ///
  GuidHob = GetFirstGuidHob (&gIqatIpReservedBaseHobGuid);
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  MemBaseAddress = (EFI_PHYSICAL_ADDRESS) (* (EFI_PHYSICAL_ADDRESS *)GET_GUID_HOB_DATA (GuidHob));
  /* Check address  - memory address Bits[48:0] are supported by DNV PCH IQAT */
  if ((MemBaseAddress == 0) || (((UINT32)(MemBaseAddress >> 32) && IQAT_SECURE_RAM_BASE_ADDRESS_HIGH_MASK) > 0)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  SetupImrRegion((UINT32)MemBaseAddress, IQAT_RESERVE_MEM_SIZE);
  Vrp2MemBase = MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, VRP2_DEVICE_NUM, VRP2_FUNC_NUM);

  // Get the QAT Bus Number (Secondary bus of VRP2)
  QatBusNum = GET_SEC_BUS(MmioRead32 (Vrp2MemBase + VRP2_BUS_OFFSET));
  DEBUG ((DEBUG_INFO, "QatBusNum: 0x%x\n", QatBusNum));

  QatMemBase = MmPciBase(QatBusNum, IQAT_DEVICE_NUM, IQAT_FUNC_NUM);

  Vrp2PciCmd = MmioRead16 (Vrp2MemBase + PCICMD_OFFSET); //B0:D6:F0  offset: 0x4
  DEBUG ((DEBUG_INFO, "Vrp2PciCmd: 0x%x\n", Vrp2PciCmd));
  if((Vrp2PciCmd & PCICMD_MSE) == FALSE) {
	 MmioOr16 (Vrp2MemBase + PCICMD_OFFSET, PCICMD_WR_EN);
  }

  QatPmiscBar = MmioRead32 (QatMemBase + PMISCBAR_OFFSET) & ~PCI_BAR_MEM_MASK; //BM:D0:F0  offset: 0x18
  DEBUG ((DEBUG_INFO, "QatPmiscBar: 0x%x\n", QatPmiscBar));

  QatPciCmd = MmioRead16 (QatMemBase + PCICMD_OFFSET); //BM:D0:F0  offset: 0x4
  DEBUG ((DEBUG_INFO, "QatPciCmd: 0x%x\n", QatPciCmd));
  if((QatPciCmd & PCICMD_MSE) == FALSE) {
     MmioOr16 (QatMemBase + PCICMD_OFFSET, PCICMD_WR_EN);
  }
  DEBUG ((DEBUG_INFO, "RamBaseAddrHi: 0x%x\n", MmioRead32 (QatPmiscBar + RAM_BASE_ADDR_HI)));
  DEBUG ((DEBUG_INFO, "RamBaseAddrLo: 0x%x\n", MmioRead32 (QatPmiscBar + RAM_BASE_ADDR_LO)));

  MmioWrite32 ((QatPmiscBar + RAM_BASE_ADDR_HI), (UINT32)(MemBaseAddress >> RAM_BASE_SHIFT));
  MmioWrite32 ((QatPmiscBar + RAM_BASE_ADDR_LO), (UINT32)(MemBaseAddress));

  DEBUG ((DEBUG_INFO, "RamBaseAddrHi: 0x%x\n", MmioRead32 (QatPmiscBar + RAM_BASE_ADDR_HI)));
  DEBUG ((DEBUG_INFO, "RamBaseAddrLo: 0x%x\n", MmioRead32 (QatPmiscBar + RAM_BASE_ADDR_LO)));

  MmioWrite16 (QatMemBase  + PCICMD_OFFSET, QatPciCmd);
  MmioWrite16 (Vrp2MemBase + PCICMD_OFFSET, Vrp2PciCmd); //B0:D6:F0  offset: 0x4

  DEBUG ((DEBUG_INFO, "ConfigureIqatSecureRam() End\n"));

  return EFI_SUCCESS;
}

/**
  Configures IQat Uncorrectable Error Workaround

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureIqatUncorrectableErrorWA ()
{
  UINTN                 Vrp2MemBase;
  UINTN                 QatMemBase;
  UINT16                Vrp2PciCmd;
  UINT16                QatPciCmd;
  UINT32                QatBusNum;
  UINT32                Data32;
  UINT32                Data32Or;
  EFI_BOOT_MODE         BootMode;

  BootMode = GetBootModeHob ();
  DEBUG ((EFI_D_INFO, "ConfigureIqatUncorrectableErrorWA () Start, boot mode 0x%x..\n",BootMode));

  if (BootMode == BOOT_ON_S3_RESUME) {
  	DEBUG ((EFI_D_INFO, "ConfigureIqatUncorrectableErrorWA: S3 Resume - returning.\n"));
  	return EFI_SUCCESS;
  }

  Vrp2MemBase = MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, VRP2_DEVICE_NUM, VRP2_FUNC_NUM);

  // Get the QAT Bus Number (Secondary bus of VRP2)
  QatBusNum = GET_SEC_BUS(MmioRead32 (Vrp2MemBase + VRP2_BUS_OFFSET));
  DEBUG ((DEBUG_INFO, "QatBusNum: 0x%x\n", QatBusNum));

  QatMemBase = MmPciBase(QatBusNum, IQAT_DEVICE_NUM, IQAT_FUNC_NUM);
  DEBUG ((DEBUG_INFO, "QatMemBase: 0x%x\n", QatMemBase));
  DEBUG ((DEBUG_INFO, "Vrp2MemBase: 0x%x\n", Vrp2MemBase));
  Vrp2PciCmd = MmioRead16 (Vrp2MemBase + PCICMD_OFFSET); //B0:D6:F0  offset: 0x4
  DEBUG ((DEBUG_INFO, "Vrp2PciCmd: 0x%x\n", Vrp2PciCmd));
  if((Vrp2PciCmd & PCICMD_MSE) == FALSE) {
	 MmioOr16 (Vrp2MemBase + PCICMD_OFFSET, PCICMD_WR_EN);
  }

  QatPciCmd = MmioRead16 (QatMemBase + PCICMD_OFFSET); //BM:D0:F0  offset: 0x4
  DEBUG ((DEBUG_INFO, "QatPciCmd: 0x%x\n", QatPciCmd));
  if((QatPciCmd & PCICMD_MSE) == FALSE) {
     MmioOr16 (QatMemBase + PCICMD_OFFSET, PCICMD_WR_EN);
  }

  /*
   * Clear the Uncorrectable error bits if set (All bits are W1C)
   *
   * pfieerruncstsr PF IE Uncorrectable Error Status Register: bit 11 - EPPE; 10 - OCDE
   *
   */
  Data32Or = 0;
  Data32 = MmioRead32 (QatMemBase + R_IQAT_PFIEERRUNCSTSR);
  DEBUG ((DEBUG_INFO, "R_IQAT_PFIEERRUNCSTSR: 0x%x\n", Data32));
  if (BIT_IS_SET(Data32,B_IQAT_PFIEERRUNCSTSR_EPPPE))
	  Data32Or |= B_IQAT_PFIEERRUNCSTSR_EPPPE;
  if (BIT_IS_SET(Data32,B_IQAT_PFIEERRUNCSTSR_OCDE))
	  Data32Or |= B_IQAT_PFIEERRUNCSTSR_OCDE;
  if (Data32Or) {
	  MmioWrite32(QatMemBase + R_IQAT_PFIEERRUNCSTSR, Data32Or);
	  Data32 = MmioRead32 (QatMemBase + R_IQAT_PFIEERRUNCSTSR);
	  DEBUG ((DEBUG_INFO, "Now R_IQAT_PFIEERRUNCSTSR: 0x%x\n", Data32));
  }
  /*
   * ppaerucs		PF PCIe AER Uncorrectable Error Status Register	bit 22 - IEUNC
   */
  Data32 = MmioRead32 (QatMemBase + R_IQAT_PPAERUCS);
  DEBUG ((DEBUG_INFO, "R_IQAT_PPAERUCS: 0x%x\n", Data32));
  if (BIT_IS_SET(Data32,B_IQAT_PPAERUCS_IEUNC)) {
	  MmioWrite32(QatMemBase + R_IQAT_PPAERUCS, B_IQAT_PPAERUCS_IEUNC);
	  Data32 = MmioRead32 (QatMemBase + R_IQAT_PPAERUCS);
	  DEBUG ((DEBUG_INFO, "Now R_IQAT_PPAERUCS: 0x%x\n", Data32));
  }
  /*
   * ppaercs	    PF PCIe AER Correctable Error Register	bit 13- ANFES
   */
  Data32 = MmioRead32 (QatMemBase + R_IQAT_PPAERCS);
  DEBUG ((DEBUG_INFO, "R_IQAT_PPAERCS: 0x%x\n", Data32));
  if (BIT_IS_SET(Data32,B_IQAT_PPAERCS_ANFES)) {
	  MmioWrite32(QatMemBase + R_IQAT_PPAERCS, B_IQAT_PPAERCS_ANFES);
	  Data32 = MmioRead32 (QatMemBase + R_IQAT_PPAERCS);
	  DEBUG ((DEBUG_INFO, "Now R_IQAT_PPAERCS: 0x%x\n", Data32));
  }
  MmioWrite16 (QatMemBase  + PCICMD_OFFSET, QatPciCmd);
  MmioWrite16 (Vrp2MemBase + PCICMD_OFFSET, Vrp2PciCmd); //B0:D6:F0  offset: 0x4

  DEBUG ((DEBUG_INFO, "ConfigureIqatUncorrectableErrorWA() End\n"));

  return EFI_SUCCESS;
}
/**
  Configures IQat Registers

  @param[in] IqatIpGlobalPolicy    The IQAT IP Policy protocol instance

**/
VOID
ConfigureIqatRegs (
  IN  IQAT_IP_GLOBAL_POLICY_HOB         *IqatIpGlobalPolicy
  )
{
  UINT32  FuseCtl;
  UINT32  FuseCtlValue;
  UINTN   Vrp2MemBase;
  UINTN   QatMemBase;
  UINT32  Vrp2PciCmd;
  UINT32  QatPciCmd;
  UINT32  QatBusNum;
  UINT32  PpdCntl;

  DEBUG ((EFI_D_INFO, "ConfigureIqatRegs() Start..\n"));

  Vrp2MemBase = MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, VRP2_DEVICE_NUM, VRP2_FUNC_NUM);

 // Get the QAT Bus Number (Secondary bus of VRP2)
  QatBusNum = GET_SEC_BUS(MmioRead32 (Vrp2MemBase + VRP2_BUS_OFFSET));
  DEBUG ((DEBUG_INFO, "QatBusNum: 0x%x\n", QatBusNum));

  QatMemBase = MmPciBase(QatBusNum, IQAT_DEVICE_NUM, IQAT_FUNC_NUM);

  Vrp2PciCmd = MmioRead32 (Vrp2MemBase + PCICMD_OFFSET); //B0:D6:F0  offset: 0x4
  DEBUG ((DEBUG_INFO, "Vrp2PciCmd: 0x%x\n", Vrp2PciCmd));
  if((Vrp2PciCmd & PCICMD_MSE) == FALSE) {
    MmioWrite32 (Vrp2MemBase + PCICMD_OFFSET, PCICMD_WR_EN);
  }

  QatPciCmd = MmioRead32 (QatMemBase + PCICMD_OFFSET); //BM:D0:F0  offset: 0x4
  DEBUG ((DEBUG_INFO, "QatPciCmd: 0x%x\n", QatPciCmd));
  if((QatPciCmd & PCICMD_MSE) == FALSE) {
    MmioWrite32 (QatMemBase + PCICMD_OFFSET, PCICMD_WR_EN);
  }

  // Configure the FUSECTL Register
  if (IqatIpGlobalPolicy->IqatIpConfig.SetFuseCtl) {

    // Check FUSECTL Register
    FuseCtl  = MmioRead32 (QatMemBase + FUSECTL_REG_OFFSET);// & FUSECTL_SKU_MASK
    DEBUG ((EFI_D_INFO, "FuseCtl = 0x%x\n",FuseCtl));

    if (!FuseCtl) {
      // Set FUSECTL depending on platform policy/setup option
      switch (IqatIpGlobalPolicy->IqatIpConfig.FuseCtlValue) {
        case 1:
          FuseCtlValue = FUSECTL_VALUE_SKU_1;
          break;
        case 2:
          FuseCtlValue = FUSECTL_VALUE_SKU_2;
          break;
        case 3:
          FuseCtlValue = FUSECTL_VALUE_SKU_3;
          break;
        case 4:
          FuseCtlValue = FUSECTL_VALUE_SKU_4;
          break;
        default:
          FuseCtlValue = FUSECTL_VALUE_SKU_1;
          break;
      }

      DEBUG ((EFI_D_INFO, "FuseCtl: set to 0x%x\n",FuseCtlValue));
      MmioOr32 ( (UINTN) (QatMemBase + FUSECTL_REG_OFFSET), (UINT32) FuseCtlValue);

      FuseCtl  = MmioRead32 (QatMemBase + FUSECTL_REG_OFFSET);
      DEBUG ((EFI_D_INFO, "FuseCtl: now = 0x%x\n",FuseCtl));

    } else {
      DEBUG ((EFI_D_ERROR, "ConfigureIqatRegs: Iqat FUSECTL already set [0x%x]\n",FuseCtl));
    }
  }

  // Configure the PPDCNTL Register
  if (IqatIpGlobalPolicy->IqatIpConfig.Set64BMrrMpl) {

    // Check PPDCNTL Register
    PpdCntl  = MmioRead32 (QatMemBase + PPDCNTL_REG_OFFSET);
    DEBUG ((EFI_D_INFO, "PPDCntl = 0x%x\n",PpdCntl));
    PpdCntl |= (PPDCNTL_MRR_64B | PPDCNTL_MPL_64B);
    DEBUG ((EFI_D_INFO, "PPDCntl: set to 0x%x\n",PpdCntl));
    MmioOr32 ( (UINTN) (QatMemBase + PPDCNTL_REG_OFFSET), (UINT32) PpdCntl);

    PpdCntl  = MmioRead32 (QatMemBase + PPDCNTL_REG_OFFSET);
    DEBUG ((EFI_D_INFO, "PPDCntl: now = 0x%x\n",PpdCntl));
  }

  MmioWrite32 (QatMemBase  + PCICMD_OFFSET, QatPciCmd);
  MmioWrite32 (Vrp2MemBase + PCICMD_OFFSET, Vrp2PciCmd); //B0:D6:F0  offset: 0x4

  DEBUG ((EFI_D_INFO, "ConfigureIqatRegs() End...\n"));
  return;
}

/*++
Routine Description:
  IQAT Init

Arguments:
  N/A

--*/
VOID
IqatIpInit (
  VOID
)
{
  IQAT_IP_GLOBAL_POLICY_HOB      *mIqatIpGlobalPolicyHob;
  EFI_PEI_HOB_POINTERS  HobPtr;

  //
  // Get PCH Policy HOB.
  //
  HobPtr.Guid   = GetFirstGuidHob (&gIqatIpPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mIqatIpGlobalPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);

  if (IqatIpIsIqatPresent() && (mIqatIpGlobalPolicyHob->IqatIpConfig.Enable)) {
    ConfigureIqatUncorrectableErrorWA();
    ConfigureIqatSecureRam();
  }
}
