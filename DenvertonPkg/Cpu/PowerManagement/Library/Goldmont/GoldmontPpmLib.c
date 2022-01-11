/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  GoldmontPpmLib.c
    
Abstract:

  This library contains power management configuration functions for 
  Denverton processors.

  Acronyms:
    PPM   Processor Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology

--*/ 

//
// Include files
//
#include "GoldmontPpmLib.h"
#include "PpmCommon.h"
#include "CpuPpmLib.h"
#include "CpuRegs.h"
#include "Library/PciLib.h"
#include <Protocol/PpmPlatformPolicy.h>
#include "..\..\PpmDxe\Ppm.h"
#include "SimicsDebug.h"

//
// processor Stepping
//
STATIC UINT16                    mProcessorStepping;
STATIC UINT8                     mTurboModeNotAvailable = 0;

//
// Power management ACPI base address
//
UINT16                           mAcpiBaseAddr = 0;

//
// Only one instance of this protocol currently supported.
//
PPM_PROCESSOR_SUPPORT_PROTOCOL_2 mPpmProcessorSupportProtocolDenverton = {
  0,                                        // PpmFlags
  0,                                        // PpmFeatureFlags
  SetPpmFlags,
  EnableCStates,
  InitEist,
  ConfigureFreqLimit,
  ConfigureRaplLimit,
  InitThermal,
  EnableTm,
  EnableProcHot,
  EnableTsc,
  NULL,
  S3SaveMsr,
  S3RestoreMsr,
  SetBootPState,
  GetCrashLogDump,
  EnableCStateIoRedirection,
  DisableCStateIoRedirection,
  EnablePStateHardwareCoordination,
  DisablePStateHardwareCoordination,
  NULL,
  NULL,                                     // PStateTransition
  NULL                                      //PowerSourceChange
};

//
// CPUID 01 values
//
STATIC EFI_CPUID_REGISTER mCpuid01 = { 0, 0, 0, 0 };

//
// PPM policy configurations
//
extern PPM_PLATFORM_POLICY_PROTOCOL mPpmPlatformPolicyProtocol;

//
// Values for FVID table calculate.
//
STATIC UINT16                     mTurboBusRatio   = 0;
STATIC UINT16                     mTurboBusRatio_3C = 0;
STATIC UINT16                     mMaxBusRatio     = 0;
STATIC UINT16                     mMinBusRatio     = 0;
STATIC UINT16                     mTurboVid        = 0;
STATIC UINT16                     mTurboVid_3C     = 0;
STATIC UINT16                     mMaxVid          = 0;
STATIC UINT16                     mMinVid          = 0;
STATIC UINT16                     mProcessorFlavor = 0;
extern FVID_TABLE                 *mFvidPointer;
//
// Fractional part of Processor Power Unit in Watts. (i.e. Unit is 1/mProcessorPowerUnit)
//
STATIC UINT8                      mProcessorPowerUnit = 0;
//
// Maximum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
//
STATIC UINT16                     mPackageMaxPower = 0;
//
// Minimum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
//
STATIC UINT16                     mPackageMinPower = 0;
//
// Processor TDP value in Watts
//
STATIC UINT16 mPackageTdp         = 0;                        ///< Processor TDP value in MSR_PACKAGE_POWER_SKU.
STATIC UINT16 mPackageTdpWatt     = 0;                        ///< Processor TDP value in Watts.

//
// Porgrammable TDP Limit
//
STATIC UINT8                      mTdpLimitProgrammble = 0;

//
// Table to convert Seconds into equivalent MSR values
// This table is used for TDP Time Window programming
//
STATIC UINT8                      mSecondsToMsrValueMapTable[][2] = {
        // Seconds       MSR Value 
       {   1        ,     0x0 },
       {   5        ,     0x1 },
       {  10        ,     0x2 },
       {  15        ,     0x3 },
       {  20        ,     0x4 },
       {  25        ,     0x5 },
       {  30        ,     0x6 }, 
       {  35        ,     0x7 }, 
       {  40        ,     0x8 },
       {  45        ,     0x9 }, 
       {  50        ,     0xA },
     //{ reserved   ,     0xB-0x7F},
       {END_OF_TABLE,END_OF_TABLE}
};


// MSR table for S3 resume
//
STATIC EFI_MSR_VALUES mMsrValues[] = {
  { MSR_IA32_CLOCK_MODULATION,          0 },  // 0x19A
  { MSR_IA32_PERF_CTL,                  0 },  // 0x199
  { MSR_PM_CFG_CTRL,                    0 },  // 0x0E2
  { MSR_IA32_MISC_ENABLES,              0 },  // 0x1A0
  { MSR_POWER_CTL,                      0 },  // 0x1FC
  { MSR_TURBO_POWER_LIMIT,              0 },  // 0x610
  { MSR_FLEX_RATIO,                     0 },  // 0x194
  { MSR_PRIMARY_PLANE_CURRENT_CONFIG,   0 },  // 0x601
  { MSR_PM_CFG_CTRL,                    0 },
  { MSR_POWER_MISC,                     0 },
  { MSR_CPU_THERM_SENS_CFG,             0 }
};

//
// Function implemenations
//
#ifdef __GNUC__
#pragma GCC push_options
#pragma GCC optimize ("O0")
#else
#pragma optimize("", off)
#endif
EFI_STATUS
InitializePowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  )
/*++

Routine Description:

  Initializes the processor power management library.  This must be called
  prior to any of the library functions being used.

Arguments:
  
  This          Pointer to the PPM support protocol instance

Returns:

  EFI_SUCCESS   Library was initialized successfully

--*/
{
  MSR_REGISTER  PackagePowerSKUUnitMsr;
  MSR_REGISTER  TempMsr;

  MSR_REGISTER  PlatformIdMsr;
  MSR_REGISTER  IaCoreRatios;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Read the CPUID information
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &mCpuid01.RegEax, &mCpuid01.RegEbx, &mCpuid01.RegEcx, &mCpuid01.RegEdx);

  //
  // Get processor Stepping
  //
  mProcessorStepping = (UINT16) (mCpuid01.RegEax & CPUID_STEPPING);
  
  //
  // Assign the protocol pointer.
  //
  *This = &mPpmProcessorSupportProtocolDenverton;

  //
  // Get Platform ID 
  //
  PlatformIdMsr.Qword = AsmReadMsr64 (MSR_IA32_PLATFORM_ID);
  mProcessorFlavor = (UINT8) RShiftU64((PlatformIdMsr.Dwords.High & PLATFORM_ID_BITS_MASK), 18);

  //
  // Get the Bus Ratio for the processor
  //
  //
  // Get Maximum Non-Turbo Limit Ratio (HFM) from PLATFORM_INFO MSR Bits[15:8]
  //
  IaCoreRatios.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  mMaxBusRatio  = IaCoreRatios.Bytes.SecondByte;

  //
  // Get Maximum Efficiency Ratio (LFM) from PLATFORM_INFO MSR Bits[47:40]. Given in units of 100 MHz.
  //
  mMinBusRatio = IaCoreRatios.Bytes.SixthByte;

  //
  // Get Ratio Limits from FREQ_LIMIT_RATIOS (or TURBO_RATIO_LIMIT) MSR Bits [7:0]
  //
  TempMsr.Qword    = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  mTurboBusRatio  = (UINT16)(TempMsr.Bytes.FirstByte & MAX_RATIO_1C_MASK);
  mTurboBusRatio_3C  = (UINT16)(TempMsr.Bytes.ThirdByte & MAX_RATIO_3C_MASK);

  
  //
  // Check setup option to LOCK/UNLOCK MSR 0x610
  //
  TempMsr.Qword = AsmReadMsr64 (EFI_MSR_PKG_RAPL_LIMIT);
  if (mPpmPlatformPolicyProtocol.PkgRaplLimitLock == 1) {
    TempMsr.Qword |= B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM_LOCK;
  } else {
    TempMsr.Qword &= ~B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM_LOCK;
  }
  AsmWriteMsr64 (EFI_MSR_PKG_RAPL_LIMIT, TempMsr.Qword);

  //
  // Get Processor TDP 
  // Get Maximum Power from Turbo Power Limit MSR Bits[14:0]
  // and convert it to units specified by Package Power SKU - Unit MSR [3:0]
  //
  TempMsr.Qword                 = AsmReadMsr64 (MSR_TURBO_POWER_LIMIT);
  PackagePowerSKUUnitMsr.Qword  = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);

  mProcessorPowerUnit           = (PackagePowerSKUUnitMsr.Bytes.FirstByte & PACKAGE_POWER_UNIT_MASK);
  if (mProcessorPowerUnit == 0) {
    mProcessorPowerUnit = 1;
  } else {
    mProcessorPowerUnit = (UINT8) LShiftU64 (1, (mProcessorPowerUnit));
  }
	mPackageTdp = (UINT16)(TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK);
#if 0
 mPackageTdpWatt = (UINT16) DivU64x32(mPackageTdp, mProcessorPowerUnit);
#endif

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}
#ifdef __GNUC__
#pragma GCC pop_options
#else
#pragma optimize("", on)
#endif


