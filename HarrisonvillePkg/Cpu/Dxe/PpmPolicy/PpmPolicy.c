/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PpmPolicy.c
  
Abstract: 
  
  This file is a wrapper for Intel PPM Platform Policy driver.
  Get Setup Value to initilize Intel PPM DXE Platform Policy.

--*/
#include "PpmPolicy.h"
#include <Protocol/MpService.h>
#include <Library/CpuConfigLib.h>
#include <Include/SetupVariable.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "CpuRegs.h"
#define EFI_CPUID_FAMILY                      0x0F00
#define EFI_CPUID_MODEL                       0x00F0
#define EFI_CPUID_STEPPING                    0x000F



EFI_STATUS
EFIAPI
PpmPolicyEntry(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_MP_SERVICES_PROTOCOL *MpService;
  EFI_CPUID_REGISTER        Cpuid01 = { 0, 0, 0, 0};
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  EFI_GUID                  SetupGuid = SYSTEM_CONFIGURATION_GUID;
  UINTN                     VariableSize = sizeof(SYSTEM_CONFIGURATION);
  SYSTEM_CONFIGURATION      SetupVariables;
  UINTN                     CpuCount;    
  UINTN                     NumberOfEnabledCPUs;
  UINT64                    MaxRatio;
  UINT64                    LfmRatio = 0;

  UINT8                     VidNumber = 0;
  UINT16                    VidMaxRatio = 0;
  UINT16                    VidMaxVid = 0;
  UINT16                    StateRatio[MAX_CUSTOM_VID_TABLE_STATES];
  UINT16                    StateVid[MAX_CUSTOM_VID_TABLE_STATES];
  UINT8                     Index;
  UINT32                    Stepping;
  UINT64                    MsrData;
  UINT8                     ProcessorCcxEnable;
  UINT8                     WriteSetUpValueFlag;
  UINT32                    Attributes; // Denverton Aptiov override - EIP#217439
  UINT32                    PowerUnit;
  UINT16                    PowValue;

  Stepping = 0;
  MsrData  = 0;
  WriteSetUpValueFlag = 0;

  for (Index = 0; Index < MAX_CUSTOM_VID_TABLE_STATES; Index ++) {
    StateRatio[Index] = 0;
    StateVid[Index] = 0;
  }

  gBS = SystemTable->BootServices;
  pBS = SystemTable->BootServices;
  pRS = SystemTable->RuntimeServices;

  //
  // Set PPM policy structure to known value
  //
  gBS->SetMem (&mDxePlatformPpmPolicy, sizeof(PPM_PLATFORM_POLICY_PROTOCOL), 0);
  mDxePlatformPpmPolicy.Revision = PPM_PLATFORM_POLICY_PROTOCOL_REVISION_4;

  //
  // Find the MpService Protocol
  //
  Status = pBS->LocateProtocol (&gEfiMpServiceProtocolGuid,
                                NULL, 
                                &MpService
                               );
  ASSERT_EFI_ERROR (Status);

  //
  // Get processor count from MP service.
  //
  Status = MpService->GetNumberOfProcessors (MpService, &CpuCount, &NumberOfEnabledCPUs);
  ASSERT_EFI_ERROR (Status);

  //
  // Store the CPUID for use by SETUP items.
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &Cpuid01.RegEax, &Cpuid01.RegEbx, &Cpuid01.RegEcx, &Cpuid01.RegEdx);
  MaxRatio = ((RShiftU64 (AsmReadMsr64(EFI_MSR_IA32_PLATFORM_ID), 8)) & 0x1F);

  // Denverton Aptiov override Start - EIP#217439
  Status = pRS->GetVariable(
                         L"IntelSetup",
                         &SetupGuid,
                         &Attributes,
                         &VariableSize,
                         &SetupVariables
                         );
  // Denverton Aptiov override End - EIP#217439

  if (!EFI_ERROR(Status)) {
      mDxePlatformPpmPolicy.FunctionEnables.EnableDumpCrashLog= SetupVariables.EnableDumpCrashLog;
      mDxePlatformPpmPolicy.FunctionEnables.EnableGv          = SetupVariables.ProcessorEistEnable;
      mDxePlatformPpmPolicy.FunctionEnables.EnableTurboMode   = SetupVariables.TurboModeEnable;
      mDxePlatformPpmPolicy.FunctionEnables.EnableTm          = SetupVariables.EnableTm1;
      mDxePlatformPpmPolicy.FunctionEnables.EnableProcHot     = ICH_DEVICE_ENABLE;
      mDxePlatformPpmPolicy.FunctionEnables.TStatesEnable     = SetupVariables.TStateEnable;
      //mDxePlatformPpmPolicy.FunctionEnables.Tm2ThrottlingType = SetupVariables.Tm2ThrottlingType;
 	    mDxePlatformPpmPolicy.FunctionEnables.EnableEmttm       = ICH_DEVICE_DISABLE;
      mDxePlatformPpmPolicy.FunctionEnables.EnableCMP   	    = ICH_DEVICE_ENABLE;
      mDxePlatformPpmPolicy.BiosRequestFreq                   = SetupVariables.BiosRequestFreq;
      mDxePlatformPpmPolicy.MfgMode                           = SetupVariables.MfgMode;
      mDxePlatformPpmPolicy.MaxCState                         = SetupVariables.MaxCState;
      mDxePlatformPpmPolicy.L1Prefetcher                      = SetupVariables.L1Prefetcher;
      mDxePlatformPpmPolicy.L2Prefetcher                      = SetupVariables.L2Prefetcher;
      mDxePlatformPpmPolicy.PkgPowerSkuUnit                   = SetupVariables.PkgPowerSkuUnit;
      mDxePlatformPpmPolicy.PackageRaplTimeWindow1            = SetupVariables.PackageRaplTimeWindow1;
      mDxePlatformPpmPolicy.PackageRaplPowerLevel1            = SetupVariables.PackageRaplPowerLevel1;
      mDxePlatformPpmPolicy.PackageRaplPowerLevel2            = SetupVariables.PackageRaplPowerLevel2;
      mDxePlatformPpmPolicy.RatioLimitsEnable                 = SetupVariables.RatioLimitsEnable;
      mDxePlatformPpmPolicy.RatioLimit0                       = SetupVariables.RatioLimit0;
      mDxePlatformPpmPolicy.RatioLimit1                       = SetupVariables.RatioLimit1;
      mDxePlatformPpmPolicy.RatioLimit2                       = SetupVariables.RatioLimit2;
      mDxePlatformPpmPolicy.RatioLimit3                       = SetupVariables.RatioLimit3;
      mDxePlatformPpmPolicy.RatioLimit4                       = SetupVariables.RatioLimit4;
      mDxePlatformPpmPolicy.RatioLimit5                       = SetupVariables.RatioLimit5;
      mDxePlatformPpmPolicy.RatioLimit6                       = SetupVariables.RatioLimit6;
      mDxePlatformPpmPolicy.RatioLimit7                       = SetupVariables.RatioLimit7;
      mDxePlatformPpmPolicy.CoreCount0                        = SetupVariables.CoreCount0;
      mDxePlatformPpmPolicy.CoreCount1                        = SetupVariables.CoreCount1;
      mDxePlatformPpmPolicy.CoreCount2                        = SetupVariables.CoreCount2;
      mDxePlatformPpmPolicy.CoreCount3                        = SetupVariables.CoreCount3;
      mDxePlatformPpmPolicy.CoreCount4                        = SetupVariables.CoreCount4;
      mDxePlatformPpmPolicy.CoreCount5                        = SetupVariables.CoreCount5;
      mDxePlatformPpmPolicy.CoreCount6                        = SetupVariables.CoreCount6;
      mDxePlatformPpmPolicy.CoreCount7                        = SetupVariables.CoreCount7;
      mDxePlatformPpmPolicy.PkgRaplLimitLock                  = SetupVariables.PkgRaplLimitLock;
      mDxePlatformPpmPolicy.TjTargetOffset                    = SetupVariables.TjTargetOffset;
      mDxePlatformPpmPolicy.TcontrolOffset                    = SetupVariables.TcontrolOffset;
      mDxePlatformPpmPolicy.TcontrolOffsetSign                = SetupVariables.TcontrolOffsetSign;
      mDxePlatformPpmPolicy.HighTempInterrupt                 = SetupVariables.HighTempInterrupt;
      mDxePlatformPpmPolicy.LowTempInterrupt                  = SetupVariables.LowTempInterrupt;
      mDxePlatformPpmPolicy.ProcHotInterruptEnable            = SetupVariables.ProcHotInterruptEnable;
      mDxePlatformPpmPolicy.OutOfSpecInterruptEnable          = SetupVariables.OutOfSpecInterruptEnable;
      mDxePlatformPpmPolicy.TempThreshold1                    = SetupVariables.TempThreshold1;
      mDxePlatformPpmPolicy.Threshold1InterruptEnable         = SetupVariables.Threshold1InterruptEnable;
      mDxePlatformPpmPolicy.TempThreshold2                    = SetupVariables.TempThreshold2;
      mDxePlatformPpmPolicy.Threshold2InterruptEnable         = SetupVariables.Threshold2InterruptEnable;
      mDxePlatformPpmPolicy.ProchotResponse                   = SetupVariables.ProchotResponse;
      mDxePlatformPpmPolicy.ProchotOutputMode                 = SetupVariables.ProchotOutputMode;
      mDxePlatformPpmPolicy.VrThermAlertDisable               = SetupVariables.VrThermAlertDisable;
      mDxePlatformPpmPolicy.LockThermInt                      = SetupVariables.LockThermInt;
      mDxePlatformPpmPolicy.ProchotFreqResponse               = SetupVariables.ProchotFreqResponse;
      mDxePlatformPpmPolicy.PL3ControlPowerLevel              = SetupVariables.PL3ControlPowerLevel;
      mDxePlatformPpmPolicy.PL3ControlEnable                  = SetupVariables.PL3ControlEnable;
      mDxePlatformPpmPolicy.PL3ControlTimeWindow              = SetupVariables.PL3ControlTimeWindow;
      mDxePlatformPpmPolicy.PL3ControlDutyCycle               = SetupVariables.PL3ControlDutyCycle;
      mDxePlatformPpmPolicy.PL3ControlPmaxPower               = SetupVariables.PL3ControlPmaxPower;
      mDxePlatformPpmPolicy.PL3ControlLock                    = SetupVariables.PL3ControlLock;
      mDxePlatformPpmPolicy.CoreBGF                           = SetupVariables.CoreBGF;
      //
      // C-states enabling => go with the Setup setting.
      //
        ProcessorCcxEnable = SetupVariables.ProcessorCcxEnable;

	  if(ProcessorCcxEnable){
	    mDxePlatformPpmPolicy.FunctionEnables.EnableCx       = ICH_DEVICE_ENABLE;
//	  	mDxePlatformPpmPolicy.FunctionEnables.MwaitEn		 = SetupVariables.MonitorMwaitEnable;
      	mDxePlatformPpmPolicy.FunctionEnables.EnableCxe      = SetupVariables.ProcessorC1eEnable;
		mDxePlatformPpmPolicy.PkgCStateLimit                 = SetupVariables.PackageCState;

	  }


//    DEBUG ((EFI_D_INFO, "\t"__FUNCTION__" : Determining Boot Ratio\n"));

      //
      // Determine if Flex Ratio is supported by Si and enabled in Setup.
      // Assume we will use the as manufacture max non-turbo ratio (GuarRatio).
      //
      mDxePlatformPpmPolicy.FlexRatioVid = (UINT32) (MaxRatio << 8);
      
      //
      // Check if GV3 is enabled
      //
      if (Cpuid01.RegEcx & BIT7) {
        //
        // Determine if the IA Core is Ratio Locked, if it is, then we cannot use FLEX
        if (!(AsmReadMsr64(EFI_MSR_IA32_PLATFORM_ID) & B_EFI_MSR_IA32_PLATFORM_ID_RATIO_LOCKED)) {
          //
          // Make sure the user selected Ratio is between GuarRatio and LfmRatio.
          //
          if (SetupVariables.ProcessorFlexibleRatioOverrideEnable){
            if ((SetupVariables.ProcessorFlexibleRatio < MaxRatio) &&
                (SetupVariables.ProcessorFlexibleRatio >= LfmRatio))
              //
              // Use the Ratio the user specified in Setup.
              //
              mDxePlatformPpmPolicy.FlexRatioVid = (UINT32) ((SetupVariables.ProcessorFlexibleRatio) << 8) | BIT16;
          }
//          AsmWriteMsr64(EFI_MSR_CLOCK_FLEX_MAX, (UINT64)mDxePlatformPpmPolicy.FlexRatioVid);
        }
      }
      DEBUG ((EFI_D_ERROR, "\t"__FUNCTION__" : FlexRatioVid -> %8X\n", mDxePlatformPpmPolicy.FlexRatioVid));
      DEBUG ((EFI_D_ERROR, "\t"__FUNCTION__" : Setup.FlexRatio -> %8X\n", (UINT32) SetupVariables.ProcessorFlexibleRatio));

      ///
      /// Auto: Power Sku Unit
      ///
      if (SetupVariables.PkgPowerSkuUnit == 0xffffffff || SetupVariables.MfgMode == 1) {
        SetupVariables.PkgPowerSkuUnit    =  AsmReadMsr32 (EFI_MSR_PKG_POWER_SKU_UNIT);
        mDxePlatformPpmPolicy.PkgPowerSkuUnit = SetupVariables.PkgPowerSkuUnit;
        DEBUG ((EFI_D_INFO, "SetupVariables.PkgPowerSkuUnit = 0x%x\n", SetupVariables.PkgPowerSkuUnit));
        WriteSetUpValueFlag = TRUE;
      }

      if (SetupVariables.PackageRaplTimeWindow1 == 0xffffffff || SetupVariables.MfgMode == 1) {
        PowerUnit = (UINT32)(SetupVariables.PkgPowerSkuUnit & B_EFI_MSR_PKG_POWER_SKU_UNIT_POWER_UNIT);
        PowValue = 2;
        Index = 0;
        do{
          Index++;
          PowValue *= 2;
        } while (Index < PowerUnit-1);
        MsrData = AsmReadMsr64 (EFI_MSR_PKG_RAPL_LIMIT);
        SetupVariables.PackageRaplTimeWindow1 = (UINT32) (MsrData >>  17) & PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME_MASK;
        mDxePlatformPpmPolicy.PackageRaplTimeWindow1 = SetupVariables.PackageRaplTimeWindow1;
        DEBUG ((EFI_D_INFO, "SetupVariables.PackageRaplTimeWindow1 = 0x%x\n", SetupVariables.PackageRaplTimeWindow1));
        SetupVariables.PackageRaplPowerLevel1 = (UINT32) ((MsrData >>  0) & PKG_RAPL_LIMIT_PKG_PWR_LIM1_MASK)/PowValue;
        mDxePlatformPpmPolicy.PackageRaplPowerLevel1 = SetupVariables.PackageRaplPowerLevel1;
        DEBUG ((EFI_D_INFO, "SetupVariables.PackageRaplPowerLevel1 = %d\n", SetupVariables.PackageRaplPowerLevel1));
        SetupVariables.PackageRaplPowerLevel2 = (UINT32) ((MsrData >> 32) & PKG_RAPL_LIMIT_PKG_PWR_LIM2_MASK)/PowValue;
        mDxePlatformPpmPolicy.PackageRaplPowerLevel2 = SetupVariables.PackageRaplPowerLevel2;
        DEBUG ((EFI_D_INFO, "SetupVariables.PackageRaplPowerLevel2 = %d\n", SetupVariables.PackageRaplPowerLevel2));
        WriteSetUpValueFlag = TRUE;
      }

      if (WriteSetUpValueFlag) {
        // Denverton Aptiov override Start - EIP#217439
        Status = pRS->SetVariable(
                         L"IntelSetup",
                         &SetupGuid,
                         Attributes,
                         VariableSize,
                         &SetupVariables
                         );
        // Denverton Aptiov override End - EIP#217439
      }
    }

  
