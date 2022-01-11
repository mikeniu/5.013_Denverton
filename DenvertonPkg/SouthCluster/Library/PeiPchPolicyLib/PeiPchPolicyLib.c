/** @file
  This file is PeiPchPolicy library.

@copyright
  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved
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
#include "PeiPchPolicyLibrary.h"

/**
  mDevIntConfig[] table contains data on INTx and IRQ for each device.
  IRQ value for devices which use ITSS INTx->PIRQx mapping need to be set in a way
  that for each multifunctional Dxx:Fy same interrupt pins must map to the same IRQ.
  Those IRQ values will be used to update ITSS.PIRx register.
  In APIC relationship between PIRQs and IRQs is:
  PIRQA -> IRQ16
  PIRQB -> IRQ17
  PIRQC -> IRQ18
  PIRQD -> IRQ19
  PIRQE -> IRQ20
  PIRQF -> IRQ21
  PIRQG -> IRQ22
  PIRQH -> IRQ23

  Devices which use INTx->PIRQy mapping are: cAVS(in PCI mode), SMBus, GbE, TraceHub, PCIe,
  SATA, HECI, IDE-R, KT Redirection, xHCI, Thermal Subsystem, Camera IO Host Controller

  PCI Express Root Ports mapping should be programmed only with values as in below table (D27/28/29)
  otherwise _PRT methods in ACPI for RootPorts would require additional patching as
  PCIe Endpoint Device Interrupt is further subjected to INTx to PIRQy Mapping

  Configured IRQ values are not used if an OS chooses to be in PIC instead of APIC mode
**/
GLOBAL_REMOVE_IF_UNREFERENCED PCH_DEVICE_INTERRUPT_CONFIG mDevIntConfig[] = {
//  {31, 0, PchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, PchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, PchNoInt, 0}, // PMC , doesn't use interrupts
    {31, 4, PchIntA, PchPIRQH}, // SMBus Controller, no default value, programmed in PciCfgSpace 3Dh
//  {31, 5, PchNoInt, 0}, // SPI , doesn't use interrupts
    {31, 7, PchIntA, PchPIRQH}, // TraceHub, INTA is default, RO register
    {28, 0, PchIntA, PchPIRQA}, // SCS: eMMC (SKL PCH-LP Only)
    {27, 0, PchIntA, PchPIRQA}, // IE: HECI #1
    {27, 1, PchIntB, PchPIRQB}, // IE: HECI #2
    {27, 3, PchIntC, PchPIRQC}, // IE: Keyboard and Text (KT) Redirection
    {27, 4, PchIntD, PchPIRQD}, // IE: HECI #3
    {26, 2, PchIntC, PchPIRQC}, // UART2
    {26, 1, PchIntB, PchPIRQB}, // UART1
    {26, 0, PchIntA, PchPIRQA}, // UART0
    {24, 0, PchIntA, PchPIRQA}, // CSME: HECI #1
    {24, 1, PchIntB, PchPIRQB}, // CSME: HECI #2
    {24, 3, PchIntC, PchPIRQC}, // CSME: Keyboard and Text (KT) Redirection
    {24, 4, PchIntD, PchPIRQD}, // CSME: HECI #3
    {23, 0, PchIntA, PchPIRQB}, // VRP1
    {22, 0, PchIntA, PchPIRQA}, // VRP0
    {21, 0, PchIntA, PchPIRQD}, // USB 3.0 xHCI Controller, no default value, programmed in PciCfgSpace 3Dh
    {20, 0, PchIntA, PchPIRQF}, // SATA Controller
    {19, 0, PchIntA, PchPIRQE}, // SATA Controller
    {18, 0, PchIntA, PchPIRQA}, // SMBus 1
    {17, 0, PchIntD, PchPIRQH}, // PCIE_RP[7]
    {16, 0, PchIntC, PchPIRQG}, // PCIE_RP[6]
    {15, 0, PchIntB, PchPIRQF}, // PCIE_RP[5]
    {14, 0, PchIntA, PchPIRQE}, // PCIE_RP[4]
    {12, 0, PchIntD, PchPIRQD}, // PCIE_RP[3]
    {11, 0, PchIntC, PchPIRQC}, // PCIE_RP[2]
    {10, 0, PchIntB, PchPIRQB}, // PCIE_RP[1]
    { 9, 0, PchIntA, PchPIRQA}, // PCIE_RP[0]
    { 6, 0, PchIntA, PchPIRQC}, // VRP2
    { 5, 0, PchIntA, PchPIRQH}, // RCEC
//  { 4, 0, PchNoInt, 0}, // GLREG, doesn't use interrupts.
};