STATIC
EFI_STATUS
SetPpmFlags (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Set the PPM flags specific to mobile Valleyview processors

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   PpmFlags updated with the features supported by the processor

--*/
{
  MSR_REGISTER                PlatformId;
  MSR_REGISTER                Ia32MiscEnable;
  EFI_CPUID_REGISTER          Cpuid01 = {0};
  EFI_CPUID_REGISTER          Cpuid05 = {0};
  EFI_CPUID_REGISTER          Cpuid06 = {0};
  EFI_CPUID_REGISTER          Cpuid0B = {0};
  UINTN                       States;
  UINT8                       NumberOfLP = 0;
  UINT8                       ThreadPerCore = 0; 
  UINT32                      Ecx = 0;
  BOOLEAN                     CpuidLimitingEnabled;

  CpuidLimitingEnabled = FALSE;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  DEBUG ((EFI_D_ERROR, "\tSetting PPM Flags specific to processor...\n"));
  //
  // Check if the processor has multiple cores
  //
  Ecx = 0;  // Set the level number to 0 for SMT level of Processor Topology.
  AsmCpuidEx (EFI_CPUID_XAPIC_PROC_TOPOLOGY, Ecx, &Cpuid0B.RegEax, &Cpuid0B.RegEbx, &Cpuid0B.RegEcx, &Cpuid0B.RegEdx);
  ThreadPerCore = (UINT8) Cpuid0B.RegEbx & 0xFF;

  Ecx = 1;  // Set the level number to 1 for Core level of Processor Topology.
  AsmCpuidEx (EFI_CPUID_XAPIC_PROC_TOPOLOGY, Ecx, &Cpuid0B.RegEax, &Cpuid0B.RegEbx, &Cpuid0B.RegEcx, &Cpuid0B.RegEdx);
  NumberOfLP = (UINT8) Cpuid0B.RegEbx & 0xFF;

  if (NumberOfLP > 2 && ThreadPerCore == 1) {
    This->PpmFlags |= (PPM_QUAD | PPM_CMP);
    DEBUG ((EFI_D_INFO, "\tQuad Core detected\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_QUAD | PPM_CMP\n"));
  } else if (NumberOfLP > 1 && ThreadPerCore == 1) {
    This->PpmFlags |= PPM_CMP;
    DEBUG ((EFI_D_INFO, "\tDual Core detected\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_CMP\n"));
  }
  
  //
  // Denverton support TSC updates
  //
  if (mCpuid01.RegEcx & (1 << 4)){
      This->PpmFlags |= PPM_TSC;
    DEBUG ((EFI_D_INFO, "\tTSC updates supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_TSC\n"));
  }  
  
  //
  // Set PROCHOT# always
  //
  This->PpmFlags |= PPM_PROC_HOT;
  DEBUG ((EFI_D_INFO, "\tPROCHOT# Capable\n"));
  DEBUG ((EFI_D_INFO, "\t-->PPM_PROC_HOT\n"));
  
  //
  // Check TM capable
  //
  if (mCpuid01.RegEdx & B_CPUID_VERSION_INFO_EDX_TM1) {
    //This->PpmFlags |= PPM_TM1;
    This->PpmFlags |= PPM_TM;
    DEBUG ((EFI_D_INFO, "\tTM Capable\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_TM\n"));
  }

  //
  // Check GV3/ EIST capable, If EIST capable, also set the boot P-state to HFM flag.
  //
  if (mCpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_EIST) {
    This->PpmFlags |= (PPM_GV3 | PPM_BOOT_P_ST_HFM) ;
    DEBUG ((EFI_D_INFO, "\tGV3 Capable\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_GV3 | PPM_BOOT_P_ST_HFM\n"));
  }

  //
  // Check if anything has been disabled
  //
  PlatformId.Qword = AsmReadMsr64 (EFI_MSR_IA32_PLATFORM_ID);
  
  //
  // Check if EIST has been fuse disabled.
  //
  if (PlatformId.Qword & FB_GV3_DISABLED) {
    This->PpmFlags &= ~(PPM_GV3 | PPM_BOOT_P_ST_HFM );
    DEBUG ((EFI_D_INFO, "\tGV3 fuse disabled."));
    DEBUG ((EFI_D_INFO, "\t\t-->~(PPM_GV3 | PPM_BOOT_P_ST_HFM )\n"));
  }

  //
  // Check if any type of automatic internal throttling has been fuse disabled
  //
  if (PlatformId.Qword & FB_THERMAL_THROT_DISABLED) {
    This->PpmFlags &= ~(PPM_TM);
    DEBUG ((EFI_D_INFO, "\tAutomatic internal throttling fuse disabled.\n"));
    DEBUG ((EFI_D_INFO, "\t\t-->~(PPM_TM)\n"));
  }

  //
  // Disable CPUID limiting (and save current setting) if enabled 
  // and enable MONITOR/MWAIT support
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (EFI_MSR_IA32_MISC_ENABLE);
  if (Ia32MiscEnable.Qword & LIMIT_CPUID) {
    Ia32MiscEnable.Qword &=(UINT64)(~LIMIT_CPUID);
    Ia32MiscEnable.Qword |=(UINT64)(MONITOR_MWAIT_ENABLE);
    AsmWriteMsr64 (EFI_MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);

    CpuidLimitingEnabled = TRUE;
  }

  //
  // Read the CPUID values we care about.  We cannot use the stored 
  // values because they may have changes since we disabled limiting 
  // and enabled MONITOR/MWAIT
  //
  AsmCpuid (1, &Cpuid01.RegEax, &Cpuid01.RegEbx, &Cpuid01.RegEcx, &Cpuid01.RegEdx);
  AsmCpuid (5, &Cpuid05.RegEax, &Cpuid05.RegEbx, &Cpuid05.RegEcx, &Cpuid05.RegEdx);
  AsmCpuid (6, &Cpuid06.RegEax, &Cpuid06.RegEbx, &Cpuid06.RegEcx, &Cpuid06.RegEdx);

  //
  // Determine if the MONITOR/MWAIT instructions are supported.
  //
  if ((Cpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_MWAIT && Cpuid05.RegEcx & B_CPUID_MONITOR_MWAIT_ECX_EXTENSIONS)) {
    This->PpmFlags |= PPM_MWAIT_EXT;
    DEBUG ((EFI_D_INFO, "\tMONITOR/MWAIT instructions are supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_MWAIT_EXT\n"));
  }

  //
  // Determine the C-State and Enhanced C-State support present.
  // Monitor/MWAIT parameters function describes the numbers supported.
  //
  States = (Cpuid05.RegEdx >> 4) & 0xF;
  if (States >= ENHANCED_CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C1 + PPM_C1E;
    DEBUG ((EFI_D_INFO, "\tCstate : C1 + C1E supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C1 + PPM_C1E\n"));
  } else if (States == CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C1;
    DEBUG ((EFI_D_INFO, "\tCstate : C1\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C1 + PPM_C1E\n"));
  }

  States = (Cpuid05.RegEdx >> 8) & 0xF;
  if (States >= ENHANCED_CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C3;
    DEBUG ((EFI_D_INFO, "\tCstate : C3 supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C3\n"));
  }

  States = (Cpuid05.RegEdx >> 12) & 0xF;
  if (States >= ENHANCED_CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C6;
    DEBUG ((EFI_D_INFO, "\tCstate : C6 supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C6\n"));
  }

  States = (Cpuid05.RegEdx >> 16) & 0xF;
  if (States >= ENHANCED_CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C7;
    DEBUG ((EFI_D_INFO, "\tCstate : C7 supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C7\n"));
  } 

	States = (Cpuid05.RegEdx >> 20) & 0xF;
  if (States >= CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C8;
    DEBUG ((EFI_D_INFO, "\tCstate : C8 supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C8\n"));
  }

	States = (Cpuid05.RegEdx >> 24) & 0xF;
  if (States >= CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C9;
    DEBUG ((EFI_D_INFO, "\tCstate : C9 supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C9\n"));
  }

	States = (Cpuid05.RegEdx >> 28) & 0xF;
  if (States >= CSTATE_SUPPORTED) {
    This->PpmFlags |= PPM_C10;
    DEBUG ((EFI_D_INFO, "\tCstate : C10 supported\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_C10\n"));
  }
  ///
  /// Check TimedMwait is supported and update the flag
  ///
  if (AsmReadMsr64 (MSR_PLATFORM_INFO) & B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED) {
    This->PpmFlags |= PPM_TIMED_MWAIT;
  }

  //
  // Check if turbo mode is supported
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  if (((Cpuid06.RegEax & TURBO_MODE_SUPPORTED) == 0) && ((Ia32MiscEnable.Dwords.High & DISABLE_MASTER_TURBO_MODE) == 0)) {
    //
    // Turbo Mode is not available in this physical processor package.
    // BIOS should not attempt to enable Turbo Mode via IA32_MISC_ENABLE MSR.
    // BIOS should show Turbo Mode as Disabled and Not Configurable.
    //
    DEBUG ((EFI_D_INFO, "\tTurbo mode : Not available\n"));
    mTurboModeNotAvailable = 1;

  } else if (((Cpuid06.RegEax & TURBO_MODE_SUPPORTED) == 0) && ((Ia32MiscEnable.Dwords.High & DISABLE_MASTER_TURBO_MODE) != 0)) {
    //
    // Turbo Mode is available but globally disabled for the all logical
    // processors in this processor package.
    // BIOS can enable Turbo Mode by IA32_MISC_ENABLE MSR 1A0h bit [38] = 0.
    //
    This->PpmFlags |= PPM_TURBO;
    DEBUG ((EFI_D_INFO, "\tTurbo mode : Available but disabled\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_TURBO\n"));
  } else if ((Cpuid06.RegEax & TURBO_MODE_SUPPORTED) == TURBO_MODE_SUPPORTED) {
    //
    // Turbo Mode is factory-configured as available and enabled for all logical processors in this processor package.
    // This case handles the cases where turbo mode is enabled before PPM gets chance to enable it
    //
    This->PpmFlags |= PPM_TURBO;
    DEBUG ((EFI_D_INFO, "\tTurbo mode : Available and Enabled\n"));
    DEBUG ((EFI_D_INFO, "\t-->PPM_TURBO\n"));
  }

  //
  // Restore the CPUID limit setting.
  //
  if (CpuidLimitingEnabled == TRUE) {
    Ia32MiscEnable.Qword = AsmReadMsr64 (EFI_MSR_IA32_MISC_ENABLE);
    Ia32MiscEnable.Qword |=(UINT64)(LIMIT_CPUID);
    AsmWriteMsr64 (EFI_MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  }
  
  //
  // Set the T-states flag
  //
  This->PpmFlags |= PPM_TSTATES;
  DEBUG ((EFI_D_INFO, "\tTState: Default Enabled\n"));
  DEBUG ((EFI_D_INFO, "\t-->PPM_TSTATES\n"));

  //
  // Set the S0ix flag
  //
  This->PpmFlags |= PPM_S0ix;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EnableCStates (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Enables C-State support as specified by the input flags on all logical
  processors and sets associated timing requirements in the chipset.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor C state support configured successfully.

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Enable C-States on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "\tApSafeEnableCStates: "));
  RunOnAllLogicalProcessors(ApSafeEnableCStates, This);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeEnableCStates (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  Enable C-State support as specified by the input flags on a logical processor.
    Configure BIOS C1 Coordination (SMI coordination)
    Enable IO redirection coordination
    Choose proper coordination method
    Configure extended C-States

  This function must be MP safe.

Arguments:
  
  Buffer        Pointer to a ENABLE_CSTATE_PARAMS containing the necessary
                information to enable C-States

Returns:

  EFI_SUCCESS   Processor C-State support configured successfully.

--*/
{
  MSR_REGISTER                      PmCfgCtrl;
  //MSR_REGISTER                    IoCaptAddr;
  MSR_REGISTER                      PowerCtrl;
  UINT32                            PpmMsr32;
  PPM_PROCESSOR_SUPPORT_PROTOCOL_2  *This;

  ///
  /// Extract parameters from the buffer
  ///
  This = (PPM_PROCESSOR_SUPPORT_PROTOCOL_2 *) Buffer;

  //
  // If C-states are disabled in setup, configure PACK_LIM in CLOCK_CST_CONFIG_CONTROL
  //
  if (!(This->PpmFlags & PPM_C_STATES)) {
    PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
    PmCfgCtrl.Dwords.Low &= ~CSTATE_LIMIT_MASK;
    AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);
    return;
  }

  //
  // Configure supported enhanced C-states
  //
  // Read Power Ctl MSR
  //
  PowerCtrl.Qword = AsmReadMsr64 (MSR_POWER_CTL);
//  DEBUG ((DEBUG_INFO, "MSR(1FC) before configuring C1E: %X %X\n", PowerCtrl.Dwords.High, PowerCtrl.Dwords.Low));

  //
  // Disable/Enable the C1E Feature
  //
  if (This->PpmFlags & PPM_C1E) {
    PowerCtrl.Dwords.Low |= C1E_ENABLE;
  } else {
    PowerCtrl.Dwords.Low &= ~C1E_ENABLE;
  }

  // Disable/Enable the CState Pre-Wake Feature
  PowerCtrl.Dwords.Low &= ~(B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE);
  if (mPpmPlatformPolicyProtocol.FunctionEnables.CStatePreWake == PPM_DISABLE) {
    PowerCtrl.Dwords.Low |= B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE;
  }

  //
  // Update Power Control MSR
  //
  AsmWriteMsr64 (MSR_POWER_CTL, PowerCtrl.Qword);
//  DEBUG ((DEBUG_INFO, "MSR(1FC) after configuring C1E: %X %X\n", PowerCtrl.Dwords.High, PowerCtrl.Dwords.Low));

  //
  // Set CState Core limit to the CState option enabled
  //
  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
  PmCfgCtrl.Dwords.Low &= ((~CSTATE_LIMIT_MASK) << 4);
  if (mPpmPlatformPolicyProtocol.MaxCState == 0) {
    PmCfgCtrl.Dwords.Low |= (CSTATE_LIMIT_C1 << 4);
//    DEBUG ((EFI_D_INFO, "CORE_CSTATE_LIMIT = C1 \n"));
  } else if (mPpmPlatformPolicyProtocol.MaxCState == 1) {
    PmCfgCtrl.Dwords.Low |= (CSTATE_LIMIT_C6 << 4);
//    DEBUG ((EFI_D_INFO, "CORE_CSTATE_LIMIT = C6 \n"));
  }
  //
  // Set CState package limit to the CState option enabled
  //
  PmCfgCtrl.Dwords.Low &= ~CSTATE_LIMIT_MASK;
  if (mPpmPlatformPolicyProtocol.PkgCStateLimit == 0) {
    PmCfgCtrl.Dwords.Low |= NOPKG_CSTATES;
//    DEBUG ((EFI_D_INFO, "PACKAGE_CSTATE_LIMIT = NO_PACKAGE_CSTATES \n"));
  } else if (mPpmPlatformPolicyProtocol.PkgCStateLimit == 1) {
    PmCfgCtrl.Dwords.Low |= NOS0IX;
//    DEBUG ((EFI_D_INFO, "PACKAGE_CSTATE_LIMIT = NO_S0Ix \n"));
  } else if (mPpmPlatformPolicyProtocol.PkgCStateLimit == 2) {
    PmCfgCtrl.Dwords.Low |= PKG_CSTATES_NOLIMIT;
//    DEBUG ((EFI_D_INFO, "PACKAGE_CSTATE_LIMIT = NO_LIMIT \n"));
  }
  //
  // Enable C State IO redirection by default
  //
  //PmCfgCtrl.Dwords.Low |= B_IO_MWAIT_REDIRECTION_ENABLE;

  //
  // Enable TimedMwait
  //
  if (This->PpmFlags & PPM_TIMED_MWAIT) {
    PmCfgCtrl.Dwords.Low &= (~B_TIMED_MWAIT_ENABLE);
    PmCfgCtrl.Dwords.Low |= B_TIMED_MWAIT_ENABLE;
  }

  ///
  /// Configure C-state auto-demotion
  ///
  PmCfgCtrl.Dwords.Low &= ~(B_C1_AUTO_DEMOTION_ENABLE | B_C3_AUTO_DEMOTION_ENABLE);
  if (mPpmPlatformPolicyProtocol.FunctionEnables.C3AutoDemotion) {
    ///
    /// Enable C6/C7 Auto-demotion to C3
    ///
    PmCfgCtrl.Dwords.Low |= B_C3_AUTO_DEMOTION_ENABLE;
  }
  if (mPpmPlatformPolicyProtocol.FunctionEnables.C1AutoDemotion) {
    ///
    /// Enable C3/C6/C7 Auto-demotion to C1
    ///
    PmCfgCtrl.Dwords.Low |= B_C1_AUTO_DEMOTION_ENABLE;
  }
  ///
  /// Configure C-state un-demotion
  ///
  PmCfgCtrl.Dwords.Low &= ~(B_C1_AUTO_UNDEMOTION_ENABLE | B_C3_AUTO_UNDEMOTION_ENABLE);
  if (mPpmPlatformPolicyProtocol.FunctionEnables.C3UnDemotion) {
    ///
    /// Enable un-demotion from demoted C3
    ///
    PmCfgCtrl.Dwords.Low |= B_C3_AUTO_UNDEMOTION_ENABLE;
  }
  if (mPpmPlatformPolicyProtocol.FunctionEnables.C1UnDemotion) {
    ///
    /// Enable un-demotion from demoted C1
    ///
    PmCfgCtrl.Dwords.Low |= B_C1_AUTO_UNDEMOTION_ENABLE;
  }
  //
  // Configure Package C-state Demotion / un-demotion
  //
  PmCfgCtrl.Dwords.Low &= ~(B_PKG_CSTATE_DEMOTION_ENABLE | B_PKG_CSTATE_UNDEMOTION_ENABLE);
  if (mPpmPlatformPolicyProtocol.FunctionEnables.PkgCStateDemotion) {
    //
    // Enable Package C-state Demotion
    //
    PmCfgCtrl.Dwords.Low |= B_PKG_CSTATE_DEMOTION_ENABLE;
  }
  if (mPpmPlatformPolicyProtocol.FunctionEnables.PkgCStateUnDemotion) {
    //
    // Enable Package C-state un-demotion
    //
    PmCfgCtrl.Dwords.Low |= B_PKG_CSTATE_UNDEMOTION_ENABLE;
  }
  AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);

  //
  // Enable/Disable L1/L2 Prefetcher
  //
  PpmMsr32 = AsmReadMsr32 (MSR_PREFETCHERS_DISABLE);
  if(mPpmPlatformPolicyProtocol.L1Prefetcher) {
    PpmMsr32 &= ~B_L1_PREFETCH_DISABLE;
  } else {
    PpmMsr32 |= B_L1_PREFETCH_DISABLE;
  }
  if(mPpmPlatformPolicyProtocol.L2Prefetcher) {
    PpmMsr32 &= ~B_L2_PREFETCH_DISABLE;
  } else {
    PpmMsr32 |= B_L2_PREFETCH_DISABLE;
  }
  AsmWriteMsr32 (MSR_PREFETCHERS_DISABLE, PpmMsr32);
  ///
  /// Enable MONITOR/MWAIT support
  /// (already done on BSP, but must be done on all components.)
  ///
  //Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  //Ia32MiscEnable.Qword |= B_MSR_IA32_MISC_ENABLE_MONITOR;
  //AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  ///
  /// Configuration of I/O capture and I/O coordination SMI MSR.
  /// Configure the base port and range in the MSR to match LVL_X settings in ACPI tables
    /// Set I/O capture base port and range
    ///
    //IoCaptAddr.Qword = AsmReadMsr64 (MSR_PMG_IO_CAPTURE_BASE);
    ///
    /// Mask off CST range and set the CST range
    ///
    //IoCaptAddr.Dwords.Low &= ~B_MSR_PMG_CST_RANGE;
    //if (This->PpmFlags & PPM_C10) {
    //  IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL7;
    //} else if (This->PpmFlags & PPM_C9) {
    //  IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL6;
    //} else if (This->PpmFlags & PPM_C8) {
    //  IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL5;
    //} else if (This->PpmFlags & PPM_C7) {
    //  IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL4;
    //} else if (This->PpmFlags & PPM_C6) {
    //  IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL3;
    //} else if (This->PpmFlags & PPM_C3) {
    //  IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL2;
    //}
    ///
    /// Set the base CST address
    ///
    //IoCaptAddr.Dwords.Low &= ~(V_IO_CAPT_LVL2_BASE_ADDR_MASK);
    //IoCaptAddr.Dwords.Low |= C3IoAddress;
    //AsmWriteMsr64 (MSR_PMG_IO_CAPTURE_BASE, IoCaptAddr.Qword);
  return;
}

STATIC
EFI_STATUS
InitThermal (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  )
/*++

Routine Description:

  This will perform general thermal initialization other then TM1, TM2, or
  PROCHOT# on all logical processors.

Arguments:
  
  This            Pointer to the protocol instance

Returns:

  EFI_SUCCESS     General thermal initialization completed successfully

--*/
{
  //
  // Run thermal code on all logical processors.
  //
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  DEBUG ((EFI_D_INFO, "\tApSafeInitThermal: "));
  RunOnAllLogicalProcessors(ApSafeInitThermal, PPMPolicy);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeInitThermal (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  This will perform general thermal initialization other then TM1, TM2, or PROCHOT#.
  This currently disables TM2 if enabled by default.  EnableTm2 will be called
  if TM2 is desired.

Arguments:
  
  This          Pointer to the protocol instance
  PpmFlags      Processor power management feature flags

Returns:

  EFI_SUCCESS     General thermal initialization completed successfully

--*/
{
  MSR_REGISTER                      Ia32MiscEnable;
  //MSR_REGISTER                      EbcHardPoweron;
  PPM_PLATFORM_POLICY_PROTOCOL      *This;

  //
  // Extract parameters from the buffer
  //
  This = (PPM_PLATFORM_POLICY_PROTOCOL *) Buffer;

  //
  // Configure Adaptive thermal monitor. IA32_MISC_ENABLE[3]
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  Ia32MiscEnable.Dwords.Low &= ~TM_ENABLE;
  if (This->FunctionEnables.EnableTm) {
    Ia32MiscEnable.Dwords.Low |= TM_ENABLE;
  }

  AsmWriteMsr64 (MSR_IA32_MISC_ENABLES, Ia32MiscEnable.Qword);

  //
  // Set the Stop-Enable (STEN) bit for Mermom so internal clocks stop during
  // AutoHalt or Stop Clock states.
  //
  //EbcHardPoweron.Qword = AsmReadMsr64 (EFI_MSR_EBC_HARD_POWERON);
  //EbcHardPoweron.Qword |= STOP_ENABLE;
  //AsmWriteMsr64 (EFI_MSR_EBC_HARD_POWERON, EbcHardPoweron.Qword);

  return;
}
STATIC
EFI_STATUS
EnableTm (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Enable TM1

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  None

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Enable TM1 on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "\tApSafeEnableTm: "));
  RunOnAllLogicalProcessors(ApSafeEnableTm, This);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeEnableTm (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  This function will enable TM.

  This function must be MP safe.

Arguments:
  
  Buffer        Pointer to the function parameters passed in.
                (Pointer to the PPM_PROCESSOR_SUPPORT_PROTOCOL_2 instance.)

Returns:

  EFI_SUCCESS             TM1 enabled

--*/
{
  /* @IMPORTANT NOTES:
   *  Enable Thermal Monitor features. Thermal throttling should take effect 
   *  when operating thermal conditions are exceeded. TM2 style throttling is 
   *  automatically engaged if GS3_EN is set. Otherwise, TM1 style throttling 
   *  will be engaged. Thermal interrupts to the CPU should also be disabled 
   *  if TM_EN is 0. Additionally, the Turbo range of operation should be 
   *  disallowed when TM_EN=0.
   *  TM_EN can only be set if FB_THERM_THROT_DIS_FUSE=0. 
   *    0 - Thermal throttling is disabled
   *    1 - Thermal throttling is enabled.
   */

  MSR_REGISTER                      Ia32MiscEnable;

  //
  // Enable TM1 in the CPU MSR
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (EFI_MSR_IA32_MISC_ENABLE);
  Ia32MiscEnable.Qword |=(UINT64)(TM_ENABLE);
  AsmWriteMsr64 (EFI_MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);

  return;
}

VOID
ApSafeEnableProcHot (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  This function will enable ProcHot.

  This function must be MP safe.

Arguments:

  Buffer        Pointer to the function parameters passed in.
                (Pointer to the PPM_PROCESSOR_SUPPORT_PROTOCOL_2 instance.)

Returns:

  EFI_SUCCESS             ProcHot enabled

--*/
{
  MSR_REGISTER            Ia32ThermInterrupt;
  UINT32                  Data;

  //
  // Configure IA32_THERM_INTERRUPT MSR 0x19B
  //
  Data  = ((UINT32)(mPpmPlatformPolicyProtocol.Threshold2InterruptEnable) << 23);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.TempThreshold2) << 16);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.Threshold1InterruptEnable) << 15);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.TempThreshold1) << 8);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.OutOfSpecInterruptEnable) << 4);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.ProcHotInterruptEnable) << 2);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.LowTempInterrupt) << 1);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.HighTempInterrupt));

  Ia32ThermInterrupt.Dwords.Low = AsmReadMsr32 ( EFI_MSR_IA32_THERM_INTERRUPT );
  Ia32ThermInterrupt.Dwords.Low &= ~( EFI_MSR_IA32_THERM_INTERRUPT_THRESHOLD2_INT_EN | EFI_MSR_IA32_THERM_INTERRUPT_TEMP_THRESHOLD_2 |
                                      EFI_MSR_IA32_THERM_INTERRUPT_THRESHOLD1_INT_EN | EFI_MSR_IA32_THERM_INTERRUPT_TEMP_THRESHOLD_1 |
                                      EFI_MSR_IA32_THERM_INTERRUPT_OUTOFSPEC_INT_EN  | EFI_MSR_IA32_THERM_INTERRUPT_PROCHOT_INT_EN   |
                                      EFI_MSR_IA32_THERM_INTERRUPT_LO_TEMP_INT_EN    | EFI_MSR_IA32_THERM_INTERRUPT_HI_TEMP_INT_EN );
  Ia32ThermInterrupt.Dwords.Low |= Data;

  AsmWriteMsr32 ( EFI_MSR_IA32_THERM_INTERRUPT, Ia32ThermInterrupt.Dwords.Low );

  return;
}

