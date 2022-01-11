/**  @file
  McFunc.c
  This file contain memory DIMM initialization function.

  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "McFunc.h"
#include "MrcFunc.h"
#include "IchRegs.h"
#include "Regs.h"

#include "ConfigMem.h"
#include "ConfigMemData.h"
#include "MmrcHooks.h"
#include "Mailbox.h"
#include "MSysProg.h"
#ifdef JTAG
#include <stdio.h>
#endif


//
// This routine invokes the Denali model to initialize all of its parameters in RTL simulation.
//
MMRC_STATUS
SimSetDunitConfigValid (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
)
{
#if SIM
  MySetDunitConfigValid();
#endif
  return MMRC_SUCCESS;
}

UINT8
GetSocStepping ()
{
  UINT8  RevId;
#ifndef SIM
  //
  // Ported from PchStepping on PchInforLib.c
  //
  RevId = PCI_CFG_32B_READ_CF8CFC (0x00, 0x1F, 0x00, 0x8) & 0xff;
#else
  //
  // If in simulation, force A0 stepping.
  //
  RevId = 0x00;
#endif
  return RevId;
}

UINT16
GetCpuType ()
{
  UINT16 Data16;
#if !defined SIM && !defined JTAG
  _asm {

    push    eax
    push    ebx
    push    ecx
    push    edx

    mov     eax, 01h
    cpuid

    and     eax, 00000FF0h

    mov     Data16, ax
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
  }
#else
  //
  // If in simulation, force AVN CPU type.
  //
  Data16 = 0x6D0;
#endif
  return Data16;
}

/**
  Initialize the MRC parameters with default values and/or from setup options

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
MrcInitializeParameters (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 Channel;
  UINT8 i;
  UINT8 Rank;

  Rank = 0;
  //
  // Initialize data structures and save the Mrc structure address
  //
  MmrcMemset (MrcData, 0, sizeof (MMRC_DATA));
  //
  // Some routines require the MrcData host structure but we don't pass the structure
  // to them for several reasons:
  //   1) To save code space - 1 less parameter per function call on routines which
  //      are frequently called.
  //   2) Macros such as MrcDeadLoop() are able to be used from any routine. If a
  //      routine does not have MrcData as a parameter, it can't pass it to MrcDeadLoop().
  //
  SaveMrcHostStructureAddress (MrcData);
  //
  // Set level of MRC serial debug messages.
  //
  MrcData->MspData.DebugMsgLevel = MSP_DBG_VERBOSE;
  //
  // Initialize the pointer to the MspData
  //
  MspInit (&MrcData->MspData, MrcData, &SetMspData);
  //
  // Get SOC stepping.
  //
  MrcData->SocStepping = GetSocStepping();
  if (MrcData->SocStepping > DNV_B0_STEP) {
    MrcData->SocStepping = DNV_B0_STEP;
  }

  MrcData->CpuType     = GetCpuType();
  //
  // Fast boot disabled by default.
  //
  MrcData->FastBootEnable = 0;
  //
  // PowerOn is set to 1 initially for power on to enforce
  // safe settings.  Once memory is stable, we can set this
  // to 0 to enable additional features and power savings.
  //
  MrcData->PowerOn = 0;
  //
  // PpoPlatform is set as soon as the PPO starts
  // set this to 'TRUE' if an AVN platform is
  // under use such as (LPK or MPK)
  //
#if defined (DNV_PPO_SUPPORT) && (DNV_PPO_SUPPORT == 1)
  MrcData->PpoPlatform = TRUE;
#else
  MrcData->PpoPlatform = FALSE;
#endif
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    MrcData->MrcParameters.ChEnabled[Channel] = TRUE;
    MrcData->TotalChannels++;
    //
    // Enable or disable based on number of strobes defined at build time.
    //
    if (MAX_STROBES == 8 || MAX_STROBES == 16) {
        MrcData->MrcParameters.EccEnabled[Channel] = FALSE;
    } else if (MAX_STROBES == 9 || MAX_STROBES == 18) {
        MrcData->MrcParameters.EccEnabled[Channel] = TRUE;
    } else {
      MrcDeadLoop();
    }
    //
    // Bitmask of all strobes which go to ECC DRAMs.
    //
    MrcData->EccStrobes[Channel] = BIT8 | BIT17;
    //
    // Initialize all timing data overrides to not applicable. 
    //
    for (i = 0; i < MaxTimingData; i++) {
      MrcData->TimingDataOverrides[Channel][i][FreqA] = MMRC_NA;
      MrcData->TimingDataOverrides[Channel][i][FreqB] = MMRC_NA;
    }
  }
  //
  // EXP_LOOP_CNT field of CPGC_CMD_CTL. This CANNOT be less than 4.
  //
  MrcData->MrcTrainingCpgcExpLoopCnt  = 10;
  MrcData->MrcCpgcNumBursts           = 8;
#ifdef SIM
  MrcData->MrcTrainingCpgcExpLoopCnt  = 1;
  MrcData->MrcCpgcNumBursts           = 2;
#endif
  MrcData->MrcRmtCpgcExpLoopCnt       = 8;
  MrcData->MrcRmtCpgcNumBursts        = 6;
  //
  // Only test memory once by default.
  //
  MrcData->MemoryTestLoop = 0;
  //
  // LoopCount for MemoryTest.
  //
  MrcData->LoopCount = 1;
  //
  // By default MCI should be enabled
  //
  MrcData->MciIehN = 1;
  //
  // Halt the system if an error is observed during memory test.
  //
  MrcData->HaltSystemOnMemTestError = MrcHaltSystemOnError;
  //
  // Do not infinitely loop MRC.
  //
  MrcData->ResetLoop = 0;
  //
  // Defines the cycles for MRC Reset Loop Test 
  //
  MrcData->ResetLoopCycles = 1;
  //
  // Enable MRC DIMM Vref Override
  //
  MrcData->VrefOverrideValue  = 65;
  MrcData->VrefOverrideEnable = 0;
  //
  // Start the fine voltage at VDDQ_CENTER which is '0'
  //
  MrcData->MrcParameters.SaveRestore.InputFineDdrVoltage = VDDQ_CENTER;
#if ME_SUPPORT_FLAG
  //
  // Size requested for ME UMA 
  //
  MrcData->MeSeg = FALSE;
  MrcData->MeUmaSize = 0;
#endif
  //
  // ZQ Calibration Short Interval enabled by default
  //
  MrcData->ZqcEnable = TRUE;
  //
  // Performance settings for out of order processing in the DUNIT.
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    MrcData->TwoClkRdPreamble[Channel] = 0;
    MrcData->TwoClkWrPreamble[Channel] = 0;
    MrcData->WriteDataEarlyEnable[Channel] = 0;
  }
  if (MrcData->PowerOn) {
    MrcData->OutOfOrderAgingThreshold = 8;
  } else {
    MrcData->OutOfOrderAgingThreshold = 0x1F;
  }
  MrcData->OutOfOrderDisable        = 0;
  MrcData->NewRequestBypassDisable  = 0;

  //
  // Setting Rank to Rank.
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    MrcData->Rk2Rk[Channel] = 1;
  }
  //
  // Scrambler Disabled by default
  //
  MrcData->EnableScrambler = 0;
  //
  // Slow power down exit Disabled by default
  //
  MrcData->EnableSlowPowerDownExit = 0;
  //
  // Open Policy Timer Disabled
  //
  MrcData->OpenPolicyTimer = 0;

  //
  // Auto
  // Forcing to be 1600 per Power On requirement
  //
  MrcData->MrcParameters.DdrFrequency = MMRC_1600;

  MrcData->PatrolScrubEnable    = 0;
  MrcData->PatrolScrubPeriod    = 0;
  MrcData->DemandScrub          = 0;
  MrcData->MaxScrubDebitCount   = 0;
  //
  // dpmc0[sredly] Self Refresh delay.
  // Default value = 30us
  //
  MrcData->MrcParameters.SaveRestore.SreDly = 30;
  //
  // dpmc0[dynpmop] PMOP Levels
  // Default value = Slow Exit
  // For both RDIMMs and SODIMMs.
  //
  MrcData->DynPmop = PMOP_SLOW_EXIT;
  //
  // Dynamic Self Refresh disabled by default at PO.
  //
  if (MrcData->PowerOn) {
    MrcData->DynSR = 0;
  } else {
    MrcData->DynSR = 1;
  }
  MrcData->SelectRefreshRate  = Rate1x;
  //
  // CKE Power Down
  // 
  MrcData->CkePowerDownDisable = 1;
  //
  // Enable this to turn off cke, clk, and odt pins for unpopulated ranks.  This
  // has been tested and works on VLV.  However, in order to avoid risk to AVN
  // power on, leave this off until it can be tested on AVN.
  //
  MrcData->EnableCkeByRank = FALSE;
  //
  // Direct ABSEG to SSA by default.
  //
  MrcData->AbSegToDram          = 0;
  //
  // Direct ESEG & FSEG to DRAM by default.
  //
  MrcData->ESegToDram           = 1;
  MrcData->FSegToDram           = 1;
  //
  // Bank Xor Mapping disabled by default
  //
  MrcData->BankXorMapEnable     = 0;
#if INTERLEAVING_SUPPORT
  //
  // Interleaving Mode 2 set by default
  // 
  MrcData->InterleaveMode       = 3;
#endif
#if MITIGATION_FEATURE == 1
  //
  // Disable Mitigation feature
  //
  MrcData->MitigationProbability  = 0;
#endif // MITIGATION_FEATURE
#if PASS_GATE_TEST == 1
  MrcData->PassGateParameters.EnableTest                = FALSE;
  MrcData->PassGateParameters.EnablePeriodicSelfRefresh = FALSE;
  MrcData->PassGateParameters.Direction                 = 0;
#ifdef SIM
  MrcData->PassGateParameters.ActivationsCount          = 4;
#else
  MrcData->PassGateParameters.ActivationsCount          = 600000;
#endif
  MrcData->PassGateParameters.IterationOnRow            = 1;
  MrcData->PassGateParameters.SwizzleMode               = 0;
  MrcData->PassGateParameters.Pattern                   = 0;
  MrcData->PassGateParameters.TargetPattern             = 1;
  MrcData->PassGateParameters.SpeedSelect               = 8;

  MrcData->PassGateParameters.EnablePartialTest         = TRUE;
  MrcData->PassGateParameters.PartialRowBankMin         = (0x1000 << 4 | 0x0);
  MrcData->PassGateParameters.PartialRowBankMax         = (0x1000 << 4 | 0x8);

  MrcData->PassGateParameters.EnableMontecarloSearch    = FALSE;
  MrcData->PassGateParameters.MontecarloMaxFailures     = 1;
  MrcData->PassGateParameters.MontecarloStartRepetition = 600000;
  MrcData->PassGateParameters.MontecarloDecrement       = 1000;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      MrcData->PassGateParameters.RankTest[Channel][Rank] = TRUE;
    }
  }

#endif // PASS_GATE_TEST

#if defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      //
      // Guardkey is always executed not matter if Hard or Soft
      //
      MrcData->PprParameters.GuardKey[Channel][Rank / 2] = TRUE;
    }
  }
#endif // defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)

#if LEAKY_BUCKET == 1
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      //
      // Default Leak Rate in Microseconds
      //
      MrcData->LeakyBucketParameters.RateUnits = 0;
      MrcData->LeakyBucketParameters.Rate[Channel][Rank] = 0;
      MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][Rank] = 0;
    }
  }
#endif // LEAKY_BUCKET

#if DRAM_RAPL == 1
  MrcData->DramRaplParameters.TimeWindow = 0;
  MrcData->DramRaplParameters.PowerLimit = 0;
  MrcData->DramRaplParameters.PowerLimitEnable = 0;
#endif // DRAM_RAPL

#if defined SIM || defined JTAG
  MrcData->MrcRmtSupport     = 0;
#endif
  //
  // Enable/Disable Parallel training when the CPGC engine is used to train the memory
  //
  MrcData->EnableParallelTraining = FALSE;
  //
  // Enable/Disable Rmt Support and Signals Independently
  //
  MrcData->MrcRmtSupport          = RMT_SUPPORT_ENABLED; //| RMT_SUPPORT_OVERRIDE;
  MrcData->MrcRmtEnableSignal     = RMT_ALL_TESTS_EXEC & (~RMT_REC_EN_EXEC) & (~RMT_TX_DQS_EXEC);
  //
  // Memory Preservation through warm reset
  //
  MrcData->MemoryPreservation     = FALSE;
  //
  // Select support for AnalogDll
  //
  MrcData->DigitalDll = 0;
  //
  // Set frequency 0.
  //
  MrcData->DVFSFreqIndex = 0;
  //
  // Set Defualt Values for 
  // Memory Thermal Throttling Variables.
  //
  MrcData->MemoryThermalEnable = 0;
  MrcData->ThermalThrottlingType = 0;
  MrcData->ClttMode = 0;
  MrcData->TempHi = 105;
  MrcData->TempMid = 85;
  MrcData->TempLo = 82;
  MrcData->BwLevelOnThrtCrit = 3;
  MrcData->BwLevelOnThrtHi = 10;
  MrcData->BwLevelOnThrtMid = 100;
  MrcData->OlttThrtBw = 90;
  //
  // Set Default Value for MEMHOT
  //
  MrcData->MemHotThrotLvl = 3;
  //
  // Set Default Values for MEMTRIP
  //
  MrcData->EnMemTrip = 0;
  MrcData->TempMemTrip = 105;

#ifdef SIM
  MrcData->SkipTrainingAlgos[LRxVoc]                = TRUE;
  MrcData->SkipTrainingAlgos[LReceiveEnable]        = TRUE;
  MrcData->SkipTrainingAlgos[LEarlyMpr]             = TRUE;
  MrcData->SkipTrainingAlgos[LFineWriteLeveling]    = TRUE;
  MrcData->SkipTrainingAlgos[LCoarseWriteLeveling]  = TRUE;
  MrcData->SkipTrainingAlgos[LReadTraining]         = TRUE;
  MrcData->SkipTrainingAlgos[LWriteTraining]        = TRUE;
  MrcData->SkipTrainingAlgos[LCommandTraining]      = TRUE;
  MrcData->SkipTrainingAlgos[LMemoryTest]           = TRUE;
#else
  if (MrcData->SocStepping > DNV_A1_STEP) {
    MrcData->SkipTrainingAlgos[LRxVoc]              = FALSE;
  } else {
    MrcData->SkipTrainingAlgos[LRxVoc]              = TRUE;
  }
  MrcData->SkipTrainingAlgos[LReceiveEnable]        = FALSE;
  MrcData->SkipTrainingAlgos[LEarlyMpr]             = FALSE;
  MrcData->SkipTrainingAlgos[LFineWriteLeveling]    = FALSE;
  MrcData->SkipTrainingAlgos[LCoarseWriteLeveling]  = FALSE;
  MrcData->SkipTrainingAlgos[LReadTraining]         = FALSE;
  MrcData->SkipTrainingAlgos[LWriteTraining]        = FALSE;
  MrcData->SkipTrainingAlgos[LCommandTraining]      = FALSE;
  MrcData->SkipTrainingAlgos[LMemoryTest]           = FALSE;
#endif
  //
  // Initialize other components such as UART, PUNIT, HPET, etc.
  //
  ExternalSiliconInit (MrcData);
#ifdef FSP_FLAG
  //
  // Set level of MRC serial debug messages before OEM hooks.
  //
  MrcData->MspData.DebugMsgLevel = MSP_DBG_NONE;
#endif //FSP_FLAG
  //
  // Detect the platform type, interposer ID, voltage startup...
  //
  OemGetPlatformType (MrcData);
  //
  // Setup basic MRC OEM information such as SPD addresses, 
  // TSEG, VREF SMB Addresses
  //
  OemSetMrcData (MrcData);
#ifdef FSP_FLAG
  //
  // Set level of MRC serial debug messages after OEM hooks.
  //
  MrcData->MspData.DebugMsgLevel = MSP_DBG_VERBOSE;
#endif //FSP_FLAG
  //
  // Determine the boot mode
  //
#if _SIMIC_ == 0 && VP_FLAG == 0
  DetermineBootMode (MrcData);
#else
  MrcData->MrcParameters.BootMode = S5;
#endif
  //
  // Only use to get more information from emulation
  //
  MrcData->MspData.EmulationOverrides.ConvertAccessToAssembly = FALSE;
  MrcData->MspData.EmulationOverrides.ConvertMchBarToP2SBAR   = FALSE;
  if (MrcData->MspData.EmulationOverrides.ConvertAccessToAssembly) {
    MrcData->MspData.DebugMsgLevel |= MSP_DBG_REG_ACCESS;
  }

  MrcData->SkipOptions = 0;
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    //
    // Start at 3N
    //
    MrcData->CurrentMode[Channel] = 3;
    if (MrcData->MspData.CurrentPlatform[Channel] == P_SIMICS) {
      MrcData->SkipOptions |= SKIP_DELAY | SKIP_TEST;
      MspDebugPrint((MSP_DBG_MAX, "Skip Pass Gate test and delays\n"));
    }

    if (MrcData->MspData.CurrentPlatform[Channel] == P_EMU ||
        MrcData->MspData.CurrentPlatform[Channel] == P_EMU_DUNIT) {
      MrcData->SkipOptions |= SKIP_MEMINIT;
    }
  }
  //
  // RxTxSkewCtl for PMO and Memory Vendors functional testing. 0: No skew 1: +2 2: -2 ticks
  //
  MrcData->RxSkewCtl = 0;
  MrcData->TxSkewCtl = 0;
  //
  // Performance profile for DMAP 
  //    Options:
  //       1: 17_19_13_18 & 17_19_13_-
  //       2: 17_19_6_18  & 17_19_6_-
  //       3: 17_19_6_7
  //
  MrcData->PerformanceProfile = 1;
  //
  // Override Settings (init)
  //
  
  MrcData->OverrideClocks.CmdOffsetValue = 0;
  if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1) {  
    MrcData->OverrideClocks.ClkOffsetValue = 64;
    MrcData->OverrideClocks.CsOffsetValue  = 64;
    MrcData->OverrideClocks.CkeOffsetValue = 64;
    MrcData->OverrideClocks.OdtOffsetValue = 64;
  } else {
    MrcData->OverrideClocks.ClkOffsetValue = 0;
    MrcData->OverrideClocks.CsOffsetValue  = 0;
    MrcData->OverrideClocks.CkeOffsetValue = 0;
    MrcData->OverrideClocks.OdtOffsetValue = 0;
  }

  //
  // Disable Per Bit Training
  //
  MrcData->ReadPerBitEnable   = TRUE;
  MrcData->WritePerBitEnable  = TRUE;
  MrcData->EarlyWriteTrainingEnable = TRUE;
  //
  // Max Threshold Per Bit
  //
  MrcData->MaxThresholdPerBit = MAX_THRESHOLD_PER_BIT_RWT;
  //
  // Correct Err Threshold set to 16384 (by default)
  //
  MrcData->CorrectErrThreshold = 16384;
  //
  // Enable SMBus SPD Write Disable function by default
  //
  MrcData->SpdWriteDisable = 0;
  return MMRC_SUCCESS;
}

/**
  Calculate timing parameters dependent on other parameters 
  gotten from the SPD Data Bytes 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
TimingParams (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8  Channel;
  UINT16 TCmd;
  UINT16 TWcl;
  UINT16 TWtp;
  UINT16 TFaw;
  UINT16 TCcd;

  TCmd  = 0;
  TWcl  = 0;
  TWtp  = 0;
  TFaw  = 0;
  TCcd  = 0;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->MrcParameters.ChEnabled[Channel]) {
      //
      // Default values in case DFX wraps off
      // the rest of the options
      //
      MrcData->MrcParameters.TimingData[Channel][TimingTrtp][FreqA] = MAX (MrcData->MrcParameters.TimingData[Channel][TimingTrtp][FreqA], 4);
      //
      // Assume we don't have any 8Gb density (DDR3) nor x8 (DDR4) width devices and set tFAW. Will patch later if we       // do find 8Gb on the channel.
      //
      if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4){
        MrcData->MrcParameters.TimingData[Channel][TimingTfaw][FreqA] = mFawTableDdr4[MrcData->MrcParameters.DdrFrequency - MINDDR][0];
      } else if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
        MrcData->MrcParameters.TimingData[Channel][TimingTfaw][FreqA] = mFawTableDdr3[MrcData->MrcParameters.DdrFrequency - MINDDR][0];
      }
      
      if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
        //
        // TCCD is always 4
        //
        MrcData->MrcParameters.TimingData[Channel][TimingTccd][FreqA] = 4;
      }

      //
      //  tWTP to JEDEC Standard                            WL              +  4  +                    TWR
      //  tWCL to JEDEC Standard                            WL
      //
      if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
        MrcData->MrcParameters.TimingData[Channel][TimingTwtp][FreqA] = mDdr3Freq2WLTable[MrcData->MrcParameters.DdrFrequency - MINDDR] + 0x4 + MrcData->MrcParameters.TimingData[Channel][TimingTwr][FreqA];
        MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA] = mDdr3Freq2WLTable[MrcData->MrcParameters.DdrFrequency - MINDDR];
      } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
        MrcData->MrcParameters.TimingData[Channel][TimingTwtp][FreqA] = mDdr4Freq2WLTable[MrcData->MrcParameters.DdrFrequency - MINDDR] + 0x4 + MrcData->MrcParameters.TimingData[Channel][TimingTwr][FreqA];
        MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA] = mDdr4Freq2WLTable[MrcData->MrcParameters.DdrFrequency - MINDDR];
      }
      //
      // For PO the CMD will be set to 3N by default
      //
      MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA] = 3;
      TCmd = MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA];
      TWcl = MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA];
      TWtp = MrcData->MrcParameters.TimingData[Channel][TimingTwtp][FreqA];
      TFaw = MrcData->MrcParameters.TimingData[Channel][TimingTfaw][FreqA];
      TCcd = MrcData->MrcParameters.TimingData[Channel][TimingTccd][FreqA];
      MspDebugPrint((MSP_DBG_MIN, "Ch %d %dN timing\n", Channel, TCmd));
      MspDebugPrint((MSP_DBG_MIN, "Ch %d TWCL  = %d\n", Channel, TWcl));
      MspDebugPrint((MSP_DBG_MIN, "Ch %d TWTP  = %d\n", Channel, TWtp));
      MspDebugPrint((MSP_DBG_MIN, "Ch %d TFAW  = %d\n", Channel, TFaw));
      MspDebugPrint((MSP_DBG_MIN, "Ch %d TCCD  = %d\n", Channel, TCcd));
    }
  }
  if (MrcData->MrcParameters.TimingData[0][TimingTcmd][FreqA] == 2 || MrcData->MrcParameters.TimingData[1][TimingTcmd][FreqA] == 2) {
    //
    // Override Settings (init)
    //
    
    if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1) {
      MrcData->OverrideClocks.CmdOffsetValue = (280 - 192 + 64);
    } else {
      MrcData->OverrideClocks.CmdOffsetValue = (280 - 192);
    }

    MspDebugPrint((MSP_DBG_MIN, "CmdOffsetValue = %d\n", MrcData->OverrideClocks.CmdOffsetValue));
  } else if (MrcData->MrcParameters.TimingData[0][TimingTcmd][FreqA] == 1 || MrcData->MrcParameters.TimingData[1][TimingTcmd][FreqA] == 1) {
    //
    // Override Settings (init)
    //
    if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1) {
      MrcData->OverrideClocks.CmdOffsetValue = (220 - 192 + 64);
    } else {
      MrcData->OverrideClocks.CmdOffsetValue = (220 - 192);
    }
  } else {
    //
    // Override Settings (init)
    //
    if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1) {
      MrcData->OverrideClocks.CmdOffsetValue = (340 - 192 + 64);
    } else {
      MrcData->OverrideClocks.CmdOffsetValue = (340 - 192);
    }
  }
  MspDebugPrint((MSP_DBG_MIN, "CmdOffsetValue = %d\n", MrcData->OverrideClocks.CmdOffsetValue));

  return MMRC_SUCCESS;
}

/**
  Overrides timing parameter detected from Dimms

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
OverrideTimingParams (
  IN  OUT   MMRC_DATA   *MrcData
)
{
  UINT8 Channel;
  UINT8 i;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->MrcParameters.ChEnabled[Channel]) {
      //
      // Now override any timing data that the user manually input from BIOS setup.
      //
      for (i = 0; i < MaxTimingData; i++) {
        if (MrcData->TimingDataOverrides[Channel][i][FreqA] != MMRC_NA) {
          MrcData->MrcParameters.TimingData[Channel][i][FreqA] = MrcData->TimingDataOverrides[Channel][i][FreqA];
          MspDebugPrint ((MSP_DBG_MIN, "Overriding [%d] = %d\n", i, MrcData->MrcParameters.TimingData[Channel][i][FreqA]));
        }
      }
    }
  }
  return MMRC_SUCCESS;
}

/**
  Initialize the Modular MRC parameters with values detected
  by the MRC. This is necessary so MMRC has its own copies of
  variables it needs to operate correctly (like which ranks
  are enabled, what the timing parameters are, etc).

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
FillModMrcDataStructure (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 Channel;
  UINT8 Rank;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    switch (MrcData->SocStepping) {
    case DNV_A0_STEP:
      MrcData->MspData.CurrentConfiguration[Channel] = C_DNV_A0;
      break;
    case DNV_A1_STEP:
      MrcData->MspData.CurrentConfiguration[Channel] = C_DNV_A1;
      break;
    case DNV_B0_STEP:
      MrcData->MspData.CurrentConfiguration[Channel] = C_DNV_B0;
      break;
    default:
      MrcData->MspData.CurrentConfiguration[Channel] = C_DNV_A0;
      break;
    }
    MrcData->MspData.CurrentFrequency[Channel] = 1 << (MrcData->MrcParameters.DdrFrequency - MINDDR);
    //
    // Translate from standard enumerated memory types to the types defined in the spreadsheet.
    //
    switch (MrcData->MrcParameters.DramType[Channel]) {
    case TypeDdr3L:
      MrcData->MspData.CurrentTechnology[Channel] = T_DDR3L;
      break;
    case TypeDdr4:
      MrcData->MspData.CurrentTechnology[Channel] = T_DDR4;
      break;
    default:
      MrcData->MspData.CurrentTechnology[Channel] = T_DDR3L;
      break;
    }
    MspDebugPrint ((MSP_DBG_MED,"CH%d P: 0x%02X\nCH%d F: 0x%02X\nCH%d C: 0x%02X\nCH%d T: 0x%02X\n", 
      Channel,
      MrcData->MspData.CurrentPlatform[Channel],
      Channel,
      MrcData->MspData.CurrentFrequency[Channel],
      Channel,
      MrcData->MspData.CurrentConfiguration[Channel],
      Channel,
      MrcData->MspData.CurrentTechnology[Channel]
    ));
    //
    // Tell MMRC which ranks are enabled.
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      MrcData->RankEnabled[Channel][Rank] = MrcData->MrcParameters.RankPresent[Channel][Rank];
    }
  }
  return MMRC_SUCCESS;
}

/**
  This function gets the platform setup

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
GetPlatformSettings (
  IN  OUT   MMRC_DATA   *MrcData
  )
{

#if !defined SIM && !defined JTAG
  UINT32        Data32;
  UINT16        Data16;
  UINT8         Data8;
#endif

#if !defined SIM && !defined JTAG
  MrcData->EcBase = PCI_CFG_32B_READ_CF8CFC (MC_BUS, 0x00, 0x00, 0x60) & 0xFFFFFFFE;
#endif

  //
  // Setting ECBase
  //
  if (MrcData->EcBase == 0) {
    MrcData->EcBase = EC_BASE;

#if !defined SIM && !defined JTAG
    PCI_CFG_32B_WRITE_CF8CFC (MC_BUS, 0x00, 0x00, 0xD4, (MrcData->EcBase | BIT0) );
    PCI_CFG_32B_WRITE_CF8CFC (MC_BUS, 0x00, 0x00, 0xD8, 0);
    PCI_CFG_32B_WRITE_CF8CFC (MC_BUS, 0x00, 0x00, 0xD0, ((CMD_WRITE_REG << 24)  + (BUNIT_PORT_ID << 16) + (BUNIT_BECREG << 8) + 0xF0) );
#endif
  }
  MspDebugPrint ((MSP_DBG_MIN, "BECREG = %08X\n", MrcData->EcBase | BIT0));
  //
  // Get MCHBAR Register
  //
#if !defined JTAG
  if (MrcData->MchBar == 0) {
    MrcData->MchBar = (UINT64) PCI_CFG_32B_READ (MrcData->EcBase, MC_BUS, 0, 0, 0x48) & ~ (BIT0);
  }
#endif
  MspDebugPrint ((MSP_DBG_MIN, "MCHBAR = %08X\n", MrcData->MchBar));
  //
  // Get P2SB BAR Register
  //
#if !defined JTAG
  if (MrcData->SbRegBase == 0) {
    MrcData->SbRegBase = PCI_CFG_32B_READ (MrcData->EcBase, MC_BUS, 0x1F, 0x1, 0x10) & ~ (BIT2);
  }
#endif
  MspDebugPrint ((MSP_DBG_MIN, "SBREG = %08X\n", MrcData->SbRegBase));

#if !defined SIM && !defined JTAG
  if (MrcData->SmbusBar == 0) {
    MrcData->SmbusBar = PCI_CFG_16B_READ (MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x20) & ~ (BIT1 | BIT0);
  }
  //
  // Init SMBus controller.
  //
  Data8 = PCI_CFG_8B_READ(MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x40); // HST_EN
  Data8 |= BIT0;
  PCI_CFG_8B_WRITE(MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x40, Data8); // HST_EN

  Data16 = PCI_CFG_16B_READ(MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x04); // IOSE
  Data16 |= BIT0;
  PCI_CFG_16B_WRITE(MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x04, Data16); // IOSE

  MspDebugPrint ((MSP_DBG_MIN, "Smbus BAR = %08X\n", MrcData->SmbusBar));
  //
  // TODO: Get the GPIO base may not be needed for DNV MRC, in the past was used for low DDR voltage switching
  //
  if (MrcData->GpioBar == 0) {
    MrcData->GpioBar = PCI_CFG_16B_READ (MrcData->EcBase, MC_BUS, 0x1F, 0x0, 0x48) & ~ (BIT1 | BIT0);
  }
  MspDebugPrint ((MSP_DBG_MIN, "GpioBar = %08X\n", MrcData->GpioBar));
  //
  // PMCBASE or PWRMBASE (PM Base Address) are different names for this Bar
  //
  if (MrcData->PmcBar == 0) {
    MrcData->PmcBar = PCI_CFG_32B_READ (MrcData->EcBase, MC_BUS, 0x1F, 0x2, PWRMBASE_PMC_PCI_CFG_REG);
    MspDebugPrint ((MSP_DBG_MIN, "PmcBar = %08X\n", MrcData->PmcBar));
  }
  //
  // PMC DISB (DRAM Initialization Scratchpad Bit)  BIT23 on DNV
  //
  Data32 = PCI_CFG_32B_READ (MrcData->EcBase, MC_BUS, 0x1F, 0x2, GEN_PMCON_A_PMC_PCI_CFG_REG);
  Data32 |= BIT23;
  PCI_CFG_32B_WRITE (MrcData->EcBase, MC_BUS, 0x1F, 0x2, GEN_PMCON_A_PMC_PCI_CFG_REG, Data32);
#endif // SIM
#ifndef JTAG
  if( (MrcData->EcBase == 0) || (MrcData->PmcBar == 0) || (MrcData->SmbusBar == 0)) {
    return MMRC_FAILURE;
  }
#endif
  return MMRC_SUCCESS;
}

/**
  This function configures the SMBus SPD Write Disable function
  DNV has two SMBus controllers and both of them have their own SPD Write Disable bit.
  SMBus Legacy (B0:D31:F4): DIMMS are connected to this controller.
  SMBus Host (B0:D18:F0): It is not intended to participate in the DIMM SPD.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
ConfigureSMBusSpdWriteDisable (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT32        Status;

  if (!MrcData->SpdWriteDisable) {
    Status = SetSMBusLegacySpdWriteDisable(MrcData);
    if (Status != MMRC_SUCCESS)
      return Status;
  }

  Status = SetSMBusHostSpdWriteDisable(MrcData);

  return Status;
}

/**
  This function sets the SPD Write Disable bit in the SMBus Legacy controller.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
SetSMBusLegacySpdWriteDisable (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
#if !defined SIM && !defined JTAG
  UINT32        Data32;

  if (MrcData->EcBase == 0)
    return MMRC_FAILURE;

  Data32 = PCI_CFG_32B_READ(MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x40);
  if (Data32 & BIT4) {
    MspDebugPrint ((MSP_DBG_MIN, "SMBus Legacy: SPD Write Disable bit has been locked!\n"));
    return MMRC_SUCCESS;
  }

  //
  // Configure SPD writes disable
  //
  Data32 |= BIT4;
  PCI_CFG_32B_WRITE(MrcData->EcBase, MC_BUS, 0x1F, 0x4, 0x40, Data32);
  MspDebugPrint ((MSP_DBG_MIN, "SMBus Legacy: SPD Write Disable bit is locked now!\n"));
#endif // SIM

  return MMRC_SUCCESS;
}

/**
  This function sets the SPD Write Disable bit in the SMBus Host controller.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
SetSMBusHostSpdWriteDisable (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
#if !defined SIM && !defined JTAG
  UINT16            Data16;
  UINT32            SmtBar;
  volatile UINT32   *Mctrl;
  BOOLEAN           ClearSmtBar = FALSE;

  if (MrcData->EcBase == 0)
      return MMRC_FAILURE;

  //
  // Set MBA Base address in SMTBAR
  //
  SmtBar = PCI_CFG_32B_READ(MrcData->EcBase, MC_BUS, 0x12, 0, 0x10);
  SmtBar &= 0xFFFFFC00;
  if (SmtBar == 0) {
    SmtBar = 0xA1576000;
    ClearSmtBar = TRUE;
    PCI_CFG_32B_WRITE(MrcData->EcBase, MC_BUS, 0x12, 0x0, 0x10, SmtBar);
  }

  //
  // Enable memory space transactions
  //
  Data16 = PCI_CFG_16B_READ(MrcData->EcBase, MC_BUS, 0x12, 0x0, 0x4);
  Data16 |= BIT1;
  PCI_CFG_16B_WRITE(MrcData->EcBase, MC_BUS, 0x12, 0x0, 0x4, Data16);

  //
  // Set SPDDIS bit
  //
  Mctrl = (volatile UINT32 *) (SmtBar + 0x108);
  *Mctrl |= BIT3;

  //
  // Disable memory space transactions
  //
  Data16 = PCI_CFG_16B_READ(MrcData->EcBase, MC_BUS, 0x12, 0x0, 0x4);
  Data16 &= ~BIT1;
  PCI_CFG_16B_WRITE(MrcData->EcBase, MC_BUS, 0x12, 0x0, 0x4, Data16);

  //
  // Clear MBA Base address in SMTBAR
  //
  if (ClearSmtBar) {
    SmtBar = 0;
    PCI_CFG_32B_WRITE(MrcData->EcBase, MC_BUS, 0x12, 0x0, 0x10, SmtBar);
  }
  MspDebugPrint ((MSP_DBG_MIN, "SMBus Host: SPD Write Disable bit is locked now!\n"));
#endif // SIM

  return MMRC_SUCCESS;
}

/**
  This function determines which kind of boot path needs to be followed in MRC flow

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
DetermineBootMode (
  IN  OUT   MMRC_DATA   *MrcData
  )
{

  UINT8       WarmResetOccurred;
  UINT8       DramInSelfRefresh;
  UINT8       MrcFlowStatus;
  MMRC_STATUS Status;
  BOOLEAN     BackupEnable;

  PRINT_FUNCTION_INFO;

  
#if defined SIM || defined JTAG
  MrcData->MrcParameters.BootMode = S5;
  return MMRC_SUCCESS;
#endif

  BackupEnable = MrcData->MspData.Enabled[0];
  WarmResetOccurred = 0;
  DramInSelfRefresh = 0;

  //
  // DetermineBootModeEntry call reads the SSKPD0, Punit Spd_Reset_Status and PMC MEM_SR
  // (Memory Placed in Self-Refresh) BIT21 on DNV. We will use this as warm reset indication
  //
  MrcData->MspData.Enabled[0] = TRUE;
  Status = MmrcExecuteTask (MrcData, DetermineBootModeEntry, NO_PRINT, 0);
  MrcData->MspData.Enabled[0] = BackupEnable;
  WarmResetOccurred = (UINT8) MrcData->MspData.DynamicVars[0][HOST_RST_STS];
  DramInSelfRefresh = (UINT8) MrcData->MspData.DynamicVars[0][MEM_SR];
  MrcFlowStatus     = (UINT8) MrcData->MspData.DynamicVars[0][MRC_FLOW_STATUS];
  MspDebugPrint ((MSP_DBG_MED, "MrcFlowStatus = 0x%04x\n", MrcData->MspData.DynamicVars[0][MRC_FLOW_STATUS]));
  MspDebugPrint ((MSP_DBG_MED, "WarmResetOccurred = %d\n", WarmResetOccurred));
  MspDebugPrint ((MSP_DBG_MED, "DramInSelfRefresh = %d\n", DramInSelfRefresh));
  MspDebugPrint ((MSP_DBG_MED, "Punit SpdResetStatus = 0x%04x\n", MrcData->MspData.DynamicVars[0][SPD_RESET_STATUS]));

  if (MrcData->MrcParameters.BootMode == S3) {
    //
    // S3 resume from BIOS
    //
    MspDebugPrint ((MSP_DBG_MED, "S3 Resume.\n"));
  } else if (MrcData->AdrRecoveryDetected) {
    //
    // ADR recovery in progress
    //
    MspDebugPrint ((MSP_DBG_MED, "ADR recovery.\n"));
    MrcData->MrcParameters.BootMode = ADR;
  } else if (DramInSelfRefresh == 1) {
    if (DramInSelfRefresh == 1 && MrcFlowStatus == MmrcFinished) {
      //
      // A warm reset happened after MRC completed and DRAM was in self refresh.
      // We can take the S0 to fast track through MRC since the DDRIO IO is already
      // initialized.
      //
      MspDebugPrint ((MSP_DBG_MED, "Warm reset detected. DDRIO is still initialized. Skipping training.\n"));
      MrcData->MrcParameters.BootMode = S0;
    } else if (MrcData->MspData.DynamicVars[0][SPD_RESET_STATUS] == 1 && MrcFlowStatus == MmrcNotStarted) {
      //
      // We haven't trained memory and we are coming from SPD Reset. Need to take cold path through MRC.
      //
      MspDebugPrint ((MSP_DBG_MED, "Coming back from SPD Reset....\n"));
      MrcData->MrcParameters.BootMode = S5;  
    } else if (MrcFlowStatus == MmrcNotStarted) {
      MspDebugPrint ((MSP_DBG_MED, "MRC_NOT_STARTED. Cold boot detected.\n"));
      MrcData->MrcParameters.BootMode = S5;
    } else {
      //
      // We're here if a warm reset happened, and this occurred either during MRC or
      // we detected that the memory was not in self refresh for whatever reason.
      // Need to force a powergood reset and take the cold path through MRC.
      //
      MspDebugPrint ((
        MSP_DBG_MED,
        "Warm reset detected before memory was properly initialized or memory was not\nin self refresh. Forcing powergood reset.\n"
      ));
      IoOut8 (0xCF9, 0x0);
      IoOut8 (0xCF9, 0xE);
#ifndef SIM
      _asm hlt
#endif
    }
  } else {
    if (MrcData->MrcParameters.BootMode != FB) {
      MspDebugPrint ((MSP_DBG_MED, "Cold boot detected.\n"));
      MrcData->MrcParameters.BootMode = S5;
    }
  }
  return MMRC_SUCCESS;
}

/**
  This function determines if coldboot is required because DIMMs have been changed
  in between boots.

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          TRUE            Cold Boot Required
  @retval          FALSE           Cold Boot Not Required
**/
BOOLEAN
CheckColdBootRequired (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 FastBootParameter;
  UINT8 *CurrentFastBootData;
  UINT8 *PreviousFastBootData;

  CurrentFastBootData   = (UINT8 *) &MrcData->FastBootDataCurrent;
  PreviousFastBootData  = (UINT8 *) &MrcData->MrcParameters.SaveRestore.FastBootData;

  for (FastBootParameter = 0; FastBootParameter < sizeof (FASTBOOTDATA); FastBootParameter++) {
    if (*(CurrentFastBootData + FastBootParameter) != *(PreviousFastBootData + FastBootParameter)) {
      MspDebugPrint ((MSP_DBG_MIN, "Unmatch parameter: %d\n", FastBootParameter));
      return TRUE;
    }
  } // FastBootParameter loop ...

  return FALSE;
}

