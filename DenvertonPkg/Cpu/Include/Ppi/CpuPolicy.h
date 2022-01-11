/** @file
  CPU policy PPI produced by a platform driver specifying various
  expected CPU settings. This PPI is consumed by CPU PEI modules.

@copyright
 Copyright (c) 2009 - 2015 Intel Corporation. All rights reserved
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

@par Specification
**/
#ifndef CPU_POLICY_H_
#define CPU_POLICY_H_

#include "BiosGuardDefinitions.h"
#include <Library/PeiDxeSmmBootGuardLibrary.h>

extern EFI_GUID gSiCpuPolicyPpiGuid;

#pragma pack(1)

typedef struct _SI_CPU_POLICY_PPI SI_CPU_POLICY_PPI;

//
// SI_CPU_POLICY_PPI revisions
//
#define SI_CPU_POLICY_PPI_REVISION_1  1

#define CPU_FEATURE_DISABLE 0
#define CPU_FEATURE_ENABLE  1
#define MAX_NUM_VRS         5

///
/// Define maximum number of custom ratio states supported
///
#define MAX_CUSTOM_RATIO_TABLE_ENTRIES 16

///
/// Define maximum number of custom ConfigTdp entries supported
///
#define MAX_CUSTOM_CTDP_ENTRIES 3

//
// TXT configuration policy
//
#define TXT_SINIT_MEMORY_SIZE         0x30000
#define TXT_HEAP_MEMORY_SIZE          0xE0000
#define TXT_DPR_MEMORY_SIZE           0x400000
#define TXT_DEVICE_MEMORY_SIZE        (TXT_SINIT_MEMORY_SIZE + TXT_HEAP_MEMORY_SIZE)

#define TXT_LCP_PD_BASE               0x0     ///< Platform default LCP
#define TXT_LCP_PD_SIZE               0x0     ///< Platform default LCP
#define TXT_TGA_MEMORY_SIZE           0x0     ///< Maximum 512K of TGA memory (aperture)
#define TXT_TGTT_MEMORY_SIZE          0x80000 ///< 512K of TGTT memory
#define TXT_TGR_MEMORY_SIZE           0x80000 ///< 512K of TGR memory (Registers)
#define TXT_TOTAL_STOLEN_MEMORY_SIZE  (TXT_DPR_MEMORY_SIZE)

///
/// PPM Package C State Limit
///
typedef enum {
  PkgC0C1                 = 0,
  PkgC2,
  PkgC3,
  PkgC6,
  PkgC7,
  PkgC7s,
  PkgC8,
  PkgC9,
  PkgC10,
  PkgCMax,
  PkgCpuDefault = 254,
  PkgAuto = 255
} MAX_PKG_C_STATE;

///
/// PPM Package C State Time Limit
///
typedef enum {
  TimeUnit1ns             = 0,
  TimeUnit32ns,
  TimeUnit1024ns,
  TimeUnit32768ns,
  TimeUnit1048576ns,
  TimeUnit33554432ns,
  TimeUnitMax
} C_STATE_TIME_UNIT;

///
/// Custom Power Uints.User can choose to enter in MilliWatts or Watts
///
typedef enum {
  PowerUnitWatts = 0,       ///< in Watts
  PowerUnit125MilliWatts,   ///< in 125 Milli Watts. Example 11.250 W Value to use for Power limts 90
  PowerUnitMax
} CUSTOM_POWER_UNIT;

///
/// PL1 thermal throttling floor units
///
typedef enum {
  Percent5  = 242,
  Percent10 = 229,
  Percent15 = 217,
  Percent20 = 204,
  Percent25 = 191,
  Percent30 = 178,
  Percent35 = 166,
  Percent40 = 153,
  Percent45 = 140,
  Percent50 = 127,
  Percent55 = 115,
  Percent60 = 102,
  Percent65 = 89,
  Percent70 = 76,
  Percent75 = 64,
  Percent80 = 51,
  Percent85 = 38,
  Percent90 = 25,
  Percent95 = 13,
  Percent100 = 0
} PL1_THERMAL_THROTTLE_FLOOR_UNIT;

///
/// Enums for EC Command Type
///
typedef enum {
  SendEcCmds = 1,
  SendEcValue,
  ReceiveEcValue,
  EcCmdMaximum
} EC_COMMAND_TYPE;

///
/// PPM Interrupt Redirection Mode Selection
///
typedef enum {
  PpmIrmFixedPriority     = 0,
  PpmIrmRoundRobin,
  PpmIrmHashVector,
  PpmIrmReserved1,
  PpmIrmPairFixedPriority,
  PpmIrmPairRoundRobin,
  PpmIrmPairHashVector,
  PpmIrmNoChange
} PPM_IRM_SETTING;

/**
  This function is for platform code to provide Microcode location since different BIOS has different flash layout.
  Platform code need to provide a function for CPU code to call to get the Microcode location in flash or memory.
**/
typedef
VOID*
(EFIAPI *PLATFORM_CPU_RETRIEVE_MICROCODE)(
  IN SI_CPU_POLICY_PPI           *This,
  IN UINT8                       **MicrocodeData
  );

/**
  This function is for platform code to provide EC Commands since different BIOS might have different EC.
  Platform code need to provide a function for CPU code to call to communicate with EC.

  @param[in] This           - Driver context.
  @param[in] EcCmdType      - EC Command Type.
  @param[in] EcCmd          - EC Command Byte to send.
  @param[in] SendData       - EC Data Byte to send.
  @param[in] ReceiveData    - EC Data Byte received.

  @retval EFI_SUCCESS          Command Read/ Write Success
  @retval EFI_DEVICE_ERROR     Command Read/ Write Error
  @retval EFI_OUT_OF_RESOURCES No enough resoruces (such as out of memory).
**/
typedef
EFI_STATUS
(EFIAPI *PLATFORM_SEND_EC_COMMAND)(
  IN SI_CPU_POLICY_PPI           *This,
  IN EC_COMMAND_TYPE             EcCmdType,
  IN UINT8                       EcCmd,
  IN UINT8                       SendData,
  IN OUT UINT8                   *ReceiveData
  );

