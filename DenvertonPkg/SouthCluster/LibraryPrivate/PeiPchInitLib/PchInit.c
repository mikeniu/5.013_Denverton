/** @file
  The PCH Init After Memory PEI module.

@copyright
  Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#include <Ppi/MePlatformPolicyPei.h>
#include <SimicsDebug.h>
#include <MeChipset.h>
#include "Wdt.h"
#include "PchSpi.h"
#include <IncludePrivate/Library/PchSmmControlLib.h>
static EFI_PEI_NOTIFY_DESCRIPTOR mHeciNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiMePlatformPolicyPpiGuid,
  PchHsioOnHeciPpi
};

/**
  Initialize IOAPIC according to IoApicConfig policy of the PCH
  Policy PPI

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchIoApicInit (
  IN  PCH_POLICY_PPI     *PchPolicyPpi
  )
{
  UINT16                RegData16;
  UINTN                 P2sbBase;
  PCH_IOAPIC_CONFIG     *IoApicConfig;
  UINT32                IoApicAddress;
  UINT32                IoApicId;

  //
  // Get P2SB base address
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );
  IoApicConfig    = &PchPolicyPpi->IoApicConfig;

  if (IoApicConfig->ApicRangeSelect != (UINT8)(MmioRead16 (P2sbBase + R_PCH_P2SB_IOAC) & B_PCH_P2SB_IOAC_ASEL)) {
    ///
    /// Program APIC Range Select bits that define address bits 19:12 for the IOxAPIC range.
    /// This value must not be changed unless the IOxAPIC Enable bit is cleared.
    ///
    MmioAnd16 (P2sbBase + R_PCH_P2SB_IOAC, (UINT16)~(B_PCH_P2SB_IOAC_AE));
    ///
    /// Program APIC Range Select bits at P2SB PCI offset 64h[7:0]
    ///
    MmioAndThenOr16 (
      P2sbBase + R_PCH_P2SB_IOAC,
      (UINT16)~(B_PCH_P2SB_IOAC_ASEL),
      (UINT16) IoApicConfig->ApicRangeSelect
      );
  }
  ///
  /// PCH BIOS Spec Section 6.6.2.1
  /// 1. Enable the IOAPIC by setting the APIC Enable bit, P2SB PCI offset 64h [8], if the
  ///    system needs to use the IOxAPIC. The APIC Enable bits needs read back after the bit is written.
  ///
  MmioOr16 (P2sbBase + R_PCH_P2SB_IOAC, B_PCH_P2SB_IOAC_AE);
  ///
  /// Reads back for posted write to take effect
  ///
  MmioRead16 (P2sbBase + R_PCH_P2SB_IOAC);

  ///
  /// Get current IO APIC ID
  ///
  IoApicAddress = (UINT32) (MmioRead8 (P2sbBase + R_PCH_P2SB_IOAC) << N_PCH_IO_APIC_ASEL);
  MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 0);
  IoApicId = MmioRead32 ((UINTN) (R_PCH_IO_APIC_DATA | IoApicAddress)) >> 24;
  ///
  /// IO APIC ID is at APIC Identification Register [27:24]
  ///
  if ((IoApicConfig->IoApicId != IoApicId) && (IoApicConfig->IoApicId < 0x10)) {
    ///
    /// Program APIC ID
    ///
    MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 0);
    MmioWrite32 ((UINTN) (R_PCH_IO_APIC_DATA | IoApicAddress), (UINT32) (IoApicConfig->IoApicId << 24));
  }

  if (IoApicConfig->IoApicEntry24_119 == FALSE) {
    ///
    /// Program IOAPIC maximal entry to 24 in PCR[ITSS] + 31FCh[9]
    /// and in IOAPIC Version Register
    PchPcrAndThenOr16 (PID_ITSS, R_PCH_PCR_ITSS_GIC, 0xFFFF, (UINT16) B_PCH_PCR_ITSS_GIC_MAX_IRQ_24);
    MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 1);
    MmioWrite32 ((R_PCH_IO_APIC_DATA | IoApicAddress), 0x170020);
  } else {
    ///
    /// Program IOAPIC maximal entry to 119 in PCR[ITSS] + 31FCh[9]
    /// and in IOAPIC Version Register
    PchPcrAndThenOr16 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT16)(~B_PCH_PCR_ITSS_GIC_MAX_IRQ_24), 0x0);
    MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 1);
    MmioWrite32 ((R_PCH_IO_APIC_DATA | IoApicAddress), 0x770020);
  }

  ///
  /// Program this field to provide a unique bus:device:function number for the internal IOxAPIC
  ///
  if (IoApicConfig->BdfValid) {
    RegData16 = ((UINT16) (IoApicConfig->BusNumber) << 8) & B_PCH_P2SB_IBDF_BUF;
    RegData16 |= ((UINT16) (IoApicConfig->DeviceNumber) << 3) & B_PCH_P2SB_IBDF_DEV;
    RegData16 |= (UINT16) (IoApicConfig->FunctionNumber) & B_PCH_P2SB_IBDF_FUNC;
    MmioWrite16 ((UINTN) (P2sbBase + R_PCH_P2SB_IBDF), RegData16);
  }

  //
  // Build the resource descriptor hob for IOAPIC address resource.
  //
  // Denverton AptioV override Start
  // Resource allocation done using RSVCHIPSET
  /*
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    R_PCH_IO_APIC_INDEX | IoApicAddress,
    0x1000
    );
  BuildMemoryAllocationHob (
    R_PCH_IO_APIC_INDEX | IoApicAddress,
    0x1000,
    EfiMemoryMappedIO
    );
    */
	// Denverton AptioV override End

  return EFI_SUCCESS;
}

