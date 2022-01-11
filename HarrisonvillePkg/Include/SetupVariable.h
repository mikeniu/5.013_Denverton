//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
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

    SetupVariable.h

Abstract:

    Driver configuration include file

Revision History

--*/
#include <Include/Guid/SetupVariable.h>


#ifndef PLATFORM_SETUP_VARIABLE_NAME
#define PLATFORM_SETUP_VARIABLE_NAME             L"IntelSetup" // Denverton Aptiov override  - EIP#217439
#define PCH_SETUP_VARIABLE_NAME                  L"PchSetup"
#define SETUP_VOLATILE_DATA_NAME                 L"SetupVolatileData"

//Note that PASSWORD_MAX_SIZE refers to the plaintext password field which is NOT stored in NVRAM.
// Denverton Aptiov override Start - EIP#249150
#ifndef PASSWORD_MAX_SIZE
#define PASSWORD_MAX_SIZE               32
#endif
// Denverton Aptiov override End - EIP#249150
#define PASSWORD_SALT_SIZE              32
#define MAX_CPU_SOCKET                  4

#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002
// Denverton Aptiov override Start - EIP#249150
#ifndef HPET_SUPPORT
#define HPET_SUPPORT                    0x1
#endif
// Denverton Aptiov override End - EIP#249150

#define PCIE_MAX_VIRTUAL_ROOT_PORTS     3
#define MAX_PCI_ROOT_BRIDGES            0x1
// Denverton Aptiov override Start - EIP#249150
#ifndef PCI_EXPRESS_SUPPORT
#define PCI_EXPRESS_SUPPORT             0x1
#endif
// Denverton Aptiov override End - EIP#249150
#define PCI_X_SUPPORT                   0x0
#define MAX_CLTT_REGS                   0x4
#define MAX_THERMAL_TRIP_EVENT_REGS     0x13
#define SB_INTERNAL_DELAY               0x0
#define NB_DMI_L0_EXIT_LATENCY          0x1
#define NB_DMI_L1_EXIT_LATENCY          0x1
#define SB_DMI_L0_EXIT_LATENCY          0x1
#define SB_DMI_L1_EXIT_LATENCY          0x1
#define NB_DMI_ASPM_SUPPORT             0x3
#define SB_DMI_ASPM_SUPPORT             0x3
//Memory
#define MAX_NUMBER_OF_RANKS             0x4
#define MAX_NUMBER_OF_CHANNELS          0x2
//Sata
#define SATA_MODE_LOOPBACK_TEST         0x3
#define ME_FIA_MUX_LANE_NUM_MAX         20
#define PCH_MAX_SATA_CONTROLLERS        0x2
#define PCH_MAX_PORT_SETUPVAR           16
#define PCIE_IP_STANDARD_ROOT_PORT      8
#define PCIE_IP_VIRTUAL_ROOT_PORT       3
#define MAX_PCIE_ROOT_PORTS            (PCIE_IP_STANDARD_ROOT_PORT + PCIE_IP_VIRTUAL_ROOT_PORT)
#define MAX_PCIE_LANES                  16
#define PCH_MAX_RST_PCIE_STORAGE_CR       3
//USB
#define PCH_H_XHCI_MAX_USB2_PORTS         4 /*For SPTH:16, For SPTLP:12*/
#define PCH_H_XHCI_MAX_USB3_PORTS         4 /*For SPTH:10, For SPTLP:6*/
#define PCH_XHCI_MAX_SSIC_PORT_COUNT      2
#define SATA_MODE_AHCI                    0
#define SATA_MODE_RAID                    1
#define SATA_TEST_MODE_ENABLE             1
#define NON_CS_DEVICES                    9
//PCI
#define PCH_MAX_PCIE_CLUSTERS           2
#define PCH_PCIE_MAX_ROOT_PORTS         0x8  /*For LTPH:8, For LPTLP:6*/
#define PCH_SATA_MAX_PORTS              0x8

//Denverton AptioV Override Start- EIP#348922
//Password
#ifndef _AMI_VFR_H_
#define    SYSTEM_PASSWORD_ADMIN	        0x0
#define    SYSTEM_PASSWORD_USER             0x1
#endif
//Denverton AptioV Override End- EIP#348922

//Serial IO
#define I2C0_SENSOR_DEVICES      7
#define I2C1_SENSOR_DEVICES      13
#define UART0_SENSOR_DEVICES     2
#define UART1_SENSOR_DEVICES     3
#define NON_CS_DEVICES           9
#define ADSP_OEM_FEATURES        3
#define PCH_MAX_HSUART             3

#define VAL_LEGACY_BOOT_SUPPORT_LEGACY_FREE  0
#define VAL_LEGACY_BOOT_SUPPORT_LEGACY_VIDEO 1
#define VAL_LEGACY_BOOT_SUPPORT_LEGACY_FULL  2

#define ADR_INTERNAL_SOURCE_COUNT 32
#define MAX_BIMRXCP               20

//!!!This macro has the copy with same purpose @/DenvertonPkg/SystemAgent/Include/SetupVariable.h and @/DenvertonPkg/SystemAgent/Include/PPI/SaPlatofrmConfig.h
#define COS_CAT_AGENT_COUNT		16

extern EFI_GUID  gMainPkgListGuid;
extern EFI_GUID  gAdvancedPkgListGuid;
extern EFI_GUID  gSysInfoPkgListGuid;
extern EFI_GUID  gTpmPkgListGuid;
extern EFI_GUID  gSecurityPkgListGuid;
extern EFI_GUID  gChipsetPkgListGuid;

#ifndef VFRCOMPILE
extern EFI_GUID gSetupVariableGuid;
extern EFI_GUID gEfiSetupVariableDefaultGuid;
extern EFI_GUID gEfiGlobalVariableControlGuid;
#endif