STATIC
EFI_STATUS
EnableProcHot (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Enables the bi-directional PROCHOT# signal on all logical processors.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   PROCHOT# configured successfully

--*/
{
  UINT32                              MemThermCtrlNewValue;
  UINT32                              Data;

  UINT64                              PtrMCHBAR;
  UINT32                              PpmMsr32;

  UINT32                              DtsConfig3Value;

  MemThermCtrlNewValue = 0;

  PpmMsr32 = 0;
  PtrMCHBAR            = 0;
  DtsConfig3Value      = 0;
  
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Get MCHBAR
  //
  PtrMCHBAR = PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_HI ) );
  PtrMCHBAR = LShiftU64(PtrMCHBAR, MCHBAR64_SHIFT_HI);
  PtrMCHBAR |= (UINT64) PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_LO ) );
  // Clear the less significative bit, because only indicate that this register is actived
  PtrMCHBAR &= (UINT64)(~BIT0);
  DEBUG ((EFI_D_INFO, "\tPtrMCHBAR: %x\n",PtrMCHBAR));

  //
  // TCONTROL_OFFSET
  //
  DtsConfig3Value = MmioRead32((UINTN )(PtrMCHBAR + DTS_CONFIG3_CFG_MCHBAR));
  DEBUG ((EFI_D_INFO, "\tDtsConfig3Value: %x\n",DtsConfig3Value));
  if (mPpmPlatformPolicyProtocol.TcontrolOffsetSign) {
    DtsConfig3Value |= TCONTROL_OFFSET_SIGN;
	DEBUG ((EFI_D_INFO, "\tTCONTROL OFFSET has a negative value\n"));
	DEBUG ((EFI_D_INFO, "\tDtsConfig3Value: %x\n",DtsConfig3Value));
  } else {
    DtsConfig3Value &= ~TCONTROL_OFFSET_SIGN;
	DEBUG ((EFI_D_INFO, "\tTCONTROL OFFSET has a positive value\n"));
	DEBUG ((EFI_D_INFO, "\tDtsConfig3Value: %x\n",DtsConfig3Value));
  }
  DtsConfig3Value &= ~TCONTROL_OFFSET;
  DEBUG ((EFI_D_INFO, "\tCLEAR TCONTROL OFFSET DtsConfig3Value: %x\n",DtsConfig3Value));
  DtsConfig3Value |= (mPpmPlatformPolicyProtocol.TcontrolOffset & TCONTROL_OFFSET);
  DEBUG ((EFI_D_INFO, "\tDtsConfig3Value: %x\n",DtsConfig3Value));
  MmioWrite32( (UINTN )( PtrMCHBAR + DTS_CONFIG3_CFG_MCHBAR ) , DtsConfig3Value );
  DEBUG ((EFI_D_INFO, "\tDTS_CONFIG3_CFG = 0x%08x\tValue = 0x%08x\n",PtrMCHBAR + DTS_CONFIG3_CFG_MCHBAR,MmioRead32((UINTN)(PtrMCHBAR + DTS_CONFIG3_CFG_MCHBAR))));

  PpmMsr32 = AsmReadMsr32 (MSR_POWER_CTL);
  if (mPpmPlatformPolicyProtocol.ProchotResponse) {
    PpmMsr32 |= B_PROCHOT_RESPONSE;
  } else {
    PpmMsr32 &= ~B_PROCHOT_RESPONSE;
  }

  //
  // Prochot Output Mode Disable
  //
  if (mPpmPlatformPolicyProtocol.ProchotOutputMode) {
    PpmMsr32 |= B_PROCHOT_OUTPUT_MODE;
  } else {
    PpmMsr32 &= ~B_PROCHOT_OUTPUT_MODE;
  }

  //
  // VrThermAlertDisable
  //
  if (mPpmPlatformPolicyProtocol.VrThermAlertDisable) {
    PpmMsr32 |= B_VR_THERM_ALERT_DISABLE;
  } else {
    PpmMsr32 &= ~B_VR_THERM_ALERT_DISABLE;
  }

  //
  // PROCHOT_FREQUENCY_RESPONSE
  //
  if (mPpmPlatformPolicyProtocol.ProchotFreqResponse) {
    PpmMsr32 |= B_PROCHOT_FREQUENCY_RESPONSE;
  } else {
    PpmMsr32 &= ~B_PROCHOT_FREQUENCY_RESPONSE;
  }

  AsmWriteMsr32 (MSR_POWER_CTL, PpmMsr32);
  DEBUG ((EFI_D_INFO, "\tMSR_POWER_CTL (MSR 0x1FC): 0x%x\n",AsmReadMsr32 ( MSR_POWER_CTL )));

  //
  // Lock Therm Int
  //
  PpmMsr32 = AsmReadMsr32 (MSR_MISC_PWR_MGMT);
  if (mPpmPlatformPolicyProtocol.LockThermInt) {
    PpmMsr32 |= B_LOCK_THERM_INT;
  } else {
    PpmMsr32 &= ~B_LOCK_THERM_INT;
  }
  AsmWriteMsr32 (MSR_MISC_PWR_MGMT, PpmMsr32);
  DEBUG ((EFI_D_INFO, "\tMSR_MISC_PWR_MGMT (MSR 0x1AA): 0x%x\n",AsmReadMsr32 ( MSR_MISC_PWR_MGMT )));

  //
  // BIOS should initialize THERM_INTERRUPT (MSR 0x1B2) with PROCHOT_INT_ENABLE = 1 (BIT 2)
  //
  PpmMsr32 = AsmReadMsr32 (MSR_PACKAGE_THERM_INTERRUPT);
  PpmMsr32 &= ~PROCHOT_INT_ENABLE;
  PpmMsr32 |= PROCHOT_INT_ENABLE;
  AsmWriteMsr32 (MSR_PACKAGE_THERM_INTERRUPT, PpmMsr32);
  DEBUG ((EFI_D_INFO, "\tMSR_PACKAGE_THERM_INTERRUPT (MSR 0x1B2): 0x%x\n",AsmReadMsr32 ( MSR_PACKAGE_THERM_INTERRUPT )));
  //
  // Configure TJ_Target in CPU_THERM_TEMPERATURE MSR
  //
  PpmMsr32 = AsmReadMsr32 (EFI_MSR_CPU_THERM_TEMPERATURE);

  PpmMsr32 &= ~(B_EFI_MSR_TJ_TARGET_OFFSET);
  PpmMsr32 |= ((UINT32)(mPpmPlatformPolicyProtocol.TjTargetOffset) << 24);
  AsmWriteMsr32 (EFI_MSR_CPU_THERM_TEMPERATURE, PpmMsr32);
  DEBUG ((EFI_D_INFO, "\tMSR_TEMPERATURE_TARGET (MSR 0x1A2): 0x%x\n",AsmReadMsr32 ( EFI_MSR_CPU_THERM_TEMPERATURE )));

  //
  // Configure IA32_THERM_INTERRUPT MSR 0x19B
  //
  Data  = ((UINT32)(mPpmPlatformPolicyProtocol.Threshold2InterruptEnable) << 23);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.TempThreshold2) << 16);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.Threshold1InterruptEnable) << 15);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.TempThreshold1) << 8);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.OutOfSpecInterruptEnable) << 4);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.ProcHotInterruptEnable) << 2);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.LowTempInterrupt) << 1);
  Data |= ((UINT32)(mPpmPlatformPolicyProtocol.HighTempInterrupt));

  PpmMsr32 = AsmReadMsr32 ( EFI_MSR_IA32_THERM_INTERRUPT );
  PpmMsr32 &= ~( EFI_MSR_IA32_THERM_INTERRUPT_THRESHOLD2_INT_EN | EFI_MSR_IA32_THERM_INTERRUPT_TEMP_THRESHOLD_2 |
                 EFI_MSR_IA32_THERM_INTERRUPT_THRESHOLD1_INT_EN | EFI_MSR_IA32_THERM_INTERRUPT_TEMP_THRESHOLD_1 |
                 EFI_MSR_IA32_THERM_INTERRUPT_OUTOFSPEC_INT_EN  | EFI_MSR_IA32_THERM_INTERRUPT_PROCHOT_INT_EN   |
                 EFI_MSR_IA32_THERM_INTERRUPT_LO_TEMP_INT_EN    | EFI_MSR_IA32_THERM_INTERRUPT_HI_TEMP_INT_EN );
  PpmMsr32 |= Data;

  AsmWriteMsr32 ( EFI_MSR_IA32_THERM_INTERRUPT, PpmMsr32 );

  DEBUG ((EFI_D_INFO, "\tApSafeEnableProcHot: "));
  RunOnAllLogicalProcessors(ApSafeEnableProcHot, This);

  DEBUG ((EFI_D_INFO, "\tMSR_THERM_INTERRUPT (MSR 0x19B): 0x%x\n",AsmReadMsr32 ( EFI_MSR_IA32_THERM_INTERRUPT )));

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeDisableGv3 (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  Enables GV3 support in a logical processor.

  This function must be MP safe.

Arguments:
  
  Buffer      Pointer to the function parameters passed in.
              (Pointer to the PPM_PROCESSOR_SUPPORT_PROTOCOL_2 instance.)

Returns:

  EFI_SUCCESS

--*/
{
  MSR_REGISTER                        Ia32MiscEnable;
  
  //
  // Enable GV3 in the CPU MSR
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  Ia32MiscEnable.Qword &= (UINT64 )(~GV3_ENABLE);
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLES, Ia32MiscEnable.Qword);

  return;
}