/**
  The function performs ModPHY specific programming.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance

  @retval EFI_SUCCESS                   The ModPHY required settings programmed correctly
**/
EFI_STATUS
EFIAPI
PchHsioProg (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{
  return EFI_SUCCESS;
}

/**
  The function performs P2SB specific programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS           The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchP2sbConfigure (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{
  UINTN                                 P2sbBase;
  UINT16                                Data16 = 0;
#ifndef FSP_FLAG
  EFI_STATUS                            Status;
  RAS_GLOBAL_POLICY_PPI                 *RasGlobalPolicyPpi;
#endif //!FSP_FLAG
  //
  // Get P2SB base address
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );

#ifndef FSP_FLAG
  //
  // Locate installed RAS Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gRasPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &RasGlobalPolicyPpi
             );

  ASSERT_EFI_ERROR (Status);

  if (Status == EFI_SUCCESS) {
    //
    //Enable P2SB Data Parity and Command parity Error in P2SB Control Register (0xE0)
    //
    MmioOr32 (P2sbBase + R_PCH_P2SB_P2SBC,  (UINT32)(B_PCH_P2SB_P2SBC_DPPEE | B_PCH_P2SB_P2SBC_CPEE | B_PCH_P2SB_P2SBC_DPEE));

    //
    //Enable P2SB PERR and SERR
    //
    if (RasGlobalPolicyPpi->PCIeSystemErrEnable)
      Data16 |= EFI_PCI_COMMAND_SERR;

    if (RasGlobalPolicyPpi->PCIeParityErrEnable)
      Data16 |= EFI_PCI_COMMAND_PARITY_ERROR_RESPOND;

    MmioAndThenOr16 (P2sbBase + PCI_COMMAND_OFFSET,  (UINT16)~(EFI_PCI_COMMAND_SERR | EFI_PCI_COMMAND_PARITY_ERROR_RESPOND), Data16);
  }
#else
  //
  //Enable P2SB PERR and SERR
  //
  Data16 |= EFI_PCI_COMMAND_SERR;
  Data16 |= EFI_PCI_COMMAND_PARITY_ERROR_RESPOND;
  MmioAndThenOr16 (P2sbBase + PCI_COMMAND_OFFSET,  (UINT16)~(EFI_PCI_COMMAND_SERR | EFI_PCI_COMMAND_PARITY_ERROR_RESPOND), Data16);
#endif //!FSP_FLAG


  return EFI_SUCCESS;
}


/**
  The function performs GPIO Power Management programming.

  @param[in] PchPolicyPpi    The PCH Policy PPI instance

  @retval EFI_SUCCESS                The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchGpioConfigurePm (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{

#ifdef PCH_PO_FLAG
  if (PchPolicyPpi->PwrOnSafeMode.Bit.GpioSafeRegister) {
    //
    // Leave MISCCFG.GPDLCGEn default values
    //
  } else {
#endif // PCH_PO_FLAG
  //
  // Program MISCCFG.GPDLCGEn and MISCCFG.GPDPCGEn for Community 0
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM0, R_PCH_PCR_GPIO_MISCCFG),
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_GPIO_MISCCFG_NCSI_STAGE_EN | B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN | B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN)
    );

  //
  // Program MISCCFG.GPDLCGEn and MISCCFG.GPDPCGEn for Community 1
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM1, R_PCH_PCR_GPIO_MISCCFG),
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_GPIO_MISCCFG_NCSI_STAGE_EN | B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN | B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN)
    );
#ifdef PCH_PO_FLAG
  }
#endif // PCH_PO_FLAG

  return EFI_SUCCESS;
}

/**
  The function performs RTC specific programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS           The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchRtcConfigure (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{

#ifdef PCH_PO_FLAG
  if (PchPolicyPpi->PwrOnSafeMode.Bit.RtcSafeRegister) {
  ///
  /// Leave default register values
  ///
  } else {
#endif // PCH_PO_FLAG
  ///
  /// Set PCR[RTC] + 3F00h[8] = 1b
  ///
  PchPcrAndThenOr32 (
    PID_RTC, R_PCH_PCR_RTC_3F00,
    ~(UINT32)(0),
    (UINT32)(BIT8)
    );
#ifdef PCH_PO_FLAG
  }
#endif // PCH_PO_FLAG

  //
  // Clear RTC SMI enable and status.
  //
  PchPcrWrite32 (PID_RTC, R_PCH_PCR_RTC_UIPSMI, 0x00030000);

  return EFI_SUCCESS;
}

/**
  The function performs SMBUS specific programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS           The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchSmbusConfigure (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{
  UINTN     SmbusBaseAddress;
  UINT32    Data32And;
  UINT32    Data32Or;

  SmbusBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_SMBUS,
                       PCI_FUNCTION_NUMBER_PCH_SMBUS
                       );
#ifdef PCH_PO_FLAG
  if (PchPolicyPpi->PwrOnSafeMode.Bit.SmbusSafeRegister) {
  ///
  /// Leave default register values
  ///
  } else {
#endif // PCH_PO_FLAG

  ///
  /// Enable SMBus dynamic clock gating by setting Smbus PCI offset 80h [16,14,12,10,8] = 0b and [5] = 1b respectively
  /// Disable SMBus dynamic clock gating of PGCB by [18] = 1
  ///
  Data32And = (UINT32)~(BIT16 | BIT14 | BIT12 | BIT10 | BIT8);
  Data32Or  = BIT18;
  if (PchPolicyPpi->SmbusConfig.DynamicPowerGating) {
    Data32Or &= (UINT32)~BIT18;
  }
  MmioAndThenOr32 (
    SmbusBaseAddress + R_PCH_SMBUS_80,
    Data32And,
    Data32Or
    );
#ifdef PCH_PO_FLAG
  }
#endif // PCH_PO_FLAG

  return EFI_SUCCESS;
}

/**
  The function performs Serial IRQ specific programming.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
EFIAPI
PchConfigureSerialIrq (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  UINTN                                 PciPmcRegBase;
  UINTN                                 PciLpcRegBase;
  UINT8                                 RegData8;

  if (PchPolicyPpi->SerialIrqConfig.SirqEnable == FALSE) {
    return;
  }

  PciLpcRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC);
  PciPmcRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC);
  RegData8        = 0;

  ///
  /// PCH BIOS Spec Section 6.3 Serial IRQs
  /// The only System BIOS requirement to use IRQs as a serial IRQ is to enable the function in LPC PCI offset 64h[7] and
  /// select continuous or quiet mode, LPC PCI offset 64h[6].
  /// PCH requires that the System BIOS first set the SERIRQ logic to continuous mode operation for at least one frame
  /// before switching it into quiet mode operation. This operation should be performed during the normal boot sequence
  /// as well as a resume from STR (S3).
  ///
  RegData8  = MmioRead8 (PciLpcRegBase + R_PCH_LPC_SERIRQ_CNT);
  RegData8  &= (UINT8) ~(B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SFPW);

  switch (PchPolicyPpi->SerialIrqConfig.StartFramePulse) {
  case PchSfpw8Clk:
    RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_8CLK;
    break;

  case PchSfpw6Clk:
    RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_6CLK;
    break;

  case PchSfpw4Clk:
  default:
    RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_4CLK;
    break;
  }
  ///
  /// Set the SERIRQ logic to continuous mode
  ///
  RegData8 |= (UINT8) (B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SIRQMD);

  MmioWrite8 (PciLpcRegBase + R_PCH_LPC_SERIRQ_CNT, RegData8);

  ///
  /// PCH BIOS Spec Section 6.3 Serial IRQs
  /// The only System BIOS requirement to use IRQs as a serial IRQ is to enable the function
  /// in LPC PCI offset 64h[7] and select continuous or quiet mode, LPC PCI offset 64h[6].
  ///
  if (PchPolicyPpi->SerialIrqConfig.SirqMode == PchQuietMode) {
    MmioAnd8 (PciLpcRegBase + R_PCH_LPC_SERIRQ_CNT, (UINT8)~B_PCH_LPC_SERIRQ_CNT_SIRQMD);
  }
}

/**
  Clear RTC PWR STS.
**/
VOID
ClearRtcPwrSts (
  VOID
  )
{
  UINTN                                 PciPmcRegBase;

  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );
  ///
  /// PCH BIOS Spec Section 19.8.1 RTC Resets
  /// The PCH will set the RTC_PWR_STS bit (PMC PCI offset A4h[2]) when the RTCRST# pin goes low.
  /// The System BIOS shouldn't rely on the RTC RAM contents when the RTC_PWR_STS bit is set.
  /// BIOS should clear this bit by writing a 0 to this bit position.
  /// This bit isn't cleared by any reset function.
  ///
  MmioAnd8 ((UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B), (UINT8) (~B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS));
}

