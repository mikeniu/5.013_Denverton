
/** @file
  Do Platform Stage System Agent initialization.

@copyright
  Copyright (c) 2013 - 2017 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include <PeiSaPolicyUpdate.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define EFI_MEMORY_CONFIG_VARIABLE_GUID \
  {0xc94f8c4d,0x9b9a,0x45fe,0x8a,0x55,0x23,0x8b,0x67,0x30,0x26,0x43 }

#define EFI_SA_PEG_CONFIG_VARIABLE_GUID \
  {0xb414caf8,0x8225,0x4d6f,0xb9,0x18,0xcd,0xe5,0xcb,0x84,0xcf,0x0b }

EFI_GUID  gEfiMemoryConfigVariableGuid  = EFI_MEMORY_CONFIG_VARIABLE_GUID;
//EFI_GUID  gEfiPegConfigVariableGuid     = EFI_SA_PEG_CONFIG_VARIABLE_GUID;
static EFI_GUID  mSystemConfigurationGuid      = SYSTEM_CONFIGURATION_GUID;

//
// Example of a Micron Dimm - UDIMM @1600
//
CONST UINT8 MicronMemoryDownDevice[MD_MAX_SPD_BYTES_512] = {
  0,0,                 // Byte 0 -> 1
  0x0B,                // Byte 2
  0x02,                // Byte 3
  0x04,                // Byte 4
  0x21,                // Byte 5
  0x02,                // Byte 6
  0x09,                // Byte 7
  0x03,                // Byte 8
  0,                   // Byte 9 -> 9
  0x01,                // Byte 10
  0x08,                // Byte 11
  0x0A,                // Byte 12
  0,                   // Byte 13 -> 13
  0xFE,                // Byte 14
  0x00,                // Byte 15
  0x69,                // Byte 16
  0x78,                // Byte 17
  0x69,                // Byte 18
  0x30,                // Byte 19
  0x69,                // Byte 20
  0x11,                // Byte 21
  0x18,                // Byte 22
  0x81,                // Byte 23
  0x20,                // Byte 24
  0x08,                // Byte 25
  0x3C,                // Byte 26
  0x3C,                // Byte 27
  0x00,                // Byte 28
  0xF0,                // Byte 29
  0,0,0,0,0,0,0,0,0,0, // Byte 30 -> 39
  0,0,0,0,0,0,0,0,0,0, // Byte 40 -> 49
  0,0,0,0,0,0,0,0,0,0, // Byte 50 -> 59
  0,0,                 // Byte 60 -> 61
  0x21,                // Byte 62
  0x01,                // Byte 63
  0,0,0,0,0,0,0,0,0,0, // Byte 64 -> 73
  0,0,0,0,0,0,0,0,0,0, // Byte 74 -> 83
  0,0,0,0,0,0,0,0,0,0, // Byte 84 -> 93
  0,0,0,0,0,0,0,0,0,0, // Byte 94 -> 103
  0,0,0,0,0,0,0,0,0,0, // Byte 104 -> 113
  0,0,0,               // Byte 114 -> 116
  0x80,                // Byte 117
  0x2C,                // Byte 118
  0x01,                // Byte 119
  0x12,                // Byte 120
  0x49,                // Byte 121
  0x1A,                // Byte 122
  0x99,                // Byte 123
  0x21,                // Byte 124
  0x6D,                // Byte 125
  0,0,0,0,0,0,0,0,0,0, // Byte 126 -> 135
  0,0,0,0,0,0,0,0,0,0, // Byte 136 -> 145
  0,0,                 // Byte 146 -> 147
  0x80,                // Byte 148
  0xAD,                // Byte 149
};


/**
Routine Description:

  This function performs SA PEI Platform Policy initialzation.

Arguments:

  PeiServices   General purpose services available to every PEIM.
  SaPlatformPolicyPpi    The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
UpdatePeiSaPolicy (
  IN OUT   SI_SA_POLICY_PPI  *SiSaPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINTN                           VarSize;
  SA_PLATFORM_CONFIG              *SaPlatformConfig;
  MEMORY_CONFIGURATION            *MemConfig;
  UINT8                           Rank;
  VTD_CONFIG                      *VtdConfig;
  PLATFORM_INFO_HOB               *PlatformInfo;
  UINT8                           Dimm;
  UINT8                           Channel;
  UINT16                          SpdOffset;
  UINT8                           Index;
  DEBUG((DEBUG_INFO, "%a()\n", __FUNCTION__));

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                            // INSTANCE
             NULL,                         // EFI_PEI_PPI_DESCRIPTOR
             &VariableServices             // PPI
             );
  ASSERT_EFI_ERROR ( Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumMemConfigId, (VOID *)&MemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumPlatformConfigId, (VOID *)&SaPlatformConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumVtdConfigId, (VOID *)&VtdConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Get System configuration variables
  //
  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &mSystemConfigurationGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );

      //
      // Initialize the Memory Configuration
      //
    if (!EFI_ERROR (Status)) {
      MemConfig->ValidSetup               = 1;
      MemConfig->CommandMode              = SystemConfiguration.CommandMode;
      MemConfig->Fastboot                 = SystemConfiguration.FastBoot;
      MemConfig->MrcDebugMsg              = SystemConfiguration.MrcDebugMsg;
      MemConfig->EccSupport               = SystemConfiguration.ECCSupport;
      MemConfig->PatrolSupport            = SystemConfiguration.PatrolSupport;
      MemConfig->PatrolPeriod             = SystemConfiguration.PatrolPeriod;
      MemConfig->DemandScrub              = SystemConfiguration.DemandScrub;
      MemConfig->MaxScrubDebitCount       = SystemConfiguration.MaxScrubDebitCount;
      MemConfig->EnableScrambler          = SystemConfiguration.EnableScrambler;
      MemConfig->EnableSlowPowerDownExit  = SystemConfiguration.EnableSlowPowerDownExit;
      MemConfig->MemoryPreservation       = (BOOLEAN) SystemConfiguration.MemoryPreservation;
      MemConfig->DynamicSelfRefresh       = SystemConfiguration.DynamicSelfRefresh;
      MemConfig->DynamicPowerModeOpcode   = SystemConfiguration.DynamicPowerModeOpcode;
      MemConfig->SelectRefreshRate        = SystemConfiguration.SelectRefreshRate;
      //MemConfig->ForceAxMapping           = SystemConfiguration.ForceAxMapping;
      MemConfig->DdrFreq                  = SystemConfiguration.DdrFreq;
      MemConfig->HalfWidthEnable          = SystemConfiguration.HalfWidthEnable;
      MemConfig->TclIdle                  = SystemConfiguration.TclIdle;
      MemConfig->EnableParallelTraining   = (BOOLEAN) SystemConfiguration.EnableParallelTraining;
      MemConfig->DdrChannels              = SystemConfiguration.DdrChannels;
      MemConfig->CommandAddressParity     = SystemConfiguration.CommandAddressParity;
      MemConfig->MrcRmtSupport            = SystemConfiguration.MrcRmtSupport;
      MemConfig->OpenPolicyTimer          = SystemConfiguration.OpenPolicyTimer;
      MemConfig->MemoryThermalEnable      = SystemConfiguration.MemoryThermalEnable;
      MemConfig->ThermalThrottlingType    = SystemConfiguration.ThermalThrottlingType;
      MemConfig->ClttMode                 = SystemConfiguration.ClttMode;
      MemConfig->TempHi                   = SystemConfiguration.TempHi;
      MemConfig->TempMid                  = SystemConfiguration.TempMid;
      MemConfig->TempLo                   = SystemConfiguration.TempLo;
      MemConfig->BwLevelOnThrtCrit        = SystemConfiguration.BwLevelOnThrtCrit;
      MemConfig->BwLevelOnThrtHi          = SystemConfiguration.BwLevelOnThrtHi;
      MemConfig->BwLevelOnThrtMid         = SystemConfiguration.BwLevelOnThrtMid;
      MemConfig->OlttThrtBw               = SystemConfiguration.OlttThrtBw;
      MemConfig->MemHotThrotLvl           = SystemConfiguration.MemHotThrotLvl;
      MemConfig->EnMemTrip                = SystemConfiguration.EnMemTrip;
      MemConfig->TempMemTrip              = SystemConfiguration.TempMemTrip;
      MemConfig->RxSkewCtl                = SystemConfiguration.RxSkewCtl;
      MemConfig->TxSkewCtl                = SystemConfiguration.TxSkewCtl;
      MemConfig->PerformanceProfile       = SystemConfiguration.PerformanceProfile;
      MemConfig->SmmSize                  = SystemConfiguration.SmmSize;
      MemConfig->OutOfOrderAgingThreshold = SystemConfiguration.OutOfOrderAgingThreshold;
      MemConfig->EnableOutOfOrderProcessing = SystemConfiguration.EnableOutOfOrderProcessing;
      MemConfig->TwoClkRdPreamble         = SystemConfiguration.TwoClkRdPreamble; 
      MemConfig->TwoClkWrPreamble         = SystemConfiguration.TwoClkWrPreamble;
      MemConfig->WriteDataEarlyEnable     = SystemConfiguration.WriteDataEarlyEnable; 
      MemConfig->EnableNewRequestBypass   = SystemConfiguration.EnableNewRequestBypass;
      MemConfig->AbSegToDram              = SystemConfiguration.AbSegToDram;
      MemConfig->ESegToDram               = SystemConfiguration.ESegToDram;
      MemConfig->FSegToDram               = SystemConfiguration.FSegToDram;
      MemConfig->VrefOverrideEnable       = SystemConfiguration.VrefOverrideEnable;
      MemConfig->VrefOverrideValue        = SystemConfiguration.VrefOverrideValue;
      MemConfig->ZqcEnable                = SystemConfiguration.ZqcEnable;
      MemConfig->TclTiming                = SystemConfiguration.TCLTiming;
      MemConfig->TRcdTiming               = SystemConfiguration.TRCDTiming;
      MemConfig->TRpTiming                = SystemConfiguration.TRPTiming;
      MemConfig->TRasTiming               = SystemConfiguration.TRASTiming;
      MemConfig->TRtpTiming               = SystemConfiguration.TRTPTiming;
      MemConfig->TRrdTiming               = SystemConfiguration.TRRDTiming;
      MemConfig->TFawTiming               = SystemConfiguration.TFAWTiming;
      MemConfig->TCcdTiming               = SystemConfiguration.TCCDTiming;
      MemConfig->TWtpTiming               = SystemConfiguration.TWTPTiming;
      MemConfig->TWclTiming               = SystemConfiguration.TWCLTiming;
      MemConfig->MrcRmtCpgcExpLoopCntValue= SystemConfiguration.MrcRmtCpgcExpLoopCntValue;
      MemConfig->MrcRmtCpgcNumBursts      = SystemConfiguration.MrcRmtCpgcNumBursts;
      MemConfig->FaultyPartContinueOnCorrectable = SystemConfiguration.FaultyPartOnCorrectable;
      MemConfig->FaultyPartTracking       = SystemConfiguration.FaultyPartEnable;
      MemConfig->MmioHighSize             = SystemConfiguration.MmioHighSize;
      MemConfig->MitigationProbability    = SystemConfiguration.MitigationProbability;
      MemConfig->ReadPerBitEnable         = SystemConfiguration.ReadPerBitEnable;
      MemConfig->WritePerBitEnable        = SystemConfiguration.WritePerBitEnable;
      MemConfig->OverrideCheckpoints      = SystemConfiguration.OverrideCheckpoints;
#if ME_SUPPORT_FLAG
      MemConfig->MeSeg                    = SystemConfiguration.MeSeg;
#endif
      MemConfig->BankXorMapEnable         = SystemConfiguration.BankXorMapEnable;
      MemConfig->Rk2RkEn                  = SystemConfiguration.Rk2RkEn;
      MemConfig->MrcMemoryTestLoop        = SystemConfiguration.MrcMemoryTestLoop;
      MemConfig->BramParityEnable         = SystemConfiguration.BramParityEnable;
      MemConfig->InterleaveMode           = SystemConfiguration.InterleaveMode;
      MemConfig->MrcHaltSystemOnMemTestError    = SystemConfiguration.MrcHaltSystemOnMemTestError;
      MemConfig->MrcTrainingCpgcExpLoopCntValue = SystemConfiguration.MrcTrainingCpgcExpLoopCntValue;
      MemConfig->MrcResetLoop             = SystemConfiguration.MrcResetLoop;
      MemConfig->MrcResetLoopCycles       = SystemConfiguration.MrcResetLoopCycles;
      MemConfig->MmioSize                 = SystemConfiguration.MmioSize;
      MemConfig->MrcTrainingCpgcNumBursts = SystemConfiguration.MrcTrainingCpgcNumBursts;
      MemConfig->LoopCount                = SystemConfiguration.LoopCount;
      MemConfig->InputEnableBunitPerformance    = SystemConfiguration.InputEnableBunitPerformance;
      MemConfig->BestEffortMaxLat         = SystemConfiguration.BestEffortMaxLat;
      MemConfig->PageHitDelay             = SystemConfiguration.PageHitDelay;
      MemConfig->IsocBankPrefetch         = SystemConfiguration.IsocBankPrefetch;
      MemConfig->BestEffortBankPref       = SystemConfiguration.BestEffortBankPref;
      MemConfig->BDebugSelfRefresh        = SystemConfiguration.BDebugSelfRefresh;
      MemConfig->SelfRefreshDelay         = SystemConfiguration.SelfRefreshDelay;
      MemConfig->CorrectErrThreshold      = SystemConfiguration.CorrectErrThreshold;
      MemConfig->CkePowerDownDisable      = SystemConfiguration.CkePowerDownDisable;
      MemConfig->ClttThermReg0            = SystemConfiguration.ClttThermReg[0];
      MemConfig->ClttThermReg1            = SystemConfiguration.ClttThermReg[1];
      MemConfig->ClttEventTrip            = SystemConfiguration.ClttEventTrip;
      MemConfig->InputPassGateEnabled     = SystemConfiguration.InputPassGateEnabled;
      MemConfig->InputPassGateEnableSelfRefresh = SystemConfiguration.InputPassGateEnableSelfRefresh;
      MemConfig->InputPassGateDirection   = SystemConfiguration.InputPassGateDirection;
      MemConfig->InputPassGateRepetitionCountRange = SystemConfiguration.InputPassGateRepetitionCountRange;
      MemConfig->InputPassGateIterationOnRow = SystemConfiguration.InputPassGateIterationOnRow;
      MemConfig->InputPassGateSwizzle     = SystemConfiguration.InputPassGateSwizzle;
      MemConfig->InputPassGatePattern     = SystemConfiguration.InputPassGatePattern;
      MemConfig->InputPassGateTargetPattern  = SystemConfiguration.InputPassGateTargetPattern;
      MemConfig->InputSpeedSelected       = SystemConfiguration.InputSpeedSelected;
      MemConfig->InputPassGatePartial     = SystemConfiguration.InputPassGatePartial;
      MemConfig->InputPassGateRowBankMin      = SystemConfiguration.InputPassGateRowBankMin;
      MemConfig->InputPassGateRowBankMax      = SystemConfiguration.InputPassGateRowBankMax;
      MemConfig->InputPassGateMonteCarloEnable = SystemConfiguration.InputPassGateMonteCarloEnable;
      MemConfig->InputMontecarloMaxFailures      = SystemConfiguration.InputMontecarloMaxFailures;
      MemConfig->InputMontecarloStartRepetition    = SystemConfiguration.InputMontecarloStartRepetition;
      MemConfig->InputMontecarloDecrement    = SystemConfiguration.InputMontecarloDecrement;

      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        MemConfig->InputPassGateRankEnable[0][Rank] = SystemConfiguration.InputPassGateRankEnableCh0[Rank];
        MemConfig->InputPassGateRankEnable[1][Rank] = SystemConfiguration.InputPassGateRankEnableCh1[Rank];
      }
      MemConfig->PmopPc0                  = SystemConfiguration.PmopPc0;
      MemConfig->PmopPcx                  = SystemConfiguration.PmopPcx;
      MemConfig->InputFineDdrVoltage      = SystemConfiguration.InputFineDdrVoltage;
      MemConfig->BmcModeEnable            = SystemConfiguration.BmcModeEnable;
      MemConfig->DramRaplTimeWindow               = SystemConfiguration.DramRaplTimeWindow;
      MemConfig->DramRaplPowerLimit               = SystemConfiguration.DramRaplPowerLimit;
      MemConfig->DramRaplPowerLimitEnable         = SystemConfiguration.DramRaplPowerLimitEnable;
      MemConfig->DramRaplLimitLock                = SystemConfiguration.DramRaplLimitLock;
      MemConfig->LeakyBucketUnits                 = SystemConfiguration.LeakyBucketUnits;
      MemConfig->EnableParityCheck        = (BOOLEAN) SystemConfiguration.EnableParityCheck;
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        MemConfig->LeakyBucketRate[0][Rank]           = SystemConfiguration.LeakyBucketRateCh0[Rank];
        MemConfig->LeakyBucketRate[1][Rank]           = SystemConfiguration.LeakyBucketRateCh1[Rank];
        MemConfig->CorrectableErrorThreshold[0][Rank] = SystemConfiguration.CorrectableErrorThresholdCh0[Rank];
        MemConfig->CorrectableErrorThreshold[1][Rank] = SystemConfiguration.CorrectableErrorThresholdCh1[Rank];
      }
      for (Index =0; Index < 8; Index++) {
        MemConfig->PprEnable[Index]     = SystemConfiguration.PprEnable[Index];
        MemConfig->Row[Index]           = SystemConfiguration.Row[Index];
        MemConfig->Bank[Index]          = SystemConfiguration.Bank[Index];
        MemConfig->BankGroup[Index]     = SystemConfiguration.BankGroup[Index];
        MemConfig->DramDevice[Index]    = SystemConfiguration.DramDevice[Index];
        MemConfig->TargetRank[Index]    = SystemConfiguration.TargetRank[Index];
        MemConfig->TargetChannel[Index] = SystemConfiguration.TargetChannel[Index];
      }
      //
      // Get Board Id
      //
      Status = GetPlatformInfo (&PlatformInfo);
      ASSERT_EFI_ERROR (Status);

      //
      // Override Settings
      //
      MemConfig->OverrideClocks.CmdOffsetValue = 0;
      MemConfig->OverrideClocks.ClkOffsetValue = 0;
      MemConfig->OverrideClocks.CsOffsetValue  = 0;
      MemConfig->OverrideClocks.CkeOffsetValue = 0;
      MemConfig->OverrideClocks.OdtOffsetValue = 0;
      //
      // Dimm Spd Address
      //
      if (PlatformInfo != NULL) {
        // MRC will ignore Spd address passed in if setting to zero.
        MemConfig->SpdAddressTable[0][0] = 0;
        MemConfig->SpdAddressTable[0][1] = 0;
        MemConfig->SpdAddressTable[1][0] = 0;
        MemConfig->SpdAddressTable[1][1] = 0;
      } else {
        ASSERT_EFI_ERROR (Status);
      }
      
      //
      // Memory Down Input parameters
      //
      for (Channel = 0; Channel < MD_MAX_CHANNELS; Channel++) {
        for (Dimm = 0; Dimm < MD_MAX_DIMMS; Dimm++) {
          MemConfig->MemDownConfiguration.EnableMemoryDown[Channel][Dimm] = MEMORY_DOWN_DISABLE;
          MemConfig->MemDownConfiguration.MaxSpdSize                      = MD_MAX_SPD_BYTES_512;
          if (MemConfig->MemDownConfiguration.EnableMemoryDown[Channel][Dimm] == MEMORY_DOWN_ENABLE) {
            for (SpdOffset = 0; SpdOffset < MD_MAX_SPD_BYTES_512; SpdOffset++) {
              MemConfig->MemDownConfiguration.MemoryDownSpdData[Channel][Dimm][SpdOffset] = MicronMemoryDownDevice[SpdOffset];
            }
          }
        }
      }
      //
      // Memory Scrub Segments Definitions
      //
      MemConfig->MemoryScrubSegments.NumberOfSegments           = MAX_SCRUB_SEGMENTS;
      MemConfig->MemoryScrubSegments.MemorySegmentationEnabled  = FALSE;
      //
      // SaPlatformConfig
      //
      SaPlatformConfig->MsiRedirAlgorithm    = SystemConfiguration.MsiRedirAlgorithm;
      SaPlatformConfig->EnableSsaClockGating = SystemConfiguration.EnableSsaClockGating;
      SaPlatformConfig->SchedulerLatency     = SystemConfiguration.SchedulerLat;
      CopyMem(SaPlatformConfig->CosCatAgent, SystemConfiguration.CosCatAgent, sizeof(SaPlatformConfig->CosCatAgent));
      SaPlatformConfig->BestEffortMaxLat    = SystemConfiguration.BestEffortMaxLat;
      //
      // VtdConfig
      //
      VtdConfig->VtdEnable                 = SystemConfiguration.VtdEnable;
      VtdConfig->InterruptRemappingSupport = (BOOLEAN)(SystemConfiguration.InterruptRemappingSupport);
      DEBUG((DEBUG_INFO, "vtd %x and remapping %x \n", VtdConfig->VtdEnable, VtdConfig->InterruptRemappingSupport ));
      //
      // Fast Boot Checker
      //
      MemConfig->FastBootChecker.EnableFastBootTester     = 1;
      MemConfig->FastBootChecker.Critical.MarginRxDelay   = 10;
      MemConfig->FastBootChecker.Critical.MarginTxDelay   = 10;
      MemConfig->FastBootChecker.Critical.MarginRxVoltage = 10;
      MemConfig->FastBootChecker.Critical.MarginTxVoltage = 10;

      MemConfig->FastBootChecker.Warning.MarginRxDelay    = 20;
      MemConfig->FastBootChecker.Warning.MarginTxDelay    = 30;
      MemConfig->FastBootChecker.Warning.MarginRxVoltage  = 30;
      MemConfig->FastBootChecker.Warning.MarginTxVoltage  = 50;

      //
      // Set Policy for SMBus SPD Write Disable (FORCE_ENABLE/0 = Bit set, FORCE_DISABLE/1 = Bit clear)
      //
      MemConfig->SpdWriteDisable = (BOOLEAN)0;
    } else {
      MemConfig->ValidSetup = 0;
    }

  return EFI_SUCCESS;
}