VOID
ApSafeSetBootPState (
  IN OUT VOID *Buffer
  )
/*++

Routine Description:

  Set processor P state to HFM or LFM.

Arguments:
  
  Buffer          Unused

Returns:

  EFI_SUCCESS   Processor MSR setting is saved.

--*/
{
  MSR_REGISTER Ia32PerfCtl;
  UINT16       BootRatio;
  UINT8        Index;

  BootRatio = 0;
  Ia32PerfCtl.Qword = AsmReadMsr64 (MSR_IA32_PERF_CTL);
  Ia32PerfCtl.Qword &= (UINT64 )(~B_IA32_PERF_CTRLP_STATE_TARGET);
  if (mPpmPlatformPolicyProtocol.BiosRequestFreq) {
    for (Index = 0; Index < mFvidPointer[0].FvidHeader.EistStates; Index++) {
      if (mFvidPointer[Index].FvidState.State == 1) {
        BootRatio = mFvidPointer[Index].FvidState.BusRatio;
        break;
      }
    }
    if (BootRatio) {
      Ia32PerfCtl.Qword |= LShiftU64 (BootRatio, P_STATE_TARGET_OFFSET);
      AsmWriteMsr64 (MSR_IA32_PERF_CTL, Ia32PerfCtl.Qword);
    }
  }

  return;
}