/**
  Pch End of PEI callback function. This is the last event before entering DXE and OS in S3 resume.
**/
VOID
PchOnEndOfPei (
  VOID
  )
{
  EFI_STATUS      Status;
  UINTN           P2sbBase;
  PCH_POLICY_PPI  *PchPolicy;
  SI_POLICY_PPI   *SiPolicy;
  PCH_SERIES      PchSeries;
  PCH_STEPPING    PchStep;
  EFI_BOOT_MODE   BootMode;

  DEBUG ((DEBUG_INFO, "PchOnEndOfPei after memory PEI module - Start\n"));

  PchSeries = GetPchSeries ();
  PchStep   = PchStepping ();

  //
  // Get Policy settings through the SiPolicy PPI
  //
  Status = PeiServicesLocatePpi (
             &gSiPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // SI_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return;
  }

  //
  // Get Policy settings through the PchPolicy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPchPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PchPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // PCH_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return;
  }

 //
  // Clear RTC PWR STS in end of PEI so others won't miss the status
  //
  ClearRtcPwrSts ();

  //
  // Lock XHCI registers
  //
  UsbEndOfInit (&(PchPolicy->UsbConfig));


  WdtEndOfPeiCallback ();

  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );

  Status = PchTraceHubOnEndOfPei (PchPolicy);

