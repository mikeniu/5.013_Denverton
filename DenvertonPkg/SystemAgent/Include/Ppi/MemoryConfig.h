/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved This
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
#ifndef _MEMORY_CONFIG_H_
#define _MEMORY_CONFIG_H_


#define MEMORY_CONFIG_REVISION 1

#define MD_MAX_CHANNELS        2
#define MD_MAX_DIMMS           2
#define MD_MAX_SPD_BYTES_128   128
#define MD_MAX_SPD_BYTES_256   256
#define MD_MAX_SPD_BYTES_512   512

typedef enum {
  MEMORY_DOWN_DISABLE = 0,    /* No memory down and a physical memory slot. */
  MEMORY_DOWN_ENABLE  = 1,    /* Memory down and not a physical memory slot. */
} MemoryDownState;


typedef struct {
  MemoryDownState   EnableMemoryDown[MD_MAX_CHANNELS][MD_MAX_DIMMS];
  UINT16            MaxSpdSize;
  UINT8             MemoryDownSpdData[MD_MAX_CHANNELS][MD_MAX_DIMMS][MD_MAX_SPD_BYTES_512];
} MEMORY_DOWN_CONFIGURATION;

typedef struct {
  UINT32    Low;    // Determines the low range for a memory segment (in MB)
  UINT32    High;   // Determines the high range for a memory segment (in MB)
} SCRUB_SEGMENT;

#define MAX_SCRUB_SEGMENTS    5

typedef struct {
  BOOLEAN         MemorySegmentationEnabled;
  UINT8           NumberOfSegments;
  SCRUB_SEGMENT   ScrubSegment[MAX_SCRUB_SEGMENTS];
} MEMORY_SCRUB_SEGMENTS;

typedef struct {
  INT32  CmdOffsetValue;
  INT32  ClkOffsetValue;
  INT32  CsOffsetValue;
  INT32  CkeOffsetValue;
  INT32  OdtOffsetValue;
} OVERRIDE_CLOCKS_OPTIONS;
typedef struct {
  UINT8   MarginRxDelay;
  UINT8   MarginTxDelay;
  UINT8   MarginRxVoltage;
  UINT8   MarginTxVoltage;
} MARGIN_LEVEL;

typedef struct {
  UINT8         EnableFastBootTester;
  MARGIN_LEVEL  Critical;
  MARGIN_LEVEL  Warning;
} FAST_BOOT_CHECKER;