STATIC
EFI_STATUS
SetBootPState (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Set processor P state to HFM or LFM.

Arguments:
  
  This          Pointer to the protocol instance

Returns:
  EFI_UNSUPPORTED EIST not supported.
  EFI_SUCCESS     Processor P state has been set.

--*/
{
  MSR_REGISTER	Ia32MiscEnable;
  BOOLEAN       EistEnabled;
  UINT64        PtrMCHBAR;
  UINT32        BiosResetCplValue;

  PtrMCHBAR = 0;
  BiosResetCplValue = 0;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Get MCHBAR
  //
  PtrMCHBAR = PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_HI ) );
  PtrMCHBAR = LShiftU64(PtrMCHBAR, MCHBAR64_SHIFT_HI);
  PtrMCHBAR |= (UINT64) PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_LO ) );

  //
  // Clear the less significative bit, because only indicate that this register is actived
  //
  PtrMCHBAR &=(UINT64)(~BIT0);

  //
  // Set BIOS_RESET_CPL bit
  //
  BiosResetCplValue = MmioRead32((UINTN)(PtrMCHBAR + BIOS_RESET_CPL_MCHBAR));
  BiosResetCplValue |= BIOS_RESET_CPL_RST_CPL0;

  MmioWrite32( ((UINTN)(PtrMCHBAR + BIOS_RESET_CPL_MCHBAR)) , BiosResetCplValue );

  DEBUG ((EFI_D_INFO, "\tBIOS_RESET_CPL was set\n"));

  //
  // This function will be executed even when EIST is disabled so processor can be switched to HFM
  // Only skip it when EIST is not capable.
  //
  if ((mCpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_EIST) == 0) {
		return EFI_UNSUPPORTED ;
  }

  //
  // Enable EIST
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  EistEnabled = (BOOLEAN)RShiftU64((Ia32MiscEnable.Qword & GV3_ENABLE),GV3_ENABLE_OFFSET);
  //
  // Check whether EIST is disabled in setup.
  //
  if ((This->PpmFlags & PPM_GV3) == 0) {
    EistEnabled = 0;
  }
  //
  // If EIST is disabled, temporarily enable it
  //
  if (EistEnabled == 0) {
    DEBUG ((EFI_D_INFO, "\tApSafeEnableEist: "));
    RunOnAllLogicalProcessors (ApSafeEnableEist, This);
  }

  //
  // Set P-state to HFM on all cores
  //
  DEBUG ((EFI_D_INFO, "\tApSafeSetBootPState: "));
  RunOnAllLogicalProcessors (ApSafeSetBootPState, This);

  //
  // Disable EIST if we enabled it previously
  //
  if (EistEnabled == 0) {
    DEBUG ((EFI_D_INFO, "\tApSafeDisableGv3: "));
    RunOnAllLogicalProcessors (ApSafeDisableGv3, This);
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

/**
  The function makes the crash dump from the PUnit through the B2P mailbox only
  when this log is supported and available, ergo, the log was generated because an
  error occurred in the previous boot (i.e system crashed or hung).

  @param[in]      None
  @param[in out]  None

  @retval         EFI_NOT_FOUND           Crash log doesn't exist due there was not a crashed or hung system
                  EFI_INVALID_PARAMETER   Intended address to access is not a valid address
                  EFI_UNSUPPORTED         Crash dump is not supported by the B2P mailbox or the PUnit
                  EFI_END_OF_MEDIA        End of the Crash log was reached
                  EFI_SUCCESS             Crash dump was successfully made
**/
EFI_STATUS
GetCrashLogDump(
  )
{
  B2P_MAILBOX_INTERFACE B2PMailbox;
  UINT16                PunitRamAddress = 0x000;
  UINT32                Data = 0;
  UINT8                 DataCounter;
  UINT64                PtrMCHBAR = 0;

  // Get MCHBAR
  PtrMCHBAR = PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_HI ) );
  PtrMCHBAR = LShiftU64(PtrMCHBAR, MCHBAR64_SHIFT_HI);
  PtrMCHBAR |= (UINT64) PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_LO ) );
  // Clear the less significative bit, because only indicate that this register is actived
  PtrMCHBAR &=(UINT64 )(~BIT0);

  // Get fisrt 4B (Header) to verify if there is a crash log available to get
  do
  {
    B2PMailbox.Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) );
    if (DXE_IN_SIMICS)
    {
      B2PMailbox.Bits.RunBusy = 0;
    }
  } while (B2PMailbox.Bits.RunBusy);
  Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR) );
  if (DXE_IN_SIMICS)
  {
    Data |= B2P_HEADER_ERROR_BITS_MASK;
  }
  if ( !(Data & B2P_HEADER_ERROR_BITS_MASK))
  {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "CRASH_LOG_START"));
  while(PunitRamAddress < B2P_PUNIT_RAM_SIZE)
  {
    DEBUG ((DEBUG_INFO, "\n0x%03x",PunitRamAddress));
    for (DataCounter = 0; DataCounter < B2P_4B_FRAMES_PER_LINE; DataCounter++,PunitRamAddress++)
    {
      do
      {
        B2PMailbox.Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) );
        if (DXE_IN_SIMICS)
        {
          B2PMailbox.Bits.RunBusy = 0;
        }
      } while (B2PMailbox.Bits.RunBusy);
      B2PMailbox.Bits.Command = MBX_CMD_READ_CRASHDUMP;
      B2PMailbox.Bits.Address = PunitRamAddress;
      B2PMailbox.Bits.RunBusy = 1;
      if (PunitRamAddress == B2P_PUNIT_RAM_SIZE)
      {
        DEBUG ((DEBUG_INFO, "\nERROR_RANGE_CHECK\n"));
        return EFI_INVALID_PARAMETER;
      }
      MmioWrite32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR), B2PMailbox.Data);
      do
      {
        B2PMailbox.Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) );
        if (DXE_IN_SIMICS)
        {
          B2PMailbox.Bits.Command = 0;
          B2PMailbox.Bits.RunBusy = 0;
        }
      } while (B2PMailbox.Bits.RunBusy);
      if (B2PMailbox.Bits.Command != B2P_SUCCESS)
      {
        if (B2PMailbox.Bits.Command == B2P_ILLEGAL_CMD)
        {
          DEBUG ((DEBUG_INFO, "\nCRASH_LOG_NOT_SUPPORTED\n"));
          return EFI_UNSUPPORTED;
        }
        if (B2PMailbox.Bits.Command == B2P_ILLEGAL_ADDR)
        {
          DEBUG ((DEBUG_INFO, "\nCRASH_LOG_END\n"));
          return EFI_END_OF_MEDIA;
        }
      }
      Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR) );
      DEBUG ((DEBUG_INFO, " %08x",Data));
    }
  }
  DEBUG ((DEBUG_INFO, "\nCRASH_LOG_END\n"));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EnableTsc (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer,
  IN     UINT16                             iFSBFrequency,
  IN OUT UINT8                              *PpmCstTmrFlags,
  IN OUT UINTN                              *PpmTscCorrFactor,
  IN OUT UINTN                              *PpmTscCorrFactorRem
  )
/*++

Routine Description:

  Initialize the TSC support.

Arguments:
  
  This                                 Pointer to the protocol instance
  FvidPointer                          Pointer to a table to be updated
  iFSBFrequency                        Processor core iFSB Frequency
  PpmCstTmrFlags                       Flag of update TSC MSR if C3 or C4  
  PpmTscCorrFactor                     TSC correction factor 
  PpmTscCorrFactorRem                  TSC correction factor remainder

Returns:

  EFI_SUCCESS   Processor TSC support configured successfully.

--*/
{
  UINT64              FrequencyId;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Initialize the TSC update variables for Denverton
  //
  *PpmCstTmrFlags = CST_UPDT_TSC;

  //
  // Initialize the FVID tables, so that the maximum ratio setting is identified.
  //
  InitFvidTable (This, FvidPointer, FVID_MAX_STATES, FVID_MIN_STEP_SIZE, FALSE);
  ASSERT (FvidPointer->FvidHeader.EistStates != 0);

  //
  // Get the maximum frequency.
  //

  FrequencyId = mMaxBusRatio;
  //
  // Direct multiply Core IFSB with Max Ratio
  FrequencyId = MultU64x32 (FrequencyId, iFSBFrequency);

  //
  // Divide by timer base frequency
  // Save value and remainder
  //
  *PpmTscCorrFactor = (UINTN) DivU64x32Remainder (FrequencyId, CST_DATA_TBASE, (UINT32 *)PpmTscCorrFactorRem);
  //*PpmTscCorrFactor = (UINTN) DivU64x32 (FrequencyId, CST_DATA_TBASE);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
S3SaveMsr (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Save processor MSR runtime settings for S3.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor MSR setting is saved.

--*/
{
  UINT32   Index;
  
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  
  for (Index = 0; Index < sizeof (mMsrValues) / sizeof (EFI_MSR_VALUES); Index++) {
    DEBUG ((EFI_D_INFO, "\n\tSaving MSR(%X) = %X", mMsrValues[Index].Index,  mMsrValues[Index].Value));
    mMsrValues[Index].Value = AsmReadMsr64 (mMsrValues[Index].Index);
  }

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return  EFI_SUCCESS;
}

STATIC
EFI_STATUS
S3RestoreMsr (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Restore processor MSR runtime settings for S3.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor MSR setting is restored.

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Restore MSR's on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "ApSafeRestoreMsr: "));
  RunOnAllLogicalProcessors(ApSafeRestoreMsr, This);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeRestoreMsr (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  This function will restore MSR settings.

  This function must be MP safe.

Arguments:
  
  Buffer        Unused

Returns:

  EFI_SUCCESS   MSR restored

--*/
{
  UINT32  Index;

  for (Index = 0; Index < sizeof (mMsrValues) / sizeof (EFI_MSR_VALUES); Index++) {
    // DEBUG ((EFI_D_INFO, "\n\tRestoring MSR(%X) = %X", mMsrValues[Index].Index,  mMsrValues[Index].Value));
    AsmWriteMsr64 (mMsrValues[Index].Index, mMsrValues[Index].Value);
  }

  AsmWriteMsr64 (MSR_POWER_MISC, AsmReadMsr64(MSR_POWER_MISC) | ENABLE_IA_UNTRUSTED_MODE);
  return;
}

STATIC
EFI_STATUS
EnableCStateIoRedirection (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Enables IO redirection C-State control on all logical processors.
  This should only be enabled if the OS and processor driver support
  independent C-State control for C2 and higher.

  This will cause the processor to capture LVL_X reads and perform the requested
  C-State transition.

  This cannot be enabled when SMI based C-State coordination is enabled, 
  so this function will disable SMI based C-State coordination.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor IO redirection C-State control enabled.

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Enable C-State I/O redirection on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "\tApSafeEnableCStateIoRedirection: "));
  RunOnAllLogicalProcessors(ApSafeEnableCStateIoRedirection, This);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeEnableCStateIoRedirection (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  Enables C-State I/O redirection on a logical processor.

  This function must be MP safe.

Arguments:
  
  Buffer        Unused

Returns:

  EFI_SUCCESS   Processor IO redirection C-State control enabled.

--*/
{
  MSR_REGISTER    PmCfgCtrl;

    //
  // Enable I/O redirection control
  //
  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
  PmCfgCtrl.Dwords.Low |= B_IO_MWAIT_REDIRECTION_ENABLE;
  AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);

  return;
}

STATIC
EFI_STATUS
DisableCStateIoRedirection (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Disables C-State IO redirection on all logical processors.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor IO redirection C-State control disabled.

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Disable C-State I/O redirection on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "ApSafeDisableCStateIoRedirection: "));
  RunOnAllLogicalProcessors(ApSafeDisableCStateIoRedirection, This);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeDisableCStateIoRedirection (
  IN OUT VOID        *Buffer
  )
/*++

Routine Description:

  Disables C-State IO redirection on a logical processor.

  This function must be MP safe.

Arguments:
  
  Buffer        Unused

Returns:

  EFI_SUCCESS   Processor IO redirection C-State control disabled.

--*/
{
  MSR_REGISTER    PmCfgCtrl;

  //
  // Disable I/O redirection C-State control
  //
  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
  PmCfgCtrl.Dwords.Low &= ~B_IO_MWAIT_REDIRECTION_ENABLE;
  AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);

  return;
}

STATIC
VOID
ApSafeDisableTurboMode (
  IN OUT VOID      *Buffer
  )
/*++

Routine Description:

  - Disable Turbo Mode at package level

Arguments:

  This          Pointer to the protocol instance
  PPMPolicy     Pointer to policy protocol instance

Returns:

  None

--*/
{
  MSR_REGISTER    Ia32MiscEnableMsr;

  //
  // Set Turbo Mode disable bit in IA32 Misc Enable MSR
  //
  Ia32MiscEnableMsr.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  Ia32MiscEnableMsr.Dwords.High |= DISABLE_MASTER_TURBO_MODE;
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLES, Ia32MiscEnableMsr.Qword);
}