#ifdef FSP_FALG
  //
  // Configure root port function number mapping
  // For FSP, execute RPFN mapping here before PCI enumeration.
  //
  Status = PchConfigureRpfnMapping ();
  ASSERT_EFI_ERROR (Status);
#endif

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // For S3 path:
  //   If it's PCH A0 stepping, skip the P2SB SBI lock and Hidden.
  //   else, lock SBI, hide P2SB and do P2SB PSF configuration space disable.
  // For S4/S5 path:
  //   Hide P2SB only.
  //   If FSP is supported, also lock SBI and do P2SB PSF configuration space disable.
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
      ///
      /// Lock down the P2sb SBI before going into OS.
      /// This only apply to PCH B0 onward.
      ///
      ConfigureP2sbSbiLock (&PchPolicy->P2sbConfig);
      ///
      /// Hide P2SB controller in the end of PEI.
      ///
      PchHideP2sb (P2sbBase);
  } else {
    ///
    /// Hide P2SB controller in the end of PEI.
    ///
    PchHideP2sb (P2sbBase);
  }

  if (BootMode == BOOT_ON_S3_RESUME) {
    RemovePsfAccess (&PchPolicy->P2sbConfig);
  }

  DEBUG ((DEBUG_INFO, "PchOnEndOfPei after memory PEI module - End\n"));
}