typedef struct {
  UINT16  BlockId;
  UINT8   BlockRevision; 
  UINT16  BlockSize;

  UINT8   ValidSetup;
  UINT8   CommandMode;
  UINT8   Fastboot;
  UINT8   MrcDebugMsg;
  UINT8   EccSupport;
  UINT8   PatrolSupport;
  UINT8   PatrolPeriod;
  UINT8   DemandScrub;
  UINT16  MaxScrubDebitCount;  
  UINT8   EnableScrambler;
  UINT8   EnableSlowPowerDownExit;
  BOOLEAN MemoryPreservation;
  UINT8   DynamicSelfRefresh;
  UINT8   DynamicPowerModeOpcode;
  UINT8   SelectRefreshRate;
  UINT8   ForceAxMapping;
  UINT8   DdrFreq;
  BOOLEAN EnableParallelTraining;
  UINT8   HalfWidthEnable;
  UINT8   TclIdle;
  UINT8   DdrChannels;
  UINT8   CommandAddressParity;
  UINT8   MrcRmtSupport;
  UINT8   OpenPolicyTimer;
  UINT8   SmmSize;
  UINT8   OutOfOrderAgingThreshold;
  UINT8   EnableOutOfOrderProcessing;
  UINT8   TwoClkRdPreamble;
  UINT8   TwoClkWrPreamble;
  UINT8   WriteDataEarlyEnable;
  UINT8   EnableNewRequestBypass;
  UINT8   AbSegToDram;
  UINT8   ESegToDram;
  UINT8   FSegToDram;
  UINT8   VrefOverrideEnable;
  UINT8   VrefOverrideValue;
  UINT8   ZqcEnable;
  UINT8   CmdRate;
  UINT8   TclTiming;
  UINT8   TRcdTiming;
  UINT8   TRpTiming;
  UINT8   TRasTiming;
  UINT8   TRtpTiming;
  UINT8   TRrdTiming;
  UINT8   TFawTiming;
  UINT8   TCcdTiming;
  UINT8   TWtpTiming;
  UINT8   TWclTiming;
  UINT8   MrcRmtCpgcExpLoopCntValue;
  UINT8   MrcRmtCpgcNumBursts;
  UINT16  MitigationProbability;
  UINT8   FaultyPartTracking;              
  UINT8   FaultyPartContinueOnCorrectable;
  UINT16  MmioHighSize;
  UINT8   ReadPerBitEnable;
  UINT8   WritePerBitEnable;
  UINT8   OverrideCheckpoints;
#if ME_SUPPORT_FLAG
  UINT8   MeSeg;
#endif
  UINT8   Rk2RkEn;
  UINT8   RtitSupportEnable;
  UINT8   MrcMemoryTestLoop;
  UINT8   BramParityEnable;
  UINT8   MrcHaltSystemOnMemTestError;
  UINT8   MrcResetLoop;
  UINT8   MrcResetLoopCycles;
  UINT8   MmioSize;
  UINT8   MrcTrainingCpgcExpLoopCntValue;
  UINT8   MrcTrainingCpgcNumBursts;
  UINT16  LoopCount;
  UINT8   InputEnableBunitPerformance;
  UINT8   BestEffortMaxLat;
  UINT8   PageHitDelay;
  UINT8   IsocBankPrefetch;
  UINT8   BestEffortBankPref;
  UINT8   BDebugSelfRefresh;
  UINT16  SelfRefreshDelay;
  UINT16  CorrectErrThreshold;
  UINT8   CkePowerDownDisable;
  UINT32  ClttThermReg0;
  UINT32  ClttThermReg1;
  UINT32  ClttEventTrip;
  UINT8   InputPassGateEnabled;
  UINT8   InputPassGateEnableSelfRefresh;
  UINT8   InputPassGateDirection;
  UINT16  InputPassGateRepetitionCountRange;
  UINT16  InputPassGateIterationOnRow;
  UINT8   InputPassGateSwizzle;
  UINT8   InputPassGatePattern;
  UINT8   InputPassGateTargetPattern;
  UINT8   InputSpeedSelected;
  UINT8   BankXorMapEnable;
  UINT8   InputPassGatePartial;
  UINT32  InputPassGateRowBankMin;
  UINT32  InputPassGateRowBankMax;
  UINT8   InputPassGateMonteCarloEnable;
  UINT8   InputMontecarloMaxFailures;
  UINT16  InputMontecarloStartRepetition;
  UINT16  InputMontecarloDecrement;
  UINT8   InputPassGateRankEnable[2][4];
  UINT8   PmopPc0;
  UINT8   PmopPcx;
  UINT8   InputFineDdrVoltage;
  UINT8   BmcModeEnable;
  UINT8   InterleaveMode;
  //
  // Leaky Bucket
  //
  UINT8   LeakyBucketUnits;
  UINT64  LeakyBucketRate[2][4];
  UINT16  CorrectableErrorThreshold[2][4];
  //
  // Memory Thermal Throttling
  //
  UINT8   MemoryThermalEnable;
  UINT8   ThermalThrottlingType;
  UINT8   ClttMode;
  UINT8   TempHi;
  UINT8   TempMid;
  UINT8   TempLo;
  UINT8   BwLevelOnThrtCrit;
  UINT8   BwLevelOnThrtHi;
  UINT8   BwLevelOnThrtMid;
  UINT8   OlttThrtBw;
  //
  // MEMHOT
  //
  UINT8   MemHotThrotLvl;
  //
  // MEMTRIP
  //
  UINT8   EnMemTrip;
  UINT8   TempMemTrip;
  //
  // Setting RxTxSkewCtl
  //
  UINT8 RxSkewCtl;
  UINT8 TxSkewCtl;
  //
  // Performance profile for DMAP
  //
  UINT8 PerformanceProfile;
  //
  // Dram Rapl
  //
  UINT64 DramRaplTimeWindow;
  UINT32 DramRaplPowerLimit;
  UINT8  DramRaplPowerLimitEnable;
  UINT8  DramRaplLimitLock;
  BOOLEAN EnableParityCheck;
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
  OVERRIDE_CLOCKS_OPTIONS   OverrideClocks;
  //
  // Dimm Spd Address
  //
  UINT8   SpdAddressTable[MD_MAX_CHANNELS][MD_MAX_DIMMS];

  MEMORY_DOWN_CONFIGURATION MemDownConfiguration;
  //
  // Memory Scrub Segments
  //
  MEMORY_SCRUB_SEGMENTS MemoryScrubSegments;
  //
  // Fast Boot Margins Tester
  //
  FAST_BOOT_CHECKER FastBootChecker;
  //
  // SMBus SPD Write Disable
  //
  BOOLEAN SpdWriteDisable;
  //
  // Skip Memory Test
  //
  UINT8 SkipMemoryTest;

} MEMORY_CONFIGURATION;


#endif // _MEMORY_CONFIG_H_