//    if (SetupVariables.ActiveProcessorCores == 1) // single core
//    {
//      mDxePlatformPpmPolicy.FunctionEnables.EnableCMP      = ICH_DEVICE_ENABLE;
//      mDxePlatformPpmPolicy.FunctionEnables.EnableCMP      = ICH_DEVICE_DISABLE;
//    }
//    else
//    {
//      mDxePlatformPpmPolicy.FunctionEnables.EnableCMP      = ICH_DEVICE_DISABLE;
//      mDxePlatformPpmPolicy.FunctionEnables.EnableCMP      = ICH_DEVICE_ENABLE;
//    }

    else {
    	mDxePlatformPpmPolicy.FunctionEnables.EnableGv       = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableCx       = ICH_DEVICE_ENABLE;
//		mDxePlatformPpmPolicy.FunctionEnables.MwaitEn		 = ICH_DEVICE_DISABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableCxe      = ICH_DEVICE_ENABLE;
//		mDxePlatformPpmPolicy.FunctionEnables.EnableC1       = ICH_DEVICE_ENABLE;
//    	mDxePlatformPpmPolicy.FunctionEnables.EnableC2       = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableC3       = ICH_DEVICE_ENABLE;
//    	mDxePlatformPpmPolicy.FunctionEnables.EnableC4       = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableC6       = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableC7       = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableTm       = ICH_DEVICE_ENABLE;
    	//mDxePlatformPpmPolicy.FunctionEnables.EnableTm2      = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableEmttm    = ICH_DEVICE_ENABLE;
        mDxePlatformPpmPolicy.FunctionEnables.EnableProcHot  = ICH_DEVICE_ENABLE;
//    	mDxePlatformPpmPolicy.FunctionEnables.HTD            = ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableCMP      = ICH_DEVICE_DISABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.TStatesEnable  = ICH_DEVICE_DISABLE;
//    	mDxePlatformPpmPolicy.FunctionEnables.EnableDynamicFsb=ICH_DEVICE_ENABLE;
    	mDxePlatformPpmPolicy.FunctionEnables.EnableTurboMode= ICH_DEVICE_ENABLE;
   
  	}
  

  //
  // Set boot performance mode based on setup option. If the ratio is greater than
  // an arbitrary max, boot in LFM so user can set up custom VID table.
  //