/**
  The function takes pointer to previous 32 bit value and modifies it to next value.
  32 bit LFSR with characteristic polinomial:  X^32 + X^22 +X^2 + X^1

  @param[in, out]  LfsrWordPtr     Pointer to previous value
**/
VOID 
Lfsr16 (
  IN  OUT     UINT16  *LfsrWordPtr
  )
{
  UINT16 bit;
  UINT16 lfsr;
  UINT8 i;

  lfsr = *LfsrWordPtr;

  for (i = 0; i < 16; i++) {
    bit  =   1 ^ (lfsr & BIT0)        ;
    bit  = bit ^ ((lfsr & BIT1)  >>  1);
    bit  = bit ^ ((lfsr & BIT2)  >>  2);
    bit  = bit ^ ((lfsr & BIT6)  >>  6);
    lfsr = ((lfsr >> 1) | (bit << 15) );
  }
  *LfsrWordPtr = lfsr;

  return;
}
/**
  This function configures the slow or fast exit power down for any type of memory (DDR3/4)

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel under examination
**/
MMRC_STATUS
SetSlowFastExit (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  MrcData->MspData.DynamicVars[Channel][SLOWEXIT]   = 0;
  MrcData->MspData.DynamicVars[Channel][TXPDLL_PD]  = 0;

  if (MrcData->EnableSlowPowerDownExit == 1 && MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    //
    // Slow exit from pre-charge can only be used with single DIMM configurations.  
    // This is because, in a two DIMM configuration, if the DIMM is in power down, 
    // it can't provide the needed termination to the other DIMM on reads.
    // Also applies only for DDR3 only
    //
    if (MrcData->DimmCount[Channel] == 1) {
      MrcData->MspData.DynamicVars[Channel][SLOWEXIT] = 1;
    } 
  }

  return MMRC_SUCCESS;
}