typedef struct {

  //
  // Keyboard
  //
  UINT8   Numlock;
  //
  // Passwords
  // AdminPassword and UserPassword now store the SHA-256 hash result, instead of the plaintext.
  // SHA-256 has a constant output size of 32 bytes, regardless of actual plaintext size.
  UINT8   UserPassword[32];
  UINT8   AdminPassword[32];
  UINT8   UserPasswordSalt[PASSWORD_SALT_SIZE];
  UINT8   AdminPasswordSalt[PASSWORD_SALT_SIZE];
  UINT8   Access;
  //
  // TPM
  //
  UINT8	  TpmAdmin;
  UINT8   TpmEnable;
  UINT8   TpmState;
  UINT8   MorState;
  //
  // Power State
  //
  UINT8   PowerState;
  //
  // Core-Uncore BGF Pointer Separation
  //
  UINT8   CoreBGF;
  //
  // Video
  //
  UINT8   VideoSelect;
  UINT8   EfiWindowsInt10Workaround;
  //UINT8   ConsoleRedir; // Denverton AptioV Override - EIP#235760
  UINT8   SataSelect; // Boot from on board Sata (0) or use PCIE instead (1)
  ///  reset testing
  UINT8   ValidationResetType;
  UINT8   ValidationResetEvent;
  UINT16  ValidationResetCount;
  //  for PCI forms
//  UINT8   Above4gDecode;          //[Disable]\ Enable  // Denverton AptioV Override
  //General PCI Settings: [] - default
  UINT8   VgaPallete;           //[Disable]\ Enable
  UINT8   PerrEnable;           //[Disable]\ Enable
  UINT8   SerrEnable;           //[Disable]\ Enable

#if PCI_X_SUPPORT
  UINT8   PciXLatency;				// 32 \[64]\ 96 \ 128 \ 160 \ 192 \ 224 \ 248
#endif

#if PCI_EXPRESS_SUPPORT
// Denverton AptioV Override Start
  //PCI Express Device Settings: [] - default
//  UINT8	  RelaxedOrdering; 		//[Disable]\ Enable
//  UINT8	  ExtTagField; 			//[Disable]\ Enable
//  UINT8	  MaxPayload;				//[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
//  UINT8   MaxReadRequest;			//[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
  //PCI Express Link settings: [] - default
//  UINT8   PchPcieGlobalAspm; 	    //[Disable]\ Auto \ Force L0
//  UINT8   ExtendedSynch;			//[Disable]\ Enable

// Denverton AptioV Override End
#endif
  UINT8   Hecbase;
  /* Processor (CPU) */
  UINT8   CpuidMaxValue;
  UINT8   ExecuteDisableBit;
  UINT8   EnableDumpCrashLog;               // Enable "Dump Crash Log" feature
  UINT8   ProcessorEistEnable;              // CPU P-States Enable
  UINT8   TurboModeEnable;                  // Turbo Mode Enable
  UINT8   TurboMode;                        // Turbo Mode
  UINT8   iTurboModeValue;                  // Turbo Mode Value
  UINT8   BiosRequestFreq;                  // BIOS Request Freq
  UINT8   EnableTm1;                        // TM1 enable
  UINT8   Tm2ThrottlingType;                // TM2 throttling mode
  UINT8   ProcessorCcxEnable;               // CPU C-States Enable
  UINT8   C1AutoDemotion;                   // C1 Auto Demotion
  UINT8   C3AutoDemotion;                   // C3 Auto Demotion
  UINT8   MaxCState;                        // Max C-State
  UINT8   PackageCState;                    // Package C-State Limit
  UINT8   ProcessorC1eEnable;               // Enabling C1E state of processor
  UINT8   TStateEnable;                     // CPU T-States Enable
  UINT8   ProcessorHyperThreadingDisable;   // Hyper Threading
  UINT8   ProcessorVmxEnable;               // Enabling VMX
  UINT8   ProcessorSmxEnable;               // Enabling SMX
  UINT8	  ThreeStrikeTimer;                 // Disable 3strike timer
  UINT8   ProcessorBistEnable;              // Enabling BIST
  UINT8   FastStringEnable;                 // Fast String
  UINT8   MonitorMwaitEnable;               // Monitor/Mwait
  UINT8   L1Prefetcher;                     // L1 Prefetcher
  UINT8   L2Prefetcher;                     // L2 Prefetcher
  UINT8   MachineCheckEnable;               // Machine Check
  UINT8   DcaEnable;                        // DCA setup option
  UINT8   DcaPrefetchDelayValue;            // DCA Prefetch Delay Value
  UINT8   MlcStreamerPrefetcherEnable;      // Hardware Prefetch
  UINT8   MlcSpatialPrefetcherEnable;       // Adjacent Cache Line Prefetch
  UINT8   DCUStreamerPrefetcherEnable;      // DCU Streamer Prefetcher
  UINT8   DCUIPPrefetcherEnable;            // DCU IP Prefetcher
  UINT8   DCUModeSelection;                 // DCU Mode Selection
  UINT8   ProcessorXapic;                   // Enable Processor XAPIC
  UINT8   BspSelection;                     // Select BSP
  UINT8   IedSize;                          // IED size
  UINT8   MTRRDefTypeUncachable;            // For changing UC to WB
  UINT8   OnDieThermalThrottling;           // Throtte ratio
  UINT8   MfgMode;
  UINT8   ActiveCoreCount;
  UINT8   AesEnable;
  UINT8   SmbusSpeed;
  //
  // Wake On Lan support
  //
  UINT8   WakeOnLanSupport;
  //
  // RTC WAKE
  //
  UINT8   WakeOnRTCS5;
  UINT8   RTCWakeupTimeHour;
  UINT8   RTCWakeupTimeMinute;
  UINT8   RTCWakeupTimeSecond;

  UINT8   DfxAdvDebugJumper;
  // UINT8   ShellPosition; // Denverton AptioV override - EIP#254421 - Variable used only in Intel RC
  //
  // RTC Lock
  //
  UINT8   PchRootPort;
  UINT8   GpioNumber;
  UINT8   PchTclockdnWA;
  UINT8   ProchotResponse;
  UINT8   ProchotOutputMode;
  UINT8   VrThermAlertDisable;
  UINT8   LockThermInt;
  UINT8   ProchotFreqResponse;

  UINT8   Iqat;        /* stores state of IQAT unit: Disabled/Enabled */
  UINT8   Iqat_SetFuseCtl;    /* Set FUSECTL option: Disabled/Enabled */
  UINT8   Iqat_FuseCtlValue;  /* FUSECTL Value to set */
  UINT8   Iqat_Set64BMrrMpl;  /* Set 64B MRR/MPL option: Disabled/Enabled */
  UINT8   IOComplianceEn;

  UINT16  SMBusController;
  UINT8   SMBusIOSFClockGating;
#if HPET_SUPPORT
  UINT8   Hpet;
#endif
  //
  //PPM CONFIG
  //
  UINT8   CxPopUpEnable;
  UINT8   C4Exit;
  UINT16  PCIClockRun;
  //
  // Boot Options
  //
  UINT8   SerialTerminal;
  UINT8   DebugMessages;
  UINT16  VariablePlatId;

  //
  // PPR Options
  //
  UINT8   PprEnable[8];
  UINT16  Row[8];
  UINT8   Bank[8];
  UINT8   BankGroup[8];
  UINT8   DramDevice[8];
  UINT8   TargetRank[8];
  UINT8   TargetChannel[8];
  UINT8   CommandMode;
  UINT8   FastBoot;
  UINT8   Reserved1[3];
  //Usb Configdata
  UINT8	  UsbLegacySupport;
  UINT8   UsbXhciSupport;
  UINT8	  UsbEmul6064;
  UINT8	  UsbMassResetDelay;
  UINT8	  UsbEmu1;
  UINT8	  UsbEmu2;
  UINT8	  UsbEmu3;
  UINT8	  UsbEmu4;
  UINT8	  UsbEmu5;
  UINT8	  UsbEmu6;
  UINT8	  UsbEmu7;
  UINT8	  UsbEmu8;
  UINT16  SystemPageSize;
  UINT8	  SvrMngmntFrb2Enable;
  UINT16  SvrMngmntFrb2Timeout;
  UINT8   SvrMngmntFrb2Policy;
  UINT8	  PowerRestorationPolicy;
  UINT8	  ClearSELLog;
  UINT8	  SMEBootDeviceForceOverride;
  UINT8   MdllOffSen;
  UINT8   MpllOffSen;
  /* CSR_TURBO_POWER_LIMIT  1:10:0:0xe8 */
  UINT16  PowerLimit2Power;
  UINT8   TurboPowerLimitLock;
  /* CSR_PCIE_ILTR_OVRD  (CSR 1:10:1:78) */
  UINT8   EnableThermalMonitor;
  UINT8   EnergyEfficientPState;
  UINT8   CStateUnDemotion;
  /* OverClocking */
  UINT8   EnableXe;
  /* Thermal Configuration */
  UINT8	  EnableDTSCalibration;
  UINT32  ClttThermReg[MAX_CLTT_REGS];
  UINT32  ClttEventTrip;
  UINT8   TjTargetOffset;
  UINT8   TcontrolOffset;
  UINT8   TcontrolOffsetSign;
  UINT8   OutOfSpecThreshold;
  UINT8   OutOfSpecInterruptEnable;
  UINT8   LowTempInterrupt;
  UINT8   HighTempInterrupt;
  UINT8   TempThreshold1;
  UINT8   TempThreshold2;
  UINT8   Threshold1InterruptEnable;
  UINT8   Threshold2InterruptEnable;
  UINT8   ProcHotInterruptEnable;
  //
  // CK420 Configuration
  //
  UINT8   spreadSpectrumEnable;
  //
  // Chipset Configuration Variables
  //
  UINT8   HideChannel1;
  UINT8   BankXorMapEnable;
  //
  // Enable/Disable ECC
  //
  UINT8   ECCSupport;
  //
  // Faulty Part tracking options
  //
  UINT8   FaultyPartEnable;
  UINT8   FaultyPartOnCorrectable;
  //
  // Enable Memory thermal Management
  //
  UINT8   MemoryThermalEnable;
  //
  // Select Memory Thermal throttling mode
  //
  UINT8   ThermalThrottlingType;
  //
  // Select Close Loop Thermal Throttling mode
  //
  UINT8   ClttMode;
  //
  // Configure CLTT Temperature Levels Hi/Mid/Lo
  //
  UINT8   TempHi;
  UINT8   TempMid;
  UINT8   TempLo;
  //
  // Configure CLTT Banwidth Levels Crit/Hi/Mid
  //
  UINT8   BwLevelOnThrtCrit;
  UINT8   BwLevelOnThrtHi;
  UINT8   BwLevelOnThrtMid;
  //
  // Select Throttled Banwidth when Open Loop Thermal Throttling enabled
  //
  UINT8   OlttThrtBw;
  //
  // Select MEMHOT Level
  //
  UINT8   MemHotThrotLvl;
  //
  // Select MEMTRIP Level
  //
  UINT8   EnMemTrip;
  UINT8   TempMemTrip;
  //
  // Select TxRxSkewCtl
  //
  UINT8 RxSkewCtl;
  UINT8 TxSkewCtl;
  //
  // Performance profile for DMAP
  //
  UINT8 PerformanceProfile;
  //
  // Dynamic Self Refresh
  //
  UINT8   DynamicSelfRefresh;
  //
  // Power Mode Opcode during Self Refresh
  //
  UINT8   DynamicPowerModeOpcode;
  //
  // Self Refresh Delay
  //
  UINT16  SelfRefreshDelay;

  //
  // CKE Power Down
  //
  UINT8   CkePowerDownDisable;
  //
  // Select the refresh rate
  //
  UINT8   SelectRefreshRate;
  //
  // Enable Parity Check
  //
  UINT8   EnableParityCheck;
  //
  // Enable/Disable Patrol and Demand Scrub
  //
  UINT8   PatrolSupport;
  UINT8   PatrolPeriod;
  UINT8   DemandScrub;
  UINT16  MaxScrubDebitCount;
  UINT8   MrcRmtSupport;
  UINT8   MrcDebugMsg;
  UINT8   ReadPerBitEnable;
  UINT8   WritePerBitEnable;
  UINT8   Rk2RkEn;
  UINT8   BDebugSelfRefresh;
  UINT16  CorrectErrThreshold;
  UINT8   PerBitMargins;
  UINT8   EnableScrambler;
  UINT8   EnableSlowPowerDownExit;
  UINT8   AbSegToDram;
  UINT8   ESegToDram;
  UINT8   FSegToDram;
  UINT8   OutOfOrderAgingThreshold;
  UINT8   EnableOutOfOrderProcessing;
  UINT8   TwoClkRdPreamble;
  UINT8   TwoClkWrPreamble;
  UINT8   WriteDataEarlyEnable;
  UINT8   EnableNewRequestBypass;
  UINT8   VrefOverrideEnable;
  UINT8   VrefOverrideValue;
  UINT8   MemoryPreservation;
  //
  // DDR voltage support
  //
  UINT8   InputFineDdrVoltage;
  UINT8   Suppress125VOption;
  UINT8   MrcRmtCpgcExpLoopCntValue;
  UINT8   MrcRmtCpgcNumBursts;
  //
  // Dci
  //
  UINT8   DebugInterfaceEnable;
  UINT8   DebugInterfaceLockEnable;
  UINT8   DebugDciEnable;
  //
  // Mitigation feature
  //
  UINT16  MitigationProbability;
  //
  // All related with Pass Gate Setup
  //
  UINT8   InputPassGateEnabled;
  UINT8   InputPassGateEnableSelfRefresh;
  UINT8   InputPassGateDirection;
  UINT16  InputPassGateRepetitionCountRange;
  UINT16  InputPassGateIterationOnRow;
  UINT8   InputPassGateSwizzle;
  UINT8   InputPassGatePattern;
  UINT8   InputPassGateTargetPattern;
  UINT8   InputSpeedSelected;
  UINT8   InputPassGatePartial;
  UINT32  InputPassGateRowBankMin;
  UINT32  InputPassGateRowBankMax;
  UINT8   InputPassGateMonteCarloEnable;
  UINT8   InputMontecarloMaxFailures;
  UINT16  InputMontecarloStartRepetition;
  UINT16  InputMontecarloDecrement;
  UINT8   HideRankCh0[MAX_NUMBER_OF_RANKS];
  UINT8   HideRankCh1[MAX_NUMBER_OF_RANKS];
  UINT8   InputPassGateRankEnableCh0[MAX_NUMBER_OF_RANKS];
  UINT8   InputPassGateRankEnableCh1[MAX_NUMBER_OF_RANKS];
  //
  // Power Mode Settings
  //
  UINT8   PmopPc0;
  UINT8   PmopPcx;
  //
  // Mmio High Setup
  //
  UINT16  MmioHighSize;
  //
  // Bunit Machine Check Mode Setup
  //
  UINT8   BmcModeEnable;

  //
  // Memory mapping variables
  //
  UINT8   MmioSize;
  UINT8   MrcMemoryTestLoop;
  UINT16  LoopCount;
  UINT8   MrcHaltSystemOnMemTestError;
  UINT8   MrcResetLoop;
  UINT8   MrcResetLoopCycles;
  UINT8   MrcTrainingCpgcExpLoopCntValue;
  UINT8   MrcTrainingCpgcNumBursts;
  UINT8   BramParityEnable;
  UINT8   InterleaveMode;
  UINT8   EnableSsaClockGating;
  UINT8   ZqcEnable;
  //NTB EP lock Bit 0 of (bus, device, function, offset) = (0,0x0,0,0xea)
  //PCIe RP BARS lock (Threre are 12) Bit 0 of (bus, device, function, offset) =
  //  (0,1,0,0xea), (0,0x2,0,0xea), (0,0x3,0,0xea), (0,0x4,0,0xea),
  //  (0,5,0,0xea), (0,0x6,0,0xea), (0,0x7,0,0xea), (0,0x8,0,0xea),
  //  (0,9,0,0xea), (0,0xa,0,0xea), (0,0xb,0,0xea), (0,0xc,0,0xea)
  //NTB IP lock Bit 0 of (bus, device, function, offset) = (0,0xd,0,0xea)
  //CBDMA BARS lock (Threre are 4) Bit 0 of (bus, device, function, offset) =
  //  (0,0x11,0,0xea), (0,0x11,1,0xea), (0,11,2,0xea), (0,11,3,0xea)
  //SMBUS BARS lock (Threre are 2) Bit 0 of (bus, device, function, offset) =
  //  (0,0x13,0,0xea), (0,0x13,1,0xea)
  UINT8   CapabilityLock[PCIE_IP_STANDARD_ROOT_PORT];
  //
  // Memory Frequency
  //
  UINT8   DdrFreq;
  UINT8   DdrChannels;
  UINT8   OpenPolicyTimer;
  UINT8   CMDRate;
  UINT8   EnableParallelTraining;
  //
  // 32bit enabled
  //
  UINT8   HalfWidthEnable;
  //
  //Modify Tcl to improve performance
  //
  UINT8   TclIdle;
  //
  // Memory Timing
  //
  UINT8   TCLTiming;
  UINT8   TRCDTiming;
  UINT8   TRPTiming;
  UINT8   TRASTiming;
  UINT8   TRTPTiming;
  UINT8   TRRDTiming;
  UINT8   TFAWTiming;
  UINT8   TCCDTiming;
  UINT8   TWTPTiming;
  UINT8   TWCLTiming;

  //
  //  VT-d
  //
  UINT8   VtdEnable;
  UINT8   InterruptRemappingSupport;


  UINT8   OverrideCheckpoints;
  UINT8   RtitSupportEnable;
  UINT8   InputEnableBunitPerformance;
  UINT8   SmmSize;

  //
  // Leaky Bucket
  //
  UINT8   LeakyBucketUnits;
  UINT64  LeakyBucketRateCh0[MAX_NUMBER_OF_RANKS];
  UINT64  LeakyBucketRateCh1[MAX_NUMBER_OF_RANKS];
  UINT16  CorrectableErrorThresholdCh0[MAX_NUMBER_OF_RANKS];
  UINT16  CorrectableErrorThresholdCh1[MAX_NUMBER_OF_RANKS];
  //
  // Dram RAPL
  //
  UINT64  DramRaplTimeWindow;
  UINT32  DramRaplPowerLimit;
  UINT8   DramRaplPowerLimitEnable;
  UINT8   DramRaplLimitLock;
  //
  // NonVolatile Memory
  //
  UINT8   NonVolMemMode;
  UINT8   NonVolMemInterleaving;
  UINT32  NonVolMemSize;
  UINT8   eraseArmNVDIMMS;
  UINT8   restoreNVDIMMS;
  UINT8   NonVolMemCacheFlushing;
  UINT8   NonVolMemAdrStatusSrc;   // ADR status source when ADR reset has occurred: Internal==1 or External==0
  UINT8   NonVolMemC2fLbaLocMethod;
  UINT32  NonVolMemC2fLba;
  UINT8   NonVolMemTestMode;
  //
  // NonVolatile Memory end
  //
  UINT8   PmcInternAdrSourcesSet;
  UINT8   PmcInternAdrSources[ADR_INTERNAL_SOURCE_COUNT];

  /* Root's Complex Configuration [ RP0, RP1, RP2  ] */
  UINT8   bifurcation[PCH_MAX_PCIE_CLUSTERS] ;

  UINT8   EnableMphyMsg;
  /* Devices enable/disable */
  UINT8   sataControllerEn[PCH_SATA_MAX_PORTS];
  UINT8   sata3ControllerEn;
  UINT8   sata3ControllerSpeed;
  // Denverton AptioV Override Start - EIP#235760
  //UINT8   UsbMainSupport; 
  UINT8   RelaxSecConf;
  //UINT8   UsbIoPm;
  //UINT8   gbeEn[4];
  // Denverton AptioV Override End - EIP#235760
  UINT8   sataExternal[4];
  UINT8   sata3External[4];
  UINT8   sataSpinUp[4];
  UINT8   sata3SpinUp[4];
  UINT8   sataHotPlug[4];
  UINT8   sata3HotPlug[4];
  UINT8   sataMechSw[4];
  UINT8   sata3MechSw[4];
  UINT8   sataPortEn[4];
  UINT8   sata3PortEn[4];
  UINT8   sata3Speed[4];
  UINT8   sataIdeMode;  // IDE Mode             0 Legacy 1 Native
  UINT8   sata3IdeMode; // IDE Mode             0 Legacy 1 Native
  UINT8   sataType;     // 0 - IDE  1 - AHCI 2 RAID
  UINT8   sata3Type;    // 0 - IDE  1 - AHCI 2 RAID
  UINT8   sataSalp;
  UINT8   sata3Salp;
  UINT8   sataLpm;
  UINT8   sata3Lpm;
  UINT8   sataTestMode;
  //----------SIR VALUES----------
  UINT8   SataOverWriteSirEn;
  UINT32  sata_SIR_LBC01;
  UINT32  sata_SIR_LBC23;
  UINT32  sata_SIR_LBC45;
  UINT8   Sata3OverWriteSirEn;
  UINT32  sata3_SIR_LBC01;
  UINT32  sata3_SIR_LBC23;
  UINT32  sata3_SIR_LBC45;
  //------------------------------
  //UINT8   LegacyPxeRom; // Denverton AptioV Override - EIP#235760
  UINT8   BoardId;
  UINT8   InspeccmmEn;
  UINT8   InOrderAPIC;
  UINT8   SchedulerLat;
  UINT8   CosCatAgent[COS_CAT_AGENT_COUNT];
  UINT8   BestEffortMaxLat;
  UINT8   PageHitDelay;
  UINT8   IsocBankPrefetch;
  UINT8   BestEffortBankPref;
  UINT8   ProcessorFlexibleRatioOverrideEnable;
  UINT8   ProcessorFlexibleRatio;           // Flex Ratio Limit - sets Max Non-Turbo available only with ratio unlocked processors
  //
  // These Turbo related registers must be configured in BIOS before BIOS_RESET_CPL[0] is set.
  //
  UINT32  PkgPowerSkuUnit;                  // MSR 0x660 PKG_POWER_SKU_UNIT value
  UINT16  PackageRaplPowerLevel1;
  UINT16  PackageRaplPowerLevel2;
  UINT32  PackageRaplTimeWindow1;           // Package Rapl Power Limit 1 MSR 0x610 PKG_RAPL_LIMIT value
  UINT8   RatioLimitsEnable;                // Control to Enable/Disable the configuration of the Ratio Limits MSRs
  UINT8   RatioLimit0;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 7:0
  UINT8   RatioLimit1;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 15:8
  UINT8   RatioLimit2;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 23:16
  UINT8   RatioLimit3;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 31:24
  UINT8   RatioLimit4;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 39:32
  UINT8   RatioLimit5;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 47:40
  UINT8   RatioLimit6;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 55:48
  UINT8   RatioLimit7;                      // MSR 0x1AD FREQ_LIMIT_RATIOS - bits 63:56
  UINT8   CoreCount0;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 7:0
  UINT8   CoreCount1;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 15:8
  UINT8   CoreCount2;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 23:16
  UINT8   CoreCount3;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 31:24
  UINT8   CoreCount4;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 39:32
  UINT8   CoreCount5;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 47:40
  UINT8   CoreCount6;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 55:48
  UINT8   CoreCount7;                       // MSR 0x1AE FREQ_LIMIT_CORES - bits 63:56
  UINT8   PkgRaplLimitLock;                 // MSR 0x610 PKG_RAPL_LIMIT_PKG_PWR_LIM_LOCK (bit 63)
  UINT16  PL3ControlPowerLevel;             // PL3_CONTROL_MCHBAR_POWER_LIMIT (14:0)
  UINT8   PL3ControlEnable;                 // PL3_CONTROL_MCHBAR_PL3_ENABLE (15)
  UINT8   PL3ControlTimeWindow;             // PL3_CONTROL_MCHBAR_TIME_WINDOW (23:17)
  UINT8   PL3ControlDutyCycle;              // PL3_CONTROL_MCHBAR_DUTY_CYCLE (bits 30:24)
  UINT16  PL3ControlPmaxPower;              // PL3_CONTROL_MCHBAR_PMAX_POWER (47:32)
  UINT8   PL3ControlLock;                   // PL3_CONTROL_MCHBAR_PWR_LIM_LOCK (bit 63)
  //
  //Event Log
  //
  UINT8   SystemErrorEn;
  UINT8   ErrorCloakingEnable;
  UINT8   MemoryErrorLog;
  UINT8   MemoryCorrErrorLog;
  UINT8   MemoryUnCorrErrorLog;
  UINT8   PCIeErrorLog;
  UINT8   PCIeCorrErrorLog;
  UINT8   PCIeNFErrorLog;
  UINT8   PCIeFatalErrorLog;
  UINT8   NativeAEREnable;
  UINT8   PCIeSystemErrEnable;
  UINT8   PCIeParityErrEnable;
  //
  // WHEA
  //
  UINT8   WheaSupport;
  UINT8   WheaEinj_05Extn;
  UINT8   WheaLogging;
  UINT8   WheaPcieErrInj;
  //
  //
  //
  UINT8   PchTcLockDown;
  UINT8   CyclonePcieSwitchWA;
  UINT8   PchSataExtOverride[6];
  UINT8   GlobalSmiLock;
  UINT8   PchDmiAspmCtrl;
  UINT8   PchSataGCAssel;
  UINT8   PchPmRegisterLock;
  UINT8   PchNandRemapPortConfigCheck;
  //
  UINT8   PchSataLtrConfigLock;
  UINT8   PchSataLtrEnable;
  UINT8   PchSataLtrOverride;
  UINT8   PchSataSnoopLatencyOverrideMultiplier;
  UINT16  PchSataSnoopLatencyOverrideValue;
  UINT8   PchDmiTsSuggestSet;
  UINT8   PchDmiTsawEn;
  UINT8   PchTs0Width;
  UINT8   PchTs1Width;
  UINT8   PchTs2Width;
  UINT8   PchTs3Width;
  UINT8   PchSataTsSuggestSet;
  UINT8   PchP0T1M;
  UINT8   PchP0T2M;
  UINT8   PchP0T3M;
  UINT8   PchP0TDisp;
  UINT8   PchP0Tinact;
  UINT8   PchP0TDispFinit;
  UINT8   PchP1T1M;
  UINT8   PchP1T2M;
  UINT8   PchP1T3M;
  UINT8   PchP1TDisp;
  UINT8   PchP1Tinact;
  UINT8   PchP1TDispFinit;
  //
  //CPPM Menu
  //
  UINT8   PchMemCloseStateEn;
  UINT8   PchInternalObffEn;
  UINT8   PchExternalObffEn;
  UINT8   PchClientObffEn;
  UINT8   PchCxObffEntryDelay;
  UINT8   PchPcieObffEnable[8];
  UINT8   PchPcieForceLtrOverride[8];
  //
  // Serial IO
  //
  UINT8   PchConvertUartIntoGpio;
  UINT8   PchAdditionalSerialIoDevices;
  UINT8   PchSerialIoIdleDetection[5];
  UINT8   PchSerialIoChannel[4];
  UINT8   PchSerialIoInterrupts[7];
  UINT8   PchSerialIoAsyncInt;
  UINT8   PchSerialIoAsyncIntOverride;
  UINT32  PchSerialIoMasterDllCrtl;
  UINT32  PchSerialIoMasterDllInit;
  UINT32  PchSerialIoMasterDllSwOverride;
  UINT32  PchSerialIoSlaveDelayDefault;
  UINT32  PchSerialIoSlaveDelaySdr25;
  UINT32  PchSerialIoSlaveDelayDdr50;
  //
  // Miscellaneous options
  //
  //UINT8   EfiNetworkSupport; // Denverton AptioV Override - EIP#235760
  UINT8   EnableClockSpreadSpec;

  //
  // Usb Config
  //
  // Denverton AptioV Override Start - EIP#235760
  /*
  UINT8   PchUsb30Mode;
  UINT8   Usb3PinsTermination;
  UINT8   EnableUsb3Pin1;
  UINT8   EnableUsb3Pin2;
  UINT8   EnableUsb3Pin3;
  UINT8   EnableUsb3Pin4;
  UINT8   EnableUsb3Pin5;
  UINT8   EnableUsb3Pin6;
  UINT8   PchUsbPerPortCtl;
  UINT8   PchUsbPort[14];
  UINT8   UsbPrecondition;
  UINT8   Btcg;
  UINT8   UsbSensorHub;
  UINT8   Usb2PortTestMode;
  UINT8   UsbUnlockForNoa;
  */
  // Denverton AptioV Override End - EIP#235760
  //
  // Sata CONFIG
  //

  UINT8   SataSpeed[PCH_SATA_MAX_PORTS];
  UINT8   SataPort[PCH_SATA_MAX_PORTS];
  UINT8   SataInterfaceMode;
  UINT8   SataHotPlug[PCH_SATA_MAX_PORTS];
  UINT8   SataMechanicalSw[6];
  UINT8   SataSpinUp[PCH_SATA_MAX_PORTS];
  UINT8   SataExternal[PCH_SATA_MAX_PORTS];
  UINT8   SataType[PCH_SATA_MAX_PORTS];
  UINT8   SataRaidR0;
  UINT8   SataRaidR1;
  UINT8   SataRaidR10;
  UINT8   SataRaidR5;
  UINT8   SataRaidIrrt;
  UINT8   SataRaidOub;
  UINT8   SataHddlk;
  UINT8   SataLedl;
  UINT8   SataRaidIooe;
  UINT8   SataRaidSrt;
  UINT8   SataRaidOromDelay;
  UINT8   SataRstForceForm;
  UINT8   SataAlternateId;
  UINT8   SataSalp;
  UINT8   SataTestMode;
  UINT8   EfiRaid;
  UINT8   HddAcousticPowerManagement;
  UINT8   HddAcousticMode;
  UINT8   PxDevSlp[PCH_SATA_MAX_PORTS];
  UINT8   EnableDitoConfig[PCH_SATA_MAX_PORTS];
  UINT16  DitoVal[PCH_SATA_MAX_PORTS];
  UINT8   DmVal[PCH_SATA_MAX_PORTS];

  //
  // PCI_EXPRESS_CONFIG, Endpoints
  //
  // Denverton AptioV Override Start - EIP#235760
  /*
  UINT8   PcieEPMaxPayload;
  UINT8   PcieEPMaxReadRequest;
  UINT8   PcieEPExtTagField;
  UINT8   PcieEPRelaxedOrdering;
  UINT8   PcieEPExtSync;
  UINT8   PcieEPAspm;
  */
  // Denverton AptioV Override End - EIP#235760
  
  //
  // PCI_EXPRESS_CONFIG, 8 ROOT PORTS
  //
  UINT8   RootPortClockGating[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortLinkSpeed[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortMaxPayload[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortMaxReadRequest[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortExtTagField[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortRelaxedOrdering[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortExtSync[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortDeEmphasis[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortLaneReversal[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortRTTO[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortCTO[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortCTORange[PCIE_IP_STANDARD_ROOT_PORT];
  UINT8   RootPortStopScream[PCIE_IP_STANDARD_ROOT_PORT];


  UINT8   PcieClockGating[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   VrpClockGating[PCH_PCIE_MAX_ROOT_PORTS];

  UINT8   ControlLaneReversal[ PCH_PCIE_MAX_ROOT_PORTS ] ;
  UINT8   ControlRtto[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   ControlCto[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   CtoRange[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   DeEmphasis[PCH_PCIE_MAX_ROOT_PORTS] ;
  UINT8   barAddrRP[ PCH_PCIE_MAX_ROOT_PORTS ] ;

  UINT8   PcieDmiExtSync;
  UINT8   PcieSBDE;
  UINT8   PcieRootPortSBDE;
  UINT8   PcieSBDEPort;
  UINT8   PcieUsbGlitchWa;
  UINT8   PcieNandRemap;
  UINT8   PcieNandConfigAccessLockDown;
  UINT8   PcieNandRemapPort;
  UINT8   PcieRootPortFunctionSwapping;
  UINT8   PcieRootPortEn[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortAspm[PCH_PCIE_MAX_ROOT_PORTS]; 
  UINT8   PcieRootPortURE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortSLD[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortFEE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortNFE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortCEE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortCTD[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortPIE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortSFE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortSNE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortSCE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortPMCE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortHPE[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortSpeed[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortTHS[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortL1SubStates[PCH_PCIE_MAX_ROOT_PORTS];

//Denverton AptioV Override Start- EIP#305575  
// PCIE Hotplug support
  UINT8   PcieExtraBusRsvd[PCH_PCIE_MAX_ROOT_PORTS];
  UINT16  PcieMemRsvd[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieMemRsvdalig[PCH_PCIE_MAX_ROOT_PORTS];
  UINT16  PciePFMemRsvd[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PciePFMemRsvdalig[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieIoRsvd[PCH_PCIE_MAX_ROOT_PORTS];
//Denverton AptioV Override End- EIP#305575
  
  
  //
  // PCIe LTR Configuration
  //
  UINT8   PchPcieLtrEnable[8];
  UINT8   PchPcieLtrConfigLock[8];
  UINT8   PchPcieSnoopLatencyOverrideMode[8];
  UINT8   PchPcieSnoopLatencyOverrideMultiplier[8];
  UINT8   PchPcieNonSnoopLatencyOverrideMode[8];
  UINT8   PchPcieNonSnoopLatencyOverrideMultiplier[8];
  UINT16  PchPcieSnoopLatencyOverrideValue[8];
  UINT16  PchPcieNonSnoopLatencyOverrideValue[8];

  //
  // GpioInt Configuration
  //
  UINT8   PchGpioIrqRoute;
  UINT8   PchDriverModeTouchPanel;
  UINT8   PchDriverModeTouchPad;
  UINT8   PchDriverModeSensorHub;
  //
  // Interrupt Configuration
  //
  UINT8   PchIoApic24119Entries;
  //
  // PCH Thermal Throttling
  //
  UINT8   PchCrossThrottling;
  //
  // ME Setup entries - start here
  //
  UINT8   MeTimeout;
  UINT8   MeWaitingTimer;    // TODO:

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  UINT8   SpsIccClkSscSettingSupported;
  UINT8   SpsIccClkSscSetting;
  UINT32  SpsAltitude;
  UINT16  SpsMctpBusOwner;
  UINT8   SpsMeShutdown;
#endif

  UINT8   MeDidEnabled;                // Whether DRAM Init Done should be sent to ME
  UINT8   MeDidTimeout;                // DRAM Init Done acknowledge timeout
  UINT8   MeDidInitStat;               // Override DRAM status sent to ME

  UINT8   MeHmrfpoLockEnabled;
  UINT8   MeHmrfpoEnableEnabled;
  UINT8   MeEndOfPostEnabled;
  UINT8   MeGrPromotionEnabled;
  UINT8   MeGrLockEnabled;
  UINT8   MeHeci1Enabled;
  UINT8   MeHeci2Enabled;
  UINT8   MeHeci3Enabled;
  UINT8   MeIderEnabled;
  UINT8   MeKtEnabled;
  UINT8   MeHeci1HideInMe;
  UINT8   MeHeci2HideInMe;
  UINT8   MeHeci3HideInMe;

  UINT8   HsioEnableMessaging;

#if ME_TESTMENU_FLAG
  UINT8   FiaMuxMessageEnabled;
#endif

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  UINT8   NmEnabled;
#endif
  UINT8   NmPwrOptBootOverride;
  UINT8   NmPwrOptBoot;
  UINT8   NmCores2DisableOverride;
  UINT8   NmCores2Disable;
  UINT8   NmPowerMsmtOverride;
  UINT8   NmPowerMsmtSupport;
  UINT8   NmHwChangeOverride;
  UINT8   NmHwChangeStatus;
  UINT8   NmPtuLoadOverride;
  UINT8   TestDisableD0I3SettingForHeci;
  UINT8   EnableMePreDidReset;
  UINT8   WaitResetWarningAck;

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  UINT8         MeSeg;
  UINT8         MeType;
  UINT8         MeFirmwareMode;
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  UINT8         MeImageType;
  UINT8         MeFwDowngrade;
  UINT8         LocalFwUpdEnabled;
  UINT8         MngState;
  UINT8         IfrUpdateEnabled;
  UINT8         MdesCapability;
  UINT8         SvtForPchCap;
  UINT8         FWProgress;
  UINT8         RemoteSessionActive;
  UINT8         AmtCiraRequest;
  UINT8         AmtCiraTimeout;
  UINT8         UnConfigureMe;
  UINT8         HideUnConfigureMeConfirm;
  UINT8         MebxDebugMsg;
  UINT8         USBProvision;
  UINT8         AmtbxSelectionScreen;
  UINT8         AmtbxHotKeyPressed;
  UINT8         MebxNonUiTextMode;
  UINT8         MebxUiTextMode;
  UINT8         MebxGraphicsMode;
  UINT8         MeStateControl;
  UINT8         EwsAvailable;
  UINT8         EwsEnabled;

#endif // AMT_SUPPORT
#endif // ME_SUPPORT_FLAG

#ifdef IE_SUPPORT
  UINT8   IeHeci1Enabled;
  UINT8   IeHeci2Enabled;
  UINT8   IeHeci3Enabled;
  UINT8   IeIderEnabled;
  UINT8   IeKtEnabled;
  UINT16  SubsystemId;
  UINT8   IeDisabledInSoftStraps;
  UINT8   DfxIe;
#endif //IE_SUPPORT

  //
  // DDR setup variables
  //
  UINT8         SuppressChannelOption;
  UINT8         EnableDdr4Options;
  UINT8         CommandAddressParity;          // DDR4 control command address parity

  //
  // MSI redirection algorithm options
  //
  UINT8         MsiRedirAlgorithm;					//MSI Algorithm

  //
  // NTB setup
  //
  UINT8   NtbBarWidth;
  UINT64  NtbBarSize;

  //
  // TraceHub options
  //
  UINT8   TraceHubEnable;
  UINT8   FWTraceEnable;
  UINT8   FWTraceDestination;
  UINT8   Mem0WrapEnable;
  UINT8   Mem1WrapEnable;
  // OVERRIDE_4757168_BEGINS
  UINT8   ProcTraceEnable;
  UINT8   ProcTraceDestination;
  UINT8   ProcTraceOutputScheme;
  UINT8   ProcTraceMemSize;
  // OVERRIDE_4757168_ENDS

  UINT8   EnableSLD;
  //
  // Debug Print Error Level
  //
  // Denverton AptioV Override Start - EIP#235760
  /*
  UINT32 DebugPrintErrorLevel;
  UINT8  DebugPrintInit;
  UINT8  DebugPrintWarn;
  UINT8  DebugPrintLoad;
  UINT8  DebugPrintFS;
  UINT8  DebugPrintPool;
  UINT8  DebugPrintPage;
  UINT8  DebugPrintInfo;
  UINT8  DebugPrintDispatch;
  UINT8  DebugPrintVariable;
  UINT8  DebugPrintBM;
  UINT8  DebugPrintBlkIO;
  UINT8  DebugPrintNet;
  UINT8  DebugPrintUNDI;
  UINT8  DebugPrintLoadfile;
  UINT8  DebugPrintEvent;
  UINT8  DebugGCD;
  UINT8  DebugCache;
  UINT8  DebugPrintVerbose;
  UINT8  DebugPrintError;
  */
  // Denverton AptioV Override End - EIP#235760
  UINT8  Sntp4TimeSync;
  UINT8  Sntp4Server;
  UINT8  FiaMuxOverride;
  UINT8  FiaLaneConfig[ME_FIA_MUX_LANE_NUM_MAX];
  // Denverton AptioV override start - EIP#332934
  UINT8  BifCtl[PCH_MAX_PCIE_CLUSTERS];
  UINT8  FiaPcieRootPortLinkWidth[PCH_PCIE_MAX_ROOT_PORTS];
  // Denverton AptioV override end - EIP#332934
} SYSTEM_CONFIGURATION;


typedef struct {

  UINT8   PmcReadDisable;
  UINT8   PchDciEn;
  UINT8   PchDciAutoDetect;
  UINT8   PchRtcLock;
  UINT8   Hpet;
  UINT8   StateAfterG3;
  UINT8   IchPort80Route;
  UINT8   EnhancePort8xhDecoding;
  UINT8   PchSirqMode;
  UINT8   PciePllSsc;

  //
  // Usb Config
  //
  UINT8   PchUsb20[2];
  UINT8   PchUsbPortDisable;
  UINT8   PchUsbHsPort[PCH_H_XHCI_MAX_USB2_PORTS];
  UINT8   PchUsbSsPort[PCH_H_XHCI_MAX_USB3_PORTS];
  UINT8   UsbPrecondition;
  UINT8   DisableComplianceMode;
  UINT8   IIL1e;
  UINT8   XhciL1e;
  //
  // Sata CONFIG
  //
  //DNV
  UINT8   PchSata[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataSpeedLimit[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataInterfaceMode[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataPort[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataHotPlug[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataMechanicalSw[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataSpinUp[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataExternal[PCH_MAX_PORT_SETUPVAR];
  UINT8   PxDevSlp[PCH_MAX_PORT_SETUPVAR];
  UINT8   EnableDitoConfig[PCH_MAX_PORT_SETUPVAR];
  UINT8   DmVal[PCH_MAX_PORT_SETUPVAR];
  UINT16  DitoVal[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataType[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataTopology[PCH_MAX_PORT_SETUPVAR];
  UINT8   SataAlternateId[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidR0[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidR1[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidR10[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidR5[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidIrrt[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidOub[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataHddlk[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataLedl[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidIooe[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidSrt[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRaidOromDelay[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataRstForceForm[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataSalp[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataLpm[PCH_MAX_SATA_CONTROLLERS];
  UINT8   SataTestMode[PCH_MAX_SATA_CONTROLLERS];
  UINT8   PortMultiplierSupported[PCH_MAX_SATA_CONTROLLERS];

  UINT8   PcieComplianceTestMode;
  UINT8   PcieTopology[MAX_PCIE_LANES];

  //
  // Interrupt Configuration
  //
  UINT8   PchIoApic24119Entries;

  //
  // PCH Thermal Throttling
  //
  UINT8   PchCrossThrottling;

  //
  //HSUART
  //
  UINT8 HsUartMode[PCH_MAX_HSUART]; //0:Pcie, 1:Legacy, 2:Hide
  UINT16 HsUartCOM[PCH_MAX_HSUART]; //If Mode=Legacy, this value is assigned as IO base
// Denverton Aptiov override start
// DBG2 ACPI table is installed by "Debug Port Tables" module  
//  UINT8 HsUartKDPort;	//Defines the HSUART Port used for Kernel Debugging.
//  UINT8 HsUartMdpt; //If PCIE mode is selected then MDPT table is instaled [0:Legacy, 1:Pcie.]
// Denverton Aptiov override End
  UINT8 GpioStatus;




  //W:\Syncs\Denverton\HarrisonvillePkg\Library\PeiPolicyUpdateLib\PeiPchPolicyUpdate.c:784
  //DNV TODO: Add setup option read for tracehub
  //
  // TraceHub Setup Options
  //
  UINT8   TraceHubEnable;
  UINT8   TraceHubFwEnable;
  UINT8   TraceHubFwDestination;
  UINT32  TraceHubMemBaseRegion0;
  UINT32  TraceHubMemBaseRegion1;
  UINT8   TraceHubPtiMode;
  UINT8   TraceHubPtiTraining;
  UINT8   TraceHubPtiSpeed;
  UINT8   TraceHubMemoryEnabled;
  UINT8   TraceHubEnableMode;
  UINT8   MemRegion0BufferSize;
  UINT8   MemRegion1BufferSize;
  //
  // Thermal Throttling Level options
  //
  UINT8   PchTtLevelSuggestSet;
  UINT16  PchThrmT0Level;
  UINT16  PchThrmT1Level;
  UINT16  PchThrmT2Level;
  UINT8   PchThrmTtEnable;
  UINT8   PchThrmTtState13Enable;
  UINT8   PchThrmTtLock;

  //
  // DMI Thermal Throttling
  //
  UINT8   PchDmiTsSuggestSet;
  UINT8   PchTs0Width;
  UINT8   PchTs1Width;
  UINT8   PchTs2Width;
  UINT8   PchTs3Width;
  UINT8   PchDmiTsawEn;
  //
  // SATA Thermal Throttling
  //
  UINT8   PchSataTsSuggestSet;
  UINT8   PchP0T1M;
  UINT8   PchP0T2M;
  UINT8   PchP0T3M;
  UINT8   PchP0TDisp;
  UINT8   PchP0Tinact;
  UINT8   PchP0TDispFinit;
  UINT8   PchP1T1M;
  UINT8   PchP1T2M;
  UINT8   PchP1T3M;
  UINT8   PchP1TDisp;
  UINT8   PchP1Tinact;
  UINT8   PchP1TDispFinit;

  //
  // Miscellaneous options
  //
#ifdef PCH_PO_FLAG
  UINT8   PchPwrOnConfigEnable;
  UINT8   PchNvmSafeMode;
  UINT8   PchGpioSafeMode;
  UINT8   PchDmiSafeMode;
  UINT8   PchSmbusSafeMode;
  UINT8   PchRtcSafeMode;
  UINT8   PchItssSafeMode;
  UINT8   PchP2sbSafeMode;
  UINT8   PchPsthSafeMode;
  UINT8   PchHostPmSafeMode;
  UINT8   PchScsSafeMode;
  UINT8   PchThermalSafeMode;
  UINT8   PchPcieSafeMode;
  UINT8   PchPsfSafeMode;
  UINT8   PchXhciSafeMode;
  UINT8   PchXdciSafeMode;
  UINT8   PchSataSafeMode;
  UINT8   PchFiaSafeMode;
  UINT8   PchLpcSafeMode;
  UINT8   PchIshSafeMode;
  UINT8   PchHdaSafeMode;
  UINT8   PchDciSafeMode;
  UINT8   PchSkyCamSafeMode;
  UINT8   PchSpiSafeMode;
#endif // PCH_PO_FLAG

} PCH_SETUP;

extern EFI_GUID  gSetupVariableGuid;
extern EFI_GUID  gPchSetupVariableGuid;
#endif