STATIC
VOID
ApSafeEnableTurboMode (
  IN OUT VOID      *Buffer
  )
/*++

Routine Description:

  - Enables Turbo Mode at package level

Arguments:

  This          Pointer to the protocol instance
  PPMPolicy     Pointer to policy protocol instance

Returns:

  None

--*/
{
  MSR_REGISTER    Ia32MiscEnableMsr;

  //
  // Clear Turbo Mode disable bit in IA32 Misc Enable MSR
  //
  Ia32MiscEnableMsr.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  Ia32MiscEnableMsr.Dwords.High &= ~DISABLE_MASTER_TURBO_MODE;
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLES, Ia32MiscEnableMsr.Qword);
}

VOID
InitEist (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy,
  IN OUT FVID_TABLE                         *FvidPointer
  )
/*++

Routine Description:

  Initializes required structures for P-State table creation and enables GV3
  support in the processor.

Arguments:
  
  This          Pointer to the protocol instance
  FvidPointer   Table to update, must be initialized.
  PPMPolicy     Pointer to policy protocol instance

Returns:

  EFI_SUCCESS

--*/
{
  MSR_REGISTER    TempMsr;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Test for Turbo Mode supported and initialize if true.
  //
  if(mTurboModeNotAvailable == 0) {
    if (This->PpmFlags & PPM_TURBO) {
      DEBUG ((EFI_D_INFO, "\tApSafeEnableTurboMode: "));
      RunOnAllLogicalProcessors (ApSafeEnableTurboMode, This);
    } else {
      DEBUG ((EFI_D_INFO, "\tApSafeDisableTurboMode: "));
      RunOnAllLogicalProcessors (ApSafeDisableTurboMode, This);
    }
  }

  //
  // Program Primary Power Plane Current Limit's
  //
  if( PPMPolicy->PrimaryPlaneCurrentLimit != AUTO ) {
    TempMsr.Qword = AsmReadMsr64 (MSR_PRIMARY_PLANE_CURRENT_CONFIG);
    TempMsr.Dwords.Low &= ~CURRENT_LIMIT_MASK;
    TempMsr.Dwords.Low |= (UINT32) PPMPolicy->PrimaryPlaneCurrentLimit;
    AsmWriteMsr64 (MSR_PRIMARY_PLANE_CURRENT_CONFIG, TempMsr.Qword);
  }
  ///
  /// Initialize the FVID tables.
  ///
  InitFvidTable (This, FvidPointer, FVID_MAX_STATES, FVID_MIN_STEP_SIZE, FALSE);
  ASSERT (FvidPointer->FvidHeader.EistStates != 0);

  ///
  /// Enable EIST on all logical processors.
  ///
  DEBUG ((EFI_D_INFO, "\tApSafeEnableEist: "));
  RunOnAllLogicalProcessors(ApSafeEnableEist, This);
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return;
}

VOID
ApSafeEnableEist (
  IN OUT VOID                         *Buffer
  )
/*++

Routine Description:

  Enables GV3 support in a logical processor.

  This function must be MP safe.

Arguments:
  
  Buffer      Pointer to the function parameters passed in.
              (Pointer to the PPM_PROCESSOR_SUPPORT_PROTOCOL_2 instance.)

Returns:

  EFI_SUCCESS

--*/
{
  MSR_REGISTER                        Ia32MiscEnable;
  MSR_REGISTER                        MiscPwrMgmt;
  PPM_PROCESSOR_SUPPORT_PROTOCOL_2    *This;

  //
  // Extract parameters from the buffer.
  //
  This = (PPM_PROCESSOR_SUPPORT_PROTOCOL_2*) Buffer;
  
  //
  // Enable EIST in the CPU MSR
  //
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  Ia32MiscEnable.Qword |= (UINT64)(GV3_ENABLE);
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLES, Ia32MiscEnable.Qword);
  
  //
  // If CMP is disabled, disable hardware coordination.
  //
  if (!(This->PpmFlags & PPM_CMP)) {
    MiscPwrMgmt.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
    MiscPwrMgmt.Qword |= (UINT64)(B_SINGLE_PCTL);
    //SINGLE_PCTL (Bit 11) was moved from SCP_CR_CST_CONFIG_CONTROL to PCU_CR_FIRM_CONFIG on NHM/GLM
    //AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);
    //DEBUG ((EFI_D_ERROR, "\n\tGV3 - Disable Hardware Coordination."));
  }

   return;
}

STATIC
EFI_STATUS
EnablePStateHardwareCoordination (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Enables GV3 hardware coordination on all logical processors.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Hardware coordination enabled.

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Enable GV3 hardware coordination on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "\tApSafeEnablePStateHardwareCoordination: "));
  RunOnAllLogicalProcessors(ApSafeEnablePStateHardwareCoordination, This);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeEnablePStateHardwareCoordination (
  IN OUT VOID   *Buffer
  )
/*++

Routine Description:

  Enable the hardware coordination of P-states.

  This function must be MP safe.

Arguments:
  
  Buffer        Unused

Returns:

  EFI_SUCCESS   Hardware coordination enabled.

--*/
{
  MSR_REGISTER    PmCfgCtrl;
  MSR_REGISTER    PowerMisc;

  // DEBUG ((EFI_D_ERROR, "\tApSafeEnablePStateHardwareCoordination()\n"));
  //
  // Enable HW coordination
  //

  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
  PmCfgCtrl.Dwords.Low |= B_SINGLE_PCTL;  
  //SINGLE_PCTL (Bit 11) was moved from SCP_CR_CST_CONFIG_CONTROL to PCU_CR_FIRM_CONFIG on NHM/GLM
  //AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);
  
  PowerMisc.Qword = AsmReadMsr64 (MSR_POWER_MISC);
  // EHalt should not be set if SINGLE_PCTL=1.
  PowerMisc.Qword &=(UINT64)(~(ENABLE_ULFM_AUTOCM_MASK | ENABLE_INDP_AUTOCM_MASK));
  AsmWriteMsr64 (MSR_POWER_MISC, PowerMisc.Qword);
  return;
}

STATIC
EFI_STATUS
DisablePStateHardwareCoordination (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Disable the hardware coordination of P-states.

Arguments:
  
  This                Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Hardware coordination disabled.

--*/
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Disable GV3 hardware coordination on all logical processors.
  //
  DEBUG ((EFI_D_INFO, "ApSafeDisablePStateHardwareCoordination: "));
  RunOnAllLogicalProcessors(ApSafeDisablePStateHardwareCoordination, This);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
ApSafeDisablePStateHardwareCoordination (
  IN OUT VOID   *Buffer
  )
/*++

Routine Description:

  Disables GV3 hardware coordination on a logical processor.

  This function must be MP safe.

Arguments:
  
  Buffer        Unused

Returns:

  EFI_SUCCESS   Hardware coordination disabled.

--*/
{
  MSR_REGISTER    PmCfgCtrl;

  // DEBUG ((EFI_D_ERROR, "\nApSafeDisablePStateHardwareCoordination()"));
  //
  // Disable HW coordination
  //
  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PM_CFG_CTRL);
  PmCfgCtrl.Dwords.Low |= B_SINGLE_PCTL;
  //SINGLE_PCTL (Bit 11) was moved from SCP_CR_CST_CONFIG_CONTROL to PCU_CR_FIRM_CONFIG on NHM/GLM
  //AsmWriteMsr64 (MSR_PM_CFG_CTRL, PmCfgCtrl.Qword);

  return;
}

STATIC
EFI_STATUS
InitFvidTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer,
  IN     UINT16                             MaxNumberOfStates,
  IN     UINT16                             MinStepSize,
  IN     BOOLEAN                            CreateDefaultTable
  )