/**
  This function configures the scrambler

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel under examination
**/
MMRC_STATUS
SetScrambler (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  UINT16                           lfsr;
  UINT8                            nvdimms_present = 0;

  PRINT_FUNCTION_INFO_MAX;  
  lfsr = 0;
  nvdimms_present = ((MrcData->NvDimmType[Channel][0] != 0) || (MrcData->NvDimmType[Channel][1] != 0));

  if (MrcData->MspData.DynamicVars[Channel][SCRAMBLER_SUPPORTED] != 0) {
    //
    // 16 bit seed is always stored in BIOS NVM.
    //
    lfsr = MrcData->MrcParameters.SaveRestore.ScramblerSeed[Channel];
    //
    // when the scrambler seed wasn't successfully saved into NVRAM 
    // or On cold boot whith no NVDIMMs...  
    //
    if ((MrcData->MrcParameters.SaveRestore.MrcParamsValidFlag == 0) || ((MrcData->MrcParameters.BootMode == S5) && (nvdimms_present==0) )) {
      //
      // Factory value is 0 and in first boot, a clock based seed is loaded.
      //
      if (lfsr == 0) {
        lfsr = (UINT16) (get_initial_seed() & 0x0000FFFF); // get seed and make sure it is not all 1's
      }
      //
      // Need to replace scrambler.
      // Get next 16bit LFSR which is the previous scrambler vector.
      //
      else {
        Lfsr16 (&lfsr);
      }
      MrcData->MrcParameters.SaveRestore.ScramblerSeed[Channel] = lfsr;               // save new seed.
    } // if (cold_boot)

    //
    // In warm boot or S3 exit, we have the previous seed.
    // In cold boot, we have the last 16bit LFSR which is the new seed.
    //
    Lfsr16 (&lfsr);

    
    MrcData->MspData.DynamicVars[Channel][SCRAMBLER_EN]    = MrcData->EnableScrambler;
    if (MrcData->EnableScrambler == 1) {
      MrcData->MspData.DynamicVars[Channel][SCRAMBLER_KEY]   = lfsr;
    } else {
      MrcData->MspData.DynamicVars[Channel][SCRAMBLER_KEY]   = 0;
    }
    MspDebugPrint ((MSP_DBG_MAX, "KEY %0x: ENABLE: %d\n", (UINT32) MrcData->MspData.DynamicVars[Channel][SCRAMBLER_KEY], (UINT32) MrcData->MspData.DynamicVars[Channel][SCRAMBLER_EN] ));
    
  } else {
    MspDebugPrint ((MSP_DBG_MAX, "SetScrambler: fuse is set.  Skipping scrambler init\n"));
  }
  return MMRC_SUCCESS;
}