///
/// This structure is used to describe the custom processor ratio table desired by the platform
///
typedef struct {
  UINT16 MaxRatio;                                   ///< The maximum ratio of the custom ratio table.
  ///
  /// The number of custom ratio state entries, it must be a value from 2 to 16 for a valid custom ratio table.
  ///
  UINT8  NumberOfEntries;
  UINT8  Reserved;                                   ///< Reserved for future use.
  UINT32 Cpuid;                                      ///< The CPU ID for which this custom ratio table applies.
  UINT16 StateRatio[MAX_CUSTOM_RATIO_TABLE_ENTRIES]; ///< The processor ratios in the custom ratio table.
#if (MAX_CUSTOM_RATIO_TABLE_ENTRIES % 2)
  UINT16 Reserved2;                                  ///< If there is an odd number of array entries, add padding for dword alignment.
#endif
} PPM_CUSTOM_RATIO_TABLE;

/**
  This structure is used to describe which of the PPM functions will be enabled by PPM implementation.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  UINT32 Eist                  : 1; ///< <b>(Optional)</b> Enable or Disable Intel SpeedStep Technology. 0: Disable; <b>1: Enable</b>
  UINT32 Cx                    : 1; ///< <b>(Optional)</b> Enable or Disable CPU power states (C-states). 0: Disable; <b>1: Enable</b>
  UINT32 C1e                   : 1; ///< <b>(Optional)</b> Enable or Disable Enhanced C-states. 0: Disable; <b>1: Enable</b>
  UINT32 C1AutoDemotion        : 1; ///< <b>(Optional)</b> Enable or Disable C6/C7 auto demotion to C1. 0: Disabled; <b>1: C1 Auto demotion</b>;
  UINT32 C3AutoDemotion        : 1; ///< <b>(Optional)</b> Enable or Disable C6/C7 auto demotion to C3  0: Disabled; <b>1: C3 Auto demotion</b>;
  UINT32 TurboMode             : 1; ///< <b>(Optional)</b> Enable or Disable long duration Turbo Mode. Disable; <b>1: Enable</b>
  UINT32 PowerLimit2           : 1; ///< <b>(Required)</b> Enable or Disable short duration Turbo Mode. Disable; <b>1: Enable</b>
  UINT32 EnergyEfficientPState : 1; ///< <b>(Optional)</b> Enable or Disable Energy Efficient P-state will be applied in Turbo mode. Disable; <b>1: Enable</b>
  UINT32 Xe                    : 1; ///< <b>(Optional)</b> Enable or Disable Intel Extreme Edition support. <b>Disable</b>; 1: Enable
  UINT32 C1UnDemotion          : 1; ///< <b>(Optional)</b> Enable or Disable C1UnDemotion. 0: Disabled; <b>1: C1 Auto undemotion</b>;
  UINT32 C3UnDemotion          : 1; ///< <b>(Optional)</b> Enable or Disable C3UnDemotion. 0: Disabled; <b>1: C3 Auto undemotion</b>;
  UINT32 PkgCStateDemotion     : 1; ///< <b>(Optional)</b> Enable or Disable Package Cstate Demotion. <b>Disable</b>; 1: Enable
  UINT32 PkgCStateUnDemotion   : 1; ///< <b>(Optional)</b> Enable or Disable Package Cstate UnDemotion. <b>Disable</b>; 1: Enable
  UINT32 CStatePreWake         : 1; ///< <b>(Optional)</b> Enable or Disable CState-Pre wake. Disable; <b>1: Enable</b>
  UINT32 LakeTiny              : 1; ///< <b>(Required)</b> Enable or Disable LakeTiny Support. <b>Disable</b>; 1: Enable
  UINT32 TimedMwait            : 1; ///< <b>(Optional)</b> Enable or Disable TimedMwait Support. <b>Disable</b>; 1: Enable
  UINT32 Hwp                   : 1; ///< <b>(Required)</b> Enable or Disable HWP(Hardware P states) Support.
  UINT32 PlatformPowerLimit1   : 1; ///< <b>(Optional)</b> MSR 0x65C[15]: PL1 Enable activates the PL1 value to limit average platform power
  UINT32 PlatformPowerLimit2   : 1; ///< <b>(Optional)</b> MSR 0x65C[47]: PL2 Enable activates the PL2 value to limit average platform power
  UINT32 Reserved              : 13; ///< Bits reserved for future use.
} PPM_FUNCTION_ENABLES;

///
/// This structure is used to describe various PPM turbo settings
///
typedef struct _PPM_TURBO_SETTINGS {
  UINT8  TurboPowerLimitLock : 1; ///< <b>(Optional)</b> MSR 0x610[63] and 0x618[63]: Lock all Turbo settings. <b>0: Disable</b>; 1: Enable.
  UINT8  PowerLimit3Lock   : 1; ///< Package PL3 MSR 615h lock.
  UINT8  PowerLimit4Lock   : 1; ///< Package PL4 MSR 601h lock.
  ///
  /// Configurable TDP Mode Lock can be set to lock the ConfigTdp mode settings from runtime changes.
  ///
  UINT8  ConfigTdpLock : 1;
  UINT8  ConfigTdpBios : 1; ///< Configuration whether load Configurable TDP SSDT.
  ///
  /// Configuration for boot TDP selection, value 0 describes TDP Nominal, value 1 describes TDP Down and
  /// value 2 describes TDP Up.
  ///
  UINT8  ConfigTdpLevel;
  UINT8  PowerLimit3DutyCycle;  ///< Package PL3 Duty Cycle.
  UINT8  PlatformPowerLimit1Time;     ///<<b>(Optional)</b> MSR 0x65C[23:17]: PL1 timewindow in seconds

  UINT16 PowerLimit1;     ///< Package Long duration turbo mode power limit in 125mw or watts.
  UINT16 PowerLimit2;     ///< Package Short duration turbo mode power limit in 125mw or watts.
  UINT16 PowerLimit3;           ///< Package PL3 power limit in 125mw or watts.
  UINT16 PowerLimit4;           ///< Package PL4 power limit in 125mw or watts.
  UINT16 PlatformPowerLimit1Power;    ///<<b>(Optional)</b> MSR 0x65C[14:0]: Platform PL1 power in watts
  UINT16 PlatformPowerLimit2Power;    ///<<b>(Optional)</b> MSR 0x65C[46:32]]: Platform PL2 power in watts
  UINT32 PowerLimit1Time; ///< Package Long duration turbo mode time window in seconds.
  UINT32 PowerLimit3Time;       ///< Package PL3 time window in seconds.
} PPM_TURBO_SETTINGS;

///
/// PPM Custom ConfigTdp Settings
///
typedef struct _PPM_CUSTOM_CTDP_TABLE {
  UINT8  CustomConfigTdpControl : 2; ///< Config Tdp Control (0/1/2) value for custom cTDP level.
  UINT8  CustomPowerLimit1Time;      ///< Short term Power Limit time window value for custom cTDP level.
  UINT8  CustomTurboActivationRatio; ///< Turbo Activation Ratio for custom cTDP level.
  UINT16 CustomPowerLimit1;          ///< Short term Power Limit value for custom cTDP level in 125mw or watts.
  UINT16 CustomPowerLimit2;          ///< Long term Power Limit value for custom cTDP level in 125mw or watts.
} PPM_CUSTOM_CTDP_TABLE;

///
/// This structure is used to configure custom ConfigTdp level settings.
///
typedef struct _PPM_CUSTOM_CTDP {
  UINT8                 ConfigTdpCustom : 1; ///< Describes whether or not Custom Config Tdp should be enabled.
  UINT8                 CustomTdpCount  : 2; ///< Describes the number of Custom Config Tdp levels required (1/2/3).
  UINT8                 CustomBootModeIndex : 2; ///< Describes the Boot mode index selection from Custom Tdp levels.
  ///
  /// Describes the details of each Custom Config Tdp levels. This supports up to MAX_CUSTOM_CTDP_ENTRIES number
  /// of Custom ConfigTdp levels.
  ///
  PPM_CUSTOM_CTDP_TABLE CustomConfigTdpTable[MAX_CUSTOM_CTDP_ENTRIES];
} PPM_CUSTOM_CTDP;

///
/// This structure is used to control enabled / disabled various PPM MSR lock settings
///
typedef struct _PPM_LOCK_ENABLES {
  UINT32 PmgCstCfgCtrlLock : 1;   ///< Setting this to 1 will set MSR 0xE2[15]
  UINT32 OverclockingLock  : 1;   ///< Setting this to 1 will set MSR 0x194[20]
  UINT32 ProcHotLock       : 1;   ///< Setting this to 1 will set MSR 0x1FC[23]
  UINT32 Reserved          : 29;  ///< Bits reserved for future use.
} PPM_LOCK_ENABLES;

///
/// This structure is used to describe the unit of which PL1 Thermal control floor.
///
typedef struct {
    PL1_THERMAL_THROTTLE_FLOOR_UNIT FloorIA;  ///< FLOOR_IA, Default: 0 (Percent100)
    PL1_THERMAL_THROTTLE_FLOOR_UNIT FloorGT;  ///< FLOOR_GT, Default: 0 (Percent100)
    PL1_THERMAL_THROTTLE_FLOOR_UNIT FloorPCH; ///< FLOOR_PCH, Default: 0 (Percent100)
} PL1_THERMAL_CONTROL_FLOOR;

///
/// This structure is used to describe which of the Thermal functions will be enabled by Thermal implementation.
///
typedef struct {
  UINT16 TStates               : 1;                  ///< Enable or Disable T states. <b>Disable</b>; 1: Enable
  UINT16 DisableProcHotOut     : 1;                  ///< Enable or Disable PROCHOT# signal being driven externally. <b>Disable</b>; 1: Enable
  UINT16 DisableVRThermalAlert : 1;                  ///< Enable or Disable VR Thermal Alert. <b>Disable</b>; 1: Enable
  UINT16 ProcHotResponse       : 1;                  ///< Enable or Disable PROCHOT# Responce. <b>Disable</b>; 1: Enable
  UINT16 AutoThermalReporting  : 1;                  ///< Enable or Disable Thermal Reporting through ACPI tables. Disable; <b>1: Enable</b>
  UINT16 ThermalMonitor        : 1;                  ///< Enable or Disable Thermal Monitor. Disable; <b>1: Enable</b>
  UINT16 Pl1ThermalControl     : 2;                  ///< PL1 thermal throttling features. 0: Disable; 1: Enable/Manual; <b>2: Auto</b>
  UINT16 ThermReserved         : 7;                  ///< Reserved
  PL1_THERMAL_CONTROL_FLOOR Pl1ThermalControlFloor;  ///< PL1 Floor Throttle Values
} THERM_FUNCTION_ENABLES;

/**
  Platform Policies for CPU features configration Platform code can enable/disable/configure features through this structure.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization.
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT8 BistOnReset           : 1; ///< <b>(Optional)</b> Enable or Disable BIST on Reset; <b>0: Disable</b>; 1 Enable.
  UINT8 HyperThreading        : 1; ///< <b>(Optional)</b> Enable or Disable Hyper Threading; 0: Disable; <b>1: Enable</b>.
  UINT8 CpuRatioOverride      : 1; ///< <b>(Optional)</b> Enable or Disable CPU Radio Override; <b>0: Disable</b>; 1: Enable.
  UINT8 VmxEnable             : 1; ///< <b>(Optional)</b> Enable or Disable VMX; 0: Disable; <b>1:Enable</b>.
  UINT8 BiosGuard             : 1; ///< <b>(Required)</b> Enable or Disable BIOS Guard; <b>0: Disable</b>; 1: Enable.
  UINT8 MlcStreamerPrefetcher : 1; ///< <b>(Optional)</b> Enable or Disable MLC Streamer Prefetcher; 0: Disable; <b>1: Enable</b>.
  UINT8 MlcSpatialPrefetcher  : 1; ///< <b>(Optional)</b> Enable or Disable MLC Spatial Prefetcher; 0: Disable; <b>1: Enable</b>.
  UINT8 Txt                   : 1; ///< <b>(Required)</b> Enable or Disable TXT; <b>0: Disable</b>; 1: Enable.

  //
  // Byte 1, bit definition for functionality enable/disable
  //
  UINT8 SmxEnable                : 1; ///< <b>(Optional)</b> Enable or Disable Secure Mode Extensions feature; 0: Disable; <b>1: Enable</b>
  UINT8 MonitorMwaitEnable       : 1; ///< <b>(Optional)</b> Enable or Disable Monitor /MWAIT instructions; 0: Disable; <b>1: Enable</b>
  UINT8 MachineCheckEnable       : 1; ///< <b>(Optional)</b> Enable or Disable initialization of machine check registers; 0: Disable; <b>1: Enable</b>
  UINT8 AesEnable                : 1; ///< <b>(Optional)</b> Enable or Disable Advanced Encryption Standard (AES) feature; 0: Disable; <b>1: Enable</b>
  UINT8 DebugInterfaceEnable     : 1; ///< <b>(Optional)</b> Enable or Disable processor debug features; <b>0: Disable</b>; 1: Enable.
  UINT8 DebugInterfaceLockEnable : 1; ///< <b>(Optional)</b> Lock or Unlock debug interface features; 0: Debug features can be still enabled or disabled; <b>1: Locks any changes to enabling or disabling debug features</b>
  /**
   Policies to obtain CPU temperature.
   - <b>0: ACPI thermal management uses EC reported temperature values.</b>
   - 1: ACPI thermal management uses DTS SMM mechanism to obtain CPU temperature values.
   - 2: ACPI Thermal Management uses EC reported temperature values and DTS SMM is used to handle Out of Spec condition.
   @note This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization.
  **/
  UINT8 EnableDts                : 2;

  //
  // Byte 2, bit definition for functionality enable/disable
  //
  UINT8 FviReport                : 1; ///< <b>(Optional)</b> Control Firmware version Information reporting; 0: Do not report FW version to SMBIOS table; <b>1: Report FW version to SMBIOS table</b>
  /**
   AP Idle Manner of waiting for SIPI.
   - <b>1: HALT loop.</b>
   - 2: MWAIT loop.
   - 3: RUN loop.
   @note The policy is recommended for validation purpose only.
  **/
  UINT8 ApIdleManner             : 2;
  UINT8 ApHandoffManner          : 2; ///< Settings for AP Handoff to OS; <b>1: HALT loop</b>; 2: MWAIT loop.
  UINT8 ActiveCoreCount          : 3; ///< <b>(Optional)</b> Number of active cores.
  UINT8    ProcTraceOutputScheme  : 1; ///< <b>(Optional)</b> control on Processor Trace output scheme; <b>0 Single Range Output</b>; 1: ToPA Output.

  UINT8                           FClkFrequency : 2; ///< <b>(Optional)</b> Processor Early Power On Configuration FCLK setting.
  UINT8                           JtagC10PowerGateDisable : 1; ///< <b>(Optional)</b> 0: Power Down JTAG  in C10 and deeper; 1: Power JTAG  in C10 and deeper for debug.

  UINT8    ProcTraceEnable        : 2; ///< <b>(Optional)</b> Enable or Disable Processor Trace feature; <b>0 Disable</b>; 1: Enable.
  UINT8   SvLtEnable               : 1;
  UINT8   McThreeStrike            : 1;  /// 0: Disable; 1: Enable (default)
  UINT8   BiosGuardModuleSelection : 1;  ///< Bios Guard Module selection; <b> 0: CRB module</b>; 1: SV module.
  UINT8   SvRsvBits                : 5;
  EFI_PHYSICAL_ADDRESS             *PatchAtResetRevision;         ///< Microcode patch revision at reset

