/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/*++

Copyright (c)  2011 - 2016 Intel Corporation. All rights reserved

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.

Module Name:
  PlatformCpuPolicy.c

Abstract: 
  EPG specific CPU Policy Platform Driver. Formely known as PlatformMicrocode.

--*/

#include <PlatformCpuPolicy.h>


CHAR16 mCpuSocketStr[4][5] = {L"CPU0", L"CPU1", L"CPU2", L"CPU3"};
CHAR16 mCpuAssetTagStr[] = L"UNKNOWN";


PLATFORM_CPU_POLICY_INSTANCE  mPlatformCpuPrivateData;


/**
  Set platform CPU data that related to SMBIOS.
**/
VOID
PlatformCpuSmbiosData (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_PLATFORM_TYPE_PROTOCOL    *PlatformType;
  UINT32                        CpuSocketCount;
  UINTN                         Index;
  CHAR16                        **CpuSocketNames;
  CHAR16                        **CpuAssetTags;

  Status = gBS->LocateProtocol (
                  &gEfiPlatformTypeProtocolGuid,
                  NULL,
                  &PlatformType
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Set the count of CPU sockets on the board.
  //
  CpuSocketCount = 1;

  PcdSet32S (PcdPlatformCpuSocketCount, CpuSocketCount);

  CpuSocketNames = AllocatePool (CpuSocketCount * sizeof (UINTN));
  ASSERT_EFI_ERROR ( CpuSocketNames != NULL );

  CpuAssetTags = AllocatePool (CpuSocketCount * sizeof (UINTN));
  ASSERT_EFI_ERROR ( CpuAssetTags != NULL );

  for (Index = 0; Index < CpuSocketCount; Index++) {
    CpuSocketNames[Index] = mCpuSocketStr[Index];
    CpuAssetTags[Index] = mCpuAssetTagStr;
  }

  PcdSet64S (PcdPlatformCpuSocketNames, (UINT64) (UINTN) CpuSocketNames);
  PcdSet64S (PcdPlatformCpuAssetTags, (UINT64) (UINTN) CpuAssetTags);
}

EFI_STATUS
PlatformCpuPolicyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
  
Routine Description:

  This is the EFI driver entry point for the CpuPolicy Driver. This
  driver is responsible for getting microcode patches from FV.

Arguments:

  ImageHandle     - Handle for the image of this driver.
  SystemTable     - Pointer to the EFI System Table.

Returns:

  EFI_SUCCESS     - Protocol installed sucessfully.

--*/
{
  EFI_STATUS                       Status;
  UINTN                            SysCfgSize;
  SYSTEM_CONFIGURATION             SysCfg;
  UINT32                           CpuPolicy;
  UINT32                           CpuPolicyEx1;
  EFI_HANDLE                       Handle;
  UINT8                            ProcessorEistEnable;
  UINT8                            ProcessorCcxEnable;
  UINT8                            TurboModeEnable;
  UINT32                           GenPmConA;
  UINT32                           GenPmConB;

  PlatformCpuSmbiosData ();

  //
  // Read the current system configuration variable store.
  //
  SysCfgSize = sizeof (SysCfg);
  // Denverton Aptiov override Start - EIP#217439
  Status = gRT->GetVariable (
                  L"IntelSetup",
                  &gSetupVariableGuid,
                  NULL, // Attributes
                  &SysCfgSize,
                  &SysCfg
                  );
  // Denverton Aptiov override Start - EIP#217439
  
  if (EFI_ERROR(Status)) {
    //
    // Safe default settings.
    //
    CpuPolicy = PCD_CPU_HT_BIT |
                PCD_CPU_C1E_BIT |
                PCD_CPU_VT_BIT |
                PCD_CPU_EXECUTE_DISABLE_BIT |
                PCD_CPU_MACHINE_CHECK_BIT |
                PCD_CPU_DCU_PREFETCHER_BIT |
                PCD_CPU_IP_PREFETCHER_BIT |
                PCD_CPU_MONITOR_MWAIT_BIT;

    CpuPolicyEx1 = PCD_CPU_MLC_STREAMER_PREFETCHER_BIT |
                   PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT;

    PcdSet8S (PcdCpuDcuMode, 0);
      

    mPlatformCpuPrivateData.PlatformCpu.HtState       = TRUE;
    mPlatformCpuPrivateData.PlatformCpu.Gv3State      = FALSE;
    mPlatformCpuPrivateData.PlatformCpu.PsdState      = 0; //HW_ALL    
    mPlatformCpuPrivateData.PlatformCpu.CcxEnable     = FALSE;
    mPlatformCpuPrivateData.PlatformCpu.PackageCState = 6;
    mPlatformCpuPrivateData.PlatformCpu.C1eEnable     = TRUE;
    mPlatformCpuPrivateData.PlatformCpu.VtEnable      = TRUE;
    mPlatformCpuPrivateData.PlatformCpu.TurboModeEnable     = FALSE;
    mPlatformCpuPrivateData.PlatformCpu.ExecuteDisableBit   = TRUE;
    mPlatformCpuPrivateData.PlatformCpu.MachineCheckEnable  = TRUE;
    mPlatformCpuPrivateData.PlatformCpu.MonitorMwaitEnable  = TRUE; 
    mPlatformCpuPrivateData.PlatformCpu.DcaState              = FALSE;
    mPlatformCpuPrivateData.PlatformCpu.DcaPrefetchDelayValue = 4;
    mPlatformCpuPrivateData.PlatformCpu.BspSelection          = 0;    
    mPlatformCpuPrivateData.PlatformCpu.XapicEnable           = 0;                          //Disabled

    mPlatformCpuPrivateData.PlatformCpu.MLCStreamerPrefetcherEnable = 1;
    mPlatformCpuPrivateData.PlatformCpu.MLCSpatialPrefetcherEnable  = 1;
    mPlatformCpuPrivateData.PlatformCpu.DCUStreamerPrefetcherEnable = 1;
    mPlatformCpuPrivateData.PlatformCpu.DCUIPPrefetcherEnable       = 1;
    mPlatformCpuPrivateData.PlatformCpu.DCUModeSelection            = 0;
    mPlatformCpuPrivateData.PlatformCpu.AesEnable                   = 0;
    mPlatformCpuPrivateData.PlatformCpu.TStateEnable                = 0;
    mPlatformCpuPrivateData.PlatformCpu.OnDieThermalThrottling      = 0;						// default 50% 

  } else {
    if (SysCfg.ProcessorEistEnable == 2) {
      ProcessorEistEnable = 1;
    } else {
      ProcessorEistEnable = SysCfg.ProcessorEistEnable;
    }

    //
    // If C-states are set to Auto mode then configure B0 for enabled and all others disabled,
    // otherwise go with the Setup setting.
    //
    if (SysCfg.ProcessorCcxEnable == 2) {
      ProcessorCcxEnable = 1;
    } else {
      ProcessorCcxEnable = SysCfg.ProcessorCcxEnable;
    }

    //
    // TurboMode option only applies 
    //
    TurboModeEnable = SysCfg.TurboModeEnable;

    //
    // Map CPU setup options to PcdCpuProcessorFeatureUserConfiguration
    //
    CpuPolicy = (SysCfg.ProcessorHyperThreadingDisable ? 0 : PCD_CPU_HT_BIT) |
                (ProcessorEistEnable ? PCD_CPU_EIST_BIT : 0) |
                ((SysCfg.ProcessorC1eEnable && ProcessorEistEnable && ProcessorCcxEnable) ? PCD_CPU_C1E_BIT : 0) |
                (SysCfg.ProcessorVmxEnable ? PCD_CPU_VT_BIT : 0) |
                (SysCfg.ProcessorSmxEnable ? PCD_CPU_LT_BIT : 0) |
                (SysCfg.FastStringEnable ? PCD_CPU_FAST_STRING_BIT : 0) |
                (SysCfg.ExecuteDisableBit ? PCD_CPU_EXECUTE_DISABLE_BIT : 0) |
                (SysCfg.MachineCheckEnable ? PCD_CPU_MACHINE_CHECK_BIT : 0) |
                (SysCfg.DCUStreamerPrefetcherEnable ? PCD_CPU_DCU_PREFETCHER_BIT : 0) |
                (SysCfg.DCUIPPrefetcherEnable ? PCD_CPU_IP_PREFETCHER_BIT : 0) |
                (SysCfg.MonitorMwaitEnable ? PCD_CPU_MONITOR_MWAIT_BIT : 0) |
                (SysCfg.TStateEnable ? PCD_CPU_TSTATE_BIT : 0) |
                (TurboModeEnable ? PCD_CPU_TURBO_MODE_BIT : 0);
				// (SysCfg.CpuidMaxValue ? PCD_CPU_MAX_CPUID_VALUE_LIMIT_BIT : 0);

    //If Interruprt Remapping is disabled, XAPIC mode should be disabled as well
// Denverton AptioV Override Start - EIP#381821	
// X2APIC is tied to VTd implementation (OS will only support X2APIC if VTd is also supported).
    if(!SysCfg.InterruptRemappingSupport || !SysCfg.VtdEnable) {
    	SysCfg.ProcessorXapic = 0;
    }
// Denverton AptioV Override End - EIP#381821

    CpuPolicyEx1 = (ProcessorCcxEnable ? PCD_CPU_C_STATE_BIT : 0) |
                   (SysCfg.C1AutoDemotion ? PCD_CPU_C1_AUTO_DEMOTION_BIT : 0) |
                   (SysCfg.C3AutoDemotion ? PCD_CPU_C3_AUTO_DEMOTION_BIT : 0) |
                   (SysCfg.MlcStreamerPrefetcherEnable ? PCD_CPU_MLC_STREAMER_PREFETCHER_BIT : 0) |
                   (SysCfg.MlcSpatialPrefetcherEnable ? PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT : 0) |
                   (SysCfg.ThreeStrikeTimer ? PCD_CPU_THREE_STRIKE_COUNTER_BIT : 0) |
                   PCD_CPU_ENERGY_PERFORMANCE_BIAS_BIT |
                   (SysCfg.DcaEnable ? PCD_CPU_DCA_BIT : 0) |
                   (SysCfg.ProcessorXapic ? PCD_CPU_X2APIC_BIT : 0) |
                   (SysCfg.AesEnable ? PCD_CPU_AES_BIT : 0) ;

    PcdSet8S (PcdCpuEnergyPolicy, EPB_DEFAULT);
    CpuPolicyEx1 |= PCD_CPU_ENABLE_IA_UNTRUSTED_MODE_BIT;

    if (ProcessorCcxEnable) {
      PcdSet16S (PcdCpuAcpiLvl2Addr, PM_BASE_ADDRESS + 0x14);
      PcdSet8S (PcdCpuPackageCStateLimit, SysCfg.PackageCState);
    }
    if (SysCfg.TStateEnable) {
      PcdSet8S (PcdCpuClockModulationDutyCycle, SysCfg.OnDieThermalThrottling);
    }
    PcdSet8S (PcdCpuDcuMode, SysCfg.DCUModeSelection);
  

    mPlatformCpuPrivateData.PlatformCpu.HtState        = (BOOLEAN) SysCfg.ProcessorHyperThreadingDisable ? 0 : 1;
    mPlatformCpuPrivateData.PlatformCpu.Gv3State       = (BOOLEAN) ProcessorEistEnable;
    mPlatformCpuPrivateData.PlatformCpu.CcxEnable      = (BOOLEAN) ProcessorCcxEnable;
    mPlatformCpuPrivateData.PlatformCpu.C1AutoDemotion = SysCfg.C1AutoDemotion;
    mPlatformCpuPrivateData.PlatformCpu.C3AutoDemotion = SysCfg.C3AutoDemotion;
    mPlatformCpuPrivateData.PlatformCpu.PackageCState  = SysCfg.PackageCState;

    mPlatformCpuPrivateData.PlatformCpu.C1eEnable     = (BOOLEAN) SysCfg.ProcessorC1eEnable;
    mPlatformCpuPrivateData.PlatformCpu.VtEnable      = (BOOLEAN) SysCfg.ProcessorVmxEnable;
    mPlatformCpuPrivateData.PlatformCpu.LtEnable      = (BOOLEAN) SysCfg.ProcessorSmxEnable;
    mPlatformCpuPrivateData.PlatformCpu.ThreeStrikeTimer      = (BOOLEAN) SysCfg.ThreeStrikeTimer;
    mPlatformCpuPrivateData.PlatformCpu.TurboModeEnable     = (BOOLEAN) TurboModeEnable;
    mPlatformCpuPrivateData.PlatformCpu.ExecuteDisableBit   = (BOOLEAN) SysCfg.ExecuteDisableBit;
    mPlatformCpuPrivateData.PlatformCpu.MachineCheckEnable  = (BOOLEAN) SysCfg.MachineCheckEnable;
    mPlatformCpuPrivateData.PlatformCpu.MonitorMwaitEnable  = (BOOLEAN) SysCfg.MonitorMwaitEnable; 
    mPlatformCpuPrivateData.PlatformCpu.BspSelection          = SysCfg.BspSelection;
//  mPlatformCpuPrivateData.PlatformCpu.VmxState          = PlatformCpuGetVmxState ();
    mPlatformCpuPrivateData.PlatformCpu.XapicEnable           = SysCfg.ProcessorXapic; 

    mPlatformCpuPrivateData.PlatformCpu.DcaState              = SysCfg.DcaEnable;
    mPlatformCpuPrivateData.PlatformCpu.DcaPrefetchDelayValue = SysCfg.DcaPrefetchDelayValue;
    
    mPlatformCpuPrivateData.PlatformCpu.MLCStreamerPrefetcherEnable = SysCfg.MlcStreamerPrefetcherEnable;
    mPlatformCpuPrivateData.PlatformCpu.MLCSpatialPrefetcherEnable  = SysCfg.MlcSpatialPrefetcherEnable;
    mPlatformCpuPrivateData.PlatformCpu.DCUStreamerPrefetcherEnable = SysCfg.DCUStreamerPrefetcherEnable;
    mPlatformCpuPrivateData.PlatformCpu.DCUIPPrefetcherEnable       = SysCfg.DCUIPPrefetcherEnable;
    mPlatformCpuPrivateData.PlatformCpu.DCUModeSelection            = SysCfg.DCUModeSelection;
    mPlatformCpuPrivateData.PlatformCpu.iTurboModeValue             = SysCfg.iTurboModeValue;
    mPlatformCpuPrivateData.PlatformCpu.AesEnable                   = SysCfg.AesEnable;
    mPlatformCpuPrivateData.PlatformCpu.TStateEnable                = SysCfg.TStateEnable;
    mPlatformCpuPrivateData.PlatformCpu.OnDieThermalThrottling      = SysCfg.OnDieThermalThrottling;						

  }

  CpuPolicy |= PCD_CPU_L3_CACHE_BIT;


  //
  //  mPlatformCpuPrivateData.PlatformCpu.XdState         	 = 0;
  //  mPlatformCpuPrivateData.PlatformCpu.IEDEnable          = 0;
  //
  mPlatformCpuPrivateData.PlatformCpu.EnableL3Cache       = TRUE;
  mPlatformCpuPrivateData.PlatformCpu.VirtualWireMode     = 0;                          //Virtual wire to A

  //mPlatformCpuPrivateData.PlatformCpu.ChipsetAcpiLv2Addr          = PCH_ACPI_BASE_ADDRESS + R_ACPI_LV2;
  //mPlatformCpuPrivateData.PlatformCpu.BspUncoreBusNum             = PlatformInfo->CpuData.UncoreBusNum[0];

  PcdSet32S (PcdCpuProcessorFeatureUserConfiguration, CpuPolicy);
  PcdSet32S (PcdCpuProcessorFeatureUserConfigurationEx1, CpuPolicyEx1);

  //
  //Check SUS_PWR_FLR bit in GEN_PMCON_B or GBL_RST_STS (Full Reset)bit in GEN_PMCON_A
  //register to check power on reset status.
  //
  GenPmConA = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_A);
  GenPmConB = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B);

  if ((GenPmConB & B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR) || (GenPmConA & B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS)) {
    PcdSetBoolS (PcdIsPowerOnReset, TRUE);
  }

  //
  // Install the protcol
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiPlatformCpuProtocolGuid,
                  &mPlatformCpuPrivateData.PlatformCpu,
                  &gIntelCpuPcdsSetDoneProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