/**
  Pch init after memory PEI module

  @param[in] SiPolicy     The Silicon Policy PPI instance
  @param[in] PchPolicy    The PCH Policy PPI instance

  @retval None
**/
VOID
EFIAPI
PchInit (
  IN  SI_POLICY_PPI  *SiPolicy,
  IN  PCH_POLICY_PPI *PchPolicy
  )
{
  EFI_STATUS                            Status;
  PCH_PCIE_DEVICE_OVERRIDE              *PcieDeviceTable;
  VOID                                  *HobPtr;
  UINTN                                 Count;
  PCH_SERIES                            PchSeries;

  DEBUG ((DEBUG_INFO, "PchInit - Start\n"));

  PchSeries = GetPchSeries ();

  //
  // Install PEI SMM Control PPI
  //
  Status = PchSmmControlInit ();

  //
  // Locate and PCIe device override table and add to HOB area.
  //
  Status = PeiServicesLocatePpi (
             &gPchPcieDeviceTablePpiGuid,
             0,
             NULL,
             (VOID **)&PcieDeviceTable
             );

  if (Status == EFI_SUCCESS) {
    for (Count = 0; PcieDeviceTable[Count].DeviceId != 0; Count++) {
    }
    DEBUG ((DEBUG_INFO, "PCH Installing PcieDeviceTable HOB (%d entries)\n", Count));
    HobPtr = BuildGuidDataHob (
               &gPchDeviceTableHobGuid,
               PcieDeviceTable,
               Count * sizeof (PCH_PCIE_DEVICE_OVERRIDE)
               );
    ASSERT (HobPtr != 0);
  }

  HobPtr = BuildGuidDataHob (&gPchPolicyHobGuid, PchPolicy, sizeof (PCH_POLICY_PPI));
  ASSERT (HobPtr != 0);

  //
  // Check if ME has the right HSIO Settings and sync with ME if required
  //
  Status = PeiServicesNotifyPpi (&mHeciNotifyList);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure P2SB
  //
  Status = PchP2sbConfigure (PchPolicy);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure IOAPIC
  //
  Status = PchIoApicInit (PchPolicy);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure interrupts.
  //
  Status = PchConfigureInterrupts (PchPolicy);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure PCH xHCI, post-mem phase
  //
  Status = ConfigureXhci (PchPolicy, SiPolicy->TempMemBaseAddr);
#ifdef FSP_FLAG
  Status = PeiMiscIpInit(PchPolicy);
#endif
  //
  // Configure GPIO PM settings
  //
  Status = PchGpioConfigurePm (PchPolicy);

  //
  // Configure RTC
  //
  Status = PchRtcConfigure (PchPolicy);

  //
  // Configure SMBUS
  //
  Status = PchSmbusConfigure (PchPolicy);

  //
  // Configure PM settings
  //
  Status = PchPmInit (PchPolicy);

  //
  // Configure Serial IRQ
  //
  PchConfigureSerialIrq (PchPolicy);

  //
  // Configure SPI after memory
  //
  ConfigureSpiAfterMem (PchPolicy);

  DEBUG ((DEBUG_INFO, "PchInit - End\n"));
}


EFI_STATUS
EFIAPI
PchHsioOnHeciPpi (
  IN EFI_PEI_SERVICES                  **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN VOID                              *Ppi
  )
{
  EFI_STATUS                            Status;
  Status = 0;
  DEBUG ((DEBUG_INFO, "PchHsioOnHeciPpi start\n"));
  if ( !PEI_IN_SIMICS ) {
    Status = PchHsioChipsetInitProg(TRUE);
  }
  return Status;
}