#ifdef LPT_FLAG
  //
  // CPU Misc Config
  //
  UINT8 FviSmbiosType;                ///< <b>(Required)</b> Create SMBIOS Table Type for Firmware Version Information
#endif // LPT_FLAG

  UINT8 CpuRatio;                     ///< <b>(Optional)</b> CPU ratio value.
  UINT8 CpuMaxNonTurboRatio;          ///< <b>(Optional)</b> CPU Maximum Non-Turbo ratio value.
  UINT8 SmmbaseSwSmiNumber;           ///< <b>(Required)</b> Software SMI Number from Smbase.
  UINT8    ProcTraceMemSize;           ///< <b>(Optional)</b> Memory region allocation for Processor Trace, 0xFF means Disable.
  EFI_PHYSICAL_ADDRESS            RetrieveMicrocode; ///< <b>(Required)</b> Platform code can provide microcode location through this function.
  EFI_PHYSICAL_ADDRESS            SendEcCmd; ///< <b>(Required)</b> Platform code can provide interface to communicate with EC through this function.
} CPU_CONFIG;

/**
  Power management Configuration for all Processor Power Management Features
  Configs are in this field. Platform code can enable/disable features through this field.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  /**
  Boot performance mode.
   - <b>0: Maximum non-turbo performance</b>
   - 1:Maximum battery performance.
   - 2:Turbo performance.
  @note The policy is recommended for validation purpose only.
  **/
  UINT8  BootInLfm : 2;
  UINT8  VrCurrentLimitLock     : 1; ///< <b>(Optional)</b> MSR 0x601[31]: when set, locks bits 31:0 of this MSR.
  UINT8  Xe                     : 1; ///< <b>(Optional)</b> Intel Extreme Edition support; 0: Disable; <b>1: Enable</b>.
  UINT8  VrMiscIdleExitRampRate     : 1; ///< <b>(Optional)</b> MSR 0x603[50]: Controls the VR ramp rate on package C-state wake events.
  UINT8  VrMiscIdleEntryRampRate    : 1; ///< <b>(Optional)</b> MSR 0x603[51]: Controls the VR ramp rate on package C-state entry events.
  UINT8  VrMiscIdleEntryDecayEnable : 1; ///< <b>(Optional)</b> MSR 0x603[52]: Enables decay mode on package C-state entry.
  UINT8  VrMiscSlowSlewRateConfig   : 2; ///< <b>(Optional)</b> MSR 0x53[54:53]: Slow Slew rate configuration.
  UINT8  VrMiscIoutOffsetSign       : 1; ///< <b>(Optional)</b> IOUT OFFSET Sign. 0 = positive offset. 1= negative offset.
  UINT8  VrPSI4enable               : 1; ///< <b>(Optional)</b> MSR 0x601[62]: PSI4 Enabled/Disabled bit in VR_CURRENT_CONFIG, it is only applied to ULT.
  /**
  24MHz BCLK support calibration.
   - 0: NO_CALIBRATE
   - <b>1: PCODE_CALIBRATE</b>
   - 2: BIOS_CALIBRATE
  **/
  UINT8                  PcodeCalibration            : 2;
  UINT8                  EnableRerunPcodeCalibration : 1; ///< Calibrate C state 24MHz BCLK support.
  UINT8                  RfiFreqTunningOffsetIsNegative : 1; ///< Specify RfiFreqTunningOffset is Positive or Negative.
  UINT8                  RfiFreqTunningOffset;           ///< Specify the Target FIVR Frequency offset.
  UINT8 PmgCstCfgCtrIoMwaitRedirection    : 1; ///< <b>(Optional)</b> Enable or Disable IO to MWAIT redirection. <b>Disable</b>; 1: Enable
  UINT8 Reserved                          : 7; ///< Bits reserved for future use.

  UINT8                  HdcOsControl        : 1; ///< Set OS Control Enabled for MISC_PWR_MGMT MSR 0x1AA[10].            0: Disable.; <b>1: Enable</b>
  UINT8                  HdcBiosControl      : 1; ///< Set Bios Control Enabled for MISC_PWR_MGMT MSR 0x1AA[9].           <b>0: Disable.</b>; 1: Enable
  UINT8                  HdcPackageEnable    : 1; ///< Set Package Enabled for IA32_HDC_PKG_CTL MSR 0xDB0[0].             0: Disable.; <b>1: Enable</b>
  UINT8                  HdcThreadEnable     : 1; ///< Set Thread Enabled for IA32_PM_CTL1 MSR 0xDB1[0].                  <b>0: Disable.</b>; 1: Enable
  UINT8                  HdcCxMonitor        : 3; ///< Set Monitor Coiunting Type for PKG_HDC_CONFIG_CTL MSR 0x652[2:0].  <b>0: No-counting.</b>; 1: PC2 only.; 2: PC3 and deeper.; 3: PC6 and deeper.;
                                                  ///                                                                     4: PC7 and deeper.; 5: PC8 and deeper.; 6: PC9 and deeper.; 7: PC10
  UINT8                  HdcMcnt             : 1; ///< Set Monitor Counter Active for PKG_HDC_CONFIG_CTL MSR 0x652[3].    <b>0: Increment.</b>; 1: No-Increment
  UINT16                 HdcMaxForceIdleDuration; ///< Set the Max Force Idle Duration Time for PKG_HDC_CONFIG_CTL MSR 0x652[10:5]
  UINT8                  HdcDirectControl;        ///< Set the Direct Control Percentage for  PKG_HDC_CONFIG_CTL MSR 0x652[18:12].
  UINT16                 HdcMinActiveTime;        ///< Set the Minimal Active Time for PKG_HDC_CONFIG_CTL MSR 0x652[26:19].

  /**
  - 1-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 1-Core Ratio Limit + OC Bins.
    - This 1-Core Ratio Limit Must be greater than or equal to 2-Core Ratio Limit, 3-Core Ratio Limit, 4-Core Ratio Limit.
  - 2-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 2-Core Ratio Limit + OC Bins.
    - This 2-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  - 3-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 3-Core Ratio Limit + OC Bins.
    - This 3-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  - 4-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 4-Core Ratio Limit + OC Bins.
    - This 4-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  @note The policy is recommended for validation purpose only.
  **/
  UINT8  RatioLimit[4];

  UINT8  VrMiscMinVid;               ///< <b>(Optional)</b> MSR 0x603[31:24]: Used to constrain the CPU's minimum voltage during package C-states.
  UINT8  VrMisc2FastRampVoltage;     ///< <b>(Optional)</b> MSR 0x636[7:0]: Update FAST_RAMP_VOLTAGE; must be 0 to 0xFF, resolution 10mV.
  UINT8  VrMisc2MinC8Voltage;        ///< <b>(Optional)</b> MSR 0x636[15:8]: Update MIN_C8_VOLTAGE; must be 0 to 0xFF, resolution 10mV.
  /**
  TCC Activation Offset. Offset from factory set TCC activation temperature at which the Thermal Control Circuit must be activated
  TCC will be activated at TCC Activation Temperature, in volts.
  @note The policy is recommended for validation purpose only.
  **/
  UINT8  TccActivationOffset;

  UINT8                  TccOffsetTimeWindow;            ///< <b>(Optional)</b> Tcc Offset Time Window value (0 ~ 128) for Runtime Average Temperature Limit (RATL)
  UINT8                  S3RestoreMsrSwSmiNumber;        ///< SW SMI number to restore the power Mgmt MSRs during S3 resume.
  /**
  MSR 0x601[12:0], Primary (IA) Current Limit, in 1/8 amp units, 0=Auto. Maximum instantaneous current allowed at
  any given time. Setting a value too far below the Iccmax specification for this platform/CPU might result
  in System hang, CATERR, BSOD depending on the platform configuration.
  @note The policy is recommended for validation purpose only.
  **/
  UINT16 VrCurrentLimit;

  UINT16 VrMiscIoutSlope;            ///< <b>(Optional)</b> MSR 0x603[49-40]: IOUT SLOPE. Default is 200.
  UINT16 VrMiscIoutOffset;           ///< <b>(Optional)</b> MSR 0x603[39-32]: IOUT_OFFSET. 0 = 0%, 625 = 6.25% (Range is +6.25% ~ -6.25%)
  UINT16                 CstateLatencyControl0Irtl;      ///< Interrupt Response Time Limit of LatencyContol0 MSR 0x60A[9:0].
  UINT16                 CstateLatencyControl1Irtl;      ///< Interrupt Response Time Limit of LatencyContol1 MSR 0x60B[9:0].
  UINT16                 CstateLatencyControl2Irtl;      ///< Interrupt Response Time Limit of LatencyContol2 MSR 0x60C[9:0].
  UINT16                 CstateLatencyControl3Irtl;      ///< Interrupt Response Time Limit of LatencyContol3 MSR 0x633[9:0].
  UINT16                 CstateLatencyControl4Irtl;      ///< Interrupt Response Time Limit of LatencyContol4 MSR 0x634[9:0].
  UINT16                 CstateLatencyControl5Irtl;      ///< Interrupt Response Time Limit of LatencyContol5 MSR 0x635[9:0].

  MAX_PKG_C_STATE        PkgCStateLimit;                 ///< This field is used to set the Max Pkg Cstate. Default set to Auto which limits the Max Pkg Cstate to deep C-state.
  C_STATE_TIME_UNIT      CstateLatencyControl0TimeUnit;  ///< TimeUnit for Latency Control0 MSR 0x60A[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl1TimeUnit;  ///< TimeUnit for Latency Control1 MSR 0x60B[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl2TimeUnit;  ///< TimeUnit for Latency Control2 MSR 0x60C[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl3TimeUnit;  ///< TimeUnit for Latency Control3 MSR 0x633[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl4TimeUnit;  ///< TimeUnit for Latency Control4 MSR 0x634[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl5TimeUnit;  ///< TimeUnit for Latency Control5 MSR 0x635[12:10].
  /**
  Power Management Custom Power Limit Unit in milli watts.
  Default power unit for following items changes to MilliWatts.
  - PPM_TURBO_SETTINGS.PowerLimit1
  - PPM_TURBO_SETTINGS.PowerLimit2
  - PPM_TURBO_SETTINGS.PowerLimit3
  - PPM_CUSTOM_CTDP_TABLE.CustomPowerLimit1
  - PPM_CUSTOM_CTDP_TABLE.CustomPowerLimit2
  **/
  CUSTOM_POWER_UNIT      CustomPowerUnit;

  EFI_PHYSICAL_ADDRESS   FunctionEnables;               ///< This points to PPM_FUNCTION_ENABLES structure describing PPM settings.
  EFI_PHYSICAL_ADDRESS   CustomRatioTable;              ///< This points to PPM_CUSTOM_RATIO_TABLE structure describing custom CPU frequency table.
  EFI_PHYSICAL_ADDRESS   TurboSettings;                 ///< This points to PPM_TURBO_SETTINGS structure describing long and short duration turbo settings.
  EFI_PHYSICAL_ADDRESS   RatioLimitPtr;                    ///< This field is a pointer to Ratio Limit.
  EFI_PHYSICAL_ADDRESS   PpmLockEnables;                ///< This field is a pointer to PPM_LOCK_ENABLES structure.
  EFI_PHYSICAL_ADDRESS   CustomCtdpSettings;            ///< This points to PPM_CUSTOM_CTDP structure is used to describe the custom config TDP settings.
  EFI_PHYSICAL_ADDRESS   ThermalFuncEnables;          ///< This points to THERM_FUNCTION_ENABLES structure is used to describe which of the thermal functions should be enabled.