/*++

Routine Description:

  This function updates the table provided with the FVID data for the processor.
  If CreateDefaultTable is TRUE, a minimam FVID table will be provided.
  The maximum number of states must be greater then or equal to two.
  The table should be initialized in such a way as for the caller to determine if the
  table was updated successfully.  This function should be deprecated in the future when
  Release 8 is integrated in favor of the EIST protocol calculating FVID information.

Arguments:
  
  This                Pointer to the protocol instance
  FvidPointer         Pointer to a table to be updated
  MaxNumberOfStates   Number of entries in the table pointed to by FvidPointer
  MinStepSize         Minimum step size for generating the FVID table
  CreateDefaultTable  Create default FVID table rather then full state support

Returns:

  EFI_SUCCESS     Table pointed to FvidPointer updated with desired FVID states

--*/
{
  EFI_STATUS    Status;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // If the FVID tables have already been created, return.
  //
  if (FvidPointer[0].FvidHeader.EistStates != 0) {
    return EFI_SUCCESS;
  }
  //
  // Create FVID table
  //
  if (CreateDefaultTable) {
    CreateDefaultFvidTable (FvidPointer);
    This->PpmFlags &= ~(PPM_TURBO | PPM_DYNAMIC_FSB);
  } else {
    Status = CreateFvidTable (This, FvidPointer, MaxNumberOfStates);
    if (EFI_ERROR (Status)) {
      CreateDefaultFvidTable (FvidPointer);
      This->PpmFlags &= ~(PPM_TURBO | PPM_DYNAMIC_FSB);
    }
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

STATIC
VOID
CreateDefaultFvidTable (
  IN OUT FVID_TABLE     *FvidPointer
  )
/*++

Routine Description:

  Create default FVID table with max and min states only.

Arguments:
  
  FvidPointer         Pointer to a table to be updated

Returns:

  None.

--*/  
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Fill in the header.
  //
  FvidPointer[0].FvidHeader.Stepping     = (mCpuid01.RegEax & CPUID_FULL_FAMILY_MODEL_STEPPING);
  FvidPointer[0].FvidHeader.MaxVid       = mMaxVid;
  FvidPointer[0].FvidHeader.MaxBusRatio  = mMaxBusRatio;
  FvidPointer[0].FvidHeader.EistStates    = 2;
  
  //
  // First entry is state 0, highest state.
  //
  FvidPointer[1].FvidState.State     = 0;
  FvidPointer[1].FvidState.Vid       = mMaxVid;
  FvidPointer[1].FvidState.BusRatio  = mMaxBusRatio;
  FvidPointer[1].FvidState.Power     = FVID_MAX_POWER;
  
  //
  // Second entry is state 1, lowest state. 
  //
  FvidPointer[2].FvidState.State     = 1;
  FvidPointer[2].FvidState.Vid       = (UINT16) mMinVid;
  FvidPointer[2].FvidState.BusRatio  = (UINT16) mMinBusRatio;
  FvidPointer[2].FvidState.Power     = FVID_MIN_POWER;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

EFI_STATUS
CreateFvidTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE *FvidPointer,
  IN UINT16         MaxNumberOfStates
  )
/*++
  Routine Description:

  Create an FVID table based on the algorithm provided by the BIOS writer's guide.

  Arguments:

  @param[in]This                 Pointer to the protocol instance
  @param[in out] FvidPointer     Pointer to a table to be updated
  @param[in] MaxNumberOfStates   Number of entries in the table pointed to by FvidPointer

  Returns:

  @retval EFI_SUCCESS            FVID table created successfully.
  @retval EFI_INVALID_PARAMETER  The bus ratio range don't permit FVID table calculation;
                                 a default FVID table should be constructed.
--*/
{
  UINT16 BusRatioRange;
  UINT16 PowerRange;
  UINT16 NumberOfStates;
  UINT16 Turbo;
  UINT16 index;
  UINT16 StepSize;
  UINT16 MaxRatio;
  UINT64 wPower1;
  UINT64 wPower2;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Determine whether turbo mode is supported or not
  //
  Turbo = 0;
  Turbo = ((This->PpmFlags & PPM_TURBO) ? 1 : 0);
  DEBUG((EFI_D_INFO, "\tTurbo Mode:[%04x]\n", Turbo));

  //
  // Determine the bus ratio range
  //
  BusRatioRange = (Turbo == 1 ? (mTurboBusRatio - mMinBusRatio) : (mMaxBusRatio - mMinBusRatio));
  MaxRatio = mMaxBusRatio;
  if (((INT16) BusRatioRange < 0) || (MaxNumberOfStates == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  DEBUG((EFI_D_INFO, "\tBusRatioRange:[%04x]\n", BusRatioRange));

  //
  // Determine the Power range
  //
  PowerRange = FVID_MAX_POWER - FVID_MIN_POWER;

  //
  //  Determine the number of states as cpu supported range or Maximum _PSS limit
  //
  NumberOfStates = (mMaxBusRatio - mMinBusRatio + 1);
  if (Turbo) {
    NumberOfStates += 1;
  }
  NumberOfStates = (NumberOfStates < MaxNumberOfStates ? NumberOfStates : MaxNumberOfStates);
  DEBUG((EFI_D_INFO, "\tNumberOfStates:[%04x]\n", NumberOfStates));

  //
  // Ensure we have at least two states
  //
  if ((NumberOfStates + Turbo) < 2) {
    //
    // In case HFM = LFM and no Turbo, at least have two states with same ratio values
    //
    NumberOfStates = 2;
  }

  //
  // Fill in the table header
  //
  FvidPointer[0].FvidHeader.Stepping    = mProcessorStepping;
  FvidPointer[0].FvidHeader.MaxBusRatio = (Turbo ? mTurboBusRatio : mMaxBusRatio);
  FvidPointer[0].FvidHeader.EistStates  = (UINT16) (NumberOfStates < MaxNumberOfStates ? NumberOfStates : MaxNumberOfStates);

  DEBUG ((EFI_D_INFO, "\tFVID[00].FvidHeader.Stepping = %x\n", FvidPointer[0].FvidHeader.Stepping));
  DEBUG ((EFI_D_INFO, "\tFVID[00].FvidHeader.MaxBusRatio = %x\n", FvidPointer[0].FvidHeader.MaxBusRatio));
  DEBUG ((EFI_D_INFO, "\tFVID[00].FvidHeader.EistStates = %x\n", FvidPointer[0].FvidHeader.EistStates));

  //
  // Add Turbo as P0 if Turbo Mode supported and initialize.
  //
  if (This->PpmFlags & PPM_TURBO) {
    FvidPointer[1].FvidState.BusRatio = mTurboBusRatio;
    FvidPointer[1].FvidState.Power = (mPackageTdpWatt * 1000); // power is calculated in milliwatts
  } else {
    FvidPointer[1].FvidState.BusRatio = mMaxBusRatio;
    FvidPointer[1].FvidState.Power = (mPackageTdpWatt * 1000);
  }
  DEBUG ((EFI_D_INFO, "\tFVID[01].FvidState.State = %x\n", FvidPointer[1].FvidState.State));
  DEBUG ((EFI_D_INFO, "\tFVID[01].FvidState.BusRatio = %x\n",  FvidPointer[1].FvidState.BusRatio));
  DEBUG ((EFI_D_INFO, "\tFVID[01].FvidState.Power = %x\n", FvidPointer[1].FvidState.Power));

  //
  // Fill in the table starting at the last entry
  // The algorithm is available in the processor BIOS writer's guide.
  //
  for (index = 1, StepSize = 1; index < NumberOfStates; index++) {
    FvidPointer[index + 1].FvidState.State    = index;
    FvidPointer[index + 1].FvidState.BusRatio = FvidPointer[index].FvidState.BusRatio - StepSize;
    if (Turbo && (index == 1)) {
      FvidPointer[index + 1].FvidState.BusRatio = mMaxBusRatio;
    }
    //
    // Calculate Relative Power
    //
    wPower1 = (MaxRatio - FvidPointer[index + 1].FvidState.BusRatio) * 625;
    wPower1 = (110000 - wPower1);
    wPower1 = DivU64x32 (wPower1, 11);
    wPower1 = MultU64x64 (wPower1, wPower1);
    //
    // Power is calculated in milliwatts
    //
    wPower2 = (((FvidPointer[index + 1].FvidState.BusRatio * 100) / MaxRatio));
    wPower2 = DivU64x32 (MultU64x32 (MultU64x64 (wPower2, wPower1), mPackageTdpWatt), 10000000);
    FvidPointer[index + 1].FvidState.Power = (UINT32) wPower2;
    if (Turbo && index == 1) {
      FvidPointer[index + 1].FvidState.Power = (mPackageTdpWatt * 1000);
    }
    DEBUG ((EFI_D_INFO, "\tFVID[%02d].FvidState.State = %x\n", (1 + index), FvidPointer[1 + index].FvidState.State));
    DEBUG ((EFI_D_INFO, "\tFVID[%02d].FvidState.BusRatio = %x\n", (1 + index), FvidPointer[1 + index].FvidState.BusRatio));
    DEBUG ((EFI_D_INFO, "\tFVID[%02d].FvidState.Power = %x\n", (1 + index), FvidPointer[1 + index].FvidState.Power));
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

UINT8 GetConvertedTime(
  IN UINT32 TimeInSeconds
  )
/*++

Routine Description:

  - Private helper function to convert various Turbo Power Limit Time from Seconds to CPU units

Arguments:

  TimeInSeconds     Time in seconds

Returns:

  Converted time in CPU units

--*/
{
  UINT8         ConvertedPowerLimitTime;
  UINT8         Index;

  //
  // Convert seconds to MSR value. Since not all values are programmable, we'll select
  // the entry from mapping table which is either equal to the user selected value. OR to a value in the mapping table
  // which is closest (but less than) to the user-selected value.
  //
  ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[0][1];

  for (Index = 0; mSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {
    if (TimeInSeconds == mSecondsToMsrValueMapTable[Index][0]) {
      ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[Index][1];
      break;
    }

    if (TimeInSeconds > mSecondsToMsrValueMapTable[Index][0]) {
      ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[Index][1];
    } else {
      break;
      break;
    }
  }
  return ConvertedPowerLimitTime;  
}

VOID
UpdateCurrentPowerInfo (
  )
/*++

Routine Description:

  Get the updated power configuration register values

Arguments:

  None

Returns:

  None

--*/
{

  MSR_REGISTER        TempMsr;

  TempMsr.Qword        = AsmReadMsr64 (MSR_PLATFORM_INFO);
  mTdpLimitProgrammble = 1;

  TempMsr.Qword        = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  //
  // Get Power Unit MSR [3:0]
  // The actual unit value is calculated by 1mW*Power(2,POWER_UNIT)..Reset value of 5 represents 32mW units.
  //
  mProcessorPowerUnit  = (TempMsr.Bytes.FirstByte & PACKAGE_POWER_UNIT_MASK);

  if (mProcessorPowerUnit == 0) {
    mProcessorPowerUnit = 1;
  } else {
    mProcessorPowerUnit = (UINT8) LShiftU64 (2, (mProcessorPowerUnit - 1));
  }
  // @Note: This field should be initialized by PUnit to IACORE_GUAR_TDP_FUSE + SOC_GUAR_TDP_FUSE
  //            Get the pre-si estimated settings for the Turbo from Punit. 
  mPackageTdp      = 0x1F;
  mPackageMaxPower = 0xFF;
  mPackageMinPower = 0x3F;

  return;
}

STATIC
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE   Procedure,
  IN OUT VOID               *Buffer
  )
/*++

Routine Description:

  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

Arguments:
  
  Procedure     The function to be run.
  Buffer        Pointer to a parameter buffer.

Returns:

  None

--*/
{
  UINTN                     NumberOfProcessors;
  UINTN                     NumberOfEnabledProcessors;
  EFI_STATUS Status;

  // DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Run the procedure on all logical processors.
  //
  (*Procedure) (Buffer);

	Status = mMpServices->GetNumberOfProcessors(mMpServices, &NumberOfProcessors, &NumberOfEnabledProcessors);

	if (!EFI_ERROR (Status) && NumberOfEnabledProcessors > 1) {
	  Status = mMpServices->StartupAllAPs (mMpServices, Procedure, FALSE, NULL, 0, Buffer, NULL);
	  DEBUG ((EFI_D_INFO , "Running Procedure on AP NumberOfProcessors = %d\n", NumberOfProcessors));
	}

  // DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return Status;
}

STATIC
VOID
ConfigureFreqLimit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  )
/*++

Routine Description:

  The function configures Ratio Limits & Core Counts to be used to limit frequency (MSR 0x1AD & MSR 0x1AE).  

Arguments:

  This          Pointer to the protocol instance
  PPMPolicy     Pointer to policy protocol instance

Returns:

  None

--*/
{
  UINT64              MsrData;
  UINT8               DefaultRatioLimit0;
  UINT8               DefaultRatioLimit1;
  UINT8               DefaultRatioLimit2;
  UINT8               DefaultRatioLimit3;
  UINT8               DefaultRatioLimit4;
  UINT8               DefaultRatioLimit5;
  UINT8               DefaultRatioLimit6;
  UINT8               DefaultRatioLimit7;
  UINT8               DefaultCoreCount0;
  UINT8               DefaultCoreCount1;
  UINT8               DefaultCoreCount2;
  UINT8               DefaultCoreCount3;
  UINT8               DefaultCoreCount4;
  UINT8               DefaultCoreCount5;
  UINT8               DefaultCoreCount6;
  UINT8               DefaultCoreCount7;

  MsrData               = 0;
  DefaultRatioLimit0    =  mPpmPlatformPolicyProtocol.RatioLimit0;
  DefaultRatioLimit1    =  mPpmPlatformPolicyProtocol.RatioLimit1;
  DefaultRatioLimit2    =  mPpmPlatformPolicyProtocol.RatioLimit2;
  DefaultRatioLimit3    =  mPpmPlatformPolicyProtocol.RatioLimit3;
  DefaultRatioLimit4    =  mPpmPlatformPolicyProtocol.RatioLimit4;
  DefaultRatioLimit5    =  mPpmPlatformPolicyProtocol.RatioLimit5;
  DefaultRatioLimit6    =  mPpmPlatformPolicyProtocol.RatioLimit6;
  DefaultRatioLimit7    =  mPpmPlatformPolicyProtocol.RatioLimit7;
  DefaultCoreCount0     =  mPpmPlatformPolicyProtocol.CoreCount0;
  DefaultCoreCount1     =  mPpmPlatformPolicyProtocol.CoreCount1;
  DefaultCoreCount2     =  mPpmPlatformPolicyProtocol.CoreCount2;
  DefaultCoreCount3     =  mPpmPlatformPolicyProtocol.CoreCount3;
  DefaultCoreCount4     =  mPpmPlatformPolicyProtocol.CoreCount4;
  DefaultCoreCount5     =  mPpmPlatformPolicyProtocol.CoreCount5;
  DefaultCoreCount6     =  mPpmPlatformPolicyProtocol.CoreCount6;
  DefaultCoreCount7     =  mPpmPlatformPolicyProtocol.CoreCount7;
  
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  if (mPpmPlatformPolicyProtocol.RatioLimitsEnable) {
    ///
    /// Set MSR 0x1AD: P_CR_FREQ_LIMIT_RATIOS (Ratio Limits).
    ///
    MsrData = AsmReadMsr64( EFI_MSR_FREQ_LIMIT_RATIOS );
    DEBUG ((EFI_D_INFO, "\tMSR P_CR_FREQ_LIMIT_RATIOS = 0x%08x", MsrData >> 32));
    DEBUG ((EFI_D_INFO, "%08x\n", MsrData));
    MsrData &= ~(B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_7 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_6 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_5 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_4 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_3 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_2 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_1 |
                  B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_0);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit0 = %x\n", DefaultRatioLimit0));
    MsrData |= (UINT64) DefaultRatioLimit0;

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit1 = %x\n", DefaultRatioLimit1));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit1, 8);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit2 = %x\n", DefaultRatioLimit2));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit2, 16);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit3 = %x\n", DefaultRatioLimit3));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit3, 24);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit4 = %x\n", DefaultRatioLimit4));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit4, 32);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit5 = %x\n", DefaultRatioLimit5));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit5, 40);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit6 = %x\n", DefaultRatioLimit6));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit6, 48);

    DEBUG ((EFI_D_INFO, "\tDefaultRatioLimit7 = %x\n", DefaultRatioLimit7));
    MsrData |= (UINT64) LShiftU64 (DefaultRatioLimit7, 56);

    AsmWriteMsr64( EFI_MSR_FREQ_LIMIT_RATIOS, MsrData );
    MsrData = AsmReadMsr64( EFI_MSR_FREQ_LIMIT_RATIOS );
    DEBUG ((EFI_D_INFO, "\tMSR P_CR_FREQ_LIMIT_RATIOS = 0x%08x", MsrData >> 32));
    DEBUG ((EFI_D_INFO, "%08x\n", MsrData));

    ///
    /// Set MSR 0x1AE: P_CR_FREQ_LIMIT_CORES (Core Limits).
    ///
    MsrData = AsmReadMsr64( EFI_MSR_FREQ_LIMIT_CORES );
    DEBUG ((EFI_D_INFO, "\tMSR P_CR_FREQ_LIMIT_CORES = 0x%08x", MsrData >> 32));
    DEBUG ((EFI_D_INFO, "%08x\n", MsrData));
    MsrData &= ~( B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_7 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_6 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_5 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_4 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_3 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_2 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_1 |
                  B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_0 );

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount0 = %x\n", DefaultCoreCount0));
    MsrData |= (UINT64) DefaultCoreCount0;

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount1 = %x\n", DefaultCoreCount1));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount1, 8);

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount2 = %x\n", DefaultCoreCount2));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount2, 16);

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount3 = %x\n", DefaultCoreCount3));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount3, 24);

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount4 = %x\n", DefaultCoreCount4));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount4, 32);

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount5 = %x\n", DefaultCoreCount5));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount5, 40);

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount6 = %x\n", DefaultCoreCount6));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount6, 48);

    DEBUG ((EFI_D_INFO, "\tDefaultCoreCount7 = %x\n", DefaultCoreCount7));
    MsrData |= (UINT64) LShiftU64 (DefaultCoreCount7, 56);

    AsmWriteMsr64( EFI_MSR_FREQ_LIMIT_CORES, MsrData );
    MsrData = AsmReadMsr64( EFI_MSR_FREQ_LIMIT_CORES );
    DEBUG ((EFI_D_INFO, "\tMSR P_CR_FREQ_LIMIT_CORES = 0x%08x", MsrData >> 32));
    DEBUG ((EFI_D_INFO, "%08x\n", MsrData));
  }

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
  The function configures RAPL limits for PL1 & PL2 (MSR 0x610).

  @param[in]      None
  @param[in out]  None

  @retval         EFI_SUCESS