/**
  mPxRcConfig[] table contains data for 8259 routing (how PIRQx is mapped to IRQy).
  This information is used by systems which choose to use legacy PIC
  interrupt controller. Only IRQ3-7,9-12,14,15 are valid. Values from this table
  will be programmed into ITSS.PxRC registers.
**/
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mPxRcConfig[] = {
  11,  // PARC: PIRQA -> IRQ11
  10,  // PBRC: PIRQB -> IRQ10
   6,  // PCRC: PIRQC -> IRQ11
   7,  // PDRC: PIRQD -> IRQ11
  12,  // PERC: PIRQE -> IRQ11
  14,  // PFRC: PIRQF -> IRQ11
  15,  // PGRC: PIRQG -> IRQ11
  15   // PHRC: PIRQH -> IRQ11
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSmbusRsvdAddresses[] = {
  0xA0,
  0xA2,
  0xA4,
  0xA6
};

/**
Initializes the Sata configuration structure policy
  @param[in] SataConfig      The pointer to Sata configuration structure
**/
VOID
CreatePolicyDefaultSata(PCH_SATA_CONFIG *SataConfig)
{
  UINT8                                 PortIndex;
  UINTN                                 Index;
  SataConfig->Enable = TRUE;

//  SataConfig->TestMode             = FALSE;
//  SataConfig->LegacyMode           = FALSE;
  SataConfig->SalpSupport = TRUE;
//  SataConfig->eSATASpeedLimit      = FALSE;
  SataConfig->SataMode = PchSataModeAhci;


  SataConfig->SpeedLimit = PchSataSpeedDefault;

  for (PortIndex = 0; PortIndex < GetPchMaxSataPortNum(); PortIndex++)
  {
    SataConfig->PortSettings[PortIndex].Enable = TRUE;
//    SataConfig->PortSettings[PortIndex].HotPlug          = FALSE;

    SataConfig->PortSettings[PortIndex].InterlockSw = FALSE;
//    SataConfig->PortSettings[PortIndex].External         = FALSE;
//    SataConfig->PortSettings[PortIndex].SpinUp           = FALSE;
//    SataConfig->PortSettings[PortIndex].SolidStateDrive  = FALSE;
//    SataConfig->PortSettings[PortIndex].DevSlp           = FALSE;
//    SataConfig->PortSettings[PortIndex].EnableDitoConfig = FALSE;
    SataConfig->PortSettings[PortIndex].DmVal = 15;
    SataConfig->PortSettings[PortIndex].DitoVal = 625;
  }

//  SataConfig->Rst.RaidAlternateId  = FALSE;
  SataConfig->Rst.Raid0 = TRUE;
  SataConfig->Rst.Raid1 = TRUE;
  SataConfig->Rst.Raid10 = TRUE;
  SataConfig->Rst.Raid5 = TRUE;
  SataConfig->Rst.Irrt = TRUE;
  SataConfig->Rst.OromUiBanner = TRUE;
  SataConfig->Rst.OromUiDelay = PchSataOromDelay2sec;
  SataConfig->Rst.HddUnlock = TRUE;
  SataConfig->Rst.LedLocate = TRUE;
  SataConfig->Rst.IrrtOnly = TRUE;
  SataConfig->Rst.SmartStorage = TRUE;

  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++)
  {
    //SataConfig->RstPcieStorageRemap[Index].Enable                     = 0;
    //SataConfig->RstPcieStorageRemap[Index].RstPcieStoragePort         = 0;
    SataConfig->RstPcieStorageRemap[Index].DeviceResetDelay = 100;
  }


}