#ifdef HSW_FLAG
  UINT8  Psi1Threshold;              ///< <b>(Optional)</b> MSR 0x601[41:32]: Current Threshold for PSI1_CODE specified in Amps.
  UINT8  Psi2Threshold;              ///< <b>(Optional)</b> MSR 0x601[51:42]: Current Threshold for PSI2_CODE specified in Amps.
  UINT8  Psi3Threshold;              ///< <b>(Optional)</b> MSR 0x601[61:52]: Current Threshold for PSI3_CODE specified in Amps.
  UINT8  FivrNotchPercent;           ///< <b>(Optional)</b> PCODE MMIO Mailbox: FIVR notch percentage value. 0 = 0%, 50 = 50% (Range is 0-50)
  UINT8  FivrSscEnable          : 1; ///< <b>(Optional)</b> Enable/Disable FIVR Spread Spectrum support
  UINT8  FivrSscPercent;             ///< <b>(Optional)</b> PCODE MMIO Mailbox: FIVR spread spectrum percentage value. 0 = 0%, 5 = 5% (Range is 0-6)
#else
  UINT8  PmonSlope;                   ///< <b>(Optional)</b> PCODE MMIO Mailbox: Platform Pmon slope correction. Range 0-2
  UINT8  PmonOffset;                  ///< <b>(Optional)</b> PCODE MMIO Mailbox: Platform Pmon offset correction
  UINT16 PsysPmax;                    ///< <b>(Optional)</b> PCODE MMIO Mailbox: Platform Power Pmax. 0-1024 A.
  UINT8  VrConfigEnable[MAX_NUM_VRS]; ///< <b>(Optional)</b> Enable/Disable BIOS configuration of VR. Hardware default values.
  UINT16 AcLoadline[MAX_NUM_VRS];     ///< <b>(Optional)</b> PCODE MMIO Mailbox: AcLoadline in mOhms. 0 = Auto
  UINT16 DcLoadline[MAX_NUM_VRS];     ///< <b>(Optional)</b> PCODE MMIO Mailbox: DcLoadline in mOhms. 0 = Auto
  UINT16 Psi1Threshold[MAX_NUM_VRS];  ///< <b>(Optional)</b> PCODE MMIO Mailbox: Power State 1 current cuttof in 1/4 Amp increments. Range is 0-128A
  UINT16 Psi2Threshold[MAX_NUM_VRS];  ///< <b>(Optional)</b> PCODE MMIO Mailbox: Power State 2 current cuttof in 1/4 Amp increments. Range is 0-128A
  UINT16 Psi3Threshold[MAX_NUM_VRS];  ///< <b>(Optional)</b> PCODE MMIO Mailbox: Power State 3 current cuttof in 1/4 Amp increments. Range is 0-128A
  UINT8  Psi3Enable[MAX_NUM_VRS];     ///< <b>(Optional)</b> PCODE MMIO Mailbox: Power State 3 enable/disable
  UINT8  Psi4Enable[MAX_NUM_VRS];     ///< <b>(Optional)</b> PCODE MMIO Mailbox: Power State 4 enable/disable
  UINT8  ImonSlope[MAX_NUM_VRS];      ///< <b>(Optional)</b> PCODE MMIO Mailbox: Imon slope correction. Specified in 1/100 increment values. Range is 0-2.
  UINT8  ImonOffset[MAX_NUM_VRS];     ///< <b>(Optional)</b> PCODE MMIO Mailbox: Imon offset correction. Range 0-255.
  UINT16 IccMax[MAX_NUM_VRS];         ///< <b>(Optional)</b> PCODE MMIO Mailbox: VR Icc Max limit. 0-255A in 1/4 A units. 400 = 100A
  UINT16 VrVoltageLimit[MAX_NUM_VRS]; ///< <b>(Optional)</b> PCODE MMIO Mailbox: VR Voltage Limit. Range is 0-8000mV.
  UINT16 TdcPowerLimit[MAX_NUM_VRS];  ///< <b>(Optional)</b> PCODE MMIO Mailbox: Thermal Design Current current limit.
  UINT8  TdcEnable[MAX_NUM_VRS];      ///< <b>(Optional)</b> PCODE MMIO Mailbox: Thermal Design Current enable/disable
  UINT8  TdcTimeWindow[MAX_NUM_VRS];  ///< <b>(Optional)</b> PCODE MMIO Mailbox: Thermal Design Current time window
  UINT8  TdcLock[MAX_NUM_VRS];        ///< <b>(Optional)</b> PCODE MMIO Mailbox: Thermal Design Current Lock. Prevents further writes.