**/
STATIC
VOID
ConfigureRaplLimit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  )
{
  UINT32              Index;
  UINT64              MsrData;
  UINT16              PowValue;
  UINT8               TauTempVal;
  UINT64              RaplTempVal;
  UINT16              PowerLimit1;
  UINT16              PowerLimit2;
  UINT32              PowerSkuPowerUnit;
  UINT16              PwrLimitResolution;
  UINT32              PackageRaplTimeWindow1;
  UINT16              PackageRaplPowerLevel1;
  UINT16              PackageRaplPowerLevel2;

  Index                  = 0;
  MsrData                = 0;
  PowValue               = 2;
  TauTempVal             = 0;
  RaplTempVal            = 0;
  PowerLimit1            = 0;
  PowerLimit2            = 0;
  PowerSkuPowerUnit      = 0;
  PwrLimitResolution     = 0;
  PackageRaplTimeWindow1 = 0;
  PackageRaplPowerLevel1 = mPpmPlatformPolicyProtocol.PackageRaplPowerLevel1;
  PackageRaplPowerLevel2 = mPpmPlatformPolicyProtocol.PackageRaplPowerLevel2;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  ///
  /// Read MSR 0x606: P_CR_PACKAGE_POWER_SKU_UNIT.
  ///
  MsrData =  AsmReadMsr64( EFI_MSR_PKG_POWER_SKU_UNIT );
  DEBUG ((EFI_D_INFO, "\tMSR P_CR_PACKAGE_POWER_SKU_UNIT = 0x%08x\n", MsrData));

  ///
  /// Get Power Unit (PWR_UNIT)
  ///
  PowerSkuPowerUnit = (UINT32)(MsrData & B_EFI_MSR_PKG_POWER_SKU_UNIT_POWER_UNIT);
  do{
    Index++;
    PowValue *= 2;
  } while (Index < PowerSkuPowerUnit-1);
  DEBUG ((EFI_D_INFO, "\tPWR_UNIT = 0x%x = 1/%d W\n", PowerSkuPowerUnit, PowValue));

  ///
  /// Set MSR 0x610: P_CR_PACKAGE_RAPL_LIMIT (PL1 & PL2)
  ///
  MsrData =  AsmReadMsr64( EFI_MSR_PKG_RAPL_LIMIT );
  DEBUG ((EFI_D_INFO, "\tMSR P_CR_PACKAGE_RAPL_LIMIT = 0x%08x", MsrData >> 32));
  DEBUG ((EFI_D_INFO, "%08x\n", MsrData));
  if (mPpmPlatformPolicyProtocol.PackageRaplTimeWindow1 == 0xFFFFFFFF) {
    DEBUG ((EFI_D_INFO, "\tRAPL PL1 & PL2 Default Values:\n", PackageRaplTimeWindow1));
    PackageRaplTimeWindow1 = (UINT32) (MsrData >> N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME);
    PackageRaplTimeWindow1 &= PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME_MASK;
    DEBUG ((EFI_D_INFO, "\tPL1 Time Window: 0x%x\n", PackageRaplTimeWindow1));

    PackageRaplPowerLevel1 = (UINT16) (MsrData >> N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1);
    PackageRaplPowerLevel1 &= PKG_RAPL_LIMIT_PKG_PWR_LIM1_MASK;
    DEBUG ((EFI_D_INFO, "\tPL1 Power Limit: %d W\n", PackageRaplPowerLevel1/PowValue));

    PackageRaplPowerLevel2 = (UINT16) (MsrData >> N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2);
    PackageRaplPowerLevel2 &= PKG_RAPL_LIMIT_PKG_PWR_LIM2_MASK;
    DEBUG ((EFI_D_INFO, "\tPL2 Power Limit: %d W\n", PackageRaplPowerLevel2/PowValue ));
  } else {
    PackageRaplTimeWindow1 = mPpmPlatformPolicyProtocol.PackageRaplTimeWindow1;
    DEBUG ((EFI_D_INFO, "\tPL1 Time Window: 0x%x\n", PackageRaplTimeWindow1));

    //PackageRaplPowerLevel1 = mPpmPlatformPolicyProtocol.PackageRaplPowerLevel1;
    DEBUG ((EFI_D_INFO, "\tPL1 Power Limit: %d W\n", PackageRaplPowerLevel1));
    PackageRaplPowerLevel1 *= PowValue;

    //PackageRaplPowerLevel2 = mPpmPlatformPolicyProtocol.PackageRaplPowerLevel2;
    DEBUG ((EFI_D_INFO, "\tPL2 Power Limit: %d W\n", PackageRaplPowerLevel2));
    PackageRaplPowerLevel2 *= PowValue;

    TauTempVal  = Time2TAU (PackageRaplTimeWindow1);
    if (!TauTempVal) {
      TauTempVal = (UINT8)PackageRaplTimeWindow1;
    }

    RaplTempVal = ((((UINT64) PackageRaplPowerLevel2) << N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2) |
                  (((UINT64) PackageRaplTimeWindow1) << N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME) |
                  ((UINT64) PackageRaplPowerLevel1)
                  );

    MsrData = AsmReadMsr64 (EFI_MSR_PKG_RAPL_LIMIT) &
                          ~(B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2 |
                          B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME |
                          B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1
                          );
    MsrData |= RaplTempVal;
    if (MsrData != AsmReadMsr64 (EFI_MSR_PKG_RAPL_LIMIT) && mPpmPlatformPolicyProtocol.MfgMode != 1) {
      AsmWriteMsr64 (EFI_MSR_PKG_RAPL_LIMIT, MsrData);
    }
  }

  PwrLimitResolution = ((1000/PowValue));
  DEBUG ((EFI_D_INFO, "\tPwrLimitResolution %d\n", PwrLimitResolution));

  PowerLimit1 = PackageRaplPowerLevel1;
  PowerLimit2 = PackageRaplPowerLevel2;

  MsrData =  AsmReadMsr64( EFI_MSR_PKG_RAPL_LIMIT );
  DEBUG ((EFI_D_INFO, "\tMSR P_CR_PACKAGE_RAPL_LIMIT = 0x%08x", MsrData >> 32));
  DEBUG ((EFI_D_INFO, "%08x\n", MsrData));
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

UINT8
Time2TAU (
  IN UINT32 PackageRaplTimeWindow
  )
/*++

Routine Description:

  This routine programs Package Rapl Limit as per following equation:
  TW = Time Unit * (1 + (x/4))*2^y
  Value provided is divided by the closest power of 2 that does not exceed the value.
  Remainder should be a number between 1 and 2. Variable x is programmed to the closest value:
  (1 -> 0, 1.25 -> 1, 1.5->2, 1.75-> 3)

Arguments:

  PackageRaplTimeWindow   Input in milliseconds.

--*/
{
  UINT8  Index;
  UINT8  TWX;
  UINT16 TWY;
  UINT32 TimeWindowY;

  TWX = 0;
  TWY = 0;
  TimeWindowY = PackageRaplTimeWindow;

  //
  // Variable Y is found by dividing or shifting until remainder is <= 200
  //
  for (Index = 0; TimeWindowY > 200; Index++) {
    TimeWindowY = (UINT32) (PackageRaplTimeWindow >> Index);
    TWY = Index;
    DEBUG ((EFI_D_INFO, "[Time2TAU]: Index %d, TW = 0x%08x%08x\n", Index, TimeWindowY));
  }

  //
  // Variable X is programmed to the closest value.
  // (1 -> 0, 1.25 -> 1, 1.5->2, 1.75-> 3)
  //
  if ((TimeWindowY >= 100) && (TimeWindowY <= 112)) {
    TWX = 0;
  } else if (TimeWindowY >= 113 && TimeWindowY <= 137) {
    TWX = 1;
  } else if (TimeWindowY >= 138 && TimeWindowY <= 162) {
    TWX = 2;
  } else if (TimeWindowY >= 163 && TimeWindowY <= 200) {
    TWX = 3;
  } else {
	DEBUG ((EFI_D_INFO, "[Time2TAU]: Package Rapl - Invalid Reminder for TWX = %d. Range Value should be between 100 and 200\n", TimeWindowY));
  }
  DEBUG ((EFI_D_INFO, "[Time2TAU]: Package Rapl - TWX = %d, TWY = %d\n", TWX, TWY));

  return ((UINT8) ((TWX << 5) | (TWY)));
}