/**
  PchCreatePolicyDefaults creates the default setting of PCH Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] PchPolicyPpi      The pointer to get PCH Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
PchCreatePolicyDefaults (
  OUT  PCH_POLICY_PPI                   **PchPolicyPpi
  )
{
  PCH_POLICY_PPI           *PchPolicy;
  PCH_SERIES               PchSeries;
  UINT32                   PortIndex;
  UINT32                   Index;
  UINT8                    IntConfigTableEntries;

  PchSeries = GetPchSeries ();

  PchPolicy = (PCH_POLICY_PPI *) AllocateZeroPool (sizeof (PCH_POLICY_PPI));
  if (PchPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Policy not listed here are set to 0/FALSE as default.
  //

  /********************************
    General initialization
  ********************************/
  PchPolicy->Revision                = PCH_POLICY_REVISION;
  PchPolicy->AcpiBase                = PcdGet16 (PcdAcpiBaseAddress);

  /********************************
    PCH general configuration
  ********************************/
  //
  // Default Svid Sdid configuration
  //
  PchPolicy->PchConfig.SubSystemVendorId = V_PCH_INTEL_VENDOR_ID;
  PchPolicy->PchConfig.SubSystemId       = V_PCH_DEFAULT_SID;
  /********************************
    SATA related settings
  ********************************/

  for (Index = 0; Index < PCH_MAX_SATA_CONTROLLERS ; ++Index) {
    CreatePolicyDefaultSata(&PchPolicy->SataConfig[Index]);
  }


  /********************************
    USB related configuration
  ********************************/
  for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb2PortNum (); PortIndex++) {
    PchPolicy->UsbConfig.PortUsb20[PortIndex].Enable  = TRUE;
  }

  for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++) {
    PchPolicy->UsbConfig.PortUsb30[PortIndex].Enable  = TRUE;
  }
  //
  // USB Port Over Current Pins mapping, please set as per board layout.
  // Default is PchUsbOverCurrentPin0(0)
  //
   PchPolicy->UsbConfig.PortUsb20[ 2].OverCurrentPin = PchUsbOverCurrentPin1;
   PchPolicy->UsbConfig.PortUsb20[ 3].OverCurrentPin = PchUsbOverCurrentPin1;
  PchPolicy->UsbConfig.PortUsb30[2].OverCurrentPin  = PchUsbOverCurrentPin1;
  PchPolicy->UsbConfig.PortUsb30[3].OverCurrentPin  = PchUsbOverCurrentPin1;
  //
  // Default values of USB2 AFE settings.
  //
  for (Index = 0; Index < PCH_H_XHCI_MAX_USB2_PORTS; Index++) {
    PchPolicy->UsbConfig.PortUsb20[Index].Afe.Petxiset  = 3;
    PchPolicy->UsbConfig.PortUsb20[Index].Afe.Txiset    = 2;
    PchPolicy->UsbConfig.PortUsb20[Index].Afe.Predeemp  = 1;
    PchPolicy->UsbConfig.PortUsb20[Index].Afe.Pehalfbit = 1;
  }

  //
  // Enable/Disable SSIC support in the setup menu
  //
  for (PortIndex = 0; PortIndex < PCH_XHCI_MAX_SSIC_PORT_COUNT; PortIndex++) {
  }


  /********************************
    Io Apic configuration
  ********************************/
  PchPolicy->IoApicConfig.IoApicId           = 0x02;
  PchPolicy->IoApicConfig.IoApicEntry24_119  = TRUE;

  /********************************
    HPET Configuration
  ********************************/
  PchPolicy->HpetConfig.Enable              = TRUE;
  PchPolicy->HpetConfig.Base                = PCH_HPET_BASE_ADDRESS;


  /********************************
    SMBus configuration
  ********************************/
  PchPolicy->SmbusConfig.Enable                = TRUE;
  PchPolicy->SmbusConfig.SmbusIoBase           = PcdGet16 (PcdSmbusBaseAddress);
  ASSERT (sizeof (mSmbusRsvdAddresses) <= PCH_MAX_SMBUS_RESERVED_ADDRESS);
  PchPolicy->SmbusConfig.NumRsvdSmbusAddresses = sizeof (mSmbusRsvdAddresses);
  CopyMem (
    PchPolicy->SmbusConfig.RsvdSmbusAddressTable,
    mSmbusRsvdAddresses,
    sizeof (mSmbusRsvdAddresses)
    );
  PchPolicy->SmbusConfig.DynamicPowerGating    = FALSE;
  PchPolicy->SmbusConfig.SMBusIOSFClockGating  = TRUE;
  PchPolicy->SmbusConfig.SMBusHostSpeedConfig  = PchSMBusStandard80Khz;

  /********************************
    Lockdown configuration
  ********************************/
  PchPolicy->LockDownConfig.GlobalSmi       = TRUE;
  //
  // PCH BIOS Spec Flash Security Recommendations,
  // Intel strongly recommends that BIOS sets the BIOS Interface Lock Down bit. Enabling this bit
  // will mitigate malicious software attempts to replace the system BIOS option ROM with its own code.
  // Here we always enable this as a Policy.
  //
  PchPolicy->LockDownConfig.BiosInterface   = TRUE;
  PchPolicy->LockDownConfig.GpioLockDown    = TRUE;
  PchPolicy->WdtConfig.DisableAndLock       = TRUE;
  PchPolicy->LockDownConfig.RtcLock         = TRUE;


  /********************************
    MiscPm Configuration
  ********************************/
  PchPolicy->PmConfig.WakeConfig.PmeB0S5Dis                = FALSE;
  PchPolicy->PmConfig.PmcReadDisable                       = TRUE;
  PchPolicy->PchHideP2sbEnable                             = TRUE;



  /********************************
    Serial IRQ Configuration
  ********************************/
  PchPolicy->SerialIrqConfig.SirqEnable       = TRUE;
  PchPolicy->SerialIrqConfig.SirqMode         = PchQuietMode;
  PchPolicy->SerialIrqConfig.StartFramePulse  = PchSfpw4Clk;


  /********************************
    Interrupt Configuration
  ********************************/
  IntConfigTableEntries = sizeof (mDevIntConfig) / sizeof (PCH_DEVICE_INTERRUPT_CONFIG);
  ASSERT (IntConfigTableEntries <= PCH_MAX_DEVICE_INTERRUPT_CONFIG);
  PchPolicy->PchInterruptConfig.NumOfDevIntConfig = IntConfigTableEntries;
  CopyMem (
    PchPolicy->PchInterruptConfig.DevIntConfig,
    mDevIntConfig,
    sizeof (mDevIntConfig)
    );

  ASSERT ((sizeof (mPxRcConfig) / sizeof (UINT8)) <= PCH_MAX_PXRC_CONFIG);
  CopyMem (
    PchPolicy->PchInterruptConfig.PxRcConfig,
    mPxRcConfig,
    sizeof (mPxRcConfig)
    );

  PchPolicy->PchInterruptConfig.GpioIrqRoute = 14;
  PchPolicy->PchInterruptConfig.SciIrqSelect = 9;
  PchPolicy->PchInterruptConfig.TcoIrqSelect = 9;


  /********************************
    DCI Configuration
  ********************************/
  PchPolicy->DciConfig.DciAutoDetect     = TRUE;

  /********************************
    LPC Configuration
  ********************************/
  PchPolicy->LpcConfig.EnhancePort8xhDecoding     = TRUE;



  //eMMC
  PchPolicy->eMMCEnabled = 1;

  *PchPolicyPpi = PchPolicy;
  return EFI_SUCCESS;
}

/**
  PchInstallPolicyPpi installs PchPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] PchPolicyPpi      The pointer to PCH Policy PPI instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
PchInstallPolicyPpi (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_PPI_DESCRIPTOR                *PchPolicyPpiDesc;

  PchPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (PchPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  PchPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PchPolicyPpiDesc->Guid  = &gPchPolicyPpiGuid;
  PchPolicyPpiDesc->Ppi   = PchPolicyPpi;

  //
  // Print whole PCH_POLICY_PPI and serial out.
  //
  PchPrintPolicyPpi (PchPolicyPpi);

  //
  // Install PCH Policy PPI
  //
  Status = PeiServicesInstallPpi (PchPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}