#endif
} POWER_MGMT_CONFIG;

/**
  Platform policies for BIOS Guard Configuration for all processor security features configuration.
  Platform code can pass relevant configuration data through this structure.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  BGUP_HEADER   BgupHeader;                        ///< BIOS Guard update package header that will be packaged along with BIOS Guard script and update data.
  BGPDT         Bgpdt;                             ///< BIOS Guard Platform Data Table contains all the platform data that will be parsed by BIOS Guard module.
  UINT64        BgpdtHash[4];                      ///< Hash of the BGPDT that will be programmed to PLAT_FRMW_PROT_HASH_0/1/2/3 MSR.
  UINT8         BiosGuardMemSize;                  ///< BIOS Guard memory size in MB
  UINT8         EcCmdDiscovery;                    ///< EC Command discovery.
  UINT8         EcCmdProvisionEav;                 ///< EC Command Provision Eav.
  UINT8         EcCmdLock;                         ///< EC Command Lock.
  BIOSGUARD_LOG BiosGuardLog;                      ///< BIOS Guard log.
  UINT8         TotalFlashSize;                    ///< Total Flash Size on the system in MB
  UINT8         BiosSize;                          ///< Bios Size in MB
} BIOS_GUARD_CONFIG;


/**
  This structure describes various settings required for TXT configuration.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  UINT64               SinitMemorySize;   ///< Size of SINIT module if installed in flash part. Zero otherwise.
  UINT64               TxtHeapMemorySize; ///< Size of memory reserved for TXT Heap. This memory is used by MLE.
  EFI_PHYSICAL_ADDRESS TxtDprMemoryBase;  ///< Base address of DPR protected memory reserved for Intel TXT component.
  UINT64               TxtDprMemorySize;  ///< Size of DPR protected memory reserved for Intel TXT component.
  ///
  /// Base address of BIOS ACM in flash part. It can be passed through platform code for customization;
  /// Intel TXT reference code would skip searching the BIOS ACM in PEI firmware volume if the field is not zero.
  ///
  EFI_PHYSICAL_ADDRESS BiosAcmBase;       ///< Base address of BIOS ACM in system firmware.
  UINT64               BiosAcmSize;       ///< Size of ACM Binary.
  ///
  /// Base address of CPU micro code patch loaded into BSP. It can be passed through platform code for customization;
  /// Intel TXT reference code would skip searching the micro code path in PEI firmware volume if the field is not zero.
  ///
  EFI_PHYSICAL_ADDRESS McuUpdateDataAddr;
  UINT64               TgaSize;           ///< Size of Trusted Graphics Aperture if supported by chipset. For Cantiga must be 0.
  EFI_PHYSICAL_ADDRESS TxtLcpPdBase;      ///< Base address of Platform Default Launch Control Policy data if installed in flash part. Zero otherwise.
  UINT64               TxtLcpPdSize;      ///< Size of Platform Default Launch Control Policy data if installed in flash part. Zero otherwise.
} TXT_CONFIG;


/**
  SGX Platform Configuration
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  UINT8  EnableC6Dram       : 1;   ///< C6DRAM Memory, 0: C6DRAM Disabled, <b> 1: C6DRAM Enabled </b>
  /**
  SGX feature configuration, 0: SGX Disabled, 1: SGX Enabled,
  BIOS will load SGX patch and PRMRR Base set by BIOS,
  2: SGX Enabled, BIOS will load SGX patch and PRMRR Base set by
  user, 3: SGX Enabled, BIOS will not load the SGX patch but
  PRMRR base will be set by BIOS, 4: SGX Enabled, BIOS will not
  load the SGX patch but, PRMRR base will be set by user
  **/
  UINT8  EnableSgx          : 3;
  UINT8  EnableSgx_7A       : 1;   ///< SGX feature Enable configuration after patch load 0: Disable, 1:Enable
  UINT16 SelectiveEnableSgx;       ///< Let selectively choose threads that will load SGX
  UINT8  Reserved           : 3;   ///< Reserved for future use
  UINT64 PrmrrBase;                ///< PRMRR Base, will be available only if EnableSGX != 0
  UINT64 EpcOffset;                ///< EPC offset from PRMRR base, will be available only if EnableSGX == 5
  UINT64 EpcLength;                ///< EPC length inside PRM range, will be available only if EnableSGX == 5
  UINT32 PrmrrSize;                ///< PRMRR Size, will be available only if EnableSGX != 0
  UINT64 SgxEpoch0;                ///< SGX EPOCH0, will be available only if EnableSGX != 0
  UINT64 SgxEpoch1;                ///< SGX EPOCH1, will be available only if EnableSGX != 0
} SGX_FUNCTION_CONFIG;

