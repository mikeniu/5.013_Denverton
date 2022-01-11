/** @file
  This file is SampleCode of the library for Intel CPU PEI Platform Policy initialzation.

  Copyright (c) 2009 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/

#include <CpuPlatformPolicyUpdatePei.h>
#include <Library/MmPciLib.h>

EFI_STATUS
EFIAPI
UpdatePeiCpuPlatformPolicy (
  IN CONST    EFI_PEI_SERVICES        **PeiServices,
  IN OUT  PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
/*++

Routine Description:

  This function performs CPU PEI Platform Policy initialzation.

Arguments:

  PeiServices             General purpose services available to every PEIM.
  CpuPlatformPolicyPpi    The CPU Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{
  EFI_STATUS                  Status;
  CPU_CONFIG_PPI              *CpuConfig;
  POWER_MGMT_CONFIG_PPI       *PowerMgmtConfig;
  EPOC_CONFIG_PPI             *EpocConfig;
  TXT_CONFIG                  *TxtConfig;
  OVERCLOCKING_CONFIG_PPI     *OcConfig;
  WDT_PPI                     *gWdtPei;
  EFI_GUID                    SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                       VariableSize;
  SYSTEM_CONFIGURATION        SetupDataBuffer;
  SYSTEM_CONFIGURATION*       SetupData;
  PFAT_CONFIG                 *PfatConfig;
  VOID                        *Sha256Context;
  VOID                        *Data;
  UINTN                       DataSize;
  UINT8                       HashValue[SHA256_DIGEST_SIZE];
  UINT8                       EcStatus;
  EFI_PEI_STALL_PPI           *StallPpi;
  EFI_PEI_CPU_IO_PPI          *CpuIo;
  UINT8                       WdtTimeout;
  UINT8                       i;
  UINT32                      FlashBase;
  UINT32                      FlashAddr;
  UINTN                       PchSpiBase;
  UINT8                       PpdtPkeySlot0[] = { 0x6a, 0x94, 0x81, 0x87, 0xfd, 0x8c, 0x51, 0x0a, 0x2a, 0x74, 0x9c, 0x7b, 0xe3, 0xf7, 0x5c, 0x3a, 0xbf, 0xce, 0x60, 0x4f, 0x68, 0xeb, 0xc6, 0x8f, 0x69, 0x98, 0x8c, 0xc5, 0xdb, 0xe3, 0x29, 0x70 };
  UINT8                       PpdtPkeySlot1[] = { 0x1c, 0x51, 0x3a, 0xc5, 0x1c, 0x3d, 0x63, 0x10, 0xef, 0xed, 0xfe, 0xc5, 0xf3, 0x4b, 0xf2, 0xe0, 0x9b, 0x22, 0xe8, 0x06, 0xab, 0xd3, 0x19, 0x2e, 0xfa, 0xfb, 0x6e, 0xd9, 0x36, 0x0d, 0x68, 0x18 };
  UINT8                       PpdtPkeySlot2[] = { 0x19, 0x0b, 0x33, 0xf8, 0xde, 0x3a, 0xa7, 0x9b, 0x57, 0xad, 0xb2, 0x45, 0x86, 0x0e, 0x7f, 0x0e, 0x40, 0x62, 0x80, 0x22, 0x8f, 0x04, 0x92, 0xec, 0x87, 0x44, 0x81, 0xd9, 0xef, 0xed, 0x9f, 0xa3 };
  PLATFORM_INFO_HOB           *PlatformInfoHob;

	PchSpiBase = MmioRead32 (MmPciBase (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                            PCI_DEVICE_NUMBER_PCH_SPI,
                            PCI_FUNCTION_NUMBER_PCH_SPI)
                            + R_PCH_SPI_BAR0) &~(0xFFF);

  CpuConfig               = CpuPlatformPolicyPpi->CpuConfig;
  PowerMgmtConfig         = CpuPlatformPolicyPpi->PowerMgmtConfig;
  EpocConfig              = CpuPlatformPolicyPpi->EpocConfig;
  TxtConfig               = CpuPlatformPolicyPpi->SecurityConfig->TxtConfig;
  PfatConfig              = CpuPlatformPolicyPpi->SecurityConfig->PfatConfig;
  OcConfig                = CpuPlatformPolicyPpi->OverclockingConfig;

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (PeiServices, &PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }

  //
  // Make sure ReadOnlyVariablePpi is available
  //
  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gEfiPeiReadOnlyVariable2PpiGuid,
                            0,
                            NULL,
                            &VariableServices
                            );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate WDT_PPI (ICC WDT PPI)
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gWdtPpiGuid,
             0,
             NULL,
             &gWdtPei
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Get Setup Variable
  //
  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                              VariableServices,
                              PLATFORM_SETUP_VARIABLE_NAME,
                              &SystemConfigurationGuid,
                              NULL,
                              &VariableSize,
                              &SetupDataBuffer
                              );
  SetupData = &SetupDataBuffer;
  if (EFI_ERROR (Status)) {
    //
    // Setup variable was not found, need to put defaults here.
    //
    DEBUG ((DEBUG_ERROR, " PlatformCpuInit: Setup Variable was not found, using defaults\n"));
    CpuConfig->BistOnReset                = 0;
    CpuConfig->HyperThreading             = 1;
    CpuConfig->VmxEnable                  = 1;
    CpuConfig->ActiveCoreCount            = 0;
    CpuConfig->CpuRatioOverride           = 0;
    CpuConfig->CpuRatio                   = 63;
    CpuConfig->CpuMaxNonTurboRatio        = 63;
    CpuConfig->Pfat                       = 0;
    PowerMgmtConfig->TccActivationOffset  = 0;
    PowerMgmtConfig->VrCurrentLimit       = (UINT16)VR_CURRENT_DEFAULT;
    PowerMgmtConfig->VrCurrentLimitLock   = 0;
    PowerMgmtConfig->BootInLfm            = 0;
    PowerMgmtConfig->VrMiscIoutSlope      = 0x200;
    PowerMgmtConfig->VrMiscIoutOffset     = 0;
    PowerMgmtConfig->VrMiscIoutOffsetSign = 0;
    PowerMgmtConfig->VrMiscMinVid                = V_MSR_VR_MISC_CONFIG_MIN_VID_DEFAULT;
    PowerMgmtConfig->VrMiscIdleExitRampRate      = CPU_FEATURE_ENABLE;
    PowerMgmtConfig->VrMiscIdleEntryRampRate     = CPU_FEATURE_DISABLE;
    PowerMgmtConfig->VrMiscIdleEntryDecayEnable  = CPU_FEATURE_ENABLE;
    if (PlatformInfoHob->PlatformType == TypeUlt) {
      PowerMgmtConfig->VrMiscSlowSlewRateConfig  = V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_DEFAULT;
      PowerMgmtConfig->VrMisc2FastRampVoltage    = V_MSR_VR_MISC_CONFIG2_FAST_RAMP_VOLTAGE_DEFAULT;
      PowerMgmtConfig->VrMisc2MinC8Voltage       = V_MSR_VR_MISC_CONFIG2_MIN_C8_VOLTAGE_DEFAULT;
      PowerMgmtConfig->VrPSI4enable              = CPU_FEATURE_ENABLE;
    }
    PowerMgmtConfig->FivrSscEnable        = 1;
    PowerMgmtConfig->FivrSscPercent       = 30;

    EpocConfig->HDEnable = SetupData->EarlyPowerOnHDEnable;
    EpocConfig->FclkFreq = SetupData->EarlyPowerOnFclkFreq;

    CpuConfig->MlcStreamerPrefetcher      = CPU_FEATURE_ENABLE;
    CpuConfig->MlcSpatialPrefetcher       = CPU_FEATURE_ENABLE;
    if (TxtConfig != NULL) {
      TxtConfig->TxtDprMemorySize = 0x400000;
      TxtConfig->McuUpdateDataAddr = FLASH_REGION_MICROCODE_BASE +
                                     ((EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FLASH_REGION_MICROCODE_BASE)->HeaderLength +
                                     sizeof (EFI_FFS_FILE_HEADER);
    }
  } else {
    CpuConfig->BistOnReset          = SetupData->BistOnReset;
    CpuConfig->HyperThreading       = SetupData->HyperThreading;
    CpuConfig->VmxEnable            = SetupData->VT;
    CpuConfig->ActiveCoreCount      = SetupData->ActiveCoreCount;
    CpuConfig->CpuRatioOverride     = SetupData->CpuRatioOverride;
    CpuConfig->CpuRatio             = SetupData->CpuRatio;
    CpuConfig->CpuMaxNonTurboRatio  = SetupData->MaxNonTurboRatio;
    CpuConfig->Pfat                 = SetupData->Pfat;
    if (CpuConfig->Pfat) {
      //
      // Select to Flash Map 0 Register to get the number of flash Component
      //
      MmioAndThenOr32 (
        PchSpiBase + R_PCH_SPI_FDOC,
        (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
        (UINT32) (V_PCH_SPI_FDOC_FDSS_FSDM | R_PCH_SPI_FDBAR_FLASH_MAP0)
      );
      //
      // Copy Zero based Number Of Components
      //
      PfatConfig->NumSpiComponents = (UINT8) (((MmioRead16 (PchSpiBase + R_PCH_SPI_FDOD) & B_PCH_SPI_FDBAR_NC) >> N_PCH_SPI_FDBAR_NC) + 1);
      //
      // Select to Flash Components Register to get Components Density
      //
      MmioAndThenOr32 (
        PchSpiBase + R_PCH_SPI_FDOC,
        (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
        (UINT32) (V_PCH_SPI_FDOC_FDSS_COMP | R_PCH_SPI_FCBA_FLCOMP)
      );
      //
      // Copy Components Density
      //
      FlashAddr = (UINT8) MmioRead32 (PchSpiBase + R_PCH_SPI_FDOD);
      PfatConfig->ComponentSize[0] = (UINT8) (FlashAddr & B_PCH_SPI_FLCOMP_COMP1_MASK);
      PfatConfig->ComponentSize[1] = (UINT8) ((FlashAddr & B_PCH_SPI_FLCOMP_COMP2_MASK) >> 4);
      FlashAddr = 0;
      for (i = 0; i < PfatConfig->NumSpiComponents; i++) {
        FlashAddr += (SPI_SIZE_BASE_512KB << PfatConfig->ComponentSize[i]);
      }
      FlashBase = FlashAddr - FLASH_SIZE;
      PfatConfig->Ppdt.LastSfam              = 4;
      PfatConfig->Ppdt.SfamData[0].FirstByte = FlashBase + FLASH_REGION_TESTMENU_OFFSET;
      PfatConfig->Ppdt.SfamData[0].LastByte  = FlashBase + FLASH_REGION_TESTMENU_OFFSET + FLASH_REGION_TESTMENU_SIZE - 1;
      PfatConfig->Ppdt.SfamData[1].FirstByte = FlashBase + FLASH_REGION_FVMAIN_OFFSET;
      PfatConfig->Ppdt.SfamData[1].LastByte  = FlashBase + FLASH_REGION_FVMAIN_OFFSET + FLASH_REGION_FVMAIN_SIZE - 1;
      PfatConfig->Ppdt.SfamData[2].FirstByte = FlashBase + FLASH_REGION_MICROCODE_OFFSET;
      PfatConfig->Ppdt.SfamData[2].LastByte  = FlashBase + FLASH_REGION_MICROCODE_OFFSET + FLASH_REGION_MICROCODE_SIZE - 1;
      PfatConfig->Ppdt.SfamData[3].FirstByte = FlashBase + FLASH_REGION_FV_RECOVERY2_OFFSET;
      PfatConfig->Ppdt.SfamData[3].LastByte  = FlashBase + FLASH_REGION_FV_RECOVERY2_OFFSET + FLASH_REGION_FV_RECOVERY2_SIZE - 1;
      PfatConfig->Ppdt.SfamData[4].FirstByte = FlashBase + FLASH_REGION_FV_RECOVERY_OFFSET;
      PfatConfig->Ppdt.SfamData[4].LastByte  = FlashBase + FLASH_REGION_FV_RECOVERY_OFFSET + FLASH_REGION_FV_RECOVERY_SIZE - 1;
      CopyMem (&PfatConfig->Ppdt.PkeySlot0[0], &PpdtPkeySlot0[0], sizeof (PpdtPkeySlot0));
      CopyMem (&PfatConfig->Ppdt.PkeySlot1[0], &PpdtPkeySlot1[0], sizeof (PpdtPkeySlot1));
      CopyMem (&PfatConfig->Ppdt.PkeySlot2[0], &PpdtPkeySlot2[0], sizeof (PpdtPkeySlot2));
      PfatConfig->PfatLog.LastPage       = MAX_PFAT_LOG_PAGE - 1;
      PfatConfig->PfatLog.LoggingOptions = PFAT_LOG_OPT_DEBUG | PFAT_LOG_OPT_FLASH_ERROR | PFAT_LOG_OPT_FLASH_ERASE | PFAT_LOG_OPT_FLASH_WRITE | PFAT_LOG_OPT_BRANCH_TRACE | PFAT_LOG_OPT_STEP_TRACE;
      if (PlatformInfoHob->EcPresent == TRUE) {
        DEBUG ((DEBUG_INFO, "EC is Present\n"));
        PfatConfig->Ppdt.PlatAttr |= EC_PRESENT;
        PfatConfig->Ppdt.EcCmd     = KSC_C_PORT;
        PfatConfig->Ppdt.EcData    = KSC_D_PORT;
        PfatConfig->EcCmdDiscovery    = 0xB0;
        PfatConfig->EcCmdProvisionEav = 0xB1;
        PfatConfig->EcCmdLock         = 0xB2;
        CpuIo  = (**PeiServices).CpuIo;
        Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, &StallPpi);
        ASSERT_EFI_ERROR ( Status);
        Status = SendKscCommand (PeiServices, CpuIo, StallPpi, PfatConfig->EcCmdDiscovery);
        if (Status == EFI_SUCCESS) {
          Status = ReceiveKscData (PeiServices, CpuIo, StallPpi, &EcStatus);
          if (((EcStatus & 0xF8) == 0) && (EcStatus & BIT0)) {
            if (((EcStatus & (BIT2 | BIT1)) == (BIT2 | BIT1)) || (((EcStatus & (BIT2 | BIT1)) == 0))) {
              DEBUG ((DEBUG_INFO, "EC FW supports PFAT\n"));
              PfatConfig->Ppdt.PlatAttr     |= EC_PFAT_PROTECTED;
              PfatConfig->Ppdt.EcCmdGetSvn   = 0xB3;
              PfatConfig->Ppdt.EcCmdOpen     = 0xB4;
              PfatConfig->Ppdt.EcCmdClose    = 0xB5;
              PfatConfig->Ppdt.EcCmdPortTest = 0xB6;
              PfatConfig->PupHeader.EcSvn    = 0x00010000;
            } else {
              DEBUG ((DEBUG_ERROR, "EC FW Error\n"));
            }
          } else {
            DEBUG ((DEBUG_WARN, "EC FW do not support PFAT\n"));
          }
        } else {
          DEBUG ((DEBUG_ERROR, "SendKscCommand(EcCmdDiscovery) Failed\n"));
        }
      }
      PfatConfig->Ppdt.PpdtSize = (sizeof (PPDT) - sizeof (PfatConfig->Ppdt.SfamData) + ((PfatConfig->Ppdt.LastSfam + 1) * sizeof (SFAM_DATA)));
      DataSize = Sha256GetContextSize ();
      Status   = ((*PeiServices)->AllocatePool) (PeiServices, DataSize, &Sha256Context);
      ASSERT_EFI_ERROR (Status);
      DataSize = PfatConfig->Ppdt.PpdtSize;
      Data     = (VOID *) &PfatConfig->Ppdt;
      Sha256Init   (Sha256Context);
      Sha256Update (Sha256Context, Data, DataSize);
      Sha256Final  (Sha256Context, HashValue);
      CopyMem (&PfatConfig->PpdtHash[0], &HashValue[0], 8);
      CopyMem (&PfatConfig->PpdtHash[1], &HashValue[8], 8);
      CopyMem (&PfatConfig->PpdtHash[2], &HashValue[16], 8);
      CopyMem (&PfatConfig->PpdtHash[3], &HashValue[24], 8);
    }

    PowerMgmtConfig->TccActivationOffset  = SetupData->TCCActivationOffset;;
    PowerMgmtConfig->VrCurrentLimit       = SetupData->VrCurrentLimit;
    PowerMgmtConfig->Psi1Threshold        = SetupData->Psi1Threshold;
    PowerMgmtConfig->Psi2Threshold        = SetupData->Psi2Threshold;
    PowerMgmtConfig->Psi3Threshold        = SetupData->Psi3Threshold;
    PowerMgmtConfig->VrCurrentLimitLock   = SetupData->VrCurrentLimitLock;
    PowerMgmtConfig->Xe                   = SetupData->EnableXe;
    PowerMgmtConfig->BootInLfm            = SetupData->BootPState;
    PowerMgmtConfig->VrMiscIoutSlope      = SetupData->VrMiscIoutSlope;
    PowerMgmtConfig->VrMiscIoutOffset     = SetupData->VrMiscIoutOffset;
    PowerMgmtConfig->VrMiscIoutOffsetSign = SetupData->VrMiscIoutOffsetSign;
    PowerMgmtConfig->VrMiscMinVid                = SetupData->VrMiscMinVid;
    PowerMgmtConfig->VrMiscIdleExitRampRate      = SetupData->VrMiscIdleExitRampRate;
    PowerMgmtConfig->VrMiscIdleEntryRampRate     = SetupData->VrMiscIdleEntryRampRate;
    PowerMgmtConfig->VrMiscIdleEntryDecayEnable  = SetupData->VrMiscIdleEntryDecayEnable;
    if (PlatformInfoHob->PlatformType == TypeUlt) {
      PowerMgmtConfig->VrMiscSlowSlewRateConfig  = SetupData->VrMiscSlowSlewRateConfig;
      PowerMgmtConfig->VrMisc2FastRampVoltage    = SetupData->VrMisc2FastRampVoltage;
      PowerMgmtConfig->VrMisc2MinC8Voltage       = SetupData->VrMisc2MinC8Voltage;
      PowerMgmtConfig->VrPSI4enable              = SetupData->VrPSI4enable;
    }
    PowerMgmtConfig->FivrSscEnable    = SetupData->EnableFivrSsc;
    PowerMgmtConfig->FivrSscPercent   = SetupData->FivrSscPercent;

    CpuConfig->MlcStreamerPrefetcher  = SetupData->HwPrefetcher;
    CpuConfig->MlcSpatialPrefetcher   = SetupData->AclPrefetch;

    EpocConfig->HDEnable = SetupData->EarlyPowerOnHDEnable;
    EpocConfig->FclkFreq = SetupData->EarlyPowerOnFclkFreq;

    //
    // Update Txt Platform Policy
    //
    if (TxtConfig != NULL) {
      CpuPlatformPolicyPpi->CpuConfig->Txt = SetupData->TXT;
      TxtConfig->TxtDprMemorySize = SetupData->DprSize * 0x100000;
      TxtConfig->McuUpdateDataAddr = FLASH_REGION_MICROCODE_BASE +
                                     ((EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FLASH_REGION_MICROCODE_BASE)->HeaderLength +
                                     sizeof (EFI_FFS_FILE_HEADER);
    }
    OcConfig->OcSupport = SetupData->OverclockingSupport;
    //
    //  Initialize Overclocking Config
    //
    DEBUG ((DEBUG_INFO, "(OC) OverclockingSupport = %X\n",SetupData->OverclockingSupport));
    WdtTimeout = gWdtPei->CheckStatus();

    //
    //  Update Overclocking information only if OC Support is enabled
    //  and no WDT timeouts have occured.
    //
    if (SetupData->OverclockingSupport && WdtTimeout == FALSE){
      //
      //  IA Core
      //
      OcConfig->CoreVoltageMode = SetupData->CoreVoltageMode;

      if (OcConfig->CoreVoltageMode == OC_LIB_OFFSET_ADAPTIVE){
        OcConfig->CoreVoltageOverride   = 0;
        OcConfig->CoreExtraTurboVoltage = SetupData->CoreExtraTurboVoltage;
      } else if (OcConfig->CoreVoltageMode == OC_LIB_OFFSET_OVERRIDE){
        OcConfig->CoreVoltageOverride   = SetupData->CoreVoltageOverride;
        OcConfig->CoreExtraTurboVoltage = 0;
      } else {
        OcConfig->CoreVoltageOverride   = 0;
        OcConfig->CoreExtraTurboVoltage = 0;
      }

      if (SetupData->CoreVoltageOffsetPrefix == 1){
        // Offset is negative, need to convert
        OcConfig->CoreVoltageOffset = (INT16)(~SetupData->CoreVoltageOffset +1);
      } else {
        OcConfig->CoreVoltageOffset = SetupData->CoreVoltageOffset;
      }
      OcConfig->CoreMaxOcTurboRatio     = SetupData->CoreMaxOcRatio;

      //
      //  CLR
      //
      OcConfig->ClrVoltageMode = SetupData->ClrVoltageMode;

      if (OcConfig->ClrVoltageMode == OC_LIB_OFFSET_ADAPTIVE){
        OcConfig->ClrVoltageOverride   = 0;
        OcConfig->ClrExtraTurboVoltage = SetupData->ClrExtraTurboVoltage;
      } else if (OcConfig->ClrVoltageMode == OC_LIB_OFFSET_OVERRIDE){
        OcConfig->ClrVoltageOverride   = SetupData->ClrVoltageOverride;
        OcConfig->ClrExtraTurboVoltage = 0;
      } else {
        OcConfig->ClrVoltageOverride   = 0;
        OcConfig->ClrExtraTurboVoltage = 0;
      }

      if (SetupData->ClrVoltageOffsetPrefix == 1){
        // Offset is negative, need to convert
        OcConfig->ClrVoltageOffset = (INT16)(~SetupData->ClrVoltageOffset +1);
      } else {
        OcConfig->ClrVoltageOffset = SetupData->ClrVoltageOffset;
      }
      OcConfig->ClrMaxOcTurboRatio      = SetupData->ClrMaxOcRatio;

      //
      //  Global
      //
      OcConfig->SvidVoltageOverride     = SetupData->SvidVoltageOverride;
      OcConfig->SvidEnable              = SetupData->SvidEnable;
      OcConfig->FivrFaultsEnable        = SetupData->FivrFaultsEnable;
      OcConfig->FivrEfficiencyEnable    = SetupData->FivrEfficiencyEnable;
      OcConfig->OcSupport               = SetupData->OverclockingSupport;
      OcConfig->BitReserved             = 0;
    } else if (WdtTimeout){
      //
      //  Wdt Timeout occured. Do not update voltage settings in the CPU.
      //  Need to keep the power up CPU settings.
      //
      OcConfig->OcSupport = 0;
    }
  }

  return EFI_SUCCESS;
}