//  if ((SetupVariables.BootPState == 0) && (MaxRatio < 0x1E)) {
//    mDxePlatformPpmPolicy.BootInLfm = 0;
//    mDxePlatformPpmPolicy.FlexRatioVid = 0;
//  } else {
//    mDxePlatformPpmPolicy.BootInLfm = 1;
//    mDxePlatformPpmPolicy.FlexRatioVid = 0;
//  }

  mDxePlatformPpmPolicy.PowerStateSwitchSmiNumber                     = POWER_STATE_SWITCH_SMI;
  mDxePlatformPpmPolicy.EnableCStateIoRedirectionSmiNumber            = ENABLE_C_STATE_IO_REDIRECTION_SMI;
  mDxePlatformPpmPolicy.DisableCStateIoRedirectionSmiNumber           = DISABLE_C_STATE_IO_REDIRECTION_SMI;
  mDxePlatformPpmPolicy.EnablePStateHardwareCoordinationSmiNumber     = ENABLE_P_STATE_HARDWARE_COORDINATION_SMI;
  mDxePlatformPpmPolicy.DisablePStateHardwareCoordinationSmiNumber    = DISABLE_P_STATE_HARDWARE_COORDINATION_SMI;
  mDxePlatformPpmPolicy.S3RestoreMsrSwSmiNumber                       = S3_RESTORE_MSR_SW_SMI;
  mDxePlatformPpmPolicy.EnableEnableC6ResidencySmiNumber              = ENABLE_C6_RESIDENCY_SMI;

  mDxePlatformPpmPolicy.CustomVidTable.VidNumber           = VidNumber;
  mDxePlatformPpmPolicy.CustomVidTable.VidMaxVid           = VidMaxVid;
  mDxePlatformPpmPolicy.CustomVidTable.VidCpuid            = (UINT16) (Cpuid01.RegEax & (EFI_CPUID_FAMILY | EFI_CPUID_MODEL | EFI_CPUID_STEPPING));
  mDxePlatformPpmPolicy.CustomVidTable.VidMaxRatio         = VidMaxRatio;
  gBS->CopyMem (mDxePlatformPpmPolicy.CustomVidTable.StateVid, StateVid, sizeof(UINT16) * MAX_CUSTOM_VID_TABLE_STATES);
  gBS->CopyMem (mDxePlatformPpmPolicy.CustomVidTable.StateRatio, StateRatio, sizeof(UINT16) * MAX_CUSTOM_VID_TABLE_STATES);

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                                                  &Handle,
                                                  &gPpmPlatformPolicyProtocolGuid,
                                                  &mDxePlatformPpmPolicy,
                                                  NULL
                                                  );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