UINT32
get_initial_seed ()
{
  UINT32  Data32;
#ifndef SIM
  _asm {

    push    eax
    push    ebx
    push    ecx
    push    edx

    //
    // Per d-unit HAS, attempt to use the DRNG for our seed.
    // Per DRNG HAS, DRNG is accessed through the RDRAND CPU instruction.
    // If CPUID ECX bit 30 is not set, cpu doesn't support RDRAND
    //
    mov     eax, 1
    cpuid
    bt      ecx, 30
    jnc     rdrand_not_supported

    RDRAND_EAX
    jc      rdrand_done
    //
    // If carry is clear, then random number "not available" per instruction definition
    //

    rdrand_not_supported:
    //
    // If either RDRAND is not supported by the CPU or is unavailable, fall back on use of cpu tsc
    //
    rdtsc

    rdrand_done:
    mov     Data32, eax

    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
  }
#else
  Data32 = 1;
#endif
  return Data32;
}

/**
  Calculates the largest integer that is both
  a power of two and less than Inpu

  @param[in]       Operand         Value to calculate power of two
  @retval                          The largest integer that is both a power of
                                   two and less than Input
**/
UINT32
GetPowerOfTwo (
  IN        UINT32        Operand
  )
{
  UINT8 Bitpos;
  Bitpos = 0;

  if (Operand == 0) {
    return 0;
  }

  while (Operand != 0) {
    Operand >>= 1;
    Bitpos++;
  }

  Operand = 1;
  Bitpos--;
  while (Bitpos != 0) {
    Operand <<= 1;
    Bitpos--;
  }

  return Operand;
}

/**
  Finds an active channel that have read the fuses and current DDR freq

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          0xFF            No Active Channel Found
  @retval          Channel         Channel Active Found
**/
UINT8
FindActiveChannel (
  IN  OUT   MMRC_DATA   *MrcData
)
{
  UINT8 Channel;
  //
  // Find an active channel that have read the fuses and current DDR freq
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->MrcParameters.ChEnabled[Channel]) {
      return Channel;
    }
  }
  return 0xFF;
}
