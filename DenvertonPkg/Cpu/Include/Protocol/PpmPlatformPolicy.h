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

  PpmPlatformPolicy.h

Abstract:

  Interface definition details between PPM and platform drivers during DXE phase.

--*/
#ifndef _PPM_PLATFORM_POLICY_H_
#define _PPM_PLATFORM_POLICY_H_

//
//  PPM policy provided by platform for DXE phase {DDABFEAC-EF63-452c-8F39-ED7FAED8265E}
//
#define PPM_PLATFORM_POLICY_PROTOCOL_GUID \
  {0xddabfeac, 0xef63, 0x452c, 0x8f, 0x39, 0xed, 0x7f, 0xae, 0xd8, 0x26, 0x5e}

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gPpmPlatformPolicyProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PPM_PLATFORM_POLICY_PROTOCOL PPM_PLATFORM_POLICY_PROTOCOL;

//
// Protocol revision number
// Any backwards compatible changes to this protocol will result in an update in the revision number
// Major changes will require publication of a new protocol
//
// Revision 1: Original version
// Revision 2: Added T-states field to the PPM_FUNCTION_ENABLES structure, Renamed unused fields - CxPopUpEnable, CxPopDownEnable, FastC4ExitEnable 
// Revision 3: Extended VidCpuid to 32 bits for extended CPUID support (Penryn)
// Revision 4: Added support for extended C6 residency enabling
//
#define PPM_PLATFORM_POLICY_PROTOCOL_REVISION     1
#define PPM_PLATFORM_POLICY_PROTOCOL_REVISION_2   2
#define PPM_PLATFORM_POLICY_PROTOCOL_REVISION_3   3
#define PPM_PLATFORM_POLICY_PROTOCOL_REVISION_4   4

//
// Define maximum number of custom VID states supported
//
#ifndef MAX_CUSTOM_VID_TABLE_STATES
 #define MAX_CUSTOM_VID_TABLE_STATES               6
#endif
//
// Custom VID table
//
typedef struct {
  UINT8   VidNumber;
  UINT32  VidCpuid;
  UINT16  VidMaxRatio;
  UINT16  VidMaxVid;
  UINT16  StateRatio[MAX_CUSTOM_VID_TABLE_STATES];
  UINT16  StateVid[MAX_CUSTOM_VID_TABLE_STATES];
} PPM_CUSTOM_VID_TABLE;

//
// PPM functional enables
//
typedef struct {
  UINT8   EnableGv                   :1; // 0: Disabled; 1: Enabled
  UINT8   EnableCx                   :1;
  UINT8   EnableCxe                  :1;
  UINT8   EnableC3                   :1;		
  UINT8   EnableC6                   :1;
  UINT8   EnableC7                   :1;
  UINT8   EnableC8                   :1;
  UINT8   EnableC9                   :1;
  UINT8   EnableC10                  :1;
  UINT8   C1AutoDemotion             :1;
  UINT8   C3AutoDemotion             :1;
  UINT8   C1UnDemotion               :1;
  UINT8   C3UnDemotion               :1;
  UINT8   PkgCStateDemotion          :1;
  UINT8   PkgCStateUnDemotion        :1;
  UINT8   CStatePreWake              :1;
  UINT8   EnableTm                   :1;
  UINT8   EnableEmttm                :1;
  UINT8   EnableTurboMode            :1;
  UINT8   PowerLimit2                :1;
  UINT8   EnableProcHot              :1;
  UINT8   EnableCMP                  :1;
  UINT8   TStatesEnable              :1;
  UINT8   S0ixSupport                :1;
  UINT8   EnableDumpCrashLog         :1;
  UINT8   Reserved2                  ;
  
} PPM_FUNCTION_ENABLES;


//
// PPM Turbo settings
//
typedef struct _PPM_TURBO_SETTINGS {
  UINT16  PowerLimit1;
  UINT32  PowerLimit1Time;
  UINT16  PowerLimit2;
  UINT8   TurboPowerLimitLock;
} PPM_TURBO_SETTINGS;
//
// SMI functions
//