/**
  All processor security features enabling definitions are in this field.
  Platform code can enable/disable features through this field.
**/
typedef struct {
  UINT8 SkipStopPbet        : 1;
  UINT8  ResetAux           : 1;   ///< Reset Auxiliary content when it is set to "TRUE"
  UINT8  EnableC6Dram       : 1;   ///< C6DRAM Memory, 0: C6DRAM Disabled, <b> 1: C6DRAM Enabled </b>
  UINT8  EnableSgx          : 3;
  UINT8  EnableSgx_7A       : 1;   ///< SGX feature Enable configuration after patch load 0: Disable, 1:Enable
  UINT16 SelectiveEnableSgx;       ///< Let selectively choose threads that will load SGX
  UINT8  Reserved           : 3;   ///< Reserved for future use
  UINT64 PrmrrBase;                ///< PRMRR Base, will be available only if EnableSGX != 0
  UINT64 EpcOffset;                ///< EPC offset from PRMRR base, will be available only if EnableSGX == 5
  UINT64 EpcLength;                ///< EPC length inside PRM range, will be available only if EnableSGX == 5
  UINT32 PrmrrSize;                ///< PRMRR Size, will be available only if EnableSGX != 0
  UINT64 SgxEpoch0;                ///< SGX EPOCH0, will be available only if EnableSGX != 0
  UINT64 SgxEpoch1;                ///< SGX EPOCH1, will be available only if EnableSGX != 0
  ///
  /// Will hold the unaligned PRMRR base adress that was allocated in PEI,
  /// in order to free the unused memory pages in DXE
  ///
  UINT64 PrmrrBaseUnaligned;
} SECURITY_CONFIG_DATA;