//
// Platform Policy
//
typedef struct _PPM_PLATFORM_POLICY_PROTOCOL {
  UINT8                                 Revision;
  PPM_FUNCTION_ENABLES                  FunctionEnables;
  PPM_CUSTOM_VID_TABLE                  CustomVidTable;
  PPM_TURBO_SETTINGS                    TurboSettings;
  UINT8                                 PowerStateSwitchSmiNumber;
  UINT8                                 EnableCStateIoRedirectionSmiNumber;
  UINT8                                 DisableCStateIoRedirectionSmiNumber;
/*********************************************************************************/ 
  UINT8                                 EnableSmiCStateCoordinationSmiNumber;
  UINT8                                 DisableSmiCStateCoordinationSmiNumber;
/*********************************************************************************/
  UINT8                                 EnablePStateHardwareCoordinationSmiNumber;
  UINT8                                 DisablePStateHardwareCoordinationSmiNumber;
  UINT8                                 S3RestoreMsrSwSmiNumber;
  UINT8                                 BootInLfm;
  UINT32                                FlexRatioVid;
  UINT8                                 EnableEnableC6ResidencySmiNumber;
  //
  // Primary and Secondary Plane Current Limits
  //
  UINT16                                PrimaryPlaneCurrentLimit;
  UINT8                                 PkgCStateLimit;
  UINT8                                 MaxCState;
  UINT8                                 L1Prefetcher;
  UINT8                                 L2Prefetcher;
  UINT16                                PackageRaplPowerLevel1;
  UINT16                                PackageRaplPowerLevel2;
  UINT32                                PackageRaplTimeWindow1;           // Package Rapl Power Limit 1 MSR 0x610 PKG_RAPL_LIMIT value
  UINT32                                PkgPowerSkuUnit;                  // MSR 0x606 PKG_POWER_SKU_UNIT value
  UINT8                                 RatioLimitsEnable;                // Control to Enable/Disable the configuration of the Ratio Limits MSRs
  UINT8                                 RatioLimit0;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 7:0
  UINT8                                 RatioLimit1;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 15:8
  UINT8                                 RatioLimit2;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 23:16
  UINT8                                 RatioLimit3;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 31:24
  UINT8                                 RatioLimit4;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 39:32
  UINT8                                 RatioLimit5;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 47:40
  UINT8                                 RatioLimit6;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 55:48
  UINT8                                 RatioLimit7;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 63:56
  UINT8                                 CoreCount0;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 7:0
  UINT8                                 CoreCount1;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 15:8
  UINT8                                 CoreCount2;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 23:16
  UINT8                                 CoreCount3;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 31:24
  UINT8                                 CoreCount4;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 39:32
  UINT8                                 CoreCount5;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 47:40
  UINT8                                 CoreCount6;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 55:48
  UINT8                                 CoreCount7;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 63:56
  UINT8                                 PkgRaplLimitLock;                 // Setup option to LOCK/UNLOCK MSR 0x610 - bit 63
  UINT8                                 TjTargetOffset;                   // Setup option to configure TJ_MAX_TCC_OFFSET at TEMPERATURE_TARGET (1A2h)
  UINT8                                 TcontrolOffset;                   // Setup option to configure TCONTROL_OFFSET
  UINT8                                 TcontrolOffsetSign;               // Setup option to configure the sign for the TCONTROL_OFFSET value
  UINT8                                 EnableDumpCrashLog;               // Setup option to enable getting crash log from Punit
  UINT8                                 CoreBGF;                          // Setup option to send info to Punit
  UINT8                                 BiosRequestFreq;                  // BIOS Request Frequency
  UINT8                                 MfgMode;                          // Manufacturing Mode Indicator
  UINT8                                 HighTempInterrupt;
  UINT8                                 LowTempInterrupt;
  UINT8                                 ProcHotInterruptEnable;
  UINT8                                 OutOfSpecInterruptEnable;         // Setup option to enable an interrupt on the transition to the OutOfSpecThreshold condition
  UINT8                                 TempThreshold1;
  UINT8                                 Threshold1InterruptEnable;
  UINT8                                 TempThreshold2;
  UINT8                                 Threshold2InterruptEnable;
  //
  // DENVERTON PM features for EPO
  //
  UINT8                                 ProchotResponse;
  UINT8                                 ProchotOutputMode;
  UINT8                                 VrThermAlertDisable;
  UINT8                                 LockThermInt;
  UINT8                                 ProchotFreqResponse;
  //
  // PL3/PL4 Support
  //
  UINT16                                PL3ControlPowerLevel;             // PL3_CONTROL_MCHBAR_POWER_LIMIT (14:0)
  UINT8                                 PL3ControlEnable;                 // PL3_CONTROL_MCHBAR_PL3_ENABLE (15)
  UINT8                                 PL3ControlTimeWindow;             // PL3_CONTROL_MCHBAR_TIME_WINDOW (23:17)
  UINT8                                 PL3ControlDutyCycle;              // PL3_CONTROL_MCHBAR_DUTY_CYCLE (bits 30:24)
  UINT16                                PL3ControlPmaxPower;              // PL3_CONTROL_MCHBAR_PMAX_POWER (47:32)
  UINT8                                 PL3ControlLock;                   // PL3_CONTROL_MCHBAR_PWR_LIM_LOCK (bit 63)
} PPM_PLATFORM_POLICY_PROTOCOL;

#endif