/**
  Security Configuration policies.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  BIOS_GUARD_CONFIG   *BiosGuardConfig;  ///< BIOS Guard Configuration settings.
  TXT_CONFIG          *TxtConfig;        ///< TxT Configuration settings.
  SGX_FUNCTION_CONFIG *SgxFunctionConfig;
  UINT32              ResetAux :  1;
  UINT32              Reserved : 31;
} SECURITY_CONFIG;

/**
  Overclocking Configuration controls which use the CPU overclocking mailbox interface are defined in this structure.
  Platform code can pass in data to the mailbox through this structure.
**/
typedef struct {
  INT16  CoreVoltageOffset;        ///< The voltage offset applied to the core while operating in turbo mode.
  UINT16 CoreVoltageOverride;      ///< The core voltage override which is applied to the entire range of cpu core frequencies.
  UINT16 CoreExtraTurboVoltage;    ///< Extra Turbo voltage applied to the cpu core when the cpu is operating in turbo mode.
  UINT16 CoreMaxOcTurboRatio;      ///< Maximum core turbo ratio override allows to increase CPU core frequency beyond the fused max turbo ratio limit.
  INT16  RingVoltageOffset;        ///< The voltage offset applied to CLR while operating in turbo mode.
  UINT16 RingVoltageOverride;      ///< The clr voltage override which is applied to the entire range of cpu frequencies.
  UINT16 RingExtraTurboVoltage;    ///< Extra Turbo voltage applied to clr.
  UINT16 RingMaxOcTurboRatio;      ///< Maximum clr turbo ratio override allows to increase CPU clr frequency beyond the fused max turbo ratio limit.
  UINT8  CoreVoltageMode      : 1; ///< Core voltage mode; <b>0: Adaptive</b>; 1: Override.
  UINT8  RingVoltageMode      : 1; ///< CLR voltage mode; <b>0: Adaptive</b>; 1:Override
  UINT8  OcSupport            : 1; ///< Over clocking support; <b>0: Disable</b>; 1: Enable.
#ifdef HSW_FLAG
  UINT8  SvidEnable           : 1; ///< SVID communication from FIVR to the external VR; <b>0: Disable</b>; 1: Enable.
  UINT8  FivrFaultsEnable     : 1; ///< FIVR Fault management; <b>0: Disable</b>; 1: Enable.
  UINT8  FivrEfficiencyEnable : 1; ///< FIVR efficiency.<b>0: Disable</b>; 1: Enable.
  UINT16 SvidVoltageOverride;      ///< Overrides the VR voltage through SVID bus to apply a new input voltage to the cpu.
  UINT8  BitReserved          : 2; ///< Bits reserved for future use.
#else
  UINT8  CoreDomainLock       : 1; ///< Locks the Core domain from subsequent writes
  UINT8  RingDomainLock       : 1; ///< Locks the Ring domain from subsequent writes
  UINT8  SfrVoltageOffset     : 2; ///< Super Filter Regultaor voltage offset level: 0-none, 1-100mV, 2-200mV
  UINT8  BitReserved          : 2; ///< Bits reserved for future use.
#endif
} CPU_OVERCLOCKING_CONFIG;

/**
  The PPI allows the platform code to publish a set of configuration information that the
  CPU drivers will use to configure the processor in the PEI phase.
  This Policy PPI needs to be initialized for CPU configuration.
  @note The PPI has to be published before processor PEIMs are dispatched.
**/
struct _SI_CPU_POLICY_PPI {
  /**
  This member specifies the revision of the CPU Policy PPI. This field is used to indicate backward
  compatible changes to the PPI. Any such changes to this PPI will result in an update in the revision number.

  <b>Revision 1</b>:
   - Initial version.
  **/
  UINT8                   Revision;
  CPU_CONFIG              *CpuConfig;          ///< Processor Features Configuration.
  POWER_MGMT_CONFIG       *PowerMgmtConfig;    ///< Processor Power ManagementFeatures Configuration.
  SECURITY_CONFIG         *SecurityConfig;     ///< Processor Security features Configuration.
  CPU_OVERCLOCKING_CONFIG *OverclockingConfig; ///< Processor OverClocking features Configuration.
  UINTN                   CpuPlatformPpiPtr;   ///< Pointer to this PPI.
};

#pragma pack()
#endif
