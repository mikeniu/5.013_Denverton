/** @file
  MmrcLibraries.c 
  Modular MMRC  libraries used throughout the MMRC, these files 
  are independent of the project. 
 
  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "MmrcLibraries.h"
#include "MmrcHooks.h"
#include "MSysProg.h"
#include "McFunc.h"

#if MRC_HUMAN_READABLE_ERRORS
UINT8 mErrorCode[MrcErrMsgMaxError][50] = {
  "All Right",                                           // MrcErrOk 
  "Debug",                                               // Err[ 1] - MrcErrDebug
  "DetectDimms Failure",                                 // Err[ 2] - MrcErrDimmsFailed
  "Install Pei Memory Error",                            // Err[ 3] - MrcErrInstallPeiMemory
  "Invalid Number of Channels",                          // Err[ 4] - MrcErrInvalidChannelNumber
  "Invalid Memory Size Found",                           // Err[ 5] - MrcErrInvalidMemorySize
  "Illegal Voltage Configuration",                       // Err[ 6] - MrcErrIllegalVoltageConfig
  "Mailbox Error",                                       // Err[ 7] - MrcErrMailbox
  "MemTest Error",                                       // Err[ 8] - MrcErrMemTest
  "Warm Reset Error",                                    // Err[ 9] - MrcErrWarmResetError
  "Error trying to get SPD data",                        // Err[10] - MrcErrGetSpdData
  "No Dimms are Present",                                // Err[11] - MrcErrNoDimmsPresent
  "Invalid Number of Ranks",                             // Err[12] - MrcErrInvalidRankNumber
  "Invalid Number of Dimms",                             // Err[13] - MrcErrInvalidDimmNumber
  "Error Configuring Memory",                            // Err[14] - MrcErrConfigureMemory
  "Null Pointer Error",                                  // Err[15] - MrcErrNullPointer
  "GetSet Value exceeds limit",                          // Err[16] - MrcSetLimitReached
  "Faulty Parts Tracking: Halt on Correctable Error",    // Err[17] - MrcErrFaultyPartCorrectable
  "Faulty Parts Tracking: Uncorrectable Error on CH0",   // Err[18] - MrcErrFaultyPartUncorrectable
  "Invalid Frequency",                                   // Err[19] - MrcErrInvalidFrequency
  "Platform Settings Error",                             // Err[20] - MrcErrPlatformSettings
  "Invalid Linear Value",                                // Err[21] - MrcErrInvalidLinearValue
  "Invalid Physical Value",                              // Err[22] - MrcErrInvalidPhysicalValue
  "More than 1 2x clock between Strobe and Rank",        // Err[23] - MrcErrRankStrobeClockDelta
  "Center 2D Accumulate Margin X is zero",               // Err[24] - MrcErrCenter2DAccMarginXZero
  "Center 2D Accumulate Margin Y is zero",               // Err[25] - MrcErrCenter2DAccMarginYZero
  "Early Command Error, Search beyond 1/2 CLK",          // Err[26] - MrcErrSearchBeyondHalfClk
  "Invalid value for current Power Knob",                // Err[27] - MrcErrInvalidPowerKnob
  "All Channels are disabled",                           // Err[28] - MrcErrChannelsDisabled
  "Setup error during Pass Gate Test",                   // Err[29] - MrcErrSetupErrorPassGate
  "Rank is not present",                                 // Err[30] - MrcRankNotSupported
  "IO level unsupported",                                // Err[31] - MrcIoLevelNotSupported
  "Margin group unsupported",                            // Err[32] - MrcMarginGroupNotSupported
  "Signal unsupported",                                  // Err[33] - MrcSignalNotSupported
  "Frequency Index not supported",                       // Err[34] - MrcFrequencyIndexNotSupported
  "Invalid Ddr Type",                                    // Err[35] - MrcErrInvalidDdrType
  "SPD Buffer overrun",                                  // Err[36] - MrcErrSpdBufferOverrun
  "Invalid PPR Input parameter",                         // Err[37] - MrcErrPprInputParameter
  "x4 devices are not supported",                        // Err[38] - MrcErrX4NotSupported
  "Per bit not supported",                               // Err[39] - MrcErrPerBitNotSupported
  "Illegal Adv Constrain",                               // Err[40] - MrcErrIllegalAdvConstrains
  "Invalid number of Max Bits",                          // Err[41] - MrcErrInvalidMaxBits
  "Invalid Command for cache access",                    // Err[42] - MrcErrInvalidCmdForCache
  "Invalid Cache Index Mask",                            // Err[43] - MrcErrInvalidCacheIndexMash
  "Parameter Out of Bound"                               // Err[44] - MrcErrParameterOutOfBound
};
#endif

UINT8 CacheTypePrefix [MaxCacheTypes] = {
  0x00,
  0x40,
  0x80,
  0xC0
};

UINT8 ManufacturerId [MAX_MANUFACTURER_ID] = {
  MICRON_ID,
  SAMSUNG_ID,
  TRANSCEND_ID,
  KINGSTON_ID,
  ELPIDA_ID,
  HYNIX_ID,
  INFINEON_ID,
  SMART_ID,
  QIMONDA_ID,
};

CONST UINT8 *ManufacturerStringId[MAX_MANUFACTURER_ID] = {
  "Micron",
  "Samsung",
  "Transcend",
  "Kingston",
  "Elpida",
  "Hynix",
  "Infineon",
  "Smart",
  "Quimoda",
};

#if SIM || JTAG
MMRC_DATA *MyMrcData;
#endif

UINT8  ResultsString[3][3]= {"P\0", "-\0","*\0"};

UINT32
ReadSkpd7 ()
{
  REGISTER_ACCESS     Register;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = SKPD7_DUNIT_COMMON_REG;

  return (UINT32)MemRegRead (DUNIT_COMMON, 0, 0, Register);
}

VOID
EnableMrcBreakpoint ()
{
  REGISTER_ACCESS     Register;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = SKPD3_DUNIT_COMMON_REG;

  MemRegWrite (DUNIT_COMMON, 0, 0, Register, 1,0xF);
}

VOID
DisableMrcBreakpoint ()
{
  REGISTER_ACCESS     Register;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = SKPD3_DUNIT_COMMON_REG;

  MemRegWrite (DUNIT_COMMON, 0, 0, Register, 0,0xF);
}

VOID
ReadDelayElements (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN        UINT32       *Value,
  IN        BOOLEAN       HeaderEn
)
{
  UINT8   DelayIndex;
  UINT32  Value32;
  UINT32  LinearDelay;

  Type  = GetOffsetIndex (MrcData, Type);
  Type -= ALGO_REG_INDEX;
  Type *= NumberOfElementsPerAlgo;

  if (HeaderEn) {
    MspDebugPrint ((MSP_DBG_MIN, "CRS_    1x 2x PI C0 C1 C2 Delay\n"));
  }
  LinearDelay = 0;
  MspDebugPrint ((MSP_DBG_MIN, "%d%d%02d    ", Channel, Rank, Strobe));
  for (DelayIndex = 0; DelayIndex < (NumberOfDelayElementsPerAlgo + NumberOfCCElements); DelayIndex++) {
    if (GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Type + DelayIndex, Cmd, &Value32) == MMRC_SUCCESS) {
      MspDebugPrint ((MSP_DBG_MIN, "%02d ", Value32));
      if (DelayIndex < NumberOfDelayElementsPerAlgo) {
        LinearDelay += Value32 * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][DelayIndex]);
      }
    } else {
      MspDebugPrint ((MSP_DBG_MIN, "-- "));
    }
  } //  DelayIndex loop ...
  MspDebugPrint ((MSP_DBG_MIN, "%04d\n", LinearDelay));
  *Value = LinearDelay;

}

VOID
PrintTaskNameType (
  IN    TASK_DESCRIPTOR     *j
)
{
#if CAPSULESTRINGS

  BOOLEAN   IsStringEqual;

  IsStringEqual = FALSE;
  //
  // Following string comparisons are tied up to Call Table
  // in Cover Sheet at DNV_MRC.xls, in case of changes for training
  // stages in Call Table then changes might be required for a correct
  // START_/STOP_ serial output display.
  //

  if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"ReceiveEnable",13)) {
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"EarlyMprRead",12)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_ReceiveEnable\n"));
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"FineWriteLeveling",17)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_EarlyMprRead\n"));
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"CoarseWriteLeveling",19)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_FineWriteLeveling\n"));
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"ReadTraining",12)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_CoarseWriteLeveling\n"));
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"WriteTraining",13)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_ReadTraining\n"));
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"CommandClockTraining",20)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_WriteTraining\n"));
    IsStringEqual = TRUE;
  } else if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"RxTxSkewCtl",11)) {
    MspDebugPrint((MSP_DBG_MIN, " STOP_CommandClockTraining\n"));
  }

  if (IsStringEqual) {
    MspDebugPrint((MSP_DBG_MIN, " START_%s\n", CapsuleStrings[j->StringIndex]));
  } else {
    MspDebugPrint((MSP_DBG_MIN, " %s\n", CapsuleStrings[j->StringIndex]));
  }
#endif //CAPSULESTRINGS

}

UINT16 
GetHalfClk (
  IN        MMRC_DATA         *MrcData,
  IN        UINT8             Channel
  )
{
  return HalfClk [ADdll [MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]];
}

/**
Check if the Per bit should be executed per project basis

@param[in, out]  MrcData         Host structure for all data related to MMRC.

**/
MMRC_STATUS
CheckAdvConstrains (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             *Str,
IN      UINT8             *function,
IN      UINT32            line
)
{
  MspDebugPrint ((MSP_DBG_MIN, "%s, in %s:%d\n", Str, function, line));
  //
  // Avoton does not per bit
  //
  MrcData->ErrorCode = MrcErrIllegalAdvConstrains;
  MrcDeadLoop();
  return MMRC_FAILURE;
}

UINT16 
GetQtrClk (
  IN        MMRC_DATA         *MrcData,
  IN        UINT8             Channel
  )
{
  return (GetHalfClk (MrcData, Channel) / 2);
}

UINT16 
GetOneClk (
  IN        MMRC_DATA         *MrcData,
  IN        UINT8             Channel
  )
{
  return (GetHalfClk (MrcData, Channel) * 2);
}

/**
  This routine converts a PFCT value from BITx to x. 
  The Current* variables must not be in BITx format. For example 
  if CurrentPlatform is set to BIT4, this routine sets 
  CurrentPlatform to 4 instead. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          NONE 
**/
VOID
ConvertPfctFromBitXToX (
  IN        MMRC_DATA         *MrcData
  )
{
  UINT8 i;
  UINT8 Channel;
  //
  // Since Current* variables is setup as BITx, need to convert BITX into x for the variable
  // that MMRC needs to do its platform comparisons in the CreatePfctSel.
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    for (i = 0; i < 16; i++) {
      if ((MrcData->MspData.CurrentPlatform[Channel] & (1 << i)) != 0) {
        MrcData->MspData.CurrentPlatform[Channel] = i;
      }
      if ((MrcData->MspData.CurrentFrequency[Channel] & (1 << i)) != 0) {
        MrcData->MspData.CurrentFrequency[Channel] = i;
      }
      if ((MrcData->MspData.CurrentConfiguration[Channel] & (1 << i)) != 0) {
        MrcData->MspData.CurrentConfiguration[Channel] = i;
      }
      if ((MrcData->MspData.CurrentTechnology[Channel] & (1 << i)) != 0) {
        MrcData->MspData.CurrentTechnology[Channel] = i;
      }
    }
  }
}

/**
  This function compares the current PFCT of this channel and 
  match it to the list of allowable PFCTs for this Task in the 
  call table. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       j               Task Descriptor.
  @retval          NONE
**/
VOID
CreateChannelMask (
  IN        MMRC_DATA            *MrcData,
  IN        TASK_DESCRIPTOR      *j
  )
{
  UINT8                   Channel;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (((j->PList & (1 << MrcData->MspData.CurrentPlatform[Channel]))      != 0) &&
        ((j->FList & (1 << MrcData->MspData.CurrentFrequency[Channel]))     != 0) &&
        ((j->CList & (1 << MrcData->MspData.CurrentConfiguration[Channel])) != 0) &&
        ((j->TList & (1 << MrcData->MspData.CurrentTechnology[Channel]))       != 0)
      )
    {
      MrcData->MspData.ExecuteOnThisChannel[Channel] = TRUE;
    } else {
      MrcData->MspData.ExecuteOnThisChannel[Channel] = FALSE;
    }
  }
}

/**
  Single entry point to MMRC. MRC calls this function
  and then MMRC acts as a black box, performing the requested
  stage of DDRIO init and returns when complete or when
  an error is encountered.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Action          Phy init, training, post-training, etc. 
  @param[in, out]  Parameters      A structure used to pass data between MRC and MMRC.        
  @retval          Status    
**/
MMRC_STATUS
MmrcEntry (
  IN  OUT   MMRC_DATA            *MrcData,
  IN        MMRC_STAGE            Action,
  IN  OUT   HANDSHAKE_PARAMETERS *Parameters
  )
{
  MMRC_STATUS             Status;
  UINTX                   ConditionalExec;
  UINT8                   Channel;
  BOOLEAN                 ChannelBackup[MAX_CHANNELS];
  TASK_DESCRIPTOR        *j;
  UINT8                   BackupMessage;
  UINT32                 initCntLow;
  UINT32                 endCntLow;
  UINT32                 elapsedTime;

  Status = MMRC_SUCCESS;
  BackupMessage = MrcData->MspData.DebugMsgLevel;
  //
  // Convert BITx values to x instead for Current* variables which contain the current PFCT.
  //
  ConvertPfctFromBitXToX (MrcData);
  //
  // Lock the Msp Data structure before executing MspExecuteTask
  //
  LockMspDataStructure ();
  //
  // Execute all steps for the given Stage.
  //
  switch (Action) {
    case EntireMmrc:
      j = InitTasks;
      if (Parameters != NULL) {
        if (Parameters->ChannelValid) {
          j->Channel = Parameters->Channel;
        }
      }
      for (j; j->Function != NULL; j++) {
        if ((MrcData->MrcParameters.BootMode & j->BootMode) != 0) {
          MrcData->PostCode = j->PostCode;
          HandleCheckpoint (MrcData, j->PostCode);
          //
          // Mark the channels which will be executing this routine. Some channels might skip execution
          // based on their PFCT and Conditional Variable.
          //
          CreateChannelMask (MrcData, j);
          if ((j->Channel == ChAll) || (j->Channel == ChAllP && !MrcData->EnableParallelTraining)) {
            for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
              //
              // Mark the channels which will be executing this routine. Some channels might skip execution
              // based on their PFCT and Conditional Variable.
              //
              CreateChannelMask (MrcData, j);
              if (!MrcData->MrcParameters.ChEnabled[Channel] || !MrcData->MspData.ExecuteOnThisChannel[Channel]) {
                continue;
              }
              if (j->CondExec != DYNVAR_MAX) {
                ConditionalExec = (UINTX) MrcData->MspData.DynamicVars[Channel][j->CondExec];
                if (ConditionalExec == 0) {
                  continue;
                }
              }
              if (j->CapsuleStartIndex == EXTERNAL) {
                PrintTaskNameType (j);
                if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"ReInitializeFunction",20)) {
                  if (MrcData->MspData.DebugMsgLevel != 0) {
                    MrcData->MspData.DebugMsgLevel |= MSP_DBG_VERBOSE; 
                  }
                }
              }
              //
              // Parallel training setup
              // We only enable the channel under test when the parallel training is not enabled
              //
              MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
              MmrcMemset (&MrcData->MspData.ExecuteOnThisChannel[0], 0, MAX_CHANNELS * sizeof (BOOLEAN));
              MrcData->MspData.ExecuteOnThisChannel[Channel] = MrcData->MrcParameters.ChEnabled[Channel];
              initCntLow  = Mmio32Read (ICH_HPET_BASE_ADDRESS + 0xF0);
              Status = (*(j->Function)) (MrcData, j->CapsuleStartIndex, j->StringIndex, Channel);
              endCntLow  = Mmio32Read (ICH_HPET_BASE_ADDRESS + 0xF0);
              if (endCntLow > initCntLow) {
                elapsedTime = ((endCntLow - initCntLow) / 14);
              } else {
                elapsedTime = ((0xFFFFFFFF - (initCntLow - endCntLow)) / 14);
              }
              MspDebugPrint ((MSP_DBG_MIN, "elapsedTime: %d(us)\n", elapsedTime));
              if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"ReInitializeFunction",20)) {
                MrcData->MspData.DebugMsgLevel = BackupMessage;
              }
              if (Status != MMRC_SUCCESS) {
                return Status;
              }
            }
          } else if (j->Channel == ChAllP) {
            if (j->CapsuleStartIndex == EXTERNAL) {
              PrintTaskNameType (j);
            }
            MrcData->MspData.ExecuteThisRoutineInParallel = TRUE;
            Status = (*(j->Function)) (MrcData,j->CapsuleStartIndex, j->StringIndex, 0);
            if (Status != MMRC_SUCCESS) {
              return Status;
            }
          } else if (j->Channel == ChNone) {
            if (j->CapsuleStartIndex == EXTERNAL) {
              PrintTaskNameType (j);
              if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"ReInitializeFunction",20)) {
                if (MrcData->MspData.DebugMsgLevel != 0) {
                  MrcData->MspData.DebugMsgLevel |= MSP_DBG_VERBOSE; 
                }
              }
            }
            //
            // ChNone - we use Channel 0 in this case as a dummy placeholder.
            // Anyway the Platform, Frequency, Configuration and Technology
            // needs to be checked prior to execute the task
            //
            CreateChannelMask (MrcData, j);
            for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
              // 
              // Channels 1..MAX_CHANNELS will get disabled, we have to backup the current value
              // and only turn on channel 0, just in case it detects any channel needs to get 
              // executed after checking the PFCT mask returns TRUE
              //
              ChannelBackup[Channel] = MrcData->MspData.Enabled[Channel];
              if (MrcData->MspData.ExecuteOnThisChannel[Channel]) {
                MrcData->MspData.ExecuteOnThisChannel[0] = TRUE;
                MrcData->MspData.Enabled[0] = TRUE;
              }
            }
#if MAX_CHANNELS > 1
            for (Channel = 1; Channel < MAX_CHANNELS; Channel++) {
              MrcData->MspData.ExecuteOnThisChannel[Channel] = FALSE;
              MrcData->MspData.Enabled[Channel] = FALSE;
            }
#endif
            //
            // Restore back which channels should be enabled.
            //
            for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
              MrcData->MspData.Enabled[Channel] = ChannelBackup[Channel];
            }
            //
            // Continue with the next function in the call table if there are no channels enabled
            // or if the conditional Variable is not set to TRUE
            //
            if (!MrcData->MspData.ExecuteOnThisChannel[0]) {
              continue;
            }
            if (j->CondExec < DYNVAR_MAX) {
              ConditionalExec = (UINTX) MrcData->MspData.DynamicVars[0][j->CondExec];
              if (ConditionalExec == 0) {
                continue;
              }
            }
            MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
            Status = (*(j->Function)) (MrcData, j->CapsuleStartIndex, j->StringIndex, 0);
            if (MmrcStringCompare(CapsuleStrings[j->StringIndex],"ReInitializeFunction",20)) {
              MrcData->MspData.DebugMsgLevel = BackupMessage;
            }
            if (Status != MMRC_SUCCESS) {
              return Status;
            }
          } else {
            if (j->CapsuleStartIndex == EXTERNAL) {
              PrintTaskNameType (j);
            }
            //
            // We're here because a specific channel has been requested.
            //
            if (MrcData->MrcParameters.ChEnabled[j->Channel] && MrcData->MspData.ExecuteOnThisChannel[j->Channel]) {
              MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
              Status = (*(j->Function)) (MrcData, j->CapsuleStartIndex, j->StringIndex, j->Channel);
              if (Status != MMRC_SUCCESS) {
                return Status;
              }
            }
          }
        } // if ((MrcData->MrcParameters.BootMode & j->BootMode) != 0) {
      } // for (j; j->Function != NULL; j++) {
      return Status;

  case ExecuteSpecificFunction:
    //
    // This is used for when a caller outside the MRC (like EV scripts) call into
    // the MMRC to execute a particular function (such as RMT). Always execute, regardless
    // of boot path.
    //
    // If the caller passed in a channel on which to operate, execute on that channel.
    // Otherwise, call routine once without a channel specified.
    //
    if (!Parameters->ChannelValid) {
      Parameters->Channel = ChNone;
    }
    if (Parameters->Function != NULL) {
      return (*(Parameters->Function)) (MrcData, Parameters->Channel);
    } else {
      MspDebugPrint ((MSP_DBG_MIN, "NULL function\n"));
    }

    break;

  default:
    MspDebugPrint ((MSP_DBG_MIN, "Invalid stage (%d) passed to MMRC\n", Action));
    break;
  }

  return Status;
}

VOID * 
MmrcMemset (
  void      *dst,
  char      value,
  UINT32    cnt
  )
{
  UINT8 *dst8 = (UINT8 *) dst;
  while (cnt--) {
    *dst8 = value;
    dst8++;
  }
  return dst;
}

VOID *
MmrcMemCopy (
  void      *dst,
  void      *src,
  UINT32    cnt
  )
{
  UINT8 *dst8 = (UINT8 *) dst;
  UINT8 *src8 = (UINT8 *) src;
  while (cnt--) {
    *dst8 = *src8;
    dst8++;
    src8++;
  }
  return dst;
}

/**
  This function detects the platform under test.

  @param[in]       *str1      Pointer to String 1 
  @param[in]       *str2      Pointer to String 2 
  @param[in]       cnt        Length for both strings
  @retval          TRUE       If both strings are the same
**/

BOOLEAN
MmrcStringCompare (
  IN    VOID    *str1, 
  IN    VOID    *str2, 
  IN    UINT32  cnt
  )
{
  
  UINT32      Index;
  UINT8       *StringA;
  UINT8       *StringB;

  StringA = (UINT8 *) str1;
  StringB = (UINT8 *) str2;

  for (Index = 0; Index < cnt; Index++) {
    if (StringA[Index] != StringB[Index]) {
      return FALSE;
    }
  }
  return TRUE;
}

UINT32
StringLength (
  char *String
  )
{
  UINT32 i;
  i = 0;
  while (String[i++] != 0);
  return i;
}

#if DUMP_REGISTER_NAMES
UINT32 
GetRegisterNumber (
  UINT8  Box,
  UINT32 Offset
  )
{
  MSP_DATA *MspData;
  UINT32 RegisterNumber;
  MspData = GetMspHostStructureAddress();
  //
  // Grab the first register string for this boxtype.
  //
  RegisterNumber = InstancePortMap[Box].StringIndex;
  if (RegisterNumber == 0xFFFFFFFF) {
    return RegisterNumber;
  }
  //
  // Loop through all registers for this box....
  //
  while (Registers[RegisterNumber].RegInfo != NULL) {
    if (Registers[RegisterNumber].RegInfo->Offset == Offset) {
      break;
    }
    RegisterNumber++;
  }
  return RegisterNumber;
}
#endif // DUMP_REGISTER_NAMES

/**
  This routine dumps all registers in the MMRC spreadsheet. 
  On the cover sheet of the SS, the user can control the debug 
  level output: 

  DUMP_REGISTER_NAMES = 1 means all register names, ports, 
  offsets, values, and default values will be printed. 
   
  DUMP_REGISTER_FIELDS = 1 means all register field names, 
  values, default values, and access types will be printed. 
   
  DUMP_REGISTER_DESCRIPTIONS = 1 will display the description 
  for each register field. 

  If DUMP_REGISTER_NAMES is set to 0, the other enables are 
  don't-cares as this is the global enable for this routine.

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
  DumpAllRegisters (
  )
{
#if DUMP_REGISTER_NAMES
  UINT32 RegisterNumber;
  UINT32 i;
  UINT8  Box;
  UINT8  Channel;
  UINT8  Instance;
  UINT8  Port;
  UINT8  Bus;
  UINT8  Device;
  UINT8  Function;
  UINT32 Junk;
  UINT32 BaseOffset;
  REGISTER_ACCESS Register;
  UINTX  Value;
  UINT8  TotalChannels;
  UINT8  TotalInstances;
#if DUMP_REGISTER_FIELDS
  UINT8  j;
  UINTX  UpperMask;
  UINTX  LowerMask;
#endif
  UINT8  MaxBit;
  MSP_DATA *MspData;

#if USE_64_BIT_VARIABLES
  MaxBit = 64;
#else
  MaxBit = 32;
#endif

  MspData = GetMspHostStructureAddress ();
  for (Box = 0; Box < MAX_BOXES; Box++) {
    //
    // If there are no registers for this box, skip the printing.
    //
    if (InstancePortMap[Box].StringIndex == 0xFFFFFFFF) {
      continue;
    }
    // 
    // If this box only has 1 sideband access command, assume it's write-only so we can't 
    // read it. This is for things like opcode 0x68 in the DUNIT where you cannot read.
    //
    if (InstancePortMap[Box].AccessMethod == eSB && InstancePortMap[Box].ReadOperation == InstancePortMap[Box].WriteOperation) {
      continue;
    }
    if (InstancePortMap[Box].Instances == 0) {
      TotalInstances = InstancePortMap[Box].TotalInstances;
      TotalChannels  = 1;
    } else {
      TotalInstances = InstancePortMap[Box].Instances;
      TotalChannels  = (InstancePortMap[Box].TotalInstances / InstancePortMap[Box].Instances);
    }
    for (Channel = 0; Channel < TotalChannels; Channel++) {
      for (Instance = 0; Instance < TotalInstances; Instance++) {
        if (GetRegisterAccessInfo (Box, Channel, Instance, &Junk, &Port, &BaseOffset, &Bus, &Device, &Function, ModeRead) == NoError) {
          //
          // Print the header for each instance.
          //
#if CAPSULESTRINGS
          MspDebugPrint ((MSP_DBG_MIN, "%s%d\n", UnitStrings[Box], (Channel * InstancePortMap[Box].Instances) + Instance));
#endif
          MspDebugPrint ((MSP_DBG_MIN, "Name"));
          for (i = 0; i < MAX_REGISTER_NAME_LENGTH + 1; i++) {
            MspDebugPrint ((MSP_DBG_MIN, " "));
          }
#if DUMP_REGISTER_FIELDS
          MspDebugPrint ((MSP_DBG_MIN, "Port     Offset   Bits               Value            Default  Access\n"));
          for (i = 0; i < MAX_REGISTER_NAME_LENGTH + 74; i++) {
            MspDebugPrint ((MSP_DBG_MIN, "_"));
          }
#else
          MspDebugPrint ((MSP_DBG_MIN, "Port     Offset   Bits               Value            Default\n"));
          for (i = 0; i < MAX_REGISTER_NAME_LENGTH + 66; i++) {
            MspDebugPrint ((MSP_DBG_MIN, "_"));
          }
#endif
          MspDebugPrint ((MSP_DBG_MIN, "\n"));
          //
          // Grab the first register string for this boxtype.
          //
          RegisterNumber = InstancePortMap[Box].StringIndex;
          //
          // Loop through all registers for this box....
          //
          while (Registers[RegisterNumber].RegInfo != NULL) {
            //
            // Display the register name, padding with spaces to align everything. Typically you could
            // use printf ("%-15s", string), but this doesn't work in the MiniBIOS, so pad manually here.
            //
            MspDebugPrint ((MSP_DBG_MIN, "%s", Registers[RegisterNumber].RegInfo->RegisterName));
            for (i = StringLength (Registers[RegisterNumber].RegInfo->RegisterName); i <= MAX_REGISTER_NAME_LENGTH + 2; i++) {
              MspDebugPrint ((MSP_DBG_MIN, " "));
            }
            //
            // Display port, offset, and value.
            //
            Register.Offset = Registers[RegisterNumber].RegInfo->Offset;
            Register.Mask   = Registers[RegisterNumber].RegInfo->Mask;
            Value = MemRegRead (Box, Channel, Instance, Register);
            if (InstancePortMap[Box].AccessMethod == ePCI) {
              MspDebugPrint ((MSP_DBG_MIN, "%02X/%02X/%01X ", Bus, Device, Function));
            } else if (InstancePortMap[Box].AccessMethod == eBAR) {
              MspDebugPrint ((MSP_DBG_MIN, "        "));
            } else {
              MspDebugPrint ((MSP_DBG_MIN, "   0x%02X ", Port));
            }
#if USE_64_BIT_VARIABLES
            MspDebugPrint ((MSP_DBG_MIN, "0x%08X [xx:xx] 0x%08X%08X 0x%08X%08X\n", 
              Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
              (UINT32) (Value >> 32),
              (UINT32) Value,
              (UINT32) (Registers[RegisterNumber].RegInfo->DefaultValue >> 32),
              (UINT32) Registers[RegisterNumber].RegInfo->DefaultValue
              ));
#else
            MspDebugPrint ((MSP_DBG_MIN, "0x%08X [xx:xx] 0x%08X%08X 0x%08X%08X\n", 
              Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
              0,
              (UINT32) Value,
              0,
              (UINT32) Registers[RegisterNumber].RegInfo->DefaultValue
              ));
#endif
#if DUMP_REGISTER_FIELDS
            //
            // Display all register fields with proper start/end bits and values.
            //
            for (i = 0; i < Registers[RegisterNumber].RegInfo->NumFields; i++) {
              MspDebugPrint ((MSP_DBG_MIN, "  %s", Registers[RegisterNumber].RegFields[i].FieldName));
              for (j = (UINT8) StringLength (Registers[RegisterNumber].RegFields[i].FieldName); j <= MAX_REGISTER_NAME_LENGTH; j++) {
                MspDebugPrint ((MSP_DBG_MIN, " "));
              }
              //
              // Calculate the mask based on start and end bits.
              //
              if (Registers[RegisterNumber].RegFields[i].EndBit == (MaxBit - 1)) {
#if USE_64_BIT_VARIABLES
                UpperMask = 0xFFFFFFFFFFFFFFFF;
#else
                UpperMask = 0xFFFFFFFF;
#endif
              } else {
                UpperMask = (((UINTX)1 << (Registers[RegisterNumber].RegFields[i].EndBit + 1)) - 1);
              }
              LowerMask = (((UINTX)1 << Registers[RegisterNumber].RegFields[i].StartBit) - 1);
              Register.Mask = UpperMask - LowerMask;
              //
              // Display port, offset, start/end bits, value, and default value.
              //
              Register.Offset = Registers[RegisterNumber].RegInfo->Offset;
              Register.ShiftBit = Registers[RegisterNumber].RegFields[i].StartBit;
              Value = MemFieldRead (Box, Channel, Instance, Register);
              if (InstancePortMap[Box].AccessMethod == ePCI) {
                MspDebugPrint ((MSP_DBG_MIN, "%02X/%02X/%01X ", Bus, Device, Function));
              } else if (InstancePortMap[Box].AccessMethod == eBAR) {
                MspDebugPrint ((MSP_DBG_MIN, "        "));
              } else {
                MspDebugPrint ((MSP_DBG_MIN, "   0x%02X ", Port));
              }
#if USE_64_BIT_VARIABLES
              MspDebugPrint ((MSP_DBG_MIN, "0x%08X [%02d:%02d] 0x%08X%08X 0x%08X%08X", 
                Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
                Registers[RegisterNumber].RegFields[i].EndBit,
                Registers[RegisterNumber].RegFields[i].StartBit,
                (UINT32) (Value >> 32),
                (UINT32) Value,
                (UINT32) (Registers[RegisterNumber].RegFields[i].DefaultFieldValue >> 32),
                (UINT32) Registers[RegisterNumber].RegFields[i].DefaultFieldValue
                ));
#else
              MspDebugPrint ((MSP_DBG_MIN, "0x%08X [%02d:%02d] 0x%08X%08X 0x%08X%08X", 
                Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
                Registers[RegisterNumber].RegFields[i].EndBit,
                Registers[RegisterNumber].RegFields[i].StartBit,
                0,
                (UINT32) Value,
                0,
                (UINT32) Registers[RegisterNumber].RegFields[i].DefaultFieldValue
                ));
#endif
              //
              // Display the access type (RW, RO, etc)
              //
              MspDebugPrint ((MSP_DBG_MIN, "  %s\n", Registers[RegisterNumber].RegFields[i].AccessType));
              //
              // Optionally display the register field description (this takes up a LOT of space for
              // the strings so only use this mode in an environment that isn't space limited such
              // as RTL simulation or JTAG mode when running from a host machine.
              //
#if DUMP_REGISTER_DESCRIPTIONS
              MspDebugPrint ((MSP_DBG_MIN, "%s\n", Registers[RegisterNumber].RegFields[i].Description)); 
#endif
            }
#endif
            RegisterNumber++;
          }

          MspDebugPrint ((MSP_DBG_MIN, "\n"));
        }
      }
    }
  }
#endif
  return MMRC_SUCCESS;
}

#if FAULTY_PART_TRACKING
/**
  Multiplication

  @param[in]       Multiplicand   
  @param[in]       Multiplier              
  @retval          Multiplication Result    
**/
UINT64
InternalMathMultU64x32 (
  IN      UINT64                    Multiplicand,
  IN      UINT32                    Multiplier
  )
{
  _asm {
    mov     ecx, Multiplier
    mov     eax, ecx
    imul    ecx, dword ptr [Multiplicand + 4]  // overflow not detectable
    mul     dword ptr [Multiplicand + 0]
    add     edx, ecx
  }
}

/**
  Division

  @param[in]       Dividend   
  @param[in]       Divisor 
  @param[out]      Remainder  
  @retval          Division Result    
**/
UINT64
InternalMathDivRemU64x32 (
  IN      UINT64                    Dividend,
  IN      UINT32                    Divisor,
  OUT     UINT32                    *Remainder
  )
{
  _asm {
    mov     ecx, Divisor
    mov     eax, dword ptr [Dividend + 4]
    xor     edx, edx
    div     ecx
    push    eax
    mov     eax, dword ptr [Dividend + 0]
    div     ecx
    mov     ecx, Remainder
    jecxz   RemainderNull                      // abandon remainder if Remainder == NULL
    mov     [ecx], edx
RemainderNull:
    pop     edx
  }
}

/**
  Count the number of bits set on a 32 bit segister with 6 arihmetic operations

  @param[in]       Value     32 bit value to operate on 
  @retval          The number of bits set on the 32 bit value    
**/
UINT8
CountOnes (
  UINT32  Value
  )
{
  UINT32 cnt;
  UINT32 cnt2;
  cnt = 0;
  cnt2 = 0;
  //
  //cnt =  ((Value & 0xfff) * 0x1001001001001L & 0x84210842108421L) % 0x1f;
  //
  InternalMathDivRemU64x32((InternalMathMultU64x32(LOOKUP_TABLE_1,(Value & MASK_LOWER_12BIT)) & LOOKUP_TABLE_2), MODULE31, &cnt);
  //
  //cnt += (((Value & 0xfff000) >> 12) * 0x1001001001001L & 0x84210842108421L) % 0x1f;
  //
  InternalMathDivRemU64x32((InternalMathMultU64x32(LOOKUP_TABLE_1,((Value & (MASK_LOWER_12BIT << 12)) >> 12)) & LOOKUP_TABLE_2), MODULE31, &cnt2);
  cnt += cnt2;
  //
  //cnt += ((Value >> 24) * 0x1001001001001L & 0x84210842108421L) % 0x1f;
  //
  InternalMathDivRemU64x32((InternalMathMultU64x32(LOOKUP_TABLE_1,(Value >> 24)) & LOOKUP_TABLE_2), MODULE31, &cnt2);
  cnt += cnt2;
  return (UINT8) cnt;
}

/**
  Initializes the error tracking variables to the maximum value. 
  This is used before each CPGC based training

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current Channel being examined.
  @param[in]       FirstTraining   
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FaultyPartTrackInit (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        BOOLEAN       FirstTraining
  )
{
  UINT8 Rank;
  Rank = 0;

  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      //
      // Initialize the error tracking with the max value and update with less than max value
      //
      MrcData->FaultyPart[Channel][Rank].Byte0to3 = (UINT32)(~(0));
      MrcData->FaultyPart[Channel][Rank].Byte4to7 = (UINT32)(~(0));
      MrcData->FaultyPart[Channel][Rank].ByteEcc = (UINT8)(~(0));
      //
      // Initialize the cumulative variables for historic error tracling across all trainings
      //
      if (FirstTraining) {
        MrcData->FaultyPart[Channel][Rank].DataBitFailCumulativeHigh = 0;
        MrcData->FaultyPart[Channel][Rank].DataBitFailCumulativeLow = 0;
        MrcData->FaultyPart[Channel][Rank].EccBitFailCumulative = 0;
      }
    }
    if (!MrcData->MspData.ExecuteThisRoutineInParallel) {
      //
      // Stop the routine after first execution
      // if not supposed to be executed in parallel
      //
      break;
    }
  } // Channel loop ...

  return MMRC_SUCCESS;
}

/**
  Process the data from the CPGC test in order to print the 
  device and the bits that failed for debug porposes.
  This function supports handling of x8 and x4 devices if a 
  different width needs to add support for it. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]   FaultyPartRankLow   Bits of bytes 0 to 3
  @param[in]   FaultyPartRankHi    Bits of bytelanes 4 to 7
  @param[in]   FaultyPartRankEcc   Bits of the ECC bytelane.
  @retval      MMRC_SUCCESS
**/
STATIC 
MMRC_STATUS
FaultyPartProcessDeviceData (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT32        FaultyPartRankLow,
  IN        UINT32        FaultyPartRankHi,
  IN        UINT8         FaultyPartRankEcc
  )
{
  UINT8  StrobeBitLanes;
  UINT8  Strobe;
  UINT8  StrobeMask;
  UINT8  StrobeBits;
  UINT8  LowCrsLimit;
  UINT8  HighCsrLimit;
  UINT8  EccCsrLimit;

  if (MrcData->MaxDq[Channel][Rank] <= MAX_STROBE_X8_DEVICES) {
    StrobeMask = BYTE_MASK;
    StrobeBits = CountOnes(BYTE_MASK);
    LowCrsLimit = 4;  
    HighCsrLimit = 3 ; 
    EccCsrLimit = 8;  
  } else {
    StrobeMask = NIBBLE_MASK;
    StrobeBits = CountOnes(NIBBLE_MASK);
    LowCrsLimit = 8;  
    HighCsrLimit = 7 ; 
    EccCsrLimit = 17;  
  }

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    StrobeBitLanes = 0;
    if ((Strobe < LowCrsLimit)) {
      StrobeBitLanes = ((FaultyPartRankLow >> (StrobeBits * Strobe)) & StrobeMask);
    }
    if ((Strobe > HighCsrLimit) && (Strobe < EccCsrLimit)) {
      StrobeBitLanes = ((FaultyPartRankHi >> (StrobeBits * (Strobe - LowCrsLimit))) & StrobeMask);
    }
    if ((Strobe >= EccCsrLimit)) {
      StrobeBitLanes = ((FaultyPartRankEcc >> (StrobeBits * (Strobe - EccCsrLimit))) & StrobeMask);
    }
    if (StrobeBitLanes != 0) {
      MspDebugPrint ((MSP_DBG_MED, "\tDevice:%d has %d bit(s) failing. Strobe's value is:0x%x \n",
        Strobe, CountOnes(StrobeBitLanes), StrobeBitLanes
        ));
    }
  }
  return MMRC_SUCCESS;
}

/**
  Programs Bunit to enable/disable the second channel

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current Channel being examined.
**/
MMRC_STATUS
FaultyPartDisableChannel (
  IN  OUT   MMRC_DATA             *MrcData,
  IN        UINT8                 Channel
  )
{
#if PUNIT_MAILBOX == 1
  //
  //  Currently we can only disable channel 1
  //
  MmrcExecuteTask (MrcData, DisableChannel, NO_PRINT, Channel);
  //
  // The reset will detect dimms again and try to enable the channel again and we will be on a loop here, is the Bmisc sticky??
  //
  MspDebugPrint ((MSP_DBG_MIN, "Disabling Channel Faulty Part\n"));
  IoOut8 (0xCF9, 0x0);
  IoOut8 (0xCF9, 0xE);
#ifndef SIM
  _asm hlt
#endif

#endif  
  return MMRC_SUCCESS;
}

/**
  Process the faulty part data gathered by the CPGC based 
  training 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current Channel being examined.
**/
MMRC_STATUS
FaultyPartProcessTrainingResult (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  UINT32 FaultyPartRankLow;
  UINT32 FaultyPartRankHi;
  UINT8  FaultyPartRankEcc;
  UINT8  Rank;
  //
  // Faulty part tracking per training step processing
  //
  if (MrcData->FaultyPartTracking && MrcData->RestorePath == FALSE) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->RankEnabled[Channel][Rank]) {
        //
        // Failures on the current training and previews trainings
        //
        FaultyPartRankLow = MrcData->FaultyPart[Channel][Rank].Byte0to3 | MrcData->FaultyPart[Channel][Rank].DataBitFailCumulativeLow;
        FaultyPartRankHi  = MrcData->FaultyPart[Channel][Rank].Byte4to7 | MrcData->FaultyPart[Channel][Rank].DataBitFailCumulativeHigh;
        FaultyPartRankEcc = MrcData->FaultyPart[Channel][Rank].ByteEcc | MrcData->FaultyPart[Channel][Rank].EccBitFailCumulative;

        if (FaultyPartRankLow != 0 || FaultyPartRankHi != 0 || FaultyPartRankEcc != 0) {
          //
          // Acumulate failures through out all trainings
          //
          MrcData->FaultyPart[Channel][Rank].DataBitFailCumulativeLow = FaultyPartRankLow;
          MrcData->FaultyPart[Channel][Rank].DataBitFailCumulativeHigh = FaultyPartRankHi;
          MrcData->FaultyPart[Channel][Rank].EccBitFailCumulative  = FaultyPartRankEcc;

          if (((CountOnes(FaultyPartRankLow) + CountOnes(FaultyPartRankHi)) > FAULTY_PART_CORRECTABLE) || (FaultyPartRankEcc != 0)) {
            MrcData->FaultyPart[Channel][Rank].ErrorType = FAULTY_PART_UNCORRECTABLE;
          } else {
            MrcData->FaultyPart[Channel][Rank].ErrorType = FAULTY_PART_CORRECTABLE;
          }
          //
          // Display the error on serial log
          //
          if (MrcData->FaultyPart[Channel][Rank].ErrorType == FAULTY_PART_UNCORRECTABLE) {
            MspDebugPrint ((MSP_DBG_MED, "FAULTY_PARTS_TRACKING -UNCORRECTABLE- Channel:%d Rank:%d \n", Channel, Rank));            
            //
            // if there is uncorrectable error on channel 1 disable it else halt because there is an uncorrectable on channel 0
            //          
            if (Channel != 0) {
              FaultyPartDisableChannel(MrcData, Channel);
            } else {            
              FaultyPartProcessDeviceData (MrcData, Channel, Rank, FaultyPartRankLow, FaultyPartRankHi, FaultyPartRankEcc);
              return MrcErrFaultyPartUncorrectable;
            }
          } else {
            MspDebugPrint ((MSP_DBG_MED, "FAULTY_PARTS_TRACKING -CORRECTABLE- Channel:%d Rank:%d \n", Channel, Rank));
            if (MrcData->FaultyPartContinueOnCorrectable  == FALSE) {
              MspDebugPrint ((MSP_DBG_MED, "FAULTY_PARTS_TRACKING -CORRECTABLE- Halt on correctable \n"));
              FaultyPartProcessDeviceData (MrcData, Channel, Rank, FaultyPartRankLow, FaultyPartRankHi, FaultyPartRankEcc);
              return MrcErrFaultyPartCorrectable;
            }
          }
          FaultyPartProcessDeviceData (MrcData, Channel, Rank, FaultyPartRankLow, FaultyPartRankHi, FaultyPartRankEcc);
          if (MrcData->MrcParameters.ChEnabled[Channel] == 0) {
            MspDebugPrint ((MSP_DBG_MED, "FAULTY_PARTS_TRACKING -CORRECTABLE promoted to UNCORRECTABLE-"));
            MspDebugPrint ((MSP_DBG_MED, "Channel %d will not be trained from this point on\n", Channel));
          }
        }  //any failure found
      }
    }
    MspDebugPrint ((MSP_DBG_MED, "\n\r"));
  }
  return MMRC_SUCCESS;
}

/**
  This API will report out the error Status for byte level, 
  bit level and ECC lane.
  If the check low/high Bits variables are non-NULL, the bit field 
  results are copied into the pointer location.
  Similarly, if the Byte Lane or ECC or non-null, 
  these results are copied into their respective locations.

  @param[in, out]  MrcData       Host structure for all data related to MMRC. 
  @param[in]       Channel       Current Channel being examined.
  @param[in]       Rank          Current Rank being examined.
  @param[in]   LowBitsErrStat    Pointer for Low bits level for error reporting
  @param[in]   HighBitsErrStat   Pointer for High bits level for error reporting
  @param[in]   ByteLanesErrStat  Pointer for ECC level of error reporting
  @retval      MMRC_SUCCESS
**/
STATIC
MMRC_STATUS
CpgcSCheckErrors (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN  OUT   UINT32        *LowBitsErrStat,
  IN  OUT   UINT32        *HighBitsErrStat,
  IN  OUT   UINT32        *ByteLanesErrStat
  )
{
  UINT32 Cpgc_Err_Stat_Lo;
  UINT32 Cpgc_Err_Stat_Hi;
  UINT8  Bytegrp_err_stat;
  UINT8  Ecc_lane_err_stat;
  UINT8  Strobe;
  UINT8  StrobeBitLanes;
  UINT8  StrobeBitLanes2;
  UINT8  StrobeMask;
  UINT8  StrobeBits;
  UINT8  LowCrsLimit;
  UINT8  HighCsrLimit;
  UINT8  EccCsrLimit;

  Strobe = 0;
  StrobeBitLanes = 0;
  StrobeBitLanes2 = 0;
  StrobeMask = 0;  
  StrobeBits = 0;  
  LowCrsLimit = 0; 
  HighCsrLimit = 0;
  EccCsrLimit = 0; 

  if (MrcData->FaultyPartTracking) {
    if (MrcData->MaxDq[Channel][Rank] <= MAX_STROBE_X8_DEVICES) {
      StrobeMask = BYTE_MASK;
      StrobeBits = 8;
      LowCrsLimit = 4;  
      HighCsrLimit = 3; 
      EccCsrLimit = 8;  
    } else {
      StrobeMask = NIBBLE_MASK;
      StrobeBits = 4;
      LowCrsLimit = 8;  
      HighCsrLimit = 7; 
      EccCsrLimit = 17;  
    }
  }

  // 
  // Read CPGC Byte Group 0-3 Lane Error Status register
  //
  Cpgc_Err_Stat_Lo = (UINT32) MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT03];
  // 
  // Read CPGC Byte Group 4-7 Lane Error Status register
  //
  Cpgc_Err_Stat_Hi = (UINT32) MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT47];
  // 
  // Read CPGC Extended Error Status register
  //
  Bytegrp_err_stat = (UINT8) MrcData->MspData.DynamicVars[Channel][BYTEGRP_ERR_STAT];
  Ecc_lane_err_stat = (UINT8) MrcData->MspData.DynamicVars[Channel][ECC_ERR_STAT];
  //
  // We will keep this debug line
  // MspDebugPrint ((MSP_DBG_VERBOSE, "Rank: %d Debug FP Err_Lo:0x%x Err_hi:0x%x Bytegrp:0x%x Ecc:0x%x\n",
  //  Rank, Cpgc_Err_Stat_Lo, Cpgc_Err_Stat_Hi, Bytegrp_err_stat, Ecc_lane_err_stat
  //  ));

  // 
  // Copy Error Status Value to each respective pointer
  //
  if (LowBitsErrStat != NULL) {
    *LowBitsErrStat = Cpgc_Err_Stat_Lo;
  }
  if (HighBitsErrStat != NULL) {
    *HighBitsErrStat = Cpgc_Err_Stat_Hi;
  }
  if (ByteLanesErrStat != NULL) {
    *ByteLanesErrStat = (UINT8) Bytegrp_err_stat;
    *ByteLanesErrStat |= (Ecc_lane_err_stat << 8);
  }

  //
  // Per training find the least number of bits failures. Ideal case would be 0, correctable 
  // would be single bit error on data and uncorrectable more than single bit or ECC bytelane error 
  //
  if (MrcData->FaultyPartTracking) {

    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
      if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
        continue;
      }
      StrobeBitLanes = 0;
      StrobeBitLanes2 = 0;
      if ((Strobe < LowCrsLimit)) {
        StrobeBitLanes = ((Cpgc_Err_Stat_Lo >> (StrobeBits * Strobe)) & StrobeMask);
        StrobeBitLanes2 = ((MrcData->FaultyPart[Channel][Rank].Byte0to3 >> (StrobeBits * Strobe)) & StrobeMask);
        //
        // If what we got from status csr is smaller than what we previously saved we update the saved value
        //
        if (CountOnes (StrobeBitLanes) < CountOnes (StrobeBitLanes2)) {
          MrcData->FaultyPart[Channel][Rank].Byte0to3 &= (UINT32) ~(StrobeMask << (StrobeBits * Strobe));
          MrcData->FaultyPart[Channel][Rank].Byte0to3 |= (UINT32) ((StrobeBitLanes << (StrobeBits * Strobe)) & (StrobeMask << (StrobeBits * Strobe)));
        }
      }
      if ((Strobe > HighCsrLimit) && (Strobe < EccCsrLimit)) {
        StrobeBitLanes = ((Cpgc_Err_Stat_Hi >> (StrobeBits * (Strobe - LowCrsLimit))) & StrobeMask);
        StrobeBitLanes2 = ((MrcData->FaultyPart[Channel][Rank].Byte4to7 >> (StrobeBits * (Strobe - LowCrsLimit))) & StrobeMask);
        if (CountOnes (StrobeBitLanes) < CountOnes (StrobeBitLanes2)) {
          MrcData->FaultyPart[Channel][Rank].Byte4to7 &= (UINT32) ~(StrobeMask << (StrobeBits * (Strobe - LowCrsLimit)));
          MrcData->FaultyPart[Channel][Rank].Byte4to7 |= (UINT32) ((StrobeBitLanes << (StrobeBits * (Strobe - LowCrsLimit))) & (StrobeMask << (StrobeBits * (Strobe - LowCrsLimit))));
        }
      }
      if ((Strobe >= EccCsrLimit)) {
        StrobeBitLanes = ((Ecc_lane_err_stat >> (StrobeBits * (Strobe - EccCsrLimit))) & StrobeMask);
        StrobeBitLanes2 = (MrcData->FaultyPart[Channel][Rank].ByteEcc & StrobeMask);
        if (CountOnes (StrobeBitLanes) < CountOnes (StrobeBitLanes2)) {
          MrcData->FaultyPart[Channel][Rank].ByteEcc &= (UINT32) ~(StrobeMask << (StrobeBits * (Strobe - EccCsrLimit)));
          MrcData->FaultyPart[Channel][Rank].ByteEcc |= (UINT32) ((StrobeBitLanes << (StrobeBits * Strobe)) & (StrobeMask << (StrobeBits * Strobe)));
        }
      }
    } // for strobe
    //
    // If there is only single bit error on only one device and ECC is enabled (Setup options) 
    // and there is no error on the ECC bytelane then ignore this correctable error
    //
    if (((CountOnes (Cpgc_Err_Stat_Lo) + CountOnes(Cpgc_Err_Stat_Hi)) == 1) && (CountOnes (Ecc_lane_err_stat) == 0) 
          && (CountOnes (Bytegrp_err_stat) == 1) && MrcData->MrcParameters.EccEnabled[Channel]) {

      Bytegrp_err_stat = 0;
      if (ByteLanesErrStat != NULL) {
        *ByteLanesErrStat = (UINT8) Bytegrp_err_stat;
        *ByteLanesErrStat |= (Ecc_lane_err_stat << 8);
      }
    }
  }
  return MMRC_SUCCESS;
}

#endif // FAULTY_PART_TRACKING

VOID
GetMaxDq (
  IN         MMRC_DATA    *MrcData
  )
{
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 Rank;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        continue;
      }
      for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
        if (MrcData->MrcParameters.RankPresent[Channel][(2 * Dimm) + Rank] == 0) {
          continue;
        }
        switch (MrcData->D_DataWidth[Channel][Dimm]) {
        case 0:
          MrcData->MaxDq[Channel][(2 * Dimm) + Rank]  = MAX_STROBE_X4_DEVICES;
          MrcData->MaxBit[Channel][(2 * Dimm) + Rank] = MIN(MAX_BITS, STROBE_BITS_X4_DEVICES);
          break;
        default:
          MrcData->MaxDq[Channel][(2 * Dimm) + Rank]  = MAX_STROBE_X8_DEVICES;
          MrcData->MaxBit[Channel][(2 * Dimm) + Rank] = MAX_BITS;
          break;
        }
        MspDebugPrint((MSP_DBG_MIN, "Ch %d, Dimm %d, Rank %d, MaxDq: %d MaxBit: %d\n", Channel, Dimm, Rank, MrcData->MaxDq[Channel][(2 * Dimm) + Rank], MrcData->MaxBit[Channel][(2 * Dimm) + Rank]));
      } // Rank
    } // Dimm
  } // Channel
}


VOID
ForceMaxBit (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        MaxBit
  )
{
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 Rank;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        continue;
      }
      for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
        if (MrcData->MrcParameters.RankPresent[Channel][(2 * Dimm) + Rank] == 0) {
          continue;
        }
        MrcData->MaxBit[Channel][(2 * Dimm) + Rank] = MaxBit;
      } // Rank
    } // Dimm
  } // Channel
}


/**
  Set the Drams to enable Read Pream Training and MRP mode needed for DDR4 
  other DDR technologies can add in this fucntion their specific receive  
  enable Dram side (MRS operation) configurations

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Entry           Parameter to select Entry or Exit hook to execute.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReceiveEnableDramEntryExit (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT8             Entry
  )
{
#if RECEIVE_ENABLE == 1 && TRAINING_ALGOS == 1
#if DDR4_SUPPORT
  DramInitDDR4EMR4  Mrs4Command;
#endif
  BOOLEAN   ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;


  if (Entry == MMRC_ENTRY) {
    //
    // Set the Drams in read preamble training / MPR mode
    //
#if DDR4_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      PrechargeAll (MrcData, Channel, Rank);    
      ToggleMprMode (MrcData, Channel, Rank, Entry);
      Mrs4Command.Data = ReadMrsCommand (MrcData, Channel, Rank, 4);
      Mrs4Command.Bits.ReadPreamMode = 1;
      WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs4Command.Data);
      //
      // Open the page on the DRAM to have back2back reads since the first sample
      //
#if CPGC_BASED_READWRITES == 1
      CpgcExecuteParallel(MrcData, Channel, Rank);
#endif 
    }
#endif
  } else {
    //
    // Set the Drams out of read preamble training / MPR mode
    //
#if DDR4_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      Mrs4Command.Data = ReadMrsCommand (MrcData, Channel, Rank, 4);
      WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs4Command.Data);
      ToggleMprMode (MrcData, Channel, Rank, Entry);
    }
#endif
  }
  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
#endif

  return MMRC_SUCCESS;
}

MMRC_STATUS
VrefValueToSwitchVOC (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
  )
{
#if VOC_TRAINING == 1
  UINT8  Strobe;
  UINT32 TempValue;
  UINT8  Bit;
  UINT8  IndBit;
  UINT32 BitSwizzling[4] = {BIT2,BIT3,BIT0,BIT1};
  UINT32 BitNormal[4] = {BIT0,BIT1,BIT2,BIT3};
  UINT32 *BitArray;

  TempValue = 0;
  //
  // Read the WriteLvl Sample.
  // The bit is considered passed if the corresponding bit in the register is sampled as 0.
  //
  for (Strobe = 0; Strobe < NumberElements[Channel]; Strobe++) {
    if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    //
    // Check if this is for the VOC sweep, if so, then we need to set all the VOC Vrefs, where were not set in the create1dsweep
    // call.
    //
    if (MrcData->VocSweep == 1) {
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVocVal0, CMD_GET_REG, &TempValue);
      if (TempValue != 0) {
        for (Bit = 1; Bit < MrcData->MaxBit[Channel][Rank]; Bit++) {
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVocVal0+Bit, CMD_SET_VAL_FC_UC, &TempValue);
        }
      }
    }
  }
  for (Strobe = 0; Strobe < NumberElements[Channel]; Strobe++) {
    if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    MspDelay (NANO_DEL, 1000);
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVocSmp, CMD_GET_REG, &TempValue);
    if (MrcData->VocSweep == 1) {
      TempValue = TempValue ^ 0xF;
    }
    
    if (((Strobe >= 4) && (Strobe <= 7)) || ((Strobe >= 13) && (Strobe <= 16))) {
      BitArray = BitSwizzling;
    } else {
      BitArray = BitNormal;
    }
    for (Bit = 0; Bit < MrcData->MaxBit[Channel][Rank]; Bit++) {
      if ((TempValue & BitArray[Bit]) != 0) {
        PassFail[Channel][0][Strobe][Bit][CURRENT_SAMPLE] = RESULTS_FAIL;
        if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_SPREAD_BIT_RESULT) {
          for (IndBit = 0; IndBit < MrcData->MaxBit[Channel][Rank]; IndBit++) {
            PassFail[Channel][0][Strobe][IndBit][CURRENT_SAMPLE] = RESULTS_FAIL;
          }
          break;
        }
      } else {
        PassFail[Channel][0][Strobe][Bit][CURRENT_SAMPLE] = RESULTS_PASS;
      }
    }
  }
#endif // VOC_TRAINING
  return MMRC_SUCCESS;
}

UINT16
SearchSubtaskName (
  IN            UINT16    CurrentTaskOffset
)
{
  UINT16 CapsuleStrIndex;
  UINT16 k;

  //
  // Search for index in capsuleStrings
  //
  CapsuleStrIndex = (UINT16) -1;
  for (k = 0; k < MAX_SUBTASKS; k++) {
    if (CurrentTaskOffset == SubtaskOffsetIndex[k].FncOffset) {
      CapsuleStrIndex = SubtaskOffsetIndex[k].StrIndex;
      break;
    }
  }
  if (CapsuleStrIndex != (UINT16) -1) {
    return CapsuleStrIndex;
  }

  MspDebugPrint ((MSP_DBG_TRACE, "ERROR"));
  return 0;

}

MMRC_STATUS
TestCycle (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  volatile UINT8 ii = 1;
  UINT32 Cycle = 0;

    EnableMrcBreakpoint();
    MmrcExecuteTask(MrcData, BREAKPOINTMRC, 0, Channel);
    DisableMrcBreakpoint();
  while (ii) {
    MspDebugPrint ((MSP_DBG_MAX, "Test Cycle = %04d\n", Cycle));
    Cycle ++;
    MrcData->MspData.DebugMsgLevel = 0;
    EnableMrcBreakpoint();
    MmrcExecuteTask(MrcData, BREAKPOINTMRC, 0, Channel);
    DisableMrcBreakpoint();
    ReInitializeFunction(MrcData, 0, 0, Channel);
    MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN | MSP_DBG_MED | MSP_DBG_MAX;
    if (Channel == 0)
      MmrcExecuteTask(MrcData, PerfLatencyDefaultsCh0, 0, Channel);
    else
      MmrcExecuteTask(MrcData, PerfLatencyDefaultsCh1, 0, Channel);
    ReceiveEnable(MrcData, 0, 0, Channel);
    MrcData->MspData.DebugMsgLevel &= ~MSP_DBG_MAX;
    EarlyMprRead(MrcData, 0, 0, Channel);
    MrcData->MspData.DebugMsgLevel |= MSP_DBG_MAX;
    DnvNibbleTrainingHook(MrcData, 0, 0, Channel);
    FineWriteLeveling(MrcData, 0, 0, Channel);
    CoarseWriteLeveling(MrcData, 0, 0, Channel);
    DnvNibbleTrainingExitHook(MrcData, 0, 0, Channel);
  }
  return MMRC_SUCCESS;
}
/**
  ReInitializeFunction Function

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex
  @param[in]       StringIndex
  @param[in]       Channel
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReInitializeFunction (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  BOOLEAN   ExecuteBackup[MAX_CHANNELS];
  if (( MrcData->D_Type[Channel][0] !=  SPD_DDR_RDIMM) && (MrcData->D_Type[Channel][1] !=  SPD_DDR_RDIMM) && (MrcData->MrcParameters.BootMode == S0)) {
    MspDebugPrint ((MSP_DBG_MED, "ReInitializeFunction Skipped \n"));
    return MMRC_SUCCESS;
  }
  MspDebugPrint ((MSP_DBG_MAX, "ReInitializeFunction\n"));

  ExecuteBackup[0] = MrcData->MspData.ExecuteOnThisChannel[0];
  ExecuteBackup[1] = MrcData->MspData.ExecuteOnThisChannel[1];

  MrcData->MspData.ExecuteOnThisChannel[0] = MrcData->MrcParameters.ChEnabled[0];
  MrcData->MspData.ExecuteOnThisChannel[1] = MrcData->MrcParameters.ChEnabled[1];
  MrcData->ExecuteTxVrefSetup = FALSE;
  ReInitializeDramsAcrossChannels (MrcData);
  MrcData->ExecuteTxVrefSetup = TRUE;
  MrcData->MspData.ExecuteOnThisChannel[0] = ExecuteBackup[0];
  MrcData->MspData.ExecuteOnThisChannel[1] = ExecuteBackup[1];

  return MMRC_SUCCESS;
}

/**
  SetRdimmRegCkeLow Function Write RCW RC0A and RC0B  RCW RC3x throug SMbus

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SetRdimmRegCkeLow(
IN        MMRC_DATA   *MrcData,
IN        UINT8        Channel
)
{
  MMRC_STATUS    Status;
  UINT8     CurrentDimm;
  UINT8     Buffer;
  UINT8     Length;
  UINT8     Rank;

  Length = VF_SC_BYTE_LEN;

  //
  // Now examine each DIMM.
  //
  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    if ((MrcData->MspData.DynamicVars[Channel][DIMMTYPE] != SPD_DDR_RDIMM) || (!MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm])) continue;
    MspDebugPrint((MSP_DBG_MAX, "RDIMM RCW RC0A and RC3x with CKE low %s(ch:%x) Dimm:%x MemDn=%d\n", __FUNCTION__, Channel, CurrentDimm, MrcData->OemMrcData.EnableMemoryDown[Channel][CurrentDimm]));
    Status = MMRC_SUCCESS;

    //
    // Write RCW RC0A and RC0B throug SMbus
    //
    Rank = (CurrentDimm * 2);
    Buffer = ((MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][26]] >> 8) & 0xF);
    //
    //External VrefCA  RC0B
    //
    Buffer |= 0x80;
    Status = MMRC_SUCCESS;
    if (!MrcData->OemMrcData.EnableMemoryDown[Channel][CurrentDimm]) {
      Status = MrcSmbusExec(
        MrcData->SmbusBar,
        (MrcData->OemMrcData.SpdAddressTable[Channel][CurrentDimm] + 0x10),
        SmbusWriteByte,
        0xD,
        &Length,
        &Buffer
        );
    }

    MspDebugPrint((MSP_DBG_MAX, "C%d.D%d: Offset RC0A and RC0B 0xD = 0x%X\n", Channel, CurrentDimm, Buffer));
    //
    // Write RCW RC3x throug SMbus
    //
    Buffer = ((MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][33]] >> 8) & 0xFF);
    Status = MMRC_SUCCESS;
    if (!MrcData->OemMrcData.EnableMemoryDown[Channel][CurrentDimm]) {
      Status = MrcSmbusExec(
        MrcData->SmbusBar,
        (MrcData->OemMrcData.SpdAddressTable[Channel][CurrentDimm] + 0x10),
        SmbusWriteByte,
        0x12,
        &Length,
        &Buffer
        );
    }

    MspDebugPrint((MSP_DBG_MAX, "C%d.D%d: Offset RC3x 0x12 = 0x%X\n", Channel, CurrentDimm, Buffer));
    MspDebugPrint((MSP_DBG_MAX, "\n"));
  }

  return MMRC_SUCCESS;
}


/**
  JedecInit Function

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex
  @param[in]       StringIndex
  @param[in]       Channel
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
JedecInitFunction (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  UINT8   Rank;
  //
  // ODT OFF has to be low when doing the MRS
  //
  MmrcExecuteTask(MrcData, FORCEODT_OFF, SearchSubtaskName(FORCEODT_OFF), Channel);
  if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE]== SPD_DDR_RDIMM) {
    //
    // Send the Register Control Setup
    // It will be executed only for RDIMM & DDR4 (PCFT - Internally)
    //
    MmrcExecuteTask(MrcData, DoRegisterInit, SearchSubtaskName(DoRegisterInit), Channel);
    //
    // Wait 500us before assert CKE
    //
    MspDelay(NANO_DEL, 500000);
    //
    // CKE has to be High when doing the RC9 and RCD
    //
    MmrcExecuteTask(MrcData, RELEASE_CKE, SearchSubtaskName(RELEASE_CKE), Channel);
    //
    // Small delay
    //
    MspDelay(NANO_DEL, 100);
    //
    // Send the RC - CKE
    // It will be executed only for RDIMM & DDR4 (PCFT - Internally)
    //
    MmrcExecuteTask(MrcData, DoRegisterInitCke, SearchSubtaskName(DoRegisterInitCke), Channel);
    //
    // PRECHARGEALL the existing ranks
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_TRACE, "R%d PRE_A\n", Rank));
      PrechargeAll (MrcData, Channel, Rank);
    }
  } else { // MrcData->MspData.DynamicVars[Channel][DIMMTYPE]== SPD_DDR_RDIMM
    //
    // CKE has to be High when doing the MRS commands
    //
    MmrcExecuteTask(MrcData, RELEASE_CKE, SearchSubtaskName(RELEASE_CKE), Channel);
  }
  //
  // NOP Commands to All Ranks
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!MrcData->RankEnabled[Channel][Rank]) {
      continue;
    }

    MspDebugPrint ((MSP_DBG_TRACE, "R%d NOP\n", Rank));
    NopCommands (MrcData, Channel, Rank);
  }
  //
  // Send MRS commands
  //
  MmrcExecuteTask(MrcData, JedecInit, SearchSubtaskName(JedecInit), Channel);
  //
  // PRECHARGEALL the existing ranks
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!MrcData->RankEnabled[Channel][Rank]) {
      continue;
    }
    MspDebugPrint ((MSP_DBG_TRACE, "R%d PRE_A\n", Rank));
    PrechargeAll (MrcData, Channel, Rank);
  }
  //
  // Zq Cal Long
  //
  DoZqCal (MrcData, Channel);
  //
  // ODT Release
  //
  MmrcExecuteTask(MrcData, FORCEODT_REL, SearchSubtaskName(FORCEODT_REL), Channel);
  return MMRC_SUCCESS;
}

MMRC_STATUS
VocTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
#if VOC_TRAINING == 1
  UINT8   Bit;
  UINT8   Strobe;
  UINT32  TempValue;
  UINT16  Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT16  Low[MAX_CHANNELS][MAX_STROBES];
  UINT16  High[MAX_CHANNELS][MAX_STROBES];
  UINT16  Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT8   DebugLevel;
  UINT8   i;
  UINT8   IndCh;
  UINT8   FirstAvailableRank[MAX_CHANNELS];
  UINT8   FirstAvailableChannel;
  UINT8   NumberElements[MAX_CHANNELS] = {0};
  UINT8   MaxDqCache[MAX_CHANNELS] = {0};
  UINT8   MaxBitCache[MAX_CHANNELS] = {0};
  BOOLEAN BackupEnable;
  UINT8   MaxThresholdBackup;
  UINT16  VrefPerVoc[16][MAX_STROBES][MAX_BITS];
  UINT16  VocPerVref[VOC_VREF_MAX + 1][MAX_STROBES][MAX_BITS];
  UINT16  CurrentVoc;
  UINT16  TargetVref[MAX_STROBES];
  UINT16  Voc[MAX_STROBES][MAX_BITS];
  UINT16  MinVref, MaxVref;

  if (MrcData->SkipTrainingAlgos[LRxVoc]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

  BackupEnable  = MrcData->MspData.Enabled[0];
  MaxThresholdBackup = MrcData->MaxThresholdPerBit;

  MrcData->SkipTrainingAlgos[LRxVoc] = TRUE;
  MrcData->MaxThresholdPerBit = MAX_THRESHOLD_PER_BIT_VOC;
  DebugLevel = MrcData->MspData.DebugMsgLevel;

  for (i = 0; i < MAX_CHANNELS; i++) {
    NumberElements[i] = MAX_STROBE_X4_DEVICES;
  }

  FirstAvailableRank[0] = 0xFF;
  FirstAvailableRank[1] = 0xFF;
  FirstAvailableChannel = 0xFF;
  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    for (i = 0; i < MAX_RANKS; i++) {
      if (MrcData->RankEnabled[IndCh][i] && MrcData->MrcParameters.ChEnabled[IndCh]) {
        if (FirstAvailableRank[0] == 0xFF && FirstAvailableRank[1] == 0xFF) {
          FirstAvailableChannel = IndCh;
        }
        if (FirstAvailableRank[IndCh] == 0xFF) {
          FirstAvailableRank[IndCh] = i;
        }
      }
    }
  }

  MspDebugPrint ((MSP_DBG_MIN, "R[0] = %d R[1] = %d C = %d\n", FirstAvailableRank[0], FirstAvailableRank[1], FirstAvailableChannel));

  if ((FirstAvailableRank[0] == 0xFF && FirstAvailableRank[1] == 0xFF) || FirstAvailableChannel == 0xFF) {
    MrcData->MaxThresholdPerBit = MaxThresholdBackup;
    return MMRC_SUCCESS;
  }

  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (FirstAvailableRank[IndCh] == 0xFF) {
      continue;
    }
    MaxDqCache[IndCh]  = MrcData->MaxDq[IndCh][FirstAvailableRank[IndCh]];
    MaxBitCache[IndCh] = MrcData->MaxBit[IndCh][FirstAvailableRank[IndCh]];
    MrcData->MaxDq[IndCh][FirstAvailableRank[IndCh]]   = MAX_STROBE_X4_DEVICES;
    MrcData->MaxBit[IndCh][FirstAvailableRank[IndCh]]  = 4;
  }

  if (!MrcData->RestorePath) {
    //
    // Save the initial values
    //
    for (Channel = FirstAvailableChannel; Channel < MAX_CHANNELS; Channel++) {
      if (!MrcData->MrcParameters.ChEnabled[Channel] || FirstAvailableRank[Channel] == 0xFF) {
        continue;
      }
      MmrcExecuteTask (MrcData, VOC_TRAINING_ENTRY, SearchSubtaskName (VOC_TRAINING_ENTRY), Channel);
    }
    if (FirstAvailableChannel == 1) {
      MrcData->MspData.Enabled[0] = TRUE;
    }

    MmrcExecuteTask (MrcData, VOC_TRAINING_ENTRY_COMMON, SearchSubtaskName (VOC_TRAINING_ENTRY_COMMON), 0);

    if (FirstAvailableChannel == 1) {
      MrcData->MspData.Enabled[0] = BackupEnable;
    }
    //
    // VOC init
    //
    for (Channel = FirstAvailableChannel; Channel < MAX_CHANNELS; Channel++) {
      if (!MrcData->MrcParameters.ChEnabled[Channel] || FirstAvailableRank[Channel] == 0xFF) {
        continue;
      }
      MmrcExecuteTask (MrcData, VOC_PHY_INIT, SearchSubtaskName (VOC_PHY_INIT), Channel);
    }
    if (FirstAvailableChannel == 1) {
      MrcData->MspData.Enabled[0] = TRUE;
    }
    MmrcExecuteTask (MrcData, VOC_PHY_INIT_COMMON, SearchSubtaskName (VOC_PHY_INIT_COMMON), 0);
    if (FirstAvailableChannel == 1) {
      MrcData->MspData.Enabled[0] = BackupEnable;
    }
    for (Channel = FirstAvailableChannel; Channel < MAX_CHANNELS; Channel++) {
      MrcData->MspData.ExecuteOnThisChannel[0] = FALSE;
      MrcData->MspData.ExecuteOnThisChannel[1] = FALSE;
      if (!MrcData->MrcParameters.ChEnabled[Channel] || FirstAvailableRank[Channel] == 0xFF) {
        continue;
      }
      MrcData->MspData.ExecuteOnThisChannel[Channel] = TRUE;

      MspDebugPrint ((MSP_DBG_MIN, "C%dRxVocEn = 1\n", Channel));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        //
        // Enable VOC
        //
        TempValue = 1;
        GetSetDdrioGroup2 (MrcData, Channel, FirstAvailableRank[Channel], Strobe, RxVocEnDq, CMD_SET_VAL_FC_UC, &TempValue);
      }

      //
      // Print out the results for the pull ups.
      //
      MrcData->MspData.DynamicVars[1][PU_CODE_GRP2] = MrcData->MspData.DynamicVars[0][PU_CODE_GRP2];
      MrcData->MspData.DynamicVars[1][PD_CODE_GRP2] = MrcData->MspData.DynamicVars[0][PD_CODE_GRP2];
      MspDebugPrint ((MSP_DBG_MIN, "Pull Up   = %d\n", (UINT32) MrcData->MspData.DynamicVars[Channel][PU_CODE_GRP2]));
      MspDebugPrint ((MSP_DBG_MIN, "Pull Down = %d\n", (UINT32) MrcData->MspData.DynamicVars[Channel][PD_CODE_GRP2]));
      //
      // Step 9.  Enable WrLVL mode for the host only.
      //
      MmrcExecuteTask (MrcData, VOC_ENABLE_WLMODE, SearchSubtaskName (VOC_ENABLE_WLMODE), Channel);

      //
      // Init the VocPerVref and TargetVref with invalid values.
      //
      for (i = 0; i <= VOC_VREF_MAX; i++) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
            VocPerVref[i][Strobe][Bit] = 0xFF;
            Voc[Strobe][Bit] = 0xFF;
          }
        }
      }
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        TargetVref[Strobe] = 0xFF;
      }
      MinVref = VOC_VREF_MAX;
      MaxVref = 0;

      //
      // Sweep the Voc and get the Vref for each Voc
      // Instead of sweeping all the possible values of Voc, just sweep the negative voltages,
      // this will reduce the time of the Voc training algorithm, and also avoid getting errors
      // when trying to get Vref for high Voc values (i.e. not finding the switching Vref ).
      //
      MspDebugPrint ((MSP_DBG_MIN, "VREF\n"));
      for (CurrentVoc = 0; CurrentVoc <= VOC_ZERO_PHYSICAL; CurrentVoc++) {
        MspDebugPrint ((MSP_DBG_MIN, "  Set RxVoc = %d\n", CurrentVoc));
        TempValue = CurrentVoc;
        //
        // Set Voc to Current Voc
        //
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          for (Bit = 0 ; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
            TempValue = CurrentVoc;
            GetSetDdrioGroup (MrcData, 0, Channel, 0, FirstAvailableRank[Channel], Strobe, 0, MrcData->DVFSFreqIndex, 0xFF, RxVocVal0 + Bit, CMD_SET_VAL_FC_UC, &TempValue);
          }
        }

        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          Dim1StartPoint[Channel][0][Strobe][LOW]  = 0;
          Dim1StartPoint[Channel][0][Strobe][HIGH] = VOC_VREF_MAX;
          Low[Channel][Strobe]                     = 0;
          High[Channel][Strobe]                    = VOC_VREF_MAX - 1;
        }
        MrcData->VocSweep = 0;
        Create1DSweep (
          MrcData,
          Channel,
          FirstAvailableRank[Channel],
          RxVref,
          1,
          Dim1StartPoint,
          Low, High,
          VOC_VREF_STEP_SIZE,
          FALSE, FALSE, TRUE,
          NumberElements,
          VrefValueToSwitchVOC,
          Results, "S", 3, RUN_PER_STROBE_BIT_LOCK | RUN_RESULT_BIT0_INDEPENDENT
        );
        //
        // Save results in Vref and Voc tables
        //
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
            continue;
          }
          MspDebugPrint ((MSP_DBG_MIN, "  SL%02d", Strobe));
          for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
            VrefPerVoc[CurrentVoc][Strobe][Bit] = Results[Channel][0][Strobe][Bit][LOW];
            if (VrefPerVoc[CurrentVoc][Strobe][Bit] > VOC_VREF_MAX) {
              MspDebugPrint ((MSP_DBG_MIN, ":--"));
              continue;
            } else {
              VocPerVref[VrefPerVoc[CurrentVoc][Strobe][Bit]][Strobe][Bit] = CurrentVoc;
              MspDebugPrint ((MSP_DBG_MIN, ":%02d", VrefPerVoc[CurrentVoc][Strobe][Bit]));
            }
            //
            // Determine the common Vref for all bits.
            // This is determined by the minimim Vref obtained when setting RxVoc = 0(Physical)
            //
            if ((CurrentVoc == 0) && (TargetVref[Strobe%9] < VrefPerVoc[CurrentVoc][Strobe][Bit] || TargetVref[Strobe%9]==0xFF)) {
              TargetVref[Strobe%9] = VrefPerVoc[CurrentVoc][Strobe][Bit];
            }
            if (MinVref > VrefPerVoc[CurrentVoc][Strobe][Bit]) {
              MinVref =  VrefPerVoc[CurrentVoc][Strobe][Bit];
            }
            if (MaxVref < VrefPerVoc[CurrentVoc][Strobe][Bit]) {
              MaxVref =  VrefPerVoc[CurrentVoc][Strobe][Bit];
            }
          }
          if (((Strobe%MAX_STROBES == 7) && (Strobe < (MrcData->MaxDq[Channel][0] - 1))) || IsEccStrobe(MrcData, Channel, Strobe))
            MspDebugPrint ((MSP_DBG_MIN, "\n"));
          else
            MspDebugPrint ((MSP_DBG_MIN, "  "));
        }
      }
      MspDebugPrint ((MSP_DBG_MIN, "TargetVref   "));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        MspDebugPrint ((MSP_DBG_MIN, "%d=%02d ", Strobe, TargetVref[Strobe%9]));
      }
      MspDebugPrint ((MSP_DBG_MIN, "\n"));
      //
      // Zero out Great then 2.
      //
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
          continue;
        }
        for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
          for (i = (UINT8)MinVref; i <= (UINT8)MaxVref; i++) {
            if ((VocPerVref[i][Strobe][Bit] >= 5) && (VocPerVref[i][Strobe][Bit] <=9)) {
              VocPerVref[i][Strobe][Bit] = 0xFF;
            }
          }
        }
      }
      //
      // Print the Vref table
      //
      MspDebugPrint ((MSP_DBG_MIN, "VREF table\n"));
      for (i = (UINT8)MinVref; i <= (UINT8)MaxVref; i++) {
        MspDebugPrint ((MSP_DBG_MIN, "  VREF%02d: ", i));
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
            continue;
          }
          for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
            TempValue = VocPerVref[i][Strobe][Bit];
            if (TempValue == 0xFF)
              MspDebugPrint ((MSP_DBG_MIN, "--:"));
            else
              MspDebugPrint ((MSP_DBG_MIN, "%02d:", TempValue));
          }
          MspDebugPrint ((MSP_DBG_MIN, "  "));
        }
        MspDebugPrint ((MSP_DBG_MIN, "\n"));
      }
      //
      // Find the Voc for the TargetVref
      //
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
          continue;
        }
        for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
          if(VocPerVref[TargetVref[Strobe%9]][Strobe][Bit] != 0xFF) {
            Voc[Strobe][Bit] = VocPerVref[TargetVref[Strobe%9]][Strobe][Bit];
          } else {
            for (i = 1; (i <= TargetVref[Strobe%9] - MinVref) && (i <= MaxVref - TargetVref[Strobe%9]); i++) {
              if (VocPerVref[TargetVref[Strobe%9] + i][Strobe][Bit] != 0xFF) {
                Voc[Strobe][Bit] = VocPerVref[TargetVref[Strobe%9] + i][Strobe][Bit];
                break;
              } else if (VocPerVref[TargetVref[Strobe%9] - i][Strobe][Bit] != 0xFF) {
                Voc[Strobe][Bit] = VocPerVref[TargetVref[Strobe%9] - i][Strobe][Bit];
                break;
              }
            }
            if (Voc[Strobe][Bit] == 0xFF) {
              MspDebugPrint ((MSP_DBG_MIN, "Error: No VOC was found on S%02dB%02d. Setting to 0\n", Strobe, Bit));
              Voc[Strobe][Bit] = 0;
            }
          }
        }
      }
      //
      // Based on the results, program the VOC final values.
      //
      MspDebugPrint ((MSP_DBG_MIN, "VOC  \n"));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MIN, "  SL%02d", Strobe));
        for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
          TempValue   = Voc[Strobe][Bit];
          GetSetDdrioGroup2 (MrcData, Channel, FirstAvailableRank[Channel], Strobe, RxVocVal0 + Bit, CMD_SET_VAL_FC_UC, &TempValue);
          MspDebugPrint ((MSP_DBG_MIN, ":%02d", TempValue));
        }
        if (((Strobe % MAX_STROBE_X8_DEVICES == 7) && (Strobe < (MrcData->MaxDq[Channel][FirstAvailableRank[Channel]] -1))) || IsEccStrobe(MrcData, Channel, Strobe))
          MspDebugPrint ((MSP_DBG_MIN, "\n"));
        else
          MspDebugPrint ((MSP_DBG_MIN, "  "));
      }
      MspDebugPrint ((MSP_DBG_MIN, "\n"));
      //
      // Get the Vref values for the programmed Voc values.
      // The values obtained here should be close to the TargetVref value.
      //
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        Dim1StartPoint[Channel][0][Strobe][LOW]  = 0;
        Dim1StartPoint[Channel][0][Strobe][HIGH] = VOC_VREF_MAX;
        Low[Channel][Strobe]                     = 0;
        High[Channel][Strobe]                    = VOC_VREF_MAX - 1;
      }
      MrcData->VocSweep = 0;
      Create1DSweep (
        MrcData,
        Channel,
        FirstAvailableRank[Channel],
        RxVref,
        1,
        Dim1StartPoint,
        Low, High,
        VOC_VREF_STEP_SIZE,
        FALSE, FALSE, TRUE,
        NumberElements,
        VrefValueToSwitchVOC,
        Results, "S", 3, RUN_PER_STROBE_BIT_LOCK | RUN_RESULT_BIT0_INDEPENDENT
      );
      //
      // Print the Vref results
      //
      MspDebugPrint ((MSP_DBG_MIN, "VREF  \n"));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MIN, "  SL%02d", Strobe));
        for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
          MspDebugPrint ((MSP_DBG_MIN, ":%02d", Results[Channel][0][Strobe][Bit][LOW]));
        }
        if (((Strobe%MAX_STROBES == 7) && (Strobe < (MrcData->MaxDq[Channel][0] - 1))) || IsEccStrobe(MrcData, Channel, Strobe))
          MspDebugPrint ((MSP_DBG_MIN, "\n"));
        else
          MspDebugPrint ((MSP_DBG_MIN, "  "));
      }

      //
      // Step 10.  Get out of Host WRL mode.
      //
      MmrcExecuteTask (MrcData, VOC_DISABLE_WLMODE, SearchSubtaskName (VOC_DISABLE_WLMODE), Channel);
    } // Channel loop...
    for (Channel = FirstAvailableChannel; Channel < MAX_CHANNELS; Channel++) {
      if (!MrcData->MrcParameters.ChEnabled[Channel] || FirstAvailableRank[Channel] == 0xFF) {
        continue;
      }
      MmrcExecuteTask (MrcData, VOC_TRAINING_EXIT, SearchSubtaskName (VOC_TRAINING_EXIT), Channel);
    }
    if (FirstAvailableChannel == 1) {
      MrcData->MspData.Enabled[0] = TRUE;
    }
    MmrcExecuteTask (MrcData, VOC_TRAINING_EXIT_COMMON, SearchSubtaskName (VOC_TRAINING_EXIT_COMMON), 0);
    if (FirstAvailableChannel == 1) {
      MrcData->MspData.Enabled[0] = BackupEnable;
    }
  } else { // !MrcData->RestorePath
    for (Channel = FirstAvailableChannel; Channel < MAX_CHANNELS; Channel++) {
      MrcData->MspData.ExecuteOnThisChannel[0] = FALSE;
      MrcData->MspData.ExecuteOnThisChannel[1] = FALSE;
      if (!MrcData->MrcParameters.ChEnabled[Channel] || FirstAvailableRank[Channel] == 0xFF) {
        continue;
      }
      MrcData->MspData.ExecuteOnThisChannel[Channel] = TRUE;
      //
      // Set all DQ VOC Offset to 0.
      //
      MspDebugPrint ((MSP_DBG_MIN, "C%dRxVocEn = 1\n", Channel));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        //
        // Enable VOC
        //
        TempValue = 1;
        GetSetDdrioGroup2 (MrcData, Channel, FirstAvailableRank[Channel], Strobe, RxVocEnDq, CMD_SET_VAL_FC_UC, &TempValue);
      }

      MspDebugPrint ((MSP_DBG_MIN, "VOC\n"));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, FirstAvailableRank[Channel], Strobe, FALSE, NULL)) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MIN, "  SL%02d", Strobe));
        for (Bit = 0; Bit < MrcData->MaxBit[Channel][FirstAvailableRank[Channel]]; Bit++) {
          GetSetDdrioGroup2 (MrcData, Channel, FirstAvailableRank[Channel], Strobe, RxVocVal0 + Bit, CMD_GET_CACHE, &TempValue);
          GetSetDdrioGroup2 (MrcData, Channel, FirstAvailableRank[Channel], Strobe, RxVocVal0 + Bit, CMD_SET_VAL_FC_UC, &TempValue);
          MspDebugPrint ((MSP_DBG_MIN, ":%02d",TempValue));
        }
        if (((Strobe % MAX_STROBE_X8_DEVICES == 7) && (Strobe < (MrcData->MaxDq[Channel][FirstAvailableRank[Channel]] -1))) || IsEccStrobe(MrcData, Channel, Strobe))
          MspDebugPrint ((MSP_DBG_MIN, "\n"));
        else
          MspDebugPrint ((MSP_DBG_MIN, "  "));
      }
      MspDebugPrint ((MSP_DBG_MIN, "\n"));
    } // Channel loop ...
  } // MrcData->RestorePath
  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (FirstAvailableRank[IndCh] == 0xFF) {
      continue;
    }
    MrcData->MaxDq[IndCh][FirstAvailableRank[IndCh]]  = MaxDqCache[IndCh] ;
    MrcData->MaxBit[IndCh][FirstAvailableRank[IndCh]] = MaxBitCache[IndCh];
  }
  MrcData->MspData.DebugMsgLevel = DebugLevel;
  MrcData->MaxThresholdPerBit    = MaxThresholdBackup;
#endif // VOC_TRAINING
  return MMRC_SUCCESS;
}


/**
  Performs the Receive Enable calibration.  All results from the calibration
  are stored within the MrcData within the TrainingData substructure.  
  Channel is the only additional parameter passed in, and the calibration will 
  be performed on the channel specified.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReceiveEnable (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if RECEIVE_ENABLE == 1 && TRAINING_ALGOS == 1
  UINT8             Rank;                       // Rank being tested.
  UINT8             Strobe;                     // Strobe being tested.
  UINT32            TempValue;                  // Temporary storage element used throughout the algorithm.
  UINT32            HalfClock[MAX_CHANNELS] = {0};    // Contains the number of UIs per Half clock.
  UINT8             NumberElements[MAX_CHANNELS];
  UINT8             InitialDelay;               // Initial delay to start the receive enable training.
  UINT8             SmallStep;                  // Small step size.
  UINT16            Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT16            Low[MAX_CHANNELS][MAX_STROBES];
  UINT16            High[MAX_CHANNELS][MAX_STROBES];
  UINT16            Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT8             ChannelBackup;
  UINT8             FirstValidChannel;
  UINT8             DebugLevel;
  UINT8             DitherVal;
  BOOLEAN           DecreaseRdCmd2RcvEn = FALSE;
//  UINT32            Temp;
#if SIM
  InitialDelay  = RCVN_INITIAL2XVAL_SIM;
  SmallStep     = RCVN_SMALL_STEP_SIM;
  DitherVal     = 1;
#else
  InitialDelay  = RCVN_INITIAL2XVAL;
  SmallStep     = RCVN_SMALL_STEP;
  DitherVal     = 4;
#endif

  if (MrcData->SkipTrainingAlgos[LReceiveEnable]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

  DebugLevel        = MrcData->MspData.DebugMsgLevel;
  ChannelBackup     = Channel;
  FirstValidChannel = 0xFF;
  //
  // Turn on 1D sweep debug messages if MAX message level is set.
  //
  if ((MrcData->MspData.DebugMsgLevel & MSP_DBG_MAX) == MSP_DBG_MAX) {
    MrcData->MspData.DebugMsgLevel |= MSP_DBG_DEBUG;
  }
  //
  // Receive Enable Sample
  //
  MrcData->SignalType = RecEnSmp;
  //
  // ReceiveEnable Ddrio phy entry hook
  //
#ifdef RECEIVE_ENABLE_ENTRY
  MmrcExecuteTask (MrcData, RECEIVE_ENABLE_ENTRY, NO_PRINT, Channel);
#endif
  if (!MrcData->RestorePath) {
#if CPGC_BASED_READWRITES == 1
    MrcData->CpgcOptions.LoopCount = 1;
    MrcData->CpgcOptions.NumBursts = 0;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_EARLYTRAINING);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
#endif
    do {
      DecreaseRdCmd2RcvEn = FALSE;
      //
      // Loop through all enabled ranks performing the rcvn enable training.
      //
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (MrcData->MspData.ExecuteThisRoutineInParallel) {
          MspDebugPrint ((MSP_DBG_MAX, "CxxR%d\n", Rank));
        } else {
          MspDebugPrint ((MSP_DBG_MAX, "C%02dR%d\n", Channel, Rank));
        }
        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          HalfClock[Channel]      = GetHalfClk (MrcData, Channel);
          NumberElements[Channel] = MrcData->MaxDq[Channel][Rank];
          if (FirstValidChannel == 0xFF) {
            FirstValidChannel = Channel;
          }
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }

#if CPGC_BASED_READWRITES == 1
          //
          // Setup the CPGC engine to to do a single read from an address within the
          // selectable rank.  The engine should be setup for LFSR mode.
          //
          MrcData->CpgcOptions.Rank = Rank;
          MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_READ;
          CpgcReconfig (MrcData, Channel);
#endif
          //
          // ReceiveEnable Dram entry hook
          //
          ReceiveEnableDramEntryExit (MrcData, Channel, Rank, MMRC_ENTRY);
          //
          // Set the minus1 and also set the initial, min, and max RecEnDelay values. Do a FIFO reset.
          //
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
#if RCVN_MINUS1_SUPPORT
            TempValue = 1;
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RecEnMinus1, CMD_SET_VAL_FC_UC, &TempValue);
#endif
            MmrcExecuteTask (MrcData, FIFO_RESET_ENABLE2, NO_PRINT, Channel);
            Dim1StartPoint[Channel][0][Strobe][LOW]  = (UINT16) (HalfClock[Channel] * InitialDelay);
            Dim1StartPoint[Channel][0][Strobe][HIGH] = Dim1StartPoint[Channel][0][Strobe][LOW] + (UINT16) (HalfClock[Channel] * HIGH_SIDE_START_RCV_EN);
            Low[Channel][Strobe]                     = 0;
            High[Channel][Strobe]                    = (UINT16)(Dim1StartPoint[Channel][0][Strobe][LOW] + (HalfClock[Channel] * (HIGH_SIDE_START_RCV_EN - 1)));
          } // Strobe loop ...
        } // Channel loop ...

        //
        // Restore back the Channel Under Test
        //
        Channel = ChannelBackup;
        //
        // Find a rising edge in the receive enable pulse. This will be returned in Results[Channel][Strobe][Bit][LOW].
        //
        Create1DSweep (MrcData, Channel, Rank, RecEnDelay, 1, Dim1StartPoint, Low, High, SmallStep,
          FALSE, FALSE, FALSE, NumberElements, SampleDqs, Results, "S", DitherVal, RUN_PER_STROBE
          );

        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          //
          // Small Fine RdLeveling sweep to characterize and center in the high pulse
          //
#if RCVN_FINECHARACTERIZATION
          //
          // This needs to be rewritten to use the Results array from above since they contain the pulsewidth already.
          //
          //RdLevelFineHighPulseCharacterizeAndCenter (MrcData, Channel, Rank, "SFneC");
#else
          //
          // If in simulation, we don't want to characterize the pulse width because we know it's
          // 64 ticks. So just add 32 to get to the center and save time.
          //
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            TempValue = Results[Channel][0][Strobe][0][LOW] + (HalfClock[Channel] / 2);
            Dim1StartPoint[Channel][0][Strobe][LOW]  = (UINT16) TempValue;
            Dim1StartPoint[Channel][0][Strobe][HIGH] = (UINT16) TempValue;
            //
            // We don't care about the high side, so set the max to the starting point. This way we won't sweep the high side.
            //
            High[Channel][Strobe] = (UINT16) TempValue;
          }
#endif
        } // Channel loop ...
        //
        // Restore back the Channel Under Test
        //
        Channel = ChannelBackup;
        //
        // Step backwards by 1x clocks until the preamble is found.
        //
        if(FirstValidChannel >= MAX_CHANNELS){
          MrcData->ErrorCode = MrcErrParameterOutOfBound;
          MrcDeadLoop();
          return MMRC_FAILURE;
        }
        Create1DSweep (MrcData, Channel, Rank, RecEnDelay, 1, Dim1StartPoint, Low, High, (UINT16) (HalfClock[FirstValidChannel] * 2),
          FALSE, FALSE, FALSE, NumberElements, SampleDqs, Results, "S", 1, RUN_PER_STROBE
          );

        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            TempValue = Results[Channel][0][Strobe][0][LOW];
            //
            // If LOW value is small (less than 1x clock + 32 + 4), suggest to decrease the RDCMD2RCVEN value
            //
            if (TempValue < HalfClock[FirstValidChannel] * 2 + 36) {
              DecreaseRdCmd2RcvEn = TRUE;
              MspDebugPrint ((MSP_DBG_MIN, "SL%02d: LOW value is less than 1x clock + 36: %d\n", Strobe, TempValue));
            }
            Dim1StartPoint[Channel][0][Strobe][LOW]  = (UINT16) TempValue;
            Dim1StartPoint[Channel][0][Strobe][HIGH] = (UINT16) TempValue;
            //
            // We don't care about the high side, so set the max to the starting point. This way we won't sweep the high side.
            //
            High[Channel][Strobe] = (UINT16) TempValue;
          }
        } // Channel loop ...
        //
        // Restore back the Channel Under Test
        //
        Channel = ChannelBackup;
        //
        // Check if RDCMD2RCVEN needs to be decreased. If so, try to decrease it and start over.
        // If RDCMD3RCVEN cannot be decreased, try to continue with the training.
        //
        if (DecreaseRdCmd2RcvEn == TRUE) {
          MmrcExecuteTask (MrcData, GET_RDCMD2RCVEN, NO_PRINT, Channel);
          if ((MrcData->MspData.DynamicVars[Channel][N0_RDCMD2RCVEN] > 0) && (MrcData->MspData.DynamicVars[Channel][N1_RDCMD2RCVEN] > 0)) {
            MrcData->MspData.DynamicVars[Channel][N0_RDCMD2RCVEN]--;
            MrcData->MspData.DynamicVars[Channel][N1_RDCMD2RCVEN]--;
            MmrcExecuteTask (MrcData, SET_RDCMD2RCVEN, NO_PRINT, Channel);
            MspDebugPrint ((MSP_DBG_MIN, "Restart ReveiceEnable with reduced Read Command to Receive Enable:\n"));
            MspDebugPrint ((MSP_DBG_MIN, "N0_RDCMD2RCVEN: %d\n", MrcData->MspData.DynamicVars[Channel][N0_RDCMD2RCVEN]));
            MspDebugPrint ((MSP_DBG_MIN, "N1_RDCMD2RCVEN: %d\n", MrcData->MspData.DynamicVars[Channel][N1_RDCMD2RCVEN]));
            break;
          } else {
            MspDebugPrint ((MSP_DBG_MIN, "Failed to decrease RDCMD2RCVEN:\n"));
            DecreaseRdCmd2RcvEn = FALSE;
          }
        }
        //
        // Walk backward to find the first rising edge after the preamble.
        //
        Create1DSweep (MrcData, Channel, Rank, RecEnDelay, 1, Dim1StartPoint, Low, High, SmallStep,
          FALSE, FALSE, FALSE, NumberElements, SampleDqs, Results, "S", DitherVal, RUN_PER_STROBE
          );

        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          //
          // Subtract X CLK to each LANE to put the RCVEN at the center of a 0. X is a multiple
          // of 1/16 clocks and is defined on a per project basis in the spreadsheet.
          //
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            TempValue = Results[Channel][0][Strobe][0][LOW] - (RCVN_PREAM_CLK_SUBTRACT * HalfClock[Channel] / 8);
#if LPDDR3_SUPPORT
            //
            // Subtracting ~1000PS for more TDQSCK margin. 205 works for all frequencies.
            //
            TempValue -= 205;
#endif
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RecEnDelay, CMD_SET_VAL_FC_UC, &TempValue);
          }
          //
          // ReceiveEnable Dram exit hook
          //
          ReceiveEnableDramEntryExit (MrcData, Channel, Rank, MMRC_EXIT);

        } // Channel loop ...
        //
        // Restore back the Channel Under Test
        //
        Channel = ChannelBackup;
      } // Rank loop ...
    } while (DecreaseRdCmd2RcvEn == TRUE);

    //
    // Restore back the Channel Under Test
    //
//    Channel = ChannelBackup;
//    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
//      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
//       if (!RunOnThisChannel (MrcData, Channel, Rank)) {
//         continue;
//       }
//       for (Strobe = 0; Strobe < MAX_STROBE_X8_DEVICES; Strobe ++) {
//         if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
//           continue;
//         }
//         GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RecEnDelay, CMD_GET_REG_UC, &Temp);
//         GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, RecEnDelay, CMD_SET_VAL_FC_UC, &Temp);
//       }
//      }
//    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;

    AverageGroups (MrcData, Channel, RecEnDelay, TRUE);

#if RCVN_MINUS1_SUPPORT == 1
    SetMinus1Select (MrcData, Channel, RecEnDelay, RecEnMinus1);
#endif // RCVN_MINUS1_SUPPORT
  } //Restore Path
  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResults (MrcData, Channel, RecEnDelay, FALSE);
  //
  // ReceiveEnable DDRIO Phy Exit hook
  //
#ifdef RECEIVE_ENABLE_EXIT
  MmrcExecuteTask (MrcData, RECEIVE_ENABLE_EXIT, NO_PRINT, Channel);
#endif
  ReceiveEnableExit (MrcData, Channel);
  MrcData->MspData.DebugMsgLevel = DebugLevel;

  if (TestMrcTrainingSteps (MrcData, ChannelBackup, RCV_ENABLE_CHECKER, 0, 0, 0, 0) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#endif // RECEIVE_ENABLE == 1
  return MMRC_SUCCESS;
}

#if (RECEIVE_ENABLE == 1 || FINE_WRITE_LEVELING == 1) && TRAINING_ALGOS == 1
MMRC_STATUS
SampleDqs (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
  )
{
  UINT8         Strobe;               // Strobe Index for currently active.
  UINT32        TempValue;            // Place holder for the read results, not used just required for the read.
  UINT8         Sample;               // Result of reading the Sample register for phase alignment.
  UINT8         SampleIndex;          // Loop Counter for performing the total number of samples.
  INT8          Results[MAX_CHANNELS][MAX_STROBES]; // Internal signed accumulation of the results, this will Count
                                      // +1 when a 1 is sampled, -1 when a 0 is sampled.
  UINT32        FinishBL[MAX_CHANNELS];// Holds whether a specific strobe has completed the total required samples.
  UINT32        TotalFinishStrobes;   // Strobe signature when all strobes have passed the required number of samples.
  UINT8         SampleThreshold;      // Number of samples that need to equal a common value before that value is chosen.
  UINT8         ChannelBackup;

  ChannelBackup = Channel;
  //
  // Initialize variables.
  //
  TotalFinishStrobes = (1 << MrcData->MaxDq[Channel][Rank]) - 1;
  TempValue = 0;
  MmrcMemset (FinishBL, 0, MAX_CHANNELS * sizeof (UINT32));
  //
  // Zero out the signed accumulated results before starting the sampling intervals.
  //
  MmrcMemset (Results, 0, MAX_CHANNELS * MAX_STROBES * sizeof (INT8));
  //
  // Set the sample index to the maximum number of samples required.  This is a project specific compile switch.
  //
#if SIM
  SampleIndex     = NUMSAMPLES_SIM;
  SampleThreshold = SAMPLETHRESH_SIM;
#else
  SampleIndex     = NUMSAMPLES;
  SampleThreshold = SAMPLETHRESH;
#endif
  //
  // Loop on the total number of samples, but sampling may stop earlier if threshold of 1's or 0's is reached.
  //
  while (SampleIndex--) {
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    //
    // Perform the required memory access.
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL_CLEAR, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL_CHECK, NO_PRINT, Channel);

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      //
      // Go through each strobe lane, checking if the threshold has been reached.
      //
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        //
        // If the strobe lane has hit its threshold, then go to the next strobe lane.
        //
        if (FinishBL[Channel] & (1 << Strobe) ) {
          continue;
        }
        //
        // Read the sample register.
        //
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, MrcData->SignalType, CMD_GET_REG, &TempValue);
        Sample = (UINT8) TempValue;
        //
        // Accumulate the results for that strobe lane, by adding a 1 if a 1 is sample, subtracting a 1 if a 0 is sampled.
        //
        Results[Channel][Strobe] += ((Sample * 2) - 1 );
        //
        // If the resultant count is above the threshold, then set the bit-field for the Strobe lane to a 1.
        //
        if ((Results[Channel][Strobe] >= SampleThreshold) || Results[Channel][Strobe] <= (SampleThreshold * -1)) {
          FinishBL[Channel] |= (1 << Strobe);
        }
      } // Strobe loop ...
      //
      // If all strobe lanes have hit the threshold, then break out of the loop early.
      //
      if (FinishBL[Channel] == TotalFinishStrobes) {
        break;  //finished all bytelane, get all 1 or 0
      }
    } // Channel loop ...
  }  // while (--SampleIndex)

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    //
    // The return results, should see if more 0's then return 0 otherwise return 1.. Zero implies take 1.
    //
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
      if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
        continue;
      }
      if (Results[Channel][Strobe] >= 0) {
        PassFail[Channel][0][Strobe][0][CURRENT_SAMPLE] = RESULTS_PASS;
      } else {
        PassFail[Channel][0][Strobe][0][CURRENT_SAMPLE] = RESULTS_FAIL;
      }
    }
  } // Channel loop ...

  return MMRC_SUCCESS;

}
#endif // (RECEIVE_ENABLE == 1 || FINE_WRITE_LEVELING == 1) && TRAINING_ALGOS == 1



/**
  Routine to indentify the right cache index array

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CacheIndex      Cache Index to be accessed, this pointer is gotten from  Elements['Type'] table.

  @retval          CacheArrayIndex
**/
UINT8
GetCacheIndexArray(
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT16        CacheIndex
)
{
  UINT8 index;

  for (index = 0; index < MaxCacheTypes; index++) {
    if ((CacheIndex & 0xC0) == CacheTypePrefix[index]) {
      break;
    }
  }

  if (index == MaxCacheTypes) {
    MrcData->ErrorCode = MrcErrInvalidCacheIndexMash;
    MrcDeadLoop ();
  }

  return index;
}


/**
  Routine to Get/Set the value to the right Cache array

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field.
  @param[in]       Bit             Bit within field being examined.
  @param[in]       CacheIndex      Cache Index to be accessed, this pointer is gotten from  Elements['Type'] table.
  @param[in]       Cmd             Command to read/write from cache (valid commands RD_ONLY and UPD_CACHE)
  @param[in]       Value           Value to be set/retrieved for the an specific Cache array.

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetSetCache (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         CacheIndex,
  IN        UINT8         Cmd,
  IN        UINT32        *Value
  )
{
  UINT8   CacheArrayIndex;

  CacheArrayIndex = GetCacheIndexArray(MrcData, CacheIndex);
  //
  // Remove the Bits 7 and 6, which is used to indicate the proper Cache Array Index.
  //
  CacheIndex &= ~0xC0;

  if (Cmd & RD_ONLY) {
    switch (CacheArrayIndex) {
    case TypeCache:
      *Value = MrcData->MrcParameters.SaveRestore.CachedValues[Channel][CacheIndex][Rank][Strobe];
      break;
    case TypeCachePB:
      *Value = MrcData->MrcParameters.SaveRestore.CachedValuesPB[Channel][CacheIndex][Rank][Strobe][Bit];
      break;
    case TypeCacheNonSR:
      *Value = MrcData->CachedValuesNonSR[Channel][CacheIndex][Rank][Strobe];
      break;
    case TypeCacheNonSRPB:
      *Value = MrcData->CachedValuesNonSRPB[Channel][CacheIndex][Rank][Strobe][Bit];
      break;
    default:
      MrcData->ErrorCode = MrcErrInvalidCacheIndexMash;
      MrcDeadLoop ();
      break;
    }
  } else if (Cmd & UPD_CACHE) {
    switch (CacheArrayIndex) {
    case TypeCache:
      MrcData->MrcParameters.SaveRestore.CachedValues[Channel][CacheIndex][Rank][Strobe] = *Value;
      break;
    case TypeCachePB:
      MrcData->MrcParameters.SaveRestore.CachedValuesPB[Channel][CacheIndex][Rank][Strobe][Bit] = *Value;
      break;
    case TypeCacheNonSR:
      MrcData->CachedValuesNonSR[Channel][CacheIndex][Rank][Strobe] = *Value;
      break;
    case TypeCacheNonSRPB:
      MrcData->CachedValuesNonSRPB[Channel][CacheIndex][Rank][Strobe][Bit] = *Value;
      break;
    default:
      MrcData->ErrorCode = MrcErrInvalidCacheIndexMash;
      MrcDeadLoop ();
      break;
    }
  } else {
    MrcData->ErrorCode = MrcErrInvalidCmdForCache;
    MrcDeadLoop ();
  }

  return MMRC_SUCCESS;
}

/**
  Routine to save size which doesn't include all the parameters
  of the real GetSetDdrioGroup. Use this routine for a single level memory
  configuration or when there is only a single level of memory per channel.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field.
  @param[in]       Type            Register to be accessed, this is pointer to
                                   a register or algorithm in the Elements[] table.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in]       Value           Value set/retrieved for the specified Type.
  @retval          MMRC_SUCCESS
  @retval          STATUS_TYPE_NOT_SUPPORTED
  @retval          STATUS_LIMIT
**/
MMRC_STATUS
GetSetDdrioGroup2 (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN        UINT32       *Value
  )
{
  if (((Strobe >= 4) && (Strobe <= 7)) || ((Strobe >= 13) && (Strobe <= 16))) {
    //
    // Per Bit Swizzling relationship
    // Log - Phy
    // B0 - B2
    // B1 - B3
    // B2 - B0
    // B3 - B1
    //
    if (Type == RxVocVal0) {
      Type = RxVocVal2;
    } else if (Type == RxVocVal1){
      Type = RxVocVal3;
    } else if (Type == RxVocVal2){
      Type = RxVocVal0;
    } else if (Type == RxVocVal3){
      Type = RxVocVal1;
    }
  }
  return GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, 0, MrcData->DVFSFreqIndex, 0xFF, Type, Cmd, Value);
}

/**
  Routine to save size which doesn't include all the parameters
  of the real GetSetDdrioGroup. Use this routine for a single level memory 
  configuration or when there is only a single level of memory per channel.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[in]       FrequencyIndex  Frequency Index.
  @param[in]       IoLevel         I/O Level.
  @param[in]       Element         Register to be accessed, this is pointer to 
                                   a register or algorithm in the Elements[] table.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in]       Value           Value set/retrieved for the specified Type.
  @retval          MMRC_SUCCESS
  @retval          STATUS_TYPE_NOT_SUPPORTED
  @retval          STATUS_LIMIT
**/
MMRC_STATUS
GetSetDdrioGroup (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT16        Element,
  IN        UINT8         Cmd,
  IN        UINT32       *Value
  )
{
#if USE_GET_SET_FROM_SPREADSHEET
  UINT8                     StartingBit;
  UINT8                     EndingBit;
  UINT8                     PhysicalChannel;
  UINT8                     PhysicalStrobe;
  MMRC_STATUS               Status;
  UINT32                    MaxLimit;
  UINT32                    WaitTime;
  UINT8                     Instance;
  EXTERNAL_GETSET_FUNCTION *j;
  REGISTER_ACCESS           Register;
  UINT8                     i;
  UINT16                    ElementIndex;
  UINT32                    CurrentValue;
  UINT32                    NewValue;
  UINT8                     ThisGroup;
  UINT8                     TempBlueprint;
  UINT16                    Type;
  UINT32                    CacheValue;
  MSP_DATA                  *MspData;

  WaitTime    = 0;
  CacheValue  = 0;
  TempBlueprint = MrcData->MspData.CurrentBlueprint;  

  MspData = GetMspHostStructureAddress ();

  //
  // Given a PFCT and IoLevel, we can select the correct blueprint for this IO. If the IoLevel is passed
  // in as 0xFF, then we ignore it and just return the first blueprint that matches the PFCT for this 
  // channel. This is useful in systems that have one level of memory per channel and also permits
  // the space efficient use of GetSetDdrioGroup2() which forces IoLevel to 0xFF.
  //
  Status = ConvertPfctAndIoLevelToBlueprint (MrcData, Channel, IoLevel, &MrcData->MspData.CurrentBlueprint);

  Type = GetOffsetIndex (MrcData, Element);

  if (Status != MMRC_SUCCESS) {
    MrcData->MspData.CurrentBlueprint = TempBlueprint;  
    MspDebugPrint ((MSP_DBG_MIN, "\nError: IO Level %d not supported for %s on Blueprint %d.\n", 
      IoLevel, SignalInfo[GetSignalInfoIndex(Type)].Name, MrcData->MspData.CurrentBlueprint
      ));
    return MrcIoLevelNotSupported;
  }

  if (Type == REG_UNDEFINED) {
    MrcData->MspData.CurrentBlueprint = TempBlueprint;  
    return STATUS_TYPE_NOT_SUPPORTED;
  } else if (Type == DELAY_TYPE_UNDEFINED) {
    return STATUS_TYPE_NOT_SUPPORTED;
  }
  // 
  // Map the passed channel and strobe to the physical channel and strobe using the global floorplan.
  //
  PhysicalChannel = FloorPlan[Channel][Strobe].Channel;
  PhysicalStrobe  = FloorPlan[Channel][Strobe].Strobelane;
  
  j = ExternalFunctions;
  // 
  // Check if the register is a physical register request or an algorithm linear value request.
  // This is specifically checking if its a physical register request.
  //
  if (Type < ALGO_REG_INDEX) {
    //
    // Check to make sure the frequency index passed in is less than the total number
    // of frequency indices listed for this element. If it is, adjust by the frequency
    // index passed in. If FrequencyIndex is 0, no adjustment occurs and we access
    // the first element.
    //
    if (FrequencyIndex < SignalInfo[GetSignalInfoIndex (Type)].FrequencyOffset) {
      if (Type < (NumberOfElementsPerAlgo * NumberAlgos)) {
        Type += (NumberOfElementsPerAlgo * FrequencyIndex);
      } else {
        Type += FrequencyIndex;
      }
    } else {
      MrcData->MspData.CurrentBlueprint = TempBlueprint;  
      MspDebugPrint ((
        MSP_DBG_MIN, 
        "\nError: %s not available for freq %d on Blueprint %d.\n", 
        SignalInfo[GetSignalInfoIndex(Type)].Name, 
        FrequencyIndex, 
        MrcData->MspData.CurrentBlueprint
        ));
      return MrcFrequencyIndexNotSupported;
    }
    //
    // If the register being requested does not have a location offset, return MMRC_FAILURE, 
    // this is to support the linear calculation which has
    //
    if (Elements[Type].RegisterLocation[Rank].Offset == MMRC_NA) {
      MrcData->MspData.CurrentBlueprint = TempBlueprint;  
      return STATUS_TYPE_NOT_SUPPORTED;  
    }
    if ((Elements[Type].AccessAttributes.Bits.GetSetExternal == 1) && (Elements[Type].Box_Type == EXTERNAL)) {
      j += Elements[Type].RegisterLocation[Rank].Offset;
      (*(j->Function)) (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, Cmd, Value);
      return MMRC_SUCCESS;
    }
    //
    // If a cache read, then read directly from the trained values array.
    //
    if ((Cmd & RD_ONLY) != 0 && (Cmd & RD_REG) == 0) {
      GetSetCache (MrcData, Channel, Rank, Strobe, Bit, Elements[Type].CacheIndex, RD_ONLY, Value);
      if (Elements[Type].Box_Type != EXTERNAL) {
        *Value &= ((((UINTX)1 << (Elements[Type].RegisterLocation[Rank].EndingBit + 1)) - 1) - 
                   (((UINTX)1 << Elements[Type].RegisterLocation[Rank].StartingBit) - 1));
        *Value >>= Elements[Type].RegisterLocation[Rank].StartingBit;
      }
      *Value = ConvertPhysicalToLinearValue(MrcData, Channel, Element, *Value);
      MrcData->MspData.CurrentBlueprint = TempBlueprint;  
      return MMRC_SUCCESS;
    }
    //
    // If the number of instances per channel > 1, then Instance = Strobelane/2; otherwise, instance = 0.
    //
    if (InstancePortMap[Elements[Type].Box_Type].Instances > 1) {
      Instance = PhysicalStrobe / MAX_BYTELANES_PER_DQ_MODULE;
    } else {
      Instance = 0;
    }
    //
    // Get the actual byte/start/ending bit for the element based on channel/rank/strobe.
    //
    Register.Offset = 0;
    GetLocation (MrcData, Elements[Type], PhysicalChannel, Rank, PhysicalStrobe, Bit, &Instance, &Register.Offset, &StartingBit, &EndingBit);
    NewValue = *Value;
    //
    // If the command is a read, then read the register, masking the bits, and shifting the bits
    // so that the value is shown starting at bit 0.
    //
    if (Cmd & RD_REG) {
      if (Elements[Type].Box_Type == EXTERNAL) {
        j += Elements[Type].RegisterLocation[Rank].Offset;
        (*(j->Function)) (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, RD_REG, Value);
        j = ExternalFunctions;
      } else {
        Register.Mask = (((UINTX)1 << (EndingBit + 1)) - 1) - (((UINTX)1 << StartingBit) - 1);
        Register.ShiftBit = StartingBit;
        *Value = (UINT32) MemFieldRead (Elements[Type].Box_Type, PhysicalChannel, Instance, Register);
      }
      //
      // Convert back to linear because this function only operates on linear values.
      //
      *Value = ConvertPhysicalToLinearValue(MrcData, Channel, Element, *Value);
      NewValue = *Value;
    }
    //
    // If the command is a write, compute the mask, shift the value to the appropriate bits,
    // and read/modify/write.
    if (Cmd & FC_WR) {
      //
      // If the value passed in is an offset from the current value, read the current value first, then
      // apply the offset.
      //
      if (Cmd & WR_OFF) {
        //
        // Read the current value of the register field.
        //
        GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, Element, RD_ONLY, &CurrentValue);
        NewValue = CurrentValue + *Value;
      }
      //
      // If it is a write, bounds check the Value first.
      //
      Status = GetDdrioGroupLimits (MrcData, Socket, Channel, Dimm, IoLevel, Type, &MaxLimit, &WaitTime);
      if ((Status != MMRC_SUCCESS) || (NewValue > MaxLimit) ) {
        MspDebugPrint ((
          MSP_DBG_MIN, 
          "0x%x > limit of 0x%x for %s. C%d%R%dS%dB%d\n",
          NewValue, 
          MaxLimit, 
          SignalInfo[GetSignalInfoIndex (Type)].Name,
          Channel, Rank, Strobe, Bit
          ));
        //
        // At least for now, just deadloop to ease debugging.  MRC spec says to "return STATUS_LIMIT" instead.
        //
        MrcData->MspData.CurrentBlueprint = TempBlueprint;  
        return MrcSetLimitReached;
      }
      //
      // Convert to physical in preparation for writing the register.
      //
      NewValue = ConvertLinearToPhysicalValue(MrcData, Channel, Element, NewValue);

      if (Elements[Type].Box_Type == EXTERNAL) {
        j += Elements[Type].RegisterLocation[Rank].Offset;
        (*(j->Function)) (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, FC_WR, &NewValue);
      } else {
        Register.Mask = (((UINTX)1 << (EndingBit + 1)) - 1) - (((UINTX)1 << StartingBit) - 1);
        Register.ShiftBit = StartingBit;
        MemRegWrite (Elements[Type].Box_Type, PhysicalChannel, Instance, Register, NewValue, 0xf);
      }
      //
      // If the action has a delay, with the set, then delay between every assignment.
      //
      if (WaitTime != 0) {
#if SIM
        MySimStall (WaitTime);
#else
        MspDelay (NANO_DEL, WaitTime);
#endif
      }
      ///////////////MemRegWrite (DDRIO, PhysicalChannel, ByteOffset, TempValue);
    }
    //
    // If a cache write, then write directly to the trained values array.
    //
    if (Cmd & UPD_CACHE) {
      if (Cmd & RD_REG) {
        //
        // If a RD_REG command happened the NewValue will contain a Linear Value,
        // before storing back to CACHE convert to physical.
        //
        NewValue = ConvertLinearToPhysicalValue (MrcData, Channel, Element, NewValue);
      }
      //
      // Update the cache.
      //
      if (Elements[Type].Box_Type != EXTERNAL) {
        GetSetCache (MrcData, Channel, Rank, Strobe, Bit, Elements[Type].CacheIndex, RD_ONLY, &CacheValue);
        CacheValue &= ~(
                        ((((UINTX)1 << (Elements[Type].RegisterLocation[Rank].EndingBit + 1)) - 1) -
                        (((UINTX)1 << Elements[Type].RegisterLocation[Rank].StartingBit) - 1))
                       );
        CacheValue |= (NewValue << Elements[Type].RegisterLocation[Rank].StartingBit);
        GetSetCache (MrcData, Channel, Rank, Strobe, Bit, Elements[Type].CacheIndex, UPD_CACHE, &CacheValue);
      } else {
        GetSetCache (MrcData, Channel, Rank, Strobe, Bit, Elements[Type].CacheIndex, UPD_CACHE, &NewValue);
      }
    }
  } else if (Type >= SGTI_INDEX) {
    //
    // This is a group signal so need to loop to perform the action on all elements in the group.
    //
    ElementIndex = Type - SGTI_INDEX;
    ThisGroup = 0;
    for (i = 0; i < SignalInfo[ElementIndex].NumSignalGroups; i++)
    {
      GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, 
        Type - (SGTI_INDEX - ALGO_REG_INDEX) + ThisGroup, Cmd, Value
        );
      ThisGroup += SignalInfo[ElementIndex].FrequencyOffset;
    }
  } else {
    NewValue = *Value;
    // 
    // When accessing the algorithm,  do a recursive call back to the Get/Set but for the individual elements.
    // For a force write, decode the value into the registers.
    //
    if (Cmd & FC_WR) {
      //
      // If the value passed in is an offset from the current value, read the current value first, then
      // apply the offset.
      //
      if (Cmd & WR_OFF) {
        GetLinearFromRegs (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, Type, RD_ONLY, &CurrentValue);
        NewValue = CurrentValue + NewValue;
      }
      //
      // Now that we have computed the value with the offset, write the total value to the register
      // by disabling the WR_OFF flag.
      //
      SetRegsToLinear (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, Type, Cmd & ~WR_OFF, &NewValue); 
    }
    //
    // If a read, then read the registers (only the elements, not cc) and return the linear value.
    //
    if (Cmd & RD_REG) {
      GetLinearFromRegs (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, Type, Cmd, Value);
      NewValue = *Value;
    }
    //
    // Make sure the frequency index passed in is valid.
    //
    if (FrequencyIndex < SignalInfo[Type - ALGO_REG_INDEX].FrequencyOffset) {
      //
      // If a cache read, then read directly from the trained values array.
      //
      if ((Cmd & RD_ONLY) != 0 && (Cmd & RD_REG) == 0) {
        *Value = MrcData->MrcParameters.SaveRestore.Trained_Value[Channel].Values[MrcData->MspData.CurrentBlueprint][(Type - ALGO_REG_INDEX) + FrequencyIndex][Rank][Strobe];
      }
      // 
      // If a cache write, then write directly to the trained values array.
      //
      if (Cmd & UPD_CACHE) {
        MrcData->MrcParameters.SaveRestore.Trained_Value[Channel].Values[MrcData->MspData.CurrentBlueprint][(Type - ALGO_REG_INDEX) + FrequencyIndex][Rank][Strobe] = (UINT16) NewValue;
      }
    } else {
      MrcData->MspData.CurrentBlueprint = TempBlueprint;  
      return MrcFrequencyIndexNotSupported;
    }
  }
#endif
  MrcData->MspData.CurrentBlueprint = TempBlueprint;  
  return MMRC_SUCCESS;
}

/**
  Based on the linear delay element specified, it will set the appropriate 
  delay and clock crossing elements to form the linear delay.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[in]       FrequencyIndex  Frequency Index.
  @param[in]       IoLevel         I/O Level.
  @param[in]       Type            Register to be accessed, 
                                   this is pointer to an algorithm.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in, out]  Value           Value set/retrieved for the
                                   specified Type.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
SetRegsToLinear (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value 
  )
{  
  UINT8  ElementIndex;          // Index to the Delay/CC being operated upon.
  UINT32 CurrentValue;          // Remaining Linear value to be programmed.
  UINT32 TempValue;             // Temporary variable used throughout the function.
  UINT32 ElementValue;          // Value to be programmed into the specific element.
  UINT16 AlgoElementsStart;     // Starting position for the Delay/CC within Elements[].
  UINT16 CCValue; 
  UINT16 CCRange;
  UINT8  ThisCcRange;
  //
  // Initialize variable.
  //
  ElementValue  = 0;
  CCValue       = 0;
  CCRange       = 0;
  //
  // Compute the starting index within the Element[] array for the elements used to assign the linear value. 
  //
  AlgoElementsStart = ((Type - ALGO_REG_INDEX)) * NumberOfElementsPerAlgo;
  //
  // CurrentValue will contain the remaining linear delay needed to write to the elements.  Initially it
  // should always be the assigned value.
  //
  CurrentValue = *Value;
  
  for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
    // 
    // For all delay elements, except the PI, the actual value programmed must be based on the Granularity of the element.  
    // The PI value, which is the last element of the assignment (MAX_NUM_DELAY_ELEMENTS - 1) always has a granularity of 1.
    // ElementValue = Actual element to program.
    // TempValue is the linear value of the element programming, for example 2x=2, TempValue = 128...
    //
    ElementValue = CurrentValue / (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]);
    TempValue = ElementValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]);  
    //
    // We cannot set any delay larger than the 2x clock. If the delay element is larger granularity, skip it.
    //
    if (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex] > GetHalfClk (MrcData, Channel))
    {
      continue;
    }
    //
    // Attempt to program the element, if successful the subtract the linear value from the total to get 
    // the remainder.  
    //
    if (GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, (UINT8)(AlgoElementsStart+ElementIndex), Cmd, &ElementValue) == MMRC_SUCCESS) {
      if (ElementIndex == NumberOfDelayElementsPerAlgo - NumberOfDependantElements) {
        CCValue = (UINT16) CurrentValue;
        CCRange = GetHalfClk (MrcData, Channel);
      }     
      CurrentValue -= TempValue;   
    }
  } // ElementIndex loop ...
  
  //
  // Set the starting index to the beginning of the Clock Crossings.
  //
  AlgoElementsStart += NumberOfDelayElementsPerAlgo;

  for (ElementIndex = 0; ElementIndex < NumberOfCCElements; ElementIndex++) {
    TempValue = 0;
    //
    // Check if the value of the PI was within the min/max percentage for that clock crossing.  If it
    // is then set the CC to the "invalue"
    //
    for (ThisCcRange = 0; ThisCcRange < NumberOfCCRanges; ThisCcRange++) {
      if ((ElementIndex == ClockCrossings[ThisCcRange].CcNumber) &&
        (CCValue <  (UINT32) (CCRange * ClockCrossings[ThisCcRange].MaxPercent / 100)) && 
        (CCValue >= (UINT32) (CCRange * ClockCrossings[ThisCcRange].MinPercent / 100))) {
        TempValue = (UINT32) ClockCrossings[ThisCcRange].InValue; 
        break;
      }
    }
    //
    // Program the value to the Clock Crossing.
    //
    if (ClockCrossings[ThisCcRange].IsAbsolute) {
      GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, (UINT8)(AlgoElementsStart + ElementIndex), Cmd, &TempValue);
    } else {
      //
      // If IsAbsolute is FALSE, we need to add the value of the CC to the value in the register.
      //
      GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, (UINT8)(AlgoElementsStart + ElementIndex), Cmd | WR_OFF, &TempValue);
    }
  }

  return MMRC_SUCCESS;
}

/**
   Based on the linear delay element specified, compute the
   linear delay.  The linear delay should include the support
   for analog/digital dll.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[in]       FrequencyIndex  Frequency Index.
  @param[in]       IoLevel         I/O Level.
  @param[in]       Type            Register to be accessed, this is pointer to 
                                   an algorithm.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[out]      Value           Value set/retrieved.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
GetLinearFromRegs (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
      OUT   UINT32        *Value 
  )
{  
#if USE_GET_SET_FROM_SPREADSHEET
  UINT8  ElementIndex;        // Index to the Delay/CC being operated upon.
  UINT32 TempValue;           // Temporary variable used throughout the function.
  UINT16 AlgoElementsStart;   // Starting position for the Delay/CC within Elements[].
  UINT16 CCValue;
  UINT16 CCRange;
  UINT8  ThisCcRange;

  //
  // Initialize the value to 0, this is the value that will get returned.
  //
  *Value = 0;
  CCValue = 0;
  TempValue = 0;
  //
  // Compute the starting index within the Element[] array for the elements used to assign the linear value. 
  //
  AlgoElementsStart = (Type - ALGO_REG_INDEX) * NumberOfElementsPerAlgo;

  for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
    //
    // We cannot set any delay larger than the 2x clock. If the delay element is larger granularity, skip it.
    //
    if (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex] > GetHalfClk (MrcData, Channel)) {
      continue;
    }
    // 
    // Attempt to read the delay element, if successful accumulate to the value variable.
    // PI element will be incremented by the read value * 1, where all the other elements will
    // be multiplied by the granularity.
    //
    if (GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, IoLevel, AlgoElementsStart + ElementIndex, Cmd, &TempValue) == MMRC_SUCCESS) {
      *Value += TempValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]);
    }
    if (ElementIndex >= NumberOfDelayElementsPerAlgo - NumberOfDependantElements) {
      CCValue += (UINT16) TempValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]);
    }     
  } // ElementIndex loop ...
  
  CCRange = GetHalfClk (MrcData, Channel);
  //
  // Set to the first clock crossing.
  //
  AlgoElementsStart += NumberOfDelayElementsPerAlgo; 

  for (ElementIndex = 0; ElementIndex < NumberOfCCElements; ElementIndex++) {
    //
    // If this clock crossing is modifying a delay register (such as the 2x clock), then we need to
    // undo the clock crossing modification when returning the total linear value. For example, if 2x clock
    // the 7 and the clock crossing modifies this to be 6, then we need to return it back to 7 when 
    // calculating the final linear value. GranularityValid == 1 is the flag for this condition.
    //
    if (Elements[AlgoElementsStart + ElementIndex].ElementAttributes.Bits.GranularityValid == 1) {
      //
      // Check if the value of the PI was within the min/max percentage for that clock crossing.  If it
      // is then set the CC to the "invalue"
      //
      for (ThisCcRange = 0; ThisCcRange < NumberOfCCRanges; ThisCcRange++) {
        if ((ElementIndex == ClockCrossings[ThisCcRange].CcNumber) &&
          (CCValue <  (UINT32) (CCRange * ClockCrossings[ThisCcRange].MaxPercent / 100)) && 
          (CCValue >= (UINT32) (CCRange * ClockCrossings[ThisCcRange].MinPercent / 100))) {
            TempValue = (UINT32) ClockCrossings[ThisCcRange].InValue; 
            break;
        }
      }
      *Value -= TempValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][Elements[AlgoElementsStart + ElementIndex].ElementAttributes.Bits.GranularityValue]);
    }
  } // ElementIndex loop ...
#endif
  return MMRC_SUCCESS;
}

/**
  Based on the Type input, returns the maximum possible value and the wait time
  required after, if necessary, chaining the value.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.   
  @param[in]       Level           I/O Level.
  @param[in]       Type            Register to be accessed, this is pointer to 
                                   an algorithm.
  @param[out]      MaxLimit        Maximum value allowed on this signal group.
  @param[out]      WaitTime        Amount of time in uSec to wait after programming 
                                   a new value.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetDdrioGroupLimits (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8          Socket,
  IN        UINT8          Channel,
  IN        UINT8          Dimm,
  IN        UINT8          Level,
  IN        UINT16         Type,
      OUT   UINT32        *MaxLimit,
      OUT   UINT32        *WaitTime
  )
{
#if USE_GET_SET_FROM_SPREADSHEET
  UINT8   ElementIndex;        // Index to the Delay/CC being operated upon.
  UINT32  TempLimit;           // Temporary variable used throughout the function.
  UINT16  AlgoElementsStart;   // Starting position for the Delay/CC within Elements[].

  if (Type < MAX_NUM_ELEMENTS) {
    if (Elements[Type].CacheIndex != 0xFF) {
      *WaitTime = Elements[Type].WaitTime;
      *MaxLimit = Elements[Type].MaxLimit;
    } else {
      *WaitTime = 0xFFFFFFFF;
      *MaxLimit = 0xFFFFFFFF;
    }
  } else {
    //
    // Initialize the values to 0, these are the values that will get returned.
    //
    *WaitTime = 0;
    *MaxLimit = 0;

    //
    // Compute the starting index within the Elements[] array for the elements used to assign the linear value.
    //
    AlgoElementsStart = (Type - ALGO_REG_INDEX) * NumberOfElementsPerAlgo;

    for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
      //
      // Wait time is straight addition.  Limits are computed with:
      // PI element will be incremented its limit * 1, where all the other elements will
      // be multiplied by the granularity.
      //
      if (Elements[AlgoElementsStart + ElementIndex].CacheIndex != 0xFF) {
        *WaitTime += Elements[AlgoElementsStart + ElementIndex].WaitTime;
        TempLimit  = Elements[AlgoElementsStart + ElementIndex].MaxLimit;
        *MaxLimit += TempLimit * Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex];
      }
    } // ElementIndex loop ...
  } // if (Type < MAX_NUM_ELEMENTS)
#endif
  return MMRC_SUCCESS;
}

/**
   Retrieves the Byteoffset, starting, and ending bit for a given channel, rank and strobe.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Element         Information on the description and flags of the desired field.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[out]      Instance        Instance being specified when looping on total Instances.
  @param[out]      ByteOffset      Final ByteOffset based on Channel/Rank/Strobe.
  @param[out]      StartingBit     Final Starting bit for the field based on Channel/Rank/Strobe.
  @param[out]      EndingBit       Final Ending bit for the field based on Channel/Rank/Strobe.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetLocation (
  IN  OUT MMRC_DATA         *MrcData,
  IN      GET_SET_ELEMENTS  Element,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT8             Strobe,
  IN      UINT8             Bit,
  OUT     UINT8             *Instance,
  OUT     UINT32            *ByteOffset,
  OUT     UINT8             *StartingBit,
  OUT     UINT8             *EndingBit
  )
{
  MSP_DATA    *MspData;

  MspData = GetMspHostStructureAddress ();
  //
  // Calculate Final ByteOffset, Starting bit, and Ending bit. If RankToRegister or StrobeToRegister tables are not to be used.
  //
  if (Element.ElementAttributes.Bits.StrobeLookupTable == 0 && Element.ElementAttributes.Bits.RankLookupTable == 0) {
    //
    // The initial ByteOffset, starting, and ending bits are taken from the Element[] array except
    // when the unique bit is set, which it will be taken from the URank array but only for ranks above 0.
    //
    *ByteOffset += (Element.RegisterLocation[Rank].Offset     + Element.SPMByteOffset * (Strobe % MAX_BYTELANES_PER_DQ_MODULE));
    *StartingBit = Element.RegisterLocation[Rank].StartingBit + Element.SPMBitOffset  * (Strobe % MAX_BYTELANES_PER_DQ_MODULE);
    *EndingBit   = Element.RegisterLocation[Rank].EndingBit   + Element.SPMBitOffset  * (Strobe % MAX_BYTELANES_PER_DQ_MODULE);
    //
    // Per-Nibble Handling. 
    //  Strobes above (MAX_STROBES / 2) are nibbles.
    //
    if (MAX_STROBES > 9 && (Strobe >= (MAX_STROBES / 2))) {
      //
      // Add Nibble Byte/Bit Offsets
      //
      *ByteOffset  += Element.NbSPMByteOffset;
      *StartingBit += Element.NbSPMBitOffset;
      *EndingBit   += Element.NbSPMBitOffset;
      //
      // If the number of instances per channel > 1, then Instance = Strobelane/2; otherwise, instance = 0.
      //
      if (InstancePortMap[Element.Box_Type].Instances > 1) {
        // 
        // Recalculate the Instance based on how many nibbles are handled withing a DQ module.
        //
        *Instance = Strobe - (MAX_STROBES / 2);
      }
    } 
  } else {
    //
    // Use StrobeToRegister and RankToRegister Tables to get RegisterOffset and BoxType Offset.
    //
    *ByteOffset += Element.RegisterLocation[Rank].Offset;
    *StartingBit = Element.RegisterLocation[Rank].StartingBit;
    *EndingBit   = Element.RegisterLocation[Rank].EndingBit;
    *Instance    = 0;

    if (Element.ElementAttributes.Bits.StrobeLookupTable == 1) {
      *Instance += (UINT8) StrobeToRegister[Strobe][0];
      *ByteOffset += StrobeToRegister[Strobe][1];
    }
    if (Element.ElementAttributes.Bits.RankLookupTable == 1) {
      *Instance += (UINT8) RankToRegister[Rank][0];
      *ByteOffset += RankToRegister[Rank][1];
    }
  }
  //
  // Per-Bit Handling
  //
  if (Element.ElementAttributes.Bits.BitBased == 1) {
    if (((Strobe >= 4) && (Strobe <= 7)) || ((Strobe >= 13) && (Strobe <= 16))) {
      //
      // Per Bit Swizzling relationship
      // Log - Phy
      // B0 - B2
      // B1 - B3
      // B2 - B0
      // B3 - B1
      //
      switch (Bit) {
      case 0:
        Bit = 2;
        break;
      case 1:
        Bit = 3;
        break;
      case 2:
        Bit = 0;
        break;
      case 3:
        Bit = 1;
        break;
      default:
        Bit = 0;
        break;
      }
    }
    *ByteOffset += Element.BitSPMByteOffset * Bit;
    *StartingBit += Bit * Element.BitSPMBitOffset;
    *EndingBit += Bit * Element.BitSPMBitOffset;
    //
    // Reached the next Register, calculate next ByteOffset and reset Bit count.
    //
    if (Bit * Element.BitSPMBitOffset >= 32) {
      *ByteOffset += ((Bit * Element.BitSPMBitOffset) / 32) * 4;
      Bit %= (32 / Element.BitSPMBitOffset);
    }
  }
  return MMRC_SUCCESS;
}


#if TRAINING_ALGOS == 1


MMRC_STATUS
BytelaneMask (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        *Mask
  )
{
#if CPGC_API
  *Mask = 0x01;
#else
  *Mask = 0x00;
  do {
    *Mask <<= BUSWIDTH;
    *Mask |= 0x00000001;
  } while (*Mask < 0x00010000);
  *Mask &= 0xffff;
#endif

  return MMRC_SUCCESS;
}

/**
  Based on the input type, this function will average out the 2x 
  values for all ranks on the given channel. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Minus1Index     Minus1 signal.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
Average (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINT16        Minus1Index
  )
{
  UINT8   Rank;
  UINT8   NumberOfRanks;
  UINT8   Strobe;
  UINT32  TempValue;
  UINT32  Average;

  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] ||
        !MrcData->MspData.ExecuteOnThisChannel[Channel] ||
        MrcData->Rk2Rk[Channel]) {
      continue;
    }

    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      Average  = 0;
      NumberOfRanks = 0;
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (!MrcData->RankEnabled[Channel][Rank]) {
          continue;
        }
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          break;
        }
        NumberOfRanks++;
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_GET_CACHE, &TempValue);
        Average += TempValue;
      } // Rank loop ...
      if (NumberOfRanks == 0) {
        continue;
      }
      Average = Average / NumberOfRanks;
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_SET_VAL_FC_UC, &Average);
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
        if (MrcData->MaxDq[Channel][Rank] <= 9) {
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe+9, Index, CMD_SET_VAL_FC_UC, &Average);
        }
#endif
        if (Minus1Index != MMRC_NA) {
          TempValue = 1;
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Minus1Index, CMD_SET_VAL_FC_UC, &TempValue);
        }
      }
    }
  } // Channel loop ...
  return MMRC_SUCCESS;
}

/**
  Based on the input type, this function will average out the 2x
  values for all ranks on the given channel.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       StartIndex      Pointer to the first Delay in the Group
  @param[in]       AllGroups       Sweep all groups
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
AverageGroups (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        StartIndex,
  IN        BOOLEAN       AllGroups
  )
{
  UINT8   MaxGroups;
  UINT8   GroupOffset;
  UINT8   GroupIndex;
  UINT16  Minus1Index;
  UINT16  TrainingDelay;


  TrainingDelay  = GetOffsetIndex (MrcData, StartIndex);

  if (AllGroups) {
    MaxGroups = SignalInfo[GetSignalInfoIndex (TrainingDelay)].NumSignalGroups;
  } else {
    MaxGroups = 1;
  }


  GroupOffset  = 0;
  for (GroupIndex = 0; GroupIndex < MaxGroups; GroupIndex++) {
    Minus1Index = SignalInfo[GetSignalInfoIndex (TrainingDelay + GroupOffset)].Minus1Index;
    Average (MrcData, Channel, TrainingDelay + GroupOffset, Minus1Index);
    GroupOffset += SignalInfo[GetSignalInfoIndex (TrainingDelay + GroupOffset)].FrequencyOffset;
  } // GroupIndex loop ...

  return MMRC_SUCCESS;
}
#if MINUS1_SUPPORT && TRAINING_ALGOS
/**
  This function will set the Minus1Select signal across all ranks and available strobes,
  and will adjust the final delay if this was previously set the Minus1 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Minus1Index     Minus1 signal.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
HandleMinus1Select (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINT16        Minus1Index
  )
{
  UINT32  Minus1Value;
  UINT32  IndexDelay;
  UINT8   Rank;
  UINT8   Strobe;
  //
  // As we need to set all the Minus1Select as '1' it is required we take a look in the current
  // Minus1Select signal and substract HALF_CLK in case it is not set, because at the end of this function
  // we will set the Minus1Select which will add HALF_CLK
  //
  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] || !MrcData->MspData.ExecuteOnThisChannel[Channel]) {
      continue;
    }
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Minus1Index, CMD_GET_CACHE, &Minus1Value);
        if (Minus1Value == 0) {
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_GET_CACHE, &IndexDelay);
          if (IndexDelay < GetHalfClk (MrcData, Channel)) {
            IndexDelay = 0;
          } else {
            IndexDelay -= GetHalfClk (MrcData, Channel);
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_SET_VAL_FC_UC, &IndexDelay);
        }
        //
        // Initialize the DQ Minus1Select for each strobe
        //
        Minus1Value = 1;
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Minus1Index, CMD_SET_VAL_FC_UC, &Minus1Value);
      } // Strobe loop ...
    } // Rank loop ...
  } // Channel loop ...

  return MMRC_SUCCESS;
}

/**
  Set the Minus1 signal according with the DELAY across Ranks.
  If the R2R switching is disabled, the current function MUST be executed after the average function

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Minus1Index     Minus1 signal.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SetMinus1Select (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINT16        Minus1Index
  )
{
  UINT8   Rank;
  UINT8   Strobe;
  UINT16  MaxValue[MAX_STROBES];
  UINT32  TempValue;
  UINT32  CurrentStrobeValue;
  UINT16  Delta;
  UINT16  CurrentHalfClk;
  UINT16  MaxHalfClk;

  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] || !MrcData->MspData.ExecuteOnThisChannel[Channel]) {
      continue;
    }
    //
    // Initialize the array
    //
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      MaxValue[Strobe] = 0;
    }
    //
    // Get the MaxValue of each strobe across ranks
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_GET_CACHE, &TempValue);
        if (TempValue > MaxValue[Strobe]) {
          MaxValue[Strobe] = (UINT16) TempValue;
        }
      } // Strobe loop ...
    } // Ranks loop ...

    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        //
        // Get the current strobe value.
        //
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_GET_CACHE, &CurrentStrobeValue);

        Delta = MaxValue[Strobe] - (UINT16) CurrentStrobeValue;
        if (Delta >= 2 * GetHalfClk (MrcData, Channel)) {
          MspDebugPrint ((MSP_DBG_MIN, "More than 1 2x clock separates strobe %d between ranks.\n", Strobe));
          return MrcErrRankStrobeClockDelta;
        }
        //
        // Determine if the MAX value across ranks is in the same HALF_CLK as the current Strobe value
        //
        MaxHalfClk      = MaxValue[Strobe] / GetHalfClk (MrcData, Channel);
        CurrentHalfClk  = (UINT16) CurrentStrobeValue / GetHalfClk (MrcData, Channel);

        if (MaxHalfClk > CurrentHalfClk) {
          //
          // Set the minus1 value.
          //
          TempValue = 0;
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Minus1Index, CMD_SET_VAL_FC_UC, &TempValue);
          //
          // We need to set the current strobe value to the same HALF_CLK as the Max Value, in order to so we need to add +1 HALF_CLK
          // assuming the max differences between current strobe and the max value is less than 2*HALF_CLK
          //
          CurrentStrobeValue += GetHalfClk (MrcData, Channel);
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_SET_VAL_FC_UC, &CurrentStrobeValue);
        }
      } // Strobe loop ...
    } // Rank loop ...
  } // Channel loop ...

  return MMRC_SUCCESS;
}
#endif // MINUS1_SUPPORT && TRAINING_ALGOS

#endif // TRAINING_ALGOS == 1

/**
  Switch the writeleveling mode on the DDRIO Physical Layer

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       WriteLevelMode  Write Leveling mode on or off
**/
MMRC_STATUS
WriteLevelingMode (
  MMRC_DATA         *MrcData,
  UINT8             Channel,
  UINT8             WriteLevelMode
)
{
#if FINE_WRITE_LEVELING == 1 && TRAINING_ALGOS == 1
  BOOLEAN   ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  if (WriteLevelMode != PHYEXITWRLVL && 
      WriteLevelMode != PHYENTERWRLVL) {
    return MMRC_FAILURE;
  }
  MrcData->MspData.DynamicVars[Channel][WRLVLMODEVAR] = WriteLevelMode;
  MmrcExecuteTask (MrcData, WRLVLMODE, NO_PRINT, Channel);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
#endif
  return MMRC_SUCCESS;
}

/**
  Set the Drams in or out of fine write leveling mode (MRS operation)
  specific DDR technologies (LPDDR, DDR3, etc) can add here their specific 
  configurations here

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Entry           Parameter to select Entry or Exit hook to execute.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FineWriteLevelingDramEntryExit (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT8             Entry
  )
{
#if FINE_WRITE_LEVELING == 1 && TRAINING_ALGOS == 1
  UINT8             ConfigureRank;              // Flag stating which rank is currently being configured.
#if DDR3_SUPPORT
  DramInitDDR3EMR1  Mrs1Command;
  DramInitDDR3EMR2  Mrs2Command;
#endif
#if DDR4_SUPPORT
  DramInitDDR4EMR1  Ddr4Mrs1Command;
  DramInitDDR4EMR2  Ddr4Mrs2Command;
#endif
  BOOLEAN   ExecuteMode;
  UINT8     St;
  UINT32    Value;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  //
  // OFF ODT
  //
  MmrcExecuteTask (MrcData, FORCEODT_OFF, NO_PRINT, Channel);

  ConfigureRank = 0;
  if (Entry == MMRC_ENTRY) {
    //
    // MMRC_ENTRY Set the Drams in Write Leveling mode
    //
#if LPDDR3_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeLpDdr3) {
      MrcData->MspData.DynamicVars[Channel][REG_DATA] = MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][2]] | LPDDR3_WL_ENABLE;
      MmrcExecuteTask (MrcData, SEND_DRAM_INIT, NO_PRINT, Channel);
    }
#endif // LPDDR3_SUPPORT
#if DDR3_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      for (ConfigureRank = 0; ConfigureRank < MAX_RANKS; ConfigureRank++) {
        if (!MrcData->RankEnabled[Channel][ConfigureRank]) {
          continue;
        }

        Mrs1Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 1);
        Mrs2Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 2);
        Mrs1Command.Bits.wlEnabled = 1;
        Mrs2Command.Bits.rtt_WR = 0;
        if (Rank != ConfigureRank) {
          Mrs1Command.Bits.Qoff = 1;
        }
        PrechargeAll (MrcData, Channel, ConfigureRank);
        WriteDramCommandSideWrap (MrcData, Channel, ConfigureRank, Mrs2Command.Data);
        WriteDramCommandSideWrap (MrcData, Channel, ConfigureRank, Mrs1Command.Data);
        MspDelay (NANO_DEL, 10000);
        if (Rank == ConfigureRank) {
          for (St = 0; St < MAX_STROBES; St++) {
            Value = 0;
            GetSetDdrioGroup2(MrcData, Channel, Rank, St, DqsdrvEnbIdle, CMD_SET_VAL_FC_UC, &Value);
          }
        }
      }
    }
#endif // DDR3_SUPPORT

#if DDR4_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      for (ConfigureRank = 0; ConfigureRank < MAX_RANKS; ConfigureRank++) {
        if (!MrcData->RankEnabled[Channel][ConfigureRank]) {
          continue;
        }
        Ddr4Mrs1Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 1);
        Ddr4Mrs2Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 2);
        Ddr4Mrs1Command.Bits.wlEnabled = 1;
        Ddr4Mrs2Command.Bits.RTT_WR = 0;
        if (Rank != ConfigureRank) {
          Ddr4Mrs1Command.Bits.Qoff = 1;
        }
        PrechargeAll (MrcData, Channel, ConfigureRank);
        WriteDramCommandSideWrap(MrcData, Channel, ConfigureRank, Ddr4Mrs2Command.Data);
        WriteDramCommandSideWrap(MrcData, Channel, ConfigureRank, Ddr4Mrs1Command.Data);
        MspDelay (NANO_DEL, 10000);
        if (Rank == ConfigureRank) {
          for (St = 0; St < MAX_STROBES; St++) {
            Value = 0;
            GetSetDdrioGroup2(MrcData, Channel, Rank, St, DqsdrvEnbIdle, CMD_SET_VAL_FC_UC, &Value);
          }
        }
      }
    }
#endif // DDR4_SUPPORT
  } else {
    //
    // MMRC_EXIT Set the Drams out of Write Leveling mode
    //
#if LPDDR3_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeLpDdr3) {
      MrcData->MspData.DynamicVars[Channel][REG_DATA] = MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][2]];
      MmrcExecuteTask (MrcData, SEND_DRAM_INIT, NO_PRINT, Channel);
    }
#endif // LPDDR3_SUPPORT
#if DDR3_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      for (ConfigureRank = 0; ConfigureRank < MAX_RANKS; ConfigureRank++) {
        if (MrcData->RankEnabled[Channel][ConfigureRank]) {
          Mrs1Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 1);
          Mrs2Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 2);
          if (Rank == 0) {
            for (St = 0; St < MAX_STROBES; St++) {
              Value = 3;
              GetSetDdrioGroup2(MrcData, Channel, Rank, St, DqsdrvEnbIdle, CMD_SET_VAL_FC_UC, &Value);
            }
          }
          WriteDramCommandSideWrap (MrcData, Channel, ConfigureRank, Mrs1Command.Data);
          WriteDramCommandSideWrap (MrcData, Channel, ConfigureRank, Mrs2Command.Data);
        }
      }
    }
#endif // DDR3_SUPPORT
#if DDR4_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      for (ConfigureRank = 0; ConfigureRank < MAX_RANKS; ConfigureRank++) {
        if (MrcData->RankEnabled[Channel][ConfigureRank]) {
          Ddr4Mrs1Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 1);
          Ddr4Mrs2Command.Data = ReadMrsCommand (MrcData, Channel, ConfigureRank, 2);
          if (Rank == 0) {
            for (St = 0; St < MAX_STROBES; St++) {
              Value = 3;
              GetSetDdrioGroup2(MrcData, Channel, Rank, St, DqsdrvEnbIdle, CMD_SET_VAL_FC_UC, &Value);
            }
          }
          WriteDramCommandSideWrap (MrcData, Channel, ConfigureRank, Ddr4Mrs1Command.Data);
          WriteDramCommandSideWrap (MrcData, Channel, ConfigureRank, Ddr4Mrs2Command.Data);
        }
      }
    }
#endif // DDR4_SUPPORT
  }

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
#endif
  return MMRC_SUCCESS;
}

/**
  Fine Write Leveling training algorithm
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FineWriteLeveling (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if FINE_WRITE_LEVELING == 1 && TRAINING_ALGOS == 1
  UINT8             Rank;                       // Current Rank being operated on.
  UINT8             Strobe;                     // Current Strobe being operated on.
  UINT32            TempValue;                  // Temporary place value for Get/Set API.
  UINT8             SmallStep;                  // Small step size.
  UINT8             NumberElements[MAX_CHANNELS];
  UINT16            Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT16            Low[MAX_CHANNELS][MAX_STROBES];
  UINT16            High[MAX_CHANNELS][MAX_STROBES];
  UINT16            Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT8             ChannelBackup;
  UINT8             DebugLevel;
  UINT8             DitherVal;
  ChannelBackup = Channel;

  if (MrcData->SkipTrainingAlgos[LFineWriteLeveling]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }


#if SIM
  SmallStep = RCVN_SMALL_STEP_SIM;
  DitherVal = 1;
#else
  SmallStep = RCVN_SMALL_STEP;
  DitherVal = 4;
#endif
  DebugLevel = MrcData->MspData.DebugMsgLevel;
  //
  // Turn on 1D sweep debug messages if MAX message level is set.
  //
  if ((MrcData->MspData.DebugMsgLevel & MSP_DBG_MAX) == MSP_DBG_MAX) {
    MrcData->MspData.DebugMsgLevel |= MSP_DBG_DEBUG;
  }
  MrcData->SignalType = WrLvlSmp;
  // 
  // Entry Hooks
  //
#ifdef FINEWRITELEVEL_ENTRY
  MmrcExecuteTask (MrcData, FINEWRITELEVEL_ENTRY, NO_PRINT, Channel);
#endif
  if (!MrcData->RestorePath) {
#if CPGC_BASED_READWRITES == 1
    MrcData->CpgcOptions.LoopCount = 1;
    MrcData->CpgcOptions.NumBursts = 0;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_EARLYTRAINING);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
#endif

    //
    // Put the active rank in WL Mode, all others enabled ranks in QBUF Off.
    // skipping the disabled ranks.
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->MspData.ExecuteThisRoutineInParallel) {
        MspDebugPrint ((MSP_DBG_MAX, "CxxR%d\n", Rank));
      } else {
        MspDebugPrint ((MSP_DBG_MAX, "C%02dR%d\n", Channel, Rank));
      }
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        NumberElements[Channel] = MrcData->MaxDq[Channel][Rank];
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
#if CPGC_BASED_READWRITES == 1
        //
        // Setup the CPGC engine to to do a single read from an address within the 
        // selectable rank.  The engine should be setup for LFSR mode.
        //
        MrcData->CpgcOptions.Rank = Rank;
        MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE;
        CpgcReconfig (MrcData, Channel);
#endif 
        //
        // Take the DRAM in Write Leveling mode.
        //
        FineWriteLevelingDramEntryExit (MrcData, Channel, Rank, MMRC_ENTRY);
#if TX_DQS_MINUS1_SUPPORT
        //
        // Initialize the DQ and DQS PtrMinus1Select for each strobe, and print out the label.
        //
        TempValue = 1;
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqMinus1, CMD_SET_VAL_FC_UC, &TempValue);
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqsMinus1, CMD_SET_VAL_FC_UC, &TempValue);
        }
#endif // TX_DQS_MINUS1_SUPPORT
        //
        // Set WDQS to WCLK initially for each strobe. Set all initial values.
        //
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, CkAll, CMD_GET_REG_UC, &TempValue);
#ifdef SIM
          //
          // This is a WA in order Rank[x] (x > 0) is able to enter into Write Leveling Mode
          //
          TempValue = (2 * GetOneClk (MrcData, Channel));
#endif
          TempValue += (1 * GetOneClk (MrcData, Channel));
          Dim1StartPoint[Channel][0][Strobe][LOW] = (UINT16) TempValue;
          Dim1StartPoint[Channel][0][Strobe][HIGH] = Dim1StartPoint[Channel][0][Strobe][LOW] + GetOneClk (MrcData, Channel) + 10;
          Low[Channel][Strobe] = 0;
          High[Channel][Strobe] = Dim1StartPoint[Channel][0][Strobe][LOW] + GetOneClk (MrcData, Channel);
        }
        //
        // Take the phy in write leveling mode.
        //
        WriteLevelingMode (MrcData, Channel, PHYENTERWRLVL);
      } // Channel loop ...

      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      //
      // Issue the ForceODT ON command
      //
      switch (Rank){
      case 0:        
        MrcData->MspData.DynamicVars[Channel][ODTVAL_FORCE_ON] = MrcData->MspData.DynamicVars[Channel][WRODT_RANK0];
        break;
      case 1:
        MrcData->MspData.DynamicVars[Channel][ODTVAL_FORCE_ON] = MrcData->MspData.DynamicVars[Channel][WRODT_RANK1];
        break;
      case 2:
        MrcData->MspData.DynamicVars[Channel][ODTVAL_FORCE_ON] = MrcData->MspData.DynamicVars[Channel][WRODT_RANK2];
        break;
      case 3:
        MrcData->MspData.DynamicVars[Channel][ODTVAL_FORCE_ON] = MrcData->MspData.DynamicVars[Channel][WRODT_RANK3];
        break;
      default:
        MrcData->MspData.DynamicVars[Channel][ODTVAL_FORCE_ON] = MrcData->MspData.DynamicVars[Channel][WRODT_RANK0];
        break;
      }

      MmrcExecuteTask (MrcData, FORCEODT_ON, NO_PRINT, Channel);
      //
      // Find a rising edge in the write leveling. This will be returned in Results[Strobe][Bit][LOW].
      //
      Create1DSweep (MrcData, Channel, Rank, TxDqsDelay, 1, Dim1StartPoint, Low, High, SmallStep,
        FALSE, FALSE, FALSE, NumberElements, SampleDqs, Results, "S", DitherVal, RUN_PER_STROBE
        );

      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        //
        // Write final FWL values to registers and cache.
        //
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          TempValue = Results[Channel][0][Strobe][0][LOW];
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqsDelay, CMD_SET_VAL_FC_UC, &TempValue);
        }
        //
        // Take the Ddrio phy out of write leveling mode.
        //
        WriteLevelingMode (MrcData, Channel, PHYEXITWRLVL);
        //
        // Take the DRAM out of Write Leveling mode.
        //
        FineWriteLevelingDramEntryExit (MrcData, Channel, Rank, MMRC_EXIT);

      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      //
      // Release ODT
      //
      MmrcExecuteTask (MrcData, FORCEODT_REL, NO_PRINT, Channel);
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
    } // Rank loop ...
  } //Restore Path
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqsDelay, FALSE);
  // 
  // DDRIO Phy Exit Hooks
  //
#ifdef FINEWRITELEVEL_EXIT
  MmrcExecuteTask (MrcData, FINEWRITELEVEL_EXIT, NO_PRINT, Channel);
#endif
  MrcData->MspData.DebugMsgLevel = DebugLevel;

  if (TestMrcTrainingSteps (MrcData, ChannelBackup, FWL_ENABLE_CHECKER, 0, 0, 0, 0) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#endif // FINE_WRITE_LEVELING_SUPPORT
  return MMRC_SUCCESS;
}

VOID
MprWrite(
  MMRC_DATA         *MrcData,
  UINT8             Channel,
  UINT8             Rank
  )
{
  DRAM_CMD_DDR4     DramCmdDdr4;
  UINT8             SpdAddMap;
  UINT8             SpdDeviceOrg;

  if(Rank >= MAX_RANKS){
    MrcData->ErrorCode = MrcErrParameterOutOfBound;
    MrcDeadLoop();
    return;
  }

  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4 &&
    MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
    //  if (mprMode) {
    //    SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, mprMode); <-- Not needed for DNV as we are not changing the pattern
    //    MprWritePatternRankDdr4 (host, socket, ch, dimm, rank, A_SIDE, pattern); <-- Not needed for DNV as we are not changing the pattern
    //    SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, 0); <-- Not needed for DNV as we are not changing the pattern
    //    SetRankMprDdr4 (host, socket, ch, dimm, rank, B_SIDE, 0, mprMode);
    //    MprWritePatternRankDdr4 (host, socket, ch, dimm, rank, B_SIDE, pattern);
    //    SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, mprMode);
    //  }
    //
    // To match the code in Purley
    //
    ToggleMprModeSide (MrcData, Channel, Rank, 0, SIDE_A);

    ToggleMprModeSide (MrcData, Channel, Rank, 1, SIDE_B);

    DramCmdDdr4.Data                = 0;
    DramCmdDdr4.Bits.command        = 4;
    DramCmdDdr4.Bits.act_n          = 1;
    //
    // As HandleAddressMirroring will invert the signals BA and BG, we have to invert them first.
    // WR BA=3 -> BA=0(after HandleAddressMirroring) --> BA=0 (Before RCD) --> BA=3 (After RCD)
    //
    DramCmdDdr4.Bits.bankaddress    = 3;
    DramCmdDdr4.Bits.bankgroup      = 0;
    DramCmdDdr4.Bits.rankselect     = Rank;
    DramCmdDdr4.Bits.ba             = 0;
    DramCmdDdr4.Bits.mpr            = 0;
    DramCmdDdr4.Bits.prea           = 0;
    DramCmdDdr4.Bits.memoryaddress  = 0x55;

    SpdAddMap = MrcData->MrcParameters.SpdData[Channel][Rank/2].Buffer[SPD_DDR4_ADD_MAPPING];
    SpdDeviceOrg = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR4_ORG];
    HandleAddressMirroring(&DramCmdDdr4.Data, SpdAddMap, SpdDeviceOrg, TypeDdr4, SPD_DDR_RDIMM, SIDE_B);
    MrcData->MspData.DynamicVars[Channel][REG_DATA] = DramCmdDdr4.Data;
    MmrcExecuteTask (MrcData, SEND_DRAM_INIT, NO_PRINT, Channel);
    ToggleMprModeSide (MrcData, Channel, Rank, 0, SIDE_B);

    ToggleMprModeSide (MrcData, Channel, Rank, 1, SIDE_A);
    ToggleMprModeSide (MrcData, Channel, Rank, 1, SIDE_B);
  } else {
    ToggleMprMode (MrcData, Channel, Rank, 1);
  }
}

/**
  Performs Early MPR Read Training. This training must be executed only before CWL in order to set
  safe read timing settings
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
EarlyMprRead (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if EARLY_READ_TRAINING == 1 && TRAINING_ALGOS == 1
  UINT8   Rank;
  UINT8   ChannelBackup;
  UINT8   DebugLevel;
  UINT8   StepSize;

  if (MrcData->SkipTrainingAlgos[LEarlyMpr]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }


  MrcData->CpgcOptions.RmtRestoreJedecResetType     = GetJedecResetTypeForRd ();
  DebugLevel = MrcData->MspData.DebugMsgLevel;
  ChannelBackup = Channel;

#if FAULTY_PART_TRACKING
  FaultyPartTrackInit (MrcData, Channel, TRUE);
#endif
  //
  // Early read training entry 
  //
  MmrcExecuteTask (MrcData, EARLY_READ_TRAINING_ENTRY, NO_PRINT, Channel);
  if (!MrcData->RestorePath) {
#if CPGC_BASED_READWRITES == 1
      MrcData->CpgcOptions.LoopCount = 1;
      MrcData->CpgcOptions.NumBursts = 0;
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        CpgcSetup (MrcData, Channel, CPGC_CONFIG_EARLY_MPR_TRAINING);
      }
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
#endif
    //
    // Training
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->MspData.ExecuteThisRoutineInParallel) {
        MspDebugPrint ((MSP_DBG_MAX, "CxxR%d\n", Rank));
      } else {
        MspDebugPrint ((MSP_DBG_MAX, "C%02dR%d\n", Channel, Rank));
      }
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
#if CPGC_BASED_READWRITES == 1
        //
        // Setup the CPGC engine to to do a single read from an address within the 
        // selectable rank.  The engine should be setup for LFSR mode.
        //
        MrcData->CpgcOptions.Rank = Rank;
        MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_READ;
        CpgcReconfig (MrcData, Channel);
#endif
        PrechargeAll (MrcData, Channel, Rank);
        //
        // Enable the MPR for the rank under training
        //
        MprWrite (MrcData, Channel, Rank);
      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
#if SIM
      //
      // Step size 10 for SIM to speed things up.
      //
      StepSize = 10;
#else
      StepSize = 2;
#endif

      SweepCenterEye2D (
        MrcData, 
        Channel, 
        Rank, 
        RxDqsDelay, 
        0, 
        EARLYMPR_RXDQS_SWEEP_END, 
        StepSize, 
        ABSOLUTE, 
        RxVref, 
        0,
        95,
        2,
        ABSOLUTE | FOUND_PASS_CONDITION
      );
      MrcData->MspData.DebugMsgLevel = DebugLevel;
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        //
        // Disable the MPR for the rank under training
        //
        ToggleMprMode (MrcData, Channel, Rank, 0);
      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
    } // Rank loop ...

   /* for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->MspData.ExecuteThisRoutineInParallel) {
        MspDebugPrint ((MSP_DBG_MAX, "CxxR%d\n", Rank));
      } else {
        MspDebugPrint ((MSP_DBG_MAX, "C%02dR%d\n", Channel, Rank));
      }
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          Temp = 32;
          GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, RxVref, CMD_SET_VAL_FC_UC, &Temp);
          GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, RxDqsNDelay, CMD_SET_VAL_FC_UC, &Temp);
          GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, RxDqsPDelay, CMD_SET_VAL_FC_UC, &Temp);
        }
      } // Channel loop ...
    }*/
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;    
#if RDQS_ODDEVENSUPPORT==1
    AverageGroups (MrcData, Channel, RxDqsNDelay, TRUE);
#else
    AverageGroups (MrcData, Channel, RxDqsDelay, TRUE);
#endif
  } // Restore path

  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;    

  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResultsRxDqsWrapper (MrcData, Channel, TRUE);
  //
  // Early read training exit
  //
  MmrcExecuteTask (MrcData, EARLY_READ_TRAINING_EXIT, NO_PRINT, Channel);

#endif // EARLY_READ_TRAINING
  return MMRC_SUCCESS;
}


MMRC_STATUS
CoarseWLTest (
IN  OUT   MMRC_DATA     *MrcData,
IN        UINT8         Channel,
IN        UINT8         Rank,
IN        UINT8         NumberElements[MAX_CHANNELS],
IN        UINT8         NumberKnobs,
IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
)
{
  UINT8   Element;
#if LPDDR3_SUPPORT == 1
  UINT32  TempValue;
#endif
#if LPDDR4_SUPPORT == 1 || DDR4_SUPPORT == 1
  UINT8   Knob;
  UINT32  DQValue;
  UINT8   AllPassFlag;
  UINT32  Center;
  UINT16  Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT16  Dim1Min[MAX_CHANNELS][MAX_STROBES];
  UINT16  Dim1Max[MAX_CHANNELS][MAX_STROBES];
  UINT16  Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT8   Cwl_Dq_Step;
  Cwl_Dq_Step = 2;
#endif

  if (!MrcData->RankEnabled[Channel][Rank]) {
    return MMRC_SUCCESS;
  }

  //
  // If LP3
  //
#if LPDDR3_SUPPORT == 1 
  if (MrcData->Channel[Channel].DramType == TypeLpDdr3 || MrcData->Channel[Channel].DramType == TypeDdr3L) {
    //
    // Tether DQ to DQS.
    //
    for (Element = 0; Element < NumberElements; Element++) {
      GetSetDdrioGroup2(MrcData, Channel, Rank, Element, TxDqsDelay, CMD_GET_REG_UC, &TempValue);
      if (MrcData->Channel[Channel].DramType == TypeDdr3L) {
        TempValue -= 1 * GetQtrClk(MrcData, Channel);
      }
      else {
        TempValue += 3 * GetQtrClk(MrcData, Channel);
      }
      GetSetDdrioGroup2(MrcData, Channel, Rank, Element, TxDqDelay, CMD_SET_VAL_FC_UC, &TempValue);
    }
    PerformFifoReset(MrcData, Channel, Rank);
    MySimStall(500);
    GenericByteMemoryTest(MrcData, Channel, Rank, NumberElements, NumberKnobs, PassFail);
  }
#endif

#if LPDDR4_SUPPORT == 1 || DDR4_SUPPORT == 1 || DDR3_SUPPORT == 1
  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4 || MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    //
    // Initialize required variables.
    // 
    for (Element = 0; Element < NumberElements[Channel]; Element++) {
      if (!IsStrobeValid(MrcData, Channel, Rank, Element, FALSE, NULL)) {
        continue;
      }
      // 
      // Read the current DQS Value
      //
      GetSetDdrioGroup2(MrcData, Channel, Rank, Element, TxDqsDelay, CMD_GET_REG_UC, &DQValue);
      for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
        Dim1Min[Channel][Element] = (UINT16)(DQValue - GetHalfClk(MrcData, Channel));
        Dim1Max[Channel][Element] = (UINT16)(DQValue + GetHalfClk(MrcData, Channel));
        Dim1StartPoint[Channel][Knob][Element][LOW] = (UINT16)(DQValue - GetHalfClk(MrcData, Channel));
        Dim1StartPoint[Channel][Knob][Element][HIGH] = (UINT16)(DQValue + GetHalfClk(MrcData, Channel));
      } // for Knob
    } // for Element
    //
    // Turn on 1D sweep debug messages if MAX message level is set.
    //
    if ((MrcData->MspData.DebugMsgLevel & MSP_DBG_MAX) == MSP_DBG_MAX) {
      MrcData->MspData.DebugMsgLevel &=((UINT8) ~MSP_DBG_DEBUG);
    }

    Create1DSweep(MrcData, 
      Channel, 
      Rank, 
      TxDqDelayG, 1, 
      Dim1StartPoint, 
      Dim1Min, 
      Dim1Max, 
      Cwl_Dq_Step, 
      FALSE, 
      FALSE, 
      FALSE, 
      NumberElements, 
      CpgcExecuteRmtTest, 
      Results, "", 
      5, RUN_PER_STROBE);
    //
    // Turn on 1D sweep debug messages if MAX message level is set.
    //
    if ((MrcData->MspData.DebugMsgLevel & MSP_DBG_MAX) == MSP_DBG_MAX) {
      MrcData->MspData.DebugMsgLevel |= MSP_DBG_DEBUG;
    }

    AllPassFlag = TRUE;
    for (Element = 0; Element < NumberElements[Channel]; Element++) {
      if (!IsStrobeValid(MrcData, Channel, Rank, Element, FALSE, NULL)) {
        continue;
      }
      if ((Results[Channel][0][Element][0][HIGH] > Results[Channel][0][Element][0][LOW]) && ((Results[Channel][0][Element][0][HIGH] - Results[Channel][0][Element][0][LOW]) > (GetQtrClk(MrcData, Channel) / 4))){
        PassFail[Channel][0][Element][0][CURRENT_SAMPLE] = RESULTS_PASS;
      } else {
        PassFail[Channel][0][Element][0][CURRENT_SAMPLE] = RESULTS_FAIL;
        AllPassFlag = FALSE;
      }
    }
    if (AllPassFlag == TRUE) {
      for (Element = 0; Element < NumberElements[Channel]; Element++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, Element, FALSE, NULL)) {
          continue;
        }
        Center = (Results[Channel][0][Element][0][HIGH] + Results[Channel][0][Element][0][LOW]) / 2;
        MspDebugPrint ((MSP_DBG_MIN, "   Dq: %d   ", Center ));
        GetSetDdrioGroup2(MrcData, Channel, Rank, Element, TxDqDelay, CMD_SET_VAL_FC_UC, &Center); 
        GetSetDdrioGroup2(MrcData, Channel, Rank, Element, TxDqDrvDelay, CMD_SET_VAL_FC_UC, &Center); 
      }
      MspDebugPrint ((MSP_DBG_MIN, "\n"));
    }
  } // if LPDDR4
#endif
  //
  // Perform a fifo reset.
  //
  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);

  return MMRC_SUCCESS;
}
/**
  Performs Coarse Write Leveling.  This is reusing many of the same routines used with the Receive Enable 
  calibration.  
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.    
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CoarseWriteLeveling (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if COARSE_WRITE_LEVELING == 1 && TRAINING_ALGOS == 1
  UINT8   Rank;
  UINT8   Strobe;
  UINT8   NumberElements[MAX_CHANNELS];
  UINT32  TempValue;
  UINT16  Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT16  Low[MAX_CHANNELS][MAX_STROBES];
  UINT16  High[MAX_CHANNELS][MAX_STROBES];
  UINT16  Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT8   ChannelBackup;
  UINT8   DebugLevel;

  ChannelBackup = Channel;

  if (MrcData->SkipTrainingAlgos[LCoarseWriteLeveling]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    MrcData->EarlyWriteTrainingEnable = TRUE;
  } else {
    MrcData->EarlyWriteTrainingEnable = FALSE;
  }

  MrcData->CpgcOptions.RmtRestoreJedecResetType     = FAIL_SINGLE_BIT;

#if FAULTY_PART_TRACKING
  FaultyPartTrackInit (MrcData, Channel, FALSE);
#endif
  DebugLevel = MrcData->MspData.DebugMsgLevel;
  //
  // Turn on 1D sweep debug messages if MAX message level is set.
  //
  if ((MrcData->MspData.DebugMsgLevel & MSP_DBG_MAX) == MSP_DBG_MAX) {
    MrcData->MspData.DebugMsgLevel |= MSP_DBG_DEBUG;
  }
  //
  // CoarseWriteLevel Entry
  //
#ifdef COARSEWRITELEVEL_ENTRY
  MmrcExecuteTask (MrcData, COARSEWRITELEVEL_ENTRY, NO_PRINT, Channel);
#endif
  if (!MrcData->RestorePath) {
#if CPGC_BASED_READWRITES == 1
    MrcData->CpgcOptions.LoopCount = 1;
    MrcData->CpgcOptions.NumBursts = 1;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_EARLYTRAINING);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
#endif
    // 
    // Training
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->MspData.ExecuteThisRoutineInParallel) {
        MspDebugPrint ((MSP_DBG_MAX, "CxxR%d\n", Rank));
      } else {
        MspDebugPrint ((MSP_DBG_MAX, "C%02dR%d\n", Channel, Rank));
      }
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        NumberElements[Channel] = MrcData->MaxDq[Channel][Rank];
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
#if CPGC_BASED_READWRITES == 1
        //
        // Setup the CPGC engine to to do a single read from an address within the 
        // selectable rank.  The engine should be setup for LFSR mode.
        //
        MrcData->CpgcOptions.Rank = Rank;
        MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE_READ;
        CpgcReconfig (MrcData, Channel);
#endif 
        PrechargeAll (MrcData, Channel, Rank);
        //
        // Perform a FIFO Reset.
        //
        MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
        //
        // Increment the WDQS by 2 CLKS tethering WDQ.
        //
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqsDelay, CMD_GET_CACHE, &TempValue);
          if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
            TempValue += 2 * GetOneClk (MrcData, Channel);
          } else {
            TempValue += 3 * GetOneClk (MrcData, Channel);
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqsDelay, CMD_SET_VAL_FC_UC, &TempValue);
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
          if (MrcData->MaxDq[Channel][Rank] <= 9) {
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxDqsDelay, CMD_SET_VAL_FC_UC, &TempValue);
          }
#endif
          Dim1StartPoint[Channel][0][Strobe][LOW]  = (UINT16) (TempValue);
          Dim1StartPoint[Channel][0][Strobe][HIGH] = Dim1StartPoint[Channel][0][Strobe][LOW];
          Low[Channel][Strobe] = 0;
          High[Channel][Strobe] = Dim1StartPoint[Channel][0][Strobe][HIGH];
          TempValue -= GetQtrClk (MrcData, Channel);
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqDelay, CMD_SET_VAL_FC_UC, &TempValue);
#ifdef TxDqDrvDelay
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqDrvDelay, CMD_SET_VAL_FC_UC, &TempValue);
#endif
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
          if (MrcData->MaxDq[Channel][Rank] <= 9) {
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxDqDelay, CMD_SET_VAL_FC_UC, &TempValue);
#ifdef TxDqDrvDelay
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxDqDrvDelay, CMD_SET_VAL_FC_UC, &TempValue);
#endif
          }
#endif
        }
      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      //
      // Find the correct clock setting for writes. When this routine returns, the correct setting
      // will be programmed.
      //
      if (MrcData->EarlyWriteTrainingEnable == TRUE) {
        Create1DSweep (
          MrcData,
          Channel,
          Rank,
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
          WrLvlDelayExt,
#else
          WrLvlDelay,
#endif
          1,
          Dim1StartPoint,
          Low,
          High,
          GetOneClk (MrcData, Channel),
          FALSE,
          TRUE,
          FALSE,
          NumberElements,
          CoarseWLTest,
          Results,
          "S",
          1,
          RUN_PER_STROBE
          );
      } else {
        Create1DSweep (
          MrcData,
          Channel,
          Rank,
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
          WrLvlDelayExt,
#else
          WrLvlDelay,
#endif
          1,
          Dim1StartPoint,
          Low,
          High,
          GetOneClk (MrcData, Channel),
          FALSE,
          TRUE,
          FALSE,
          NumberElements,
          CpgcExecuteRmtTest,
          Results,
          "S",
          1,
          RUN_PER_STROBE
          );
      }

      if (MrcData->EarlyWriteTrainingEnable == FALSE) {
        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            //
            // Update the cache so everything is coherent for this rank.
            //
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqsDelay, CMD_GET_REG_UC, &TempValue);
            TempValue -= GetQtrClk (MrcData, Channel);
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqDelayG, CMD_SET_VAL_FC_UC, &TempValue);
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqDelay, CMD_SET_VAL_FC_UC, &TempValue);
#ifdef TxDqDrvDelay
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqDrvDelay, CMD_SET_VAL_FC_UC, &TempValue);
#endif
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
            if (MrcData->MaxDq[Channel][Rank] <= 9) {
              GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxDqDelay, CMD_SET_VAL_FC_UC, &TempValue);
#ifdef TxDqDrvDelay
              GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxDqDrvDelay, CMD_SET_VAL_FC_UC, &TempValue);
#endif
            }
#endif
          }
        } // Channel loop ...
      }
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
    } // Rank loop ...

    AverageGroups (MrcData, Channel, TxDqsDelay,   FALSE);
    AverageGroups (MrcData, Channel, TxDqDelay,    FALSE);
#ifdef TxDqDrvDelay
    AverageGroups (MrcData, Channel, TxDqDrvDelay, FALSE);
#endif

#if TX_DQS_MINUS1_SUPPORT == 1
    SetMinus1Select (MrcData, Channel, TxDqsDelay, TxDqsMinus1);
#endif // TX_DQS_MINUS1_SUPPORT
#if TX_DQ_MINUS1_SUPPORT == 1
    SetMinus1Select (MrcData, Channel, TxDqDelay, TxDqMinus1);
#endif // TX_DQ_MINUS1_SUPPORT
  }
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqsDelay, FALSE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqDelay, TRUE);
#ifdef TxDqDrvDelay
  DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDrvDelay, FALSE);
#endif

  //
  // CoarseWriteLevel Exit 
  //
#ifdef COARSEWRITELEVEL_EXIT
  MmrcExecuteTask (MrcData, COARSEWRITELEVEL_EXIT, NO_PRINT, Channel);
#endif
  MrcData->MspData.DebugMsgLevel = DebugLevel;

  if (MrcData->MemoryPreservation && ((MrcData->MrcParameters.BootMode == S0) || (MrcData->MrcParameters.BootMode == ADR))) {
    //
    // Skip memory test
    //
    MspDebugPrint ((MSP_DBG_MIN, "Skip memory check (Memory Preservation)\n"));
    return MMRC_SUCCESS;
  }

  if (TestMrcTrainingSteps (MrcData, ChannelBackup, CPGC_CONFIG_EARLYTRAINING, CPGC_CMDSEQ_WRITE_READ, 0, 1, 1) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#endif // COARSE_WRITE_LEVELING
  return MMRC_SUCCESS;
}

BOOLEAN
IsEccStrobe (
  MMRC_DATA *MrcData,
  UINT8      Channel,
  UINT8      Strobe
  )
{
  //
  // If this is an ECC strobe, return true.
  //
  if ((MrcData->EccStrobes[Channel] & (1 << Strobe)) != 0) {
    return TRUE;
  }
  return FALSE;
}

/**
IsStrobeValid.   Is used to select the strobe when the platforms
supports x4 and x8 configurations also provides the MaxBit value
that should be used by that strobe. On x4 configurations MaxBit would be4
and on x8 configurations MaxBit should be 8

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@param[in]       Channel         Current Channel being examined.
@param[in]       Rank            Rank.
@param[in]       Strobe          Strobe.
@param[in]       BitLevel        Enables the fill to the return value for MaxBit.
@param[out]      MaxBit          1 if no bit control needed, 4 or 8 depending on the device width configuration.
@retval          MMRC_SUCCESS
**/
BOOLEAN 
IsStrobeValid (
  IN OUT MMRC_DATA *MrcData,
  IN     UINT8      Channel,
  IN     UINT8      Rank,
  IN     UINT8      Strobe,
  IN     BOOLEAN    BitLevel,
  OUT    UINT8     *MaxBit
  )
{
  //
  // Skip this strobe if it's greater than the total number of strobes in the system or
  // if it is an ECC strobe and ECC is disabled.
  //
  if (Strobe >= MrcData->MaxDq[Channel][Rank] ||
    (!MrcData->MrcParameters.EccEnabled[Channel] && (IsEccStrobe (MrcData, Channel, Strobe)))) {
    return FALSE;
  } else {
    if (MaxBit != NULL) {
      //
      // Some algorithms and functions handle strobes at nibble, bytelane or bit level hence MaxBit 
      // would be 1 [strobe level use] else set 4 or 8 [bit level use] according to the configuration
      //
      if (BitLevel == FALSE) {
        *MaxBit = 1;
      } else {
        *MaxBit = MrcData->MaxBit[Channel][Rank];
      }
    }
    if (MrcData->MspData.DynamicVars[Channel][CHANNEL_WIDTH_32] == 1) {
      if (((Strobe >=4)&& (Strobe <=7)) || ((Strobe >=13)&& (Strobe <=16))) {
        return FALSE;
      }
    }
    return TRUE;
  }
}


MMRC_STATUS
CpgcExecuteSingleTest (
  IN  OUT   MMRC_DATA     *MrcData, 
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  OUT       UINT32        *CompareFlag,
  OUT       UINT32        *LaneErrorHigh,
  OUT       UINT32        *LaneErrorLow
  )
{
  //
  // Perform the CPGC Test.
  //
  if ((MrcData->SkipOptions & SKIP_TEST) != 0) {
    *CompareFlag = 0;
    if (LaneErrorHigh != NULL) {
      *LaneErrorHigh = 0;
    }
    if (LaneErrorLow != NULL) {
      *LaneErrorLow  = 0;
    }
    return MMRC_SUCCESS;
  }
  MmrcExecuteTask (MrcData, CPGC_RUNTEST, NO_PRINT, Channel);
  *CompareFlag = (UINT32)MrcData->MspData.DynamicVars[Channel][BYTEGRP_ERR_STAT];
  if (MrcData->MrcParameters.EccEnabled[Channel]) {
    *CompareFlag |= (MrcData->MspData.DynamicVars[Channel][ECC_ERR_STAT] << 8);
  }
  if (LaneErrorHigh != NULL) {
    *LaneErrorHigh = (UINT32) MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT47];
    MspDebugPrint ((MSP_DBG_TST, "    *LANE_ERR_STAT47 = 0x%08x\n", (UINT32) MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT47]));
  }
  if (LaneErrorLow != NULL) {
    *LaneErrorLow  = (UINT32) MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT03];
    MspDebugPrint ((MSP_DBG_TST, "    *LANE_ERR_STAT03  = 0x%08x\n", (UINT32) MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT03]));
  }

  return MMRC_SUCCESS;
}

#if TRAINING_ALGOS
VOID
PrintAndRestoreAfterFailureValues (
  IN    OUT     MMRC_DATA         *MrcData,
  IN            UINT8             Channel,
  IN            UINT8             Rank,
  IN            UINT8             NumberElements[MAX_CHANNELS],
  IN            UINT8             SignalsToRestore,
  IN            UINT16            DelayOffset,
  IN            UINT16            GroupOffset,
  IN            UINT32            *ValueToRestore,
  IN            UINT8             *Str
)
{
  UINT8   Strobe;
  UINT32  TempValue;

  UINT8   DebugLevel;
  DebugLevel = MrcData->MspData.DebugMsgLevel;

  MspDebugPrint ((MSP_DBG_TST, "%s C%dR%dSg%d", Str, Channel, Rank, SignalsToRestore));

  for (Strobe = 0; Strobe < NumberElements[Channel]; Strobe++) {
    if (MrcData->RestoreStrobeMaskEnable & ((UINT32)(1 << Strobe))) {
      MspDebugPrint((MSP_DBG_TST, " _S%02d", Strobe));
    } 
  } // Strobe loop ...
  MspDebugPrint ((MSP_DBG_TST, "\n"));

  MspDebugPrint((MSP_DBG_TST, "Value:     "));
  for (Strobe = 0; Strobe < NumberElements[Channel]; Strobe++) {
    if (MrcData->RestoreStrobeMaskEnable & ((UINT32)(1 << Strobe))) {
      TempValue = ValueToRestore[Strobe];
      MspDebugPrint((MSP_DBG_TST, " %04d", TempValue));
      GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, DelayOffset + GroupOffset, CMD_SET_VAL_FC_UC, &TempValue);
    }
  } // Strobe loop ...
  MspDebugPrint((MSP_DBG_TST, "\n"));
  MspDebugPrint((MSP_DBG_TST, "DelayOffset"));
  for (Strobe = 0; Strobe < NumberElements[Channel]; Strobe++) {
    if (MrcData->RestoreStrobeMaskEnable & ((UINT32)(1 << Strobe))) {
      MspDebugPrint((MSP_DBG_TST, " %04x", DelayOffset));
    }
  } // Strobe loop ...
  MspDebugPrint((MSP_DBG_TST, "\n"));

  MspDebugPrint((MSP_DBG_TST, "GroupOffset"));
  for (Strobe = 0; Strobe < NumberElements[Channel]; Strobe++) {
    if (MrcData->RestoreStrobeMaskEnable & ((UINT32)(1 << Strobe))) {
      MspDebugPrint((MSP_DBG_TST, " %04x", GroupOffset));
    }
  } // Strobe loop ...
  MspDebugPrint((MSP_DBG_TST, "\n"));

  MrcData->MspData.DebugMsgLevel = DebugLevel;
  return; 
}


/**
  CPGC routines to begin the CPGC Test, poll for the completion, 
  after the test is done return the the status of the test which 
  is a bytelane+ecc bit-test. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined. 
  @param[in]       NumberElements  Number of elements to test 
                                   (i.e. MAX_STROBES or MAX_BITS, etc).
  @param[in]       NumberKnobs     Number of knobs.
  @param[in, out]  PassFail        Array of passing or failing result for every element.         
  @retval          MMRC_SUCCESS    If all elements passed.
  @retval         !MMRC_SUCCESS    Otherwise.
**/
MMRC_STATUS
CpgcExecuteRmtTest (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
  )
{
  UINT8   ChannelBackup;
  UINT8   DevWidthIndex;
  UINT16  GroupOffset;
  UINT16  DelayOffset;

  UINT8   ConsoleElements;
  UINT8   Strobe;
  UINT8   Knob;

  UINT8   MaxBits;
  UINT8   IndBit;

  UINT8   IndStrobeTarget;
  UINT8   IndBitTarget;

  UINT32  CurrentError;
  UINT16  DynamicOffset;
  UINT32  DynamicMask;
  UINT8   DynamicShift;
  UINT8   SignalsToRestore;

  BOOLEAN BreakStrobeLoop;
  BOOLEAN BreakBitLoop;

  BOOLEAN AnyBitPassed;
  //
  // Index into the StrobeToErrorMask structure. 
  //  0 = x8 or any DDR system where things are trained at a bytelane level.
  //  1 = x4 or any system which is trained at a nibble lane level.
  // By using "18" we account for any system with > 18 strobes so this 
  //  should work for SXP memory too.
  //
  DevWidthIndex = MrcData->MaxDq[Channel][Rank] / 18;
  ChannelBackup = Channel;
  CurrentError  = 0;

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  }

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    //
    // Issue a Jedec Reset prior to any test
    //
    if (MrcData->CpgcOptions.RmtRestoreJedecResetType & PIMOVE_JEDEC_RESET) {
      ReInitializeDramsAcrossChannels (MrcData);
    }
    //
    // Fifo reset to avoid glitch the rx fifo during training
    //
    MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  } // Channel loop ...
  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;
  //
  // Perform the CPGC Test.
  //
  CpgcExecuteParallel (MrcData, Channel, Rank);

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }

#if FAULTY_PART_TRACKING
    if (MrcData->FaultyPartTracking) {
      CpgcSCheckErrors (MrcData, Channel, Rank, NULL, NULL, (UINT32 *)&MrcData->MspData.DynamicVars[Channel][BYTEGRP_ERR_STAT]);
    }
#endif
    //
    // Populate the array of PassFail with passing or failing values.
    //
    ConsoleElements = NumberElements[Channel];
    if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_SINGLE_STROBE) {
      //
      // Detect any error across all strobes
      //
      ConsoleElements = MrcData->MaxDq[Channel][Rank];
    }
    for (Knob = 0; Knob < NumberKnobs; Knob++) {
      //
      // Set the limit to search on the CPGC Error registers hi, lo and ecc
      //
      if ((NumberKnobs > 1) && (Knob ==0)) {
        MrcData->CpgcOptions.ChunkErrorMask = CPGC_CHUNK_EVEN;
      } else if ((NumberKnobs > 1) && (Knob ==1)) {
        MrcData->CpgcOptions.ChunkErrorMask = CPGC_CHUNK_ODD;
      } else {
        MrcData->CpgcOptions.ChunkErrorMask = 0xFF;
      }
      BreakStrobeLoop = FALSE;
      for (Strobe = 0; Strobe < ConsoleElements; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, TRUE, &MaxBits)) {
          continue;
        }
        if (MaxBits == 8 || MaxBits == 4 || MaxBits == 1) {
          //
          // Any of these values should be fine
          //
        } else {
          MrcData->ErrorCode = MrcErrInvalidMaxBits;
          MrcDeadLoop ();
        }
        DynamicOffset = StrobeToErrorMask[DevWidthIndex][Strobe].DynamicVar + (2 * Knob);
        DynamicMask   = StrobeToErrorMask[DevWidthIndex][Strobe].ErrorMask;
        DynamicShift  = StrobeToErrorMask[DevWidthIndex][Strobe].OffsetShift;

        if (DynamicOffset >= DYNVAR_MAX || DynamicOffset == 0xFFFF) {
          CheckAdvConstrains (MrcData, "Invalid StrobeToErrorMask", __FUNCTION__, __LINE__);
        }
        //
        // Look at the dynamic variable holding the error data and mask off the appropriate
        // bits for this element (typically a strobe). Record fail if it is non zero.
        //
        CurrentError = MrcData->MspData.DynamicVars[Channel][DynamicOffset] & DynamicMask;
        CurrentError = CurrentError >> DynamicShift;

        BreakBitLoop = FALSE;
        for (IndBit = 0; IndBit < MaxBits; IndBit++) {
          if (((CurrentError & (1 << IndBit)) != 0 && MaxBits > 1) ||
              (CurrentError != 0 && MaxBits == 1)) {
            PassFail[Channel][Knob][Strobe][IndBit][CURRENT_SAMPLE] = RESULTS_FAIL;

            if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_SINGLE_BIT) {
              //
              // When FAIL_SINGLE_BIT is enabled we store the failure
              // result in the bit '0' for each strobe and knob
              //
              PassFail[Channel][Knob][Strobe][0][CURRENT_SAMPLE] = RESULTS_FAIL;
              BreakBitLoop = TRUE;
            }
            if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_SPREAD_BIT_RESULT) {
              for (IndBitTarget = 0; IndBitTarget < MaxBits; IndBitTarget++) {
                //
                // When FAIL_SPREAD_BIT_RESULT is enabled we store the failure
                // result across all bits for each strobe and knob
                //
                PassFail[Channel][Knob][Strobe][IndBitTarget][CURRENT_SAMPLE] = RESULTS_FAIL;
              } // IndBitTarget loop ...
              BreakBitLoop = TRUE;
            }
            if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_SINGLE_STROBE) {
              for (IndStrobeTarget = 0; IndStrobeTarget < MrcData->MaxDq[Channel][Rank]; IndStrobeTarget++) {
                for (IndBitTarget = 0; IndBitTarget < MaxBits; IndBitTarget++) {
                  //
                  // When FAIL_SINGLE_STROBE is enabled we store the failure
                  // result across all bits and all strobes for each knob
                  //
                  PassFail[Channel][Knob][IndStrobeTarget][IndBitTarget][CURRENT_SAMPLE] = RESULTS_FAIL;
                } // IndBitTarget loop ...
              } // IndStrobeTarget loop ...
              BreakStrobeLoop = TRUE;
              BreakBitLoop    = TRUE;
            } // FAIL_SINGLE_STROBE
          } else {
            PassFail[Channel][Knob][Strobe][IndBit][CURRENT_SAMPLE] = RESULTS_PASS;
          }
          if (BreakBitLoop) {
            break;
          }
        } // IndBit loop ...
        
        if ((MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_RESTORE_PI_CENTER_PERBIT) != 0) {
          MrcData->RestoreStrobeMaskEnable = 0;
          AnyBitPassed = FALSE;
          GroupOffset = 0;
          for (IndBitTarget = 0; IndBitTarget < MaxBits; IndBitTarget++) {
            if (PassFail[Channel][Knob][Strobe][IndBitTarget][CURRENT_SAMPLE] !=  RESULTS_FAIL) {
              AnyBitPassed = TRUE;
            }
          } // IndBitTarget Loop

          if (!AnyBitPassed) {
            MspDebugPrint ((MSP_DBG_TST, "Restoring C%dR%dS%d\n", Channel, Rank, Strobe));
            MspDebugPrint ((MSP_DBG_TST, "MaxBits %d\n", MaxBits));
            //
            // MrcData->RestoreStrobeMaskEnable is used to enable the Strobe that needs to be Restored only, it is because all Bits
            // on this Strobe failed. The detection of all Bits failed is checked by AnyBitPassed variable.
            //
            MrcData->RestoreStrobeMaskEnable = (UINT32)(1 << Strobe); 
            for (SignalsToRestore = 0; SignalsToRestore < MrcData->CpgcOptions.RmtRestoreSignalCount; SignalsToRestore++) {
              DelayOffset = GetOffsetIndex(MrcData, MrcData->CpgcOptions.RmtRestoreSignalGroupBegin);
              PrintAndRestoreAfterFailureValues(
                MrcData,
                Channel,
                Rank,
                NumberElements,
                SignalsToRestore,
                DelayOffset,
                GroupOffset,
                &MrcData->CpgcOptions.RmtRestoreSignalValue[SignalsToRestore][Channel][Rank][0],
                " PI"
                );
              GroupOffset += SignalInfo[GetSignalInfoIndex(DelayOffset + GroupOffset)].FrequencyOffset;
            } // SignalsToRestore Loop
          } // AnyBitPassed
        } // FAIL_RESTORE_PI_CENTER_PS

        if (BreakStrobeLoop) {
          break;
        }
      } // Strobe < ConsoleElements loop ...
    } // Knob loop ...

    if (PassFail[Channel][0][0][0][CURRENT_SAMPLE] ==  RESULTS_FAIL) {
      if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_RESTORE_PI_CENTER) {

        MrcData->RestoreStrobeMaskEnable = 0;
        //
        // In this case MrcData->RestoreStrobeMaskEnable is set to enable all Strobes
        // so all Strobe will be Restored at the same time.
        //
        MrcData->RestoreStrobeMaskEnable = (UINT32)((1 << MrcData->MaxDq[Channel][Rank]) - 1);
        GroupOffset = 0;
        for (SignalsToRestore = 0; SignalsToRestore < MrcData->CpgcOptions.RmtRestoreSignalCount; SignalsToRestore++) {
          DelayOffset = GetOffsetIndex (MrcData, MrcData->CpgcOptions.RmtRestoreSignalGroupBegin);
          PrintAndRestoreAfterFailureValues (
            MrcData,
            Channel,
            Rank,
            NumberElements,
            SignalsToRestore,
            DelayOffset,
            GroupOffset,
            &MrcData->CpgcOptions.RmtRestoreSignalValue[SignalsToRestore][Channel][Rank][0],
            " PI"
          );
          GroupOffset += SignalInfo[GetSignalInfoIndex (DelayOffset + GroupOffset)].FrequencyOffset;
        } // SignalsToRestore loop ...

        GroupOffset = 0;
        for (SignalsToRestore = 0; SignalsToRestore < MrcData->CpgcOptions.RmtRestoreSignalCountVref; SignalsToRestore++) {
          DelayOffset = GetOffsetIndex (MrcData, MrcData->CpgcOptions.RmtRestoreSignalGroupBeginVref);
          PrintAndRestoreAfterFailureValues (
            MrcData,
            Channel,
            Rank,
            MrcData->CpgcOptions.NumberOfElementsVref,
            SignalsToRestore,
            DelayOffset,
            GroupOffset,
            &MrcData->CpgcOptions.RmtRestoreSignalValueVref[SignalsToRestore][Channel][Rank][0],
            "VRF"
          );
          GroupOffset += SignalInfo[GetSignalInfoIndex (DelayOffset + GroupOffset)].FrequencyOffset;
        } // SignalsToRestore loop ...
      } // FAIL_RESTORE_PI_CENTER
      if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_JEDEC_RESET) {
        if (MrcData->CpgcOptions.RmtRestoreJedecResetType & FAIL_RESTORE_DDR4_TXVREF) {
          ReInitializeDramsAcrossChannels (MrcData);
        } else {
          ReInitializeDramsAcrossChannels (MrcData);
        }
      } // FAIL_JEDEC_RESET
    } // RESULTS_FAIL
    MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  } // Channel loop ...

  return MMRC_SUCCESS;
}
#endif // TRAINING_ALGOS

#if POWER_TRAINING == 1
/**
  Executes Read Write Training
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReadWriteTraining (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
  if (MrcData->SettingsPowerKnobs.Execute[ExecuteRead]) {
    ReadTraining (MrcData, CapsuleStartIndex, StringIndex, Channel);
  }
  if (MrcData->SettingsPowerKnobs.Execute[ExecuteWrite]) {
    WriteTraining(MrcData, CapsuleStartIndex, StringIndex, Channel);
  }
  return MMRC_SUCCESS;
}

/**
  Check for the contrains of the power knob under test
   
  @param[in, out]  MrcData            Host structure for all data related to MMRC.
  @param[in]       Delta              Margin width for the current iteration.
  @param[in]       PreviousDelta      Margin width for the previous iteration.
  @param[in]       Constrains         Specifies the criteria in order to declare a 
                                      power knob value as good/bad.
  @param[in]  GoToPreviousValidIndex  Returns a true or false in case the current 
                                      iteration broke some constrains, so we indicate 
                                      we need to go back to the previous valid power 
                                      knob iteration.
  @param[in]       Triggers           Pointer to a set of different trigger for 
                                      the algorithm.
  @param[in]       CurrentTrigger     Specifies the current trigger.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CheckConstrains (
  IN  OUT MMRC_DATA         *MrcData, 
  IN      UINT16            Delta,
  IN      UINT16            PreviousDelta,
  IN      CONST UINT8       *Constrains,
  IN      BOOLEAN           *GoToPreviousValidIndex, 
  IN      BOOLEAN           *Triggers,
  IN      UINT8             CurrentTrigger
  )
{
  UINT8   AbsoluteValue;
  UINT8   AbsoluteTriggerValue;
  UINT8   RelativeValue;
  UINT8   ExecuteTrainingDropValue;
  BOOLEAN FirstTimeExecution;

  AbsoluteValue         = Constrains[Absolute];
  AbsoluteTriggerValue  = Constrains[AbsoluteTrigger];
  RelativeValue         = Constrains[Relative];
  ExecuteTrainingDropValue = Constrains[ExecuteTrainingDrop];
  
  FirstTimeExecution    = Triggers[GlobalFirstTimeExecution];

  if (Delta < AbsoluteValue) {
    MspDebugPrint ((MSP_DBG_MIN, "   Delta < %d (ABS)", AbsoluteValue));
    if (FirstTimeExecution) {
      MspDebugPrint ((MSP_DBG_MIN, ":: WARNING LOW MARGIN DETECTED!!!!\n"));
      *GoToPreviousValidIndex = FALSE;
      Triggers[FailureDetectedOnFirstKnob]  = TRUE;
    } else {
      *GoToPreviousValidIndex = TRUE;
    }
  } else if (Delta < AbsoluteTriggerValue) {
    MspDebugPrint ((MSP_DBG_MIN, "   Delta < %d(ABS_TRIGGER[%d])\n", AbsoluteTriggerValue, Triggers[CurrentTrigger]));
    if (Triggers[CurrentTrigger]) {
      if ((PreviousDelta > Delta) && ((PreviousDelta - Delta) >= RelativeValue)) {
        MspDebugPrint ((MSP_DBG_MIN, "   Relative: %d\n", RelativeValue));
        *GoToPreviousValidIndex = TRUE;
      }
    } else {
      Triggers[CurrentTrigger] = TRUE;
    }
  }
  if ((PreviousDelta > Delta) && ((PreviousDelta - Delta) >= ExecuteTrainingDropValue) && !(*GoToPreviousValidIndex)) {
    MspDebugPrint ((MSP_DBG_MIN, "   Exec Training %d/%d\n", Delta, PreviousDelta));
    Triggers[ExecuteTrainingTrigger] = TRUE;
  }
  
  return MMRC_SUCCESS;
}

/**
  Executes the power training sweep according with the input 
  parameters 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       InitIndex       Defines the POR value for the power knob under test.
  @param[in]       MaxIndex        Defines the maximum number of iterations fo powerknob
                                   under test.
  @param[in] PowerTrainingValues   Specifies all the power knobs values for iterations. 
  @param[in] PowerTrainingCriteria Specifies the criteria in order to declare a power knob 
                                   value as good/bad.
  @param[in]       InvalidValue    Value to declare a value in the array as invalid.
  @param[in]       HookSetValue    Function to be executed in order to set the right
                                   power knob value.
  @param[in]       TrainingStep    Function to be executed when re-center is needed 
                                   a register or algorithm in the Elements[] table.
  @param[in] CheckTrainingToExecute Specifies the affected areas for the current power knob.
  @param[in]       SettingString   Pointer to the string of the current power knob.
  @param[in, out]  PowerKnobResult Return the value of the final value of the power knob 
                                   under test
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PowerTrainingSweep (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             InitIndex,
  IN      UINT8             MaxIndex,
  IN      CONST UINT8       *PowerTrainingValues,
  IN      CONST UINT8       *PowerTrainingCriteria,
  IN      UINT8             InvalidValue,
  IN      HOOK_SET_VALUE    HookSetValue,
  IN      TRAINING_STEP     TrainingStep,
  IN      CONST BOOLEAN     *CheckTrainingToExecute,
  IN      UINT8             *SettingString,
  IN  OUT UINT8             *PowerKnobResult
  )
{
  UINT8     Index;
  UINT8     LastIndex;
  BOOLEAN   TriggerLog[MaxTrainingRequired][MaxTriggerLog];
  UINT8     Rank;
  UINT16    MinDeltaT[MaxTrainingRequired];
  UINT16    MinDeltaV[MaxTrainingRequired];
  UINT16    DeltaT[MaxTrainingRequired];
  UINT16    DeltaV[MaxTrainingRequired];
  UINT16    PreviousMinDeltaT[MaxTrainingRequired];
  UINT16    PreviousMinDeltaV[MaxTrainingRequired];
  BOOLEAN   GoToPreviousValidIndex;
  BOOLEAN   ErrorOnFirstKnob;
  UINT8     BackupMrcDebugMsgLevel;
  UINT8     TrainingExecuteStep;
  UINT8     OffsetPowerCriteria;
  
  BackupMrcDebugMsgLevel  = MrcData->MspData.DebugMsgLevel;

  MmrcMemset (PreviousMinDeltaT, 0, MaxTrainingRequired * sizeof (UINT16));
  MmrcMemset (PreviousMinDeltaV, 0, MaxTrainingRequired * sizeof (UINT16));
  MmrcMemset (DeltaT, 0, MaxTrainingRequired * sizeof (UINT16));
  MmrcMemset (DeltaV, 0, MaxTrainingRequired * sizeof (UINT16));

  for (TrainingExecuteStep = 0; TrainingExecuteStep < MaxTrainingRequired; TrainingExecuteStep++) {
    TriggerLog[TrainingExecuteStep][GlobalFirstTimeExecution]   = TRUE;
    TriggerLog[TrainingExecuteStep][TimingBelowTrigger]         = FALSE;
    TriggerLog[TrainingExecuteStep][VoltageBelowTrigger]        = FALSE;
    TriggerLog[TrainingExecuteStep][FailureDetectedOnFirstKnob] = FALSE;
    TriggerLog[TrainingExecuteStep][ExecuteTrainingTrigger]     = FALSE;
  }
  LastIndex = InitIndex;
  for (Index = InitIndex; Index < MaxIndex; Index++) {
    if (PowerTrainingValues[Index] == InvalidValue) {
      continue;
    }
    LastIndex = Index;
    MspDebugPrint ((MSP_DBG_MIN, "Set %a --> %d\n", SettingString, Index));
    HookSetValue (MrcData, Channel, Index);
    MrcData->MspData.DebugMsgLevel = 0;
    SearchRmtWrapper (MrcData, 0, 0, Channel);
    MrcData->MspData.DebugMsgLevel = BackupMrcDebugMsgLevel;

    MmrcMemset (MinDeltaT, 0xff, MaxTrainingRequired * sizeof (UINT16));
    MmrcMemset (MinDeltaV, 0xff, MaxTrainingRequired * sizeof (UINT16));
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }

      DeltaT[ExecuteRead]   = MrcData->RmtData[Channel][Rank][RxDqRight] - MrcData->RmtData[Channel][Rank][RxDqLeft];
      DeltaV[ExecuteRead]   = MrcData->RmtData[Channel][Rank][RxVHigh]   - MrcData->RmtData[Channel][Rank][RxVLow];
      DeltaT[ExecuteWrite]  = MrcData->RmtData[Channel][Rank][TxDqRight] - MrcData->RmtData[Channel][Rank][TxDqLeft];
      DeltaV[ExecuteWrite]  = MrcData->RmtData[Channel][Rank][TxVHigh]   - MrcData->RmtData[Channel][Rank][TxVLow];

      for (TrainingExecuteStep = 0; TrainingExecuteStep < MaxTrainingRequired; TrainingExecuteStep++) {
        if (DeltaT[TrainingExecuteStep] < MinDeltaT[TrainingExecuteStep]) {
          MinDeltaT[TrainingExecuteStep] = DeltaT[TrainingExecuteStep];
        }
        if (DeltaV[TrainingExecuteStep] < MinDeltaV[TrainingExecuteStep]) {
          MinDeltaV[TrainingExecuteStep] = DeltaV[TrainingExecuteStep];
        }
      } // TrainingExecuteStep loop ...
    } //  Rank loop ...

    GoToPreviousValidIndex  = FALSE;
    ErrorOnFirstKnob        = FALSE;
    MmrcMemset (MrcData->SettingsPowerKnobs.Execute, FALSE, MaxTrainingRequired * sizeof (BOOLEAN));
    for (TrainingExecuteStep = 0; TrainingExecuteStep < MaxTrainingRequired; TrainingExecuteStep++) {
      TriggerLog[TrainingExecuteStep][ExecuteTrainingTrigger]   = FALSE;
    }
    //
    // Verify all the constrains look right
    //
    for (TrainingExecuteStep = 0; TrainingExecuteStep < MaxTrainingRequired; TrainingExecuteStep++) {
      if (!CheckTrainingToExecute[TrainingExecuteStep]) {
        continue;
      }

      OffsetPowerCriteria = (MaxMarginVerify * MaxPowerCriteria * TrainingExecuteStep);

      MspDebugPrint ((
        MSP_DBG_MIN, 
        "Step:%d --> T: %d PT: %d V: %d PV:%d\n", 
        TrainingExecuteStep,
        MinDeltaT[TrainingExecuteStep],
        PreviousMinDeltaT[TrainingExecuteStep],
        MinDeltaV[TrainingExecuteStep],
        PreviousMinDeltaV[TrainingExecuteStep]
      ));
      //
      // Check constrains for Timing for Read or Write
      //
      CheckConstrains (
        MrcData, 
        MinDeltaT[TrainingExecuteStep], 
        PreviousMinDeltaT[TrainingExecuteStep],
        &PowerTrainingCriteria[OffsetPowerCriteria + MaxPowerCriteria],
        &GoToPreviousValidIndex, 
        &TriggerLog[TrainingExecuteStep][0],
        TimingBelowTrigger
      );
      //
      // Check constrains for Voltage for Read or Write
      //
      CheckConstrains (
        MrcData, 
        MinDeltaV[TrainingExecuteStep], 
        PreviousMinDeltaV[TrainingExecuteStep],
        &PowerTrainingCriteria[OffsetPowerCriteria],
        &GoToPreviousValidIndex, 
        &TriggerLog[TrainingExecuteStep][0],
        VoltageBelowTrigger
      );
      //
      // Update which training should be re-executed according with the Constrains
      // These parameters are only used when Write & Read Training are both required
      // during the Power Knob under test
      //
      if (TriggerLog[TrainingExecuteStep][ExecuteTrainingTrigger]) {
        MrcData->SettingsPowerKnobs.Execute[TrainingExecuteStep] = TRUE;
      }
      //
      // If an ABSOLUTE error (only First knob error) was detected during Timing
      // or voltage margin, for Reads or Writes it is required stop
      // execution for the next power knob values
      //
      if (TriggerLog[TrainingExecuteStep][FailureDetectedOnFirstKnob]) {
        ErrorOnFirstKnob = TRUE;
      }
      //
      // Update the latest PreviousMinDelta with the new MinDelta
      //
      PreviousMinDeltaT[TrainingExecuteStep] = MinDeltaT[TrainingExecuteStep];
      PreviousMinDeltaV[TrainingExecuteStep] = MinDeltaV[TrainingExecuteStep];

      TriggerLog[TrainingExecuteStep][GlobalFirstTimeExecution] = FALSE;
    } // TrainingExecuteStep loop ...

    if (MrcData->SettingsPowerKnobs.Execute[ExecuteRead] || MrcData->SettingsPowerKnobs.Execute[ExecuteWrite]) {
      MrcData->MspData.DebugMsgLevel = 0;
      TrainingStep (MrcData, 0, 0, Channel);
      MrcData->MspData.DebugMsgLevel = BackupMrcDebugMsgLevel;
    }
    
    if (GoToPreviousValidIndex && !ErrorOnFirstKnob) {
      while (PowerTrainingValues[--Index] == InvalidValue);
      MspDebugPrint ((MSP_DBG_MED, "Previous valid: %a = %d\n", SettingString, Index));
      LastIndex = Index;
      break;
    } else if (ErrorOnFirstKnob) {
      MspDebugPrint ((MSP_DBG_MED, "Failure detected on first knob: %a = %d\n", SettingString, Index));
      break;
    }
  } // Index loop ...
  // 
  // Retrain with the last valid value
  // and save the last Power Knob Index
  // 
  HookSetValue (MrcData, Channel, LastIndex);
  *PowerKnobResult = LastIndex;
  MrcData->MspData.DebugMsgLevel = 0;
  TrainingStep (MrcData, 0, 0, Channel);
  MrcData->MspData.DebugMsgLevel = BackupMrcDebugMsgLevel;

  return MMRC_SUCCESS;
}

/**
  Executes the Power training on SoC Odt

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in, out]  Training        Returns a pointer to the function that should be
                                   executed examined.
  @param[in] CheckTrainingToExecute Pointer to the parameters affected by current power knob.
  @retval          MMRC_SUCCESS     At least one training should 
                                    be executed
  @retval         !MMRC_SUCCESS     No training pointer found
**/
BOOLEAN
DetermineTrainingStep (
  IN  OUT MMRC_DATA         *MrcData,
  IN  OUT TRAINING_STEP     *Training,
  IN      CONST BOOLEAN     *CheckTrainingToExecute
  )
{
  if (CheckTrainingToExecute[ExecuteRead] && CheckTrainingToExecute[ExecuteWrite]) {
    *Training = &ReadWriteTraining;
  } else if (CheckTrainingToExecute[ExecuteRead]) {
    *Training = &ReadTraining;
  } else if (CheckTrainingToExecute[ExecuteWrite]) {
    *Training = &WriteTraining;
  } else {
    return FALSE;
  }
  return TRUE;
}

#endif // POWER_TRAINING

/**
  Executes the Power training on SoC Odt 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          NONE
**/
MMRC_STATUS
SocOdtTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if POWER_TRAINING == 1
  BOOLEAN       FoundTrainingStep;
  UINT8         BackupRmtSupport;
  UINT32        BackupMrcRmtEnableSignal;
  TRAINING_STEP Training;

  if ((MrcData->PowerTrainingEnable & SOC_ODT_KNOB_ENABLE) == 0) {
    return MMRC_SUCCESS;
  }

  BackupRmtSupport                      = MrcData->MrcRmtSupport;
  BackupMrcRmtEnableSignal              = MrcData->MrcRmtEnableSignal;

  MrcData->MrcRmtSupport                = RMT_SUPPORT_ENABLED | RMT_SUPPORT_OVERRIDE;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  MmrcExecuteTask (MrcData, SOC_ODT_TRAINING_ENTRY, NO_PRINT, Channel);

  FoundTrainingStep = DetermineTrainingStep (MrcData, &Training, &TrainingToExecute[PowerDdr3l][SocOdt][0]);

  if (FoundTrainingStep) {

    SetupRmtLevelTraining (
      MrcData, 
      &TrainingToExecute[PowerDdr3l][SocOdt][0]
    );

    PowerTrainingSweep (
      MrcData, 
      Channel, 
      MrcData->SettingsPowerKnobs.InitialValue[PowerDdr3l][SocOdt],
      MaxOdtValues,
      &SocOdtValues[PowerDdr3l][0],
      &CriteriaPowerNobs[SocOdt][0],
      InvalidOdtValue,
      SetSocOdt,
      Training,
      &TrainingToExecute[PowerDdr3l][SocOdt][0],
      "SocOdt",
      &MrcData->SettingsPowerKnobs.FinalValue[PowerDdr3l][SocOdt]
    );
  }

  MrcData->MrcRmtSupport      = BackupRmtSupport;
  MrcData->MrcRmtEnableSignal = BackupMrcRmtEnableSignal;
#endif // POWER_TRAINING
         
  return MMRC_SUCCESS;
}
/**
  Executes CtleTunning for DDR4
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
                                   table to show the register names.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CtleTunning (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  UINT8              Strobe;
  INT32              Bias;
  INT32              Cap;
  UINT32             RmtFlag;
  UINT8              RmtEnFlag;
  INT16              TotalMarginRx[MAX_CHANNELS][MAX_STROBES/2];
  INT32              BestCoef[MAX_CHANNELS][MAX_CHANNELS][MAX_STROBES/2];
  UINT8              Rank;
  UINT8              DebugFlag;

  Strobe             = 0;
  Bias               = 0;
  Cap                = 0;
  Rank               = 0;

  MmrcMemset (TotalMarginRx, 0, MAX_CHANNELS * (MAX_STROBES/2) * sizeof (INT16));
  MmrcMemset (BestCoef, 0, MAX_CHANNELS * MAX_CHANNELS * (MAX_STROBES/2) * sizeof (INT16));
  MmrcMemset (MrcData->RmtMargins, 0xff, MAX_CHANNELS * MAX_RANKS * MAX_STROBES * sizeof (INT16));

  if ((MrcData->MrcParameters.DramType[Channel] != TypeDdr4) || (MrcData->MrcParameters.ChEnabled[Channel]== 0)) {
    return MMRC_SUCCESS;
  }
  DebugFlag = MrcData->MspData.DebugMsgLevel;
  if (!MrcData->RestorePath) {
    RmtFlag = MrcData->MrcRmtEnableSignal;
    RmtEnFlag = MrcData->MrcRmtSupport;
    MrcData->MrcRmtSupport = RMT_SUPPORT_ENABLED;
    MrcData->MrcRmtEnableSignal     = RMT_RX_VREF_EXEC;
    MspDebugPrint ((MSP_DBG_MAX, "Running CTLE Training for CH0%d... \n", Channel));
    MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt+1;
    MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
    MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE_READ;

    MrcData->MspData.DebugMsgLevel = 0;

    for (Cap =0; Cap <4; Cap++) {
      for (Bias = 5; Bias <= 7; Bias++) {
	    for (Rank = 0; Rank < MAX_RANKS; Rank++){
          for (Strobe = 0; Strobe < MAX_STROBES/2; Strobe++) {
            GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Ctle_bias, CMD_SET_VAL_FC_UC, &Bias);
            GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Ctle_cap, CMD_SET_VAL_FC_UC, &Cap);
            GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Ctle_caps, CMD_SET_VAL_FC_UC, &Cap);
          }
	    }
	    SearchRmt (MrcData, Channel);
        for (Rank = 0; Rank < MAX_RANKS; Rank+=2) {
          if (!MrcData->RankEnabled[Channel][Rank]) {
            continue;
          }
	      for (Strobe = 0; Strobe < MAX_STROBES/2; Strobe++) {
	        if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) continue;
	        //
	        //compare RMT margins with Best margin found in previous coefficients
	        //
	        if (TotalMarginRx[Rank/2][Strobe] <= MrcData->RmtMargins[Channel][Rank][Strobe]) {
	          TotalMarginRx[Rank/2][Strobe] = MrcData->RmtMargins[Channel][Rank][Strobe];
	          BestCoef[Rank/2][0][Strobe] = Bias;
	          BestCoef[Rank/2][1][Strobe] = Cap;
	        }
          }
        }
      } //Bias
    } //Cap

    //
    //Take the coefficient of the rank with the lowest margin per strobe (if rank with lowest is present)
    //
    for (Strobe = 0; Strobe < MAX_STROBES/2; Strobe++) {
      if ((TotalMarginRx[0][Strobe] > TotalMarginRx[1][Strobe]) && (MrcData->RankEnabled[Channel][2])) {
        Rank = 1;
      } else if ((TotalMarginRx[1][Strobe] > 0) && (MrcData->RankEnabled[Channel][2]) && (!MrcData->RankEnabled[Channel][0])) {
        Rank = 1;
      } else {
        Rank = 0;
      }
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_bias, CMD_SET_VAL_FC_UC, &BestCoef[Rank][0][Strobe]);
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_cap, CMD_SET_VAL_FC_UC, &BestCoef[Rank][1][Strobe]);
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_caps, CMD_SET_VAL_FC_UC, &BestCoef[Rank][1][Strobe]);
      //
      //Save parameters for save Restore
      //
      MrcData->MrcParameters.SaveRestore.CtleBias[Channel][Strobe] = BestCoef[Rank][0][Strobe];
      MrcData->MrcParameters.SaveRestore.CtleCap[Channel][Strobe] =  BestCoef[Rank][1][Strobe];
    }

    MrcData->MrcRmtSupport = RmtEnFlag;
    MrcData->MrcRmtEnableSignal = RmtFlag;
    //
    //Enable Voc Training at the end of the algorithm
    //
    MrcData->SkipTrainingAlgos[LRxVoc] = FALSE;
  } // !restore path

  if (MrcData->RestorePath) {
    //
    //To handle Ctle Wraper in restore Bias should be = 0 and Cap != 0
    //
    Cap = 1;
    for (Strobe = 0; Strobe < MAX_STROBES/2; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, 0, Strobe, FALSE, NULL)) continue;
      Bias = 0;
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_Wrap, CMD_GET_REG, &Bias);
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_Wrap, CMD_GET_REG, &Cap);
      MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN;
      MspDebugPrint ((MSP_DBG_MIN, "Restored BIAS value for strobe %d = %d  \n", Strobe, Bias));
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_bias, CMD_SET_VAL_FC_UC, &Bias);
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_cap, CMD_SET_VAL_FC_UC, &Cap);
      GetSetDdrioGroup2(MrcData, Channel, 0, Strobe, Ctle_caps, CMD_SET_VAL_FC_UC, &Cap);
    }
  }
  MrcData->MspData.DebugMsgLevel = DebugFlag;

  return MMRC_SUCCESS;
}

/**
  Executes Read Training 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          NONE
**/
MMRC_STATUS
ReadTraining (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if READ_TRAINING == 1 && TRAINING_ALGOS == 1
#if NEW_RTWT_ALGO == 1
  UINT8   ch;
  UINT8   ChannelBackup;

  if (MrcData->SkipTrainingAlgos[LReadTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }
  ChannelBackup = Channel;
  //
  // Before beginning the algo or restoring the trained values, call the entry function to setup the 
  // system.
  //
  #ifdef READTRAINING_ENTRY
  for (ch = 0; ch < MAX_CHANNELS; ch++) {
    if (MrcData->MrcParameters.ChEnabled[ch] == TRUE) {
      MmrcExecuteTask(MrcData, READTRAINING_ENTRY, NO_PRINT, ch);
    }
  }
  #endif
  // 
  // If on the full boot path (not restore path), setup CPGC for Write training
  // and call the training algorithm, otherwise restore the passed in values.
  //
  MspDebugPrint ((MSP_DBG_MAX, "Read Training Loopcount = %d, numBursts = %d MT: %d\n", MrcData->MrcTrainingCpgcExpLoopCnt+1, MrcData->MrcCpgcNumBursts, MrcData->MaxThresholdPerBit));
  if (!MrcData->RestorePath) {
    MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt+1;
    MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
    MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE_READ;
    for (ch = 0; ch < MAX_CHANNELS; ch++) {
      //
      // STOP_ON_ERR_CONTROL set to 0 for advanced read and write
      //
      MrcData->MspData.DynamicVars[ch][ADVANCED_RD_WR] = 1;
      if (MrcData->MrcParameters.ChEnabled[ch] == TRUE) {
        CpgcSetup(MrcData, ch, CPGC_CONFIG_VA);
        MrcData->CpgcOptions.Rank       = 0;          
        CpgcReconfig (MrcData, ch);
      }
    }

#if defined (READ_TRAININ_PN) && READ_TRAININ_PN == 1

    AdvancedTrainingAlgo (MrcData, ControlKnobsReadPN, NumberControlKnobsReadPN, EyeMaskRead, NumberEyeMaskRead, MrcData->ReadPerBitEnable);

#else
    AdvancedTrainingAlgo (MrcData, ControlKnobsRead, NumberControlKnobsRead, EyeMaskRead, NumberEyeMaskRead, MrcData->ReadPerBitEnable);
#endif
    Channel = ChannelBackup;
#if RDQS_ODDEVENSUPPORT==1
    AverageGroups (MrcData, Channel, RxDqsNDelay, TRUE);
#else
    AverageGroups (MrcData, Channel, RxDqsDelay, TRUE);
#endif
  } //  !RestorePath
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResultsRxDqsWrapper(MrcData, Channel, TRUE);
  //
  // End with the exit task for write training.
  //
#ifdef READTRAINING_EXIT
   MmrcExecuteTask (MrcData, READTRAINING_EXIT, NO_PRINT, Channel);
#endif
   if (MrcData->MemoryPreservation && ((MrcData->MrcParameters.BootMode == S0) || (MrcData->MrcParameters.BootMode == ADR))) {
     //
     // Skip memory test
     //
     MspDebugPrint ((MSP_DBG_MIN, "Skip memory check (Memory Preservation)\n"));
     return MMRC_SUCCESS;
   }

   if (TestMrcTrainingSteps (MrcData, ChannelBackup, CPGC_CONFIG_VA, CPGC_CMDSEQ_WRITE_READ, 0, 8, 6) != 0) {
     MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
   }
#else // NEW_RTWT_ALGO
  UINT8   RxdqsKnobs;
  UINT16  RxdqsIndex;
  UINT8   RxdqsFlags;
  UINT8   Rank;
  UINT8   ChannelBackup;

  RxdqsKnobs = 0;
  RxdqsIndex = 0;
  ChannelBackup = Channel;

  if (MrcData->SkipTrainingAlgos[LReadTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

  MrcData->CpgcOptions.RmtRestoreJedecResetType     = GetJedecResetTypeForRd ();

#if FAULTY_PART_TRACKING
  FaultyPartTrackInit (MrcData, Channel, FALSE);
#endif
  //
  // ReadTraining Entry 
  //
#ifdef READTRAINING_ENTRY
  MmrcExecuteTask (MrcData, READTRAINING_ENTRY, NO_PRINT, Channel);
#endif
  if (!MrcData->RestorePath) {
#if CPGC_BASED_READWRITES == 1
    MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt;
    MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_VA);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
#endif
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->MspData.ExecuteThisRoutineInParallel) {
        MspDebugPrint ((MSP_DBG_MED, "Read CxxR%d\n", Rank));
      } else {
        MspDebugPrint ((MSP_DBG_MED, "Read C%02dR%d\n", Channel, Rank));
      }
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
#if CPGC_BASED_READWRITES == 1
        //
        // Setup the CPGC engine to to do a single read from an address within the 
        // selectable rank.  The engine should be setup for LFSR mode.
        //
        MrcData->CpgcOptions.Rank = Rank;
        MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE_READ;
        CpgcReconfig (MrcData, Channel);
#endif 
      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      //
      // If Odd/Even is supported for the RDQS, then the number of knobs is set to 2 with the initial dim index pointing to the first
      // element.
      //
#if RDQS_ODDEVENSUPPORT==1
      RxdqsKnobs = 2;
      RxdqsIndex = RxDqsPDelay;
      RxdqsFlags = ABSOLUTE | MULTIKNOB;
#else
      RxdqsKnobs = 1;
      RxdqsIndex = RxDqsDelay;
      RxdqsFlags = ABSOLUTE;
#endif
      SweepCenterEye2D (MrcData, Channel, Rank, RxDqsDelay, 0,
        GetHalfClk (MrcData, Channel), 
        1, ABSOLUTE, RxVref, RX_VREF_MIN, RX_VREF_MAX, 2, ABSOLUTE | START_SWEEP_CENTER);
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;      
    } // Rank loop ...
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;    
#if RDQS_ODDEVENSUPPORT==1
    AverageGroups (MrcData, Channel, RxDqsNDelay, TRUE);
#else
    AverageGroups (MrcData, Channel, RxDqsDelay, TRUE);
#endif
    AverageGroups (MrcData, Channel, RxVref, FALSE);
  } // Restore Path
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResultsRxDqsWrapper(MrcData, Channel, TRUE);
  //
  // ReadTraining Exit
  //
#ifdef READTRAINING_EXIT
  MmrcExecuteTask (MrcData, READTRAINING_EXIT, NO_PRINT, Channel);
#endif
  if (TestMrcTrainingSteps (MrcData, ChannelBackup, CPGC_CONFIG_VA, CPGC_CMDSEQ_WRITE_READ, 0, 8, 6) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#endif // NEW_READ_TRAINING_ALGO
#endif // READ_TRAINING
  return MMRC_SUCCESS;
}

/**
  Executes the Power training on Dram Rtt Nom and Rtt Wr 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.    
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DimmOdtTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if POWER_TRAINING == 1
  BOOLEAN       FoundTrainingStep;
  UINT8         BackupRmtSupport;
  UINT32        BackupMrcRmtEnableSignal;
  UINT8         MaxOdtValuesForRttWr;
  TRAINING_STEP Training;

  BackupRmtSupport                      = MrcData->MrcRmtSupport;
  BackupMrcRmtEnableSignal              = MrcData->MrcRmtEnableSignal;

  MrcData->MrcRmtSupport                = RMT_SUPPORT_ENABLED | RMT_SUPPORT_OVERRIDE;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  FoundTrainingStep = DetermineTrainingStep (MrcData, &Training, &TrainingToExecute[PowerDdr3l][DramRttNomOdt][0]);

  if (FoundTrainingStep && ((MrcData->PowerTrainingEnable & DIMM_RTTNOM_KNOB_ENABLE) != 0)) {
    SetupRmtLevelTraining (
      MrcData, 
      &TrainingToExecute[PowerDdr3l][DramRttNomOdt][0]
    );
    PowerTrainingSweep (
      MrcData, 
      Channel, 
      MrcData->SettingsPowerKnobs.InitialValue[PowerDdr3l][DramRttNomOdt],
      MaxOdtValues,
      &DimmRttNomValues[PowerDdr3l][0],
      &CriteriaPowerNobs[DramRttNomOdt][0],
      InvalidOdtValue,
      SetDramOdtRttNom,
      Training,
      &TrainingToExecute[PowerDdr3l][DramRttNomOdt][0],
      "Dram RttNom Odt",
      &MrcData->SettingsPowerKnobs.FinalValue[PowerDdr3l][DramRttNomOdt]
    );
  }

  FoundTrainingStep = DetermineTrainingStep (MrcData, &Training, &TrainingToExecute[PowerDdr3l][DramRttWrOdt][0]);

  if (FoundTrainingStep && ((MrcData->PowerTrainingEnable & DIMM_RTTWR_KNOB_ENABLE) != 0)) {

    SetupRmtLevelTraining (
      MrcData, 
      &TrainingToExecute[PowerDdr3l][DramRttWrOdt][0]
    );

    if (MrcData->SettingsPowerKnobs.FinalValue[PowerDdr3l][DramRttNomOdt] == OdtInfinite) {
      MaxOdtValuesForRttWr = OdtInfinite - 1;
    } else {
      MaxOdtValuesForRttWr = OdtInfinite;
    }

    PowerTrainingSweep (
      MrcData, 
      Channel, 
      MrcData->SettingsPowerKnobs.InitialValue[PowerDdr3l][DramRttWrOdt],
      MaxOdtValuesForRttWr,
      &DimmRttWrValues[PowerDdr3l][0],
      &CriteriaPowerNobs[DramRttWrOdt][0],
      InvalidOdtValue,
      SetDramOdtRttWr,
      Training,
      &TrainingToExecute[PowerDdr3l][DramRttWrOdt][0],
      "Dram RttWr Odt",
      &MrcData->SettingsPowerKnobs.FinalValue[PowerDdr3l][DramRttWrOdt]
    );
  }

  MrcData->MrcRmtSupport      = BackupRmtSupport;
  MrcData->MrcRmtEnableSignal = BackupMrcRmtEnableSignal;
#endif // POWER_TRAINING
         
  return MMRC_SUCCESS;
}
/**
  Executes TxeqTunning for DDR4
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
TxeqTunning (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel
  )
{
  INT32              TxeqCoef;
  BOOLEAN            LowMargin;
  UINT32             RmtFlag;
  UINT8              RmtEnFlag;
  INT16              TotalMarginTx[MAX_CHANNELS];
  UINT8              Rank;
  UINT8              DebugFlag;

  TxeqCoef           = 1;
  Rank               = 0;
  LowMargin          = FALSE;
  MmrcMemset (TotalMarginTx, 0, MAX_CHANNELS * sizeof (INT16));

  if ((MrcData->MrcParameters.DramType[Channel] != TypeDdr4) || (MrcData->MrcParameters.ChEnabled[Channel]== 0)) {
    return MMRC_SUCCESS;
  }

  RmtFlag = MrcData->MrcRmtEnableSignal;
  RmtEnFlag = MrcData->MrcRmtSupport;
  MrcData->MrcRmtSupport = RMT_SUPPORT_ENABLED;
  MrcData->MrcRmtEnableSignal     = RMT_TX_VREF_EXEC;
  MspDebugPrint ((MSP_DBG_MAX, "Running RMT for TXEQ Feature... \n"));
  MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt+2;
  MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
  DebugFlag = MrcData->MspData.DebugMsgLevel;
  MrcData->MspData.DebugMsgLevel = MSP_DBG_NONE;
  SearchRmt(MrcData, Channel);

  for (Rank = 0; Rank < MAX_RANKS; Rank+=2) {
    if (!MrcData->RankEnabled[Channel][Rank]) {
      continue;
    }
    //
    //Save TxDqDelay2 and TxVref to compare later in loop for TXEQ coefficient
    //
    TotalMarginTx[Rank/2] = MrcData->RmtData[Channel][Rank][RmtTable[RmtTxVref].HighSide] - MrcData->RmtData[Channel][Rank][RmtTable[RmtTxVref].LowSide];
  }

  do {
    //
    //Get margins for next coefficient
    //
    GetSetDdrioGroup2(MrcData, Channel, 0, 0, TxEqualization, CMD_SET_VAL_FC_UC, &TxeqCoef);
    SearchRmt(MrcData, Channel);
    for (Rank = 0; Rank < MAX_RANKS; Rank+=2) {
      if ((!MrcData->RankEnabled[Channel][Rank]) || (LowMargin == TRUE)) {
        continue;
      }
      //
      //Save TxDqDelay2 AND TxDqDelay2 coeff
      //
      if (TotalMarginTx[Rank/2] > MrcData->RmtData[Channel][Rank][RmtTable[5].HighSide] - MrcData->RmtData[Channel][Rank][RmtTable[5].LowSide]) {
        LowMargin = TRUE;
        TxeqCoef--;
        continue;
      } else {
        TotalMarginTx[Rank/2] = MrcData->RmtData[Channel][Rank][RmtTable[5].HighSide] - MrcData->RmtData[Channel][Rank][RmtTable[5].LowSide];

      }
    }
    if (LowMargin == FALSE) {
      TxeqCoef++;
    }
  } while ((TxeqCoef <=8) && (LowMargin == FALSE));

  GetSetDdrioGroup2(MrcData, Channel, 0, 0, TxEqualization, CMD_SET_VAL_FC_UC, &TxeqCoef);
  MrcData->MrcRmtSupport = RmtEnFlag;
  MrcData->MrcRmtEnableSignal = RmtFlag;
  MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN;
  MspDebugPrint ((MSP_DBG_MIN, "TXEQ Final Value = %d \n", TxeqCoef-1));
  MrcData->MspData.DebugMsgLevel = DebugFlag;

  return MMRC_SUCCESS;
}

/**
  Executes Advanced Write Training  
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
WriteTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if WRITE_TRAINING == 1 && TRAINING_ALGOS == 1
#if NEW_RTWT_ALGO == 1
  UINT8 ch;
#ifdef SIM
  UINT8  rk;
  UINT8  st;
  UINT32 TempValue;
#endif
  UINT8   ChannelBackup;
#if defined(NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT==0
  UINT8   Rank;
  UINT8   Strobe;
  UINT32  Temp;
#endif
  UINT32 Txeq;
  if (MrcData->SkipTrainingAlgos[LWriteTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }
  Txeq = 0;
  ChannelBackup = Channel;
  //
  // Before beginning the algo or restoring the trained values, call the entry function to setup the system.
  //
#ifdef WRITETRAINING_ENTRY
  for (ch = 0; ch < MAX_CHANNELS; ch++) {
    if (MrcData->MrcParameters.ChEnabled[ch] == TRUE) {
      MmrcExecuteTask (MrcData, WRITETRAINING_ENTRY, NO_PRINT, ch);
    }
  }
#endif
  // 
  // If on the full boot path (not restore path), setup CPGC for Write training
  // and call the training algorithm, otherwise restore the passed in values.
  //
  if (!MrcData->RestorePath) {
  //
  //Will be enabled for B0 PO. Still under test
  //
  TxeqTunning(MrcData, Channel);
#ifdef SIM
    MrcData->CpgcOptions.NumBursts = 6;
    MrcData->CpgcOptions.LoopCount = 1;
#else
    MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt;
    MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
#endif
    MspDebugPrint ((MSP_DBG_MAX, "Write Training Loopcount = %d, numBursts = %d\n", MrcData->MrcTrainingCpgcExpLoopCnt, MrcData->MrcCpgcNumBursts));
    for (ch = 0; ch < MAX_CHANNELS; ch++) {
      //
      // STOP_ON_ERR_CONTROL set to 0 for advanced read and write
      //
      MrcData->MspData.DynamicVars[ch][ADVANCED_RD_WR] = 1;
      if (MrcData->MrcParameters.ChEnabled[ch] == TRUE) {
        CpgcSetup(MrcData, ch, CPGC_CONFIG_VA);
        MrcData->CpgcOptions.Rank = 0;
        CpgcReconfig (MrcData, ch);
      }
    }

#if defined SIM
    for (ch = 0; ch < MAX_CHANNELS; ch++) {
      if (MrcData->MrcParameters.ChEnabled[ch] == FALSE) continue;
      for (rk = 0; rk < MAX_RANKS; rk++) {
        if (!RunOnThisChannel(MrcData, ch, rk)) continue;
        for (st = 0; st < MAX_STROBES; st++) {
          if (!IsStrobeValid(MrcData, ch, rk, st, FALSE, NULL)) continue;
          GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDelay2, CMD_GET_REG_UC, &TempValue);
          GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDrvDelay2, CMD_GET_REG_UC, &TempValue);
        }
      }
    }
#endif

    MrcData->PdaEnable = FALSE;
    //
    // Perform the Advanced WriteTraining Algorithm with the Write Control knobs.
    //
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      MspDebugPrint ((MSP_DBG_MAX, "ControlKnobsWriteDdr4\n"));
      AdvancedTrainingAlgo(MrcData, ControlKnobsWrite, NumberControlKnobsWrite, EyeMaskWrite, NumberEyeMaskWrite, MrcData->WritePerBitEnable);
    } else {
      MspDebugPrint ((MSP_DBG_MAX, "ControlKnobsWriteDdr3\n"));
      AdvancedTrainingAlgo(MrcData, ControlKnobsWriteDdr3, NumberControlKnobsWrite, EyeMaskWrite, NumberEyeMaskWrite, MrcData->WritePerBitEnable);
    }

    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
#if defined(NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT==0
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
       if (!RunOnThisChannel (MrcData, Channel, Rank)) {
         continue;
       }
       for (Strobe = 0; Strobe < MAX_STROBE_X8_DEVICES; Strobe ++) {
         if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
           continue;
         }
         GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxVref, CMD_GET_REG_UC, &Temp);
         GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxVref, CMD_SET_VAL_FC_UC, &Temp);
       }
      }
    }
#endif
    Channel = ChannelBackup;
    //
    // This will keep coherency for the register values in the cache
    //
    BlastFinalResults (MrcData, Channel, TxDqDelay, 0);
    BlastFinalResults (MrcData, Channel, TxDqDrvDelay, 0);

    AverageGroups (MrcData, Channel, TxDqDelay,    FALSE);
#ifdef TxDqDrvDelay
    AverageGroups (MrcData, Channel, TxDqDrvDelay, FALSE);
#endif
  } //  !RestorePath
  //
  //Will be enabled for B0 PO. Still under test
  //

  if (MrcData->RestorePath) {
    GetSetDdrioGroup2(MrcData, Channel, 0, 0, TxEqualization, CMD_GET_REG, &Txeq);
    GetSetDdrioGroup2(MrcData, Channel, 0, 0, TxEqualization, CMD_SET_VAL_FC_UC, &Txeq);
    MspDebugPrint ((MSP_DBG_MIN, "TxEQ Coefficient after restoring = %d \n", Txeq));
  }

  MrcData->PdaEnable = TRUE;
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqDelay, TRUE);
#ifdef TxDqDrvDelay
  DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDrvDelay, FALSE);
#endif
  if (MrcData->WritePerBitEnable) {
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqBitDelay, TRUE);
  }
  //
  // End with the exit task for write training.
  //
#ifdef WRITETRAINING_EXIT
  MmrcExecuteTask (MrcData, WRITETRAINING_EXIT, NO_PRINT, Channel);
#endif
  WriteTrainingExit(MrcData, Channel);

     if (MrcData->MemoryPreservation && ((MrcData->MrcParameters.BootMode == S0) || (MrcData->MrcParameters.BootMode == ADR))) {
    //
    // Skip memory test
    //
    MspDebugPrint ((MSP_DBG_MIN, "Skip memory check (Memory Preservation)\n"));
    return MMRC_SUCCESS;
  }
  if (TestMrcTrainingSteps (MrcData, ChannelBackup, CPGC_CONFIG_VA, CPGC_CMDSEQ_WRITE_READ, 0, 8, 6) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#else // NEW_RTWT_ALGO

  UINT8   Rank;
  UINT32  TempValue;
  UINT8   Strobe;
  UINT8   ChannelBackup;

  ChannelBackup = Channel;

  if (MrcData->SkipTrainingAlgos[LWriteTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

  MrcData->CpgcOptions.RmtRestoreJedecResetType     = GetJedecResetTypeForWr ();

#if FAULTY_PART_TRACKING
  FaultyPartTrackInit (MrcData, Channel, FALSE);
#endif
  TempValue = 0;
  Strobe = 0;
  //
  // WriteTraining Entry
  //
#ifdef WRITETRAINING_ENTRY
  MmrcExecuteTask (MrcData, WRITETRAINING_ENTRY, NO_PRINT, Channel);
#endif
  if (!MrcData->RestorePath) {
#if CPGC_BASED_READWRITES == 1
    MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt;
    MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_VA);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
#endif
#if TX_DQ_MINUS1_SUPPORT
    HandleMinus1Select (MrcData, Channel, TxDqDelay, TxDqMinus1);
#endif // TX_DQ_MINUS1_SUPPORT

    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxDqDelay, CMD_GET_REG_UC, &TempValue);
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
          if (MrcData->MaxDq[Channel][Rank] <= 9) {
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe + 9, TxDqDelay, CMD_GET_REG_UC, &TempValue);
          }
#endif
        } // Strobe loop ...
      } // Channel loop ...
    } // Rank loop ...
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->MspData.ExecuteThisRoutineInParallel) {
        MspDebugPrint ((MSP_DBG_MED, "Write CxxR%d\n", Rank));
      } else {
        MspDebugPrint ((MSP_DBG_MED, "Write C%02dR%d\n", Channel, Rank));
      }
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
#if CPGC_BASED_READWRITES == 1
        //
        // Setup the CPGC engine to to do a single read from an address within the 
        // selectable rank.  The engine should be setup for LFSR mode.
        //
        MrcData->CpgcOptions.Rank = Rank;
        MrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE_READ;
        CpgcReconfig (MrcData, Channel);
#endif 
      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      //
      // Sweep a full 1X clock.
      //
      SweepCenterEye2D (
        MrcData, 
        Channel, 
        Rank, 
        TxDqDelayGExt,
        -1 * GetQtrClk (MrcData, Channel),
        3 * GetQtrClk (MrcData, Channel), 
        1, 
        RELATIVE, 
        TxVref, 
        TX_VREF_MIN, 
        TX_VREF_MAX, 
        2,
        ABSOLUTE | START_SWEEP_CENTER | SWEEP_ALL_STROBES_TOGETHER
      );
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
    } // Rank loop ...

    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;

    AverageGroups (MrcData, Channel, TxDqDelay,    FALSE);
#ifdef TxDqDrvDelay
    AverageGroups (MrcData, Channel, TxDqDrvDelay, FALSE);
#endif

#if TX_DQ_MINUS1_SUPPORT == 1
    SetMinus1Select (MrcData, Channel, TxDqDelay, TxDqMinus1);
#endif // TX_DQ_MINUS1_SUPPORT
  }
  //
  // Print out or restore the training results.
  //
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqDelay, TRUE);
  if (MrcData->WritePerBitEnable) {
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqBitDelay, TRUE);
  }
  //
  // WriteTraining Exit
  //
#ifdef WRITETRAINING_EXIT
  MmrcExecuteTask (MrcData, WRITETRAINING_EXIT, NO_PRINT, Channel);
#endif
  WriteTrainingExit(MrcData, Channel);
  if (TestMrcTrainingSteps (MrcData, ChannelBackup, CPGC_CONFIG_VA, CPGC_CMDSEQ_WRITE_READ, 0, 8, 6) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#endif // NEW_RTWT_ALGO
#endif // READ_WRITE_TRAINING
  return MMRC_SUCCESS;
}

MMRC_STATUS
GetLowHighMarginPerStrobe (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN   UINT32       CurrentValue[MAX_STROBES],
  IN   UINT16       DelaySignal,
  IN   UINT8        RmtLowDataOffset,
  IN   UINT8        RmtHighDataOffset
)
{
  UINT8   Strobe;
  INT16   StrobeLowMargin;
  INT16   StrobeHighMargin;
  INT16   RmtMinLeftMargin;
  INT16   RmtMinRightMargin;

  RmtMinLeftMargin  = -32767;
  RmtMinRightMargin = +32767;

#if RMT_DEBUG 
  MspDebugPrint ((MSP_DBG_DEBUG, "RMT_DEBUG\n"));
  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    MspDebugPrint ((MSP_DBG_DEBUG, " ----S%02d----  ", Strobe));
  }
  MspDebugPrint ((MSP_DBG_DEBUG, "\n"));
#endif
  //
  // Look for the worst margin
  //
  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
    if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }

    StrobeLowMargin   = (INT16) Results[Channel][0][Strobe][0][LOW]  - (INT16) CurrentValue[Strobe]; 
    if (StrobeLowMargin > RmtMinLeftMargin) {
      RmtMinLeftMargin  = StrobeLowMargin;
    }

    StrobeHighMargin  = (INT16) Results[Channel][0][Strobe][0][HIGH] - (INT16) CurrentValue[Strobe];
    if (StrobeHighMargin < RmtMinRightMargin) {
      RmtMinRightMargin = StrobeHighMargin;
    }
    MrcData->RmtMargins[Channel][Rank][Strobe] = StrobeHighMargin - StrobeLowMargin;
#if RMT_DEBUG
    MspDebugPrint ((MSP_DBG_DEBUG, " %04d %02d:%02d  ", CurrentValue[Strobe], StrobeLowMargin, StrobeHighMargin));
#endif

    if ((MrcData->MrcParameters.DramType[Channel] == TypeDdr4) && (DelaySignal == TxVref)) {
       //
       // Skipping BreakStrobeLoop when calculating DDR4 TxVref results.
       //
    } else {
      BreakStrobeLoop (MrcData, DelaySignal, &Strobe);
    }
  }// Strobe loop
#if RMT_DEBUG
  MspDebugPrint ((MSP_DBG_DEBUG, "\n"));
  MspDebugPrint ((MSP_DBG_DEBUG, "Worst case: %02d : %02d\n", RmtMinLeftMargin, RmtMinRightMargin));
#endif
  MrcData->RmtData[Channel][Rank][RmtHighDataOffset] = RmtMinRightMargin;
  MrcData->RmtData[Channel][Rank][RmtLowDataOffset]  = RmtMinLeftMargin;

  return MMRC_SUCCESS;
}

UINT16
GetIndexBasedOnFastBootParameter (
  UINT8   inputParameter
)
{
  switch (inputParameter) {
  case MrcFastBootRxNDqs:
    return RxDqsNDelay;
    break;
  case MrcFastBootRxPDqs:
    return RxDqsPDelay;
    break;
  case MrcFastBootRxVref:
    return RxVref;
    break;
  case MrcFastBootTxDq:
    return TxDqDelayG;
    break;
  case MrcFastBootTxVref:
    return TxVref;
    break;
  default:
    return MMRC_NA;
  }
}

VOID
RestoreFastBootParameters (
  MMRC_DATA   *MrcData,
  UINT8       Channel,
  UINT8       Rank,
  UINT8       inputParameter
)
{
  UINT8   Strobe;
  UINT16  Dim1Index;

  Dim1Index = GetIndexBasedOnFastBootParameter (inputParameter);

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    GetSetDdrioGroup2 (
      MrcData,
      Channel, Rank, Strobe,
      Dim1Index,
      CMD_SET_VAL_FC_UC,
      &MrcData->CpgcOptions.RmtRestoreSignalValue[inputParameter][Channel][Rank][Strobe]
    );
  }
}

/**
  Check basic margins during fast boot and S5

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FastBootChecker (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  INT8    minLowTxVref  = 0;
  INT8    maxHighTxVref = 0;
  INT8    marginLow[MaxMrcFastBootKnobs]   = {-128, -128, -128, -128, -128};
  INT8    marginHigh[MaxMrcFastBootKnobs]  = { 127,  127,  127,  127,  127};
  UINT8   Rank;
  UINT8   Strobe;
  UINT8   fastBootParameter;
  UINT8   endLoop, nibbleLoop;
  UINT8   marginFinal;
  UINT8   msgBackup;
  UINT8   Elements[MAX_CHANNELS];
  INT16   lowOffset;
  INT16   highOffset;
  UINT16  dim1Index;
  UINT16  Low[MaxMrcFastBootKnobs][MAX_CHANNELS][MAX_STROBES];
  UINT16  High[MaxMrcFastBootKnobs][MAX_CHANNELS][MAX_STROBES];
  UINT16  Results[MaxMrcFastBootKnobs][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT16  Dim1StartPoint[MaxMrcFastBootKnobs][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT32  centerPoint;
  UINT32  avgTxVref = 0;              // Across all Strobes

  if (MrcData->MrcFastBootChecker.EnableFastBootTester == 0) {
    return MMRC_SUCCESS;
  }

  msgBackup = MrcData->MspData.DebugMsgLevel;
  MrcData->MspData.DebugMsgLevel = 0;

  MrcData->MspData.DynamicVars[Channel][ADVANCED_RD_WR] = 1;

  CpgcSetup(MrcData, Channel, CPGC_CONFIG_VA);
  MrcData->CpgcOptions.CmdSequence                    = CPGC_CMDSEQ_WRITE_READ;
  MrcData->CpgcOptions.LoopCount                      = MrcData->MrcRmtCpgcExpLoopCnt;
  MrcData->CpgcOptions.NumBursts                      = MrcData->MrcRmtCpgcNumBursts;
  MrcData->CpgcOptions.RmtRestoreSignalGroupBeginVref = MMRC_NA;
  MrcData->CpgcOptions.RmtRestoreSignalCountVref      = 0;
  MrcData->CpgcOptions.RmtRestoreJedecResetType       = NO_ACTION;
  MrcData->CpgcOptions.RmtRestoreSignalGroupBegin     = MMRC_NA;
  MrcData->CpgcOptions.RmtRestoreSignalCount          = 0;

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    // Clear Results prior to start sweeping the Rank
    MmrcMemset (Results, 0xff, MaxMrcFastBootKnobs *MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * MAX_BITS *2 * sizeof (UINT16));
    avgTxVref     = 0;
    minLowTxVref  = 0;
    maxHighTxVref = 0;

    MrcData->CpgcOptions.Rank = Rank;
    CpgcReconfig (MrcData, Channel);

    //
    // Disable Hook for RxDqs, RxVref & TxVref
    //
    DnvNibbleTrainingExitHook(MrcData, 0, 0, Channel);

    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank,Strobe, FALSE, NULL)) {
        continue;
      }
      //
      // Backup RxDqs Delay Values for N & P
      //
      GetSetDdrioGroup2 (
        MrcData,
        Channel, Rank, Strobe,
        RxDqsNDelay,
        CMD_GET_REG,
        &MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxNDqs][Channel][Rank][Strobe]
      );
      GetSetDdrioGroup2 (
        MrcData,
        Channel, Rank, Strobe,
        RxDqsPDelay,
        CMD_GET_REG,
        &MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxPDqs][Channel][Rank][Strobe]
      );
      MrcData->RxDqsTetherVal[Strobe]  = (INT8)(MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxNDqs][Channel][Rank][Strobe] - MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxPDqs][Channel][Rank][Strobe]);
      //
      // Backup Rx Vref
      //
      GetSetDdrioGroup2 (
        MrcData,
        Channel, Rank, Strobe,
        RxVref,
        CMD_GET_REG,
        &MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxVref][Channel][Rank][Strobe]
      );
      //
      // Backup TxDq Delay
      //
      GetSetDdrioGroup2 (
        MrcData,
        Channel, Rank, Strobe,
        TxDqDelay2,
        CMD_GET_REG,
        &MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxDq][Channel][Rank][Strobe]
      );
      if ((MrcData->MaxDqCache[Channel][Rank] == MAX_STROBE_X8_DEVICES) && (Strobe < MAX_STROBE_X8_DEVICES)) {
        // In case we are operating in x8 read the second nibble for TxDq
        GetSetDdrioGroup2 (
          MrcData,
          Channel, Rank, Strobe + MAX_STROBE_X8_DEVICES,
          TxDqDelay2,
          CMD_GET_REG,
          &MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxDq][Channel][Rank][Strobe + MAX_STROBE_X8_DEVICES]
        );
      }
      //
      // Backup Tx Vref per Strobe PDA must be enabled
      //
      MrcData->PdaEnable = TRUE;
      GetSetDdrioGroup2 (
        MrcData,
        Channel, Rank, Strobe,
        TxVref,
        CMD_GET_REG,
        &MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxVref][Channel][Rank][Strobe]
      );
      //
      // Do an average for TxVref
      //
      avgTxVref += MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxVref][Channel][Rank][Strobe];

      MspDebugPrint ((
        MSP_DBG_TST, "C%dR%dS%d N:%d P:%d D:%d RV:%d Tn0:%d",
        Channel, Rank, Strobe,
        MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxNDqs][Channel][Rank][Strobe],
        MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxPDqs][Channel][Rank][Strobe],
        MrcData->RxDqsTetherVal[Strobe],
        MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootRxVref][Channel][Rank][Strobe],
        MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxDq][Channel][Rank][Strobe]
      ));
      if ((MrcData->MaxDqCache[Channel][Rank] == MAX_STROBE_X8_DEVICES) && (Strobe < MAX_STROBE_X8_DEVICES)) {
        MspDebugPrint ((
          MSP_DBG_TST, "/Tn1:%d",
          MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxDq][Channel][Rank][Strobe + MAX_STROBE_X8_DEVICES]
        ));
      }
      MspDebugPrint ((
        MSP_DBG_TST, " TV:%d TV(acc):%d\n",
        MrcData->CpgcOptions.RmtRestoreSignalValue[MrcFastBootTxVref][Channel][Rank][Strobe],
        avgTxVref
      ));
    } // Strobe loop...
    //
    // Calculate the avg for TxVref
    //
    avgTxVref /= MrcData->MaxDqCache[Channel][Rank];
    MspDebugPrint ((
      MSP_DBG_TST, " TV(avg):%d\n",
      avgTxVref
    ));

    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank,Strobe, FALSE, NULL)) {
        continue;
      }
      for (fastBootParameter = 0; fastBootParameter < MaxMrcFastBootKnobs; fastBootParameter++) {
        // Skip MrcFastBootRxNDqs, we should be working only with MrcFastBootRxPDqs
        if (fastBootParameter == MrcFastBootRxNDqs) continue;

        dim1Index = GetIndexBasedOnFastBootParameter(fastBootParameter);
        if (MrcData->MaxDq[Channel][Rank] == MAX_STROBE_X8_DEVICES &&
            fastBootParameter == MrcFastBootTxDq) {
          endLoop = 2;
        } else {
          endLoop = 1;
        }
        for (nibbleLoop = 0; nibbleLoop < endLoop; nibbleLoop++) {
          if (Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES) >= MAX_STROBES) {
            CheckAdvConstrains (MrcData, "Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES) >= MAX_STROBES", __FUNCTION__, __LINE__);
            continue;
          }
          if (MrcData->MrcParameters.SaveRestore.MrcMarginChecker.LowMargin[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)]  != 0 &&
              MrcData->MrcParameters.SaveRestore.MrcMarginChecker.HighMargin[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)] != 0) {
            lowOffset   = MrcData->MrcParameters.SaveRestore.MrcMarginChecker.LowMargin[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)];
            highOffset  = MrcData->MrcParameters.SaveRestore.MrcMarginChecker.HighMargin[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)];
          } else {
            if (fastBootParameter == MrcFastBootTxVref) {
              lowOffset   = 0;
              highOffset  = 0;
            } else {
              lowOffset   = 0 - (MrcData->MrcFastBootChecker.Warning.Margin[fastBootParameter] / 2);
              highOffset  = (MrcData->MrcFastBootChecker.Warning.Margin[fastBootParameter] / 2);
            }
          }

          Dim1StartPoint[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][LOW]  = 0;
          Dim1StartPoint[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][HIGH] = (UINT16) SignalUpperBound(MrcData, Channel, Rank, dim1Index);
          if (fastBootParameter == MrcFastBootTxVref) {
            centerPoint = avgTxVref;
          } else {
            centerPoint = MrcData->CpgcOptions.RmtRestoreSignalValue[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)];
          }
          // Determine the starting point for the Low Side
          if ((INT32)(centerPoint + lowOffset) > 0) {
            Dim1StartPoint[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][LOW]  = (UINT16) (centerPoint + lowOffset);
          }
          // Determine the starting point for the High Side
          if ((INT32)(centerPoint + highOffset) < SignalUpperBound(MrcData, Channel, Rank, dim1Index)) {
            Dim1StartPoint[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][HIGH] = (UINT16) (centerPoint + highOffset);
          }
          Low[fastBootParameter][Channel][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)]  = 0;
          High[fastBootParameter][Channel][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)] = (UINT16) SignalUpperBound (MrcData, Channel, Rank, dim1Index);
          MspDebugPrint ((
            MSP_DBG_TST, "[%d] DS[L]:%d DS[H]:%d L:%d H:%d\n",
            fastBootParameter,
            Dim1StartPoint[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][LOW],
            Dim1StartPoint[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][HIGH],
            Low[fastBootParameter][Channel][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)],
            High[fastBootParameter][Channel][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)]
          ));
        } // nibbleLoop ... ...
      } // fastBootParameter loop ...
   } // Strobe loop...

    Elements[0] = MrcData->MaxDq[0][Rank];
    Elements[1] = MrcData->MaxDq[1][Rank];
    // RxDqs
    Create1DSweep (
      MrcData,
      Channel, Rank,
      RxDqsTether, 1,
      Dim1StartPoint[MrcFastBootRxPDqs], Low[MrcFastBootRxPDqs], High[MrcFastBootRxPDqs],
      1,
      FALSE, FALSE, FALSE,
      Elements,
      CpgcExecuteRmtTest,
      Results[MrcFastBootRxPDqs],
      "Rx ",
      1,
      RUN_PER_STROBE
    );
    RestoreFastBootParameters (MrcData, Channel, Rank, MrcFastBootRxNDqs);
    RestoreFastBootParameters (MrcData, Channel, Rank, MrcFastBootRxPDqs);

    //
    // Rx Vref always operates at a Byte Level (x8)
    //
    MrcData->MaxDq[Channel][Rank]   = MAX_STROBE_X8_DEVICES;
    MrcData->MaxBit[Channel][Rank]  = STROBE_BITS_X8_DEVICES;
    Elements[0] = MrcData->MaxDq[0][Rank];
    Elements[1] = MrcData->MaxDq[1][Rank];
    Create1DSweep (
      MrcData,
      Channel, Rank,
      RxVref, 1,
      Dim1StartPoint[MrcFastBootRxVref], Low[MrcFastBootRxVref], High[MrcFastBootRxVref],
      1,
      FALSE, FALSE, FALSE,
      Elements,
      CpgcExecuteRmtTest,
      Results[MrcFastBootRxVref],
      "RxV",
      1,
      RUN_PER_STROBE
    );
    RestoreFastBootParameters (MrcData, Channel, Rank, MrcFastBootRxVref);
    // As RxVref is per Byte Level copy the results to the 2nd Nibble only if x4 Dimm
    for (Strobe = 0; Strobe < MAX_STROBES && MrcData->MaxDqCache[Channel][Rank] == MAX_STROBE_X4_DEVICES; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank,Strobe, FALSE, NULL)) {
        continue;
      }
      if (Strobe + MAX_STROBE_X8_DEVICES >= MAX_STROBES) {
        CheckAdvConstrains (MrcData, "Strobe + MAX_STROBE_X8_DEVICES >= MAX_STROBES", __FUNCTION__, __LINE__);
        continue;
      }
      MspDebugPrint ((
        MSP_DBG_TST, "Copying from S%d.[B7:B4]-->S%d.[B3:B0]\n",
        Strobe, Strobe + MAX_STROBE_X8_DEVICES
      ));
      //
      // Copy from S0.[B7:B0] -> S9.[B7:B0], mainly B0 which is the one with the PER_STROBE result
      //
      MmrcMemCopy (
        (UINT8 *)&Results[MrcFastBootRxVref][Channel][0][Strobe + MAX_STROBE_X8_DEVICES][0][0],
        (UINT8 *)&Results[MrcFastBootRxVref][Channel][0][Strobe][0][0],
        MAX_BITS * sizeof(UINT16) * 2
      );
    } // Strobe loop ...
    //
    // Enable Hook for TxDq
    //
    DnvNibbleTrainingHook(MrcData, 0, 0, Channel);
    Elements[0] = MrcData->MaxDq[0][Rank];
    Elements[1] = MrcData->MaxDq[1][Rank];
    // TxDq
    Create1DSweep (
      MrcData,
      Channel, Rank,
      TxDqDelayG, 1,
      Dim1StartPoint[MrcFastBootTxDq], Low[MrcFastBootTxDq], High[MrcFastBootTxDq],
      1,
      FALSE, FALSE, FALSE,
      Elements,
      CpgcExecuteRmtTest,
      Results[MrcFastBootTxDq],
      "Tx ",
      1,
      RUN_PER_STROBE
    );
    RestoreFastBootParameters (MrcData, Channel, Rank, MrcFastBootTxDq);

    //
    // Disable Hook for RxDqs, RxVref & TxVref
    //
    DnvNibbleTrainingExitHook(MrcData, 0, 0, Channel);
    Elements[0] = MrcData->MaxDq[0][Rank];
    Elements[1] = MrcData->MaxDq[1][Rank];
    // It will run in Lock Mode
    MrcData->PdaEnable = 0;
    // TxVref
    Create1DSweep (
      MrcData,
      Channel, Rank,
      TxVref, 1,
      Dim1StartPoint[MrcFastBootTxVref], Low[MrcFastBootTxVref], High[MrcFastBootTxVref],
      1,
      FALSE, FALSE, FALSE,
      Elements,
      CpgcExecuteRmtTest,
      Results[MrcFastBootTxVref],
      "TxV",
      1,
      RUN_PER_STROBE_LOCK
    );
    MrcData->PdaEnable = 1;
    RestoreFastBootParameters (MrcData, Channel, Rank, MrcFastBootTxVref);

    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank,Strobe, FALSE, NULL)) {
        continue;
      }
      MspDebugPrint ((
        MSP_DBG_TST, "C%dR%dS%d\n",
        Channel, Rank, Strobe
      ));
      for (fastBootParameter = 0; fastBootParameter < MaxMrcFastBootKnobs; fastBootParameter++) {
        // Skip MrcFastBootRxNDqs, we should be working only with MrcFastBootRxPDqs
        if (fastBootParameter == MrcFastBootRxNDqs) continue;

        if (MrcData->MaxDq[Channel][Rank] == MAX_STROBE_X8_DEVICES &&
            fastBootParameter == MrcFastBootTxDq) {
          endLoop = 2;
        } else {
          endLoop = 1;
        }
        for (nibbleLoop = 0; nibbleLoop < endLoop; nibbleLoop++) {
          if (Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES) >= MAX_STROBES) {
            CheckAdvConstrains (MrcData, "Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES) >= MAX_STROBES", __FUNCTION__, __LINE__);
            continue;
          }
          MspDebugPrint ((
            MSP_DBG_TST, "[%d] R[L]:%d R[H]:%d R:%d\n",
            fastBootParameter,
            Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW],
            Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH],
            Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] - Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW]
          ));

          centerPoint = MrcData->CpgcOptions.RmtRestoreSignalValue[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)];

          if (Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] < Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH]) {
            if (Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] < centerPoint) {
              // Keeps track per Strobe/Rank/Channel
              MrcData->MrcParameters.SaveRestore.MrcMarginChecker.LowMargin[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)] = (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] - centerPoint);
              // For TxVref keep track of the min value in the low side (most negative), relatively to avgTxVref
              if (fastBootParameter == MrcFastBootTxVref) {
                if (minLowTxVref >
                   (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] - avgTxVref)) {
                  minLowTxVref = (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] - avgTxVref);
                }
              }
              // Keeps track across all ranks and strobes
              if ((INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] - centerPoint) > marginLow[fastBootParameter]) {
                marginLow[fastBootParameter] = (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][LOW] - centerPoint);
              }
            }
            if (Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] > centerPoint) {
              // Keeps track per Strobe/Rank/Channel
              MrcData->MrcParameters.SaveRestore.MrcMarginChecker.HighMargin[fastBootParameter][Channel][Rank][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)] = (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] - centerPoint);
              // Keeps track across all ranks and strobes
              if ((INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] - centerPoint) < marginHigh[fastBootParameter]) {
                marginHigh[fastBootParameter] = (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] - centerPoint);
              }
              // For TxVref keep track of the max value in the high side (most positive), relatively to avgTxVref
              if (fastBootParameter == MrcFastBootTxVref) {
                if (maxHighTxVref <
                    (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] - avgTxVref)) {
                  maxHighTxVref = (INT8)(Results[fastBootParameter][Channel][0][Strobe + (nibbleLoop * MAX_STROBE_X8_DEVICES)][0][HIGH] - avgTxVref);
                }
              }
            }
          } // nibbleLoop ...
          MspDebugPrint ((
            MSP_DBG_TST, "    M[L]:%d M[H]:%d\n",
            marginLow[fastBootParameter],
            marginHigh[fastBootParameter]
          ));
        }
      } // fastBootParameter loop ...
    } // Strobe loop ...

    MspDebugPrint ((
      MSP_DBG_TST, "C%dR%d: Overwriting TxVref L:%d H:%d\n",
      Channel, Rank,
      minLowTxVref,
      maxHighTxVref
    ));

    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank,Strobe, FALSE, NULL)) {
        continue;
      }
      MrcData->MrcParameters.SaveRestore.MrcMarginChecker.LowMargin[MrcFastBootTxVref][Channel][Rank][Strobe]   = minLowTxVref;
      MrcData->MrcParameters.SaveRestore.MrcMarginChecker.HighMargin[MrcFastBootTxVref][Channel][Rank][Strobe]  = maxHighTxVref;
    }
  } // Rank loop ...

  //
  // Determine the Pass, Warning and Critical levels across all knobs
  //
  MrcData->MspData.DebugMsgLevel = msgBackup;
  for (fastBootParameter = 0; fastBootParameter < MaxMrcFastBootKnobs; fastBootParameter++) {
    // Skip MrcFastBootRxNDqs, we should be working only with MrcFastBootRxPDqs
    if (fastBootParameter == MrcFastBootRxNDqs) continue;
    MspDebugPrint ((
      MSP_DBG_MAX, "[%d] mL:%d mH:%d\n",
      fastBootParameter,
      marginLow[fastBootParameter], marginHigh[fastBootParameter]
    ));
    marginFinal = marginHigh[fastBootParameter] - marginLow[fastBootParameter];
    if (marginFinal < MrcData->MrcFastBootChecker.Critical.Margin[fastBootParameter] &&
        MrcData->MrcFastBootChecker.CriteriaTestResult < CriteriaCritical) {
      MrcData->MrcFastBootChecker.CriteriaTestResult = CriteriaCritical;
      MspDebugPrint ((
        MSP_DBG_MAX, "[%d] MF: %d C:%d\n",
        fastBootParameter,
        marginFinal,
        MrcData->MrcFastBootChecker.Critical.Margin[fastBootParameter]
      ));
    } else if (marginFinal < MrcData->MrcFastBootChecker.Warning.Margin[fastBootParameter] &&
        MrcData->MrcFastBootChecker.CriteriaTestResult < CriteriaWarning) {
      MrcData->MrcFastBootChecker.CriteriaTestResult = CriteriaWarning;
      MspDebugPrint ((
        MSP_DBG_MAX, "[%d] MF: %d W:%d\n",
        fastBootParameter,
        marginFinal,
        MrcData->MrcFastBootChecker.Warning.Margin[fastBootParameter]
      ));
    }
    MspDebugPrint ((
      MSP_DBG_MAX, "[%d] Criteria Test Result: %d\n",
      fastBootParameter,
      MrcData->MrcFastBootChecker.CriteriaTestResult
    ));
  }

  return MMRC_SUCCESS;
}

/**
  Executes Rank Margin Tool Setup
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SearchRmtWrapper (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
  BOOLEAN   BackupEnable;

  BackupEnable = MrcData->MspData.Enabled[0];
  MrcData->CpgcOptions.LoopCount = MrcData->MrcRmtCpgcExpLoopCnt;
  MrcData->CpgcOptions.NumBursts = MrcData->MrcRmtCpgcNumBursts;

  if (MrcData->MrcRmtSupport & RMT_SUPPORT_OVERRIDE) {
    MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt;
    MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
  }
  
  if (MrcData->MrcRmtSupport & RMT_SUPPORT_COMMAND_MODE) {
    while (1) {
      MspDebugPrint ((MSP_DBG_MIN, "RMT COMMAND: SKPD0{RMT_COMMAND} and SKPD1{RMT_VALUE} specified always in CH0\n"));
      MrcData->MspData.Enabled[0] = TRUE;
      MmrcExecuteTask (MrcData, RMT_CLEAR_REGISTERS, NO_PRINT, 0);
      do {
        MmrcExecuteTask (MrcData, RMT_GET_CMD_AND_VALUE, NO_PRINT, 0);
      } while (MrcData->MspData.DynamicVars[0][RMT_COMMAND] == 0);
      //
      // Read again the values of the command and value
      //
      MmrcExecuteTask (MrcData, RMT_GET_CMD_AND_VALUE, NO_PRINT, 0);
      MrcData->MspData.Enabled[0] = BackupEnable;
      switch (MrcData->MspData.DynamicVars[0][RMT_COMMAND]) {
      case RmtCommandSetLoopCount:
        MrcData->CpgcOptions.LoopCount = (UINT8) MrcData->MspData.DynamicVars[0][RMT_VALUE];
        MspDebugPrint ((MSP_DBG_MIN, "New loopcount = %03d\n", MrcData->CpgcOptions.LoopCount));
        break;
      case RmtCommandSetNumBursts:
        MrcData->CpgcOptions.NumBursts = (UINT8) MrcData->MspData.DynamicVars[0][RMT_VALUE];
        MspDebugPrint ((MSP_DBG_MIN, "New numburst = %03d\n", MrcData->CpgcOptions.NumBursts));
        break;
      case RmtCommandExit:
        return MMRC_SUCCESS;
        break;
      case RmtCommandRun:
        SearchRmt (MrcData, Channel);
        break;
      case RmtCommandEnableSignal:
        MrcData->MrcRmtEnableSignal   = (UINT32) MrcData->MspData.DynamicVars[0][RMT_VALUE];
        MspDebugPrint ((MSP_DBG_MIN, "New MrcRmtEnableSignal = 0x%08x\n", MrcData->MrcRmtEnableSignal));
        break;
      default:
        break;
      }
    } // while (1) loop ...
  }

  return SearchRmt (MrcData, Channel);
}

/**
  Executes Rank Margin Tool
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.

  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
SearchRmt (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel
  )
{
#if RMT == 1 && TRAINING_ALGOS == 1
  UINT8   i;
  UINT8   Rank;
  UINT8   Strobe;

  UINT8   Limit[MAX_CHANNELS];
  UINT16  Element;
  UINT8   ChannelBackup;
  BOOLEAN ChannelTested[MAX_CHANNELS];
  
  UINT16  Low[MAX_CHANNELS][MAX_STROBES];
  UINT16  High[MAX_CHANNELS][MAX_STROBES];
  UINT32  TempValue[MAX_CHANNELS][MAX_STROBES];
  UINT32  RestoreValue[MAX_CHANNELS][MAX_STROBES][MAX_CMDS];
  UINT16  Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT16  Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT8   DebugMsgLevel;
  UINT16  SignalGroupOffset;
  UINT16  SignalTrainingDelay;
  UINT8   NumSignalGroups;
  UINT8   SignalGroupCount;
  UINT16  GroupId;
  UINT16  SignalOffsetIndex;
  UINT8   MarginRunType;

  UINT16  DimmStartAvg;
  UINT8   DimmStartElements;

  if ((MrcData->MrcRmtSupport & RMT_SUPPORT_ENABLED) == 0) {
    return MMRC_SUCCESS;
  }
  //
  // Adding a ReInitializeDramsAcrossChannels since the last training stage
  // before RMT could have been CMD training, therefore after CMD training
  // it is always neededing a ReInitializeDramsAcrossChannels.
  //
  ReInitializeDramsAcrossChannels(MrcData);

  ChannelBackup = Channel;
  DimmStartAvg = 0;
  DimmStartElements = 0;
  SignalTrainingDelay = 0;
  NumSignalGroups = 0;
  GroupId = 0;

  MmrcMemset (ChannelTested, FALSE, MAX_CHANNELS * sizeof (BOOLEAN));

  DebugMsgLevel = MrcData->MspData.DebugMsgLevel;
  MmrcMemset (Limit, 0, MAX_CHANNELS * sizeof (UINT8));
  
  // 
  // Skip if MRC Debug Message is disabled to expedite boot time
  //
  if (MrcData->MrcRmtSupport & RMT_SUPPORT_ENABLED) {
    //
    // Turn off debug messages for RMT training steps.
    //
    MspDebugPrint ((MSP_DBG_MIN, "Loopcount = %02d: NumBurst = %02d\n", MrcData->CpgcOptions.LoopCount, MrcData->CpgcOptions.NumBursts));
    MrcData->MspData.DebugMsgLevel = MSP_DBG_NONE;
    for (Rank = 0; Rank < MAX_RANKS; Rank ++) {
      MmrcMemset (Results, 0xff, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * MAX_BITS *2 * sizeof (UINT16));

      for (i = 0; i < (sizeof (RmtTable) / sizeof (RMT_SIGNAL)); i++) {
        if (!(MrcData->MrcRmtEnableSignal & (1 << i))) {
          continue;
        }
        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          ChannelTested[Channel] = TRUE;
#if CPGC_BASED_READWRITES == 1
          MrcData->MspData.DynamicVars[Channel][ADVANCED_RD_WR] = 1;
          CpgcSetup (MrcData, Channel, RmtTable[i].CpgcConfig);
          //
          // Setup the CPGC engine to to do a single read from an address within the 
          // selectable rank.  The engine should be setup for VA mode or CADB_MODE
          //
          MrcData->CpgcOptions.Rank         = Rank;
          MrcData->CpgcOptions.CmdSequence  = CPGC_CMDSEQ_WRITE_READ;
          CpgcReconfig (MrcData, Channel);
          MrcData->MspData.DynamicVars[Channel][ADVANCED_RD_WR] = 0;
#endif 
        } // Channel loop ...

        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          if ((RmtTable[i].Signal == RxVref) && (MrcData->MaxDqCache[Channel][Rank] == MAX_STROBE_X4_DEVICES)) {
            if (MrcData->MrcParameters.EccEnabled[Channel] == TRUE) {
              Limit[Channel] = MAX_STROBE_X8_DEVICES;
            } else {
              Limit[Channel] = MAX_STROBE_X8_DEVICES - 1;
            }
          } else {
            if (RmtTable[i].NumElements == STROBE) {
              Limit[Channel] = MrcData->MaxDq[Channel][Rank];
            } else {
              Limit[Channel] = RmtTable[i].NumElements;
            }
          }
          
          SignalTrainingDelay  = GetOffsetIndex (MrcData, RmtTable[i].Signal);
          NumSignalGroups = SignalInfo[GetSignalInfoIndex (SignalTrainingDelay)].NumSignalGroups;
          for (Strobe = 0; Strobe < Limit[Channel]; Strobe++) { 
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            //
            // Saving Register values to be restored after RMT completes.
            //

            SignalGroupOffset    = 0;
            SignalGroupCount      = 0;

            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RmtTable[i].Signal, CMD_GET_REG, &TempValue[Channel][Strobe]);
            RestoreValue[Channel][Strobe][SignalGroupCount] = TempValue[Channel][Strobe];
            MspDebugPrint((MSP_DBG_DEBUG, "Debugging before 1DSweep RestoreValue[%d][%d][%d] = 0x%08x\n", Channel, Strobe, SignalGroupCount, RestoreValue[Channel][Strobe][SignalGroupCount]));

            //
            // Check below enables RMT to run TxDq per nibble when there is a x8 device width.
            // When there is a x4 device width the Limit[Channel] loop will take care of it.
            //
            if ((MrcData->MaxBit[Channel][Rank] == STROBE_BITS_X8_DEVICES) && ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_DQ_EXEC)) {
              GetSetDdrioGroup2(MrcData, Channel, Rank, (Strobe + MAX_STROBE_X8_DEVICES), RmtTable[i].Signal, CMD_GET_REG, &TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)]);
              RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount] = TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)];
              MspDebugPrint((MSP_DBG_DEBUG, "Debugging before 1DSweep RestoreValueDevicesx8[%d][%d][%d] = 0x%08x\n", Channel, (Strobe + MAX_STROBE_X8_DEVICES), SignalGroupCount, RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount])); 
            }
            //
            // Following Register access for RMT_CMD_EXEC.
            //
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, SignalTrainingDelay + SignalGroupOffset, CMD_GET_REG, &MrcData->CpgcOptions.RmtRestoreSignalValue[SignalGroupCount][Channel][Rank][Strobe]);

            Low[Channel][Strobe]  = 0;
            High[Channel][Strobe] = (UINT16) SignalUpperBound (MrcData, Channel, Rank, RmtTable[i].Signal);

            //
            // Saving register values if RMT test is TxVref or RxVref to calculate an average
            // so 1DSweep can start from the same value on all strobes since TxVref do not stay
            // in illegal values and RxVref values are shared.
            //
            if (((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_VREF_EXEC) || ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_RX_VREF_EXEC)) {
              DimmStartAvg += (UINT16)TempValue[Channel][Strobe];
              DimmStartElements++;
            } else {
              Dim1StartPoint[Channel][0][Strobe][LOW]  = (UINT16) TempValue[Channel][Strobe];
              Dim1StartPoint[Channel][0][Strobe][HIGH] = Dim1StartPoint[Channel][0][Strobe][LOW];
            }

            MspDebugPrint((MSP_DBG_DEBUG, "Debugging NumSignalGroups = %d\n", NumSignalGroups));
            if (RmtTable[i].RunByGroup == RUN_GROUP) {
              for (SignalGroupCount = 1; SignalGroupCount < NumSignalGroups; SignalGroupCount++) {
                //
                // Saving the values for SIGNAL_GRP['x']
                //
                
                SignalGroupOffset += SignalInfo[GetSignalInfoIndex (SignalTrainingDelay + SignalGroupOffset)].FrequencyOffset;
                GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, SignalTrainingDelay + SignalGroupOffset, CMD_GET_REG, &TempValue[Channel][Strobe]);
                RestoreValue[Channel][Strobe][SignalGroupCount] = TempValue[Channel][Strobe];
                MspDebugPrint((MSP_DBG_DEBUG, "Debugging before 1DSweep RestoreValue[%d][%d][%d] = 0x%08x\n", Channel, Strobe, SignalGroupCount, RestoreValue[Channel][Strobe][SignalGroupCount]));
                //
                // Following Register access for RMT_CMD_EXEC.
                //
                GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, SignalTrainingDelay + SignalGroupOffset, CMD_GET_REG, &MrcData->CpgcOptions.RmtRestoreSignalValue[SignalGroupCount][Channel][Rank][Strobe]);

                //
                // For Signals running by group type.
                // Check below enables RMT to run TxDq per nibble when there is a x8 device width.
                // When there is a x4 device width the Limit[Channel] loop will take care of it.
                //
                if ((MrcData->MaxBit[Channel][Rank] == STROBE_BITS_X8_DEVICES) && ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_DQ_EXEC)) {
                  GetSetDdrioGroup2(MrcData, Channel, Rank, (Strobe + MAX_STROBE_X8_DEVICES), RmtTable[i].Signal, CMD_GET_REG, &TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)]);
                  RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount] = TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)];
                  MspDebugPrint((MSP_DBG_DEBUG, "Debugging before 1DSweep RestoreValueDevicesx8[%d][%d][%d] = 0x%08x\n", Channel,(Strobe + MAX_STROBE_X8_DEVICES), SignalGroupCount, RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount]));
                  GetSetDdrioGroup2 (MrcData, Channel, Rank, (Strobe + MAX_STROBE_X8_DEVICES), SignalTrainingDelay + SignalGroupOffset, CMD_GET_REG, &MrcData->CpgcOptions.RmtRestoreSignalValue[SignalGroupCount][Channel][Rank][(Strobe + MAX_STROBE_X8_DEVICES)]);
                }
              } // SignalGroupOffset loop ...
            } // RmtTable[i].RunByGroup check ...
          } // Strobe loop ...
          //
          // Setting Average Values to DimStart to run 1DSweep for TxVref and RxVref
          // since those values are shared.
          //
          if (((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_VREF_EXEC) || ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_RX_VREF_EXEC)) {
            if (DimmStartElements > 0) {
              DimmStartAvg = DimmStartAvg / DimmStartElements;
            } else {
              MspDebugPrint((MSP_DBG_MIN, "ERROR! improper value for DimmStartElements.\n"));
              MrcDeadLoop();
            }
            for (Strobe = 0; Strobe < Limit[Channel]; Strobe++) {
              if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
                continue;
              }
              Dim1StartPoint[Channel][0][Strobe][LOW] = (UINT16)DimmStartAvg;
              Dim1StartPoint[Channel][0][Strobe][HIGH] = Dim1StartPoint[Channel][0][Strobe][LOW]; 
            }
            DimmStartAvg = 0;
            DimmStartElements = 0;
          }
        } // Channel loop ...
        //
        // Rmt settings for Jedec reset
        //
        MrcData->CpgcOptions.RmtRestoreJedecResetType     = RmtTable[i].JedecResetType;
        MrcData->CpgcOptions.RmtRestoreSignalGroupBegin   = RmtTable[i].Signal;
        //
        // Selecting the Group ID from RmtTable 
        // in case RmtSearch runs by group.
        //
        if (RmtTable[i].RunByGroup == RUN_GROUP) {
          GroupId = RmtTable[i].GroupId;
        }
        //
        // RMT Signal running.
        //
        MspDebugPrint((MSP_DBG_MIN, "%s\n", SignalInfo[GetSignalInfoIndex(SignalTrainingDelay)].Name));

        MrcData->CpgcOptions.RmtRestoreSignalCount        = 0;

        MrcData->CpgcOptions.RmtRestoreSignalGroupBeginVref   = MMRC_NA;
        for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
          MrcData->CpgcOptions.NumberOfElementsVref[Channel]  = 0;
        }
        MrcData->CpgcOptions.RmtRestoreSignalCountVref        = 0;

        //
        // RMT Run Type Selection.
        //
        if (RmtTable[i].MarginPerBit) {
          MarginRunType = RUN_PER_BIT;
        } else {
          MarginRunType = RUN_PER_STROBE;
        }

        //
        // Check if RMT test is TxVref or RxVref
        // if so, disable PDA mode (TxVref only),
        // before running 1DSweep on Lock_Step mode.
        //
        if ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_VREF_EXEC) {
          MrcData->PdaEnable = FALSE;
          MarginRunType |= RUN_LOCK_STEP_MODE;
        } else if (((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_RX_VREF_EXEC) || ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_CMD_EXEC)) {
          MarginRunType &= ~RUN_LOCK_STEP_MODE;
        }
        //
        // Restore back the Channel Under Test
        //
        Channel = ChannelBackup;
        if (RmtTable[i].RunByGroup == RUN_GROUP) {
          MrcData->CpgcOptions.RmtRestoreSignalCount      = NumSignalGroups;
          Create1DSweep (MrcData, Channel, Rank, GroupId, 1, Dim1StartPoint, Low, High, RmtTable[i].StepSize, FALSE, TRUE, RmtTable[i].MarginPerBit, Limit, CpgcExecuteRmtTest, Results, "", 1, MarginRunType);
        } else {
          MrcData->CpgcOptions.RmtRestoreSignalCount      = 1;
          Create1DSweep(MrcData, Channel, Rank, RmtTable[i].Signal, 1, Dim1StartPoint, Low, High, RmtTable[i].StepSize, FALSE, FALSE, RmtTable[i].MarginPerBit, Limit, CpgcExecuteRmtTest, Results, "", 1, MarginRunType);
        }

        //
        // Check if RMT test is TxVref
        // if so, enable PDA mode after
        // running 1DSweep.
        //
        if ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_VREF_EXEC) {
          MrcData->PdaEnable = TRUE;
        }

        for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) {
            continue;
          }
          for (Strobe = 0; Strobe < Limit[Channel]; Strobe++) {
            if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            TempValue[Channel][Strobe] = (UINT32) RestoreValue[Channel][Strobe][0];
          }
          GetLowHighMarginPerStrobe (MrcData, Channel, Rank, Results, TempValue[Channel], RmtTable[i].Signal, RmtTable[i].LowSide, RmtTable[i].HighSide);
#if 0
          if (RmtTable[i].MarginPerBit) {
            if (DebugMsgLevel != MSP_DBG_NONE) {
        	  MrcData->MspData.DebugMsgLevel |= MSP_DBG_MIN;
            }
            MspDebugPrint((MSP_DBG_MIN, "START_RMT_PER_BIT_REPORT\n"));
            MspDebugPrint((MSP_DBG_MIN, "C%d.R%d\n", Channel, Rank));
            PrintRmtSummaryPerBit (MrcData, Channel, Rank, Limit, 1, Dim1StartPoint, RmtTable[i].StepSize, Results, TRUE);
            MspDebugPrint((MSP_DBG_MIN, "END_RMT_PER_BIT_REPORT\n"));
            MrcData->MspData.DebugMsgLevel &= MSP_DBG_MIN;
          }
#endif

          SignalTrainingDelay  = GetOffsetIndex (MrcData, RmtTable[i].Signal);
          NumSignalGroups = SignalInfo[GetSignalInfoIndex (SignalTrainingDelay)].NumSignalGroups;
          for (Strobe = 0; Strobe < Limit[Channel]; Strobe++) {
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            //
            // Restore Register values previously saved before RMT execution.
            //
            SignalGroupOffset    = 0;
            SignalGroupCount     = 0;
            TempValue[Channel][Strobe] = RestoreValue[Channel][Strobe][SignalGroupCount];
            MspDebugPrint((MSP_DBG_DEBUG, "Debugging RestoreValue[%d][%d][%d] = 0x%08x\n", Channel, Strobe, SignalGroupCount, RestoreValue[Channel][Strobe][SignalGroupCount]));
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RmtTable[i].Signal, CMD_SET_VAL_FC_UC, &TempValue[Channel][Strobe]);

            //
            // Check below enables RMT to run TxDq per nibble when there is a x8 device width.
            // When there is a x4 device width the Limit[Channel] loop will take care of it.
            //
            if ((MrcData->MaxBit[Channel][Rank] == STROBE_BITS_X8_DEVICES) && ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_DQ_EXEC)) {
              TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)] = RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount];
              MspDebugPrint((MSP_DBG_DEBUG, "Debugging RestoreValueDevicesx8[%d][%d][%d] = 0x%08x\n", Channel, (Strobe + MAX_STROBE_X8_DEVICES), SignalGroupCount, RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount]));
              GetSetDdrioGroup2(MrcData, Channel, Rank, (Strobe + MAX_STROBE_X8_DEVICES), RmtTable[i].Signal, CMD_SET_VAL_FC_UC, &TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)]); 
            }

            if (RmtTable[i].RunByGroup == RUN_GROUP) {
              for (SignalGroupCount = 1; SignalGroupCount < NumSignalGroups; SignalGroupCount++) {
                //
                // Restore the values for SIGNAL_GRP['x'], previously saved.
                //
                MspDebugPrint((MSP_DBG_DEBUG, "Debugging when restoring values the number of CommandGroupCount = %d\n", SignalGroupCount));
                SignalGroupOffset += SignalInfo[GetSignalInfoIndex (SignalTrainingDelay + SignalGroupOffset)].FrequencyOffset;
                TempValue[Channel][Strobe] = RestoreValue[Channel][Strobe][SignalGroupCount];
                MspDebugPrint((MSP_DBG_DEBUG, "Debugging TempValue[%d][%d] = 0x%08x\n", Channel, Strobe, TempValue[Channel][Strobe]));
                MspDebugPrint((MSP_DBG_DEBUG, "Debugging RestoreValue[%d][%d][%d] = 0x%08x\n", Channel, Strobe, SignalGroupCount, RestoreValue[Channel][Strobe][SignalGroupCount]));
                GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, SignalTrainingDelay + SignalGroupOffset, CMD_SET_VAL_FC_UC, &TempValue[Channel][Strobe]);

                //
                // For Signals running by group type.
                // Check below enables RMT to run TxDq per nibble when there is a x8 device width.
                // When there is a x4 device width the Limit[Channel] loop will take care of it.
                //
                if ((MrcData->MaxBit[Channel][Rank] == STROBE_BITS_X8_DEVICES) && ((MrcData->MrcRmtEnableSignal & (1 << i)) == RMT_TX_DQ_EXEC)) {
                  TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)] = RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount];
                  MspDebugPrint((MSP_DBG_DEBUG, "Debugging TempValueDevicesx8[%d][%d] = 0x%08x\n", Channel, (Strobe + MAX_STROBE_X8_DEVICES), TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)]));
                  MspDebugPrint((MSP_DBG_DEBUG, "Debugging RestoreValueDevicesx8[%d][%d][%d] = 0x%08x\n", Channel, (Strobe + MAX_STROBE_X8_DEVICES), SignalGroupCount, RestoreValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)][SignalGroupCount]));
                  GetSetDdrioGroup2(MrcData, Channel, Rank, (Strobe + MAX_STROBE_X8_DEVICES), SignalTrainingDelay + SignalGroupOffset, CMD_SET_VAL_FC_UC, &TempValue[Channel][(Strobe + MAX_STROBE_X8_DEVICES)]); 
                }
              } // CommandGroupCount loop ...
            } // RmtTable[i].RunByGroup check ...
          } // Strobe loop ...
        } // Channel loop ...
      } // i loop ...
    } // Rank loop

#if EV_RMT_SUMMARY == 1
    if (DebugMsgLevel != MSP_DBG_NONE) {
      MrcData->MspData.DebugMsgLevel |= MSP_DBG_MIN;
    }
    MspDebugPrint ((MSP_DBG_MIN, "START_RMT: \n"));
    //
    // This header is tied up to the number of
    // RmtTable signals configured to run by MrcRmtEnableSignal.
    //   e.g.
    //      RmtTable [] {
    //         {RecEnDelay,
    //         {TxDqsDelay,
    //         {RxDqsNDelay,
    //         ....
    //      Header below must print:  " RecEn     TxDqs     RxDqs ... "
    //
    MspDebugPrint ((MSP_DBG_MIN, "\n          "));
    for (i = 0; i < (sizeof (RmtTable) / sizeof (RMT_SIGNAL)); i++) {
      if (!(MrcData->MrcRmtEnableSignal & (1 << i))) {
          continue;
      }
      SignalOffsetIndex  = GetOffsetIndex (MrcData, RmtTable[i].Signal);
      if ((1 << i) < (1 << RmtTxDq)) {
        MspDebugPrint((MSP_DBG_MIN, "%s       ", SignalInfo[GetSignalInfoIndex(SignalOffsetIndex)].Name)); 
      } else {
        MspDebugPrint((MSP_DBG_MIN, "%s          ", SignalInfo[GetSignalInfoIndex(SignalOffsetIndex)].Name)); 
      }
    } // i loop ...
    MspDebugPrint ((MSP_DBG_MIN, "\n"));

    for (Rank = 0; Rank < MAX_RANKS; Rank ++) {
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {

        MspDebugPrint ((MSP_DBG_DEBUG, "Channel[%d] Tested = %d, Rank[%d] Enabled = %d\n", Channel, ChannelTested[Channel], Rank, MrcData->RankEnabled[Channel][Rank]));

        if ((!ChannelTested[Channel]) || (!MrcData->RankEnabled[Channel][Rank])) {
          continue;
        }

        MspDebugPrint ((MSP_DBG_MIN, "C%d.R%d:    ", Channel, Rank));
        for (i = 0; i < (sizeof (RmtTable) / sizeof (RMT_SIGNAL)); i++) {
          if (!(MrcData->MrcRmtEnableSignal & (1 << i))) {
            continue;
          }
          Element = RmtTable[i].Signal;
          Element = GetOffsetIndex (MrcData, Element);
          if (Element > ALGO_REG_INDEX) {
            Element = (Element - ALGO_REG_INDEX) * NumberOfElementsPerAlgo;
          }
          MspDebugPrint ((
            MSP_DBG_MIN, "%3d %3d          ",
            MrcData->RmtData[Channel][Rank][RmtTable[i].LowSide],
            MrcData->RmtData[Channel][Rank][RmtTable[i].HighSide]
          ));
        } // i loop ...
        MspDebugPrint ((MSP_DBG_MIN, "\n"));
      } // Channel loop ...
    } // Rank loop ...
    MspDebugPrint ((MSP_DBG_MIN, "\nSTOP_RMT: \n"));
    MrcData->MspData.DebugMsgLevel = DebugMsgLevel;
#else
    if (DebugMsgLevel != MSP_DBG_NONE) {
      MrcData->MspData.DebugMsgLevel |= MSP_DBG_MIN;
    }
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!ChannelTested[Channel]) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_MIN, "START_RMT: \n"));
      for (Rank = 0; Rank < MAX_RANKS; Rank ++) {
        if (!MrcData->RankEnabled[Channel][Rank]) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MIN, "Rank %d\n", Rank));
        for (i = 0; i < (sizeof (RmtTable) / sizeof (RMT_SIGNAL)); i++) {
          if (!(MrcData->MrcRmtEnableSignal & (1 << i))) {
            continue;
          }
          Element = RmtTable[i].Signal;
          Element = GetOffsetIndex (MrcData, Element);
          if (Element > ALGO_REG_INDEX) {
            Element = (Element - ALGO_REG_INDEX) * NumberOfElementsPerAlgo;
          }
          MspDebugPrint ((
            MSP_DBG_MIN, " %s  Low: %3d High: %3d\n",
            SignalInfo[GetSignalInfoIndex(Element)].Name,
            MrcData->RmtData[Channel][Rank][RmtTable[i].LowSide],
            MrcData->RmtData[Channel][Rank][RmtTable[i].HighSide]
          ));
        }
      } // Rank loop ...
      MspDebugPrint ((MSP_DBG_MIN, "STOP_RMT: \n"));
    } // Channel loop ...
    //
    // Restore debug messages.
    //
    MrcData->MspData.DebugMsgLevel = DebugMsgLevel;
#endif
  } // MrcData->MrcRmtSupport statement ...
  MrcData->MspData.DebugMsgLevel = DebugMsgLevel;
  //
  // Adding a ReInitializeDramsAcrossChannels since the last RMT stage
  // before exiting RMT could have been CMD margining, therefore after CMD margining
  // it is always neededing a ReInitializeDramsAcrossChannels.
  //
  ReInitializeDramsAcrossChannels(MrcData);
#endif // RMT == 1
  return MMRC_SUCCESS;
}

 
#if TRAINING_ALGOS == 1
MMRC_STATUS
Create2DEye (
  IN          MMRC_DATA       *MrcData,
  IN   OUT    SAMPLE_SWEEP    *Results,
  IN          UINT8           Channel,
  IN          UINT8           Rank,
  IN          UINT16          Dim1Index,
  IN          UINT8           Dim1Knobs,
  IN          UINT16          Dim1Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim1Step,
  IN          UINT16          Dim2Index,
  IN          UINT16          Dim2Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim2Step
)
{
  UINT8   StringToPrint[MAX_LINE_SIZE + 1];
  UINT8   Strobe;
  INT16   AxisY;
  INT16   AxisX;
  UINT8   i;
  UINT16  CenterY;
  UINT16  OrigCenterY;
  UINT16  CenterX[MAX_KNOBS];
  UINT16  AxisXResult[2];
  UINT8   Knob;
  UINT32  TempValue;
  UINT16  StepSize;
  //
  // Step size should be 1/2 clock divided by 64.
  //
  StepSize = GetHalfClk (MrcData, Channel) / 64;

  if (StepSize == 0) {
    //
    // Minimum StepSize should be '1'
    //
    StepSize = 1;
  }

  TempValue = 0;
  MmrcMemset(StringToPrint, 0, MAX_LINE_SIZE + 1);
  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
      if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
        continue;
      }

      MspDebugPrint ((MSP_DBG_MED, "C%d.R%d.S%d:\n  ", Channel, Rank, Strobe));
      for (i = 0; i < 8; i++) {
        MspDebugPrint ((MSP_DBG_MED, "DQ%d, ", Strobe * 8 + 7 - i));
      }
      MspDebugPrint ((MSP_DBG_MED, "\n"));

      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Dim1Index + Knob, CMD_GET_CACHE, &TempValue);
        CenterX[Knob] = (UINT16) TempValue;
      }
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Dim2Index, CMD_GET_CACHE, &TempValue);
      CenterY = (UINT16) TempValue;
      OrigCenterY = CenterY;
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        CenterY = OrigCenterY;
        //
        // Print the real linear value before doing the adjust
        //
        MspDebugPrint ((MSP_DBG_MED, "  Cx: %d Cy: %d\n  ", CenterX[Knob], CenterY));
        //
        // During Create2DSweep the value in Axis Y always starts from '0', so it is needed to 
        // to be adjusted during Center2D to get the good center in Y, so for display the 2D
        // eye the Axis 2D Min needs to be subtracted again
        //
        CenterX[Knob] -= Dim1Min[Channel][Strobe];
        CenterY -= Dim2Min[Channel][Strobe];
        //
        // Adjust to the Previous number which could be divided by Dim2Step
        //
        CenterY = CenterY - CenterY % Dim2Step;

        for (AxisY = (MAX_ELEMENTS_DIM_Y - 1); AxisY >= 0; AxisY -= 1) {
          if (Results[AxisY].Sample[Channel][Knob][Strobe][0][HIGH] <= Results[AxisY].Sample[Channel][Knob][Strobe][0][LOW]) {
            continue;
          }
          AxisXResult[HIGH] = Results[AxisY].Sample[Channel][Knob][Strobe][0][HIGH] - Dim1Min[Channel][Strobe];
          AxisXResult[LOW]  = Results[AxisY].Sample[Channel][Knob][Strobe][0][LOW] - Dim1Min[Channel][Strobe];

          for (AxisX = 0; AxisX < (MAX_LINE_SIZE * StepSize); AxisX += StepSize) {
            if (AxisY == CenterY && ((AxisX <= CenterX[Knob]) && ((AxisX + StepSize) > CenterX[Knob]))) {
              StringToPrint[AxisX / StepSize] = '+';
            } else if ((AxisX <= AxisXResult[HIGH]) && ((AxisX + StepSize) > AxisXResult[HIGH])) {
              //
              // Right edge
              //
              StringToPrint[AxisX / StepSize] = '*';
              StringToPrint[(AxisX / StepSize) + 1] = '\0';
              break;
            } else if ((AxisX <= AxisXResult[LOW]) && ((AxisX + StepSize) > AxisXResult[LOW])) {
              //
              // Left edge
              //
              StringToPrint[AxisX / StepSize] = '*';
            } else {
              //
              // Left of eye or in the eye
              //
              StringToPrint[AxisX / StepSize] = ' ';
            }
          }
          MspDebugPrint ((MSP_DBG_MED, "%s\n", (UINT8 *) StringToPrint));
        } // AxisY loop ...
      } // Knob Loop
    } // Strobe loop ...
  } // Channel loop ...

  return MMRC_SUCCESS;
}
#endif // TRAINING_ALGOS == 1

#if COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
/**
  For Command Training, prints the three values associated with a command training group.
  Low/High/Default values.  The command informs which value should be printed and which values
  should be shown with xxx.

  @param[in]       Command         Command to inform which value is to be printed or masked out.
  @param[in]       LowValue        Low Value associated with the group.
  @param[in]       HighValue       High Value associated with the group.
  @param[in]       DefaultValue    Default value associated with the group.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
PrintCommandLowHigh (
  IN        UINT8         Command,
  IN        UINT32        LowValue,
  IN        UINT32        HighValue,
  IN        UINT32        DefaultValue
  ) 
{
  //
  // Print the low range value.
  //
  if (Command == CMDLOWHIGH_LOW || Command == CMDLOWHIGH_BOTH) {
     MspDebugPrint ((MSP_DBG_MAX, " %03d:", LowValue));
  } else {
     MspDebugPrint ((MSP_DBG_MAX, " xxx:"));
  }
  // 
  // Print the high range value.
  //
  if (Command == CMDLOWHIGH_HIGH || Command == CMDLOWHIGH_BOTH) {
     MspDebugPrint ((MSP_DBG_MAX, "%03d", HighValue));
  } else {
     MspDebugPrint ((MSP_DBG_MAX, "xxx"));
  }
  //
  // Print the default value.
  //
  MspDebugPrint ((MSP_DBG_MAX, "  %03d  ", DefaultValue));
 
  return MMRC_SUCCESS;
}

MMRC_STATUS
HeaderCommandClockTraining (   
  IN    MMRC_DATA *MrcData,
  IN    UINT8  HeaderType,
  IN    UINT8  Channel,
  IN    UINT8  Rank,
  IN    UINT8  Step,
  IN    UINT16 *CmdLow,
  IN    UINT16 *CmdHigh,
  IN    UINT32 *CmdValue
)
{
  UINT8 TempValue;

  if (!RunOnThisChannel (MrcData, Channel, Rank)) {
    return MMRC_SUCCESS;
  }
  switch (HeaderType) {
  case CMD_HEADER_ENTRY:

    MspDebugPrint ((MSP_DBG_MAX, "Stp     Size  CRS "));
    for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
      MspDebugPrint ((MSP_DBG_MAX, "   CMD%d    DEF ", TempValue));
    }
    MspDebugPrint ((MSP_DBG_MAX, "  PF\n"));
    MspDebugPrint ((MSP_DBG_MAX, "------  ----  --- "));
    for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
      MspDebugPrint ((MSP_DBG_MAX, "  -------  --- "));
    }
    MspDebugPrint ((MSP_DBG_MAX, "  --\n"));
    break;
  case CMD_HEADER_INIT:

    MspDebugPrint ((MSP_DBG_MAX, "INIT     xx   %xxx  ", Channel));
    for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
      MspDebugPrint ((MSP_DBG_MAX, " xxx:xxx  %03d  ", CmdValue[TempValue]));
    }
    MspDebugPrint ((MSP_DBG_MAX, " P\n"));
    break;
  case CMD_HEADER_COMMON:
    MspDebugPrint ((MSP_DBG_MAX, "Common   %02d   %d%dx  ", Step, Channel, Rank));
    break;
#if MAX_CMDS != 1
  case CMD_HEADER_INDEPENDENT:
    MspDebugPrint ((MSP_DBG_MAX, "Indep.   %02d   %d%dx  ", Step, Channel, Rank));
    break;
  case CMD_HEADER_TETHER:
    MspDebugPrint ((MSP_DBG_MAX, "Tether   %02d   %d%dx  ", Step, Channel, Rank));
    break;
  case CMD_HEADER_FINAL:
    MspDebugPrint ((MSP_DBG_MAX, "FINAL    xx   %dxx  ", Channel));
    break;
#endif
    default:
    break;
  }

  if (HeaderType != CMD_HEADER_ENTRY && HeaderType != CMD_HEADER_INIT) {
      for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
        PrintCommandLowHigh (CMDLOWHIGH_BOTH, CmdLow[TempValue], CmdHigh[TempValue], CmdValue[TempValue]);
      }
      MspDebugPrint ((MSP_DBG_MAX, " P\n"));
  }
  return MMRC_SUCCESS;
}
#endif // COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1

/**
  Restores the values of the CMD training for the Dunit
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandClockEntry (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  )
{
#if COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
  UINT8   BackMsg;

  BackMsg = MrcData->MspData.DebugMsgLevel;
  MrcData->MspData.DynamicVars[Channel][PATH_ENABLE] = 0;
  MrcData->MspData.DebugMsgLevel |= MSP_DBG_DEBUG;
  ChangeCmdMode (MrcData, Channel, MrcData->CurrentMode[Channel]);
  MrcData->MspData.DebugMsgLevel = BackMsg;
#endif // COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
  return MMRC_SUCCESS;
}

/**
  Restores the values of the CMD training for the DDRIO
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandRestore (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  )
{
#if COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
  UINT8   Index;
  UINT16  CommandGroupOffset;
  UINT16  CommandTrainingDelay;

  if (MrcData->SkipTrainingAlgos[LCommandTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }
  CommandClockEntry (MrcData, 0, 0, Channel);
  CommandTrainingDelay  = GetOffsetIndex (MrcData, CmdGrp0);
  CommandGroupOffset    = 0;

  for (Index = 0; Index < MAX_CMDS; Index++) {
    DisplayOrRestoreTrainingResults (MrcData, Channel, CommandTrainingDelay + CommandGroupOffset, FALSE);
    CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
  }
#endif // COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
  return MMRC_SUCCESS;
}

/**
  Restores the values of the CTL and CLK training for the DDRIO

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GroupSaveRestore (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  )
{

  DisplayOrRestoreTrainingResults (MrcData, Channel, CtlGrp0,  TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, CtlGrp1,  TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, CtlGrp2,  TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, CkAll,  TRUE);

  return MMRC_SUCCESS;
}

VOID
BlastCommandGroups (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT32            Value,
  IN      UINT8             Mode
)
{
  UINT8     Rank;
  UINT8     Strobe;
  UINT8     CmdLoop;
  UINT16    CommandTrainingDelay;
  UINT16    CommandGroupOffset;

  MrcData->CpgcOptions.RmtRestoreSignalGroupBegin       = CmdGrp0;
  MrcData->CpgcOptions.RmtRestoreSignalCount            = MAX_CMDS;

  MspDebugPrint ((MSP_DBG_MIN, "Setting CMD GRP = %d at %dN\n", Value, Mode));
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      //
      // Set new safe value according with the Mode
      //
      CommandTrainingDelay = GetOffsetIndex (MrcData, MrcData->CpgcOptions.RmtRestoreSignalGroupBegin);
      CommandGroupOffset   = 0;
      for (CmdLoop = 0; CmdLoop < MrcData->CpgcOptions.RmtRestoreSignalCount; CmdLoop++) {
        GetSetDdrioGroup2 (
          MrcData, Channel, Rank, Strobe,
          CommandTrainingDelay + CommandGroupOffset,
          CMD_SET_VAL_FC_UC,
          &Value
        );
        MrcData->CpgcOptions.RmtRestoreSignalValue[CmdLoop][Channel][Rank][Strobe] = Value;
        CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
      } // CmdLoop ...
    } //  Strobe loop ...
  } // Rank loop ...
}

MMRC_STATUS
ChangeCmdMode (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Mode
)
{
  MrcData->MspData.DynamicVars[Channel][TXP_CMD]  = MrcData->MspData.DynamicVars[Channel][TXP];
  MrcData->MspData.DynamicVars[Channel][TCMD_CMD] = Mode - 1;

  if (MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED] == 0 && Mode == 1) {
    MspDebugPrint ((MSP_DBG_MIN, "1N Mode NOT Supported\n"));
  } else if (Mode == 1) {
    //
    // 1N mode
    //
    MrcData->MspData.DynamicVars[Channel][TXP_CMD]  += 1;
    MrcData->MspData.DynamicVars[Channel][TCMD_CMD]  = 0;
  }
  MspDebugPrint ((
    MSP_DBG_DEBUG,
    "Command Training will be at %dN Mode tXPC = %d tXP = %d\n",
    (UINT32) (MrcData->MspData.DynamicVars[Channel][TCMD_CMD] + 1),
    (UINT32) MrcData->MspData.DynamicVars[Channel][TXP_CMD],
    (UINT32) MrcData->MspData.DynamicVars[Channel][TXP]
  ));
  MmrcExecuteTask (MrcData, COMMANDCLOCKTRAINING_ENTRY, NO_PRINT, Channel);

  return MMRC_SUCCESS;
}

#define MAX_SAMPLES   21
const INT32   Samples[MAX_SAMPLES] = {0, -16, -32, -48, -64, -80, -96, -112, -128, -144, -160, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160};
MMRC_STATUS
SampleAndVerifyCmdStress (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Mode,
  IN      UINT32            SafeValue
)
{
  UINT32  FailuresDetected;
  UINT32  OriginalValue;
  UINT32  FirstPass;
  UINT32  NewValue;
  INT32   Delta;
  UINT8   SampleIndex;
  INT32   LowLimit;
  INT32   HighLimit;
  UINT8   BackupMsg;

  BackupMsg     = MrcData->MspData.DebugMsgLevel;
  OriginalValue = SafeValue;
  LowLimit      = -1;
  HighLimit     = -1;
  FirstPass     = 0;
  for (SampleIndex = 0; SampleIndex < MAX_SAMPLES; SampleIndex++) {
    BlastCommandGroups (MrcData, Channel, OriginalValue + Samples[SampleIndex], Mode);
    ChangeCmdMode (MrcData, Channel, Mode);
    MrcData->MspData.DebugMsgLevel = MSP_DBG_NONE;
    FailuresDetected = TestMrcTrainingSteps (
      MrcData, Channel,
      CPGC_CONFIG_CMD_STRESS, CPGC_CMDSEQ_WRITE_READ,
      0,
      MrcData->CpgcOptions.LoopCount, MrcData->CpgcOptions.NumBursts
    );
    MrcData->MspData.DebugMsgLevel = BackupMsg;
    if (FailuresDetected != 0) {
      MspDebugPrint ((MSP_DBG_MIN, "  "));
      BlastCommandGroups (MrcData, Channel, SafeValue, (UINT8)MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA]);
      MspDebugPrint ((MSP_DBG_DEBUG, "  "));
      ChangeCmdMode (MrcData, Channel, (UINT8)MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA]);
      MspDebugPrint ((MSP_DBG_MIN, "  **ReInitialize\n"));
      ReInitializeDramsAcrossChannels (MrcData);
    } else {
      if (FirstPass == 0) {
        FirstPass = OriginalValue + Samples[SampleIndex];
      }
      if (SampleIndex == 0) {
        LowLimit  = Samples[SampleIndex];
        HighLimit = Samples[SampleIndex];
      } else if (SampleIndex <= (MAX_SAMPLES / 2)) {
        LowLimit  = Samples[SampleIndex];
      } else {
        HighLimit = Samples[SampleIndex];
      }
    }
  } // SampleIndex loop ...

  //
  // We need to adjust to the first Passing Point
  //
  Delta = FirstPass - OriginalValue;
  if (HighLimit != -1) {
    HighLimit -= Delta;
  }
  if (LowLimit != -1) {
    LowLimit  -= Delta;
  }
  if (LowLimit != -1 && HighLimit != -1) {
    NewValue = ((HighLimit + LowLimit) / 2) + FirstPass;
  } else if (LowLimit != -1 && HighLimit == -1) {
    NewValue = (2*FirstPass + LowLimit) / 2;
  }  else if (LowLimit == -1 && HighLimit != -1) {
    NewValue = (2*FirstPass + HighLimit) / 2;
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Margin not found at %dN\n", Mode));
    return MMRC_FAILURE;
  }

  BlastCommandGroups (MrcData, Channel, NewValue, Mode);
  MspDebugPrint ((MSP_DBG_MIN, "**Result L: %d H:%d New Value = %d\n", LowLimit, HighLimit, NewValue));

  return MMRC_SUCCESS;
}


/**
  Performs the Command Training.  Based on the DIMM configuration (1DPC/2DPC), 
  the training will be completed in 1N/2N mode. No assumption is 
  made on the beginning mode for the Command bus; however, 
  memory should be stable prior to entering. Channel is the only 
  additional parameter passed in, and the calibration will be 
  performed on the channel specified. 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandClockTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
#if !defined _SIMIC_
#if NEW_RTWT_ALGO == 1
  UINT8     ChannelBackup;
  UINT8     IndCh;
  UINT8     CmdLoop;
  UINT16    CommandTrainingDelay;
  UINT16    CommandGroupOffset;
  UINT32    SafeValue;
  MMRC_STATUS Status;

  if (MrcData->SkipTrainingAlgos[LCommandTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }
  MrcData->MspData.DynamicVars[0][PATH_ENABLE] = 1;
  MrcData->MspData.DynamicVars[1][PATH_ENABLE] = 1;
  //
  // Sample Safe Values
  //
  GetSetDdrioGroup2 (
    MrcData, Channel, 0, 0,
    CmdGrp0,
    CMD_GET_REG_UC,
    &SafeValue
  );
  //
  // Blast the safe results
  //
  BlastCommandGroups (MrcData, Channel, SafeValue, (UINT8)MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA]);
  //
  // Verify 3N Mode is healthy with CPGC STRESS
  // Valid values for 1N and 2N should also be valid for 3N
  //
  Status = SampleAndVerifyCmdStress (MrcData, Channel, (UINT8)MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA], SafeValue);
  if (Status == MMRC_FAILURE) {
    return MMRC_FAILURE;
  }
  //
  // Sample Safe Values
  //
  GetSetDdrioGroup2 (
    MrcData, Channel, 0, 0,
    CmdGrp0,
    CMD_GET_REG_UC,
    &SafeValue
  );
  //
  // Verify N Mode is healthy with CPGC STRESS
  //
  Status = SampleAndVerifyCmdStress (MrcData, Channel, MrcData->CurrentMode[Channel], SafeValue);
  if (Status == MMRC_FAILURE) {
    MspDebugPrint ((MSP_DBG_MIN, "Not Found any Margin at %dN. Trying %dN\n", MrcData->CurrentMode[Channel], MrcData->CurrentMode[Channel]+1));
    MrcData->CurrentMode[Channel] += 1;
    //
    // Verify N+1 Mode is healthy with CPGC STRESS
    //
    SampleAndVerifyCmdStress (MrcData, Channel, MrcData->CurrentMode[Channel], SafeValue);
    if (Status == MMRC_FAILURE) {
      return MMRC_FAILURE;
    }
  }
  //
  // Sample Safe Values
  //
  GetSetDdrioGroup2 (
    MrcData, Channel, 0, 0,
    CmdGrp0,
    CMD_GET_REG_UC,
    &SafeValue
  );
  ChangeCmdMode (MrcData, Channel, MrcData->CurrentMode[Channel]);

  ChannelBackup = Channel;
  // 
  // If on the full boot path (not restore path), setup CPGC for Write training
  // and call the training algorithm, otherwise restore the passed in values.
  //
  if (!MrcData->RestorePath) {
#ifdef SIM
      MrcData->CpgcOptions.NumBursts = 6;
      MrcData->CpgcOptions.LoopCount = 1;
#else
      MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt;
      MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
#endif
    Channel = ChannelBackup;
    for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
      //
      // STOP_ON_ERR_CONTROL set to 0 for advanced read and write and 2 for CMD training
      //
      MrcData->MspData.DynamicVars[IndCh][ADVANCED_RD_WR] = 0;
      CpgcSetup (MrcData, IndCh, CPGC_CONFIG_CMD_STRESS);
      MrcData->CpgcOptions.Rank = 0;
      CpgcReconfig (MrcData, IndCh);
    } // IndCh loop ...

    ReInitializeDramsAcrossChannels (MrcData);
    ForceMaxBit (MrcData, 1);
    AdvancedTrainingAlgo (MrcData, ControlKnobsCmd, NumberControlKnobsCmd, EyeMaskCmd, NumberEyeMaskCmd, FALSE);
    ReInitializeDramsAcrossChannels (MrcData);
    GetMaxDq (MrcData);
  }
  //
  // Print out or restore the training results.
  //
  CommandTrainingDelay = GetOffsetIndex (MrcData, CmdGrp0);
  CommandGroupOffset   = 0;
  for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
    DisplayOrRestoreTrainingResults (MrcData, Channel, CommandTrainingDelay + CommandGroupOffset, TRUE);
    CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
  }


#ifdef COMMANDCLOCKTRAINING_EXIT
  MmrcExecuteTask(MrcData, COMMANDCLOCKTRAINING_EXIT, NO_PRINT, Channel);
#endif
  if (TestMrcTrainingSteps (MrcData, ChannelBackup, CPGC_CONFIG_VA, CPGC_CMDSEQ_WRITE_READ, 0, 8, 6) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }
#else
  UINT16    CommandTrainingDelay;
  UINT16    CommandGroupOffset;
#if MAX_CMDS != 1 && !defined(SIM)
  UINT16    CommandGroupOffsetTemp;
#endif
  UINT8     NumberElements[MAX_CHANNELS];
  UINT32    CmdValue[MAX_CMDS];
  UINT16    CmdLowerFinal[MAX_CHANNELS][MAX_CMDS];
  UINT16    CmdHigherFinal[MAX_CHANNELS][MAX_CMDS];
  UINT8     CmdLoop;
  UINT32    TempValue;
  UINT8     CmdSweepDelta;
  UINT16    CmdLower[MAX_CHANNELS][MAX_CMDS];
  UINT16    CmdHigher[MAX_CHANNELS][MAX_CMDS];
  UINT8     Rank;
  UINT8     Strobe;
  UINT16    Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT16    StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT16    CmdMin[MAX_CHANNELS][MAX_STROBES];
  UINT16    CmdMax[MAX_CHANNELS][MAX_STROBES];
  INT8      TetherVal[MAX_CHANNELS];
#if MAX_CMDS != 1 && !defined(SIM)
  UINT16    TempLow;
  UINT16    TempHigh;
#endif 
  UINT8     ChannelBackup;
  BOOLEAN   ChannelTested[MAX_CHANNELS];
  BOOLEAN   AnyValidChannel;

  ChannelBackup = Channel;

  if (MrcData->SkipTrainingAlgos[LCommandTraining]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

  //
  // Rmt settings for Jedec reset & Restore Signal
  //
  MrcData->CpgcOptions.RmtRestoreJedecResetType         = FAIL_SINGLE_STROBE | FAIL_RESTORE_PI_CENTER | FAIL_JEDEC_RESET;
#if defined RMT && RMT==1
  MrcData->CpgcOptions.RmtRestoreSignalGroupBegin       = RmtTable[RmtCmd].Signal;
#else
  MrcData->CpgcOptions.RmtRestoreSignalGroupBegin       = CmdGrp0;
#endif
  MrcData->CpgcOptions.RmtRestoreSignalCount            = MAX_CMDS;
  MrcData->CpgcOptions.RmtRestoreSignalGroupBeginVref   = CmdVref;
  MrcData->CpgcOptions.NumberOfElementsVref[0]          = 1;
  MrcData->CpgcOptions.NumberOfElementsVref[1]          = 1;
  MrcData->CpgcOptions.RmtRestoreSignalCountVref        = 0;



#if FAULTY_PART_TRACKING
  FaultyPartTrackInit (MrcData, Channel, FALSE);
#endif

  CommandTrainingDelay = GetOffsetIndex (MrcData, CmdGrp0);
  //
  // Entry Hooks
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {

    NumberElements[Channel] = 1;
    MrcData->MspData.DynamicVars[Channel][PATH_ENABLE] = 1;
    TetherVal[Channel]      = 0;
    ChannelTested[Channel]  = FALSE;
    for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
      CmdLowerFinal[Channel][CmdLoop]  = 0;
      CmdHigherFinal[Channel][CmdLoop] = 0xffff;
    }
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        GetSetDdrioGroup2 (
          MrcData, Channel, Rank, Strobe,
          MrcData->CpgcOptions.RmtRestoreSignalGroupBeginVref,
          CMD_GET_CACHE,
          &MrcData->CpgcOptions.RmtRestoreSignalValueVref[0][Channel][Rank][Strobe]
        );
      }
    }
  } // Channel loop ...
  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;

#ifdef COMMANDCLOCKTRAINING_ENTRY
  MmrcExecuteTask (MrcData, COMMANDCLOCKTRAINING_ENTRY, NO_PRINT, Channel);
#endif

  HeaderCommandClockTraining (MrcData, CMD_HEADER_ENTRY, Channel, 0, 0, CmdLower[Channel], CmdHigher[Channel], CmdValue);
  //
  // Loop through all ranks re-running the same procedure with CPGC testing the specific ranks.
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    MmrcMemset (Results, 0xff, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 2 * sizeof (UINT16));
    AnyValidChannel = FALSE;
    //
    // Let's set the step size the COMMON stage for any Dimm Configuration
    //
#ifdef SIM
    CmdSweepDelta = 100;
#else
    CmdSweepDelta = 20;
#endif
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      ChannelTested[Channel]  = TRUE;
      AnyValidChannel         = TRUE;
      //
      // Initialization
      //
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        CmdLower[Channel][CmdLoop]       = 0;
        CmdHigher[Channel][CmdLoop]      = 0;
      }
#if CPGC_BASED_READWRITES == 1
#ifdef SIM
      MrcData->CpgcOptions.NumBursts = 6;
      MrcData->CpgcOptions.LoopCount = 1;
#else 
      MrcData->CpgcOptions.NumBursts = MrcData->MrcCpgcNumBursts;
      MrcData->CpgcOptions.LoopCount = MrcData->MrcTrainingCpgcExpLoopCnt;
#endif
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_CMD_STRESS);
      //
      // Setup the CPGC engine to to do a single read from an address within the 
      // selectable rank.  The engine should be setup for LFSR mode.
      //
      MrcData->CpgcOptions.Rank = Rank;
      CpgcReconfig (MrcData, Channel);
#endif 
      //
      // Read the Default Command values for each group storing them in the CmdValue array for each group.
      // This is the restore values for Phase 0: Coarse.  All subsequence phases will use the prior phases
      // results as the restore values.
      //
      CommandGroupOffset = 0;
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CommandTrainingDelay + CommandGroupOffset, CMD_GET_REG_UC, &(CmdValue[CmdLoop]));
        MrcData->CpgcOptions.RmtRestoreSignalValue[CmdLoop][Channel][Rank][0] = CmdValue[CmdLoop];
        CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
      }
      //
      // Print out the Initial CMD timing data.
      //
      HeaderCommandClockTraining (MrcData, CMD_HEADER_INIT, Channel, Rank, 0, CmdLower[Channel], CmdHigher[Channel], CmdValue);
      //
      // Do coarse sweep from Min to Max CMD delay edges starting from passing point (current CMD value)
      //
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        CmdLower[Channel][CmdLoop]  = (UINT16) CmdValue[CmdLoop];
        CmdHigher[Channel][CmdLoop] = (UINT16) CmdValue[CmdLoop];
      }
      CmdMin[Channel][0] = 0;
      CmdMax[Channel][0] = (UINT16) SignalUpperBound (MrcData, Channel, Rank, MrcData->CpgcOptions.RmtRestoreSignalGroupBegin);
      StartPoint[Channel][0][0][LOW]   = (UINT16)CmdValue[0];
      StartPoint[Channel][0][0][HIGH]  = (UINT16)CmdValue[0];
    } // Channel loop ...
    if (!AnyValidChannel) {
      //
      // Skip Rank if it is not valid on either channel
      //
      continue;
    }

    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    Create1DSweep (MrcData, Channel, Rank, CmdAll, 1, StartPoint, CmdMin, CmdMax, CmdSweepDelta, FALSE, FALSE, FALSE, NumberElements, CpgcExecuteRmtTest, Results, "", 1, RUN_PER_STROBE);

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      //
      // For phase 0, the results should be Copy the results the the CmdLower/CmdHigher for each of the other CmdElements.
      //
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        CmdLower[Channel][CmdLoop]  = Results[Channel][0][0][0][LOW];
        CmdHigher[Channel][CmdLoop] = Results[Channel][0][0][0][HIGH];
        CmdValue[CmdLoop]           = (CmdLower[Channel][CmdLoop] + CmdHigher[Channel][CmdLoop]) / 2;
        MrcData->CpgcOptions.RmtRestoreSignalValue[CmdLoop][Channel][Rank][0] = CmdValue[CmdLoop];
      }
      //
      // Avoid the next phase starts at the edge of the latest good known value
      //
      StartPoint[Channel][0][0][LOW]   = Results[Channel][0][0][0][LOW]  + CMD_THRESHOLD;
      StartPoint[Channel][0][0][HIGH]  = Results[Channel][0][0][0][HIGH] - CMD_THRESHOLD;
      //
      // Print out the results.
      //
      HeaderCommandClockTraining (MrcData, CMD_HEADER_COMMON, Channel, Rank, CmdSweepDelta, CmdLower[Channel], CmdHigher[Channel], CmdValue);
      //
      // In phase 0 the memory was left in a bad state, restore known good CMD timings before reinitializing the DRAM devices.
      // Otherwise, the DRAMs may not receive the MRS commands correctly.
      // 
      CommandGroupOffset = 0;
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CommandTrainingDelay + CommandGroupOffset, CMD_SET_VAL_FC_UC, &(CmdValue[CmdLoop]));
        CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
      }
      ReInitializeDrams (MrcData, Channel, Rank, 0);

    } // Channel loop ...
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;

#if MAX_CMDS != 1 && !defined(SIM)
    //
    // BEGINNING Phase 0: Fine Loop
    // Phase 0 Fine starts at the endpoints determined from Phase 0 Coarse, and expands the range by stepping
    // in fine steps until the true failure margins are found.  The Command Groups are still tethered together.
    //
#ifdef SIM
    CmdSweepDelta = 20;
#else
    CmdSweepDelta = 2;
#endif

    Create1DSweep (MrcData, Channel, Rank, CmdAll, 1, StartPoint, CmdMin, CmdMax, CmdSweepDelta, FALSE, FALSE, FALSE, NumberElements, CpgcExecuteRmtTest, Results, "", 1, RUN_PER_STROBE);

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      //
      // Compute the new center based on the new limits for each Cmd Group.
      //
      for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
        CmdLower[Channel][TempValue]  = Results[Channel][0][0][0][LOW];
        CmdHigher[Channel][TempValue] = Results[Channel][0][0][0][HIGH];
        CmdValue[TempValue]           = (CmdLower[Channel][TempValue] + CmdHigher[Channel][TempValue]) / 2;
        MrcData->CpgcOptions.RmtRestoreSignalValue[TempValue][Channel][Rank][0] = CmdValue[TempValue];
      }
      //
      // Print out the results, phase 0 and phase2 only loop once, where as phase1 loops MAX_CMD times.
      //
      HeaderCommandClockTraining (MrcData, CMD_HEADER_COMMON, Channel, Rank, CmdSweepDelta, CmdLower[Channel], CmdHigher[Channel], CmdValue);
    } // Channel loop ...
    //
    //  BEGINNING Phase 1: Independent. Margin CMD groups independently
    //
#ifdef SIM
    CmdSweepDelta = 20;
#else
    CmdSweepDelta = 1;
#endif

    CommandGroupOffsetTemp = 0;
    for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        //
        // Since we are sweeping each CMD group individually we need to return to a known good state to get better margin
        //
        CommandGroupOffset = 0;
        for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
          GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CommandTrainingDelay + CommandGroupOffset, CMD_SET_VAL_FC_UC, &(CmdValue[CmdLoop]));
          CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
        }
        ReInitializeDrams (MrcData, Channel, Rank, 0);

        StartPoint[Channel][0][0][LOW]   = (UINT16)CmdValue[TempValue];
        StartPoint[Channel][0][0][HIGH]  = (UINT16)CmdValue[TempValue];
      } // Channel loop ...

      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;
      Create1DSweep (MrcData, Channel, Rank, CommandTrainingDelay + CommandGroupOffsetTemp, 1, StartPoint, CmdMin, CmdMax, CmdSweepDelta, FALSE, FALSE, FALSE, NumberElements, CpgcExecuteRmtTest, Results, "", 1, RUN_PER_STROBE);
      CommandGroupOffsetTemp += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffsetTemp)].FrequencyOffset;

      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        CmdLower[Channel][TempValue]   = Results[Channel][0][0][0][LOW];
        CmdHigher[Channel][TempValue]  = Results[Channel][0][0][0][HIGH];
      } // Channel loop ...
    } // TempValue loop ...

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      //
      // Compute the new center based on the new limits for each Cmd Group.
      //
      TempLow   = 0xffff;
      TempHigh  = 0;
      for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
        CmdValue[TempValue] = (CmdLower[Channel][TempValue] + CmdHigher[Channel][TempValue]) / 2;
        MrcData->CpgcOptions.RmtRestoreSignalValue[TempValue][Channel][Rank][0] = CmdValue[TempValue];
        //
        // Find the biggest range for the last phase
        //
        if (TempLow > CmdLower[Channel][TempValue]) {
          TempLow = CmdLower[Channel][TempValue];
        }
        if (TempHigh < CmdHigher[Channel][TempValue]) {
          TempHigh = CmdHigher[Channel][TempValue];
        }
      }
      //
      // Print out the results, phase 0 and phase2 only loop once, where as phase1 loops MAX_CMD times.
      //
      HeaderCommandClockTraining (MrcData, CMD_HEADER_INDEPENDENT, Channel, Rank, CmdSweepDelta, CmdLower[Channel], CmdHigher[Channel], CmdValue);
      //
      // BEGINNING Phase 2: Tethering between the CMD groups
      //
      TetherVal[Channel] = (UINT8)(CmdValue[1] - CmdValue[0]);
      for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
        CmdLower[Channel][TempValue]  = TempLow;
        CmdHigher[Channel][TempValue] = TempHigh;
      }
      //
      // Set the initial value, including the tether offset.
      //
      CommandGroupOffset = 0;
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CommandTrainingDelay + CommandGroupOffset, CMD_SET_VAL_FC_UC, &(CmdValue[CmdLoop]));
        CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
      }
      //
      // Since we are calling 1D Sweep with the option to skip initial register programming enabled, it means
      // that we must make sure StartPoint[][] matches the current value for CmdGrp0.
      //
      StartPoint[Channel][0][0][LOW]   = (UINT16)CmdValue[0];
      StartPoint[Channel][0][0][HIGH]  = (UINT16)CmdValue[0];
    } // Channel loop ...
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    Create1DSweep (MrcData, Channel, Rank, CmdAll, 1, StartPoint, CmdMin, CmdMax, CmdSweepDelta, FALSE, TRUE, FALSE,  NumberElements, CpgcExecuteRmtTest, Results, "", 1, RUN_PER_STROBE);

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      //
      // Compute the new center based on the new limits for each Cmd Group.
      //
      for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
        CmdLower[Channel][TempValue]  = (UINT16)(Results[Channel][0][0][0][LOW]  + TetherVal[Channel] * TempValue);
        CmdHigher[Channel][TempValue] = (UINT16)(Results[Channel][0][0][0][HIGH] + TetherVal[Channel] * TempValue);
        CmdValue[TempValue]           = (CmdLower[Channel][TempValue] + CmdHigher[Channel][TempValue]) / 2;
      }
      //
      // Print out the results, phase 0 and phase2 only loop once, where as phase1 loops MAX_CMD times.
      //
      HeaderCommandClockTraining (MrcData, CMD_HEADER_TETHER, Channel, Rank, CmdSweepDelta, CmdLower[Channel], CmdHigher[Channel], CmdValue);
      //
      // Update the final CmdRanges so that the largest minimum and the smallest maximum are maintained for all enabled ranks.
      //
      for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
        if (CmdHigherFinal[Channel][TempValue] > CmdHigher[Channel][TempValue]) {
          CmdHigherFinal[Channel][TempValue] = CmdHigher[Channel][TempValue];
        }
        if (CmdLowerFinal[Channel][TempValue] < CmdLower[Channel][TempValue]) {
          CmdLowerFinal[Channel][TempValue] = CmdLower[Channel][TempValue];
        }
      }
    } // Channel loop ...
#endif // MAX_CMDS != 1
  } // Rank loop ...

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!ChannelTested[Channel]) {
      continue;
    }
#if MAX_CMDS != 1
    //
    // Calculate and set the final values from the Low/High ranges.
    //
    CommandGroupOffset = 0;
    for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
       CmdValue[TempValue] = (CmdLowerFinal[Channel][TempValue] + CmdHigherFinal[Channel][TempValue]) / 2;
       GetSetDdrioGroup2 (MrcData, Channel, 0, 0, CommandTrainingDelay + CommandGroupOffset, CMD_SET_VAL_FC_UC, &(CmdValue[TempValue]));
       CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
    }
    ReInitializeDrams (MrcData, Channel, 0, 0); 
    //
    // Print out the final results
    //
    HeaderCommandClockTraining (MrcData, CMD_HEADER_FINAL, Channel, 0, 0, CmdLowerFinal[Channel], CmdHigherFinal[Channel], CmdValue);
#endif // MAX_CMDS != 1
  } // Channel loop ...
  //
  // Print out or restore the training results.
  //
  CommandGroupOffset = 0;
  for (TempValue = 0; TempValue < MAX_CMDS; TempValue++) {
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    DisplayOrRestoreTrainingResults (MrcData, Channel, CommandTrainingDelay + CommandGroupOffset, TRUE);
    CommandGroupOffset += SignalInfo[GetSignalInfoIndex (CommandTrainingDelay + CommandGroupOffset)].FrequencyOffset;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!ChannelTested[Channel]) {
        continue;
      }
      ReInitializeDrams (MrcData, Channel, 0, 0);
    } // Channel loop ...
  }
  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;
  //MmrcExecuteTask (MrcData, CPGC_DISABLE, NO_PRINT, Channel);
  //
  // Exit Hooks
  //
#ifdef COMMANDCLOCKTRAINING_EXIT
  MmrcExecuteTask (MrcData, COMMANDCLOCKTRAINING_EXIT, NO_PRINT, Channel);
#endif
#endif // NEW_RTWT_ALGO
#endif // !defined _SIMIC_
#endif // COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
  return MMRC_SUCCESS;
}

/**
  Prints the information gotten from the training results
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
PhyViewTable (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if PHYVIEW_TABLE == 1
  UINT16 i;
  BOOLEAN Restore;

  //
  // Save the debug level and restore path values for PhyViewTable
  //
  Restore = MrcData->RestorePath;
  MrcData->RestorePath = FALSE;
  //
  // Loop through each delay, printing the values for each.
  //
  for (i = REG_INDEX; i < (REG_INDEX + NumberAlgos); i++) {
    if (GetOffsetIndex (MrcData, i) != REG_UNDEFINED && GetOffsetIndex (MrcData, i) != DELAY_TYPE_UNDEFINED) {
      BlastFinalResults (MrcData, Channel, i, 0);
      DisplayOrRestoreTrainingResults (MrcData, Channel, i, FALSE);
    }
  }
  //
  // Restore path values after PhyViewTable
  //
  MrcData->RestorePath = Restore;
#endif // PHYVIEW_TABLE
  return MMRC_SUCCESS;
}

/**
  Implements Diffamp, Odt and tRD Dynamic Formulas
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
PerformanceSetting (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if PERFORMANCE_SETTINGS
  UINT8  Rank;
  UINT8  Strobe;
  UINT16 TrainingDelay;
  UINT8  ElementIndex;
  UINT8  ThisSignalExists;
  UINT32 RcvnLinearValue;
  UINT32 MaxRcvn[MAX_STROBES];
  UINT32 MinRcvn[MAX_STROBES];
  UINT32 TempValue;
  UINT32 Denominator;
  UINT32 MaxtRDValue;
  UINT32 RcvEn1x;
  UINT32 RcvEn2x;
  UINT32 OdtDiffampDly[MAX_STROBES];
  UINT32 OdtDiffampLen[MAX_STROBES];

  if (!MrcData->PerformanceSettings) {
    return MMRC_SUCCESS;
  }

  //
  // Compute the maximum Receive Enable on each Rank.
  //
  Denominator = 2 * GetHalfClk (MrcData, Channel);
  TrainingDelay = GetOffsetIndex (MrcData, RecEnDelay);
  TrainingDelay = ((TrainingDelay - ALGO_REG_INDEX) * NumberOfElementsPerAlgo);
  //
  // Clear MaxtRDValue, MaxRcvn and MinRcvn to 0
  //
  MaxtRDValue = 0;
  RcvEn1x = 0;
  RcvEn2x = 0;
  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    MaxRcvn[Strobe] = 0;
    MinRcvn[Strobe] = 0;
  }

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    //
    // For each strobe, restore the Rcvn value and the minus1 select.
    //
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->RankEnabled[Channel][Rank]) {
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        ThisSignalExists = 0;
        RcvnLinearValue = 0;
        for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
          if (GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TrainingDelay + ElementIndex, CMD_GET_CACHE, &TempValue) == MMRC_SUCCESS) {
            RcvnLinearValue += TempValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]);
            ThisSignalExists = ElementIndex;
            if (ElementIndex == 0) {
              RcvEn1x = TempValue;
            }
            if (ElementIndex == 1) {
              RcvEn2x = TempValue;
            }
          }
        } // ElementIndex loop ...
        if (GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, SignalInfo[GetSignalInfoIndex (TrainingDelay)].Minus1Index, CMD_GET_CACHE, &TempValue) == MMRC_SUCCESS) {
          if (TempValue == 1) {
            RcvnLinearValue += GetHalfClk (MrcData, Channel);
          }
        } 
        //
        // Get the MAX
        //
        if (RcvnLinearValue > MaxRcvn[Strobe]) {
          MaxRcvn[Strobe] = RcvnLinearValue;
        }
        //
        // Get the MIN
        //
        if (MinRcvn[Strobe] == 0) {
          MinRcvn[Strobe] = RcvnLinearValue;
        } else if (RcvnLinearValue < MinRcvn[Strobe]) {
          MinRcvn[Strobe] = RcvnLinearValue;
        }
        TempValue = ((2 * (RcvEn1x + 3 + (RcvEn2x / 2) + 1)) + 1 + 1) / 2;
        if (TempValue > MaxtRDValue) {
          MaxtRDValue = TempValue;
        }
      }
    } // Rank loop ...
  } // Strobe loop ...

  MaxtRDValue = ((2 * (MaxtRDValue + 2 - 4) + 1 + 1) / 2) + 1;

  //
  // Rank is Don't care as the formula signals are per channel and strobe
  //
  Rank = 0;

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    //
    // MCHODT/Diffampen Launch Formula:
    // 2 is because we are pulling back from RCVEN value
    //
    OdtDiffampDly[Strobe] = (MinRcvn[Strobe] / Denominator) - 2;

    //
    // MCHODT/Diffampen Length Formula:
    //
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr3L){
      //
      // 2 is because we are pulling back from RCVEN value
      // 5 is to cover the entire Read burst (BL8)
      // (2 + 5 + "2") extra "2" is for LPDDR3 tDQSCK variation of 2.3ns
      // (2 + 5 + 2 + "1") extra "1" is for guardband
      //
    } else {  
      OdtDiffampLen[Strobe] = (UINT32)(((MaxRcvn[Strobe] - MinRcvn[Strobe]) / Denominator));
      //
      // 2 is because we are pulling back from RCVEN value
      // 5 is to cover the entire Read burst (BL8)
      // 2 is for guardband
      //
      OdtDiffampLen[Strobe] = OdtDiffampLen[Strobe] + 2 + 5 + 2;
    }
  }

  MspDebugPrint ((MSP_DBG_MIN, "\nSignal    CRS  Value\n"));
  MspDebugPrint ((MSP_DBG_MIN, "------    ---  -----\n"));

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    //
    // Program Max tRD value to all strobes
    //
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Trd, CMD_SET_VAL_FC, &MaxtRDValue);
    //
    // Perform a FIFO Reset after programmed tRD
    //
    MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
    MspDebugPrint ((MSP_DBG_MIN, "tRD       %d-%d  %02d (DEC)\n", Channel, Strobe, MaxtRDValue));
  }

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, DiffampDelay, CMD_SET_VAL_FC, &OdtDiffampDly[Strobe]);
    MspDebugPrint ((MSP_DBG_MIN, "DiffDly   %d-%d  %02d (DEC)\n", Channel, Strobe, OdtDiffampDly[Strobe]));
  }

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, DiffampLength, CMD_SET_VAL_FC, &OdtDiffampLen[Strobe]);
    MspDebugPrint ((MSP_DBG_MIN, "DiffLen   %d-%d  %02d (DEC)\n", Channel, Strobe, OdtDiffampLen[Strobe]));
  }

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, MchOdtDelay, CMD_SET_VAL_FC, &OdtDiffampDly[Strobe]);
    MspDebugPrint ((MSP_DBG_MIN, "OdtDly    %d-%d  %02d (DEC)\n", Channel, Strobe, OdtDiffampDly[Strobe]));
  }

  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
      continue;
    }
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, MchOdtLength, CMD_SET_VAL_FC, &OdtDiffampLen[Strobe]);
    MspDebugPrint ((MSP_DBG_MIN, "OdtLen    %d-%d  %02d (DEC)\n", Channel, Strobe, OdtDiffampLen[Strobe]));
  }
  MspDebugPrint ((MSP_DBG_MIN ,"\n"));
#endif // PERFORMANCE_SETTINGS  
  return MMRC_SUCCESS;
}
/**
  It will generate a reset signal for both channels with the current settings for each Channel

  @param[in, out]  MrcData         Host structure for all data related to MMRC.

  @retval          MMRC_SUCCESS
**/
VOID
ReInitializeDramsAcrossChannels(
  IN OUT    MMRC_DATA     *MrcData
)
{
  UINT8   IndCh;
  UINT8   IndRk;
  UINT8   IndSt;
  UINT32  Value;
  BOOLEAN BackupEnable;

  BackupEnable = MrcData->MspData.Enabled[0];
  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (!MrcData->MrcParameters.ChEnabled[IndCh]) {
      continue;
    }
    MmrcExecuteTask (MrcData, FIFO_RESET_ENABLE, NO_PRINT, IndCh);
    MspDelay(NANO_DEL, 100);
    MmrcExecuteTask (MrcData, FIFO_RESET_DISABLE, NO_PRINT, IndCh);
  }


  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (!MrcData->MrcParameters.ChEnabled[IndCh]) {
      continue;
    }
    MmrcExecuteTask (MrcData, DISABLE_CKE, SearchSubtaskName (DISABLE_CKE), IndCh);
  }
  MrcData->MspData.Enabled[0] = TRUE;
  MmrcExecuteTask (MrcData, RESETDRAMS_COMMON_SET, SearchSubtaskName(RESETDRAMS_COMMON_SET), 0);
  if (MrcData->SecondTimeExecution) {
    MspDebugPrint ((MSP_DBG_TRACE, "DELAY: %dns\n", 100));
    MspDelay(NANO_DEL, 100);
  } else {
    MspDebugPrint ((MSP_DBG_TRACE, "DELAY: %dus\n", 200));
    MspDelay(NANO_DEL, 200000);
    MrcData->SecondTimeExecution = TRUE;
  }
  MmrcExecuteTask (MrcData, RESETDRAMS_COMMON_CLEAR, SearchSubtaskName(RESETDRAMS_COMMON_CLEAR), 0);
  MrcData->MspData.Enabled[0] = BackupEnable;
  //
  // Timing already included in the RESETDRAMS_COMMON task (500 us)
  //
  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (!MrcData->MrcParameters.ChEnabled[IndCh]) {
      continue;
    }
    //
    // CKE Low. SMBus directly to write RC0A and RC3x with CKE low.
    //
    //MmrcExecuteTask(MrcData, DISABLE_CKE, SearchSubtaskName(DISABLE_CKE), IndCh);
    //SetRdimmRegCkeLow(MrcData, IndCh);
    //
    // This is a WA in order to send the CS as Low when doing the Phase 0 of the RCD
    //
    MmrcExecuteTask (MrcData, ENABLE_RELEASE_CKE, SearchSubtaskName (ENABLE_RELEASE_CKE), IndCh);
  }
  //
  // Wait to tStab 5uS
  //
  MspDelay(NANO_DEL, 5000);
  
  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (!MrcData->MrcParameters.ChEnabled[IndCh]) {
      continue;
    }
    //
    // Execute the JEDEC init sequence
    //
    JedecInitFunction (MrcData, 0, 0, IndCh);
    //
    // Wait 100 us before executing the PDA
    //
    MspDelay (NANO_DEL, 100000);

    MrcData->PdaEnable = TRUE;
    if (MrcData->ExecuteTxVrefSetup) {
      if (MrcData->MrcParameters.DramType[IndCh] == TypeDdr4) {
        for (IndRk = 0; IndRk < MAX_RANKS; IndRk++) {
          if (!MrcData->RankEnabled[IndCh][IndRk]) continue;
          for (IndSt = 0; IndSt < MAX_STROBES; IndSt++) {
            if (!IsStrobeValid (MrcData, IndCh, IndRk, IndSt, FALSE, NULL) ||
              ((MrcData->MaxDqCache[IndCh][IndRk] == MAX_STROBE_X8_DEVICES) && (IndSt > MAX_LOOP_STROBE_X8_DEVICES)) ||
              ((MrcData->PdaEnable == FALSE) && (IndSt > 0))) {
              continue;
            }
            //
            // Read from cache and write the MRS
            //
            Value = MrcData->CacheTxVref[IndCh][IndRk][IndSt];
            GetSetDdrioGroup2(MrcData, IndCh, IndRk, IndSt, TxVref, CMD_SET_VAL_FC_UC, &Value);
          }
        }
      } // RestoreTxVref
    }
  }
}

/**
  Prints the header for training results

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       SignalNameLength    
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DisplayTrainingResultsHeader (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         SignalNameLength
  )
{
  UINT8  ElementIndex;
  UINT8  i;

  MspDebugPrint ((MSP_DBG_MED, "\nSignal "));
  //
  // Start at 7 which is the length of "Signal ".
  //
  for (i = 7; i <= SignalNameLength; i++) {
    MspDebugPrint ((MSP_DBG_MED, " "));
  }
  MspDebugPrint ((MSP_DBG_MED, "CH RK ST BT HZ  "));
  for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
    MspDebugPrint ((MSP_DBG_MED, "%3d ", 
      Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]
      ));
  }
  MspDebugPrint ((MSP_DBG_MED, " M "));
  for (ElementIndex = 0; ElementIndex < NumberOfCCElements; ElementIndex++) {
    MspDebugPrint ((MSP_DBG_MED, "CC%d ", ElementIndex));
  }
  MspDebugPrint ((MSP_DBG_MED, "Vref Dly\n"));
  //
  // If the name of the signal (i.e. TX_DQ) is less than 6 characters, need to
  // draw extra "-" symbols to completely underline the word "Signal".
  //
  if (i < 8) {
    i = 8;
  }
  for (i; i > 2; i--) {
    MspDebugPrint ((MSP_DBG_MED, "-"));
  }
  if (SignalNameLength < 7) {
    MspDebugPrint ((MSP_DBG_MED, " -- -- -- -- --  "));
  } else {
    MspDebugPrint ((MSP_DBG_MED, "  -- -- -- -- --  "));
  }
  for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
    MspDebugPrint ((MSP_DBG_MED, "--- ", 
      Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]
      ));
  }
  //
  // Minus1 underline.
  //
  MspDebugPrint ((MSP_DBG_MED, " - "));
  for (ElementIndex = 0; ElementIndex < NumberOfCCElements; ElementIndex++) {
    MspDebugPrint ((MSP_DBG_MED, "--- ", ElementIndex));
  }
  //
  // Vref and delay underline.
  //
  MspDebugPrint ((MSP_DBG_MED, "---- ----\n"));

  return MMRC_SUCCESS;
}

/**
  This does a copy from the register to CACHE (Trained_Value) if the


  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       TrainingDelay   Training we want to print delay.
  @param[in]       BlastDirection  0: From REG to CACHE
                                   1: From CACHE to REG
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
BlastFinalResults (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        TrainingDelay,
  IN        UINT8         BlastDirection
)
{
  UINT8   ChannelBackup;
  UINT8   Rank;
  UINT8   Strobe;
  UINT8   Bit;
  UINT32  Value;
  UINT16  ThisSignalExists;
  UINT8   ElementIndex;
  BOOLEAN BitDelay;
  UINT8   MaxBits;
  UINT32  TempValue;
  UINT16  TrainingDelayStartElement;
  UINT16  Type;

  ChannelBackup = Channel;

  TrainingDelay = GetOffsetIndex (MrcData, TrainingDelay);

  if (TrainingDelay >= ALGO_REG_INDEX && TrainingDelay < SGTI_INDEX) {
    //
    // This is OK
    //
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "TrainingDelay %x\n", TrainingDelay));
    MrcData->ErrorCode = MrcSignalNotSupported;
    MrcDeadLoop ();
  }

  TrainingDelayStartElement  = TrainingDelay;
  TrainingDelayStartElement -= ALGO_REG_INDEX;
  TrainingDelayStartElement *= NumberOfElementsPerAlgo;

  ThisSignalExists = 0;
  for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
    //
    // Attempt to read the delay element, if successful accumulate to the value.
    //
    if (GetSetDdrioGroup2 (MrcData, Channel, 0, 0, TrainingDelayStartElement + ElementIndex, CMD_GET_REG, &TempValue) == MMRC_SUCCESS) {
      //
      // Since some delays may not have all element indices (i.e. RX_DQS may not have 1x or 2x...only pi),
      // we need to store the existing delay index so we can use it later to lookup the Minus1 and Vref value.
      //
      ThisSignalExists = TrainingDelayStartElement + ElementIndex;
      break;
    }
  }
  if (ThisSignalExists >= MAX_NUM_ELEMENTS) {
    CheckAdvConstrains (MrcData, "ThisSignalExists >= MAX_NUM_ELEMENTS", __FUNCTION__, __LINE__);
  }

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] || !MrcData->MspData.ExecuteOnThisChannel[Channel]) {
      continue;
    }
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->RankEnabled[Channel][Rank]) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, TRUE, &MaxBits)) {
//            continue;
          }
          if (ThisSignalExists != 0 && Elements[ThisSignalExists].ElementAttributes.Bits.BitBased) {
            BitDelay  = TRUE;
          } else {
            BitDelay  = FALSE;
            MaxBits   = 1;
          }
          for (Bit = 0; Bit < MaxBits; Bit++) {
            if (BlastDirection == 0) {
              GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay, CMD_GET_REG_UC, &Value);
            } else {
              GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay, RD_ONLY, &Value);
              if (BitDelay) {
                Type = GetOffsetIndex (MrcData, TrainingDelay);
                GetLinearFromRegs(MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, Type, RD_ONLY, &Value);
              }
              GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay, CMD_SET_VAL_FC_UC, &Value);
            }
          } // Bit loop ...
        } // Strobe loop ...
      } // MrcData->RankEnabled[Channel][Rank]
    } // Rank loop ...
  } // Channel loop ...

  return MMRC_SUCCESS;
}

/**
  Prints the Training Results

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       TrainingDelay   Training we want to print delay.
  @param[in]       Restore         If set the function will restore training values form MRC's cache
                                   else it will display the training results.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DisplayOrRestoreTrainingResults (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        TrainingDelay,
  IN        BOOLEAN       FaultyPartProcess
  )
{
#if USE_GET_SET_FROM_SPREADSHEET
  UINT8  Rank;
  UINT8  Strobe;
  UINT8  Bit;
  UINT32 TempValue;
  UINT32 RecalculatedDelay;
  UINT8  ElementIndex;
  UINT16 OrgTrainingDelay;
  UINT8  AccessMethod;
  UINT16 ThisSignalExists;
  UINT8  ChannelBackup;
  UINT8  ThisCcRange;
  UINT16 CCValue;
  UINT16 CCRange;
  BOOLEAN BitDelay;
  UINT8   MaxBits;

  ChannelBackup = Channel;
  CCValue       = 0;
  //
  // Initialize element values and indexes based on the elements structure array
  //  
  OrgTrainingDelay = TrainingDelay;
  TrainingDelay = GetOffsetIndex (MrcData, TrainingDelay);

  if (TrainingDelay == DELAY_TYPE_UNDEFINED) {
    return MMRC_SUCCESS;
  }

  if (MrcData->RestorePath) {
    AccessMethod = CMD_GET_CACHE;
    BlastFinalResults (MrcData, Channel, TrainingDelay, 1);
  } else {
    AccessMethod = CMD_GET_REG_UC;
    BlastFinalResults (MrcData, Channel, TrainingDelay, 0);
  }

  TrainingDelay -= ALGO_REG_INDEX;
  TrainingDelay *= NumberOfElementsPerAlgo;

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] || !MrcData->MspData.ExecuteOnThisChannel[Channel]) {
      continue;
    }
    //
    // Compute the starting index for the linear delay elements
    //
    DisplayTrainingResultsHeader (MrcData, Channel, (UINT8)StringLength (SignalInfo[GetSignalInfoIndex (TrainingDelay)].Name));
    ThisSignalExists = 0;
    for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
      //
      // Attempt to read the delay element, if successful accumulate to the value.
      //
      if (GetSetDdrioGroup2 (MrcData, Channel, 0, 0, TrainingDelay + ElementIndex, CMD_GET_REG, &TempValue) == MMRC_SUCCESS) {
        //
        // Since some delays may not have all element indices (i.e. RX_DQS may not have 1x or 2x...only pi),
        // we need to store the existing delay index so we can use it later to lookup the Minus1 and Vref value.
        //
        ThisSignalExists = TrainingDelay + ElementIndex;
        break;
      }
    }

    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->RankEnabled[Channel][Rank]) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, TRUE, &MaxBits)) {
//            continue;
          }
          if (ThisSignalExists != 0 && Elements[ThisSignalExists].ElementAttributes.Bits.BitBased) {
            BitDelay  = TRUE;
          } else {
            BitDelay  = FALSE;
            MaxBits   = 1;
          }
          for (Bit = 0; Bit < MaxBits; Bit++) {
            //
            // Print out the Label for the row.
            //
            RecalculatedDelay = 0;

            MspDebugPrint ((
              MSP_DBG_MED,
              "%s  %02d %02d %02d %02d %02d   ",
              SignalInfo[GetSignalInfoIndex (TrainingDelay)].Name,
              Channel,
              Rank,
              Strobe,
              Bit,
              SignalInfo[GetSignalInfoIndex (TrainingDelay)].FrequencyIndex
            ));

            for (ElementIndex = 0; ElementIndex < NumberOfDelayElementsPerAlgo; ElementIndex++) {
              //
              // Attempt to read the delay element, if successful accumulate to the value.
              //
              if (GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay + ElementIndex, AccessMethod, &TempValue) == MMRC_SUCCESS) {
                RecalculatedDelay += TempValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex]);
                if (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][ElementIndex] == 1) {
                  CCValue = (UINT16)TempValue;
                }

                MspDebugPrint ((MSP_DBG_MED, "%02d  ", TempValue));
                if (MrcData->RestorePath) {
                  GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay + ElementIndex, CMD_SET_VAL_FC_UC, &TempValue);
                }
              } else {
                MspDebugPrint ((MSP_DBG_MED, "--  "));
              }
            } // ElementIndex loop ...
            //
            // Get/print/save the Minus1.
            //
            if (GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, SignalInfo[GetSignalInfoIndex (TrainingDelay)].Minus1Index, AccessMethod, &TempValue) == MMRC_SUCCESS) {

              if (TempValue == 1) {
                RecalculatedDelay += GetHalfClk (MrcData, Channel);
              }
              MspDebugPrint ((MSP_DBG_MED, "%d ", TempValue));
              if (MrcData->RestorePath) {
                GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, SignalInfo[GetSignalInfoIndex (TrainingDelay)].Minus1Index, CMD_SET_VAL_FC_UC, &TempValue);
              }
            } else {
              MspDebugPrint ((MSP_DBG_MED, "- "));
            }
            //
            // Get/print/save the CCs.
            //
            CCRange = GetHalfClk(MrcData, Channel);
            for (ElementIndex = 0; ElementIndex < NumberOfCCElements; ElementIndex++) {
              if (GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay + NumberOfDelayElementsPerAlgo + ElementIndex, AccessMethod, &TempValue) == MMRC_SUCCESS) {
                MspDebugPrint ((MSP_DBG_MED, "  %d ", TempValue));
                if (MrcData->RestorePath) {
                  GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, TrainingDelay + NumberOfDelayElementsPerAlgo + ElementIndex, CMD_SET_VAL_FC_UC, &TempValue);
                }
              } else {
                MspDebugPrint ((MSP_DBG_MED, "  - "));
              }
              //
              // If this clock crossing is modifying a delay register (such as the 2x clock), then we need to
              // undo the clock crossing modification when returning the total linear value. For example, if 2x clock
              // the 7 and the clock crossing modifies this to be 6, then we need to return it back to 7 when
              // calculating the final linear value. GranularityValid == 1 is the flag for this condition.
              //
              if (Elements[TrainingDelay + NumberOfDelayElementsPerAlgo + ElementIndex].ElementAttributes.Bits.GranularityValid == 1) {
                TempValue = 0;
                //
                // Check if the value of the PI was within the min/max percentage for that clock crossing.  If it
                // is then set the CC to the "invalue"
                //
                for (ThisCcRange = 0; ThisCcRange < NumberOfCCRanges; ThisCcRange++) {
                  if ((ElementIndex == ClockCrossings[ThisCcRange].CcNumber) &&
                    (CCValue < (UINT32)(CCRange * ClockCrossings[ThisCcRange].MaxPercent / 100)) &&
                    (CCValue >= (UINT32)(CCRange * ClockCrossings[ThisCcRange].MinPercent / 100))) {
                    TempValue = (UINT32)ClockCrossings[ThisCcRange].InValue;
                    break;
                  }
                }
                RecalculatedDelay -= TempValue * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][Elements[TrainingDelay + NumberOfDelayElementsPerAlgo + ElementIndex].ElementAttributes.Bits.GranularityValue]);
              }
            }
            //
            // Get/print/save the Vref.
            //
            if (GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, SignalInfo[GetSignalInfoIndex (TrainingDelay)].VrefIndex, AccessMethod, &TempValue) == MMRC_SUCCESS) {
              MspDebugPrint ((MSP_DBG_MED, "%04d ", TempValue));
              if (MrcData->RestorePath) {
                GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, SignalInfo[GetSignalInfoIndex (TrainingDelay)].VrefIndex, CMD_SET_VAL_FC_UC, &TempValue);
              }
            } else {
              MspDebugPrint ((MSP_DBG_MED, "  -- "));
            }
            //
            // Print and save the recalculated delay value.
            //
            MspDebugPrint ((MSP_DBG_MED, "%04d\n", RecalculatedDelay));
          } // Bit loop ...
          BreakStrobeLoop (MrcData, OrgTrainingDelay, &Strobe);
        } // Strobe loop ...
      }
    } // Rank loop ...

#if FAULTY_PART_TRACKING
    if (FaultyPartProcess && !MrcData->RestorePath) {
      FaultyPartProcessTrainingResult (MrcData, Channel);
    }
#endif
  } // Channel loop ...
#endif
  return MMRC_SUCCESS;
}

//
// These functions are unused right now, but we will likely bring them back in the future when
// we add SSA support to the MMRC.
//
#if 0
MMRC_STATUS
HandleDdrioPhyInit (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        DummyChannel
)
{
  MMRC_STATUS  Status;

  //
  // Single entry point to MMRC to initialize the DDRIO PHY.
  //
  Status = MmrcEntry (MrcData, DdrioPhyInit, NULL);
  return Status;
}

MMRC_STATUS
HandleMemoryTraining (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        DummyChannel
)
{
  MMRC_STATUS  Status;

  //
  // Single entry point to MMRC to train memory.
  //
  Status = MmrcEntry (MrcData, MemoryTraining, NULL);

  return Status;
}

#endif // 0

/**
  Decodes and executes the Capsule initialization. 
  The routine starts at a beginning index in the CapsuleInit 
  compressed data structure and continues until a ASSIGN_DONE 
  flag is detected. 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MmrcExecuteTask (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
  return ((MSP_STATUS) MspExecuteTask (&MrcData->MspData, 0, CapsuleStartIndex, StringIndex, Channel));
}
#if TRAINING_ALGOS
/**
  Translates the value according to the absolute/relative flag

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Start           Start. 
  @param[in]       End             End.   
  @param[in]       Flag            ABSOLUTE/RELATIVE Flag.
  @param[in]       Min             Returned Start.
  @param[in]       Max             Returned End.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetMinMaxRelativeAbsolute (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Index,
  IN   INT16        Start,
  IN   INT16        End,
  IN   UINT8        Flag,
  IN   UINT16       Min[MAX_CHANNELS][MAX_STROBES],
  IN   UINT16       Max[MAX_CHANNELS][MAX_STROBES]
  )
{
  UINT8   Strobe;
  UINT32  TempValue;

#if COMMAND_TRAINING == 1
  if (Index == CmdAll) {
    Index = CmdGrp0;
  }
#endif
  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    //
    // Get the Min/Max values for each strobe according ABSOLUTE/RELATIVE Flag
    //
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
      if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
        continue;
      }
      if (Flag & RELATIVE) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Index, CMD_GET_CACHE, &TempValue);
        if (((INT16)TempValue + Start) < 0) {
          Min[Channel][Strobe] = 0;
        } else {
          Min[Channel][Strobe] = (UINT16) TempValue + Start;
        }
        if (((INT16)TempValue + End) < 0) {
          Max[Channel][Strobe] = 0;
        } else {
          Max[Channel][Strobe] = (UINT16) TempValue + End;
        }
      } else {
        Min[Channel][Strobe] = Start;
        Max[Channel][Strobe] = End;
      }
    } // Strobe loop ...
  } // Channel loop ...
  return MMRC_SUCCESS;
}
#endif

#if TRAINING_ALGOS == 1
MMRC_STATUS
Center2D (
  IN          MMRC_DATA       *MrcData,
  IN   OUT    SAMPLE_SWEEP    *Results,
  IN          UINT8           Channel,
  IN          UINT8           Rank,
  IN          UINT16          Dim1Index,
  IN          UINT8           Dim1Knobs,
  IN          UINT16          Dim1Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim2Index,
  IN          UINT16          Dim2Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim2Step,
  IN          UINT8           Dim2ConsolidateStrobes
  )
{
  UINT8           Strobe;
  UINT8           StrobeLoopCount;
  UINT16          AxisYCounter;
  UINT16          AxisXCounter;
  UINT8           Knob;
  UINT16          MaxMarginInX;
  UINT32          CenterX[MAX_KNOBS];
  UINT32          CenterY;
  UINT32          CenterYAligned;
  SAMPLE_SWEEP    ResultsComposite[MAX_ELEMENTS_DIM_Y];

  UINT16  EyeWidth[MAX_ELEMENTS_DIM_Y];
  UINT16  EyeHeight;
  UINT16  MaxEyeHeight[MAX_KNOBS][MAX_STROBES];
  UINT16  EyeWidthAtCenter[MAX_KNOBS];

  UINT32  AccumulateMarginFactor;
  UINT32  AccumulateMargin;

  UINT32  SumWeights;
  UINT32  SumLR;

  StrobeLoopCount = 0;
  CenterY = 0;
  CenterYAligned = 0;

  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    //
    // Build a composite of the Results for the calculation of the Y Center, this composite is the combinations of all knobs.
    //
    MmrcMemset (ResultsComposite, 0xff, MAX_ELEMENTS_DIM_Y * sizeof (SAMPLE_SWEEP));
    for (AxisYCounter = 0; AxisYCounter < MAX_ELEMENTS_DIM_Y; AxisYCounter++) {
      for  (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          if (ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][LOW] == 0xffff 
               || ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][LOW] < Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][LOW]
             )
          {
              ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][LOW] = Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][LOW];
          }
          if (ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][HIGH] == 0xffff
               || ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][HIGH] > Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][HIGH]
             )
          {
              ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][HIGH] = Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][HIGH];
          }
        }
      }
    }
    //
    // Build a composite of the Results for the calculation of the Y Center, this composite is the combinations of all strobes.
    //
    if (Dim2ConsolidateStrobes == 1) {
      MmrcMemset (ResultsComposite, 0xff, MAX_ELEMENTS_DIM_Y * sizeof (SAMPLE_SWEEP));
      for (AxisYCounter = 0; AxisYCounter < MAX_ELEMENTS_DIM_Y; AxisYCounter++) {
        StrobeLoopCount = 1;
        for (Knob = 0; Knob < Dim1Knobs; Knob++) {
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
           if (ResultsComposite[AxisYCounter].Sample[Channel][Knob][0][0][LOW] == 0xffff 
                || ResultsComposite[AxisYCounter].Sample[Channel][Knob][0][0][LOW] < Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][LOW]
              )
            {
                ResultsComposite[AxisYCounter].Sample[Channel][Knob][0][0][LOW] = Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][LOW];
            }
            if (ResultsComposite[AxisYCounter].Sample[Channel][Knob][0][0][HIGH] == 0xffff 
                 || ResultsComposite[AxisYCounter].Sample[Channel][Knob][0][0][HIGH] > Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][HIGH]
               )
            {
                ResultsComposite[AxisYCounter].Sample[Channel][Knob][0][0][HIGH] = Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][HIGH];
            }
          }
        }
      }
    } else {
      StrobeLoopCount = MrcData->MaxDq[Channel][Rank];
    }

    //
    // Fill results across Y with the Margins in X, this table will be used to calculate Center in Y
    //
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
      if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
        continue;
      }
      if (Strobe < StrobeLoopCount) {
        MmrcMemset (EyeWidth, 0xff, MAX_ELEMENTS_DIM_Y * sizeof (UINT16));

        MaxMarginInX = 0;
        for (AxisYCounter = 0; AxisYCounter < MAX_ELEMENTS_DIM_Y; AxisYCounter++) {
          if (ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][HIGH] < ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][LOW]) {
            EyeWidth[AxisYCounter] = 0;
            continue;
          }
          EyeWidth[AxisYCounter] = ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][HIGH] - 
                                    ResultsComposite[AxisYCounter].Sample[Channel][0][Strobe][0][LOW];
          //
          // Get the MaxMargin in X
          //
          if (MaxMarginInX < EyeWidth[AxisYCounter]) {
            MaxMarginInX = EyeWidth[AxisYCounter];
          }
        } // AxisYCounter loop ...
        //
        // Calculate the accumulate MarginInX * (Axis Y) and accumulate MarginInX
        // only if MarginInX > PASS_MARGIN(MaxMarginInX)
        // The basic formula being utilized is "Cum(EW*VREF)/Cum(EW)".
        //
        AccumulateMarginFactor  = 0;
        AccumulateMargin        = 0;
        for (AxisYCounter = 0; AxisYCounter < MAX_ELEMENTS_DIM_Y; AxisYCounter++) {
          if (EyeWidth[AxisYCounter] >= PASS_MARGIN_AXIS_X (MaxMarginInX)) {
            AccumulateMarginFactor  += EyeWidth[AxisYCounter] * (AxisYCounter + Dim2Min[Channel][Strobe]);
            AccumulateMargin        += EyeWidth[AxisYCounter];
            MspDebugPrint ((MSP_DBG_TST, "  Y[%d]AMF: %d AM: %d\n", 
              AxisYCounter, AccumulateMarginFactor, AccumulateMargin
              ));
          }
        } // AxisYCounter loop ...
        if (AccumulateMargin == 0) {
          MspDebugPrint ((MSP_DBG_MIN, "Axis Width 0\n"));
          return MrcErrCenter2DAccMarginXZero;
        }
        CenterY = (AccumulateMarginFactor + (AccumulateMargin / 2)) / AccumulateMargin;
        MspDebugPrint ((MSP_DBG_TST, "  Y: AMF: %d AM: %d CenterY: %d\n", 
          AccumulateMarginFactor, AccumulateMargin, CenterY
          ));
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Dim2Index, CMD_SET_VAL_FC_UC, &CenterY);
  
      //
      // Calculate the maximum eye height from this data.
      //
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        MaxEyeHeight[Knob][Strobe] = 0;
        for (AxisXCounter = 0; AxisXCounter < MAX_ELEMENTS_DIM_X; AxisXCounter++) {
          EyeHeight = 0;
          for (AxisYCounter = 0; AxisYCounter < MAX_ELEMENTS_DIM_Y; AxisYCounter++) {
            if (AxisXCounter >= Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][LOW] &&
                AxisXCounter <= Results[AxisYCounter].Sample[Channel][Knob][Strobe][0][HIGH]) {
               EyeHeight += Dim2Step;
            }
          }
          if (EyeHeight > MaxEyeHeight[Knob][Strobe]) {
            MaxEyeHeight[Knob][Strobe] = EyeHeight;
          }
        }
      }
      //
      // Get the nearest VREF that we can use for the center.
      //

      CenterYAligned = (UINT16)((CenterY * 10 + 5) / 10) ;
      CenterYAligned -= Dim2Min[Channel][Strobe];
      CenterYAligned /= Dim2Step;
      CenterYAligned *= Dim2Step;
    }  
      //
      // Calculate the Center X for each Knob.
      //
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        EyeWidthAtCenter[Knob] = Results[CenterYAligned].Sample[Channel][Knob][Strobe][0][HIGH] - Results[CenterYAligned].Sample[Channel][Knob][Strobe][0][LOW];
        SumLR  = (  
                   ( 3 * MaxEyeHeight[Knob][Strobe] + 0 * EyeWidthAtCenter[Knob] ) * 
                      ( Results[CenterYAligned + 0 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + Results[CenterYAligned - 0 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + 
                        Results[CenterYAligned + 0 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  + Results[CenterYAligned - 0 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  
                      )
                  );
        SumWeights = 2*( 3 * MaxEyeHeight[Knob][Strobe] + 0 * EyeWidthAtCenter[Knob] );

        if ((CenterYAligned + 1 * Dim2Step < MAX_ELEMENTS_DIM_Y && Results[CenterYAligned + 1 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] != 0xffff) && 
            ((INT32)CenterYAligned - 1 * Dim2Step >= 0 && Results[CenterYAligned - 1 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] != 0xffff)) {
            SumLR += (  
                       ( 2 * MaxEyeHeight[Knob][Strobe] + 1 * EyeWidthAtCenter[Knob] ) * 
                       (Results[CenterYAligned + 1 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + Results[CenterYAligned - 1 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] +
                            Results[CenterYAligned + 1 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  + Results[CenterYAligned - 1 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  
                          )
                      );
            SumWeights +=  2*( 2 * MaxEyeHeight[Knob][Strobe] + 1 * EyeWidthAtCenter[Knob] ) ;
        }

        if ((CenterYAligned + 2 * Dim2Step < MAX_ELEMENTS_DIM_Y && Results[CenterYAligned + 2 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] != 0xffff) && 
            ((INT32) CenterYAligned - 2 * Dim2Step >= 0 && Results[CenterYAligned - 2 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] != 0xffff)) {
          SumLR += (  
                     ( 1 * MaxEyeHeight[Knob][Strobe] + 2 * EyeWidthAtCenter[Knob] ) * 
                        ( Results[CenterYAligned + 2 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + Results[CenterYAligned - 2 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + 
                          Results[CenterYAligned + 2 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  + Results[CenterYAligned - 2 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  
                        )
                    );
          SumWeights += 2*( 1 * MaxEyeHeight[Knob][Strobe] + 2 * EyeWidthAtCenter[Knob] );
        }
        if ((CenterYAligned + 3 * Dim2Step < MAX_ELEMENTS_DIM_Y && Results[CenterYAligned + 3 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] != 0xffff) && 
            ((INT32)CenterYAligned - 3 * Dim2Step >= 0 && Results[CenterYAligned - 3 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] != 0xffff)) {
            SumLR += (  
                       ( 0 * MaxEyeHeight[Knob][Strobe] + 3 * EyeWidthAtCenter[Knob] ) * 
                          ( Results[CenterYAligned + 3 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + Results[CenterYAligned - 3 * Dim2Step].Sample[Channel][Knob][Strobe][0][HIGH] + 
                            Results[CenterYAligned + 3 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  + Results[CenterYAligned - 3 * Dim2Step].Sample[Channel][Knob][Strobe][0][LOW]  
                          )
                      );
            SumWeights  += 2*( 0 * MaxEyeHeight[Knob][Strobe] + 3 * EyeWidthAtCenter[Knob] );
        }
        if (SumWeights != 0) {
          CenterX[Knob] = (SumLR / SumWeights) / 2;
        } else {
          CenterX[Knob] = 0;
        }
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Dim1Index + Knob, CMD_SET_VAL_FC_UC, &CenterX[Knob]);
      }

      // 
      // If consolation of the strobes are being done, then store the same center to DIM2, so that Create2DEye can read it directly
      // from cache.
      //
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Dim2Index, CMD_SET_VAL_FC_UC, &CenterY);
    } // Strobe loop ...
  } // Channel loop ...

  return MMRC_SUCCESS;
}
#endif // TRAINING_ALGOS == 1

#if TRAINING_ALGOS == 1

/**
  If PerBitSweep == TRUE:
    - It will update the result only for the passing bits

  If PerBitSweep == FALSE:
    - It will propagate the same result from BIT0 across all bits within the strobe

  Note: Results[0] keeps tracking of the offset being tested across all bits within the same strobe

  @param[in, out]  MrcData          Host structure for all data related to MMRC.
  @param[in]       MaximumBits      Max bits per strobe
  @param[in]       PerBitSweep      Indication if we are running per bit (PerBitSweep = TRUE) or per strobe (PerBitSweep = FALSE)
  @param[in]       LowHighFlag      LOW or HIGH
  @param[in]       PassFailPerBit   Array per strobe which contains the information per bit of the CURRENT_SAMPLE, FIRST_SAMPLE and EDGE_SAMPLES
  @param[in, out]  Results          Results array returning the high/low edges per strobe.

  @retval          NONE
**/
VOID
SpreadBitResults (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        MaximumBits,
  IN         BOOLEAN      PerBitSweep,
  IN         UINT8        LowHighFlag,
  IN         UINT8        (*PassFailPerBit)[3],
  IN   OUT   UINT16       (*Results)[2],
  IN   OUT   UINT16       (*ResultsB0Backup)
  )
{
  UINT8     Bits;
  UINT16    Bit0Result[2];

  Bit0Result[LowHighFlag]   = Results[0][LowHighFlag];

  for (Bits = 1; Bits < MaximumBits; Bits++) {
    if (PerBitSweep) {
      if (PassFailPerBit[Bits][CURRENT_SAMPLE] == RESULTS_PASS) {
        Results[Bits][LowHighFlag] = Bit0Result[LowHighFlag];
      }
    } else {
      Results[Bits][LowHighFlag] = Bit0Result[LowHighFlag];
    }
  }
  if (PassFailPerBit[0][CURRENT_SAMPLE] == RESULTS_PASS) {
    ResultsB0Backup[LowHighFlag] = Bit0Result[LowHighFlag];
  }
}

/**
  If PerBitSweep == TRUE:
    - We will continue running the 1DSweep until all bits in the strobe fail.

  If PerBitSweep == FALSE:
    - We will continue running the 1DSweep until all bits in the strobe pass.

  The number EDGE_SAMPLES is determined by the minimum value across all bits within the strobe

  @param[in, out]  MrcData          Host structure for all data related to MMRC.
  @param[in]       MaximumBits      Max bits per strobe
  @param[in]       PerBitSweep      Indication if we are running per bit (PerBitSweep = TRUE) or per strobe (PerBitSweep = FALSE)
  @param[in]       PassFailPerBit   Array per strobe which contains the information per bit of the CURRENT_SAMPLE, FIRST_SAMPLE and EDGE_SAMPLES
  @param[in, out]  SampleIndex      CURRENT_SAMPLE, FIRST_SAMPLE or EDGE_SAMPLES

  @retval          UINT8
**/
UINT8
DeterminePassFailPerStrobe (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        MaximumBits,
  IN         BOOLEAN      PerBitSweep,
  IN         UINT8        (*PassFailPerBit)[3],
  IN         UINT8        SampleIndex
  )
{
  UINT8     Bits;
  UINT8     PassFailPerStrobe;

  if (PerBitSweep) {
    PassFailPerStrobe = RESULTS_FAIL;
  } else {
    PassFailPerStrobe = RESULTS_PASS;
  }
  if (SampleIndex == EDGE_SAMPLES) {
    PassFailPerStrobe = 0xFF;
  }

  for (Bits = 0; Bits < MaximumBits; Bits++) {
    if (SampleIndex == CURRENT_SAMPLE || SampleIndex == FIRST_SAMPLE) {
      if (PassFailPerBit[Bits][SampleIndex] == RESULTS_PASS && PerBitSweep) {
        //
        // With PerBit, it is required to continue sweeping even when a single bit passes
        //
        PassFailPerStrobe = RESULTS_PASS;
        break;
      } else if (PassFailPerBit[Bits][SampleIndex] == RESULTS_FAIL && !PerBitSweep) {
        //
        // Without PerBit, it is required to stop sweeping when a single bit fails
        //
        PassFailPerStrobe = RESULTS_FAIL;
        break;
      } else if (PassFailPerBit[Bits][SampleIndex] == RESULTS_INIT) {
        return RESULTS_INIT;
        break;
      }
    } else if (SampleIndex == EDGE_SAMPLES) {
      if (PassFailPerBit[Bits][SampleIndex] == RESULTS_INIT) {
        return RESULTS_INIT;
      } else if (PassFailPerBit[Bits][SampleIndex] < PassFailPerStrobe) {
        PassFailPerStrobe = PassFailPerBit[Bits][SampleIndex];
      }
    }
  }
  return PassFailPerStrobe;
}

/**
  If 'FlagDirection' == 0, it calculates the new Dim1Start for each strobe, it will pick a safe passing window in order to execute the PER_BIT stage
                           There MUST be a passing window per strobe if we want to execute PER_BIT
  If 'FlagDirection' == 1, it restores the passing window per strobe and store it in Bit 0 of each strobe array

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ChannelBackup   Current Channel being examined.
  @param[in]       Rank            Rank to sweep.
  @param[in]       Dim1Knobs       How many knobs does this signal have? Most are 1 but ones
                                   like RxDqsDelay can have a HI and LOW (or P and N) component.
                                   For this case, Dim1Knobs would be 2
  @param[in]       NumberElements  How many elements are we sweeping? Typically, MAX_STROBES for strobe based delays or 1
                                   for CMD signals or others which aren't strobe-based.
  @param[in, out]  Results         Results array returning the high/low edges per channel/knob/strobe.
  @param[in]       Dim1Start       Starting value (per channel/knob/strobe/edge) for both the
                                   high and low edges.
  @param[in]       Stage           Determines the function's behavior, valid parameters are:
                                   RUN_PER_STROBE - Detects margins within strobe
                                   RUN_PER_BIT    - Detects margins within bits
                                   RUN_PER_STROBE_AND_BIT - Detects margins first within strobe and then executes per bit (Note: Bit0
                                   will always contain the strobe result)
                                   RUN_PER_STROBE_BIT_LOCK (PerBitSweep must be TRUE to execute per bit) - Detects margins first within strobe and then executes per bit (Note: Bit0
                                   will always contain the strobe result) all in lock step mode
  @param[in]       FlagDirection
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CalculateOrRestoreResults (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        ChannelBackup,
  IN         UINT8        Rank,
  IN         UINT8        Dim1Knobs,
  IN         UINT8        NumberElements[MAX_CHANNELS],
  IN  OUT    UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN         UINT16       Dim1Start[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  IN         UINT8        FlagDirection,
  IN         UINT8        Stage
)
{
  UINT8   Channel;
  UINT8   Knob;
  UINT8   Element;
  UINT16  GetMinMax[2];

  MmrcMemset(&GetMinMax[LOW],  0x00, sizeof(UINT16));
  MmrcMemset(&GetMinMax[HIGH], 0xFF, sizeof(UINT16));

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    for (Knob = 0; Knob < Dim1Knobs; Knob++) {
      for (Element = 0; Element < NumberElements[Channel]; Element++) {
        if (!IsStrobeValid (MrcData, Channel, Rank, Element, FALSE, NULL)) {
          continue;
        }
        if (FlagDirection == 0) {
          if (Results[Channel][Knob][Element][0][HIGH] > Results[Channel][Knob][Element][0][LOW]) {
          } else {
            MspDebugPrint ((MSP_DBG_MAX, "Could not run per bit phase\n"));
            return MMRC_FAILURE;
          }
          Dim1Start[Channel][Knob][Element][LOW]  = Results[Channel][Knob][Element][0][LOW]  + MrcData->MaxThresholdPerBit;
          Dim1Start[Channel][Knob][Element][HIGH] = Results[Channel][Knob][Element][0][HIGH] - MrcData->MaxThresholdPerBit;
          if (GetMinMax[HIGH] >  Dim1Start[Channel][Knob][Element][HIGH]) {
            GetMinMax[HIGH] = Dim1Start[Channel][Knob][Element][HIGH];
          }
          if (GetMinMax[LOW] <  Dim1Start[Channel][Knob][Element][LOW]) {
            GetMinMax[LOW] = Dim1Start[Channel][Knob][Element][LOW];
          }
        } else {
          Results[Channel][Knob][Element][0][LOW]   = Dim1Start[Channel][Knob][Element][LOW]   - MrcData->MaxThresholdPerBit;
          Results[Channel][Knob][Element][0][HIGH]  = Dim1Start[Channel][Knob][Element][HIGH]  + MrcData->MaxThresholdPerBit;
        }
      } // Element loop...
    } // Knob loop ...
  } // Channel loop ...

  //
  // Get LOCK_STEP_MODE all element must start at the same value for the PER_BIT Phase
  //
  if ((Stage & RUN_LOCK_STEP_MODE) != 0) {
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Element, FALSE, NULL)) {
            continue;
          }
          if (FlagDirection == 0) {
            Dim1Start[Channel][Knob][Element][LOW]  = GetMinMax[LOW];
            Dim1Start[Channel][Knob][Element][HIGH] = GetMinMax[HIGH];
          }
        } // Element loop...
      } // Knob loop ...
    } // Channel loop ...
  }
  return MMRC_SUCCESS;
}

/**
  This routine sweeps any signal and returns the edges where the signal transitions from pass to fail.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Rank to sweep.
  @param[in]       Dim1Index       Signal type to sweep (i.e. RxDqsDelay, TxVref, CmdGrp0, etc).
  @param[in]       Dim1Knobs       How many knobs does this signal have? Most are 1 but ones 
                                   like RxDqsDelay can have a HI and LOW (or P and N) component. 
                                   For this case, Dim1Knobs would be 2
  @param[in]       Dim1Start       Starting value (per channel/knob/strobe/edge) for both the 
                                   high and low edges.
  @param[in]       Dim1Min         The minimum value to allow the sweep to hit before stopping.
  @param[in]       Dim1Max         The maximum value to allow the sweep to hit before stopping.
  @param[in]       Dim1Step        Step size for sweep. Set to 1 for maximum detail or higher for speed improvements
                                   at the expense of accuracy.
  @param[in]       FullSweep       THIS IS NOT CURRENTLY IMPLEMENTED. If TRUE, sweep the entire range from min to max.
  @param[in] SkipInitialValueProgramming When TRUE, do not program initial low edge starting values or high edge 
                                         starting values. This is useful when you want to margin a signal starting 
                                         at its current value or when margining a group of signals that need to stay 
                                         tethered together. For example, in CMD training if CmdGrp0 and CmdGrp1 need
                                         to sweep together but be programmed to different values (i.e. tethered), 
                                         this would be set the TRUE so the two groups aren't programmed to an identical'
                                         starting value. If the user wants to skip the initial value programming,
                                         they MUST assign Dim1Start[][][][] to the current register settings upon entry
                                         to this routine.
  @param[in]       NumberElements  How many elements are we sweeping? Typically, MAX_STROBES for strobe based delays or 1 
                                   for CMD signals or others which aren't strobe-based.
  @param[in]       ExecuteTest     Pointer to the point test function which evaluates pass/fail for the current value
                                   of the signal.
  @param[in, out]  Results         Results array returning the high/low edges per channel/knob/strobe.
  @param[in]       HeaderLabel     Label to print if debugging is turned on. If "S" then the header will be a strobe 
                                   header like S00   S01   S02, etc.
  @param[in]       DitherVal       The number of successive samples that need to be either PASS or FAIL before this routine
                                   considers an edge to be found. For example, if the first sample is FAIL, the sweep will
                                   look to read X number of PASS values in a row before it considers this a FAIL to PASS
                                   edge. X = DitherVal.
  @param[in]       Stage           Determines the function's behavior, valid parameters are:
                                   RUN_PER_STROBE - Detects margins within strobe
                                   RUN_PER_BIT    - Detects margins within bits
                                   RUN_PER_STROBE_AND_BIT - Detects margins first within strobe and then executes per bit (Note: Bit0
                                   will always contain the strobe result)
                                   RUN_PER_STROBE_BIT_LOCK (PerBitSweep must be TRUE to execute per bit) - Detects margins first within strobe and then executes per bit (Note: Bit0
                                   will always contain the strobe result) all in lock step mode
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
Create1DSweep (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT16       Dim1Index,
  IN         UINT8        Dim1Knobs,
  IN         UINT16       Dim1Start[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  IN         UINT16       Dim1Min[MAX_CHANNELS][MAX_STROBES],
  IN         UINT16       Dim1Max[MAX_CHANNELS][MAX_STROBES],
  IN         UINT16       Dim1Step,
  IN         BOOLEAN      FullSweep,
  IN         BOOLEAN      SkipInitialValueProgramming,
  IN         BOOLEAN      PerBitSweep,
  IN         UINT8        NumberElements[MAX_CHANNELS],
  IN         MMRC_STATUS  ExecuteTest (MMRC_DATA *, UINT8, UINT8, UINT8 NumberElements[MAX_CHANNELS], UINT8, UINT8 PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]),
  IN   OUT   UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN         UINT8        *HeaderLabel,
  IN         UINT8        DitherVal,
  IN         UINT8        Stage
  )
{
  UINT16    Dim1StartRestore[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT8     LowHighFlag[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES];
  UINT8     Element;
  UINT8     Knob;
  UINT8     PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3];
  UINT16    ResultsB0Backup[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT8     ChannelBackup;
  UINT32    Value[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES];
  UINT8     TestFinished;
  BOOLEAN   EdgeFound[MAX_CHANNELS][MAX_STROBES];
  BOOLEAN   ElementDone[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES];
  UINT8     Bits;
  UINT8     MaximumBits;
  BOOLEAN   PerBitSweepBackup;
  UINT8     LockStepRun;
  UINT8     IndCh;
  UINT8     IndSt;
  UINT16    PostTestFlags;
  BOOLEAN   LimitFound;
  MMRC_STATUS Status;
  
  MmrcMemCopy (Dim1StartRestore, Dim1Start, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * 2 * sizeof (UINT16));

  PerBitSweepBackup = PerBitSweep;  
  MaximumBits       = 0;
  LockStepRun       = 0;

  if ((Stage & RUN_PER_STROBE) == RUN_PER_STROBE) {
    PerBitSweepBackup = PerBitSweep;
    PerBitSweep       = FALSE;
    MrcData->CpgcOptions.RmtRestoreJedecResetType |= FAIL_SPREAD_BIT_RESULT;
    MspDebugPrint ((MSP_DBG_DEBUG, "Strobe Phase\n"));
  } else if ((Stage & RUN_PER_BIT) == RUN_PER_BIT) {
    PerBitSweepBackup = PerBitSweep;
    PerBitSweep       = TRUE;
    MrcData->CpgcOptions.RmtRestoreJedecResetType &= ~FAIL_SPREAD_BIT_RESULT;
    MspDebugPrint ((MSP_DBG_DEBUG, "Bit Phase\n"));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Invalid input parameters for Create1DSweep Stage: 0x%02x\n", Stage));
    return MMRC_SUCCESS;
  }

  PostTestFlags = MrcData->CpgcOptions.RmtRestoreJedecResetType;

  if ((Stage & RUN_LOCK_STEP_MODE) != 0) {
    LockStepRun = RUN_LOCK_STEP_MODE;
  }

  ChannelBackup = Channel;
  //
  // Print out the header.
  //
  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (*HeaderLabel != 0 && RunOnThisChannel (MrcData, Channel, Rank)) {
      MspDebugPrint ((MSP_DBG_MAX, "    "));
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Element, PerBitSweep, &MaximumBits)) {
            continue;
          }
          for (Bits = 0; Bits < MaximumBits; Bits++) {
            MspDebugPrint ((MSP_DBG_MAX, "    %s%02dK%dB%d  ", HeaderLabel, Element, Knob, Bits));
          }
        }
      }
      MspDebugPrint ((MSP_DBG_MAX, "\n"));
      break;
    }
  }

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    //
    // Set all the initial values.
    //
    for (Knob = 0; Knob < Dim1Knobs; Knob++) {
      for (Element = 0; Element < NumberElements[Channel]; Element++) {
        if (!IsStrobeValid (MrcData, Channel, Rank, Element, PerBitSweep, &MaximumBits)) {
          continue;
        }
        LowHighFlag[Channel][Knob][Element] = LOW;
        for (Bits = 0; Bits < MaximumBits; Bits++) {
          Results[Channel][Knob][Element][Bits][LOW]  = Dim1Start[Channel][Knob][Element][LOW];
          Results[Channel][Knob][Element][Bits][HIGH] = 0;
          if ((Stage & RUN_ALL_MASK) == RUN_PER_BIT_AFTER_STROBE) {
            Results[Channel][Knob][Element][Bits][HIGH] = Dim1Start[Channel][Knob][Element][HIGH];
          }
        }
        //
        // If the user wants to skip the initial value programming, they MUST assign Dim1Start[][][][]
        // to the current register settings upon entry to this routine.
        //
        if (!SkipInitialValueProgramming) {
          Value[Channel][Knob][Element] = Dim1Start[Channel][Knob][Element][LOW];
          GetSetDdrioGroup2(MrcData, Channel, Rank, Element, Dim1Index, CMD_SET_VAL_FC_UC, &Value[Channel][Knob][Element]);
        }
      }
    }
  } // Channel loop ...
  //
  // Initialize Pass/Fail flags to fail before running the sweep for the low or high edge.
  //
  MmrcMemset(PassFail, RESULTS_INIT, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 3 * sizeof (UINT8));
  MmrcMemset(ElementDone, 0, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * sizeof (UINT8));
  TestFinished = FALSE;
  if ((Stage & RUN_LOCK_STEP_MODE) != 0) {
    //
    // We need to force that the first sample is the same across all strobes when in LOCK_STEP_MODE.
    // i.e.
    // Let's assume the first sample across strobes is 0001000, during the LOW phase strobe 3 will count down
    // while the remaining strobes will count up, which will bring issues as we want to run all strobes together
    // Adding the flag FAIL_SINGLE_STROBE will change 0001000 -> 00000000 and then all strobes will continue up
    // After the first ExecuteTest flag FAIL_SINGLE_STROBE might be removed (depending on the original value)
    // and the tracking will be done independently per strobe until all of them pass or one finds the upper limit
    //
    MrcData->CpgcOptions.RmtRestoreJedecResetType |= FAIL_SINGLE_STROBE;
  }
  //
  // First find the low edge then find the high edge.
  //
  while (!TestFinished) {
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      //
      // Set the 1st dimension to the sweep value.
      //
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Element, FALSE, NULL)) {
            continue;
          }
          if (!ElementDone[Channel][Knob][Element] &&
              DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], CURRENT_SAMPLE) != RESULTS_INIT) {
            GetSetDdrioGroup2 (MrcData, Channel, Rank, Element, Dim1Index, CMD_SET_OFFSET_UC, &Value[Channel][Knob][Element]);
          }
        }
      }
    } // Channel loop ...
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    //
    // Run the test and collect results in PassFail.
    //
    ExecuteTest (MrcData, Channel, Rank, NumberElements, Dim1Knobs, PassFail);
    MrcData->CpgcOptions.RmtRestoreJedecResetType = PostTestFlags;
    //
    // Look at all tested elements.
    //
    TestFinished = TRUE;
    LimitFound   = FALSE;

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_DEBUG, "CH%02d", Channel));
      //
      // Print out the result from the test.
      //
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Element, PerBitSweep, &MaximumBits)) {
            continue;
          }
          SpreadBitResults (MrcData, MAX_BITS, PerBitSweep, LowHighFlag[Channel][Knob][Element], PassFail[Channel][Knob][Element], Results[Channel][Knob][Element], ResultsB0Backup[Channel][Knob][Element]);

          for (Bits = 0; Bits < MaximumBits; Bits++) {
            MspDebugPrint ((MSP_DBG_DEBUG, "    %04d:%s   ",
              Results[Channel][Knob][Element][Bits][LowHighFlag[Channel][Knob][Element]],
              ResultsString[PassFail[Channel][Knob][Element][Bits][CURRENT_SAMPLE]]
            ));
          }
        }
      }
      MspDebugPrint((MSP_DBG_DEBUG, "\n"));

      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Element, PerBitSweep, &MaximumBits)) {
            continue;
          }
          EdgeFound[Channel][Element] = TRUE;
          //
          // If we are on the first pass OR the current and previous sample match, it means we haven't found an edge.
          //
          if (!FoundEdge(MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], DitherVal) && !ElementDone[Channel][Knob][Element]) {
            //
            // If we made it here it means at least 1 element hasn't found an edge.
            //
            TestFinished = FALSE;
            EdgeFound[Channel][Element] = FALSE;
            //
            // Now either increase or decrease the results array in preparation for the next pass.
            // If we are on the LOW side and we detect a failure, increase.
            // If we are on the LOW side and we detect a pass, decrease.
            // If we are on the HIGH side and we detect a failure, decrease.
            // If we are on the HIGH side and we detect a pass, increase.
            //
            if ((LowHighFlag[Channel][Knob][Element] == LOW && DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], FIRST_SAMPLE) == RESULTS_PASS) ||
              (LowHighFlag[Channel][Knob][Element] == HIGH && DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], FIRST_SAMPLE) == RESULTS_FAIL)) {

              if ((Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] - Dim1Step < Dim1Min[Channel][Element]) ||
                (Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] - Dim1Step > Dim1Max[Channel][Element])) {
                //
                // We've reached a limit. Mark it as an edge.
                //
                EdgeFound[Channel][Element] = TRUE;
                LimitFound = TRUE;
              } else {
                Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] -= Dim1Step;
                Value[Channel][Knob][Element] = 0 - Dim1Step;
              }
            } else {
              if ((Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] + Dim1Step > Dim1Max[Channel][Element]) ||
                (Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] + Dim1Step < Dim1Min[Channel][Element])) {
                //
                // We've reached a limit. Mark it as an edge.
                //
                EdgeFound[Channel][Element] = TRUE;
                LimitFound = TRUE;
              } else {
                Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] += Dim1Step;
                Value[Channel][Knob][Element] = Dim1Step;
              }
            }
          }
        } // Element loop ...
      } // Knob loop ...
    } // Channel loop ...

    if ((Stage & RUN_LOCK_STEP_MODE) != 0) {
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        for (Knob = 0; Knob < Dim1Knobs; Knob++) {
          for (Element = 0; Element < NumberElements[Channel]; Element++) {
            if (!IsStrobeValid (MrcData, Channel, Rank, Element, FALSE, NULL)) {
              continue;
            }
            if (!EdgeFound[Channel][Element]) {
              for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
                if (!RunOnThisChannel (MrcData, IndCh, Rank)) {
                  continue;
                }
                for (IndSt = 0; IndSt < NumberElements[IndCh]; IndSt++) {
                  if (!IsStrobeValid (MrcData, IndCh, Rank, IndSt, FALSE, NULL)) {
                    continue;
                  }
                  if (LimitFound) {
                    EdgeFound[IndCh][IndSt] = TRUE;
                  } else {
                    EdgeFound[IndCh][IndSt] = FALSE;
                  }
                }
                //
                // Will force Element and channels loops to break
                //
                Element = MAX_STROBES;
                Channel = MAX_CHANNELS;
              } // IndCh loop ...
            } // !EdgeFound[Channel][Element]
          } // Element loop ...
        } // Knob loop ...
      } // Channel loop ...
    } // Stage & RUN_LOCK_STEP_MODE
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Element, PerBitSweep, &MaximumBits)) {
            continue;
          }
          if (EdgeFound[Channel][Element]) {
            //
            // We have found an edge or reached a min/max limit of the sweep.
            //
            //
            // Move back to the last passing element if this last sample was a failure.
            //
            if (!ElementDone[Channel][Knob][Element] && DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], CURRENT_SAMPLE) == RESULTS_FAIL) {
              if (LowHighFlag[Channel][Knob][Element] == LOW) {
                //
                // Make sure we don't exceed the max.
                //
                if (Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] + (Dim1Step * DitherVal) <= Dim1Max[Channel][Element]) {
                  Results[Channel][Knob][Element][0][LOW] += (Dim1Step * DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], EDGE_SAMPLES));
                }
              } else {
                //
                // Make sure we don't exceed the min.
                //
                if (Results[Channel][Knob][Element][0][LowHighFlag[Channel][Knob][Element]] - (Dim1Step * DitherVal) >= Dim1Min[Channel][Element]) {
                  Results[Channel][Knob][Element][0][HIGH] -= (Dim1Step * DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], EDGE_SAMPLES));
                }
              }
            } else {
              //
              // The last sample was as PASS so we need to back up to the first PASS value, taking into account
              // dithering. If we had a DitherVal of 3 and sampled F F P P P, we need to subtract 2 to get back
              // to the first P sample. However if we have 0 EDGE_SAMPLES it could mean a situation where our first
              // sample was at the limit. For example, we sample a PASS at the lower limit delay of 0. In this case
              // we don't want to do any sort of backing up.
              //
              if (!ElementDone[Channel][Knob][Element] && DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], EDGE_SAMPLES) != 0) {
                if (LowHighFlag[Channel][Knob][Element] == LOW) {
                  Results[Channel][Knob][Element][0][LOW] -= (Dim1Step * (DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], EDGE_SAMPLES) - 1));
                }
                else {
                  Results[Channel][Knob][Element][0][HIGH] += (Dim1Step * (DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail[Channel][Knob][Element], EDGE_SAMPLES) - 1));
                }
              }
            }
            //
            // If the high edge has been found, this element is done.
            //
            if (LowHighFlag[Channel][Knob][Element] == HIGH) {
              ElementDone[Channel][Knob][Element] = TRUE;
            } else {
              //
              // Switch directions to find the high edge.
              //
              LowHighFlag[Channel][Knob][Element] = HIGH;
              Results[Channel][Knob][Element][0][HIGH] = Dim1Start[Channel][Knob][Element][HIGH];
              //
              // Program HIGH side initial values. If user opts to skip this step, the HIGH side sweep
              // will start where the LOW side ended.
              //
              if (!SkipInitialValueProgramming) {
                Value[Channel][Knob][Element] = Dim1Start[Channel][Knob][Element][HIGH];
                GetSetDdrioGroup2(MrcData, Channel, Rank, Element, Dim1Index, CMD_SET_VAL_FC_UC, &Value[Channel][Knob][Element]);
              }
              TestFinished = FALSE;
              for (Bits = 0; Bits < MaximumBits; Bits++) {
                MmrcMemset(&PassFail[Channel][Knob][Element][Bits][0], RESULTS_INIT, 3);
              }
              if ((Stage & RUN_LOCK_STEP_MODE) != 0) {
                MrcData->CpgcOptions.RmtRestoreJedecResetType |= FAIL_SINGLE_STROBE;
              }
            }
          } // Edge Statement ...
        } // Element loop ...
      } // Knob loop ...
    } // Channel loop ...
  } // TestFinished loop ...
  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;

  //
  // Restoring SX[b0] after PerBitSweep run.
  //
  if (PerBitSweep && ((Stage & RUN_PER_BIT) == RUN_PER_BIT)) {
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel(MrcData, Channel, Rank)) {
        continue;
      }
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Element = 0; Element < NumberElements[Channel]; Element++) {
          if (!IsStrobeValid(MrcData, Channel, Rank, Element, PerBitSweep, &MaximumBits)) {
            continue;
          }
          Results[Channel][Knob][Element][0][LOW]  = ResultsB0Backup[Channel][Knob][Element][LOW];
          Results[Channel][Knob][Element][0][HIGH] = ResultsB0Backup[Channel][Knob][Element][HIGH];
        } // Element loop
      } // Knob loop
    } // Channel loop
  } // if (PerBitSweep)

  Channel = ChannelBackup;
  PrintFooter1DSweep (MrcData, Channel, Rank, Dim1Index, Dim1Knobs, Results, PerBitSweep);

  if (PerBitSweepBackup && ((Stage & RUN_PER_STROBE) == RUN_PER_STROBE) && ((Stage & RUN_PER_BIT) == RUN_PER_BIT)) {
    PerBitSweep = PerBitSweepBackup;

    Status = CalculateOrRestoreResults (MrcData, ChannelBackup, Rank, Dim1Knobs, NumberElements, Results, Dim1Start, 0, Stage);
    if (Status != MMRC_SUCCESS) {
      return 0;
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    //
    // Execute the second phase (Per Bit)
    //
    Create1DSweep (
      MrcData, Channel, Rank, Dim1Index, Dim1Knobs, Dim1Start, Dim1Min, Dim1Max, Dim1Step, FullSweep, SkipInitialValueProgramming,
      PerBitSweep, NumberElements, ExecuteTest, Results, HeaderLabel, DitherVal, RUN_PER_BIT_AFTER_STROBE | LockStepRun
    );
    //
    // Do not result on BIT0 the results per Strobe if the RUN_RESULT_BIT0 flag is on
    //
    if ((Stage & RUN_RESULT_BIT0_INDEPENDENT) == 0) {
      CalculateOrRestoreResults (MrcData, ChannelBackup, Rank, Dim1Knobs, NumberElements, Results, Dim1Start, 1, Stage);
    }
    MmrcMemCopy (Dim1Start, Dim1StartRestore, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * 2 * sizeof (UINT16));
  }
  return MMRC_SUCCESS;
}

/**
  Compares the initial sample with the current sample. If they don't match, it increases a counter by 1. When this
  counter equals DitherVal it means we've found an edge. Higher dither values means we look for a stronger edge. 
  For example if DitherVal is 3 and the first sample if FAIL, it means this routine will return TRUE after sampling
  3 PASS values in a row.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       MaximumBits     Max bits per strobe
  @param[in]       PerBitSweep     Indication if we are running per bit (PerBitSweep = TRUE) or per strobe (PerBitSweep = FALSE)
  @param[in]       PassFail        Array of samples performed by the sweep function.
  @param[in]       DitherVal       The number of successive samples that need to be either PASS or FAIL before this routine
                                   considers an edge to be found. For example, if the first sample is FAIL, the sweep will
                                   look to read X number of PASS values in a row before it considers this a FAIL to PASS
                                   edge. X = DitherVal.

  @retval          TRUE/FALSE       Is the edge found?
**/
BOOLEAN
FoundEdge (
  IN        MMRC_DATA    *MrcData,
  IN        UINT8        MaximumBits,
  IN        BOOLEAN      PerBitSweep,
  IN        UINT8        (*PassFail)[3],
  IN        UINT8        DitherVal
)
{
  UINT8     Bits;
  BOOLEAN   FoundAllEdges;
  //
  // If this is the first time in this routine, set the edge count to 0. This
  // count will increase when successive samples equal the same value. For example,
  // if we are sweeping and get FAIL, FAIL, PASS, PassFail[EDGE_SAMPLES] would be 1
  // because we've detected 1 sample in a row that's different from the first sample.
  // If we sample FAIL, FAIL, PASS, PASS, PASS, PassFail[EDGE_SAMPLES] = 3.
  //
  if (DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail, EDGE_SAMPLES) == RESULTS_INIT) {
    for (Bits = 0; Bits < MaximumBits; Bits++) {
      PassFail[Bits][EDGE_SAMPLES] = 0;
      //
      // Save the first sample for comparison later.
      //
      PassFail[Bits][FIRST_SAMPLE] = PassFail[Bits][CURRENT_SAMPLE];
    }
    return FALSE;
  }
  //
  // If the current sample equals the first sample of this sweep, then we haven't
  // found an edge and we need to re-init the edge count to 0.
  //
  FoundAllEdges = TRUE;
  for (Bits = 0; Bits < MaximumBits; Bits++) {
    if (PassFail[Bits][FIRST_SAMPLE] == PassFail[Bits][CURRENT_SAMPLE]) {
      PassFail[Bits][EDGE_SAMPLES] = 0;
      FoundAllEdges = FALSE;
    }
  }
  if(!FoundAllEdges) {
    return FALSE;
  }
  //
  // If the current sample is different from the first sample, we need to increase the
  // edge count. If this count equals the dither value, it means we've read X successive
  // samples in a row which a DIFFERENT from the first sample, where X = DitherVal. This
  // means we've found an edge.
  //
  FoundAllEdges = FALSE;
  for (Bits = 0; Bits < MaximumBits; Bits++) {
    if (PassFail[Bits][FIRST_SAMPLE] != PassFail[Bits][CURRENT_SAMPLE]) {
      if (DitherVal > PassFail[Bits][EDGE_SAMPLES]) {
        PassFail[Bits][EDGE_SAMPLES]++;
      }
      if (DeterminePassFailPerStrobe (MrcData, MaximumBits, PerBitSweep, PassFail, EDGE_SAMPLES) >= DitherVal) {
        FoundAllEdges = TRUE;
      }
    }
  }
  return FoundAllEdges;
}

#endif // TRAINING_ALGOS == 1

/*
This is the last pass 1D sweep which finds the low edge for all strobes first before switching to
the high edge. This is less complicated than the 1D search routine above. Leaving this here for now
in case we need to bring it back for ANN PO.
MMRC_STATUS
  Create1DSweep (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT8        Dim1Index, 
  IN         UINT16       Dim1Start[MAX_STROBES][2], 
  IN         UINT16       Dim1Min[MAX_STROBES],
  IN         UINT16       Dim1Max[MAX_STROBES],
  IN         UINT16       Dim1Step,
  IN         BOOLEAN      FullSweep,
  IN         UINT8        NumberElements,
  IN         MMRC_STATUS   ExecuteTest (MMRC_DATA *, UINT8, UINT8, UINT8, UINT8 PassFail[MAX_STROBES][2]), 
  IN   OUT   UINT16       Results[MAX_STROBES][2],
  IN         UINT8        *HeaderLabel
  )
{
  UINT8     LowHighFlag;
  UINT8     Element;
  UINT8     TestFinished;
  UINT8     PassFail[MAX_STROBES][2];
  BOOLEAN   ElementDone[MAX_STROBES];
  UINT32    Value[MAX_STROBES];

  if (HeaderLabel != "") {
    for (Element = 0; Element < NumberElements; Element++) {
      MspDebugPrint ((MSP_DBG_MAX, "    %s%02d   ", HeaderLabel, Element));
    }
    MspDebugPrint ((MSP_DBG_MAX, "\n"));
  }
  //
  // First find the low edge then find the high edge.
  //
  for (LowHighFlag = LOW; LowHighFlag <= HIGH; LowHighFlag++) {
    //
    // Set all the initial values.
    //
    for (Element = 0; Element < NumberElements; Element++) {
      Results[Element][LowHighFlag] = Dim1Start[Element][LowHighFlag];
      Value[Element] = Results[Element][LowHighFlag];
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Element, Dim1Index, CMD_SET_VAL_FC, &Value[Element]);
    }
    //
    // Initialize Pass/Fail flags to fail before running the sweep for the low or high edge.
    //
    MmrcMemset (PassFail, RESULTS_INIT, MAX_STROBES * 2 * sizeof (UINT8));
    MmrcMemset (ElementDone, 0, MAX_STROBES * sizeof (UINT8));
    TestFinished = FALSE;

    while (!TestFinished) {
      //
      // Set the 1st dimension to the sweep value.
      //
      for (Element = 0; Element < NumberElements; Element++) {
        if (!ElementDone[Element] && PassFail[Element][CURRENT_SAMPLE] != RESULTS_INIT) {
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Element, Dim1Index, CMD_SET_OFFSET_UC, &Value[Element]);
          PassFail[Element][PREVIOUS_SAMPLE] = PassFail[Element][CURRENT_SAMPLE];
        }
      }

      ExecuteTest (MrcData, Channel, Rank, NumberElements, PassFail);
      //
      // Print out the result from this one sweep value.
      //
      for (Element = 0; Element < NumberElements; Element++) {
        MspDebugPrint ((MSP_DBG_DEBUG, "   %03d:%c  ", Results[Element][LowHighFlag], ResultsString[PassFail[Element][CURRENT_SAMPLE]]));
      }
      MspDebugPrint((MSP_DBG_DEBUG, "\n"));
      //
      // Look at all tested elements.
      //
      TestFinished = TRUE;
      for (Element = 0; Element < NumberElements; Element++) {
        //
        // If we are on the first pass OR the current and previous sample match, it means we haven't found an edge.
        //
        if (PassFail[Element][PREVIOUS_SAMPLE] == RESULTS_INIT ||
          (PassFail[Element][CURRENT_SAMPLE] == PassFail[Element][PREVIOUS_SAMPLE] && 
          !ElementDone[Element])) {
            //
            // If we made it here it means at least 1 element hasn't found an edge.
            //
            TestFinished = FALSE;
            //
            // Now either increase or decrease the results array in preparation for the next pass.
            // If we are on the LOW side and we detect a failure, increase.
            // If we are on the LOW side and we detect a pass, decrease.
            // If we are on the HIGH side and we detect a failure, decrease.
            // If we are on the HIGH side and we detect a pass, increase.
            //
            if ((LowHighFlag == LOW && PassFail[Element][CURRENT_SAMPLE] == RESULTS_PASS) ||
              (LowHighFlag == HIGH && PassFail[Element][CURRENT_SAMPLE] == RESULTS_FAIL)) {
                if (Results[Element][LowHighFlag] - Dim1Step < Dim1Min[Element]) {
                  TestFinished = TRUE;
                  break;
                } else {
                  Results[Element][LowHighFlag] -= Dim1Step;
                  Value[Element] = 0 - Dim1Step;
                }
            } else {
              if (Results[Element][LowHighFlag] + Dim1Step > Dim1Max[Element]) {
                TestFinished = TRUE;
                break;
              } else {
                Results[Element][LowHighFlag] += Dim1Step;
                Value[Element] = Dim1Step;
              }
            }
        } else {
          //
          // Move back to the last passing element if this last sample was a failure.
          //
          if (!ElementDone[Element] && PassFail[Element][CURRENT_SAMPLE] == RESULTS_FAIL) {
            if (LowHighFlag == LOW) {
              Results[Element][LOW] += Dim1Step;
            } else {
              Results[Element][HIGH] -= Dim1Step;
            }
          }
          ElementDone[Element] = TRUE;
        }
      }
    }
  }
  PrintFooter1DSweep (MrcData, Dim1Index, Results);

  return MMRC_SUCCESS;
}
*/

#if TRAINING_ALGOS == 1
MMRC_STATUS
SweepCenterEye2D (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Dim1Index,
  IN   INT16        Dim1Start,
  IN   INT16        Dim1End,
  IN   UINT16       Dim1Step,
  IN   UINT8        Dim1Flag,
  IN   UINT16       Dim2Index,
  IN   INT16        Dim2Start,
  IN   INT16        Dim2End,
  IN   UINT16       Dim2Step,
  IN   UINT8        Dim2Flag
)
{
  UINT8   ConsolidateFlag;
  UINT8   Dim1Knobs;

  SAMPLE_SWEEP  Results[MAX_ELEMENTS_DIM_Y];
  UINT16  Dim1Min[MAX_CHANNELS][MAX_STROBES];
  UINT16  Dim1Max[MAX_CHANNELS][MAX_STROBES];
  UINT16  Dim2Min[MAX_CHANNELS][MAX_STROBES];
  UINT16  Dim2Max[MAX_CHANNELS][MAX_STROBES];

  MmrcMemset (Results, 0xff, MAX_ELEMENTS_DIM_Y * sizeof (SAMPLE_SWEEP));
  if (Dim2Flag & CONSOLIDATE_STROBES) {
    ConsolidateFlag = 1;
  } else {
    ConsolidateFlag = 0;
  }

    Dim1Knobs = 1;

  GetMinMaxRelativeAbsolute (MrcData, Channel, Rank, Dim1Index, Dim1Start, Dim1End, Dim1Flag, Dim1Min, Dim1Max);
  GetMinMaxRelativeAbsolute (MrcData, Channel, Rank, Dim2Index, Dim2Start, Dim2End, Dim2Flag, Dim2Min, Dim2Max);

#if READWRITE_TRAINING_SIM == 0
  Create2DSweep (MrcData, Results, Channel, Rank, Dim1Index, Dim1Knobs, Dim1Start, Dim1End, Dim1Step, Dim1Flag, Dim2Index, Dim2Start, Dim2End, Dim2Step, Dim2Flag);
#else
  Create2DSweepSim (MrcData, Results, Channel, Rank, Dim1Index, Dim1Knobs, Dim1Start, Dim1End, Dim1Step, Dim1Flag, Dim2Index, Dim2Start, Dim2End, Dim2Step, Dim2Flag);
#endif
  Center2D (MrcData, Results, Channel, Rank, Dim1Index, Dim1Knobs, Dim1Min, Dim2Index, Dim2Min, Dim2Step, ConsolidateFlag);
  Create2DEye (MrcData, Results, Channel, Rank, Dim1Index, Dim1Knobs, Dim1Min, Dim1Step, Dim2Index, Dim2Min, Dim2Step);
  
  return MMRC_SUCCESS;
}
#endif // TRAINING_ALGOS == 1

#if TRAINING_ALGOS == 1
MMRC_STATUS
Create2DSweep (
  IN          MMRC_DATA       *MrcData,
  IN   OUT    SAMPLE_SWEEP    Results[MAX_ELEMENTS_DIM_Y],
  IN          UINT8           Channel,
  IN          UINT8           Rank,
  IN          UINT16          Dim1Index,
  IN          UINT8           Dim1Knobs,
  IN          INT16           Dim1Start,
  IN          INT16           Dim1End,
  IN          UINT16          Dim1Step,
  IN          UINT8           Dim1Flag,
  IN          UINT16          Dim2Index,
  IN          INT16           Dim2Start,
  IN          INT16           Dim2End,
  IN          UINT16          Dim2Step,
  IN          UINT8           Dim2Flag
)
{
  UINT16    Dim2Min[MAX_CHANNELS][MAX_STROBES];
  UINT16    Dim2Max[MAX_CHANNELS][MAX_STROBES];
  UINT32    Dim2PhyValue;
  UINT16    Dim1Min[MAX_CHANNELS][MAX_STROBES];
  UINT16    Dim1Max[MAX_CHANNELS][MAX_STROBES];
  UINT16    Dim1StartPoint[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT8     Strobe;
  UINT8     Knob;
  UINT8     ChannelBackup;
  UINT8     FirstValidChannel;
  UINT8     NumberElements[MAX_CHANNELS] = {0};
  UINT16    DimYOffset[MAX_CHANNELS] = {0};
  UINT8     LowHighFlag;
  BOOLEAN   AtLeastOneChannelUnderTest;
  BOOLEAN   TestDone[MAX_CHANNELS];
  BOOLEAN   AllChannelsTestDone;
  BOOLEAN   HighSideDone;

  Dim2PhyValue      = 0;
  ChannelBackup     = Channel;
  FirstValidChannel = 0xFF;
  HighSideDone      = FALSE;
  //
  // Get Min/Max points according with the Start/End values
  // and also the ABSOLUTE/RELATIVE flag
  //
  GetMinMaxRelativeAbsolute (MrcData, Channel, Rank, Dim2Index, Dim2Start, Dim2End, Dim2Flag, Dim2Min, Dim2Max);
  GetMinMaxRelativeAbsolute (MrcData, Channel, Rank, Dim1Index, Dim1Start, Dim1End, Dim1Flag, Dim1Min, Dim1Max);

  AtLeastOneChannelUnderTest = FALSE;

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    if (FirstValidChannel == 0xFF) {
      FirstValidChannel = Channel;
    }
    //
    // Initialize Start point for Dim2 (outer loop)
    // START_SWEEP_CENTER only works when the MAX & MIN values has pretty much the same Delta across channels
    //
    if ((Dim2Flag & START_SWEEP_CENTER) != 0) {
      DimYOffset[Channel]  = (Dim2Max[Channel][0] - Dim2Min[Channel][0]) / 2;
      DimYOffset[Channel] -= DimYOffset[Channel] % Dim2Step;
    } else {
      DimYOffset[Channel] = 0;
    }
  }

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }

    AtLeastOneChannelUnderTest = TRUE;

    //
    // Initialize Start point for Dim1 (inner loop)
    //
    for (Knob = 0; Knob < Dim1Knobs; Knob++) {
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        Dim1StartPoint[Channel][Knob][Strobe][LOW]  = Dim1Min[Channel][Strobe];
        Dim1StartPoint[Channel][Knob][Strobe][HIGH] = Dim1Max[Channel][Strobe];
      } // Strobe loop ...
    }
  } // Channel loop ...

  if (!AtLeastOneChannelUnderTest) {
    return MMRC_SUCCESS;
  }
   
  //
  // Search for Low Edge or Min boundary then
  // start to search High Edge or Max boundary
  //
  for (LowHighFlag = LOW; LowHighFlag <= HIGH; LowHighFlag++) {
   
    do {
      //
      // There are 2 ways to consider the test on a channel is done (TestDone[Channel]):
      //    1. (SWEEP_ALL_STROBES_TOGETHER == 0) When all the strobes are failing
      //    2. (SWEEP_ALL_STROBES_TOGETHER == 1) When a single strobe fails
      //    3. (FOUND_PASS_CONDITION == 1 && SWEEP_ALL_STROBES_TOGETHER == 0) do a full sweep always
      // There are 2 ways to consider the test across channels is done (AllChannelsTestDone):
      //    1. (SWEEP_ALL_STROBES_TOGETHER == 0) When both channels are complete
      //    2. (SWEEP_ALL_STROBES_TOGETHER == 1) When a single completes
      //
      MmrcMemset (TestDone, 0, MAX_CHANNELS * sizeof (BOOLEAN));
      if ((Dim2Flag & SWEEP_ALL_STROBES_TOGETHER) != 0) {
        AllChannelsTestDone = FALSE;
      } else {
        AllChannelsTestDone = TRUE;
      }
      //
      // Set the value for all strobes
      //
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        NumberElements[Channel] = MrcData->MaxDq[Channel][Rank];
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MED, "CH%02d %s = ", Channel, 
          SignalInfo[GetSignalInfoIndex (GetOffsetIndex (MrcData, Dim2Index))].Name
          ));
        for (Strobe = 0; Strobe < MrcData->MaxDq[Channel][Rank]; Strobe++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
            continue;
          }
          Dim2PhyValue = Dim2Min[Channel][Strobe] + DimYOffset[Channel];
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Dim2Index, CMD_SET_VAL_FC_UC, &Dim2PhyValue);
        } // Strobe loop ...

        MspDebugPrint ((MSP_DBG_MED, "%03d\n", Dim2PhyValue));

      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;

      if(FirstValidChannel >= MAX_CHANNELS){
        MrcData->ErrorCode = MrcErrParameterOutOfBound;
        MrcDeadLoop();
        return MMRC_FAILURE;
      }
      //
      // Create the 1D Sweep
      //
      Create1DSweep (MrcData, Channel, Rank, Dim1Index, Dim1Knobs, Dim1StartPoint, Dim1Min, Dim1Max, Dim1Step, FALSE, FALSE, FALSE, NumberElements, CpgcExecuteRmtTest, Results[DimYOffset[FirstValidChannel]].Sample, "S", 1, RUN_PER_STROBE_AND_BIT);
      //
      // Check the results across channels
      //
      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        for (Knob = 0; Knob < Dim1Knobs; Knob++) {
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            //
            // If this sample passed, create new starting points for the next time the 1D sweep is called.
            // This will allow the 1D sweep to start at the last known passing points instead of doing
            // a full sweep to save time.
            //
            if (Results[DimYOffset[FirstValidChannel]].Sample[Channel][Knob][Strobe][0][HIGH] > Results[DimYOffset[FirstValidChannel]].Sample[Channel][Knob][Strobe][0][LOW]) {
              Dim1StartPoint[Channel][Knob][Strobe][LOW]  = Results[DimYOffset[FirstValidChannel]].Sample[Channel][Knob][Strobe][0][LOW];
              Dim1StartPoint[Channel][Knob][Strobe][HIGH] = Results[DimYOffset[FirstValidChannel]].Sample[Channel][Knob][Strobe][0][HIGH];
              if ((Dim2Flag & SWEEP_ALL_STROBES_TOGETHER) != 0) {
                //
                // Do nothing
                //
              } else {
                //
                // As soon any Strobe passes we have to mark the test immediately as NotDone
                //
                TestDone[Channel] = FALSE;
                // 
                // In order to avoid it gets override if the failure 
                // is detected in the last strobe(s) 
                //
                MspDebugPrint ((MSP_DBG_TST, "TestDone = FALSE "));
                break;
              }
            } else {
              if ((Dim2Flag & SWEEP_ALL_STROBES_TOGETHER) != 0) {
                TestDone[Channel] = TRUE;
                break;
              } else {
                //
                // Start tracking from the Strobe[First] and if fails mark the Test as Done
                //
                if ((Dim2Flag & FOUND_PASS_CONDITION) == 0) {
                  TestDone[Channel] = TRUE;
                }
                MspDebugPrint ((MSP_DBG_TST, "TestDone = TRUE "));
              }
            }
          } // Strobe loop ...
        } // Knob loop ...
      } // Channel loop ...
      //
      // Restore back the Channel Under Test
      //
      Channel = ChannelBackup;

      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }

        if (LowHighFlag == LOW) {
          if (((DimYOffset[Channel] - Dim2Step) < 0) || TestDone[Channel]) {
            //
            // Done with the Low Side
            //
            TestDone[Channel] = TRUE;
            if ((Dim2Flag & START_SWEEP_CENTER) != 0) {
              DimYOffset[Channel]  = (Dim2Max[Channel][0] - Dim2Min[Channel][0]) / 2;
              DimYOffset[Channel] -= DimYOffset[Channel] % Dim2Step;
              MspDebugPrint ((MSP_DBG_TST, "DimYOffset[%d] = %d\n", Channel, DimYOffset[Channel]));
            } else {
              DimYOffset[Channel] = 0;
            }
            DimYOffset[Channel] += Dim2Step;
          } else {
            if ((Dim2Flag & FOUND_PASS_CONDITION) == 0) {
              DimYOffset[Channel] -= Dim2Step;
            } else {
              if ((DimYOffset[Channel] + Dim2Step) > (Dim2Max[Channel][0] - Dim2Min[Channel][0])) {
                //
                // Done with the High Side
                //
                TestDone[Channel] = TRUE;
              } else {
                DimYOffset[Channel] += Dim2Step;
                TestDone[Channel] = FALSE;
              }
            } // (Dim2Flag & FOUND_PASS_CONDITION) == 0
            MspDebugPrint ((MSP_DBG_TST, "_DimYOffset[%d] = %d\n", Channel, DimYOffset[Channel]));
          }
        } else {
          if ((DimYOffset[Channel] + Dim2Step) > (Dim2Max[Channel][0] - Dim2Min[Channel][0])) {
            //
            // Done with the High Side
            //
            TestDone[Channel] = TRUE;
          } else {
            DimYOffset[Channel] += Dim2Step;
          }
        }
        
        if ((Dim2Flag & SWEEP_ALL_STROBES_TOGETHER) != 0) {
          if (TestDone[Channel]) {
            AllChannelsTestDone = TRUE;
          }
        } else if (!TestDone[Channel]) {
          AllChannelsTestDone = FALSE;
        }
      } // Channel loop ...
      
      //
      // If the Strobe are sweep all together we need to re-set the initial value for the High Side for all channels
      //
      for (Channel = ChannelBackup; 
          Channel < MAX_CHANNELS && 
          AllChannelsTestDone && 
          (Dim2Flag & SWEEP_ALL_STROBES_TOGETHER) != 0 && 
          LowHighFlag == LOW; 
          Channel++
          ) {
        if ((Dim2Flag & START_SWEEP_CENTER) != 0) {
          DimYOffset[Channel]  = (Dim2Max[Channel][0] - Dim2Min[Channel][0]) / 2;
          DimYOffset[Channel] -= DimYOffset[Channel] % Dim2Step;
          MspDebugPrint ((MSP_DBG_TST, "**DimYOffset[%d] = %d\n", Channel, DimYOffset[Channel]));
        } else {
          DimYOffset[Channel] = 0;
        }
        DimYOffset[Channel] += Dim2Step;
        MspDebugPrint ((MSP_DBG_TST, "**DimYOffset[%d] = %d\n", Channel, DimYOffset[Channel]));
      } // Channel loop ...

      MspDebugPrint ((MSP_DBG_TST, "AllChannelsTestDone = %d\n", AllChannelsTestDone));
    } while (!AllChannelsTestDone);
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      if (DimYOffset[Channel] > (Dim2Max[Channel][0] - Dim2Min[Channel][0])) {
        //
        // Done automatically with with the High Side
        //
        HighSideDone = TRUE;
      }
    } // Channel loop ...

    if (HighSideDone) {
      break;
    }
  } // LowHighFlag loop ...

  return MMRC_SUCCESS;
}

#endif //#if TRAINING_ALGOS == 1

/**
  Unified routine to do initial setup on all cpgc routines

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       SetupType       Chooses which type of CPGC setup to perform. 
                                   Typically one per algo.
  @retval          MMRC_SUCCESS    In case the setup is done properly
**/
MMRC_STATUS
CpgcSetup ( 
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        SetupType
  )
{
  BOOLEAN   ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

#if RMT_FAKE_ERRORS == 1
  MrcData->CpgcOptions.RmtFakeErrorCount = 0;
  return SUCCESS;
#endif

  CpgcOptionsSetup (MrcData, Channel);
  //
  // SetupType will be retained as state, so you don't have to pass into CpgcReconfig, and CpgcRunTest
  //
  MrcData->CpgcOptions.SetupType = SetupType;  
  
  MmrcExecuteTask (MrcData, CPGC_ENABLE, NO_PRINT, Channel);  
  MrcData->MspData.DynamicVars[Channel][LANE_ROTATE_RATE]       = 0;//MrcData->CpgcOptions.NumBursts - 3;
  //
  // Always LANE_ROTATE_RATE+1
  //
  MrcData->MspData.DynamicVars[Channel][RELOAD_LFSR_SEED_RATE]  = 0;//MrcData->MspData.DynamicVars[Channel][LANE_ROTATE_RATE] + 1;
#ifdef SAVE_LFSR_SEED_RATE
  //
  // 8 per rank tested, should never get to 8th victim 
  //
  MrcData->MspData.DynamicVars[Channel][SAVE_LFSR_SEED_RATE]    = 0;//8;
#endif
  MrcData->MspData.DynamicVars[Channel][NUM_BURSTS]             = MrcData->CpgcOptions.NumBursts;
  MrcData->MspData.DynamicVars[Channel][LOOP_COUNT]             = MrcData->CpgcOptions.LoopCount;

  switch (SetupType)  {
#if ECC_SCRUB == 1 || MEMORY_TEST == 1
  case  CPGC_CONFIG_MEMINIT:
  case  CPGC_CONFIG_MEMTEST:
    break;  // end case CPGC_CONFIG_MEMINIT
#endif
  case CPGC_CONFIG_VA:
    //
    // Already setup above (numbursts, loopcount, etc)
    //
    MmrcExecuteTask (MrcData, CPGC_SETUP_VA, NO_PRINT, Channel);
    break; 
  case CPGC_CONFIG_EARLYTRAINING:
  case CPGC_CONFIG_EARLY_MPR_TRAINING:
    //
    // Inputs: LoopCount, NumBursts
    //
    MrcData->MspData.DynamicVars[Channel][RELOAD_LFSR_SEED_RATE]  = 0;
#ifdef SAVE_LFSR_SEED_RATE 
    MrcData->MspData.DynamicVars[Channel][SAVE_LFSR_SEED_RATE]    = 0;
#endif
    MrcData->MspData.DynamicVars[Channel][LANE_ROTATE_RATE]       = 0;
    MmrcExecuteTask (MrcData, CPGC_SETUP_VA, NO_PRINT, Channel);
    break;
#if RMT_ENABLE_TURNAROUNDS_TEST == 1
  case CPGC_CONFIG_TURNAROUNDS:
    //
    // Already setup above (numbursts, loopcount, etc)
    //
    MmrcExecuteTask (MrcData, CPGC_SETUP_VA, NO_PRINT, Channel);
    break;
#endif
#if RMT_ENABLE_LFSR_ADDRESS_TEST == 1
  case CPGC_CONFIG_LFSR_ADDRESS:
    MrcData->MspData.DynamicVars[Channel][RELOAD_LFSR_SEED_RATE]  = 0;      // Disable all the lane rotations stuff, since we sweep numbursts anyways
    MrcData->MspData.DynamicVars[Channel][SAVE_LFSR_SEED_RATE]    = 0; 
    MrcData->MspData.DynamicVars[Channel][LANE_ROTATE_RATE]       = 0;
    MrcData->MspData.DynamicVars[Channel][NUM_BURSTS]             = 1;
    MmrcExecuteTask (MrcData, CPGC_SETUP_VA, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_LFSR_ADDRESS, NO_PRINT, Channel);
    break;
#endif
  case CPGC_CONFIG_CMD_STRESS:
    //
    // Inputs: LoopCount, NumBursts
    // Setup the basic VA mode.
    //
    //
    // VAR_SEG_HIGHADR_HI/LO sets the LFSR addressing for Subseq 2 & 3 to target high address bits, such as rows
    //
    MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_HI] = (MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1 + 3;
#if LPDDR3_SUPPORT
    if (MrcData->MrcParameters.DramType[Channel] == TypeLpDdr3) {
      if ((MrcData->CpgcOptions.MinRankBit - 5 - MrcData->CpgcOptions.BusWidth) < 
          ((MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1)
         )
      {
        MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_LO] = (MrcData->CpgcOptions.MinBankBit) - 5 - MrcData->CpgcOptions.BusWidth;   
      } else {
        //
        // Set low 3 bits of var segment to just be adjacent to hi part of var segment
        //
        MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_LO] = (MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1;
      }
    }
#endif
#if DDR3_SUPPORT
    if  (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      //
      // Make sure min bank bit is low enough , otherwise hi/lo segment will alias
      //
      if (((MrcData->CpgcOptions.MinBankBit + 1 - MrcData->CpgcOptions.BusWidth) - 5 - MrcData->CpgcOptions.BusWidth) <
          ((MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1)
         )
      {
        //
        // Set low 3 bits of var segment to point at banks
        //
        MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_LO] = (MrcData->CpgcOptions.MinBankBit + 1 - MrcData->CpgcOptions.BusWidth ) - 5 - MrcData->CpgcOptions.BusWidth; 
      } else {
        //
        // Set low 3 bits of var segment to just be adjacent to hi part of var segment
        //
        MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_LO] = (MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1;
      }
    }
#endif
#if DDR4_SUPPORT
    if  (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      //
      // Make sure min bank bit is low enough , otherwise hi/lo segment will alias
      //
      if (((MrcData->CpgcOptions.MinBankBit + 1 - MrcData->CpgcOptions.BusWidth) - 5 - MrcData->CpgcOptions.BusWidth) <
          ((MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1)
         )
      {
        //
        // Set low 3 bits of var segment to point at banks
        //
        MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_LO] = (MrcData->CpgcOptions.MinBankBit + 1 - MrcData->CpgcOptions.BusWidth ) - 5 - MrcData->CpgcOptions.BusWidth; 
      } else {
        //
        // Set low 3 bits of var segment to just be adjacent to hi part of var segment
        //
        MrcData->MspData.DynamicVars[Channel][VAR_SEG_HIGHADDR_LO] = (MrcData->CpgcOptions.MinRankBit - 1) - 5 - MrcData->CpgcOptions.BusWidth - 15 + 1;
      }
    }
#endif
    // 
    // Initially target rank=0, but this should be overridden by CpgcReconfig
    //
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT] = 0; 
    MmrcExecuteTask (MrcData, CPGC_SETUP_VA, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_CADB1, NO_PRINT, Channel);
    break; 
  default:  
    MrcDeadLoop ();
  } // main case statement (SetupType)

  if (SetupType != CPGC_CONFIG_MEMINIT && SetupType != CPGC_CONFIG_MEMTEST) {
    MmrcExecuteTask (MrcData, CPGC_LOOPS, NO_PRINT, Channel);
  }

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;

  return MMRC_SUCCESS;
}

/**
  Similar description as CpgcSetup.  This is called after CpgcSetup 
  for a particular SetupType, to reconfigure it to a new rank, 
  loopcount, or algo specific change during the algorithm. All 
  these kind of options are passed through MrcData->CpgcOptions 
 
  WARNING:  A call to CpgcSetup is required before using CpgcReconfig !!
            This is true for each algo (need to call CpgcSetup first
            inside each algo) 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CpgcReconfig (
  IN OUT  MMRC_DATA  *MrcData,
  IN      UINT8      Channel
  )
{
  BOOLEAN   ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

#if RMT_FAKE_ERRORS == 1
  MrcData->CpgcOptions.RmtFakeErrorCount =0;
  return SUCCESS;
#endif

  switch (MrcData->CpgcOptions.SetupType)  {
#if ECC_SCRUB == 1 || MEMORY_TEST == 1
  case CPGC_CONFIG_MEMINIT:
    MmrcExecuteTask (MrcData, CPGC_MEMINIT, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_MEMINIT_AND_TEST, NO_PRINT, Channel);
    break;
  case CPGC_CONFIG_MEMTEST:
    MmrcExecuteTask (MrcData, CPGC_MEMTEST, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_MEMINIT_AND_TEST, NO_PRINT, Channel);
    break; 
#endif
  case CPGC_CONFIG_VA:
    //
    // Inputs: Rank
    //
    MrcData->MspData.DynamicVars[Channel][SEQ_START]  = 0;
    MrcData->MspData.DynamicVars[Channel][SEQ_END]    = 1;
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0]  = MrcData->CpgcOptions.Rank << (GetAddressBit(MrcData, Channel, MrcData->CpgcOptions.Rank) - (5 + MrcData->CpgcOptions.BusWidth));    
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_1]  = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];    
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_2]  = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];    
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_3]  = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];    
    MmrcExecuteTask (MrcData, CPGC_FIXVAR, NO_PRINT, Channel);  
    MmrcExecuteTask (MrcData, CPGC_SETUP_LVL, NO_PRINT, Channel);
    break; 
#if RMT_ENABLE_TURNAROUNDS_TEST == 1
  case CPGC_CONFIG_TURNAROUNDS:
    //
    // Inputs: Rank ... has special values of ALL_COMBOS, DR_COMBOS, DD_COMBOS, SR_COMBOS
    // We actually don't do anything here with Rank though
    //
    break;
#endif
  case CPGC_CONFIG_EARLYTRAINING:
  case CPGC_CONFIG_EARLY_MPR_TRAINING:

    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0] = MrcData->CpgcOptions.Rank << (GetAddressBit(MrcData, Channel, MrcData->CpgcOptions.Rank) - (5 + MrcData->CpgcOptions.BusWidth));    
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_1] = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];
    MmrcExecuteTask (MrcData, CPGC_FIXVAR, NO_PRINT, Channel);  
    switch (MrcData->CpgcOptions.CmdSequence) {
    case CPGC_CMDSEQ_READ:
      MrcData->MspData.DynamicVars[Channel][SEQ_START]  = 1;
      MrcData->MspData.DynamicVars[Channel][SEQ_END]    = 1;
      break;
    case CPGC_CMDSEQ_WRITE:
      MrcData->MspData.DynamicVars[Channel][SEQ_START]  = 0;
      MrcData->MspData.DynamicVars[Channel][SEQ_END]    = 0;
      break;
    case CPGC_CMDSEQ_WRITE_READ:
      MrcData->MspData.DynamicVars[Channel][SEQ_START]  = 0;
      MrcData->MspData.DynamicVars[Channel][SEQ_END]    = 1;
      break;
    }
    MrcData->MspData.DynamicVars[Channel][AGG_PATTERN] = MrcData->CpgcOptions.Pattern;
    MrcData->MspData.DynamicVars[Channel][VIC_PATTERN] = MrcData->CpgcOptions.VicPattern;
    MmrcExecuteTask (MrcData, CPGC_CWL_PATTERN, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CPGC_SETUP_LVL, NO_PRINT, Channel);  
    if (MrcData->CpgcOptions.SetupType == CPGC_CONFIG_EARLYTRAINING) {
      break;
    }
    //
    // Setup the CPGC as Buffer Mode for Uniseq0/1
    //
    MmrcExecuteTask (MrcData, CPGC_MPR_TRAINING, NO_PRINT, Channel);    
    break;

#if RMT_ENABLE_LFSR_ADDRESS_TEST == 1
  case CPGC_CONFIG_LFSR_ADDRESS:
    break;
#endif
  case CPGC_CONFIG_CMD_STRESS:
    //
    // Inputs: Rank
    //
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0] = MrcData->CpgcOptions.Rank << ((MrcData->CpgcOptions.MinRankBit) - 5 - MrcData->CpgcOptions.BusWidth);
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_1] = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_2] = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];
    MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_3] = MrcData->MspData.DynamicVars[Channel][FIX_SEGMENT_RANK_SELECT_0];
    MmrcExecuteTask (MrcData, CPGC_FIXVAR, NO_PRINT, Channel);  
    break; 
  default:
    MrcDeadLoop ();
  } // Main case statement (SetupType)

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;

  return MMRC_SUCCESS;
}

UINT32
TestMrcTrainingSteps (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             SetupType,
  IN      UINT8             CmdSequence,
  IN      UINT16            PostTestFlag,
  IN      UINT8             LoopCount,
  IN      UINT8             NumBurst
)
{
  UINT8   Rank;
  UINT8   Strobe;
  UINT8   Bit;
  UINT8   MaxBits;
  UINT8   NumberOfElements[MAX_CHANNELS];
  UINT8   PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3];
  UINT32  ErrorsDetected;
  UINT8   ErrorsPerStrobe;
  UINT32  TempValue;
  UINT32  MinValue;
  UINT32  MaxValue;
  UINT16  DelayType;

  MinValue = RCV_MIN_VALUE;
  MaxValue = RCV_MAX_VALUE;
  DelayType = RecEnDelay;

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    MspDebugPrint((MSP_DBG_MIN, "    "));
    for (Strobe = 0; Strobe <  MrcData->MaxDq[Channel][Rank]; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
        continue;
      }
      MspDebugPrint((MSP_DBG_MIN, "S%02d ", Strobe));
    }
    MspDebugPrint((MSP_DBG_MIN, "\n"));

    break;
  }

  if (SetupType >= RCV_ENABLE_CHECKER && SetupType <= FWL_ENABLE_CHECKER) {
    if (SetupType == RCV_ENABLE_CHECKER) {
      MinValue  = RCV_MIN_VALUE;
      MaxValue  = RCV_MAX_VALUE;
      DelayType = RecEnDelay;
    } else if (SetupType == FWL_ENABLE_CHECKER) {
      MinValue  = FWL_MIN_VALUE;
      MaxValue  = FWL_MAX_VALUE;
      DelayType = TxDqsDelay;
    }
    ErrorsDetected = 0;
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      MspDebugPrint((MSP_DBG_MIN, "R%d  ", Rank));
      for (Strobe = 0; Strobe <  MrcData->MaxDq[Channel][Rank]; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, DelayType, CMD_GET_REG, &TempValue);
        MspDebugPrint((MSP_DBG_MIN, "%03d ", TempValue));
        if (TempValue > MaxValue || TempValue < MinValue) {
          ErrorsDetected |= (1 << Strobe);
        }
      }
      MspDebugPrint((MSP_DBG_MIN, "\n"));
    }
    return ErrorsDetected;
  }
  MmrcMemset(PassFail, 0, MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 3 * sizeof (UINT8));
  MrcData->CpgcOptions.LoopCount = LoopCount;
  MrcData->CpgcOptions.NumBursts = NumBurst;
  CpgcSetup (MrcData, Channel, SetupType);
  ErrorsDetected = 0;
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    if (SetupType == CPGC_CONFIG_EARLY_MPR_TRAINING) {
      PrechargeAll (MrcData, Channel, Rank);
      MprWrite (MrcData, Channel, Rank);
    }

    NumberOfElements[0] = MrcData->MaxDq[Channel][Rank];
    NumberOfElements[1] = MrcData->MaxDq[Channel][Rank];

    MrcData->CpgcOptions.Rank = Rank;
    MrcData->CpgcOptions.CmdSequence = CmdSequence;
    CpgcReconfig (MrcData, Channel);
    MrcData->CpgcOptions.RmtRestoreJedecResetType = PostTestFlag;
    CpgcExecuteRmtTest (MrcData, Channel, Rank, NumberOfElements, 1, PassFail);

    MspDebugPrint((MSP_DBG_MIN, "R%d  ", Rank));
    for (Strobe = 0; Strobe <  MrcData->MaxDq[Channel][Rank]; Strobe++) {
      if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, TRUE, &MaxBits)) {
        continue;
      }
      ErrorsPerStrobe = 0;
      for (Bit = 0; Bit < MaxBits; Bit++) {
        if (PassFail[Channel][0][Strobe][Bit][CURRENT_SAMPLE] == RESULTS_FAIL) {
          ErrorsDetected  |= (1 << Strobe);
          ErrorsPerStrobe |= (1 << Bit);
        }
      } // Bit loop ...
      MspDebugPrint((MSP_DBG_MIN, " %02x ", ErrorsPerStrobe));
    } // Strobe loop ...
    MspDebugPrint((MSP_DBG_MIN, "\n"));
    if (SetupType == CPGC_CONFIG_EARLY_MPR_TRAINING) {
      ToggleMprMode (MrcData, Channel, Rank, 0);
    }
  } // Rank loop ...
  return ErrorsDetected;
}

/**
  This function is mainly used for MRC RTL testing as a check that the DDRIO init left RD/WR in 
  good shape. It can also be used as training on RTL hook for the same purpose, make sure certain
  training is not breaking RD/WR transactions. To be run pre/post training step as debug aid.

  @param[in, out]  MrcData            Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex  Starting point within the CapsuleData structure
                                      where the Init should begin.
  @param[in]       StringIndex        When provided, this is the index into the string
                                      table to show the register names.
  @retval          Channel            Run in this channel.
**/
MMRC_STATUS
CpgcBaseLineTest (
  IN  OUT MMRC_DATA         *ModMrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
)
{
#if TRAINING_ALGOS == 1
  UINT8   Rank;
  INT32   i;
  UINT8   VA_LOOP_COUNT;
  UINT8   VA_NUM_BURSTS;
  if (
    (ModMrcData->SkipTrainingAlgos[LReceiveEnable] &&
     ModMrcData->SkipTrainingAlgos[LEarlyMpr] &&
     ModMrcData->SkipTrainingAlgos[LFineWriteLeveling] &&
     ModMrcData->SkipTrainingAlgos[LCoarseWriteLeveling] &&
     ModMrcData->SkipTrainingAlgos[LReadTraining] &&
     ModMrcData->SkipTrainingAlgos[LWriteTraining] &&
     ModMrcData->SkipTrainingAlgos[LCommandTraining])
    ) {
    return MMRC_SUCCESS;
  }
  PRINT_FUNCTION_INFO;
  MmrcExecuteTask (ModMrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
#if defined CPGC_ONETIME_INIT
  MmrcExecuteTask (ModMrcData, CPGC_ONETIME_INIT, NO_PRINT, Channel);
#endif

#if SIM
    VA_LOOP_COUNT=1;
    VA_NUM_BURSTS=3;  
#else
    VA_LOOP_COUNT=8; //change to 14 for max stress
    VA_NUM_BURSTS=6; //change to 14 for richer pattern generation
#endif

  ModMrcData->CpgcOptions.LoopCount = VA_LOOP_COUNT;
  ModMrcData->CpgcOptions.NumBursts = VA_NUM_BURSTS;
  CpgcSetup (ModMrcData, Channel, CPGC_CONFIG_VA);


  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    //
    // Only perform WRLevel training on the ranks that are enabled.
    //
    if (ModMrcData->RankEnabled[Channel][Rank]) {
      ModMrcData->CpgcOptions.Rank = Rank;
      ModMrcData->CpgcOptions.CmdSequence = CPGC_CMDSEQ_WRITE_READ;
      CpgcReconfig( ModMrcData, Channel );

      for (i=0; i< 1; i++) {
          //PrechargeAll (ModMrcData, Channel, Rank);  // This will add greater determinism because we have refresh on and so a precharge (Ron)
          //
          // Perform the required memory access.
          //
          MmrcExecuteTask (ModMrcData, CPGC_RUNTEST_PARALLEL_CLEAR, NO_PRINT, Channel);
          ModMrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] = (UINTX) (ModMrcData->MrcParameters.ChEnabled[Channel] && ModMrcData->MspData.ExecuteOnThisChannel[Channel]);
          MmrcExecuteTask (ModMrcData, CPGC_RUNTEST_PARALLEL, NO_PRINT, Channel);
          MmrcExecuteTask (ModMrcData, CPGC_RUNTEST_PARALLEL_CHECK, NO_PRINT, Channel);
      }
      MspDebugPrint((MSP_DBG_MIN, "BaseLine CPGC test channel:%d rank:%d ECC_ERR_STAT= 0x%08x\n", Channel, Rank, ModMrcData->MspData.DynamicVars[Channel][ECC_ERR_STAT]));
      MspDebugPrint((MSP_DBG_MIN, "BaseLine CPGC test channel:%d rank:%d BYTEGRP_ERR_STAT= 0x%08x\n", Channel, Rank, ModMrcData->MspData.DynamicVars[Channel][BYTEGRP_ERR_STAT]));
    } // MrcData->RankEnabled[Channel][Rank]
  } // Rank loop ...
#endif //#if TRAINING_ALGOS == 1
  return MMRC_SUCCESS;
}

/**
  DoZqCal Long across ranks within a channel

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DoZqCal (
  IN     MMRC_DATA *MrcData,
  IN     UINT8      Channel
  )
{
#if USE_GET_SET_FROM_SPREADSHEET
  BOOLEAN   ExecuteMode;
  DRAM_CMD_DDR4 Dram4Cmd;
  DramInitMisc  Dram3Cmd;
  UINT32        MiscCmd;
  UINT8         Rank;

  MiscCmd       = 0;
  Dram4Cmd.Data = 0;
  Dram3Cmd.Data = 0;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      Dram3Cmd.Bits.command       = 6;
      Dram3Cmd.Bits.bankAddress   = 0;
      Dram3Cmd.Bits.multAddress   = BIT10;
      Dram3Cmd.Bits.rankSelect    = Rank;

      MiscCmd = Dram3Cmd.Data;
    } else {
      Dram4Cmd.Bits.command         = 6;
      Dram4Cmd.Bits.act_n           = 1;
      Dram4Cmd.Bits.bankaddress     = 0;
      Dram4Cmd.Bits.bankgroup       = 0;
      Dram4Cmd.Bits.memoryaddress   = BIT10;
      Dram4Cmd.Bits.rankselect      = Rank;
      Dram4Cmd.Bits.ba              = 0;
      Dram4Cmd.Bits.mpr             = 0;
      Dram4Cmd.Bits.prea            = 0;

      MiscCmd = Dram4Cmd.Data;
    }
    MspDebugPrint ((MSP_DBG_TRACE, "R%d ZqCal Long\n", Rank));
    WriteDramCommandSideWrap (MrcData, Channel, Rank, MiscCmd);
  }

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
#endif
  return MMRC_SUCCESS;
}

/**
  Precharge all will detect the DDR type DDR3 vs DDR4 and apply 
  the correct command 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PrechargeAll (
  IN     MMRC_DATA *MrcData,
  IN     UINT8      Channel,
  IN     UINT8      Rank
  )
{
#if USE_GET_SET_FROM_SPREADSHEET
  BOOLEAN   ExecuteMode;
  DRAM_CMD_DDR4 Dram4Cmd;
  DramInitMisc  Dram3Cmd;
  UINT32        MiscCmd;

  MiscCmd       = 0;
  Dram4Cmd.Data = 0;
  Dram3Cmd.Data = 0;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
  if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    Dram3Cmd.Bits.command       = 2;
    Dram3Cmd.Bits.bankAddress   = 0;
    Dram3Cmd.Bits.multAddress   = BIT10;
    Dram3Cmd.Bits.rankSelect    = Rank;

    MiscCmd = Dram3Cmd.Data;
  } else {
    Dram4Cmd.Bits.command         = 2;
    Dram4Cmd.Bits.act_n           = 1;
    Dram4Cmd.Bits.bankaddress     = 0;
    Dram4Cmd.Bits.bankgroup       = 0;
    Dram4Cmd.Bits.memoryaddress   = BIT10;
    Dram4Cmd.Bits.rankselect      = Rank;
    Dram4Cmd.Bits.ba              = 0;
    Dram4Cmd.Bits.mpr             = 0;
    Dram4Cmd.Bits.prea            = 0;

    MiscCmd = Dram4Cmd.Data;
  }
  WriteDramCommandSideWrap (MrcData, Channel, Rank, MiscCmd);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
#endif
  return MMRC_SUCCESS;
}


/**
  NOP commands to all ranks

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
NopCommands (
  IN     MMRC_DATA *MrcData,
  IN     UINT8      Channel,
  IN     UINT8      Rank
  )
{
#if USE_GET_SET_FROM_SPREADSHEET
  BOOLEAN   ExecuteMode;
  DRAM_CMD_DDR4 Dram4Cmd;
  DramInitMisc  Dram3Cmd;
  UINT32        MiscCmd;

  MiscCmd       = 0;
  Dram4Cmd.Data = 0;
  Dram3Cmd.Data = 0;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
  if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    Dram3Cmd.Bits.command       = 7;
    Dram3Cmd.Bits.bankAddress   = 0;
    Dram3Cmd.Bits.multAddress   = 0;
    Dram3Cmd.Bits.rankSelect    = Rank;

    MiscCmd = Dram3Cmd.Data;
  } else {
    Dram4Cmd.Bits.command         = 7;
    Dram4Cmd.Bits.act_n           = 1;
    Dram4Cmd.Bits.bankaddress     = 0;
    Dram4Cmd.Bits.bankgroup       = 0;
    Dram4Cmd.Bits.memoryaddress   = 0;
    Dram4Cmd.Bits.rankselect      = Rank;
    Dram4Cmd.Bits.ba              = 0;
    Dram4Cmd.Bits.mpr             = 0;
    Dram4Cmd.Bits.prea            = 0;

    MiscCmd = Dram4Cmd.Data;
  }
  WriteDramCommandSideWrap (MrcData, Channel, Rank, MiscCmd);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
#endif
  return MMRC_SUCCESS;
}

#if TRAINING_ALGOS
MMRC_STATUS
CpgcRunCadbTest (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        Rank
)
{
#if ((COMMAND_TRAINING == 1 || RMT == 1) && TRAINING_ALGOS == 1)
  UINT8     i;
  UINT8     Knob;
  UINT8     RotationGroup;
  UINT8     RotationSignal;
  UINT8     ChannelBackup;
  UINT8     DevWidthIndex;
  BOOLEAN   FailureOnChannel[MAX_CHANNELS];
  UINT32    RowColAddress[4];
  UINT32    BankAddress[4];
  UINT32    Cmd[4];
  UINT8     MaxMaGroups;
  UINT8     MaxMaSignals;
  UINT8     *CadbMaGroupPtr;
  UINT8     DramType;
  UINT8     DebugLevel;
  
  DevWidthIndex   = MrcData->MaxDq[Channel][Rank] / 18;
  MaxMaGroups     = 0;
  MaxMaSignals    = 0;
  DramType        = 0xff;
  CadbMaGroupPtr  = (UINT8 *) NULL;
  DebugLevel      = 0;
  //
  // Backup Channel
  //
  ChannelBackup = Channel;
  //
  // Initialize the number of groups
  //
  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }

    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
#if defined(DDR3_SUPPORT) && DDR3_SUPPORT==1
      MaxMaGroups     = MAX_MA_GROUPS_DDR3;
      MaxMaSignals    = MAX_MA_SIGNALS_DDR3;
      DramType        = TypeDdr3All;
#endif
    } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
#if defined(DDR4_SUPPORT) && DDR4_SUPPORT==1
      MaxMaGroups     = MAX_MA_GROUPS_DDR4;
      MaxMaSignals    = MAX_MA_SIGNALS_DDR4;
      DramType        = TypeDdr4;
#endif
    } else {
      MaxMaGroups     = 0;
      MaxMaSignals    = 0;
      MrcData->ErrorCode = MrcErrInvalidDdrType;
      MrcDeadLoop ();
    }
    //
    // Both Channels must have the same DDR Type, so there is no need to sweep across all of them
    //
    break;
  } // Channel loop ...

  //
  // Restore back the Channel Under Test
  //
  Channel = ChannelBackup;

  MmrcMemset (FailureOnChannel, 0, MAX_CHANNELS);
  
  for (RotationGroup = 0; RotationGroup < MaxMaGroups; RotationGroup++) {
    MspDebugPrint ((DebugLevel, "RotationGroup %d\n", RotationGroup));

    //
    // Initialize buffer o be all aggressors
    //
    RowColAddress[0]  = 0;
    RowColAddress[1]  = 0xffff;
    RowColAddress[2]  = 0;
    RowColAddress[3]  = 0xffff;

    BankAddress[0]    = 0;
    BankAddress[1]    = 7;
    BankAddress[2]    = 0;
    BankAddress[3]    = 7;

    Cmd[0]            = 0;
    Cmd[1]            = 7;
    Cmd[2]            = 0;
    Cmd[3]            = 7;
    //
    // Initialize the Pointer the the right MA Group
    //
    if (DramType <= TypeDdr3All) {
#if defined(DDR3_SUPPORT) && DDR3_SUPPORT==1
      CadbMaGroupPtr  = (UINT8 *) &CadbMaGroupsDdr3[RotationGroup][0];
#endif
    } else if (DramType == TypeDdr4) {
#if defined(DDR4_SUPPORT) && DDR4_SUPPORT==1
      CadbMaGroupPtr  = (UINT8 *) &CadbMaGroupsDdr4[RotationGroup][0];
#endif
    } else {
      MrcData->ErrorCode = MrcErrInvalidDdrType;
      MrcDeadLoop ();
    }


    for (RotationSignal = 0; RotationSignal < MaxMaSignals; RotationSignal++) {

      MspDebugPrint ((DebugLevel, "RotationSignal[%d] = %d\n", RotationSignal, CadbMaGroupPtr[RotationSignal]));

      if (CadbMaGroupPtr[RotationSignal] == MMRC_NA) {
        continue;
      }

      for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) {
          continue;
        }
        if (FailureOnChannel[Channel]) {
          MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] = 0;
        } else {
          MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] = 1;
        }
        MspDebugPrint ((DebugLevel, "ENABLE_RUN_TEST[%d] = %d\n", Channel, (UINT32) MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST]));
        
      } // Channel loop ...
      if (DramType <= TypeDdr3All) {
        if (CadbMaGroupPtr[RotationSignal] <= MA15) {
          RowColAddress[1]  &= ~(1 << (CadbMaGroupPtr[RotationSignal] - MA0));
          RowColAddress[2]  |=  (1 << (CadbMaGroupPtr[RotationSignal] - MA0));
        } else if (CadbMaGroupPtr[RotationSignal] <= BA2) {
          BankAddress[1]    &= ~(1 << (CadbMaGroupPtr[RotationSignal] - BA0));
          BankAddress[2]    |=  (1 << (CadbMaGroupPtr[RotationSignal] - BA0));
        } else if (CadbMaGroupPtr[RotationSignal] <= WE) {
          Cmd[1]            &= ~(1 << (CadbMaGroupPtr[RotationSignal] - RAS));
          Cmd[2]            |=  (1 << (CadbMaGroupPtr[RotationSignal] - RAS));
        }
      } else if (DramType == TypeDdr4) {
        if (CadbMaGroupPtr[RotationSignal] <= MA15) {
          RowColAddress[1]  &= ~(1 << (CadbMaGroupPtr[RotationSignal] - MA0));
          RowColAddress[2]  |=  (1 << (CadbMaGroupPtr[RotationSignal] - MA0));
        } else if (CadbMaGroupPtr[RotationSignal] <= BA2) {
          BankAddress[1]    &= ~(1 << (CadbMaGroupPtr[RotationSignal] - BA0));
          BankAddress[2]    |=  (1 << (CadbMaGroupPtr[RotationSignal] - BA0));
        } else if (CadbMaGroupPtr[RotationSignal] <= WE) {
          Cmd[1]            &= ~(1 << (CadbMaGroupPtr[RotationSignal] - RAS));
          Cmd[2]            |=  (1 << (CadbMaGroupPtr[RotationSignal] - RAS));
        }
      }
    } // RotationSignal loop ...
    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }
      MrcData->MspData.DynamicVars[Channel][CADB_ROW_COL_0] = RowColAddress[0];
      MrcData->MspData.DynamicVars[Channel][CADB_ROW_COL_1] = RowColAddress[1];
      MrcData->MspData.DynamicVars[Channel][CADB_ROW_COL_2] = RowColAddress[2];
      MrcData->MspData.DynamicVars[Channel][CADB_ROW_COL_3] = RowColAddress[3];

      MrcData->MspData.DynamicVars[Channel][CADB_BANK_0]    = BankAddress[0];
      MrcData->MspData.DynamicVars[Channel][CADB_BANK_1]    = BankAddress[1];
      MrcData->MspData.DynamicVars[Channel][CADB_BANK_2]    = BankAddress[2];
      MrcData->MspData.DynamicVars[Channel][CADB_BANK_3]    = BankAddress[3];

      MrcData->MspData.DynamicVars[Channel][CADB_CMD_0]     = Cmd[0];
      MrcData->MspData.DynamicVars[Channel][CADB_CMD_1]     = Cmd[1];
      MrcData->MspData.DynamicVars[Channel][CADB_CMD_2]     = Cmd[2];
      MrcData->MspData.DynamicVars[Channel][CADB_CMD_3]     = Cmd[3];
    } // Channel loop ...
    for (i = 0; i < 4; i++) {
      MspDebugPrint ((DebugLevel, "RowColAddress[%d] = 0x%04x\n", i, RowColAddress[i]));
    }
    for (i = 0; i < 4; i++) {
      MspDebugPrint ((DebugLevel, "BankAddress[%d]   = 0x%04x\n", i, BankAddress[i]));
    }
    for (i = 0; i < 4; i++) {
      MspDebugPrint ((DebugLevel, "Cmd[%d]           = 0x%04x\n", i, Cmd[i]));
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    //
    // Prepare the CPGC Engine to test both channels
    //
    MmrcExecuteTask (MrcData, CPGC_CADB1_BUFx, NO_PRINT, Channel);
    //
    // Prepare the CPGC Engine to test both channels
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL_CLEAR, NO_PRINT, Channel);
    //
    // Run the test only in the Ch0, this is supposed to execute in both channels
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL, NO_PRINT, Channel);
    //
    // Check Errors in both CPGC engines
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL_CHECK, NO_PRINT, Channel);

    for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) {
        continue;
      }

      MspDebugPrint ((DebugLevel, "Ch%d ", Channel));

      for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
        for (i = 0; i < MAX_STROBES; i++) {
          if (!IsStrobeValid (MrcData, Channel, Rank, i, FALSE, NULL)) {
            continue;
          }
          //
          // Look at the dynamic variable holding the error data and mask off the appropriate
          // bits for this element (typically a strobe). Record fail if it is non zero.
          //
          if ((MrcData->MspData.DynamicVars[Channel][StrobeToErrorMask[DevWidthIndex][i].DynamicVar + (2 * Knob)] & 
            StrobeToErrorMask[DevWidthIndex][i].ErrorMask) != 0 || FailureOnChannel[Channel]) {
            FailureOnChannel[Channel] = TRUE;
            MspDebugPrint ((DebugLevel, "S%d-- ", i));
          } else {
            MspDebugPrint ((DebugLevel, "S%d-P ", i));
          }
        } // i loop ...
        MspDebugPrint ((DebugLevel, "\n"));
      } // Knob loop ...
      MspDebugPrint ((DebugLevel, "\n"));

      MspDebugPrint ((DebugLevel, "Rank: %d\n", Rank));
      MspDebugPrint ((
        DebugLevel,
        "  ALL LANES (HIGH, LOW)    = 0x%08x, 0x%08x\n",
        (UINT32)MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT47],
        (UINT32)MrcData->MspData.DynamicVars[Channel][LANE_ERR_STAT03]
      ));
      MspDebugPrint ((
        DebugLevel,
        "  LANE_ERR(Bytelane Chunk) = 0x%02x\n",
        (UINT32)MrcData->MspData.DynamicVars[Channel][CHUNK_STAT]
      ));
    } // Channel loop ...
#ifdef SIM
    //
    // For Simulation we are only interesting on testing a single pattern
    //
    break;
#endif
  } //  RotationGroup loop ...

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    for (i = 0; i < MAX_STROBES; i++) {
      if (!IsStrobeValid (MrcData, Channel, Rank, i, FALSE, NULL)) {
        continue;
      }
      if (FailureOnChannel[Channel]) {
        MrcData->MspData.DynamicVars[Channel][StrobeToErrorMask[DevWidthIndex][i].DynamicVar] = 0xffffffff;
      } else {
        MrcData->MspData.DynamicVars[Channel][StrobeToErrorMask[DevWidthIndex][i].DynamicVar] = 0;
      }
    } // i loop ...
  } // Channel loop ...

#endif
  return MMRC_SUCCESS;
}
#endif
/**
  Start the Cpgc test in both channels (only if both are
  enabled). DynamicVars will contains all the errors found
  during the execution of the test

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          MMRC_SUCCESS    In case the setup is done properly
**/
MMRC_STATUS
CpgcExecuteParallel (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        Rank
  )
{
  BOOLEAN   SingleTest;

  SingleTest = TRUE;

  switch (MrcData->CpgcOptions.SetupType) {
  case CPGC_CONFIG_MEMINIT:
  case CPGC_CONFIG_MEMTEST:
  case CPGC_CONFIG_VA:
  case CPGC_CONFIG_EARLYTRAINING:
  case CPGC_CONFIG_EARLY_MPR_TRAINING:
    break;
  case CPGC_CONFIG_TURNAROUNDS:
    SingleTest = FALSE;
    break;
  case CPGC_CONFIG_LFSR_ADDRESS:
    SingleTest = FALSE;
    break;
#if TRAINING_ALGOS && COMMAND_TRAINING
  case CPGC_CONFIG_CMD_STRESS:
    SingleTest = FALSE;
    CpgcRunCadbTest (MrcData, Channel, Rank);
    break;
#endif 

  }
  if (SingleTest) {
    //
    // Prepare the CPGC Engine to test both channels
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL_CLEAR, NO_PRINT, Channel);
    //
    // Run the test only in the Ch0, this is supposed to execute in both channels
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL, NO_PRINT, Channel);
    //
    // Check Errors in both CPGC engines
    //
    MmrcExecuteTask (MrcData, CPGC_RUNTEST_PARALLEL_CHECK, NO_PRINT, Channel);
  }

  return MMRC_SUCCESS;
}


/**
  Wrapper function to call memory init or memory test.  Also handles error
  conditions such as whether or not to halt the system on errors 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       MemInitFlag     0 for memtest, 1 for meminit. 
   
  @retval          Status of success or failure, depending on 
                   errors observed. Also prints out which rank
                   failed the CPGC test if failure occurs. For
                   rank detection to work, the address map must
                   be left in its default state. If MRC changes
                   the address map, this function must be
                   modified to change it back to default at the
                   beginning, then restore it at the end.
**/
MMRC_STATUS
MemInit (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        MemInitFlag
  )
{
#if ECC_SCRUB == 1 || MEMORY_TEST == 1
  UINT32    LoopNum;
  UINT32    MaxLoopNum;
  UINT32    Errors;
  UINT8     StrobeLane;
  UINT8     ChannelBackup;
  UINT8     Sequence;
  UINT8     PatternSelect;
  BOOLEAN   GlobalError;


  LoopNum       = 0;
  MaxLoopNum    = 1;
  GlobalError   = FALSE;
  ChannelBackup = Channel;
  //
  // Set the total number of memory tests to run.
  //
  if (MrcData->MemoryTestLoop != 0) {
    MaxLoopNum = MrcData->LoopCount;
  }
  switch (MemInitFlag) {
  case MrcMemInit:
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] = (UINTX) (MrcData->MrcParameters.ChEnabled[Channel] && MrcData->MspData.ExecuteOnThisChannel[Channel]);
      //
      // If user requests memory init, write 0s to all the memory and return.
      //
      MrcData->MspData.DynamicVars[Channel][SEQ_START]       = 0;
      MrcData->MspData.DynamicVars[Channel][SEQ_END]         = 0;
      MrcData->MspData.DynamicVars[Channel][BUF_START_PTR]   = 0;
      MrcData->MspData.DynamicVars[Channel][BUF_END_PTR]     = 0;
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_MEMINIT);
      CpgcReconfig (MrcData, Channel);
      MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, 0, Channel);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    //
    // Setup, execute and read errors when parallel training is enabled
    //
    CpgcExecuteParallel (MrcData, Channel, 0);

    return MMRC_SUCCESS;
  case MrcMemTest:
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      CpgcSetup (MrcData, Channel, CPGC_CONFIG_MEMTEST);
    }
    //
    // Restore back the Channel Under Test
    //
    Channel = ChannelBackup;
    while (LoopNum < MaxLoopNum) {
      LoopNum++;
      //
      // If we're cycling, display the current count.
      //
      if (MrcData->MemoryTestLoop != 0) {
        MspDebugPrint ((MSP_DBG_MAX, "Memtest %d\n", LoopNum));
      }
      for (PatternSelect = 0; PatternSelect < 4; PatternSelect++) {
        for (Sequence = 0; Sequence < 2; Sequence++) {
          //
          // Sequence = 0; will execute Writes
          // Sequence = 1; will execute Reads
          //
          for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
            MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] = (UINTX) (MrcData->MrcParameters.ChEnabled[Channel] && MrcData->MspData.ExecuteOnThisChannel[Channel]);
            MrcData->MspData.DynamicVars[Channel][SEQ_START]       = Sequence;
            MrcData->MspData.DynamicVars[Channel][SEQ_END]         = Sequence;
            MrcData->MspData.DynamicVars[Channel][BUF_START_PTR]   = PatternSelect;
            MrcData->MspData.DynamicVars[Channel][BUF_END_PTR]     = PatternSelect;
            // 
            // Setup all CPGC registers for first write pass of memtest.
            //
            CpgcReconfig (MrcData, Channel);
          }
          //
          // Restore back the Channel Under Test
          //
          Channel = ChannelBackup;
          //
          // Setup, execute and read errors when parallel training is enabled
          //
          CpgcExecuteParallel (MrcData, Channel, 0);

          if (Sequence == 1) {
            for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
              if (MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] == 0) {
                continue;
              }
              //
              // Check Errors during Read Sequence
              //
              Errors = (UINT32) MrcData->MspData.DynamicVars[Channel][BYTEGRP_ERR_STAT];
              if (MrcData->MrcParameters.EccEnabled[Channel]) {
                Errors |= (UINT32)(MrcData->MspData.DynamicVars[Channel][ECC_ERR_STAT] << 8);
              }

#if FAULTY_PART_TRACKING
              if (MrcData->FaultyPartTracking) {
                CpgcSCheckErrors (MrcData, Channel, 0, NULL, NULL, &Errors);
              }
#endif
              //
              // If there are errors, decode them and optionally halt the system.
              //
              if (Errors != 0) {
                MspDebugPrint ((MSP_DBG_MIN, "Memtest failed on channel %d - 0x%08x\n", Channel, Errors));
                //
                // Print out which bytelanes had errors.
                //
                for (StrobeLane = 0; StrobeLane < MAX_STROBES; StrobeLane++) { 
                  if (!IsStrobeValid(MrcData, Channel, 0, StrobeLane, FALSE, NULL)) {
                    continue;
                  }
                  //
                  // StrobeLane errors indicators are in bits 16:8.
                  //
                  if ((Errors & (1 << (StrobeLane))) != 0) {
                   MspDebugPrint ((MSP_DBG_MIN, "Error on strobe %d\n", StrobeLane));
                  }
                }
                  GlobalError = TRUE;
              } // Errors != 0

            } // Channel loop ...
          } // Sequence == 1 (Read Sequence)
        } // Sequence loop ...
      } // PatternSelect loop ...
    } // LoopNum < MaxLoopNum loop ...
    if (GlobalError) {
      return MMRC_FAILURE;
    }
    break;
  default:
    return MMRC_FAILURE;
  }
#endif
  return MMRC_SUCCESS;
}

/**
  This function develops the scrub memory functionality 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ScrubMemory (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if ECC_SCRUB == 1
  MMRC_STATUS   Status;
  BOOLEAN       BackupEnable;

  BackupEnable = MrcData->MspData.Enabled[0];
  Status = MMRC_SUCCESS;

  MspDebugPrint ((MSP_DBG_MIN, "MrcData->MrcParameters.EccEnabled = %d\n", MrcData->MrcParameters.EccEnabled[Channel]));

  if (MrcData->MemoryOverwriteRequestControl) {
    MspDebugPrint ((MSP_DBG_MIN, "MemoryOverwriteRequestControl\n"));
    Status = MemInitPerDimm (MrcData, Channel);
  } else if (MrcData->MemoryPreservation && ((MrcData->MrcParameters.BootMode == S0) || (MrcData->MrcParameters.BootMode == ADR))) {
    //
    // Skip memory initialization
    //
    MspDebugPrint ((MSP_DBG_MIN, "Skip memory init (Memory Preservation)\n"));
  } else if (((MrcData->SkipOptions & SKIP_MEMINIT) == 0) && !IsAdrFlow (MrcData)) {
    //
    // Use CPGC to initialize memory per Dimm basis.
    //
    MspDebugPrint ((MSP_DBG_MIN, "Memory Init executed (MemInitPerDimm)\n"));
    Status = MemInitPerDimm (MrcData, Channel);
  }
  if (MrcData->MrcParameters.EccEnabled[Channel]) {
    MrcData->MspData.DynamicVars[Channel][UCE_FILTER] = 0x03;
    MmrcExecuteTask (MrcData, InitializeEcc, 0, Channel);
    MrcData->MspData.Enabled[0] = TRUE;
    MmrcExecuteTask (MrcData, EnableScrub, 0, Channel);
    MrcData->MspData.Enabled[0] = BackupEnable;

    MspDebugPrint ((MSP_DBG_MIN, "ECC Enabled successfully\n"));
  } else {
    MrcData->MspData.DynamicVars[Channel][UCE_FILTER] = 0;
    MmrcExecuteTask (MrcData, SetUceFilter, 0, Channel);
  }
  return Status;
#else
  return MMRC_SUCCESS;
#endif // ECC_SCRUB == 1
}

/**
  This function displays all the information related with Dunit 
  settings, Dimm/Dram Manufacturer, etc 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DisplayInformation (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
  UINT8     MrsCommand;
  UINT8     Rank;
  UINT8     ItemId;
  UINT8     DimmId;
  UINT8     DramId;
  UINT8     Dimm;
  UINT16    DimmManufacturerIdOffset;
  UINT16    DramManufacturerIdOffset;

  if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    DimmManufacturerIdOffset = SPD_DDR3_MANUFACTURER_ID_HI;
    DramManufacturerIdOffset = SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH;
  } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    DimmManufacturerIdOffset = SPD_DDR4_MMIDH;
    DramManufacturerIdOffset = SPD_DDR4_MIDCH;
  } else {
    return MMRC_SUCCESS;
  }

  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    MspDebugPrint ((MSP_DBG_MIN, "Slot[%d] - ", Dimm));
    if (MrcData->MrcParameters.DimmPresent[Channel][Dimm] == 0) {
      MspDebugPrint ((MSP_DBG_MIN, "Not present\n"));
      continue;
    }
    DimmId  = 0xff;
    DramId  = 0xff;
    //
    // Determine the Dimm Manufacturer Id
    //
    MspDebugPrint ((MSP_DBG_MIN, "DimmId {0x%02x : ", 
      MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[DimmManufacturerIdOffset]
      ));
    for (ItemId = 0; ItemId < (sizeof (ManufacturerId) / sizeof (ManufacturerId[0])); ItemId++) {
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[DimmManufacturerIdOffset] == ManufacturerId[ItemId]) {
        DimmId = ItemId;
      }
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[DramManufacturerIdOffset] == ManufacturerId[ItemId]) {
        DramId = ItemId;
      }
    }
    if (DimmId > (sizeof (ManufacturerId) / sizeof (ManufacturerId[0]))) {
      MspDebugPrint ((MSP_DBG_MIN, "Unknown  }  - "));
    } else {
      MspDebugPrint ((MSP_DBG_MIN, "%s} - ", ManufacturerStringId[DimmId]));
    }
    //
    // Determine the Dram Manufacturer Id
    //
    MspDebugPrint ((MSP_DBG_MIN, "DramId {0x%02x : ", 
      MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[DramManufacturerIdOffset]
      ));
    if (DramId > (sizeof (ManufacturerId) / sizeof (ManufacturerId[0]))) {
      MspDebugPrint ((MSP_DBG_MIN, "Unknown  }"));
    } else {
      MspDebugPrint ((MSP_DBG_MIN, "%s}", ManufacturerStringId[DramId]));
    }
    MspDebugPrint ((MSP_DBG_MIN, "\n"));
  } // Dimm loop ... 

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (MrcData->RankEnabled[Channel][Rank]) {
      MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValid = TRUE;
    } else {
      MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValid = FALSE;
      continue;
    }
    MspDebugPrint ((MSP_DBG_MIN, "R%d\n", Rank));
    for (MrsCommand = 0; MrsCommand < MR_COUNT; MrsCommand++) {
      MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValue[MrsCommand] = (UINT32) MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][MrsCommand]];
      MspDebugPrint ((MSP_DBG_MIN, "  Mrs[%d]: 0x%08x\n", MrsCommand, MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValue[MrsCommand]));
    } // MrsCommand loop ...
  } // Rank loop ...


  return MMRC_SUCCESS;
}

/**
  This function does a memory testing 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MemoryTest (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if MEMORY_TEST == 1
  MMRC_STATUS Status;
  BOOLEAN     BackupEnable;
  
  //
  //set EXTBUF to default values before memory test
  //
  SetExtDefault(MrcData, Channel);
  if (MrcData->SkipTrainingAlgos[LMemoryTest]) {
    MspDebugPrint ((MSP_DBG_MIN, "Skipped\n"));
    return MMRC_SUCCESS;
  }

     if (MrcData->MemoryPreservation && ((MrcData->MrcParameters.BootMode == S0) || (MrcData->MrcParameters.BootMode == ADR))) {
    //
    // Skip memory test
    //
    MspDebugPrint ((MSP_DBG_MIN, "Skip memory test (Memory Preservation)\n"));
    return MMRC_SUCCESS;
  }

  if (MrcData->MspData.ExecuteThisRoutineInParallel) {
    MspDebugPrint ((MSP_DBG_MIN, "Testing: %dMB\n", MrcData->MrcParameters.SystemMemorySize));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "CH%d Testing: %dMB\n", Channel, MrcData->MrcParameters.TotalMem[Channel]));
  }
  MspDebugPrint ((MSP_DBG_MIN, "CH%d Test TOTAL_CACHE_LINES 0x%08x\n", 0, MrcData->MspData.DynamicVars[0][TOTAL_CACHE_LINES]));
  MspDebugPrint ((MSP_DBG_MIN, "CH%d Test TOTAL_CACHE_LINES 0x%08x\n", 1, MrcData->MspData.DynamicVars[1][TOTAL_CACHE_LINES]));
  //
  // Run memory test on all ranks and halt if an error is observed.
  //
  Status = MemInit (
             MrcData,
             Channel,
             MrcMemTest
           );
  if (Status != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Memory test failures detected, write soc.ddr.dunit0.skpd4 = 0 to continue\n"));
    EnableMrcBreakpoint ();
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "\n"));
    DisableMrcBreakpoint ();
  }
  BackupEnable = MrcData->MspData.Enabled[0];
  MrcData->MspData.Enabled[0] = TRUE;
  MmrcExecuteTask(MrcData, BREAKPOINTMRC, NO_PRINT, 0);
  MrcData->MspData.Enabled[0] = BackupEnable;

  Status = 0;
  if (MrcData->HaltSystemOnMemTestError == MrcHaltSystemOnError && Status != MMRC_SUCCESS) {
    return MrcErrMemTest;
  }
  return Status;
#else
  return MMRC_SUCCESS;
#endif // MEMORY_TEST
}

/**
  Saves the MRC host structure to an MMX register for future use.

  @param[in, out]  MrcDataAddress  Address of MRC Host Structure.
  @retval          NONE
**/
VOID
SaveMrcHostStructureAddress (
  IN    MMRC_DATA   *MrcDataAddress
  )
{
#if SIM || JTAG
  MyMrcData = MrcDataAddress;
#else
#if __GNUC__
  asm (
    "movd %0,%%mm3;"
    :/* no output */
    : "c" (MrcDataAddress)
    :/* no clobber */
    );
#else
  _asm {
    movd mm3, MrcDataAddress;
  }
#endif
#endif
}

/**
  Returns the MRC host structure from an MMX register.

  @retval          MrcDataAddress  Address of MRC Host Structure.
**/
MMRC_DATA *
GetMrcHostStructureAddress (
  )
{
#if SIM || JTAG
  return MyMrcData;
#else
  MMRC_DATA *MrcData;
#if __GNUC__
  asm (
    "movd %%mm3,%0;"
    : "=c" (MrcData)
    :/* no input */
    :/* no clobber */
    );
#else
  _asm {
    movd MrcData, mm3;
  }
#endif
  return MrcData;
#endif
}

VOID
PrintMemoryErrorCode (
  IN        UINT8                 PostCode,
  IN        UINT8                 IndexError
  )
{
#if MRC_HUMAN_READABLE_ERRORS
  MmrcErrorPrint ((MSP_DBG_MIN, "CP 0x%02X, Status[%d]: %s\n", PostCode, IndexError, (UINT8 *) &mErrorCode[IndexError][0]));
#else
  //
  // Human readable messages are disabled so just show the error index.
  //
  MmrcErrorPrint ((MSP_DBG_MIN, "CP 0x%02X, Status[%d]\n", PostCode, IndexError));
#endif
  return;
}

VOID MrcDeadLoop (
  )
{
  VOLATILE UINT32  DeadLoopVar;
  MMRC_DATA       *DeadLoopMrcData;
#if PHYVIEW_TABLE  
  UINT8            Channel;
#endif

  DeadLoopVar = 1;
  DeadLoopMrcData = GetMrcHostStructureAddress ();
  DeadLoopMrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
  //
  // First display all delays which is useful to see where training went wrong.
  //
#if PHYVIEW_TABLE
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (DeadLoopMrcData->MrcParameters.ChEnabled[Channel]) {
      MmrcMemset (&DeadLoopMrcData->MspData.ExecuteOnThisChannel[0], 0, MAX_CHANNELS * sizeof (BOOLEAN));
      DeadLoopMrcData->MspData.ExecuteOnThisChannel[Channel] = DeadLoopMrcData->MrcParameters.ChEnabled[Channel];
      PhyViewTable (DeadLoopMrcData, 0, MMRC_NA, Channel);
    }
  }
#endif
  //
  // Now dump all registers in the spreadsheet. Note that for this to work,
  // DUMP_REGISTER_NAMES must be set to 1 in the spreadsheet.
  //  
  DumpAllRegisters ();
  //
  // Handle error codes generically and with an OEM callout.
  //
  PrintMemoryErrorCode (DeadLoopMrcData->PostCode, (UINT8) DeadLoopMrcData->ErrorCode);
  OemMemoryErrorCode ((UINT8)DeadLoopMrcData->ErrorCode);

#if SIM
  //
  // In RTL simulation environment, call back to the test bench letting them
  // know we've hit a fatal error.
  //
  MyFatalMRCError(); 
#endif
  while (DeadLoopVar);
}

/**
  Performs Early Command Training.   If the SEARCH_FOR_EARLY_CMD_PASS 
  is set, the system will test the default value and verify that 
  this value passes the test, if it fails, it will begin 
  searching for a default value checking +/- 1/2 CLK. Otherwise, 
  if the SEARCH_FOR_EARLY_CMD_PASS is not set, the system will 
  "assume" the default value is passing and will search for the 
  best value. 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
EarlyCommandTraining (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  )
{
#if EARLY_COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
  UINT32 InitialCommandDelay[MAX_RANKS][MAX_CMDS];
  UINT32 FinalCommandDelay[MAX_RANKS][MAX_CMDS];
  UINT32 TotalCommandDelay[MAX_CMDS];
  UINT8  CmdSweepDelta;
  UINT8  Rank;
  UINT8  FindDefaultValueFlag;
  UINT32 Divisor;
  UINT16 HalfClock;
  UINT8  CmdLoop;
  UINT16 Dim1Start[MAX_KNOBS][MAX_STROBES][2];
  UINT16 Dim1Min[MAX_STROBES];
  UINT16 Dim1Max[MAX_STROBES];
  UINT16 Results[MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT8  ThisGroup;
#if SEARCH_FOR_EARLY_CMD_PASS
  UINT8  Pass;
  UINT16 TotalDelta;
  UINT32 Direction;
  UINT32 TempValue;
#endif
  //
  // Loop through all enabled ranks getting the initial Cmd values.
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (MrcData->RankEnabled[Channel][Rank]) {
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CmdGrp0 + CmdLoop, CMD_GET_REG_UC, &InitialCommandDelay[Rank][CmdLoop]);
      }
    }
  }

  //
  // Loop through all enabled ranks performing the early command training.
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (MrcData->RankEnabled[Channel][Rank]) {
      MspDebugPrint ((MSP_DBG_MAX, "Channel %d, Rank %d\n  ", Channel, Rank));
      //
      // Initialize the variables.
      //
      CmdSweepDelta = 16;
      Channel = 0;
      FindDefaultValueFlag = TRUE;
      HalfClock = GetHalfClk (MrcData, Channel);
      //
      // Reset the initial values of the command delays, which could have been changed for the prievious rank.
      //
      for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
        GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CmdGrp0 + CmdLoop, CMD_SET_VAL_FC_UC, &InitialCommandDelay[Rank][CmdLoop]);
      }
#ifdef SEARCH_FOR_EARLY_CMD_PASS
      //
      // Test if the initial Value will pass.
      //
      Pass = (UINT8) CATrainingResultsPass (MrcData, Channel, Rank, NUMBER_EARLY_COMMAND_RETRIES);
      //
      // If the test passed, then begin the sweep calculation, otherwise, sweep across a 1/2CLK around the initial value for a pass.
      //
      if (Pass == FALSE ) {
        MspDebugPrint ((MSP_DBG_MAX, "** Default Value failed, searching for Nominal Value **\n" ));
        // 
        // The training will loop +delta, -delta, +2delta, -2delta, +3delta.. until either a pass/fail occurs
        // or a complete 2x was traversed.  If a 2x has been traversed, an error is flagged to the operator.
        //
        TotalDelta = 0;
        Direction  = 1;
        //
        // Loop until an error or a pass occurs.
        //
        while (1)  {

          TotalDelta += 16;
          //
          // Check for limit reached because the delta is a complete 2x clock away from the initial value.
          //
          if (TotalDelta > 2 * HalfClock) {
            MspDebugPrint ((MSP_DBG_MIN, "Error: Search Beyond 1/2 CLK!\n"));
            return MrcErrSearchBeyondHalfClk;
          }

          MspDebugPrint ((MSP_DBG_MAX, "Offset : %02d \t", TotalDelta*Direction));
          //
          // Set the new CmdValue and rerun Jedec.
          //
          TempValue = TotalDelta * (UINT32) Direction;
          GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CmdAll, CMD_SET_OFFSET_UC, &TempValue);
          MmrcExecuteTask (MrcData, JedecInit, NO_PRINT, Channel);

          if (CATrainingResultsPass (MrcData, Channel, Rank, NUMBER_EARLY_COMMAND_RETRIES) == TRUE) {
            MspDebugPrint ((MSP_DBG_MAX, " P\n"));
            break;
          }
          MspDebugPrint ((MSP_DBG_MAX, " F\n"));

          Direction *= -1;
        }
      }
#endif

     for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
     // GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CmdGrp0, CMD_GET_REG_UC, &InitialCommandDelay[CmdLoop]);
      Dim1Start[0][CmdLoop][LOW] = (UINT16) InitialCommandDelay[Rank][CmdLoop];
      if (InitialCommandDelay[Rank][CmdLoop] > HalfClock) {
        Dim1Min[CmdLoop] = (UINT16) InitialCommandDelay[Rank][CmdLoop]-HalfClock;
      } else {
       Dim1Min[CmdLoop] = 0;
      }
      Dim1Max[CmdLoop] = (UINT16) InitialCommandDelay[Rank][CmdLoop] + HalfClock;
      Dim1Start[0][CmdLoop][HIGH] = (UINT16) InitialCommandDelay[Rank][CmdLoop];
    }
    // 
    // Do the 1D Sweep.
    //
    MspDebugPrint ((MSP_DBG_MAX, "Sweeping: Min=%d, Max=%d, Start=%d, Step=%d\n",
      Dim1Min[0], Dim1Max[0], Dim1Start[0][0][LOW], ECT_STEP_SIZE
      ));
    Create1DSweep (MrcData, Channel, Rank, CmdAll, MAX_CMDS, Dim1Start, Dim1Min, Dim1Max, ECT_STEP_SIZE,
      FALSE, FALSE, FALSE, MAX_CMDS, EarlyCmdTrainingTest, Results, "ECT", 1, RUN_PER_STROBE
      );

    for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
      if (Results[0][CmdLoop][0][HIGH] > Results[0][CmdLoop][0][LOW]) {
        FinalCommandDelay[Rank][CmdLoop] = (Results[0][CmdLoop][0][HIGH] + Results[0][CmdLoop][0][LOW])/2;
      } else {
        MrcDeadLoop ();
      }
     }
   }
  }
  //
  // Loop through all enabled ranks averaging them.
  //
  for (CmdLoop = 0; CmdLoop < MAX_CMDS; CmdLoop++) {
    Divisor = 0;
    TotalCommandDelay[CmdLoop]=0;
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (MrcData->RankEnabled[Channel][Rank]) {
        TotalCommandDelay[CmdLoop] += FinalCommandDelay[Rank][CmdLoop];
        Divisor += 1;
      }
    }
    TotalCommandDelay[CmdLoop]/= Divisor;

    MspDebugPrint ((MSP_DBG_MAX, "Final Delay %d=%d\n", CmdLoop, TotalCommandDelay[CmdLoop]));
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      GetSetDdrioGroup2 (MrcData, Channel, Rank, 0, CmdGrp0 + CmdLoop, CMD_SET_VAL_FC_UC, &TotalCommandDelay[CmdLoop]);
    }
  }
  //
  // Perform JEDEC on all ranks for the channel.
  //
  MmrcExecuteTask (MrcData, JedecInit, NO_PRINT, Channel);
#endif
  return MMRC_SUCCESS;
}
#if EARLY_COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
/**
  Sets the Command/Address pins on the CA Bus for LPDDR3. This 
  function will transmit the LPDDR3 Rise/Fall data on the CA 
  Bus.  The 2 modes are 3N Mode or normal mode.  When in 3N 
  mode, the chip select is extended to 3 clocks (1 full clock 
  before the data and 1 full clock after the data), the data is 
  extended to 2 clocks with the data being sent out 1/2 clock 
  before the data and left 1/2 clock.  The normal mode is for 
  the CS to be enabled for just 1 clock, and the rise/fall data 
  to be applied on the rise/fall of the 1 clock. 
  

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       CARise          Data to be put on the LPDDR3 Rising clock data.
  @param[in]       CAFall          Data to be put on the LPDDR3 Rising clock data.
  @param[in]       Mode3N          Binary flag on whether the data pattern should be 
                                   transmitted in 3N mode, 
                                   0-> Normal Mode, 1-> Extended Mode.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandAddressPinControl (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT16        CARise,
  IN        UINT16        CAFall,
  IN        UINT8         Mode3N
  )
{
  //
  // If in 3N mode, issue the command to set the extended chip select an to enable the 3N mode.
  // Rise/Fall are actually populated with the Rise data structure, since in extended mode the same data
  // must be applied to the rise and fall data.
  //
  if (Mode3N == TRUE) {
    MmrcExecuteTask (MrcData, SET_ECSEN, NO_PRINT, Channel);
    MrcData->MspData.DynamicVars[Channel][REG_DATA] = (Mode3N << 31) | (Rank << 22) | (CARise << 10 | (CARise));
    MmrcExecuteTask (MrcData, SEND_DRAM_INIT, NO_PRINT, Channel);
    MmrcExecuteTask (MrcData, CLR_ECSEN, NO_PRINT, Channel);
  } else {
    // 
    // If not in 3N mode, simply issue the command with the data in Rise/Fall being sent out accordingly.
    //
    MrcData->MspData.DynamicVars[Channel][REG_DATA] = (Mode3N << 31) | (Rank << 22) | (CAFall << 10 | (CARise));
    MmrcExecuteTask (MrcData, SEND_DRAM_INIT, NO_PRINT, Channel);
  }
  return MMRC_SUCCESS;
}
#endif

#if EARLY_COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
/**
  For Early Command training, tests the CA Bus by transmitting a known set 
  of patterns across the CA Bus a number of times.  The actual 
  number of retries is a parameter to this algorithm.  The 
  number of patterns and the patterns themselves are compile 
  time generated. 
  
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined. 
  @param[in]       Retries         Runtime parameter to support the number 
                                   of retries that should be executed on the
                                   pattern to verify errors/no errors occurred.
  @retval          TRUE            If Passes 
  @retval          FALSE           Otherwise 
**/
MMRC_STATUS
CATrainingResultsPass (
  IN OUT    MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Retries
  )
{
  UINT8 PatternIndex;
  UINT8 RetryCount;
  UINT32 MR41Value;
  UINT32 MR48Value;
  UINT8 Pass;
  //
  // Loop through the patterns until a failure is detected or all patterns pass.
  //
  for (PatternIndex = 0; PatternIndex < NUMBER_EARLY_COMMAND_PATTERNS; PatternIndex++) {
    //
    // Within each pattern, continue sending out the same pattern for a total number of "retries" which is a runtime parameter.
    //
    for (RetryCount = 0; RetryCount < Retries; RetryCount++)  {
      // 
      // Send out the MR41 command to put the DRAM in Training mode. Must exit out CA Training mode from the DUNIT/PHY
      // to send any data to the DRAMS. The MR41 allows us to test all CA pins except CA4 and CA9.
      //
      MmrcExecuteTask (MrcData, CLR_CA_TRAIN, NO_PRINT, Channel);
      CommandAddressPinControl (MrcData, Channel, Rank, MRW41, MRW41, TRUE);
      MmrcExecuteTask (MrcData, SET_CA_TRAIN, NO_PRINT, Channel);
      // 
      // Transmit out the test pattern.
      //
      CommandAddressPinControl (MrcData, Channel, Rank, EarlyCommandPatternsRise[PatternIndex],
        EarlyCommandPatternsFall[PatternIndex], FALSE
        );
#if SIM
      //
      // For simulation purposes, stall the system 100ps so that the result can be captured.
      //
      MySimStall (100);    
#endif
      //
      // Retrieve the result from the pattern capture register.
      //
      MmrcExecuteTask (MrcData, GET_CA_TRAIN, NO_PRINT, Channel);
      MR41Value = (UINT32)MrcData->MspData.DynamicVars[Channel][REG_DATA];
      // 
      // Exit CA Training mode from the DUNIT/PHY and tell the DRAM to exit CA Training mode.
      //
      MmrcExecuteTask (MrcData, CLR_CA_TRAIN, NO_PRINT, Channel);
      CommandAddressPinControl (MrcData, Channel, Rank, MRW42, MRW42, TRUE);
      //
      // If the first result passes, then continue testing the remaining CA pins.
      //
      if (MR41Value == EarlyCommandResults41[PatternIndex]) {
        //
        // Send out the MR48 command to the DRAM to begin testing CA4 and CA9.
        //
        CommandAddressPinControl (MrcData, Channel, Rank, MRW48, MRW48, TRUE);
        MmrcExecuteTask (MrcData, SET_CA_TRAIN, NO_PRINT, Channel);
        //
        // Send out the test pattern to the DRAMS.
        //
        CommandAddressPinControl (MrcData, Channel, Rank, EarlyCommandPatternsRise[PatternIndex],
          EarlyCommandPatternsFall[PatternIndex], FALSE
          );
        //
        // For simulation purposes, stall the system 100ps so that the result can be captured.
        //
        MySimStall (100);    
        //
        // Retrieve the result from the pattern capture register.
        //
        MmrcExecuteTask (MrcData, GET_CA_TRAIN, NO_PRINT, Channel);
        MR48Value = (UINT32)MrcData->MspData.DynamicVars[Channel][REG_DATA];
        // 
        // Exit CA Training mode from the DUNIT/PHY and tell the DRAM to exit CA Training mode.
        //
        MmrcExecuteTask (MrcData, CLR_CA_TRAIN, NO_PRINT, Channel);
        CommandAddressPinControl (MrcData, Channel, Rank, MRW42, MRW42, TRUE);
      } else {
        //
        // If the MR41 value failed, then automatically set the MR48 value to a failed value.  In this way
        // testing out the MR48 value, can tell us if the overall system passed.
        //
        MR48Value = 0xffffffff;
      }
      //
      // Test out the MR48 Value to see if the test pattern did pass.
      //
      if (MR48Value == EarlyCommandResults48[PatternIndex]) {
        Pass = TRUE;
      } else {
        Pass = FALSE;
        PatternIndex = NUMBER_EARLY_COMMAND_PATTERNS;
        RetryCount = Retries;
      }
    }  // Retry Count
  }  // Pattern Index
  return Pass;
}
#endif

#if EARLY_COMMAND_TRAINING == 1 && TRAINING_ALGOS == 1
/**
  Test function the is called from the sweep for Early Command Training. 
  It simply calls the CATrainingPass, however this function 
  returns true if pass, where as this function requires 1 for 
  failure. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined. 
  @param[in]       NumberKnobs     Number of knobs.
  @param[in]       NumberElements  Number of elements to test 
                                   (i.e. MAX_STROBES or MAX_BITS, etc).
  @param[in, out]  PassFail        Array of passing or failing result for every element.
  @retval          MMRC_SUCCESS    If all elements passed.
  @retval         !MMRC_SUCCESS    Otherwise.
**/
MMRC_STATUS
EarlyCmdTrainingTest (
  IN  OUT   MMRC_DATA     *MrcData, 
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberKnobs,
  IN        UINT8         NumberElements,
  IN  OUT   UINT8         PassFail[MAX_KNOBS][MAX_STROBES][MAX_BITS][2]
  )
{

  PassFail[0][0][0][CURRENT_SAMPLE] = 1 - (UINT8) CATrainingResultsPass (MrcData, Channel, Rank, NUMBER_EARLY_COMMAND_RETRIES);

  return MMRC_SUCCESS;
}
#endif

#ifndef __GNUC__
#if !defined(SUSSW) || defined(CAR)
void * __cdecl memcpy (void *dst, void *src, size_t cnt)
{
  UINT8 *dst8 = (UINT8 *)dst;
  UINT8 *src8 = (UINT8 *)src;  
  while (cnt--) {
    *dst8++ = *src8++;
  }
  return dst;
}

void * __cdecl memset (void *dst, char value, size_t cnt)
{
  UINT8 *dst8 = (UINT8 *)dst;
  while (cnt--) {
    *dst8++ = value;
  }
  return dst;
}
#endif // SUSSW
#endif // __GNUC__

#ifndef SUSSW
#if USE_64_BIT_VARIABLES
/**
  Performs a shift left on a 64 bit number.
  In non-64-bit modes and default 64-bit mode only bits 0 through 4 of 
  the count are used. This masks the count to a value between 0 and 31. 
  If a count is greater than the operand size, the result is undefined.
  For this reason, we must do a SHL for the upper 32 bits and 
  another for the lower 32 bits. 
           
  @param[in]   CL                 Shift amount (in bits).
  @param[in]   EDX:EAX            64 bit number to shift.
  @param[out]  EDX:EAX            64 bit result of shift left.	
**/
void __cdecl _allshl (void)
{
#if __GNUC__
  asm (
    "cmp  $32, %%cl;"
    "jae  ShiftLowDword;"
    "shld %%cl, %%edx, %%eax;"
    "shl  %%cl, %%eax;"
    "ret;"
"ShiftLowDword:;"
    "mov  %%eax, %%edx;"
    "xor  %%eax, %%eax;"
    "sub  $32, %%cl;"
    "shl  %%cl, %%edx;"
    "ret;"
    : /* no output */
    : /* no input */
    : "%eax", "%edx"
  );
#else
_asm {
  cmp  cl, 32
  jae  ShiftLowDword    ; If shifting more than 31 bits, ignore the high DWORD since it be shifted out.
  shld edx, eax, cl
  shl  eax, cl
  jmp  exitFunction
ShiftLowDword:
  mov  edx, eax
  xor  eax, eax
  sub  cl,  32          ; We already shifted the value by 32 bits by moving EAX into EDX. So just shift what is left over above 32 bits.
  shl  edx, cl
exitFunction:
}
#endif
}
/**
  Performs a shift right on a 64 bit number.
  In non-64-bit modes and default 64-bit mode only bits 0 through 4 of 
  the count are used. This masks the count to a value between 0 and 31. 
  If a count is greater than the operand size, the result is undefined.
  For this reason, we must do a SHR for the upper 32 bits and another for
  the lower 32 bits.
           
  @param[in]   CL                 Shift amount (in bits).
  @param[in]   EDX:EAX            64 bit number to shift.
  @param[out]  EDX:EAX            64 bit result of shift right.
**/
void __cdecl _aullshr (void)
{
#if __GNUC__
  asm (
    "cmp  $32, %%cl;"
    "jae  ShiftHighDword;"
    "shrd %%cl, %%eax, %%edx;"
    "shr  %%cl, %%edx;"
    "ret;"
    "ShiftHighDword:;"
    "mov  %%edx, %%eax;"
    "xor  %%edx, %%edx;"
    "sub  $32, %%cl;"
    "shr  %%cl, %%eax;"
    "ret;"
    : /* no output */
  : /* no input */
  : "%eax", "%edx"
    );
#else
  _asm {
    cmp  cl, 32
    jae  ShiftHighDword   ; If shifting more than 31 bits, ignore the low DWORD since it be shifted out.
    shrd eax, edx, cl
    shr  edx, cl
    jmp  exitFunction
ShiftHighDword:
    mov  eax, edx
    xor  edx, edx
    sub  cl,  32          ; We already shifted the value by 32 bits by moving EDX into EAX. So just shift what is left over above 32 bits.
    shr  eax, cl
exitFunction:
  }
#endif
}
#endif // USE_64_BIT_VARIABLES
#endif // SUSSW

BOOLEAN
IsCmdSignalDelay (  
  IN   MMRC_DATA    *MrcData,
  IN   UINT16       Delay
)
{
  UINT8   i;
  UINT8   TotalCommandGroups;
  UINT16  CommandDelayType;

  CommandDelayType = GetOffsetIndex (MrcData, CmdGrp0);

  TotalCommandGroups = 0;
  for (i = 0; i < MAX_CMDS; i++) {
    TotalCommandGroups += SignalInfo[GetSignalInfoIndex (CommandDelayType + TotalCommandGroups)].FrequencyOffset;
  }
  if ((GetOffsetIndex (MrcData, Delay) >= CommandDelayType && GetOffsetIndex (MrcData, Delay) < (CommandDelayType + TotalCommandGroups)) || 
       Delay == MmrcCmdAll || Delay == CmdAll) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
IsCtlSignalDelay (
  IN   MMRC_DATA    *MrcData,
  IN   UINT16       Delay
)
{
  UINT8   i;
  UINT8   TotalControlGroups;
  UINT16  ControlDelayType;

  ControlDelayType = GetOffsetIndex (MrcData, CtlGrp0);

  TotalControlGroups = 0;
  for (i = 0; i < MAX_CTL; i++) {
    TotalControlGroups += SignalInfo[GetSignalInfoIndex (ControlDelayType + TotalControlGroups)].FrequencyOffset;
  }
  if ((GetOffsetIndex (MrcData, Delay) >= ControlDelayType && GetOffsetIndex (MrcData, Delay) < (ControlDelayType + TotalControlGroups)) ||
       Delay == MmrcCtlAll || Delay == CtlAll) {
    return TRUE;
  }

  return FALSE;
}

#if TRAINING_ALGOS
MMRC_STATUS
PrintFooter1DSweep (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Dim1Index,
  IN   UINT8        Dim1Knobs,
  IN   UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN   BOOLEAN      PerBitSweep
)
{
  UINT8 Strobe;
  UINT8 Knob;
  UINT8 ChannelBackup;
  UINT8 MaximumBits;
  UINT8 Bits;

  ChannelBackup = Channel;

  for (Channel = ChannelBackup; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel (MrcData, Channel, Rank)) {
      continue;
    }
    if (!IsCmdSignalDelay (MrcData, Dim1Index)) {
      MspDebugPrint ((MSP_DBG_MAX, "Ch%02d  ", Channel));
      for (Knob = 0; Knob < Dim1Knobs; Knob++) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) { 
          if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, PerBitSweep, &MaximumBits)) {
            continue;
          }
          for (Bits = 0; Bits < MaximumBits; Bits++) {
            if (Results[Channel][Knob][Strobe][Bits][HIGH] > Results[Channel][Knob][Strobe][Bits][LOW]) {
              MspDebugPrint ((MSP_DBG_MAX, "%04d:%04d:P  ",
                Results[Channel][Knob][Strobe][Bits][LOW],
                Results[Channel][Knob][Strobe][Bits][HIGH]
                ));
            } else {
              MspDebugPrint ((MSP_DBG_MAX, "%04d?%04d:F  ",
                Results[Channel][Knob][Strobe][Bits][LOW],
                Results[Channel][Knob][Strobe][Bits][HIGH]
                ));
            }
          } // Bits loop ...
        } // Strobe loop ...
      }
      MspDebugPrint ((MSP_DBG_MAX, "\n"));
    }
  } // Channel loop ...
  return MMRC_SUCCESS;
}
#endif

#if RMT == 1

MMRC_STATUS
PrintRmtLaneStatus (
  IN   MMRC_DATA  *MrcData,
  IN   UINT8      Channel,
  IN   UINT8      Rank,
  IN   UINT8      Strobe,
  IN   UINT8      MaximumBits,
  IN   BOOLEAN    PerBitSweep,
  IN   UINT8      LaneErrorVector[MAX_STROBES]
  )
{
  INT8    StrobeSweep;
  INT8    BitSweep;
  UINT8   BitMask;

  for (StrobeSweep = (Strobe - 1); StrobeSweep >= 0; StrobeSweep--) {
    if (!IsStrobeValid(MrcData, Channel, Rank, StrobeSweep, PerBitSweep, NULL)) {
      continue;
    }
    if ((StrobeSweep % 4) == 0) {
      MspDebugPrint((MSP_DBG_MIN, "  "));
    }
    for (BitSweep = (MaximumBits - 1); BitSweep >= 0; BitSweep--) {
      BitMask = (UINT8)(1 << BitSweep);
      if ((LaneErrorVector[StrobeSweep] & BitMask) == BitMask) {
        MspDebugPrint((MSP_DBG_MIN, "#"));
      } else {
        MspDebugPrint((MSP_DBG_MIN, "."));
      }
    }
    MspDebugPrint((MSP_DBG_MIN, " "));
  }
  return MMRC_SUCCESS;
}

MMRC_STATUS
PrintRmtSummaryPerBit (
  IN   OUT   MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT8        Strobe[MAX_CHANNELS],
  IN         UINT8        Dim1Knobs,
  IN         UINT16       Dim1Start[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  IN         UINT8        StepSize,
  IN         UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN         BOOLEAN      PerBitSweep
  )
{
  INT8     StrobeSweep;
  INT8     BitSweep;
  UINT16   StrobeMinOffset;
  UINT16   StrobeMaxOffset;
  UINT8    Knob;
  UINT8    MaximumBits;
  UINT16   IndexStrobe;
  UINT8    LaneErrorVector[MAX_STROBES];
  INT8     HighLowFlag;
  UINT8    MsgLevelBackup;

  MsgLevelBackup = MrcData->MspData.DebugMsgLevel;
  MrcData->MspData.DebugMsgLevel |= MSP_DBG_MIN;

  MaximumBits = MrcData->MaxBit[Channel][Rank];

  for (Knob = 0; Knob < Dim1Knobs; Knob++) {
    MspDebugPrint((MSP_DBG_MIN, "Knob[%d] PI Offset Values Pre-RMT: ", Knob));
    for (StrobeSweep = 0; StrobeSweep < Strobe[Channel]; StrobeSweep++) {
      if (!IsStrobeValid(MrcData, Channel, Rank, StrobeSweep, PerBitSweep, NULL)) {
        continue;
      }
      MspDebugPrint((MSP_DBG_MIN, "Strobe[%d] = %03d, ", StrobeSweep, Dim1Start[Channel][Knob][StrobeSweep][0]));
    }
    MspDebugPrint((MSP_DBG_MIN, "\n"));
  }

  for (HighLowFlag = 1; HighLowFlag >= 0; HighLowFlag--) {
    for (Knob = 0; Knob < Dim1Knobs; Knob++) {
      if (HighLowFlag == HIGH) {
        MmrcMemset(LaneErrorVector, 0xFF, MAX_STROBES * sizeof(UINT8));
      } else {
        MmrcMemset(LaneErrorVector, 0, MAX_STROBES * sizeof(UINT8));
      }

      StrobeMinOffset = 0xFF;
      StrobeMaxOffset = 0;
      for (StrobeSweep = 0; StrobeSweep < Strobe[Channel]; StrobeSweep++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, StrobeSweep, PerBitSweep, NULL)) {
          continue;
        }
        for (BitSweep = 0; BitSweep < MaximumBits; BitSweep++) {
          if (StrobeSweep < MAX_STROBES) {
            if (Results[Channel][Knob][StrobeSweep][BitSweep][HighLowFlag] < StrobeMinOffset) {
              StrobeMinOffset = Results[Channel][Knob][StrobeSweep][BitSweep][HighLowFlag];
            }
          	if (Results[Channel][Knob][StrobeSweep][BitSweep][HighLowFlag] > StrobeMaxOffset) {
              StrobeMaxOffset = Results[Channel][Knob][StrobeSweep][BitSweep][HighLowFlag];
            }
          }
        }
      }

      if (HighLowFlag == HIGH) {
        StrobeMaxOffset += 1;
      }

      MspDebugPrint((MSP_DBG_DEBUG, "HighLowFlag = %d, StrobeMinOffset = %d, StrobeMaxOffset = %d\n", HighLowFlag, StrobeMinOffset, StrobeMaxOffset));

      for (IndexStrobe = StrobeMaxOffset; StrobeMinOffset <= IndexStrobe; IndexStrobe -= StepSize) {
        if (HighLowFlag == HIGH) {
          MspDebugPrint((MSP_DBG_MIN, "H%03d 0x", IndexStrobe));
          MmrcMemset(LaneErrorVector, 0xFF, MAX_STROBES * sizeof(UINT8));
        } else {
          MspDebugPrint((MSP_DBG_MIN, "L%03d 0x", IndexStrobe));
          MmrcMemset(LaneErrorVector, 0, MAX_STROBES * sizeof(UINT8));
        }

        for (Knob = 0; Knob < Dim1Knobs; Knob++) {
          for (StrobeSweep = (Strobe[Channel] - 1); StrobeSweep >= 0; StrobeSweep--) {
            if (!IsStrobeValid(MrcData, Channel, Rank, StrobeSweep, PerBitSweep, NULL)) {
              continue;
            }
            for (BitSweep = (MaximumBits - 1); BitSweep >= 0; BitSweep--) {
              if (HighLowFlag == HIGH) {
                if (Results[Channel][Knob][StrobeSweep][BitSweep][HighLowFlag] >= IndexStrobe) {
                  LaneErrorVector[StrobeSweep] &= (UINT8)(~(1 << BitSweep));
                }
              } else {
                if (Results[Channel][Knob][StrobeSweep][BitSweep][HighLowFlag] > IndexStrobe) {
                  LaneErrorVector[StrobeSweep] |= (UINT8)(1 << BitSweep);
                }
              }
            }
            MspDebugPrint((MSP_DBG_MIN, "%02x", LaneErrorVector[StrobeSweep]));
          }
        }

        MspDebugPrint((MSP_DBG_MIN, " "));
        PrintRmtLaneStatus (MrcData, Channel, Rank, Strobe[Channel], MaximumBits, PerBitSweep, LaneErrorVector);
        MspDebugPrint((MSP_DBG_MIN, "\n"));

        if (IndexStrobe == SECURE_OVR_FLOW) {
          break;
        }
      } // IndexStrobe loop
    } // Knobs loop
  } // HighLowFlag loop

  MrcData->MspData.DebugMsgLevel = MsgLevelBackup;
  return MMRC_SUCCESS; 
}
#endif // RMT == 1

/**
  Returns the index into the SignalInfo array which matches the signal index
  into the Elements array.

  @param[in]       Signal         Index into the Elements array.
  @retval                         SignalInfo Index 
**/
UINT16
GetSignalInfoIndex (
  UINT16        Signal
  )
{
  MMRC_DATA *MrcData;
  MrcData = GetMrcHostStructureAddress ();

  if (Signal >= SGTI_INDEX) {
    return Signal - SGTI_INDEX;
  } else if (Signal >= ALGO_REG_INDEX) {
    return Signal - ALGO_REG_INDEX;
  } else if (Signal < (NumberAlgos * NumberOfElementsPerAlgo)) {
    return Signal / NumberOfElementsPerAlgo;
  } else {
    return Signal - (NumberAlgos * NumberOfElementsPerAlgo) + NumberAlgos;
  }
}

/**
  Converts Type

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Type            Register to be accessed, this is pointer to
                                   a register or algorithm in
                                   the Elements[] table.
  @retval          Type
**/
UINT16 
GetOffsetIndex ( 
  IN          MMRC_DATA   *MrcData,
  IN          UINT16       Type
  )
{
  //
  // Check if this type is defined by the GSM_GT or GSM_CSN enum. If it is, we need to convert it to the internal
  // MMRC define for that type.
  //
  if (Type >= GSM_CSN_INDEX) {
    Type = GsmCsnToMmrcDefine[Type - GSM_CSN_INDEX];
    if (Type == MMRC_NA) {
      return DELAY_TYPE_UNDEFINED;
    }
  } else if (Type >= GSM_GT_INDEX) {
    Type = GsmGtToMmrcDefine[Type - GSM_GT_INDEX];
    if (Type == MMRC_NA) {
      return DELAY_TYPE_UNDEFINED;
    }
  }
  if (Type >= REG_INDEX && Type < GROUP_INDEX) {
    Type -= REG_INDEX;
    Type = DelayIndexToOffsetLayer [Type].DelayOffset;
  } else if (Type >= GROUP_INDEX && Type < UNIQUE_INDEX) {
    Type -= GROUP_INDEX;
    Type = GroupIndexToOffsetLayer [Type].DelayOffset;
  } else if (Type >= UNIQUE_INDEX) {
    Type -= UNIQUE_INDEX;
    Type = UniqueIndexToOffsetLayer [Type].DelayOffset;
  }
  return Type;
}

/**
  Finds the blueprint that corresponds to the current 
  configuration. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       IoLevel 
  @param[out]      Blueprint        
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ConvertPfctAndIoLevelToBlueprint (
  IN        MMRC_DATA  *MrcData, 
  IN        UINT8       Channel, 
  IN        UINT8       IoLevel,
     OUT    UINT8      *Blueprint
  )
{
  UINT8 i;
  i = 0;
  //
  // Loop through the table and find the blueprint that corresponds to the current configuration.
  // If the IoLevel is a don't care then just return the blueprint which matches the PFCT. This is
  // useful in systems which have 1 level of memory per channel.
  //
  for (i = 0; i < MAX_IO_LEVELS; i++) {
    if ((((1 << MrcData->MspData.CurrentPlatform[Channel])      & PfctAndIoLevelToBlueprint[i].PMask) != 0) &&
        (((1 << MrcData->MspData.CurrentFrequency[Channel])     & PfctAndIoLevelToBlueprint[i].FMask) != 0) &&
        (((1 << MrcData->MspData.CurrentConfiguration[Channel]) & PfctAndIoLevelToBlueprint[i].CMask) != 0) &&
        (((1 << MrcData->MspData.CurrentTechnology[Channel])       & PfctAndIoLevelToBlueprint[i].TMask) != 0) &&
        ((IoLevel == PfctAndIoLevelToBlueprint[i].IoLevel) || (IoLevel == 0xFF))
       ) 
    {
      *Blueprint = PfctAndIoLevelToBlueprint[i].Blueprint;
      return MMRC_SUCCESS;
    }
  }
  return MrcIoLevelNotSupported;
}

/**
  This function tells if a Channel should be skipped/executed 
  when looping through Channels. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          TRUE            Run in this channel 
  @retval          TRUE            Skip this channel   
**/
BOOLEAN
RunOnThisChannel (
  IN        MMRC_DATA  *MrcData,
  IN        UINT8       Channel,
  IN        UINT8       Rank
  )
{
  MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] = (UINTX) (MrcData->RankEnabled[Channel][Rank] & 
                                                            MrcData->MspData.ExecuteOnThisChannel[Channel] & 
                                                            MrcData->MrcParameters.ChEnabled[Channel]);
  if (MrcData->MspData.DynamicVars[Channel][ENABLE_RUN_TEST] == 0) {
    return FALSE;
  }
  return TRUE;
}

VOID
SetMspData (  
  IN      MSP_DATA         *MspData,
  IN      VOID             *InputStructure
  )
{
  MMRC_DATA   *MrcData;
  UINT8       i;
  
  MrcData = (MMRC_DATA *) InputStructure;
    
  for (i = 0; i < MAX_CHANNELS; i++) {
    FillUpMspDataStructure (
      MspData, 
      i, 
      MrcData->MrcParameters.ChEnabled[i],
      MspData->CurrentPlatform[i], 
      MspData->CurrentFrequency[i], 
      MspData->CurrentTechnology[i], 
      MspData->DebugMsgLevel,                       
      MspData->ExecuteThisRoutineInParallel,
      MspData->CurrentBlueprint,
      MrcData->EcBase
    );
  }
}

#if NEW_RTWT_ALGO == 1
//#if (WRITE_TRAINING == 1 || READ_TRAINING == 1 || COMMAND_TRAINING == 1) && TRAINING_ALGOS == 1
/**
  This function is the center for the Advanced Training Algos.  Currently there are 5 possible elements to loop on:
  Channel, Rank, Knob, Strobe, and Bit.  The first 5 loops on each of these elements and if they are not shared
  uses that index and goes to the next element.  If they are shared, the loop goes to the next element but will not loop
  to the next element such that only those elements that are independent will be looping on...
    For example, RANK_SHARED, BIT_SHARED   Ch[0->MAX_CHANNELS],Rn[0],Knob[0->MAX_KNOBS], Sl[0->MAX_STROBES],Bit[0]
  At the innermost loop (bit), all permutations of the non-shared [CH][RN][KN][SL][BI] will be hit.
  The system will not do another 5 loops, looping through each of these elements again but this time doing the opposite.
  Namely, hitting all the shared items... the outermost loops use IndChn, IndRnk,.. inner uses ShrChn, ShrRnk, etc...
  So at the innermost loop now, each shared elements of the unique elements can be checked.

  A couple of features can be accomplished by this:
    1.  Composite, the system can check the ranges of the passed in data and collect the worst range of all.
    2.  Composite count, the system can check the counts if not max, set to 0....
    3.  Test Function can be called with/without the compositing passing in the unique Ch/Rn/Kn/Sl/Bi.
  The test function removes the requirement to have many functions performing the same looping as required by this function.

  @param[in]  *MrcData        Host structure for all data related to MMRC.
  @param[in]  SharedFlag      Flag specifying how the current index is to be combined.
  @param[in]  NumberElements  For CMD training =1, all other, equal to number of strobes active.
  @param[in]  CompositeFlag   Upon looping on each unique item, Results/Count are composited based on the flag.
  @param[in]  NumberCounts    Number of count array elements, needed for printing, (number eyemasks)
  @param[out] Count           Count of number passing Vrefs for each eyemask.
  @param[out] Results         Single 1-D Sweep from Create1DSweep() function.
  @param[in]  Param           Local parameter being passed to test function call.
  @param[in]  TestFunction    Function to call at the end of all the compositing.
**/
//#pragma optimize("", off)
void 
BuildComposite (
  IN MMRC_DATA *MrcData,
  IN UINT16 SharedFlag,
  IN UINT8 CompositeFlag, 
  IN UINT8 NumberCounts,
  IN UINT8 MaxCount,
  IN UINT8 Count[][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN UINT16 Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN BuildCompositeParams *Param,
  IN MMRC_STATUS TestFunction (MMRC_DATA *, UINT16 *, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, BuildCompositeParams*)
) {
  UINT8  IndChn;
  UINT8  ShrChn;
  UINT8  ShrChnRange[2];
  UINT8  IndRnk;
  UINT8  ShrRnk;
  UINT8  ShrRnkRange[2];
  UINT8  IndKnb;
  UINT8  ShrKnb;
  UINT8  ShrKnbRange[2];
  UINT8  IndStb;
  UINT8  ShrStb;
  UINT8  ShrStbRange[2];
  UINT8  IndBit;
  UINT8  ShrBit;
  UINT8  ShrBitRange[2];
  UINT8  IndTiming;
  UINT8  ShrTiming;
  UINT8  ShrTimingRange[2];

  UINT16 Edge[MAX_EDGES];
  UINT8  Loop;
  UINT8  CountLoop;
  UINT16 CompositeCount[MAX_COMPOSITE_COUNT];
  UINT16 OverwriteFlag; // 0=no overwrite, 1=overwrite
  UINT8  FirstActiveChannelFound;
  UINT8  FirstAvailableRank[MAX_CHANNELS];
  UINT8  MaxBits;
  UINT32 i;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  FirstActiveChannelFound=FALSE;
  MaxBits = 0;

  MmrcMemset(FirstAvailableRank, 0xFF, MAX_CHANNELS * sizeof(UINT8));
  for (IndChn = 0 ; IndChn < MAX_CHANNELS; IndChn++) {
    for (IndRnk = 0 ; IndRnk < MAX_RANKS; IndRnk++) {
      if (RunOnThisChannel (MrcData, IndChn, IndRnk)) {
        if (FirstAvailableRank[IndChn] == 0xFF) {
          FirstAvailableRank[IndChn] = IndRnk;
        }
      }
    }
  }
  //
  // Loop on all active channels and ranks that are independent.
  //
  for (IndChn = 0 ; IndChn < MAX_CHANNELS; IndChn++) {
    if (((SharedFlag & CHANNEL_SHARED) == CHANNEL_SHARED)) {
      ShrChnRange[0] = 0;
      ShrChnRange[1] = MAX_CHANNELS;
    } else {
      ShrChnRange[0] = IndChn;
      ShrChnRange[1] = IndChn + 1;
    }
    for (IndRnk = 0 ; IndRnk < MAX_RANKS; IndRnk++) {
     if (!RunOnThisChannel (MrcData, IndChn, IndRnk)) continue;
      FirstActiveChannelFound = TRUE;
      if (((SharedFlag & RANK_SHARED) == RANK_SHARED)) {
      if (IndRnk > FirstAvailableRank[IndChn] || FirstAvailableRank[IndChn] == 0xFF) { continue; }
        ShrRnkRange[0] = 0;
        ShrRnkRange[1] = MAX_RANKS;
      } else {
        ShrRnkRange[0] = IndRnk;
        ShrRnkRange[1] = IndRnk + 1;
      }
      for (IndTiming = 0; IndTiming < MrcData->NumberOfTiming; IndTiming++) {
        if ((SharedFlag & TIMING_SHARED) == TIMING_SHARED) {
          if (IndTiming > 0) { continue; }
          ShrTimingRange[0] = 0;
          ShrTimingRange[1] = MrcData->NumberOfTiming;
        } else {
          ShrTimingRange[0] = IndTiming;
          ShrTimingRange[1] = IndTiming + 1;
        }
        for (IndKnb = 0 ; IndKnb < MAX_KNOBS; IndKnb++) {
          if (((SharedFlag & KNOB_SHARED) == KNOB_SHARED)) {
          if (IndKnb > 0) { continue; }
            ShrKnbRange[0] = 0;
            ShrKnbRange[1] = MAX_KNOBS;
          } else {
            ShrKnbRange[0] = IndKnb;
            ShrKnbRange[1] = IndKnb + 1;
          }
          for (IndStb = 0 ; IndStb < MrcData->MaxDq[IndChn][IndRnk]; IndStb++) {
            if (!IsStrobeValid(MrcData, IndChn, IndRnk, IndStb, TRUE, &MaxBits)) {
              continue;
            }
            if (MaxBits > MrcData->MaxBit[IndChn][IndRnk]) {
              CheckAdvConstrains (MrcData, "MaxBits > MrcData->MaxBit[IndChn][IndRnk]",(UINT8 *)__FUNCTION__, __LINE__);
            }
            if (((SharedFlag & STROBE_SHARED) == STROBE_SHARED)) {
            if (IndStb > 0) { continue; }
              ShrStbRange[0] = 0;
              ShrStbRange[1] = MrcData->MaxDq[IndChn][IndRnk];
            } else {
              ShrStbRange[0] = IndStb;
              ShrStbRange[1] = IndStb + 1;
            }
            for (IndBit = 0 ; IndBit < MaxBits; IndBit++) {
              if (((SharedFlag & BIT_SHARED) == BIT_SHARED)) {
                if (IndBit > 0) { continue; }
                ShrBitRange[0] = 0;
                ShrBitRange[1] = MaxBits;
              } else {
                ShrBitRange[0] = IndBit;
                ShrBitRange[1] = IndBit + 1;
              }
              //
              // At this point, we have the looping occurring on all independent controls, and the ranges needed for the shared knobs.
              //
              Edge[0]  = 0x0000;
              Edge[1] = 0xffff;
              //
              // Check if there is reason to loop on all SHARED elements.
              //
              if ((CompositeFlag & (COMPOSITE_RESULTS | COMPOSITE_COUNT)) != 0){
                for (CountLoop=0; CountLoop < NumberCounts; CountLoop++) {
              
                  if (CountLoop >= MAX_COMPOSITE_COUNT) {
                    CheckAdvConstrains (MrcData, "Boundary exception (CountLoop >= MAX_COMPOSITE_COUNT)", (UINT8 *)__FUNCTION__, __LINE__);
                  }
                  CompositeCount[CountLoop] = 0xffff; 
                }
                OverwriteFlag = (CompositeFlag & 4)<<2;
                for (Loop=0; Loop < OverwriteFlag+1; Loop++) {
                  for (ShrChn = ShrChnRange[0]; ShrChn < ShrChnRange[1]; ShrChn++) {
                    for (ShrRnk = ShrRnkRange[0]; ShrRnk < ShrRnkRange[1]; ShrRnk++) {
                      if (!RunOnThisChannel (MrcData, ShrChn, ShrRnk)) continue;
                      for (ShrTiming = ShrTimingRange[0]; ShrTiming < ShrTimingRange[1]; ShrTiming++) {
                        for (ShrKnb = ShrKnbRange[0]; ShrKnb < ShrKnbRange[1]; ShrKnb++) {
                          if (ShrKnb >= MAX_KNOBS) {
                            CheckAdvConstrains (MrcData, "ShrKnb > MAX_KNOBS",(UINT8 *)__FUNCTION__, __LINE__);
                          }
                          for (ShrStb = ShrStbRange[0]; ShrStb < ShrStbRange[1]; ShrStb++) {
                            if (ShrStb > MrcData->MaxDq[ShrChn][ShrRnk]) {
                              CheckAdvConstrains (MrcData, "ShrStb > MrcData->MaxDq",(UINT8 *)__FUNCTION__, __LINE__);
                            }
                            if (!IsStrobeValid(MrcData, ShrChn, ShrRnk, ShrStb, FALSE, NULL)) {
                              continue;
                            }
                            for (ShrBit = ShrBitRange[0]; ShrBit < ShrBitRange[1]; ShrBit++) {
                              if (ShrBit > 0) {
                                CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
                              }
                              if (ShrBit > (MAX_BITS - 1)) {
                                CheckAdvConstrains (MrcData, "ShrBit > (MAX_BITS - 1)", (UINT8 *)__FUNCTION__, __LINE__);
                              }
                              //
                              // This is the innermost loop, this area is looped again by another variable called loop.
                              // when loop=0, its the first time, and the system must check for the results or count.
                              // when loop=1, the system will apply those recorded results.  THereby overwriting the initial values.
                              if (Loop == 0) {
                                //
                                // Check if need to composite on Results.
                                //
                                if ((CompositeFlag & COMPOSITE_RESULTS) == COMPOSITE_RESULTS)  {
                                if (Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][LOW] <=  Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][HIGH]) {
                                  if (Edge[0] < Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][LOW]) 
                                    Edge[0] =Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][LOW];
                                  if (Edge[1] > Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][HIGH]) 
                                    Edge[1] = Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][HIGH];
                                }
                                }
                                //
                                // Check if need to composite on Count.
                                //
                                if ((CompositeFlag & COMPOSITE_COUNT) == COMPOSITE_COUNT) {
                                  for (CountLoop=0; CountLoop < NumberCounts; CountLoop++) {
                                    if (CountLoop >= MAX_COMPOSITE_COUNT) {
                                      CheckAdvConstrains (MrcData, "Boundary exception (CountLoop >= MAX_COMPOSITE_COUNT)", (UINT8 *)__FUNCTION__, __LINE__);
                                    }
                                    if (Count[CountLoop][ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit] < CompositeCount[CountLoop]) {
                                      CompositeCount[CountLoop] = Count[CountLoop][ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit];
                                      if ((2+CountLoop) >= MAX_EDGES) {
                                        CheckAdvConstrains (MrcData, "Boundary exception ((2+CountLoop) >= MAX_EDGES)",(UINT8 *)__FUNCTION__, __LINE__);
                                      }
                                      Edge[2+CountLoop] = CompositeCount[CountLoop];
                                    }
                                  }
                                }
                              } else {
                                //
                                // If  overwriting and the COMPOSITE_RESULTS is set to true, then apply the composite
                                // results to each of the shared elements thus overwriting the ranges with the composite.
                                //
                                if ((CompositeFlag & COMPOSITE_RESULTS) == COMPOSITE_RESULTS)  {
                                  Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][LOW] = Edge[0];
                                  Results[ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit][HIGH] = Edge[1];
                                }
                                //
                                // If overwriting and the COMPOSITE_COUNT is set to true, then apply the count to each of the
                                // shared elements.  The principle here is if the composite is less then the passed in maximum count
                                // the clear the count; otherwise, store the count.
                                if ((CompositeFlag & COMPOSITE_COUNT) == COMPOSITE_COUNT) {
                                  for  (CountLoop=0; CountLoop < NumberCounts; CountLoop++) {
                                    if (CountLoop >= MAX_COMPOSITE_COUNT) {
                                      CheckAdvConstrains (MrcData, "Boundary exception (CountLoop >= MAX_COMPOSITE_COUNT)", (UINT8 *)__FUNCTION__, __LINE__);
                                    }
                                    if (CompositeCount[CountLoop] < MaxCount) {
                                      CompositeCount[CountLoop] = 0;
                                    }
                                    Count[CountLoop][ShrTiming][ShrChn][ShrRnk][ShrKnb][ShrStb][ShrBit] = (UINT8) CompositeCount[CountLoop];
                                  }
                                }
                              }  // if loop
                            } // for ShrTiming
                          } // for ShrBit
                        }  // for ShrStb
                      }  // for ShrKnb
                    }  // for ShrRnk
                  }  // for ShrChn
                } // for OverwriteFlag
              } // if CompositeFlag
              //
              // Call the test function passing in the info structure and the low/high data.
              // When CompositeResults, Edge[0][1] are the high/low composites.
              // When CompositeCount, Edge[2] is the results.
              //
              if (TestFunction != (MMRC_STATUS (*)(MMRC_DATA *, UINT16 *, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, BuildCompositeParams*)) -1) {
                MspDebugPrint ((MSP_DBG_TST, "\n"));
                MspDebugPrint ((MSP_DBG_TST, "T%d C%d R%d K%d S%d B%d\n", IndTiming, IndChn, IndRnk, IndKnb, IndStb, IndBit));
                for (i = 0; i < sizeof (Edge) / sizeof (UINT16); i++) {
                  MspDebugPrint ((MSP_DBG_TST, " E[%d] ", i));
                }
                MspDebugPrint ((MSP_DBG_TST, "\n"));
                for (i = 0; i < sizeof (Edge) / sizeof (UINT16); i++) {
                  MspDebugPrint ((MSP_DBG_TST, " %04x ", Edge[i]));
                }
                MspDebugPrint ((MSP_DBG_TST, "\n"));
              
                TestFunction(MrcData, Edge, IndChn, IndRnk, IndKnb, IndStb, IndBit, IndTiming, Param);
              }
            } // for IndTiming
          } // for IndBit
        } // for IndStb
      } // for IndKnb
    } // for IndRnk
  } // for IndChn
}

/**
  The recursive function for the determine passing vrefs that will be called for each control
  knob outside of timing and vref.  Each knob will loop over a range given a starting point and
  will start at the starting point going to the max, then returning to the starting point and going
  to the min.  The control knob also has a step size to reach those endpoints.  The function will 
  run across its knob and for each internal loop will either process the vref/timing or call itself 
  with the next knob until finally the vref/timing can be tested.
  The only method this function can be completed finished is if either all the knobs have hit their
  maximum value or the first eyemask has been satisfied.

  @param[in]  *MrcData        Host structure for all data related to MMRC.
  @param[in]  *EyeMask        Pointer to the various eyemasks.
  @param[in]  ControlKnobs    Control parameters for each dimension.
  @param[in]  ControlIndex    Index into the control knobs for current dimension.
  @param[in]  UnknownLastPass Flag specifying if current sweep can use lastpass results.
  @param[in]  Dim1StartPoint  Length for both strings
  @param[in]  Low             Length for both strings
  @param[in]  High            Length for both strings
  @param[out] ResultsV        Single Eyemask of consecutive sweeps.
  @param[out] ResultsS        Temporary remaining Eyemask sweeps.
  @param[out] Results         Single 1-D Sweep from Create1DSweep() function.
  @param[out] Count           Count of number passing Vrefs for each eyemask.
  @param[out] PowerKnobs      Passing powerknobs for each eyemask except first.
  @param[out] AllDoneFlag     TRUE/FALSE flag specifying if EyeMask0 has reached its completion.
**/
MMRC_STATUS 
DeterminePassingVref1 ( 
  IN  MMRC_DATA   *MrcData, 
  IN  EYEMASK_T    EyeMask[], 
  IN  CONTROLKNOBS ControlKnobs[],  
  IN  UINT8        ControlIndex, 
  IN  UINT8        UnknownLastPass,
  IN  UINT16       Dim1StartPoint[MAX_TIMING][MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  IN  UINT16       Low[MAX_TIMING][MAX_CHANNELS][MAX_STROBES],
  IN  UINT16       High[MAX_TIMING][MAX_CHANNELS][MAX_STROBES],
  OUT UINT16       ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  OUT UINT16       ResultsS[MAX_EYEMASKS-1][MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  OUT UINT16       Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  OUT UINT8        Count[MAX_EYEMASKS][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  OUT UINT8        PowerKnobs[MAX_POWER_KNOBS][MAX_EYEMASKS-1][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  OUT UINT8        *AllDoneFlag
) {
  //
  // Local Variables
  //
  UINT16 ControlValue;
  UINT32 TempValue;
  UINT8  ch,rk,st;
  BuildCompositeParams Param;
  MspDebugPrint ((MSP_DBG_TST, "\n%s ControlIndex = %d Knobs = %d\n", __FUNCTION__, ControlIndex, ControlKnobs[ControlIndex].NumberKnobs));
  //
  // If control's index is set to the timing delay (index=0), then run the sweep; otherwise, 
  // loop through the valid range re-calling the function with the next index.
  //
  if (ControlIndex > (ControlKnobs[0].NumberOfTiming - 1)) {
    //
    // When beginning the loop, it needs to be known that the lastpass values are invalid.
    //
    UnknownLastPass = TRUE;
    //
    // Loop on the Control knobs starting at the initial value, incrementing by STEP, and going to the HIGH point.
    //
    for (ControlValue = ControlKnobs[ControlIndex].StartValue[LOW]; ControlValue <= ControlKnobs[ControlIndex].Ranges[HIGH]; ControlValue += ControlKnobs[ControlIndex].CriteriaStep) {
      //
      // Set the Controls value to the specified value. Set the criteria based on the first eyemask, the remaining eyemasks are used for checking, but
      // controls should be continued until eyemask[0] has met all its requirements.
      //
      Param.SetCriteria2.ControlIndex = ControlIndex;
      Param.SetCriteria2.ControlValue = ControlValue;
      Param.SetCriteria2.ControlKnobs = ControlKnobs;
      Param.SetCriteria2.EyeMask = EyeMask;
      MspDebugPrint ((MSP_DBG_TST, "\n%s ControlValue = %d CriteriaStep = %d\n", __FUNCTION__, ControlValue, ControlKnobs[ControlIndex].CriteriaStep));
      BuildComposite(MrcData, ControlKnobs[ControlIndex].SharedFlags, COMPOSITE_COUNT, 1, (UINT8)EyeMask[0].NumberConsecutiveVoltages, Count, 0, &Param, SetCriteria2);
      //
      // Re-Call this function but for the next control index.
      //
      DeterminePassingVref1(MrcData, EyeMask, ControlKnobs, ControlIndex - 1, UnknownLastPass, Dim1StartPoint, Low, High, ResultsV, ResultsS, Results, Count, PowerKnobs, AllDoneFlag);

      if (ControlIndex == ControlKnobs[0].NumberOfTiming) {
        //
        // Composite the Bit results into Byte if PBD is disabled
        //
        if (ControlKnobs[0].PBSharedFlags == (UINT16)-1) {
          BuildComposite(MrcData, BIT_SHARED, COMPOSITE_RESULTS | OVERWRITE, 2, (UINT8) 1,  0, Results, 0, (MMRC_STATUS (*) (MMRC_DATA *, UINT16 *, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, BuildCompositeParams*))-1);
        }
        //
        // Store all passing eyemasks criteria into the global elements.
        // This will increment count if the result passes the eyemask criteria and the criteria is not already met.
        //
        CheckAndStoreEyeMasks(MrcData, ControlKnobs, Results, ResultsV, ResultsS, &Count[0], PowerKnobs, EyeMask);
        // UINT8 1 is for checking if any of the values are 0, then zero out all of the composites.... 1 says 1,2,3,4.. will stay.
        BuildComposite(MrcData, ControlKnobs[ControlIndex].SharedFlags, COMPOSITE_COUNT | OVERWRITE, 2, (UINT8)1, &Count[0], 0, 0, (MMRC_STATUS(*) (MMRC_DATA *, UINT16 *, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, BuildCompositeParams*)) - 1);
        //
        // print the results
        //
        {
          Param.PrintCount.AllDoneFlag = 1;
          Param.PrintCount.ControlIndex = ControlIndex;
          Param.PrintCount.ControlKnobs = ControlKnobs;
          Param.PrintCount.EyeMaskIndex = 0;
          Param.PrintCount.EyeMask = EyeMask;
          BuildComposite(MrcData, ControlKnobs[0/*ControlIndex*/].SharedFlags, COMPOSITE_RESULTS | COMPOSITE_COUNT, 2, (UINT8) EyeMask[0].NumberConsecutiveVoltages,  Count, Results, &Param, PrintCount);
          *AllDoneFlag = Param.PrintCount.AllDoneFlag;
          if (Param.PrintCount.AllDoneFlag == 1) {
            return MMRC_SUCCESS;
          }
        }
      } else {
        //
        // Build a composite of the count for the purpose of resetting those counts that have not reached the top.
        //
        if (*AllDoneFlag == 1) {
          return MMRC_SUCCESS;
        }
        BuildComposite(MrcData, ControlKnobs[ControlIndex].SharedFlags, COMPOSITE_COUNT | OVERWRITE, 2, (UINT8)EyeMask[0].NumberConsecutiveVoltages, &Count[0], 0, 0, CheckCount);
        *AllDoneFlag = 1;
      }
      MspDebugPrint ((MSP_DBG_MAX, "\n"));
    }
    MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
    MspDebugPrint ((MSP_DBG_TST, "\nControlKnobs[ControlIndex].StartValue[0] - ControlKnobs[ControlIndex].Step[0] = %d\n", ControlKnobs[ControlIndex].StartValue[0] - ControlKnobs[ControlIndex].Step[0]));
    for (ControlValue = ControlKnobs[ControlIndex].StartValue[0] - ControlKnobs[ControlIndex].Step[0]; (INT16) ControlValue > (INT16) ControlKnobs[ControlIndex].Ranges[LOW]; ControlValue -= ControlKnobs[ControlIndex].Step[0]) {
      MspDebugPrint ((MSP_DBG_TST, "\n *-ControlValue = %d ControlIndex = %d\n", ControlValue, ControlIndex));
      //
      // Set the Controls value to the specified value.
      //
      Param.SetCriteria2.ControlIndex = ControlIndex;
      Param.SetCriteria2.ControlValue = ControlValue;
      Param.SetCriteria2.ControlKnobs = ControlKnobs;
      Param.SetCriteria2.EyeMask = EyeMask;

      BuildComposite(MrcData, ControlKnobs[ControlIndex].SharedFlags, COMPOSITE_COUNT, 1, (UINT8)EyeMask[0].NumberConsecutiveVoltages, &Count[0], 0, &Param, SetCriteria2);
      //
      // Re-Call this function but for the next control index.
      //
      DeterminePassingVref1(MrcData, EyeMask, ControlKnobs, ControlIndex - 1, UnknownLastPass, Dim1StartPoint, Low, High, ResultsV, ResultsS, Results, Count, PowerKnobs, AllDoneFlag);
      if (ControlIndex == ControlKnobs[0].NumberOfTiming) {
        //
        // Store all passing eyemasks criteria into the global elements.
        // This will increment count if the result passes the eyemask criteria and the criteria is not already met.
        //
        CheckAndStoreEyeMasks(MrcData, ControlKnobs, Results, ResultsV, ResultsS, Count, PowerKnobs, EyeMask);
      } else {
        //
        // Build a composite of the count for the purpose of resetting those counts that have not reached the top.
        //
        BuildComposite(MrcData, ControlKnobs[ControlIndex].SharedFlags, COMPOSITE_COUNT | OVERWRITE, 1, (UINT8)EyeMask[0].NumberConsecutiveVoltages, &Count[0], 0, 0, CheckCount);
      }
    }
  } else {
    Param.StoreRestore.ControlIndex[0] = ControlKnobs[0].Index;
    Param.StoreRestore.ControlIndex[1] = ControlKnobs[1].Index;
    Param.StoreRestore.NumberTimings = ControlKnobs[0].NumberOfTiming;
    BuildComposite(MrcData, ControlKnobs[ControlIndex - 1].SharedFlags, COMPOSITE_NONE, 1, 0, 0, 0, &Param, StoreDelay);
    if (ControlKnobs[0].NumberOfTiming > 1) {      
      ProgramInitialSettingsFor1DSweep(MrcData, ControlKnobs[ControlIndex-1], Low[ControlIndex-1], High[ControlIndex-1], Dim1StartPoint[ControlIndex-1], 0);
      CompleteSweep(MrcData, ControlKnobs, ControlIndex-1, High[ControlIndex-1], Low[ControlIndex-1], UnknownLastPass, Dim1StartPoint[ControlIndex-1], Results[ControlIndex-1]);
      BuildComposite(MrcData, ControlKnobs[ControlIndex-1].SharedFlags, COMPOSITE_NONE, 1, 0, 0, 0, &Param, RestoreDelay);
    }
    if((ControlKnobs[0].Index == TxDqDelayG) && (ControlKnobs[0].RelAbsFlag == REL)) {
      //
      // When we use relative sweeps we need to retore the training values to compare from the same starting point different sweeps
      //
      for (ch = 0; ch < MAX_CHANNELS; ch++) {
        for (rk = 0; rk < MAX_RANKS; rk++) {
          if (!RunOnThisChannel(MrcData, ch, rk)) continue;
          for (st = 0; st < MAX_STROBES; st++) {
            if (!IsStrobeValid (MrcData, ch, rk, st, FALSE, NULL)) {
              continue;
            }
            GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDelay, CMD_GET_CACHE, &TempValue);
            GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDelay2, CMD_SET_VAL_FC_UC, &TempValue);
            GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDrvDelay2, CMD_SET_VAL_FC_UC, &TempValue);
          }
        }
      }
    }
    ProgramInitialSettingsFor1DSweep(MrcData, ControlKnobs[ControlIndex], Low[ControlIndex], High[ControlIndex], Dim1StartPoint[ControlIndex], 0);
    CompleteSweep(MrcData, ControlKnobs, ControlIndex, High[ControlIndex], Low[ControlIndex], UnknownLastPass, Dim1StartPoint[ControlIndex], Results[ControlIndex]);
    //BuildComposite(MrcData, ControlKnobs[ControlIndex].SharedFlags, COMPOSITE_NONE, 1, 0, 0, 0, &Param, RestoreDelay);
  } // if ControlIndex == 0
  return TRUE;
}

/**
  This function takes the input data and decodes the linear value.  Any number below 0xfa is a linear number
  and the value is simply returned, 0xfa-0xff is a function of the half clock.  The half clock is read on a
  per-channel basis and that is why channel is passed in.  0xff=1x, 0xfe=0x2x, 0xfd=4x, 0xfc=8x, 0xfb=16x, and 0xfa=32x.
  
  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current channel being operated on.
  @param[out]      InputData     Input data from ControlKnobs that may be encoded.
  @retval          Data          Returns valid 16-bit value.
**/
UINT16
ConvertRange (
  IN MMRC_DATA *MrcData,
  IN UINT8 Channel,
  IN UINT16 InputData
) {
  UINT16 Data;
  // ff = 1x
  // fe = 2x
  // fd = 4x
  // fc = 8x
  // fb = 16x
  // fa = 32x
  // all other values are integers.
  switch (InputData) {
    case 0xff:
      // 
      // Full Clock
      //
      Data = 2*GetHalfClk(MrcData, Channel);
      break;
    case 0xfe:
      //
      // 1/2 Clock.
      //
      Data = GetHalfClk(MrcData, Channel);
      break;
    case 0xfd:
      //
      // 1/4 Clock.
      //
      Data = GetHalfClk(MrcData, Channel)/2;
      break;
    case 0xfc:
      //
      // 1/8 Clock.
      //
      Data = GetHalfClk(MrcData, Channel)/4;
      break;
    case 0xfb:
      //
      // 1/16 Clock.
      //
      Data = GetHalfClk(MrcData, Channel)/8;
      break;
    case 0xfa:
      //
      // 1/32 Clock.
      //
     Data = GetHalfClk(MrcData, Channel)/16;
     break;
    default:
      //
      // Return the input data.
      //
      Data = InputData;
  }
  return Data;
}

/**
  When preparing for the 1DSweep, this function takes the information from the
  specific control knob and sets up the initial  Low/High/Startpoint needed
  for passing to the 1DSweep function.  The Low/High are the physical range for
  the 1dsweep where the Dim1StartPoint is the initial position.  The low/high does
  not get changed, but the startpoint will get modified after each 1DSweep to provide
  the lastpass features.
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    External array of controls for the algorithm.
  @param[out]      Low             Low range for 1DSweep control.
  @param[out]      High            High range for 1DSweep control.
  @param[out]      Dim1StartPoint  Starting position for 1DSweep control.
  @param[in]       Index           Specifies how the input parameters are used to fill in the output.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
ProgramInitialSettingsFor1DSweep (
  MMRC_DATA      *MrcData, 
  CONTROLKNOBS   ControlKnobs, 
  UINT16          Low[MAX_CHANNELS][MAX_STROBES],
  UINT16          High[MAX_CHANNELS][MAX_STROBES],
  UINT16          Dim1StartPoint[MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  UINT8           Index
) {
  UINT8  Channel;           // Channel assignement variable.
  UINT8  Rank;              // Rank assignment variable.
  UINT8  Strobe;            // Strobe assignment variable.
  UINT8  Knob;              // Knob assignment variable.
  UINT32 TempValue;         // 32-bit temporary variable used for GetSets.
  UINT16 Ranges[2];         // 16-bit range hi/low variable.
  UINT16 StartValue[2];     // 16-bit start hi/low variables.
  UINT16 Absolute[2][MAX_CHANNELS][MAX_STROBES];

  // 
  // The passed in index parameter, if = 0, states that the range and startvalues within 
  // the control knobs are to be used to specfy the equivalent range and startvalue output; 
  // however, if =1, then the phase2 parameters are used to specify the range and start values.
  //
  MspDebugPrint ((MSP_DBG_TST, "\n%s Index = %d\n", __FUNCTION__, Index));
  MmrcMemset( (UINT8 *)&Absolute[LOW][0][0], 0xff, MAX_CHANNELS * MAX_STROBES * sizeof(UINT16));
  MmrcMemset((UINT8 *)&Absolute[HIGH][0][0], 0x00, MAX_CHANNELS * MAX_STROBES * sizeof(UINT16));

  if (Index < ControlKnobs.NumberOfTiming) {
    Ranges[LOW] = ControlKnobs.Ranges[LOW];
    Ranges[HIGH] = ControlKnobs.Ranges[HIGH];
    StartValue[LOW] = ControlKnobs.StartValue[LOW];
    StartValue[HIGH] = ControlKnobs.StartValue[HIGH];
  } else {
    Ranges[LOW] = ControlKnobs.Ranges[LOW];
    Ranges[HIGH] = ControlKnobs.Ranges[HIGH];
    StartValue[LOW] = ControlKnobs.Phase2VrefStart;
    StartValue[HIGH] = ControlKnobs.Phase2VrefStop;
  }
  //
  // Loop through all active channels and ranks setting the Low/high and startpoints
  // for the tests. 
  //
  for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        // 
        // If the RelAbsFlag is REL, then read the current index value and the ranges low/high are differences 
        // to the read value as are the start low/high values.
        //
        if (ControlKnobs.RelAbsFlag == REL) {
          GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, ControlKnobs.Index, CMD_GET_CACHE, &TempValue);
          if ((INT32)(TempValue - ConvertRange(MrcData, Channel, Ranges[LOW])) < (INT32)GetHalfClk(MrcData, Channel)) {
            Low[Channel][Strobe]  = GetHalfClk(MrcData, Channel);
          } else {
            Low[Channel][Strobe]  = (UINT16) TempValue - (UINT16) ConvertRange(MrcData, Channel, Ranges[LOW]);
          }
          High[Channel][Strobe] = (UINT16) TempValue + (UINT16) ConvertRange(MrcData, Channel, Ranges[HIGH]);
          for (Knob=0; Knob < MAX_KNOBS; Knob++) {
            Dim1StartPoint[Rank][Channel][Knob][Strobe][LOW] = (INT16)TempValue - (INT16)ConvertRange(MrcData, Channel, StartValue[LOW]);
            Dim1StartPoint[Rank][Channel][Knob][Strobe][HIGH]= (INT16)TempValue + (INT16)ConvertRange(MrcData, Channel, StartValue[HIGH]);
          } // For kn
          if (Low[Channel][Strobe] < Absolute[LOW][Channel][Strobe]) {
            Absolute[LOW][Channel][Strobe] = Low[Channel][Strobe];
          }
          if (High[Channel][Strobe] > Absolute[HIGH][Channel][Strobe]) {
            Absolute[HIGH][Channel][Strobe] = High[Channel][Strobe];
          }
        } else {
          //
          // If the RelAbsFlag is ABS, then the ranges low/high and start low/high are absolute settings and they are
          // just assigned the specified values.
          //
          Low[Channel][Strobe]                       = (UINT16) ConvertRange(MrcData, Channel, Ranges[LOW]);
          High[Channel][Strobe]                      = (UINT16) ConvertRange(MrcData, Channel, Ranges[HIGH]);
          for (Knob=0; Knob < MAX_KNOBS; Knob++) {
            Dim1StartPoint[Rank][Channel][Knob][Strobe][LOW] = (INT16)ConvertRange(MrcData, Channel, StartValue[LOW]);
            Dim1StartPoint[Rank][Channel][Knob][Strobe][HIGH]= (INT16)ConvertRange(MrcData, Channel, StartValue[HIGH]);
          }  // for Knob
        } // If ControlKnobs
        MspDebugPrint ((MSP_DBG_TST, "\nDim1StartPoint[%d][%d][0][%d][0] = %03d", Rank, Channel, Strobe, Dim1StartPoint[Rank][Channel][0][Strobe][LOW]));
        MspDebugPrint ((MSP_DBG_TST, "\nDim1StartPoint[%d][%d][0][%d][1] = %03d", Rank, Channel, Strobe, Dim1StartPoint[Rank][Channel][0][Strobe][HIGH]));
      } // for Strobe          
    } // for Rank
  } // for Channel
  for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        //
        // Spread the Min/Max Value per Strobe Across all Ranks if REL
        //
        if (ControlKnobs.RelAbsFlag == REL) {
          Low[Channel][Strobe]  = Absolute[LOW][Channel][Strobe];
          High[Channel][Strobe] = Absolute[HIGH][Channel][Strobe];
        }
        MspDebugPrint ((MSP_DBG_TST, "\nLow [%d][%d] = %03d", Channel, Strobe, Low[Channel][Strobe]));
        MspDebugPrint ((MSP_DBG_TST, "\nHigh[%d][%d] = %03d", Channel, Strobe, High[Channel][Strobe]));
      }
      //
      // We do not to execute for all ranks
      //
      break;
    }
  }


  return MMRC_SUCCESS;
}
/**
  This is the top function for the DeterminePassingVref step.  This function declares the variables that are needed throughout
  the lifespan of this step as well as initializes the variables needed.  The largest seconday array is the ResultsVTemp structure
  which holds the passing vrefs for the non-primary eye masks.  Rather then making this in the top function, it is put here because
  at the time this step is completed, the only results needed is the passed in ResultsV which has all the necessary data.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       ControlKnobs  Control parameters for each dimension.
  @param[in]       ControlIndex  Index of current control knob.
  @param[out]      ResultsV      Single Eyemask of consecutive sweeps.
  @param[in]       EyeMask       Eyemasks used for this algorithm.
  @param[in]       NumberEyeMask Total number of eyemasks.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
DeterminePassingVref2 ( 
  IN MMRC_DATA *MrcData, 
  IN CONTROLKNOBS ControlKnobs[],  
  IN UINT8 ControlIndex, 
  IN OUT UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN EYEMASK_T *EyeMask,
  IN UINT8 NumberEyeMask
)
{
  //
  // Local Variables.
  //
  UINT16   ResultsVTemp[MAX_EYEMASKS-1][MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];// Temporary Storage locations for remaining Eyemasks.
  UINT16   Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];                                 // Location of current results to be stored.
  UINT8    Count[MAX_EYEMASKS][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];                        // Current consecutive count stored in results.
  UINT8    PowerKnobs[MAX_POWER_KNOBS][MAX_EYEMASKS-1][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];// Power knob settings for passing eyemasks.
  UINT16   Low[MAX_TIMING][MAX_CHANNELS][MAX_STROBES];                                                                        // Lowest value allowed on the sweep used in recursive function.
  UINT16   High[MAX_TIMING][MAX_CHANNELS][MAX_STROBES];                                                                       // Highest value allowed on the sweep used in recursive function.
  INT16    Dim1StartPoint[MAX_TIMING][MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];                                    // Starting point on where to begin the sweep.
  UINT8    AllDoneFlag;                                                                                           // Flag stating if all tests have passed.
  UINT8    loop;                                                                                                  // Internal looping variable.
  UINT8    MaxElements;
  BuildCompositeParams Param;                                                                                     // Parameters for the LoopOnComposite function.
  UINT8  NumberOfTiming;
  UINT8  VrefIndex;                                                                                    
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  if (ControlKnobs[0].NumberOfTiming > 1) {
    NumberOfTiming = 2;
    VrefIndex = 2;
  } else {
    NumberOfTiming = 1;
    VrefIndex = 1;
  }
  //
  // The upper and lower limits of the sweep gets done one time at the beginning of the algo, and does not get 
  // modified throughout the entire algorithm. These are the Low/High ranges and are used in the 1DSweep.
  //
  for (loop = 0; loop< NumberOfTiming; loop++) {
    ProgramInitialSettingsFor1DSweep(MrcData, ControlKnobs[loop], Low[loop], High[loop], Dim1StartPoint[loop],0);
  }
  // 
  // When starting the determination, the consecutive counts for each test should be set to 0.
  //
  MmrcMemset(Count, 0, MAX_EYEMASKS*MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS);
  MmrcMemset(ResultsVTemp, 0, (MAX_EYEMASKS - 1)*MAX_CONSEC * MAX_TIMING * MAX_CHANNELS * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 2 * 2);
  MmrcMemset(Results, 0, MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS*2);
  MmrcMemset(PowerKnobs, 0, MAX_POWER_KNOBS*(MAX_EYEMASKS - 1)*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS);

  AllDoneFlag = 1;
  //
  // Print out the header
  //
  {
    UINT8 i;
    UINT8 i2;
    UINT8 count;
    MspDebugPrint ((MSP_DBG_TST, "\nGetMaxElements(MrcData) = %d ControlIndex = %d", GetMaxElements(MrcData), ControlIndex));
    MspDebugPrint ((MSP_DBG_MAX, "\nCRK  "));
    count = 3;
    if ((ControlKnobs[0].SharedFlags & STROBE_SHARED) == STROBE_SHARED) {
    } else {
      MaxElements = GetMaxElements(MrcData);
      for (i2=0; i2 < MaxElements; i2++) {
         for (i = ControlIndex; i > 0 ; i--) {
           MspDebugPrint ((MSP_DBG_MAX, "%s:", ControlKnobs[i].Label));
           count+=3;
         }
         MspDebugPrint ((MSP_DBG_MAX, "HGH:LOW:EM  "));
         count+=12;
      }
    }
    MspDebugPrint ((MSP_DBG_MAX, "\n"));
    while (count-- > 0) {
      MspDebugPrint ((MSP_DBG_MAX, "="));
    }
    MspDebugPrint ((MSP_DBG_MAX, "\n"));
  }
  //
  // Begin calling the recursive function that will perform the Late Training.  Note that the results for the 
  // eyemasks are stored in two different arrays, ResultsV and ResultsS.  ResultsS is a temporary storage 
  // for the N-1 eyemasks and is removed at the completion of this function.  The ResultsV is passed in and gets
  // the actual data for the accepted EyeMask..  This is not removed and is returned to the calling function.
  //
  DeterminePassingVref1(MrcData, EyeMask, ControlKnobs, ControlIndex, 0, Dim1StartPoint, Low, High, ResultsV, ResultsVTemp, Results, Count, PowerKnobs, &AllDoneFlag);
  //
  // For each granularity of the power knob, set the desired value.
  //
  for (loop = NumberOfTiming+1; loop < ControlKnobs[0].NumberKnobs; loop++) {
    Param.GetSet.ControlKnobs = ControlKnobs;
    Param.GetSet.ControlIndex = loop;
    Param.GetSet.PowerKnobs = PowerKnobs;
    Param.GetSet.EyeMask = EyeMask;
    BuildComposite(MrcData, ControlKnobs[loop].SharedFlags | KNOB_SHARED, COMPOSITE_COUNT, 3, 0, Count, 0, &Param, GetSet);
  }
  //
  // Copy the correct results data based on the count data to the ResultsV. This is the only data returned from this function
  //
  Param.CopyResults.ResultsV = ResultsV;
  Param.CopyResults.ResultsS = ResultsVTemp;
  Param.CopyResults.Count = Count;
  Param.CopyResults.MaxCount = (UINT8) EyeMask[0].NumberConsecutiveVoltages;
  BuildComposite(MrcData, NO_SHARED, 0, 0, 0, 0, 0, &Param, CopyResults);
  //
  // Return with success.
  //
  return MMRC_SUCCESS;
}

MMRC_STATUS
CheckCount (MMRC_DATA *MrcData, UINT16 *Range, UINT8 Ch, UINT8 Rn, UINT8 Kn, UINT8 St, UINT8 Bi, UINT8 Timing, BuildCompositeParams *Param) {
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function. At the end of the Determine Passing Vrefs,
  there are two arrays ResultsS and ResultsV.  ResultsV has one EyeMask of data, and ResultsS has the 
  remaining.  The ResultsS will be removed.  ResultsV has only EyeMask[0]'s responses, but before leaving
  those eyemasks that are passing the test needed to be copied to ResultsV.  Thereby, at the end the resultsV
  could have different EyeMasks passing different elements.  The system will check if eyemask0 passed this 
  instance, just return, otherwise find the passing one and copy it from ResultsS to ResultsV.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CopyResults (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  UINT8 index;
  UINT8 index2;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  // 
  // If the current count has already met the requirements, do not copy it over, just return as we are done.
  //
  if (Param->CopyResults.Count[0][Timing][Channel][Rank][Knob][Strobe][Bit] == Param->CopyResults.MaxCount) return MMRC_SUCCESS;
  //
  // Loop through each of the secondary eye masks, and if they pass their input copy them over. But if they have
  // also reached there max count simply exit out.
  //
  for (index = 1; index < RT_NUM_EYEMASKS; index++) {
    if (Param->CopyResults.Count[index][Timing][Channel][Rank][Knob][Strobe][Bit] == Param->CopyResults.MaxCount) {
      //
      // Copy all of the Vrefs/Pis (MaxCount) from the temporary to the final results.
      for (index2=0; index2<Param->CopyResults.MaxCount; index2++) {
        Param->CopyResults.ResultsV[index2][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] = Param->CopyResults.ResultsS[index-1][index2][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH];
        Param->CopyResults.ResultsV[index2][Timing][Channel][Rank][Knob][Strobe][Bit][LOW] = Param->CopyResults.ResultsS[index-1][index2][Timing][Channel][Rank][Knob][Strobe][Bit][LOW];
      }
      return MMRC_SUCCESS;
    }
  }
  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function. This function upon completion of the vref
  passing criteria search, restores the power knobs to the setting that brought about the passing.
  These values are printed out at the end of the DetermineVref() function.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetSet (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  UINT32 value;
  UINT8 index;
  UINT8 *Label;
  UINT8 PowerKnob;
  UINT8 NumberKnobs;
  UINT8 EyeMaskMet;
  UINT8 BitLoop;
  UINT8 bit;

  EyeMaskMet = 0;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  value = 0;
  //
  // Extract the parameters from the Param flags.
  //
  Label = Param->GetSet.ControlKnobs[Param->GetSet.ControlIndex].Label;
  NumberKnobs = Param->GetSet.ControlKnobs[Param->GetSet.ControlIndex].NumberKnobs;
  //
  // New line on each bit/strobes = 0.  followed by the channel, rank, knob printout.
  //
  if (Bit==0 && Strobe==0) {
    MspDebugPrint ((MSP_DBG_MAX, "\n%d%d%d  ", Channel, Rank, Knob));
  }
  //
  // Loop through each EyeMask, determining which eyemask passed the criteria. Always start on the first.
  //
  for (index = 0; index < RT_NUM_EYEMASKS; index++) {
    if (Range[2+index] == Param->GetSet.EyeMask[0].NumberConsecutiveVoltages) {
      EyeMaskMet = 1;
      //
      // Once a passing criteria has been met, restore the power knobs to their recorded value.
      // Note that the first knob does not require new settings as the test would have stopped immediately
      // upon passing.
      //
      for (PowerKnob=NumberKnobs-1; PowerKnob >= 2; PowerKnob--) {
        //
        // Print out the power knob value.
        //
        if (index == 0) {
          GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, Param->PrintCount.ControlKnobs[PowerKnob].Index, CMD_GET_REG, &value); //Change to CMD_GET_CACHE
        } else {
          value = Param->GetSet.PowerKnobs[PowerKnob-2][index-1][Channel][Rank][Knob][Strobe][Bit];
        }
        MspDebugPrint ((MSP_DBG_MAX, "%02d:", value));
        if ((Param->PrintCount.ControlKnobs[PowerKnob].SharedFlags & (BIT_SHARED|BIT_CONTROL)) == (BIT_SHARED | BIT_CONTROL)) {
          BitLoop=MAX_BITS;
        } else {
          BitLoop = Bit + 1;
        }
        for (bit = Bit; bit < BitLoop; bit++) {
          GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, bit, MrcData->DVFSFreqIndex, 0xFF, Param->PrintCount.ControlKnobs[PowerKnob].Index, CMD_SET_VAL_FC_UC, &value); //Change to CMD_GET_CACHE
        }
      }
      //
      // On the power knob restoration, give a standard output that denotes this is a restoration.
      //
        MspDebugPrint ((MSP_DBG_MAX, "..:...:...:..  ", value));
      break;
    }
  }
  if (EyeMaskMet == 0) {
    MspDebugPrint ((MSP_DBG_MIN, "\n\n<> Halting!! <> ERROR, no minimum Eyemask criteria was met.!\n\n"));
    return MMRC_FAILURE; //while(1);
  }

  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function. When doing the vref search for the eyemask 
  that can pass the tests, this is the function that prints out the response.  However, this function 
  should not be commented out as it also is used for determining if the eyemask criteria has search.
  This may be moved to another task, but then another looping function will be needed.  The format 
  of the output is CRK, Ranges for each element and followed by the EM counts.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PrintCount (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  UINT8 index;
  UINT32 value;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // Print out the channel rank knob header only if the strobe and bit indexes are set to 0.
  //
  if (Strobe == 0 && Bit == 0) {
    MspDebugPrint ((MSP_DBG_MAX, "\n%d%d%d  ", Channel, Rank, Knob));
  }
  //
  // Loop through each of the knobs except the first one (>0) and print out the value.  The first one being the timing.
  //
  for (index = Param->PrintCount.ControlKnobs[0].NumberKnobs-1; index > 0; index--) {
    if (index < 0 || index >= Param->PrintCount.ControlKnobs[0].NumberKnobs) {
      CheckAdvConstrains (MrcData, "index < 0 || index >= ControlKnobs", (UINT8 *)__FUNCTION__, __LINE__);
    }
    if (Param->PrintCount.ControlKnobs[0].NumberOfTiming > 1 ) {
      GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, 
        Param->PrintCount.ControlKnobs[Param->PrintCount.ControlKnobs[0].NumberOfTiming].Index, CMD_GET_REG, &value); 
    } else {
      GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, 
        Param->PrintCount.ControlKnobs[index].Index, CMD_GET_REG, &value); 
    }
    MspDebugPrint ((MSP_DBG_MAX, "%02d:", value));
  }
  // 
  // Print out the range for this entry.
  //
  if (Range[0] == 0 && Range[1] == 0xFFFF) {
    MspDebugPrint ((MSP_DBG_MAX, "xxx:xxx:", Range[0], Range[1]));
  } else {
    MspDebugPrint ((MSP_DBG_MAX, "%03d:%03d:", Range[0], Range[1]));
  }
  //
  // While printing the data, if it is determined that the count is set to the eyemask criteria
  // then leave the alldoneflag alone, otherwise set to 0. It is set to 1 on each interation,
  // and by the end if it remains 1, then the system is done and the first eyemask has met the
  // criteria.
  if (Range[2] != Param->PrintCount.EyeMask[0].NumberConsecutiveVoltages) {
    Param->PrintCount.AllDoneFlag = 0;
  }
  // 
  // In order print out the EM counts side-by-side.
  //
  for (index=0; index < 2; index++) {
    MspDebugPrint ((MSP_DBG_MAX, "%d", Range[2+index]));
  }
  MspDebugPrint ((MSP_DBG_MAX, "  "));
  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function. This function applies the Set to the specific control.
  The function first tests if the knob needs to be set, and the only reason to not set it is if the first eyemask's
  criteria has been met.  If it has, then exit without setting the knob.  If it needs to still be set, then apply
  the value either.  The only "note" is that this is where the BIT_CONTROL fields is used such that if a register
  has bit-level controls but is not being used at that level, then BIT_CONTROL should be set to 1 and the function
  will set all the bits for the strobe to the same value.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SetCriteria2 (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  EYEMASK_T *EyeMask;
  CONTROLKNOBS *ControlKnobs;
  UINT8 BitLoop;
  UINT8 i;
  UINT8 MaxBit;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  EyeMask = Param->SetCriteria2.EyeMask;
  ControlKnobs = Param->SetCriteria2.ControlKnobs;
  //
  // The only reason to not set the specific value is if the first eyemask has reached its criteria for 
  // this unique combination. If the other eyemasks have met their requirements, we still need to continue
  // searching for the first to pass.
  //
  if (EyeMask[0].NumberConsecutiveVoltages == Range[2]) {
    //
    // This element has reached the requirements, simply return.
    //
    return MMRC_SUCCESS;
  }

  //
  // Set the element to the specific value.
  //
  if ((ControlKnobs[Param->SetCriteria2.ControlIndex].SharedFlags & (BIT_SHARED|BIT_CONTROL)) == (BIT_SHARED | BIT_CONTROL)) {
    IsStrobeValid (MrcData, Channel, Rank, Strobe, TRUE, &MaxBit);
    BitLoop = MaxBit;
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
  } else {
    BitLoop = 1;
  }
  // 
  // If the shared flag has BIT_CONTROL set to 1, then the getset needs to set each of the bits for this index.
  // otherwise, one single write is all that is needed.  Currently CTLE has bit-control but it is only being used
  // at the strobe level, so to keep all bits equal, this knob needs BIT_CONTROL set.
  //
  for (i=0; i < BitLoop; i++) {
    GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit+i, MrcData->DVFSFreqIndex, 0xFF, ControlKnobs[Param->SetCriteria2.ControlIndex].Index, 
       CMD_SET_VAL_FC_UC, &Param->SetCriteria2.ControlValue); 
  }
  return MMRC_SUCCESS;
}

/**
  This function takes the data from Results, and compares the data to the passing criteria of the various
  eyemasks.  If the data point passes the eyemask, the data element is copied to the Results[S/V] array and the 
  count is incremented.  The first EyeMask goes into the ResultsV array, the remaining EyeMasks go into ResultsS
  so EM0->ResultsV, EM1->ResultsS[0], EM2->ResultsS[1],etc.  Count[EM0], Count[EM1], Etc. When the eyemask criteria
  is met, the PoweKnobs array is updated; this is needed for all but the first eyemask, since if the eyemask 0 passes,
  the test stops, but all others continue to search.  If the first never passes, the system will look for the prior 
  passing cases and restore those power knobs.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    Control parameters for each dimension.
  @param[in]       Results         Single 1-D Sweep from Create1DSweep() function.
  @param[in]       ResultsV        Single Eyemask of consecutive sweeps.
  @param[in]       ResultsS        Temporary remaining Eyemask sweeps.
  @param[in]       Count           Count of number passing Vrefs for each eyemask.
  @param[in]       PowerKnobs      Passing powerknobs for each eyemask except first.
  @param[in]       EyeMask         Eyemasks used for this algorithm.
  @retval          MMRC_SUCCESS    Return value.
**/
MMRC_STATUS
CheckAndStoreEyeMasks (
  IN MMRC_DATA *MrcData,
  IN CONTROLKNOBS *ControlKnobs,
  IN UINT16 Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
  IN UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN UINT16 ResultsS[MAX_EYEMASKS-1][MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
  IN UINT8 Count[MAX_EYEMASKS][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN UINT8 PowerKnobs[MAX_POWER_KNOBS][MAX_EYEMASKS-1][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN EYEMASK_T *EyeMask
) {
  UINT8 Channel;
  UINT8 Rank;
  UINT8 Knob;
  UINT8 Strobe;
  UINT8 Bit;
  UINT8 Timing;
  UINT8 EMIndex;
  UINT8 Index;
  UINT8 PowerIndex;
  UINT32 Value;

  //
  // Loop through all channel, ranks, knobs, strobes, bits testing the eyemasks.
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
      for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          for (Bit = 0; Bit < MAX_BITS; Bit++) {
            for (Timing = 0; Timing < ControlKnobs[0].NumberOfTiming; Timing++) {
              //
              // Loop through each EyeMask.
              for (EMIndex = 0; EMIndex < MAX_EYEMASKS; EMIndex++) {
                Index = Count[EMIndex][Timing][Channel][Rank][Knob][Strobe][Bit];
                //
                // Only test the current EyeMask if it has not already hit the passing criteria.
                //
                if (Index != EyeMask[EMIndex].NumberConsecutiveVoltages) {
                //
                //  Unsigned int, so make sure the High>low before subtracting - MP
                //
                if ((Results[Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] > Results[Timing][Channel][Rank][Knob][Strobe][Bit][LOW]) &&
                  ((Results[Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] - Results[Timing][Channel][Rank][Knob][Strobe][Bit][LOW]) > EyeMask[EMIndex].MinimumDelayWidth)) {
                    //
                    // If the EM passed, copy if the first into ResultsV, otherwise copy into ResultsS[EM-1] and increment
                    // the appropriate count.
                    //
                    if (EMIndex==0) {
                      ResultsV[Index][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] = Results[Timing][Channel][Rank][Knob][Strobe][Bit][HIGH];
                      ResultsV[Index][Timing][Channel][Rank][Knob][Strobe][Bit][LOW]  = Results[Timing][Channel][Rank][Knob][Strobe][Bit][LOW];
                    } else {
                     ResultsS[EMIndex-1][Index][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] = Results[Timing][Channel][Rank][Knob][Strobe][Bit][HIGH];
                     ResultsS[EMIndex-1][Index][Timing][Channel][Rank][Knob][Strobe][Bit][LOW]  = Results[Timing][Channel][Rank][Knob][Strobe][Bit][LOW];
                    }
                    Count[EMIndex][Timing][Channel][Rank][Knob][Strobe][Bit]++;
                    //
                    // Loop through the power knobs, storing their positions to be retrieved if needed later.
                    //
                    for (PowerIndex=0; PowerIndex < ControlKnobs[0].NumberKnobs-2; PowerIndex++) {
                      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, ControlKnobs[PowerIndex+2].Index+Knob, CMD_GET_CACHE, &Value);
                      if (EMIndex != 0) {
                        PowerKnobs[PowerIndex][EMIndex-1][Channel][Rank][Knob][Strobe][Bit] = (UINT8) Value;
                      }
                    }
                  } else {
                    // 
                    // If the EyeMask failed, reset the count to 0, as the number of consecutive passes must be restarted.
                    //
                    Count[EMIndex][Timing][Channel][Rank][Knob][Strobe][Bit] = 0;
                  }
                }   
              } // for EMIndex
            } // for Timing
          } // for Bit
        } // for Strobe
      } // for Knob
    } // for Rank
  } // for Channel

  return MMRC_SUCCESS;
}


VOID
ReAdjust2xRestriction (
   IN  MMRC_DATA *MrcData,
   IN  CONTROLKNOBS *ControlKnobs,
   IN  UINT16 Dim2Control
)
{
  UINT8         Channel;
  UINT8         Rank;
  UINT8         Knob;
  UINT8         Strobe;
  UINT32        OrigValue;
  UINT32        CacheValue;
  UINT8         NumOfRanks;
  UINT32        SumAverage;
  BOOLEAN       MissMatched2x[MAX_KNOBS][MAX_STROBES];

  for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
    MmrcMemset(MissMatched2x, FALSE, MAX_KNOBS * MAX_STROBES * sizeof(BOOLEAN));
    NumOfRanks  = 0;
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
      NumOfRanks++;
      for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
        for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
          if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) continue;
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, ControlKnobs[Dim2Control].Index + Knob, CMD_GET_REG, (UINT32 *)&OrigValue);
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, ControlKnobs[Dim2Control].Index + Knob, CMD_GET_CACHE, (UINT32 *)&CacheValue);
          if (OrigValue != CacheValue) {
            MissMatched2x[Knob][Strobe] = TRUE;
            MspDebugPrint ((MSP_DBG_MAX, "R%dS%d Warning: Org = %d Cache = %d Delta (C-O) = %d\n", Rank, Strobe, OrigValue, CacheValue, CacheValue - OrigValue));
          } // OrigValue != CacheValue
        } // Strobe loop ...
      } // Knob loop ...
    } // Rank loop ...
    for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        SumAverage  = 0;
        if (!MissMatched2x[Knob][Strobe]) continue;
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
          if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) continue;
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, ControlKnobs[Dim2Control].Index + Knob, CMD_GET_CACHE, (UINT32 *)&CacheValue);
          SumAverage += CacheValue;
        } // Rank loop ...

        if (NumOfRanks > 0) {
          SumAverage = SumAverage / NumOfRanks;
		    } else {
          MspDebugPrint((MSP_DBG_MIN, "ERROR! improper value for NumOfRanks.\n"));
          MrcDeadLoop();
        }

        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
          if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) continue;
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, ControlKnobs[Dim2Control].Index + Knob, CMD_GET_CACHE, (UINT32 *)&CacheValue);
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, ControlKnobs[Dim2Control].Index + Knob, CMD_SET_VAL_FC_UC, (UINT32 *)&SumAverage);
          MspDebugPrint ((MSP_DBG_MAX, "R%dS%d Cache = %d Average = %d Delta (A - C) = %d\n", Rank, Strobe, CacheValue, SumAverage, SumAverage - CacheValue));
        } // Rank loop ...
      } // Strobe loop ...
    } // Knob loop ...
  } // Channel loop ...

}

MMRC_STATUS
RestoreDim1Values (
 IN     MMRC_DATA     *MrcData,
 IN     CONTROLKNOBS  *ControlKnobs,
 IN     UINT16        Dim1Control,
 IN     UINT8         Dim2Loop,
 IN OUT UINT16        RestoreResultsPN[MAX_TIMING][MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES]
)
{
  UINT8   Timing;
  UINT8   Channel;
  UINT8   Rank;
  UINT8   Knob;
  UINT8   Strobe;
  UINT32  TempValue;

  TempValue = 0;

  for (Timing=0; Timing < ControlKnobs[0].NumberOfTiming ; Timing++) {
    for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
        for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
          for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
            if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) continue;
            if (ControlKnobs[Dim1Control].Index == TxDqDelayG && ControlKnobs[Dim1Control].RelAbsFlag == REL) {
              GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, TxDqDelay, CMD_GET_CACHE, &TempValue);
              GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, TxDqDelay2, CMD_SET_VAL_FC_UC, &TempValue);
              GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, TxDqDrvDelay2, CMD_SET_VAL_FC_UC, &TempValue);
            } else if (ControlKnobs[Dim1Control].RelAbsFlag == MMRC_ABS && ControlKnobs[0].NumberOfTiming > 1 &&
                      (ControlKnobs[Dim1Control + Timing].Index == RxDqsPDelay || ControlKnobs[Dim1Control + Timing].Index == RxDqsNDelay)) {
              if (Dim2Loop == 0) {
                //
                // Get the value calculated from Phase 1&2 for all timing
                //
                GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, ControlKnobs[Dim1Control + Timing].Index, CMD_GET_REG, &TempValue);
                RestoreResultsPN[Timing][Rank][Channel][Knob][Strobe] = (UINT16)TempValue;
              }
              TempValue = RestoreResultsPN[Timing][Rank][Channel][Knob][Strobe];
              GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, ControlKnobs[Dim1Control + Timing].Index, CMD_SET_VAL_FC_UC, &TempValue);
            }
          } // Strobe loop ...
        } // Knob loop ...
      } // Rank loop ...
    } // Channel loop ...
  } // Timing

  return MMRC_SUCCESS;
}

/**
  This function is used twice in the advanced algo function.  When sweeping the Vref N 
  times at specific delays to determine the Vref center and when sweeping the Delays
  N times at specific vrefs to determine the Delay center. The dim1 and dim2 controls are
  the index's into the ControlKnob that are related to the sweep dimensions.  The overrides
  are to provide the overrides to the entries int he controlKnob table; for example, when 
  doing the sweep of vrefs, the delays will need to be knob shared.. but the actual knob is 
  not shared, so the override will have knob_shared.  The final results from the sweep are 
  placed in NewResults.  The original number of samples are needed to determine the maximum
  eyewidth of the current data which is about to be overwritten.  This maximum value is needed
  for the center calculation which is the max EyeHeight for the upcoming sweep.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ResultsV        Selected series of data to calculate center.
  @param[in]       OrigNumberSamples   Number of the dim2 samples.
  @param[in]       ControlKnobs    Control parameters for each dimension.
  @param[in]       Dim1Control     Index for the X-dim controls.
  @param[in]       Dim2Control     Index for the Y-dim controls.
  @param[in]       Dim1Override    Overrides for dim1 during this sweep.
  @param[in]       Dim2Override    Overrides for dim2 during this sweep.
  @param[out]      NewResultsV     Output results for this sweep.
  @param[in]       NewNumberSamples The actual number of samples for this sweep.
  @param[out]      Max             Maximum Width from current data, prior to new sweep.
  @retval          MMRC_SUCCESS    Return value.
**/
MMRC_STATUS
SweepData1 (
   IN  MMRC_DATA *MrcData,
   IN  UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
   IN  UINT16 OrigNumberSamples,
   IN  CONTROLKNOBS *ControlKnobs,
   IN  UINT16 Dim1Control,
   IN  UINT16 Dim2Control,
   IN  UINT8 Dim1Override,
   IN  UINT8 Dim2Override,
   OUT UINT16 NewResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
   OUT UINT16 NewNumberSamples,
   IN  UINT16 Max[3][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
   IN  UINT8  UsePreviousMax
)
{
  UINT16        Low[MAX_TIMING][MAX_CHANNELS][MAX_STROBES];
  UINT16        High[MAX_TIMING][MAX_CHANNELS][MAX_STROBES];
  UINT16        Dim1StartPoint[MAX_TIMING][MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
  UINT16        RestoreResultsPN[MAX_TIMING][MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES];
  UINT16        Min[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT16        Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  BuildCompositeParams  Param;
  UINT8         Loop;
  UINT8         Dim2Loop;
  UINT8         Channel;
  UINT8         Rank;
  UINT8         Knob;
  UINT8         Strobe;
  UINT16        SharedFlag;
  UINT8         MaxElements;
  UINT8         MaxBit;
  UINT8         Timing;
  UINT8         Dim2TimingLoop;
  UINT8         Dim1TimingLoop;

  if (MrcData->NumberOfTiming > 1) {
    //if sweeping vref, timing loop is 1
    if (Dim2Control == ControlKnobs[0].NumberOfTiming) {
      Dim2TimingLoop = 1;
      Dim1TimingLoop = 2;
    } else {
      Dim1TimingLoop = 1;
      Dim2TimingLoop = 2;
    }
  } else {
      Dim1TimingLoop = 1;
      Dim2TimingLoop = 1;
  }
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  if (UsePreviousMax == FALSE) {
    //
    // Initialize the min and max arrays to 0 and 0xff.
    //
    MmrcMemset(Min, 0xff, MAX_TIMING * MAX_CHANNELS * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 2);
    MmrcMemset(Max, 0, MAX_TIMING * MAX_CHANNELS * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 2 * 3);
    //
    // Find the Maximum EW of the original data which is swept across the swapped accesses.  Therefore it should be at a function
    // of the dim2 flags. This maximum is needed for the calculation of the center.
    //
    Param.MinMax.Min = Min;
    Param.MinMax.Max = Max[Dim2Control];
    for (Loop = 0; Loop < OrigNumberSamples; Loop++){
      SharedFlag = ControlKnobs[Dim2Control].SharedFlags;
      if (ControlKnobs[Dim2Control].PBIndex != (UINT16)-1) {
        SharedFlag &= ~BIT_SHARED;
      }
      SharedFlag |= Dim2Override;
      BuildComposite(MrcData, SharedFlag, COMPOSITE_RESULTS, 1, 0, 0, ResultsV[Loop], &Param, FindMaxMinEW);
    }
    SharedFlag = ControlKnobs[Dim2Control].SharedFlags |KNOB_SHARED;
    if ((Dim2Control < ControlKnobs[0].NumberOfTiming) && (MrcData->NumberOfTiming > 1)) {

      Param.MinMax.Min             = Min;
      Param.MinMax.Max             = Max[Dim2Control];
      BuildComposite(MrcData, SharedFlag|TIMING_SHARED,COMPOSITE_NONE, 1, 0, 0, ResultsV[Dim2Control], &Param, FindCommonMaxMinEW);
    }

    //
    // Must find the maximum again, but at the granularity of the dim1, this is required for the determining of the
    // delay steps needed for performing the second sweep.  This is because the granularity of the dim2 (vref) cannot be
    // used when setting the dim1 steps.
    Param.MinMax.Max = Max[Dim1Control];
    for (Loop = 0; Loop < OrigNumberSamples; Loop++){
      SharedFlag = ControlKnobs[Dim1Control].SharedFlags;
      if (ControlKnobs[Dim1Control].PBIndex != (UINT16)-1) {
        SharedFlag &= ~BIT_SHARED;
      }
      SharedFlag |= Dim2Override;
      BuildComposite(MrcData, SharedFlag, COMPOSITE_RESULTS, 1, 0, 0, ResultsV[Loop], &Param, FindMaxMinEW);
    }
  }
  //
  // Now sweep the vrefs for each pi.  This will be the data used to calculat the center vref.  The original data in the 
  // ResultsV can be overridden as it is no longer needed.
  //
  // Re-Initialize the parameters of the 1DSweep to run across the VRefs.
  //
  for (Loop = 0; Loop < Dim1TimingLoop; Loop++) {
    ProgramInitialSettingsFor1DSweep(MrcData, ControlKnobs[Dim1Control], Low[Loop], High[Loop],
      Dim1StartPoint[Loop], (UINT8) (Dim1Control+Loop));
  }

  MmrcMemset(RestoreResultsPN, 0, MAX_TIMING * MAX_RANKS * MAX_CHANNELS * MAX_KNOBS * MAX_STROBES * sizeof(UINT16));
  //
  // Print out the header.
  //
  {
    UINT8 i;
    UINT8 i2;
    UINT8 count;

    count = 4;
    MspDebugPrint ((MSP_DBG_MAX, "\nCRKH"));
    MaxElements = GetMaxElements(MrcData);
    (MaxElements > 9) ? (MaxBit = 4) : (MaxBit = GetMaxBits (MrcData));
    for (i=0; i< MaxElements; i++) {
      MspDebugPrint((MSP_DBG_MAX, "  SL%d", i));
      count+= 5;
      for (i2 = 0; i2 < MaxBit; i2++) {
        MspDebugPrint((MSP_DBG_MAX, ":B%d", i2));
        count+= 3;
      }
    }

    MspDebugPrint((MSP_DBG_MAX, "\n"));
    while (count > 0) {
      MspDebugPrint((MSP_DBG_MAX, "="));
      count--;
    }
    MspDebugPrint((MSP_DBG_MAX, "\n"));

  }
  //
  // Loop throuch each PI sampling needed to get the data required for centering the vref.  The placement of the pi
  // is relative, so the initial value must be set correctly, and it will adjust accordingly.
  //
  for (Dim2Loop = 0; Dim2Loop < NewNumberSamples; Dim2Loop++) {
    // 
    // Set the PI Delay.
    //
    Param.ReAdjust.Max         = Max[Dim2Control];
    Param.ReAdjust.Flags       = ControlKnobs[Dim2Control].SharedFlags | Dim2Override;
    Param.ReAdjust.Index[0]    = ControlKnobs[Dim2Control].Index;
    Param.ReAdjust.Loop        = Dim2Loop;
    Param.ReAdjust.LoopTotal   = NewNumberSamples;

    SharedFlag = ControlKnobs[Dim2Control].SharedFlags |KNOB_SHARED;
    if ((Dim2Control == ControlKnobs[0].NumberOfTiming) && (MrcData->NumberOfTiming > 1)) {
      SharedFlag |= TIMING_SHARED;
    } else {
      if ((Dim2Control < MrcData->NumberOfTiming) && (MrcData->NumberOfTiming > 1)) {
        Param.ReAdjust.Index[1]    = ControlKnobs[Dim2Control+1].Index;
      }
    }

    BuildComposite(MrcData, SharedFlag, COMPOSITE_NONE, 1, 0, 0, ResultsV[Dim2Loop], &Param, ReAdjust);

    if (Param.ReAdjust.Flags & HW_RESTRICTION_2X) {
      ReAdjust2xRestriction(MrcData, ControlKnobs, Dim2Control);
    } // Param.ReAdjust.Flags & HW_RESTRICTION_2X

    RestoreDim1Values(MrcData, ControlKnobs, Dim1Control, Dim2Loop, RestoreResultsPN);
    //
    // Perform the sweep.
    //

    Timing = 0;
    ProgramInitialSettingsFor1DSweep(MrcData, ControlKnobs[Dim1Control], Low[Timing], High[Timing],Dim1StartPoint[Timing], (UINT8)Dim1Control);

    CompleteSweep(MrcData, ControlKnobs, Dim1Control, High[Timing], Low[Timing], 1, Dim1StartPoint[Timing], Results[Timing]);
    if ((Dim2Control == ControlKnobs[0].NumberOfTiming) && (MrcData->NumberOfTiming > 1)) {
      RestoreDim1Values(MrcData, ControlKnobs, Dim1Control, 1, RestoreResultsPN);
      ProgramInitialSettingsFor1DSweep(MrcData, ControlKnobs[Dim1Control+1], Low[Dim1Control+1], High[Dim1Control+1], Dim1StartPoint[Dim1Control+1], (UINT8) (Dim1Control+1));
      CompleteSweep(MrcData, ControlKnobs, (Dim1Control+1), High[Dim1Control+1], Low[Dim1Control+1], 1, Dim1StartPoint[Dim1Control+1], Results[Dim1Control+1]);
    }
    //
    // Copy the results to the vref results array.
    //
    for (Loop = 0; Loop < ControlKnobs[0].NumberOfTiming; Loop++) {
    for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (!RunOnThisChannel (MrcData, Channel, Rank)) continue;
        memcpy(ResultsV[Dim2Loop][Loop][Channel][Rank], Results[Loop][Channel][Rank], MAX_KNOBS * MAX_STROBES * MAX_BITS * 2 * 2);
        //
        // Support byte granularity signal with x4 configuration
        //
        SharedFlag = ControlKnobs[Dim1Control].SharedFlags;
        if (((SharedFlag & STROBE_GROUP_BYTE) == STROBE_GROUP_BYTE) && (MrcData->MaxDq[Channel][Rank] == MAX_STROBE_X4_DEVICES)) {
          for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
            for (Strobe = 0; Strobe < 9; Strobe++) {
              memcpy(ResultsV[Dim2Loop][Loop][Channel][Rank][Knob][Getx4FloorPlan(Strobe)], Results[Loop][Channel][Rank][Knob][Strobe], MAX_BITS * 2 * 2);
            }
          }
        }
      } // for Rn
    } // for Ch
    } //for loop
    //
    // Display the output of the sweep.
    //
    MspDebugPrint ((MSP_DBG_MAX, "\n 1nd timing:   \n "));
    Param.PrintHiLow.ControlKnobs = ControlKnobs;
    Param.PrintHiLow.ControlIndex = 0;//Dim1Control;
    Param.PrintHiLow.ResultsV = ResultsV;
    Param.PrintHiLow.Dim2Index = (UINT8) Dim2Control;
    Param.PrintHiLow.Dim2Loop = Dim2Loop;
    PrintHiLow(MrcData, &Param);
    
    if ((MrcData->NumberOfTiming > 1) && (Dim2Control == MrcData->NumberOfTiming)) {
      MspDebugPrint ((MSP_DBG_MIN, "\n 2nd Timing: \n"));
      Param.PrintHiLow.ControlIndex = 1;
      PrintHiLow(MrcData, &Param);
    }

  } // for Dim2Loop

  return MMRC_SUCCESS;
}

/**
  Based on the dim2 center position, this function calculates the center x position
  by taking weights around the center such that as the sweep gets further away from
  the dim2 center, the values have less an impact on the calculations.  This function
  all needs to know the dim2 total range which is passed in.. This was determined by
  using the previous data and calculating the maximum eye width.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ResultsV        Selected series of data to calculate center.
  @param[in]       Flags           Shared flags of data.
  @param[in]       NumberSamples   Number of the dim2 samples.
  @param[out]      PICenter        Center results at the granularity of Flags.
  @param[in]       Max             Maximum eye height from previous sweep.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CalculateCenter2 (
   IN MMRC_DATA  *MrcData,
   IN UINT16      ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
   IN UINT16      Flags,
   IN UINT8       NumberSamples,
   OUT UINT16     PiCenter[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
   IN  UINT16     Max[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS]
) {
  UINT8         VrefLoop;
  BuildCompositeParams  Param;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // Overwrite the results with the composite data at this time.  This is needed int he final 
  // calculations as it will be looping through all the vrefs in parallel.  
  //
  for (VrefLoop = 0; VrefLoop < NumberSamples; VrefLoop++){
    BuildComposite(MrcData, Flags, COMPOSITE_RESULTS|OVERWRITE, 1, 0, 0, ResultsV[VrefLoop], 0,
      (MMRC_STATUS (*) (MMRC_DATA *, UINT16 *, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, BuildCompositeParams*)) -1);
  }

  //
  // Do the final calculations, rather then looping here, passing in the number samples and resultsV arrays 
  // allows the looping to be done inside of the LoopOnComposite function
  //
  Param.FinalCalculateCenter2.NumberSamples = NumberSamples;
  Param.FinalCalculateCenter2.ResultsV = ResultsV;
  Param.FinalCalculateCenter2.Max = Max;
  Param.FinalCalculateCenter2.Center = PiCenter;
  Param.FinalCalculateCenter2.Flags = Flags;

  BuildComposite(MrcData, Flags, COMPOSITE_NONE, 1, 0, 0, 
    0, &Param, FinalCalculateCenter2);

  return MMRC_SUCCESS;
}

/**
  One of two methods to calculate the center for a series of results.  This
  method assumes we do not know anything about the center of the y posisiton, it
  calculates the center based on weights only.  If the flags are perbit, it will
  calculate the per-bit center, if the flags are per-strobelane.  This method is 
  used to estimate the center position when doing advanced training since a center 
  y position is not known.  

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ResultsV        Selected series of data to calculate center.
  @param[in]       Flags           Shared flags of data.
  @param[in]       SelectedEyeMask Used to provide the number of vref samples.
  @param[out]      PICenter        Center results at the granularity of Flags.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CalculateCenter1 (
   IN MMRC_DATA  *MrcData, 
   IN UINT16     ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
   IN UINT16     Flags,
   IN EYEMASK_T  *EyeMask,
   IN UINT8      SelectedEyeMask, 
   IN UINT16     PICenter[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS]
) {
  UINT8         VrefLoop;
  UINT16        Min[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT16        Max[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT8         AccumWeights[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  BuildCompositeParams  Param;
  UINT8         Timing;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // Initialize all arrays.  
  //
  MmrcMemset(Min, 0xff, MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS*2);
  MmrcMemset(Max, 0, MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS*2);
  MmrcMemset(AccumWeights, 0, MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS);
  MmrcMemset(PICenter, 0, MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS*2);

  //
  // Loop through each of the dim2 values and determine the maximum and minimimum eyewidths.
  //
  for (Timing = 0; Timing < MAX_TIMING; Timing++) {
    Param.MinMax.Max             = Max;
    Param.MinMax.Min             = Min;
  }
  
  for (VrefLoop = 0; VrefLoop < EyeMask[SelectedEyeMask].NumberConsecutiveVoltages; VrefLoop++)
  {
    BuildComposite(MrcData, Flags, COMPOSITE_RESULTS, 1, 
      (UINT8) EyeMask[0].NumberConsecutiveVoltages,  0, ResultsV[VrefLoop], &Param, FindMaxMinEW);
  }

  // 
  // Loop again though each of the dim2 values but this time get the center.  This takes the 
  // min and max values previously determined as an input and determins the picenter.  The 
  // AccumWeights array is a temporary location that is needed as each vref is stepped through.
  //
  Param.MinMaxPICenter.Max      = Max;
  Param.MinMaxPICenter.Min      = Min;
  Param.MinMaxPICenter.PICenter = PICenter;
  Param.MinMaxPICenter.AccumWeights    = AccumWeights;

  for (VrefLoop = 0; VrefLoop < EyeMask[SelectedEyeMask].NumberConsecutiveVoltages; VrefLoop++)
  {
    // 
    // On all but the first vrefs, set the final flag to 0, otherwise set it to 1.
    // This flag is used to tell the centering algorithm to now divide the total by the 
    // accumulated weights.
    //
    if (VrefLoop == EyeMask[SelectedEyeMask].NumberConsecutiveVoltages-1) {
      Param.MinMaxPICenter.Final[0] = 1;
      Param.MinMaxPICenter.Final[1] = 1;
    } else {
      Param.MinMaxPICenter.Final[0] = 0;
      Param.MinMaxPICenter.Final[1] = 0;
    }
    BuildComposite(MrcData, Flags, COMPOSITE_RESULTS, 1, 
      (UINT8) EyeMask[0].NumberConsecutiveVoltages, 0, ResultsV[VrefLoop], &Param, ApplyMaxMinEWWeight);
  }
  
  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function. This is the most generic print function.
  It simply prints out the Channel, rank, and strobe if the strobe and bit index's are set to 0 but also
  prints out the low and high ranges seperated by a ":" for all entries.  The end result is to see :
        000 020:055 010:022 012:054 ... 

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GenericPrint2 (
  IN MMRC_DATA    *MrcData, 
  IN UINT16       *Range, 
  IN UINT8         Channel, 
  IN UINT8         Rank, 
  IN UINT8         Knob, 
  IN UINT8         Strobe, 
  IN UINT8         Bit, 
  IN UINT8         Timing,
  IN BuildCompositeParams *Param
) {
 if (Strobe == 0 && Bit == 0) {
    MspDebugPrint ((MSP_DBG_MIN, "\n%d%d%d ", Channel, Rank, Knob));
  }
 MspDebugPrint ((MSP_DBG_MIN, "%03d:%03d ", Range[LOW], Range[HIGH]));
 return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function.  When calculating the center based on the center position,
  the only additional data required is the maximum Eye Height.  This is passed in on the param structures in Max.
  The function will loop though each sweep giving the larger weight to the center y and less and less weight as the sweep
  sample gets further away from the center.  This goes until NumberSamples is reached.  
  The final center position is then divided by the sum weights.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FinalCalculateCenter2 (
  IN MMRC_DATA *MrcData, 
  IN UINT16 *Range, 
  IN UINT8 Channel, 
  IN UINT8 Rank, 
  IN UINT8 Knob, 
  IN UINT8 Strobe, 
  IN UINT8 Bit, 
  IN  UINT8 Timing,
  IN BuildCompositeParams *Param
) {
  UINT16  NumberSamples;
  UINT32  EyeWidthAtCenter;
  UINT32  SumLR;
  UINT32  SumWeights;
  UINT8   Loop;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // Determine the number of samples.
  //
  NumberSamples = Param->FinalCalculateCenter2.NumberSamples;

  if (NumberSamples == 1) {
	  Param->FinalCalculateCenter2.Center[Timing][Channel][Rank][Knob][Strobe][Bit] = 
		  (Param->FinalCalculateCenter2.ResultsV[0][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] +
		  Param->FinalCalculateCenter2.ResultsV[0][Timing][Channel][Rank][Knob][Strobe][Bit][LOW])/2;
	  return MMRC_SUCCESS;
  }
  //
  // Determine the eye width at the center Y. Give Number Samples, this is the NumberSamples/2 position.
  //
  EyeWidthAtCenter = Param->FinalCalculateCenter2.ResultsV[NumberSamples/2][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] - 
                     Param->FinalCalculateCenter2.ResultsV[NumberSamples/2][Timing][Channel][Rank][Knob][Strobe][Bit][LOW];
  //
  // Initialize the SumLR and SumWeights to 0.
  //
  SumLR = 0;
  SumWeights = 0;
  //
  // Loop from center going up, so the number of loops is sample/2 accumulating the SumLR and SumWeights.
  // the SumLR uses the value at the +y and -y... this is why there are only sample/2 y's.  
  //
  for (Loop = 0; Loop < (NumberSamples)/2; Loop++) {
    SumLR += ((((NumberSamples) / 2 - Loop) * (UINT16)Param->FinalCalculateCenter2.Max[Timing][Channel][Rank][Knob][Strobe][Bit] + Loop * EyeWidthAtCenter) *
      ((UINT16)Param->FinalCalculateCenter2.ResultsV[NumberSamples / 2 + Loop][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] + (UINT16)Param->FinalCalculateCenter2.ResultsV[NumberSamples / 2 - Loop][Timing][Channel][Rank][Knob][Strobe][Bit][HIGH] +
      (UINT16)Param->FinalCalculateCenter2.ResultsV[NumberSamples / 2 - Loop][Timing][Channel][Rank][Knob][Strobe][Bit][LOW] + (UINT16)Param->FinalCalculateCenter2.ResultsV[NumberSamples / 2 + Loop][Timing][Channel][Rank][Knob][Strobe][Bit][LOW]
    ));
    SumWeights += 2*( ((NumberSamples)/2-Loop) * (UINT16)Param->FinalCalculateCenter2.Max[Timing][Channel][Rank][Knob][Strobe][Bit] + (Loop) * EyeWidthAtCenter );
  } // for Loop
  //
  // If the sumWeights > 0 (0 cannot be dividied), then divide the center by the SumWeights; otherwise, set the center to 0.
  //
  if (SumWeights != 0) {
    Param->FinalCalculateCenter2.Center[Timing][Channel][Rank][Knob][Strobe][Bit] = (UINT16)((SumLR / SumWeights) / 2);
  } else {
    Param->FinalCalculateCenter2.Center[Timing][Channel][Rank][Knob][Strobe][Bit] = (UINT16) 0;
  }

 return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function.  All ranges are compared with each
  other and the maximum and minimum values are determined and stored into the max and min array 
  elements.  This is to find the max/min at different dim2 components, not at the flags level.
  Prior to this funtion being called the maximum value must be set to 0, and the minimum value must
  be set to all 0xffff.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FindMaxMinEW (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Ch, 
  IN  UINT8 Rn, 
  IN  UINT8 Kn, 
  IN  UINT8 St, 
  IN  UINT8 Bi, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  INT16  Width;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  // 
  // Determine the width of the eye.
  //
  Width = Range[HIGH] - Range[LOW]; 
  if (Range[LOW] > Range[HIGH]) {
	  Param->MinMax.Max[Timing][Ch][Rn][Kn][St][Bi] = 0;
	  return MMRC_SUCCESS;
  }
  //
  // If the stored max is smaller then the new width, then overwrite with the new width.
  //
  if (Param->MinMax.Max[Timing][Ch][Rn][Kn][St][Bi] < Width && Width > 0){
    Param->MinMax.Max[Timing][Ch][Rn][Kn][St][Bi] = Width;
  }
  //
  // If the stored min is smalled then the new width, then overwrite with the new width.
  //
  if (Param->MinMax.Min[Timing][Ch][Rn][Kn][St][Bi] > Width){
    Param->MinMax.Min[Timing][Ch][Rn][Kn][St][Bi] = Width;
  } 

  return MMRC_SUCCESS;
}

MMRC_STATUS
FindCommonMaxMinEW (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Ch, 
  IN  UINT8 Rn, 
  IN  UINT8 Kn, 
  IN  UINT8 St, 
  IN  UINT8 Bi, 
  IN  UINT8 Tim,
  IN  BuildCompositeParams *Param
) {

  //
  // If the stored max is smaller then the new width, then overwrite with the new width.
  //
  if (Param->MinMax.Max[Ch][0][Rn][Kn][St][Bi] < Param->MinMax.Max[Ch][1][Rn][Kn][St][Bi]){
    Param->MinMax.Max[Ch][0][Rn][Kn][St][Bi] = Param->MinMax.Max[Ch][1][Rn][Kn][St][Bi];
  } else {
    Param->MinMax.Max[Ch][1][Rn][Kn][St][Bi] = Param->MinMax.Max[Ch][0][Rn][Kn][St][Bi];
  }
  //
  // If the stored min is smalled then the new width, then overwrite with the new width.
  //
  if (Param->MinMax.Min[Ch][0][Rn][Kn][St][Bi] > Param->MinMax.Min[Ch][1][Rn][Kn][St][Bi] ){
    Param->MinMax.Min[Ch][0][Rn][Kn][St][Bi] = Param->MinMax.Min[Ch][1][Rn][Kn][St][Bi] ;
  } else {
    Param->MinMax.Min[Ch][1][Rn][Kn][St][Bi] = Param->MinMax.Min[Ch][0][Rn][Kn][St][Bi] ;
  } 

  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function.  When calculating the center position 
  using Maximum/Medium/Minimum widths method, this function is used to do the summation.  It is assumed
  that the maximum and minimum values are determined prior and setup in the Max and Min array elements.
  The function will look at the current value, compare it with the min/max values and multiply the 
  average eyewidth by the min/mid/max weights. At the end, the sum of the individual min/mid/max weights 
  that were multiplied are then divided.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ApplyMaxMinEWWeight (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Ch, 
  IN  UINT8 Rn, 
  IN  UINT8 Kn, 
  IN  UINT8 St, 
  IN  UINT8 Bi, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  INT16 Width;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // Check if the eyewidth is one of the maximium values.
  //
  Width = Range[HIGH] - Range[LOW];
  if ((Range[HIGH] - Range[LOW]) == Param->MinMaxPICenter.Max[Timing][Ch][Rn][Kn][St][Bi]) {
    // 
    // Multiple the center position (x+y/2) by the max weight and accumulate the max weight.
    //
    Param->MinMaxPICenter.AccumWeights[Timing][Ch][Rn][Kn][St][Bi] += (UINT8) (MAX_WEIGHT);
    Param->MinMaxPICenter.PICenter[Timing][Ch][Rn][Kn][St][Bi] += ((Range[HIGH] + Range[LOW]) * MAX_WEIGHT) / ( 2);
  //
  // Check if the eyewidth is one of the minimum values.
  //
  } else if ((Range[HIGH] - Range[LOW]) == Param->MinMaxPICenter.Min[Timing][Ch][Rn][Kn][St][Bi]) {
    // 
    // Multiple the center position (x+y/2) by the min weight and accumulate the min weight.
    //
    Param->MinMaxPICenter.AccumWeights[Timing][Ch][Rn][Kn][St][Bi] +=MIN_WEIGHT;
    Param->MinMaxPICenter.PICenter[Timing][Ch][Rn][Kn][St][Bi] += ((Range[HIGH] + Range[LOW]) * MIN_WEIGHT) / ( 2);
  //
  // If this point is reached, it must have been a middle/medimum value.
  //
  } else if (Width > 0) {
    // 
    // Multiple the center position (x+y/2) by the med weight and accumulate the med weight.
    //
    Param->MinMaxPICenter.AccumWeights[Timing][Ch][Rn][Kn][St][Bi] +=MID_WEIGHT;
    Param->MinMaxPICenter.PICenter[Timing][Ch][Rn][Kn][St][Bi] += ((Range[HIGH] + Range[LOW]) * MID_WEIGHT) / ( 2);
  }
  // 
  // If on the last Vref, (the final flag will not be set)  then divide the total accumulation by the sum of the weights.
  //
  if (Param->MinMaxPICenter.Final[Timing] == 1) {
    Param->MinMaxPICenter.PICenter[Timing][Ch][Rn][Kn][St][Bi] /= Param->MinMaxPICenter.AccumWeights[Timing][Ch][Rn][Kn][St][Bi];
  }
  return MMRC_SUCCESS;
}

MMRC_STATUS
SetPBDLinear(
IN  MMRC_DATA *MrcData,
IN  UINT16 Value,
IN  UINT16 PBIndex,
IN  UINT16 PBFlags,
IN  UINT8  AllBitsFlag,
IN  UINT8  Done[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS])
{
  BuildCompositeParams  Param;

  Param.ProgramSetGetLinear.PI = Value;
  Param.ProgramSetGetLinear.PBIndex = PBIndex;
  Param.ProgramSetGetLinear.AllBits = AllBitsFlag;
  Param.ProgramSetGetLinear.Done = Done;
  //
  // PBD is always per Nibble basis
  //
  DnvNibbleTrainingHook(MrcData, 0 , 0 ,0);
  BuildComposite(MrcData, PBFlags, COMPOSITE_NONE, 1, 0, 0, 0, &Param, ProgramSetGetLinear);
  if (PBFlags & NIBBLE_PBD_X4X8_DLY) {
    //
    // Restore the original number of DQ's
    // Only when DLY needs to be changed per Nibble or Byte
    //
    DnvNibbleTrainingExitHook(MrcData, 0, 0, 0);
  }
  return MMRC_SUCCESS;
}

MMRC_STATUS
ProgramSetGetLinear(
  IN  MMRC_DATA *MrcData,
  IN  UINT16 *Range,
  IN  UINT8 Channel,
  IN  UINT8 Rank,
  IN  UINT8 Knob,
  IN  UINT8 Strobe,
  IN  UINT8 Bit,
  IN  UINT8 Timing,
IN  BuildCompositeParams *Param
)
{
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  if (Param->ProgramSetGetLinear.AllBits == FALSE) {
    if (Param->ProgramSetGetLinear.Done[Channel][Rank][Knob][Strobe][Bit] == TRUE)
      return MMRC_SUCCESS;
    GetSetDdrioGroup(MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, Param->ProgramSetGetLinear.PBIndex, CMD_SET_VAL_FC_UC, &Param->ProgramSetGetLinear.PI);
  }

  if (Param->ProgramSetGetLinear.AllBits == TRUE) {
    GetSetDdrioGroup(MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, Param->ProgramSetGetLinear.PBIndex, CMD_SET_VAL_FC_UC, &Param->ProgramSetGetLinear.PI);
  }

  return MMRC_SUCCESS;
}

MMRC_STATUS
TestNonLinearPBForDone(
  IN  MMRC_DATA *MrcData,
  IN  UINT16 *Range,
  IN  UINT8 Channel,
  IN  UINT8 Rank,
  IN  UINT8 Knob,
  IN  UINT8 Strobe,
  IN  UINT8 Bit,
  IN  UINT8 Timing, 
  IN  BuildCompositeParams *Param
)
{
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  if (Param->ProgramSetGet.Done[Timing][Channel][Rank][Knob][Strobe][Bit] == FALSE) {
    //
    //  for DNV Advanced writes use PBDirect flag as FALSE and Writes the opposite
    //
    if (Param->ProgramSetGet.ControlKnobs[0].PBDirectFlag == FALSE) {
      if ((Range[1] - Param->ProgramSetGet.Lo[Timing][Channel][Rank][Knob][Strobe][Bit] >= (Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe] - Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit])) &&
        (Range[1] > Param->ProgramSetGet.Lo[Timing][Channel][Rank][Knob][Strobe][Bit])) {
        Param->ProgramSetGet.Done[Timing][Channel][Rank][Knob][Strobe][Bit] = TRUE;
      } else {
        Param->ProgramSetGet.AllDone = FALSE;
      }
    } else {
      //
      // PBDirect flag as TRUE
      //
      if ((Param->ProgramSetGet.Lo[Timing][Channel][Rank][Knob][Strobe][Bit] - Range[0] >= (Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit] - Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe])) &&
        (Range[0] < Param->ProgramSetGet.Lo[Timing][Channel][Rank][Knob][Strobe][Bit])) {
        Param->ProgramSetGet.Done[Timing][Channel][Rank][Knob][Strobe][Bit] = TRUE;
      } else {
        Param->ProgramSetGet.AllDone = FALSE;
      }
    }
  }
  return MMRC_SUCCESS;
}

/**

  For an x8 devices all the Data will be across 8 Strobes and 8 bits,
  but as the PBD requires data is Nibble mode, so we need to split the full data.
  St[0].B[7:0] --> {St[9].[B3:B0] <- St[0].[B7:B4], St[0].[B3:B0] <- St[0].[B3:B0]}

  For a x4 device there is no change all Data is handled per Nibble

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in, out]  Data          Data to be swapped

  @retval          MMRC_SUCCESS
**/

MMRC_STATUS
SplitByteResultsPerNibble (
  IN        MMRC_DATA       *MrcData,
  IN        UINT16          PBFlags,
  IN  OUT   UINT16          (*Data)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN  OUT   UINT16          (*DataHL)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2]
)
{
  UINT8     Ch;
  UINT8     Rk;
  UINT8     St;
  UINT8     Ti;

  for (Ti = 0; Ti < MAX_TIMING; Ti++) {
    for (Ch = 0; Ch < MAX_CHANNELS; Ch++) {
      for (Rk = 0; Rk < MAX_RANKS; Rk++) {
        if (!RunOnThisChannel(MrcData, Ch, Rk)) {
          continue;
        }
        if (MrcData->MaxDqCache[Ch][Rk] == MAX_STROBE_X8_DEVICES && ((PBFlags & NIBBLE_PBD_X4X8_DLY) != 0)) {
          for (St = 0; St < MAX_STROBE_X8_DEVICES; St++) {
            if (Data != NULL) {
              //
              // Copy from S[0].[B7:B4] -> S[9].[B3:B0]
              //
              MmrcMemCopy (
                (UINT8 *)&Data[Ti][Ch][Rk][0][St + MAX_STROBE_X8_DEVICES][0],
                (UINT8 *)&Data[Ti][Ch][Rk][0][St][4],
                STROBE_BITS_X4_DEVICES * sizeof(UINT16)
              );
              //
              // Clear S[0].[B7:B4]
              //
              MmrcMemset(
                (UINT8 *)&Data[Ti][Ch][Rk][0][St][4],
                0xff,
                STROBE_BITS_X4_DEVICES * sizeof(UINT16)
              );
            } // Data != NULL
            if (DataHL != NULL) {
              //
              // Copy from S0.[B7:B4] -> S9.[B3:B0]
              //
              MmrcMemCopy (
                (UINT8 *)&DataHL[Ti][Ch][Rk][0][St + MAX_STROBE_X8_DEVICES][0][0],
                (UINT8 *)&DataHL[Ti][Ch][Rk][0][St][4][0],
                STROBE_BITS_X4_DEVICES * sizeof(UINT16) * 2
              );
              //
              // Clear S0.[B7:B4]
              //
              MmrcMemset(
                (UINT8 *)&DataHL[Ti][Ch][Rk][0][St][4][0],
                0xff,
                STROBE_BITS_X4_DEVICES * sizeof(UINT16) * 2
              );
            } // DataHL != NULL
          } // St loop ...
        } // MAX_STROBE_X8_DEVICES
      } // Rk loop ...
    } //  Ch loop ...
  } //  Ti loop ...

  return MMRC_SUCCESS;
}

MMRC_STATUS
DetermineStrobeVal(
  IN  MMRC_DATA *MrcData,
  IN  UINT16 *Range,
  IN  UINT8 Channel,
  IN  UINT8 Rank,
  IN  UINT8 Knob,
  IN  UINT8 Strobe,
  IN  UINT8 Bit,
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
)
{
  UINT8 BitLoop;
  UINT8 MaxBit;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  IsStrobeValid(MrcData, Channel, Rank, Strobe, TRUE, &MaxBit);
  if (Param->ProgramSetGet.ControlKnobs[0].PBDirectFlag == TRUE) {
    //
    // if the Per-bit and strobe impact each other (both are DQ's, then the SL value needs to be the maximum.
    // Find the maximum of each bit of the strobe.
    //
    Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe] = 0xffff;
    for (BitLoop = 0; BitLoop < MaxBit; BitLoop++) {
      if (Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][BitLoop] < Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe]) {
        Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe] = Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][BitLoop];
      }
    }
  } else {
    //
    // if the Per-bit and strobe impact each other inversly (DQ/DQS, then the SL value needs to be the minimum.
    // Find the minimum of each bit of the strobe.
    //
    Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe] = 0x00;
    for (BitLoop = 0; BitLoop < MaxBit; BitLoop++) {
      if (Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][BitLoop] > Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe]) {
        Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe] = Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][BitLoop];
      }
    }
  }
  if (Bit == 0 && Strobe == 0) {
    MspDebugPrint ((MSP_DBG_MAX, "\n"));
  }
  if (Bit == 0) {
    MspDebugPrint((MSP_DBG_MAX, "  %03d", Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe]));
  }
  MspDebugPrint((MSP_DBG_MAX, " %03d", Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit] - Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe]));
  if (Bit == (MaxBit - 1)) {
    MspDebugPrint ((MSP_DBG_MAX, "\n"));
  }
  MspDebugPrint ((MSP_DBG_TST, "\n%s - Done\n", __FUNCTION__));
  return MMRC_SUCCESS;
}

MMRC_STATUS
CopyToLow(
  IN  MMRC_DATA *MrcData,
  IN  UINT16 *Range,
  IN  UINT8 Channel,
  IN  UINT8 Rank,
  IN  UINT8 Knob,
  IN  UINT8 Strobe,
  IN  UINT8 Bit,
  IN  UINT8 Timing,  
  IN  BuildCompositeParams *Param
)
{
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  if (Param->ProgramSetGet.ControlKnobs[0].PBDirectFlag == FALSE) {
    Param->ProgramSetGet.Lo[Timing][Channel][Rank][Knob][Strobe][Bit] = Range[HIGH];
  } else {
    Param->ProgramSetGet.Lo[Timing][Channel][Rank][Knob][Strobe][Bit] = Range[LOW];
  }
  if (Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit] - Param->ProgramSetGet.StrobeVal[Channel][Rank][Knob][Strobe] == 0) {
    Param->ProgramSetGet.Done[Timing][Channel][Rank][Knob][Strobe][Bit] = TRUE;
  } else {
    Param->ProgramSetGet.AllDone = FALSE;
  }
  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function.  Upon looking at the flags, specified within the
  param structure, if the per-bit is set and the calculation is set to true, the system will calculate the per-bit
  value.  The per-bit value is done by getting the average of each bit, and subtracting the current value from the
  average.  if the per-bit flag is not set, the system will simply set the value and exit the function.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ProgramSetGet (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) 
{
  UINT32 StrobeValue;
  UINT32 BitValue;
  UINT8  BitLoop;
  UINT8  MaxBit;
  UINT16 SharedFlags;
  UINT8  ch,rk,st;

  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // In case the signal has byte granularity and we have x4 configuration the limit is strobe 8th
  //
  SharedFlags = Param->ProgramSetGet.Flags;
  if (((SharedFlags & STROBE_GROUP_BYTE) == STROBE_GROUP_BYTE) && 
       (Strobe >= MAX_STROBE_X8_DEVICES) && (Param->ProgramSetGet.Flags != 0xFFFF)) {
    return MMRC_SUCCESS;
  }
  //
  // With printing enabled, the first line will always have the specific Channel, Rank, and Strobe.
  //
  if (Strobe == 0 && Bit == 0) {
    MspDebugPrint ((MSP_DBG_MAX, "\n%d%d%d", Channel, Rank, Timing));
  }
  // 
  // The actual value to be programmed should be put into the "StrobeValue" variable.
  //
  StrobeValue = Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit];
  //
  // Check if the per-bit is needed to be programmed.
  //
  if ((Param->ProgramSetGet.Flags & BIT_SHARED) != BIT_SHARED) {
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    //
    // If the per-bit is needed, check if the bit can be calculated or needs to be swept.
    //
    if ((Param->ProgramSetGet.Flags & CALCULATE_BIT) == CALCULATE_BIT) {
      //
      // Since this function will be called at a bit-level, multiple times for each strobelane,
      // the average strobe lane will be used, this is accomplished by adding all the values
      // from each bit and then dividing it by the number of bits.
      //
        if (Bit == 0) {
          //StrobeValue = 0x3ff; //Todo make Automatic hcasti2
          IsStrobeValid(MrcData, Channel, Rank, Strobe, TRUE, &MaxBit);
          for (BitLoop = 1; BitLoop < MaxBit; BitLoop++) {
            if (Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][BitLoop] < StrobeValue) {
              StrobeValue = Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][BitLoop];
            }
          }
          Param->ProgramSetGet.StrobeValue = StrobeValue;
          MspDebugPrint ((MSP_DBG_MAX, "  %03d", StrobeValue));
        } else { // if (Bi == 0) 
          StrobeValue = Param->ProgramSetGet.StrobeValue;
        } // if (Bi == 0)
        //
        // Program the per-bit value, this number is the difference from the strobe average value 
        // and the per-bit value. This number can be postive or negative.
        //
        if (Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit] - StrobeValue > 15 ) {
          BitValue = 0;
        } else {
          BitValue = 15-(Param->ProgramSetGet.PI[Timing][Channel][Rank][Knob][Strobe][Bit] - StrobeValue);
        }
        GetSetDdrioGroup(MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 0xFF, Param->ProgramSetGet.Index[Timing], CMD_SET_VAL_FC_UC, &BitValue);
        MspDebugPrint ((MSP_DBG_MAX, ":%03d", BitValue));
      } else { // if (Param->ProgramSetGet.Flags & BIT_CALCULATE)
        UINT16           High[MAX_CHANNELS][MAX_STROBES];
        UINT16           Low[MAX_CHANNELS][MAX_STROBES];
        UINT16           Dim1StartPoint[MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2];
        UINT16           Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
        UINT16           Initial[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
        UINT8            Done[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
        UINT16           PBStartValue;        
        UINT32           TempValue;  
        //
        // Doing a sweep to compute delay per-bit value.
        //
        //
        if (Param->ProgramSetGet.FirstEntryFlag == TRUE) {
          if (Strobe == 0 && Bit == 0) {
            MspDebugPrint ((MSP_DBG_MAX, "\n"));
          }
          Param->ProgramSetGet.FirstEntryFlag = FALSE;

          if (SharedFlags & NIBBLE_PBD_X4X8_DLY){
            //
            // We always execute PBD per Nibble
            // The results were already format from Byte --> Nibble Mode 
            // Function: SplitByteResultsPerNibble is executed prior to ProgramSetGet
            //
            DnvNibbleTrainingHook(MrcData, 0, 0, 0);
          }
          BuildComposite(MrcData, Param->ProgramSetGet.Flags, COMPOSITE_NONE, 1, 0, 0, Results, Param, DetermineStrobeVal);
          MspDebugPrint((MSP_DBG_MAX, "Estimating Per Bit Deskew please wait....>\n"));
          // 
          // Do the first sweep to get a baseline to compare future sweeps.
          //
          if((Param->ProgramSetGet.ControlKnobs[0].Index == TxDqDelayG) && (Param->ProgramSetGet.ControlKnobs[0].RelAbsFlag == REL)) {
            //
            // When we use relative sweeps we need to retore the training values to compare from the same starting point different sweeps
            //
            for (ch = 0; ch < MAX_CHANNELS; ch++) {
              for (rk = 0; rk < MAX_RANKS; rk++) {
                if (!RunOnThisChannel(MrcData, ch, rk)) continue;
                for (st = 0; st < MAX_STROBES; st++) {
                  if (!IsStrobeValid (MrcData, ch, rk, st, FALSE, NULL)) {
                    continue;
                  }
                  GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDelay, CMD_GET_CACHE, &TempValue);
                  GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDelay2, CMD_SET_VAL_FC_UC, &TempValue);
                  GetSetDdrioGroup2(MrcData, ch, rk, st, TxDqDrvDelay2, CMD_SET_VAL_FC_UC, &TempValue);
                }
              }
            }
          }
          ProgramInitialSettingsFor1DSweep(MrcData, Param->ProgramSetGet.ControlKnobs[0], Low, High, Dim1StartPoint, (UINT8)0);
          if (SharedFlags & NIBBLE_PBD_X4X8_DLY){
            //
            // Get the results per Byte or Nibble
            //
            DnvNibbleTrainingExitHook(MrcData, 0, 0, 0);
          }
          CompleteSweep(MrcData, Param->ProgramSetGet.ControlKnobs, 0, High, Low, 1, Dim1StartPoint, Results[Timing]);
          if (SharedFlags & NIBBLE_PBD_X4X8_DLY){
            //
            // We always execute PBD per Nibble
            // And then we split the results in a Nibble Format
            //
            DnvNibbleTrainingHook(MrcData, 0, 0, 0);
          }
          SplitByteResultsPerNibble(MrcData, SharedFlags, NULL, Results);

          MmrcMemset(Done, FALSE, MAX_CHANNELS * MAX_TIMING * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * sizeof (UINT8));
          Param->ProgramSetGet.AllDone = FALSE;   // Initial, CopyToLow will change it.
          Param->ProgramSetGet.Done = Done;
          Param->ProgramSetGet.Lo = Initial;
          PBStartValue = Param->ProgramSetGet.ControlKnobs[0].PBStartValue;
          BuildComposite(MrcData, Param->ProgramSetGet.Flags, COMPOSITE_RESULTS, 1, 0, 0, Results, Param, CopyToLow);
          //BuildComposite(MrcData, Param->ProgramSetGet.Flags, COMPOSITE_RESULTS, 1, 0, 0, Results, Param, TestNonLinearPBForDone);
        
          while (Param->ProgramSetGet.AllDone == FALSE && (Param->ProgramSetGet.ControlKnobs[0].PBStartValue > 16)) {
            Param->ProgramSetGet.ControlKnobs[0].PBStartValue -= 16;
            Param->ProgramSetGet.AllDone = TRUE;

            SetPBDLinear(MrcData, Param->ProgramSetGet.ControlKnobs[0].PBStartValue, Param->ProgramSetGet.ControlKnobs[0].PBIndex, Param->ProgramSetGet.ControlKnobs[0].PBSharedFlags, FALSE, Done[Timing]);

            if (SharedFlags & NIBBLE_PBD_X4X8_DLY){
              //
              // Get the results per Byte or Nibble
              //
              DnvNibbleTrainingExitHook(MrcData, 0, 0, 0);
            }

            CompleteSweep(MrcData, Param->ProgramSetGet.ControlKnobs, 0, High, Low, 1, Dim1StartPoint, Results[Timing]);
            if (SharedFlags & NIBBLE_PBD_X4X8_DLY){
              //
              // We always execute PBD per Nibble
              // And then we split the results in a Nibble Format
              //
              DnvNibbleTrainingHook(MrcData, 0, 0, 0);
            }
            SplitByteResultsPerNibble(MrcData, SharedFlags, NULL, Results);

            for (Knob = 0; Knob < MAX_KNOBS; Knob++) {
              for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
                Dim1StartPoint[Rank][Channel][Knob][Strobe][HIGH] = (UINT16)(Results[Timing][Channel][Rank][Knob][Strobe][0][HIGH]);
              } // for St
            } // for Knob
            BuildComposite(MrcData, Param->ProgramSetGet.Flags, COMPOSITE_RESULTS, 1, 0, 0, Results, Param, TestNonLinearPBForDone);
          }
        }
        // This should not be done for phase 0.
        //
      } // if (Param->ProgramSetGet.Flags & BIT_CALCULATE)
  } else { // if Param->ProgramSetGet.Flags
    //
    // Only set the Strobe lane if the bi index is set to 0 and there is a valid index.
    //
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Param->ProgramSetGet.Index[Timing], CMD_SET_VAL_FC_UC, &StrobeValue);
    if ((Param->ProgramSetGet.Flags & RANK_SHARED) == RANK_SHARED) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Param->ProgramSetGet.Index[Timing], CMD_SET_VAL_FC_UC, &StrobeValue);
      }
    }
    MspDebugPrint ((MSP_DBG_MAX, "  %03d", StrobeValue));
  }

  return MMRC_SUCCESS;
}

VOID FillSignalIndexArray(
  IN  OUT   UINT16   Index[MAX_TIMING],
  IN CONTROLKNOBS    *ControlKnobs,
  IN        UINT8    IdxArrayFlag
  ) {
  UINT8 Loop;

  for (Loop = 0; Loop < ControlKnobs[0].NumberOfTiming; Loop++) {
    switch (IdxArrayFlag) {
    case IDX_CLEAR:
      Index[Loop] = (UINT16)-1;
      break;
    case IDX_TIMING:
      Index[Loop] = ControlKnobs[Loop].Index;
      break;
    case IDX_VREF:
      Index[Loop] = ControlKnobs[ControlKnobs[0].NumberOfTiming].Index;
      break;
    default:
      Index[Loop] = ControlKnobs[Loop].Index;
      break;
    }
  }
}

/**
  Supports Late Command Training, Late Read Training, and Late Write training.
  Performs the steps outlined in the Advanced Training Algorihtm document.
  There are two main inputs to the algorithem, Control Knobs and eye Masks.  Both of
  these structures are passed in as they are dependent on the algorithm.  The main algorithm
  can be broken down into 4 fundamental steps.
    
    1.  Determing passing power/Vref given the eyemask.
    2.  Estimate Delay center given the passing eyemasks.
    3.  At the estimated Delay, determine Vref center.
    4.  At the Vref Center, sample/calculate Delay Center.
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    External array of controls for the algorithm.
  @param[in]       NumberContrlKnbs Number of control knobs array.
  @param[in]       EyeMask         Array of passing EyeMasks.
  @param[in]       NumberEM        Number of eyemasks in the array.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
AdvancedTrainingAlgo (
  IN MMRC_DATA *MrcData,
  IN CONTROLKNOBS *ControlKnobsInNV,
  IN UINT8 NumberControlKnobs,
  IN EYEMASK_T  *EyeMask,
  IN UINT8 NumberEyeMask,
  IN UINT8 PerBitEnable
) {
  UINT16    ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT16    PICenter[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT16    PICenterPB[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT16    PICenterVref[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT16    Max[3][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  UINT8     VrefIndex;
  UINT16    Index[MAX_TIMING];
  UINT16    SharedFlags;
  UINT8     Ch;
  CONTROLKNOBS ControlKnobs[5];
  UINT8     EyeMaskIndex;

  UINT8    Loop;
  UINT8    Sample;
  UINT8    Channel;

  MrcData->NumberOfTiming = ControlKnobsInNV[0].NumberOfTiming;

  for (EyeMaskIndex = 0; EyeMaskIndex < NumberEyeMask; EyeMaskIndex++) {
    MspDebugPrint((MSP_DBG_MAX, "EyeMask%d = {%d,%d}\n", EyeMaskIndex, EyeMask[EyeMaskIndex].NumberConsecutiveVoltages, EyeMask[EyeMaskIndex].MinimumDelayWidth));
  }

  MspDebugPrint((
                MSP_DBG_MAX,
                "CN: %d Total: %d Timing: %d\n",
                sizeof (*ControlKnobsInNV),
                sizeof (CONTROLKNOBS) * ControlKnobsInNV[0].NumberKnobs,
                MrcData->NumberOfTiming
                ));
  MmrcMemCopy(ControlKnobs, ControlKnobsInNV, sizeof (CONTROLKNOBS) * NumberControlKnobs);

  //
  // Phase 1 and 2, will always used a single timing even with multiple specified if the sharedIndex 
  // value is set to non -1.  Store the original Control knobs prior to changing so that the original
  // can be restored during phase 3.
  //
  if (ControlKnobs[0].SharedIndex != 0xffff) {
    for (Loop = 0; Loop < NumberControlKnobs; Loop++) {
      if (Loop > 0) {
        ControlKnobs[Loop-1] = ControlKnobs[Loop];
      }
    }
    NumberControlKnobs--;
    ControlKnobs[0].NumberOfTiming = 1;
    MrcData->NumberOfTiming = 1;
    ControlKnobs[0].Index = ControlKnobs[0].SharedIndex;
  }
  //
  // If the number of signals non-per bit is different of 1 then the VrefIndex will change
  //
  VrefIndex = MrcData->NumberOfTiming;
  if (PerBitEnable == TRUE) {
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    if (ControlKnobs[0].PBIndex != 0xffff) {
      SetPBDLinear ( MrcData, ControlKnobs[0].PBStartValue, ControlKnobs[0].PBIndex, ControlKnobs[0].PBSharedFlags, TRUE, NULL);
    }
  }
  for (Ch = 0; Ch < MAX_CHANNELS; Ch++) {
    if (!MrcData->MrcParameters.ChEnabled[Ch]) continue;
    MmrcExecuteTask(MrcData, PERFORM_FIFO_RESET, NO_PRINT, Ch);
  }
  //
  // Step 1. Determine Passing Vrefs.
  // The return data is in ResultsV and the Actual EyeMask that was selected is in the SelectEyeMask 
  // variable.
  //
  Channel = FindActiveChannel(MrcData);
  if(Channel == 0xFF){
    MrcData->ErrorCode = MrcErrParameterOutOfBound;
    MrcDeadLoop();
    return MMRC_FAILURE;
  }
  MmrcMemset(ResultsV, 0, MAX_CONSEC*MAX_TIMING*MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS * 2 * 2);
  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4 && (GetOffsetIndex (MrcData,ControlKnobs[VrefIndex].Index) == GetOffsetIndex (MrcData,TxVref))) {
    MspDebugPrint ((MSP_DBG_MAX, "\n\n Forcing STROBE_SHARED 0x%02x --> ", ControlKnobs[VrefIndex].SharedFlags));
    ControlKnobs[VrefIndex].SharedFlags |= STROBE_SHARED;
    MspDebugPrint ((MSP_DBG_MAX, "0x%02x", ControlKnobs[VrefIndex].SharedFlags));
  }
  MspDebugPrint ((MSP_DBG_MAX, "\n\n PHASE 1: ESTIMATING SL CENTER"));
  MspDebugPrint((MSP_DBG_MAX, "\n\nDetermining Passing Vrefs."));
  DeterminePassingVref2(MrcData, ControlKnobs, NumberControlKnobs-1, ResultsV, EyeMask, NumberEyeMask); 
  //
  // Set the flags for PBD training. This could be optimized further in case it's needed
  //
  if (PerBitEnable == TRUE) {
    for (Loop = 0; Loop < ControlKnobs[0].NumberKnobs; Loop++) {
      MspDebugPrint ((MSP_DBG_MIN, "\nbSweepFlags = 0x%02x\n", ControlKnobs[Loop].SweepFlags));
      //
      // Run the Create1DSweep PER_BIT and keep Bit0 Independent
      //
      ControlKnobs[Loop].SweepFlags |= RUN_PER_BIT | RUN_RESULT_BIT0_INDEPENDENT;
      MspDebugPrint ((MSP_DBG_MIN, "aSweepFlags = 0x%02x\n", ControlKnobs[Loop].SweepFlags));
    }
  } else {
    for (Loop = 0; Loop < ControlKnobs[0].NumberKnobs; Loop++) {
      MspDebugPrint ((MSP_DBG_MIN, "\nbSweepFlags = 0x%02x\n", ControlKnobs[Loop].SweepFlags));
      //
      // Run the Create1DSweep PER_STROBE, Bit0 will keep results across STROBE
      //
      ControlKnobs[Loop].SweepFlags &= ~(RUN_PER_BIT | RUN_RESULT_BIT0_INDEPENDENT);
      MspDebugPrint ((MSP_DBG_MIN, "aSweepFlags = 0x%02x\n", ControlKnobs[Loop].SweepFlags));
    }
  }
  //
  // If the timing is per-bit enabled, apply the per-bit value based on the granularity of the per-bit field.  It should
  // be noted that the per-bit is not necessarily at the same granularity of the StrobeLane, so it must have its own
  // shared flag, this is specific to the timing per-bit flag.  It should be not that the per-bit will be used in the
  // estimation if there is a per-bit and if the per-bit can be calculated, if it must be swept, this step is skipped.
  //
  if (PerBitEnable == TRUE) {
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    if ((ControlKnobs[0].PBSharedFlags != 0xffff) && (ControlKnobs[0].PBSharedFlags & CALCULATE_BIT) == CALCULATE_BIT) {
      //
      // Calculate the center PI, this is an estimate only, and no matter what algorithm, the timing flags should be
      // Knob shared during this phase.  This algorithm, if bit-independent will computer the center per-bit and apply the per-bit 
      // as a difference from the bit to the strobe-level max.
      //
      CalculateCenter1(MrcData, ResultsV, ControlKnobs[0].PBSharedFlags, EyeMask, 0, PICenter);
      //
      // Apply The PICenter per-bit value only.  This will set the Per-Bit fields only.  
      //
      MspDebugPrint((MSP_DBG_MAX, "\n\nEstimating PerBit Pi Center."));
      FillSignalIndexArray (Index, ControlKnobs, IDX_CLEAR);
      ApplySetting (MrcData, PICenter, (UINT16)-1, ControlKnobs[0].PBSharedFlags, Index, ControlKnobs[0].PBIndex, ControlKnobs);
      //
      // Recalculate the center position, at this point, the center position should be the same for all bits.
      //
      MspDebugPrint((MSP_DBG_MAX, "\n\nReDetermining Passing Vrefs."));
      DeterminePassingVref2(MrcData, ControlKnobs, NumberControlKnobs - 1, ResultsV, EyeMask, NumberEyeMask);
    }
  } // if PerBitEnable
  // 
  // Now re-calculate the PICenter but at the granularity of the StrobeLane, not the per-bit.  This uses the same
  // centering algorithm done for the per-bit but at the granularity of the strobes which can be different then the
  // per-bit.
  //
  CalculateCenter1(MrcData, ResultsV, ControlKnobs[0].SharedFlags|KNOB_SHARED, EyeMask, 0, PICenter);
  //
  // Apply the Strobelane values.
  //
  MspDebugPrint((MSP_DBG_MAX, "\n\nEstimating SL PI Center."));
  FillSignalIndexArray (Index, ControlKnobs, IDX_TIMING);
  ApplySetting(MrcData, PICenter, ControlKnobs[0].SharedFlags | KNOB_SHARED, (UINT16)-1, Index, (UINT16)-1, ControlKnobs);

  if (ControlKnobs[VrefIndex].Ranges[HIGH] == ControlKnobs[VrefIndex].Ranges[LOW]) {
    return MMRC_SUCCESS;
  }

  // Step 2a.  The function will compute the maximum eyemask and sweep the data across vrefs overwriting the original
  // data in ResultsV.  No matter what the granularity of the PI, the vref will set the delay values to a specific 
  // strobelane across all bits and knobs.
  //
  MspDebugPrint ((MSP_DBG_MAX, "\n\n PHASE 2: FINAL VREF CENTERING"));
  MspDebugPrint((MSP_DBG_MAX, "\n\nDisplay the VRef Swept Data at the bit granularity."));
  SweepData1(MrcData, ResultsV, EyeMask[0].NumberConsecutiveVoltages, ControlKnobs, VrefIndex, 0, 0, KNOB_SHARED|BIT_SHARED, ResultsV, ControlKnobs[1].NumberSweeps, Max, FALSE);
  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4 && (GetOffsetIndex (MrcData,ControlKnobs[VrefIndex].Index) == GetOffsetIndex (MrcData,TxVref))) {
    MspDebugPrint ((MSP_DBG_MAX, "\n\n Releasing STROBE_SHARED 0x%02x --> ", ControlKnobs[VrefIndex].SharedFlags));
    ControlKnobs[VrefIndex].SharedFlags &= ~STROBE_SHARED;
    MspDebugPrint ((MSP_DBG_MAX, "0x%02x", ControlKnobs[VrefIndex].SharedFlags));
  }
  //
  // Step 2b.  After collecting the swept data across a series of delays for the vref range, the center vref can be calculated.
  // This value is at the granularity of the second (VREF) controlknob.  The centering algorithm is same as step 1 
  //
  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4 && (GetOffsetIndex (MrcData,ControlKnobs[VrefIndex].Index) == GetOffsetIndex (MrcData,TxVref))) {
    //
    // For DDR4 up to now we need to set a safe vref for all strobes hence add strobe shared flag, before doing PDA
    //
    CalculateCenter1(MrcData, ResultsV, ControlKnobs[VrefIndex].SharedFlags | STROBE_SHARED, EyeMask, 0, PICenterVref);
    FillSignalIndexArray (Index, ControlKnobs, IDX_VREF);
    ApplySetting(MrcData, PICenterVref, ControlKnobs[VrefIndex].SharedFlags, (UINT16)-1, Index, ControlKnobs[VrefIndex].PBIndex, ControlKnobs);
    MrcData->PdaEnable = TRUE;
    ControlKnobs[VrefIndex].SweepFlags &= ~RUN_LOCK_STEP_MODE;
  }
  CalculateCenter1(MrcData, ResultsV, ControlKnobs[VrefIndex].SharedFlags, EyeMask, 0, PICenterVref);
  FillSignalIndexArray (Index, ControlKnobs, IDX_VREF);
  SharedFlags = ControlKnobs[VrefIndex].SharedFlags;
  if (ControlKnobs[0].NumberOfTiming > 1) {
    SharedFlags |= TIMING_SHARED;
  }
  ApplySetting(MrcData, PICenterVref, SharedFlags, (UINT16)-1, Index, ControlKnobs[VrefIndex].PBIndex, ControlKnobs);

  MspDebugPrint((MSP_DBG_MAX, "\n\nReapplying PI Center after phase2, which changed values."));
  FillSignalIndexArray(Index, ControlKnobs, IDX_TIMING);
  ApplySetting(MrcData, PICenter, ControlKnobs[0].SharedFlags | KNOB_SHARED, (UINT16)-1, Index, (UINT16)-1, ControlKnobs);
  //
  // Step 3a.   Sweep the timing delays across a series of Vrefs, this will overwrite the ResultsV again but with various vrefs
  // sampled around Vref Center.
  //
  // Restore the orignal.
  MspDebugPrint((MSP_DBG_MAX, "\n\n PHASE 3: FINAL SL PI CENTERING"));

  if (ControlKnobs[0].SharedIndex != 0xffff && PerBitEnable == FALSE) {
    NumberControlKnobs++;
    MmrcMemCopy(ControlKnobs, ControlKnobsInNV, sizeof (CONTROLKNOBS) * NumberControlKnobs);
    //
    // Must duplicate the results across all timings.
    //
    for (Sample=0; Sample < ControlKnobs[MrcData->NumberOfTiming].NumberSweeps; Sample++) {
      MmrcMemCopy(ResultsV[Sample][1], ResultsV[Sample][0], MAX_CHANNELS * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 2 * 2);
    }
    for (Loop=1; Loop < ControlKnobs[0].NumberOfTiming; Loop++) {
      MmrcMemCopy(PICenter[Loop], PICenter[0], MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS * 2);
    }
    MrcData->NumberOfTiming = ControlKnobs[0].NumberOfTiming;
    VrefIndex = ControlKnobs[0].NumberOfTiming;
  }
  if (PerBitEnable == TRUE) {
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    if (ControlKnobs[0].PBIndex != 0xffff) {
      SetPBDLinear(MrcData, ControlKnobs[0].PBStartValue, ControlKnobs[0].PBIndex, ControlKnobs[0].PBSharedFlags, TRUE, NULL);
    }
  }
  MspDebugPrint ((MSP_DBG_MAX, "\n\n PHASE 3: FINAL SL CENTERING"));
  MspDebugPrint((MSP_DBG_MAX, "\n\n Display the Delay Swept Data at the bit granularity."));
  SweepData1(MrcData, ResultsV, ControlKnobs[MrcData->NumberOfTiming].NumberSweeps, ControlKnobs, 0, VrefIndex, 0, 0, ResultsV, ControlKnobs[0].NumberSweeps, Max, FALSE);
  //
  // Step 3b.  ReApply the VREF value.  Since the Vref does not have a per-bit, the PBIndex will be -1 for the knob.  
  // Note that this is happenning AFTER the Vref sweep since it will be corrupted by that step.
  //
  MspDebugPrint ((MSP_DBG_MAX, "\n\nRestore Step2  Vref Centering"));
  FillSignalIndexArray (Index, ControlKnobs, IDX_VREF);
  ApplySetting(MrcData, PICenterVref, SharedFlags, (UINT16)-1, Index, ControlKnobs[VrefIndex].PBIndex, ControlKnobs);
  //
  // Step 3c.  Re-sample and re-calculate the a more accurate Delay Centering. This approach uses the weighting method
  // outlined in the second centering approach.  Similar to the first estimation, it will calculate the per-bit first, 
  // then calculate the strobe lane level.  
  //
  if (PerBitEnable == TRUE) {
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    if (ControlKnobs[0].PBSharedFlags != 0xFFFF) {
      //
      // Calculate the center PI, this is an estimate only, and no matter what algorithm, the timing flags should be
      // Knob shared during this phase.  This algorithm, if bit-independent will computer the center per-bit and apply the per-bit 
      // as a difference from the bit to the strobe-level max.
      //
      CalculateCenter2(MrcData, ResultsV, ControlKnobs[0].PBSharedFlags, ControlKnobs[0].NumberSweeps, PICenterPB, Max[0]);
      //
      // If the results are in Byte Mode we need to split them into Nibble Mode
      //
      SplitByteResultsPerNibble (MrcData, ControlKnobs[0].PBSharedFlags, PICenterPB, NULL);
      //
      // Apply The PICenter per-bit value only.  The Strobe level value does not need to be set at 
      // this time.  During the Vref sweeping, the PI Byte-Center value will be set.
      //
      MspDebugPrint((MSP_DBG_MAX, "\n\nFinal PerBit Pi Center."));
      FillSignalIndexArray (Index, ControlKnobs, IDX_CLEAR);
      ApplySetting(MrcData, PICenterPB, (UINT16)-1, ControlKnobs[0].PBSharedFlags, Index, ControlKnobs[0].PBIndex, ControlKnobs);
      //
      // Recalculate the center position, at this point, the center position should be the same for all bits.
      //
      MspDebugPrint((MSP_DBG_MAX, "\n\nReStore Step1 Timing Centering (Required for P/N Training)."));
      FillSignalIndexArray (Index, ControlKnobs, IDX_TIMING);
      if (ControlKnobs[0].PBSharedFlags & NIBBLE_PBD_X4X8_DLY) {
        //
        // Execute Per Nibble or Byte
        //
        DnvNibbleTrainingExitHook (MrcData, 0, 0, 0);
      }
      ApplySetting(MrcData, PICenter, ControlKnobs[0].SharedFlags|KNOB_SHARED, (UINT16)-1, Index, (UINT16)-1, ControlKnobs);
      MspDebugPrint((MSP_DBG_MAX, "\n\nRestore Step2  Vref Centering"));
      FillSignalIndexArray (Index, ControlKnobs, IDX_VREF);
      ApplySetting(MrcData, PICenterVref, SharedFlags, (UINT16)-1, Index, ControlKnobs[VrefIndex].PBIndex, ControlKnobs);
      MspDebugPrint ((MSP_DBG_MAX, "\n\nReSweep the data."));
      if (ControlKnobs[0].PBSharedFlags & NIBBLE_PBD_X4X8_DLY) {
        //
        // Execute Per Nibble or Byte
        //
        DnvNibbleTrainingExitHook (MrcData, 0, 0, 0);
      }
      if (ControlKnobs[0].SharedIndex != 0xffff) {
        MspDebugPrint((MSP_DBG_MAX, "\n\nEnabling Independent Timing"));
        NumberControlKnobs++;
        MmrcMemCopy(ControlKnobs, ControlKnobsInNV, sizeof (CONTROLKNOBS) * NumberControlKnobs);
        //
        // Must duplicate the results across all timings.
        //
        for (Sample=0; Sample < ControlKnobs[MrcData->NumberOfTiming].NumberSweeps; Sample++) {
          MmrcMemCopy(ResultsV[Sample][1], ResultsV[Sample][0], MAX_CHANNELS * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * 2 * 2);
        }
        for (Loop=1; Loop < ControlKnobs[0].NumberOfTiming; Loop++) {
          MmrcMemCopy(PICenter[Loop], PICenter[0], MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS * 2);
        }
        MrcData->NumberOfTiming = ControlKnobs[0].NumberOfTiming;
        VrefIndex = ControlKnobs[0].NumberOfTiming;
      }
      SweepData1(MrcData, ResultsV, ControlKnobs[MrcData->NumberOfTiming].NumberSweeps, ControlKnobs, 0, VrefIndex, 0, 0, ResultsV, ControlKnobs[0].NumberSweeps, Max, TRUE);
    }
  }
  // 
  // Now re-calculate the PICenter but at the granularity of the StrobeLane, not the per-bit.  This is what is needed for the next
  // steps.
  //
  CalculateCenter2(MrcData, ResultsV, ControlKnobs[0].SharedFlags, ControlKnobs[0].NumberSweeps, PICenter, Max[0]);
  //
  // Step 3d.  Apply the new center values.
  //
  MspDebugPrint((MSP_DBG_MAX, "\n\nPlacing Final SL Pi Center."));
  FillSignalIndexArray (Index, ControlKnobs, IDX_TIMING);
  ApplySetting(MrcData, PICenter, ControlKnobs[0].SharedFlags, (UINT16)-1, Index, (UINT16)-1, ControlKnobs);
  MspDebugPrint((MSP_DBG_MAX, "\n\nPlacing Final SL Vref Center."));
  FillSignalIndexArray (Index, ControlKnobs, IDX_VREF);
  ApplySetting(MrcData, PICenterVref, SharedFlags, (UINT16) -1, Index, ControlKnobs[VrefIndex].PBIndex, ControlKnobs);
  MspDebugPrint((MSP_DBG_MAX, "\n\n"));

  for (Ch = 0; Ch < MAX_CHANNELS; Ch++) {
    if (!MrcData->MrcParameters.ChEnabled[Ch]) continue;
    MmrcExecuteTask(MrcData, PERFORM_FIFO_RESET, NO_PRINT, Ch);
  }
  //
  // Complete.
  //
  return MMRC_SUCCESS;
}

/**
  Using the LoopOnComposite(), this function performs 2 steps: Applies the per-bit setting, and the
  strobelane setting.  The PBFlags and Flags accordingly, if not -1, are used to determine if the 
  appropriate step shoud be called.  It should be noted that the PBFlags and Flags are used if the
  seperate settings are indendent, but if the flags are set to bit-independent, then the flags could
  be used for per-bit setting.  

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Value         Array of values at the granulairty of Flags/PBFlags.
  @param[in]       Flags         Sharing level for the top level (strobes).
  @param[in]       PBFlags       Sharing level for the top level (per-bits).
  @param[in]       Index         GetSet Index for the top level (strobes).
  @param[in]       PBIndex       GetSet Per-bit Index for the top level (per-bits).
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ApplySetting (
  IN  MMRC_DATA *MrcData,
  IN  UINT16 Value[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS], 
  IN  UINT16 Flags,
  IN  UINT16 PBFlags,
  IN  UINT16 Index[MAX_TIMING],
  IN  UINT16 PBIndex,
  IN  CONTROLKNOBS *ControlKnobs
) {
  BuildCompositeParams  Param;
  UINT16                TempResults[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT16                StrobeVal[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES];
  MmrcMemset(TempResults, 0, MAX_TIMING * MAX_CHANNELS * MAX_RANKS * MAX_KNOBS * MAX_STROBES * MAX_BITS * sizeof (UINT16));
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // Print the final resultant vectors that was selected.
  //
  {
    UINT8 i;
    UINT8 i2;
    UINT8 count;
    UINT8 MaxElements;
    UINT8 MaxBit;

    MspDebugPrint ((MSP_DBG_MAX, "\nCRT"));
    count = 3;
    MaxElements = GetMaxElements (MrcData);
    (MaxElements > 9) ? (MaxBit = 4) : (MaxBit = GetMaxBits(MrcData));
    if ((Flags & STROBE_SHARED) == 0 || Flags == (UINT16)-1) {
      for (i=0; i< MaxElements; i++) {
        if (((Flags & STROBE_GROUP_BYTE) == STROBE_GROUP_BYTE) && (i > MAX_LOOP_STROBE_X8_DEVICES) && (Flags != 0xFFFF)) {
          break;
        }
        MspDebugPrint ((MSP_DBG_MAX, "  SL%d", i));
        count+= 5;
        if ((PBFlags & BIT_SHARED) == 0) {
          for (i2 = 0; i2 < MaxBit; i2++) {
            MspDebugPrint ((MSP_DBG_MAX, ":Bi%02d", i2));
            count+= 3;
          }
        }
      }
    }

    MspDebugPrint ((MSP_DBG_MAX, "\n"));
    while (count > 0) {
      MspDebugPrint ((MSP_DBG_MAX, "="));
      count--;
    }

  }
  //
  // If the flags are non-0xff, then indev/flags value are used to set the value.  Because this is not using
  // the Results or counter, the actual values being applied are passed in as the PI variable.
  //
  if (Flags != 0xffff) {
    Param.ProgramSetGet.StrobeVal     = StrobeVal;
    Param.ProgramSetGet.FirstEntryFlag = TRUE;
    Param.ProgramSetGet.PI            = Value;
    Param.ProgramSetGet.Flags         = Flags;
    Param.ProgramSetGet.Index[0]      = Index[0];
    Param.ProgramSetGet.Index[1]      = Index[1];
    Param.ProgramSetGet.ControlKnobs  = ControlKnobs;
    BuildComposite(MrcData, Flags, COMPOSITE_NONE, 1, 0, 0, TempResults, &Param, ProgramSetGet);
  }
  //
  // If the PBflags are non-0xff, then PBindex/PBflags value are used to set the value.  Because this is not using
  // the Results or counter, the actual values being applied are passed in as the PI variable.  This is to set the 
  // per-bit value.
  //
  if (PBFlags != 0xffff) {
    CheckPerBitSupport (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    Param.ProgramSetGet.StrobeVal     = StrobeVal;
    Param.ProgramSetGet.FirstEntryFlag = TRUE;
    Param.ProgramSetGet.PI            = Value;
    Param.ProgramSetGet.Flags         = PBFlags;
    Param.ProgramSetGet.Index[0]      = PBIndex;
    Param.ProgramSetGet.ControlKnobs  = ControlKnobs;
    BuildComposite(MrcData, PBFlags, COMPOSITE_NONE, 1, 0, 0, TempResults, &Param, ProgramSetGet);
  }

  return MMRC_SUCCESS;
}

/**
  This functiongoes through all active channel and ranks, and for each knob, strobe, and bit prints out
  the results.  This is a full dump of all the active ranks, it does not look at the flags, it simply
  prints out all responses.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PrintHiLow (
  IN  MMRC_DATA *MrcData, 
  IN BuildCompositeParams *Param
) {
  UINT32 value;
  UINT8 Channel;
  UINT8 Rank;
  UINT8 Knob;
  UINT8 Strobe;
  UINT8 Bit;
  UINT8 HiLow;

  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  for (Channel=0; Channel< MAX_CHANNELS; Channel++) {
    for (Rank=0; Rank< MAX_RANKS; Rank++) {
      if (RunOnThisChannel (MrcData, Channel, Rank)) {
        for (Knob=0; Knob< MAX_KNOBS; Knob++) {
          for (HiLow=LOW; HiLow<=HIGH; HiLow++) {
            for (Strobe = 0; Strobe < MrcData->MaxDq[Channel][Rank]; Strobe++) {
              for (Bit = 0; Bit < MrcData->MaxBit[Channel][Rank]; Bit++) {
                if (Strobe == 0 && Bit == 0) {
                   MspDebugPrint ((MSP_DBG_MAX, "\n%d%d%d%d", Channel, Rank, Param->PrintHiLow.ControlIndex, HiLow));
                }
                if (Bit == 0) {
                  GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 
                    0xFF, Param->PrintHiLow.ControlKnobs[Param->PrintHiLow.Dim2Index].Index, 
                    CMD_GET_REG, &value); 
                  MspDebugPrint ((MSP_DBG_MAX, "  %03d", value));
                  if ((MrcData->NumberOfTiming > 1) && (Param->PrintHiLow.Dim2Index < MrcData->NumberOfTiming)){
                    GetSetDdrioGroup (MrcData, 0, Channel, 0, Rank, Strobe, Bit, MrcData->DVFSFreqIndex, 
                      0xFF, Param->PrintHiLow.ControlKnobs[(Param->PrintHiLow.Dim2Index)+1].Index, 
                      CMD_GET_REG, &value); 
                    MspDebugPrint ((MSP_DBG_MIN, " - %03d", value));
                  }
                }
                MspDebugPrint ((MSP_DBG_MAX, ":%02d", 
                  Param->PrintHiLow.ResultsV[Param->PrintHiLow.Dim2Loop][Param->PrintHiLow.ControlIndex][Channel][Rank][Knob][Strobe][Bit][HiLow]));
              } // for Bit
            } // for Strobe
          } // for HiLow
        } // for Knob
      } // if RunOnThisChanel
    } // for Rank
  } // for Channel
  MspDebugPrint ((MSP_DBG_MAX, "\n"));
  MspDebugPrint ((MSP_DBG_TST, "\n%s - Done\n", __FUNCTION__));
  return MMRC_SUCCESS;
}

/**
  This function is called from the LoopOnComposite function.  It will adjust the value through
  the use of the GetSet.  The offset is taken as 70% of the maximum value.  Given that the default
  value is the center, when "loop" = 0, the delta is determined by taking 70% of the max value 
  eyeWidth, dividing by 2, and subtracting from the current value.  All other loops, it simply adds
  one delta unit which is 70% max eyewidth / Number of samples.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReAdjust (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN BuildCompositeParams *Param
) {
  INT32    DeltaOffset;
  UINT32   OrigValue;
  UINT32   CacheValue;
  UINT16   SharedFlags;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  //
  // In case the signal has byte granularity and we have x4 configuration the limit is strobe 8th
  //
  SharedFlags = Param->ReAdjust.Flags;
  if (((SharedFlags & STROBE_GROUP_BYTE) != 0) && (Strobe > MAX_LOOP_STROBE_X8_DEVICES) && (Param->ReAdjust.Flags != 0xFFFF)) {
    return MMRC_SUCCESS;
  }
  //
  // Based on loop, either set the delta to the first calculated value, or simply add one delta to value.
  //
  if (Param->ReAdjust.LoopTotal == 1) {
    return MMRC_SUCCESS;
  }
  GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Param->ReAdjust.Index[Timing] + Knob, CMD_GET_REG, (UINT32 *)&OrigValue);
  GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Param->ReAdjust.Index[Timing] + Knob, CMD_GET_CACHE, (UINT32 *)&CacheValue);
  if (Param->ReAdjust.Loop == 0) {
    DeltaOffset = (((Param->ReAdjust.Max[Timing][Channel][Rank][Knob][Strobe][Bit]*5)/15)/2 * -1);
    if (DeltaOffset*-1 > (INT32) OrigValue) {
      DeltaOffset = -1 * OrigValue;
    }
  } else {
    DeltaOffset = ((Param->ReAdjust.Max[Timing][Channel][Rank][Knob][Strobe][Bit]*5)/15/(Param->ReAdjust.LoopTotal-1));
  } 
  //
  // Set the value through the getset, note that if knobs are specificied, it simply gets added to the index.
  //
  MspDebugPrint ((MSP_DBG_TST, " DltO      Loop  Indx  LopT  Original  Cache _Max \n"));
  MspDebugPrint ((MSP_DBG_TST, " %08x  %04x  %04x  %04x  %08x  %04x  %04x\n",
                                DeltaOffset,
                                Param->ReAdjust.Loop,
                                Param->ReAdjust.Index,
                                Param->ReAdjust.LoopTotal,
                                OrigValue,
                                CacheValue,
                                Param->ReAdjust.Max[Channel][Rank][Knob][Strobe][Bit]
                                ));

  GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Param->ReAdjust.Index[Timing] + Knob, CMD_SET_OFFSET_UC, (UINT32 *)&DeltaOffset);
  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  return MMRC_SUCCESS;
}





MMRC_STATUS
Supportx4withByteLevelSignals(
  IN  MMRC_DATA       *MrcData,
  IN  UINT16          Index,
  IN  UINT16          SharedFlags,
  IN  UINT8           Channel,
  IN  UINT8           Rank, 
  OUT UINT8           *NumberElements
) {
  //
  // if the configuration is x4 but the signal has only bytelevel granularity the sweep has to be at byte level
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel(MrcData, Channel, Rank)) continue;
    if (((SharedFlags & STROBE_GROUP_BYTE) != 0) && (MrcData->MaxDq[Channel][Rank] == MAX_STROBE_X4_DEVICES)) {
      NumberElements[Channel]        = MAX_STROBE_X8_DEVICES;
      MrcData->MaxBit[Channel][Rank] = STROBE_BITS_X8_DEVICES;
      CheckX4Support (MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    } else {
      NumberElements[Channel] = MrcData->MaxDq[Channel][Rank];
    }
  }
  return MMRC_SUCCESS;
}

MMRC_STATUS
Restorex4withByteLevelSignals(
IN  MMRC_DATA       *MrcData,
IN  UINT16          Index,
IN  UINT16          SharedFlags,
IN  UINT8           Channel,
IN  UINT8           Rank
) {
  //
  // Restore the original values
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!RunOnThisChannel(MrcData, Channel, Rank)) continue;
    if (((SharedFlags & STROBE_GROUP_BYTE) != 0) && (MrcData->MaxDq[Channel][Rank] == MAX_STROBE_X4_DEVICES)) {
      MrcData->MaxBit[Channel][Rank] = STROBE_BITS_X4_DEVICES;
      CheckX4Support(MrcData, (UINT8 *)__FUNCTION__, __LINE__);
    }
  }
  return MMRC_SUCCESS;
}


/**
  This is the generic function that calls the Create1DSweep function.  It does three individual calls to the sweep 
  function for small/medimum/large steps.  If the LastPastInValid flag, which was passed in, was set to false only the first
  "small" step will be run.  This is because the three individual tests are used to speed up the initial sweep only.
  The high/low/dim1startpoint are all passed in to the sweep function.  The 1DSweep only does 1 channel and 1 rank, so 
  each of these sweeps are looped on every active channel/rank and are combined into the results array.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    External array of controls for the algorithm.
  @param[in]       ControlIndex    Current control index being operated from ControlKnobs array.
  @param[in]       High            High range for the test to execute.
  @param[in]       Low             Low range for the test to execute.
  @param[in]       LastPassInValid Last time function was called, it was invalid, then do not do medium/large steps.
  @param[in, out]  Dim1StartPoint  The starting point and ranges for the 1dsweep.
  @param[out]      Results         Final results.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CompleteSweep (
  IN  MMRC_DATA       *MrcData,
  IN  CONTROLKNOBS    *ControlKnobs, 
  IN  UINT16           ControlIndex, 
  IN  UINT16           High[MAX_CHANNELS][MAX_STROBES], 
  IN  UINT16           Low[MAX_CHANNELS][MAX_STROBES], 
  IN  UINT8            LastPassInValid,
  IN  UINT16           Dim1StartPoint[MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  OUT UINT16           Results[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2]
) {
#if TRAINING_ALGOS == 1
  UINT8  Channel;
  UINT8  Rank;
  UINT8  Knob;
  UINT8  Strobe;
  UINT16 ResultsTemp[MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
  UINT8  NumberElements[MAX_CHANNELS] = {0};
  UINT8  RunType;
  UINT8  ParalellCh;
  UINT8  DebugMsg;
  BOOLEAN   FoundOneActiveChannel;
  MspDebugPrint ((MSP_DBG_TST, "\n%s\n", __FUNCTION__));
  MmrcMemset(ResultsTemp, 0, MAX_CHANNELS*MAX_RANKS*MAX_KNOBS*MAX_STROBES*MAX_BITS * 2 * 2);

  DebugMsg = MrcData->MspData.DebugMsgLevel;

  RunType = ControlKnobs[ControlIndex].SweepFlags;
  MrcData->CpgcOptions.RmtRestoreJedecResetType = ControlKnobs[ControlIndex].PostJedecFlags;
  MrcData->MspData.DebugMsgLevel &= ~MSP_DBG_MAX;
  //
  // This is the first array, this is the only sweep that does not look at the LastPassValid as it must always be called. 
  // When this is called, the params to the 1dSweep are all pre setup so the call is all that is needed.  After each call
  // the new starting points are loaded into the Dim1StartPoint based on the results.
  //
  if (ControlKnobs[ControlIndex].Step[0] != 0) {
    //
    // Loop on all active channels and ranks.
    //
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (RunOnThisChannel (MrcData, Channel, Rank)) {
          //
          // In case of x4 config with Byte Level Granularity get/set signals
          //
          Supportx4withByteLevelSignals(MrcData, ControlKnobs[ControlIndex].Index, ControlKnobs[ControlIndex].SharedFlags, Channel, Rank, NumberElements);
        } // if RunOnThisChannel
      } // for Rank
    } // for Channel

    FoundOneActiveChannel = FALSE;
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (RunOnThisChannel (MrcData, Channel, Rank)) {
          FoundOneActiveChannel = TRUE;
          MspDebugPrint ((MSP_DBG_TST, "\nNumberElements[0] = %d NumberElements[1] = %d\n", NumberElements[0], NumberElements[1]));
          //
          // Do the sweep.
          //
          if (MrcData->MspData.ExecuteThisRoutineInParallel) {
            ParalellCh = 0;
          } else {
            ParalellCh = Channel;
          }
          Create1DSweep (MrcData, ParalellCh, Rank, ControlKnobs[ControlIndex].Index, 1, Dim1StartPoint[Rank], Low, High, ControlKnobs[ControlIndex].Step[0], FALSE, FALSE, TRUE, NumberElements, GenericMemoryTest, ResultsTemp[Rank], "", 1, RunType);
        } // if RunOnThisChannel
      } // for Rank
      if (MrcData->MspData.ExecuteThisRoutineInParallel && FoundOneActiveChannel) {
        break;
      }
    } // for Channel
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (RunOnThisChannel (MrcData, Channel, Rank)) {
          // 
          // Restore the original values
          //
          Restorex4withByteLevelSignals(MrcData, ControlKnobs[ControlIndex].Index, ControlKnobs[ControlIndex].SharedFlags, Channel, Rank);
          // 
          // Set new starting points based on the results.
          //
          for (Knob= 0; Knob < MAX_KNOBS; Knob++) {
            for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
              Dim1StartPoint[Rank][Channel][Knob][Strobe][LOW]  = (UINT16) (ResultsTemp[Rank][Channel][Knob][Strobe][0][LOW]);
              Dim1StartPoint[Rank][Channel][Knob][Strobe][HIGH]  = (UINT16) (ResultsTemp[Rank][Channel][Knob][Strobe][0][HIGH]);
            } // for St
          } // for Knob
        } // if RunOnThisChannel
      } // for Rank
    } // for Channel
  } // if Step[0]
  //
  // The second and third sweep are only called if the LastPastInValid was set to true.  Each of the individual sweeps themselves
  // are called if the appropriate steps were set.  If set to 0, then the sweep is not taken.  Similar to the first sweep, each 
  // one is called multiple times, once for each active channel/rank combination.
  //
  if (LastPassInValid == 1) {
    //
    // Only do sweep if step[1] is non-zero.
    //
    if (ControlKnobs[ControlIndex].Step[1] != 0) {
      //
      // Loop through all active rank/channels.
      //
      for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          NumberElements[Channel] = MrcData->MaxDq[Channel][Rank];
          if (RunOnThisChannel (MrcData, Channel, Rank)) {
            //
            // Call the sweep.
            //
            Supportx4withByteLevelSignals(MrcData, ControlKnobs[ControlIndex].Index, ControlKnobs[ControlIndex].SharedFlags, Channel, Rank, NumberElements);
          } // if RunOnThisChannel
        } // for Rank
      } // for Channel

      FoundOneActiveChannel = FALSE;
      for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (RunOnThisChannel (MrcData, Channel, Rank)) {
            FoundOneActiveChannel = TRUE;
            if (MrcData->MspData.ExecuteThisRoutineInParallel) {
              Create1DSweep(MrcData, 0, Rank, ControlKnobs[ControlIndex].Index, 1, Dim1StartPoint[Rank], Low, High, ControlKnobs[ControlIndex].Step[1], FALSE, FALSE, TRUE, NumberElements, GenericMemoryTest, ResultsTemp[Rank], "", 1, RunType);
            } else {
              Create1DSweep(MrcData, Channel, Rank, ControlKnobs[ControlIndex].Index, 1, Dim1StartPoint[Rank], Low, High, ControlKnobs[ControlIndex].Step[1], FALSE, FALSE, TRUE, NumberElements, GenericMemoryTest, ResultsTemp[Rank], "", 1, RunType);
            }
          } // if RunOnThisChannel
        } // for Rank
        if (MrcData->MspData.ExecuteThisRoutineInParallel && FoundOneActiveChannel) {
          break;
        }
      } // for Channel
      for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (RunOnThisChannel (MrcData, Channel, Rank)) {
            Restorex4withByteLevelSignals(MrcData, ControlKnobs[ControlIndex].Index, ControlKnobs[ControlIndex].SharedFlags, Channel, Rank);
            // 
            // Set new starting points based on the results.
            //
            for (Knob= 0; Knob < MAX_KNOBS; Knob++) {
              for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
                Dim1StartPoint[Rank][Channel][Knob][Strobe][LOW] = (UINT16)(ResultsTemp[Rank][Channel][Knob][Strobe][0][LOW]);
                Dim1StartPoint[Rank][Channel][Knob][Strobe][HIGH] = (UINT16)(ResultsTemp[Rank][Channel][Knob][Strobe][0][HIGH]);
              } // for Strobe
            } // for Kn
          } // if RunOnThisChannel
        } // for Rank
      } // for Channel
    } // if Step[1]
    //
    // Only do sweep if step[2] is non-zero.
    //
    if (ControlKnobs[ControlIndex].Step[2] != 0) {
      //
      // Loop through all active rank/channels.
      //
      for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (RunOnThisChannel (MrcData, Channel, Rank)) {
            //
            // Call the sweep.
            //
            Supportx4withByteLevelSignals(MrcData, ControlKnobs[ControlIndex].Index, ControlKnobs[ControlIndex].SharedFlags, Channel, Rank, NumberElements);
          } // if RunOnThisChannel
        } // for Rank
      } // for Channel
      FoundOneActiveChannel = FALSE;
      for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (RunOnThisChannel (MrcData, Channel, Rank)) {
            FoundOneActiveChannel = TRUE;
            if (MrcData->MspData.ExecuteThisRoutineInParallel) {
              Create1DSweep(MrcData, 0, Rank, ControlKnobs[ControlIndex].Index, 1, Dim1StartPoint[Rank], Low, High, ControlKnobs[ControlIndex].Step[2], FALSE, FALSE, TRUE, NumberElements, GenericMemoryTest, ResultsTemp[Rank], "", 1, RunType);
            } else {
              Create1DSweep(MrcData, Channel, Rank, ControlKnobs[ControlIndex].Index, 1, Dim1StartPoint[Rank], Low, High, ControlKnobs[ControlIndex].Step[2], FALSE, FALSE, TRUE, NumberElements, GenericMemoryTest, ResultsTemp[Rank], "", 1, RunType);
            }
          } // if RunOnThisChannel
        } // for Rank
        if (MrcData->MspData.ExecuteThisRoutineInParallel && FoundOneActiveChannel) {
          break;
        }
      } // for Channel
      for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
        for (Rank = 0; Rank < MAX_RANKS; Rank++) {
          if (RunOnThisChannel (MrcData, Channel, Rank)) {
            Restorex4withByteLevelSignals(MrcData, ControlKnobs[ControlIndex].Index, ControlKnobs[ControlIndex].SharedFlags, Channel, Rank);
          } // if RunOnThisChannel
        } // for Rank
      } // for Channel
    } // if Step[2]
  } // UnknownLastPass

  // 
  // The results from the Create1DSweepLastPass or not i the normal [ch][rn][kn][st][bi], so the values must be taken
  // from he resultsTemp array and reordered correctly into the Results array.
  //
  for (Channel=0; Channel < MAX_CHANNELS; Channel++) {
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (RunOnThisChannel (MrcData, Channel, Rank)) {
        memcpy(Results[Channel][Rank], ResultsTemp[Rank][Channel], MAX_KNOBS*MAX_STROBES*MAX_BITS*2*2);
      } // if RunOnThisChannel
    } // for Rn
  } // for Channel

  MrcData->MspData.DebugMsgLevel = DebugMsg;
  MspDebugPrint ((MSP_DBG_TST, "\n%s - Done\n", __FUNCTION__));

#endif
  return MMRC_SUCCESS;
}

MMRC_STATUS
GenericMemoryTest (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
  )
{
  UINT8   IndCh;


  for (IndCh = 0; IndCh < MAX_CHANNELS; IndCh++) {
    if (!RunOnThisChannel (MrcData, IndCh, Rank)) {
      continue;
    }
    MrcData->CpgcOptions.Rank = Rank;
    CpgcReconfig (MrcData, IndCh);
  } // Channel loop ...

  CpgcExecuteRmtTest(MrcData, Channel, Rank, NumberElements, NumberKnobs, PassFail);

  return MMRC_SUCCESS;
}

#endif
UINT8
GetMaxElements(
IN  OUT   MMRC_DATA     *MrcData
)
{
  UINT8 MaxElements;
  UINT8 ch;
  UINT8 rk;
  MaxElements = 0;
  for (ch = 0; ch < MAX_CHANNELS; ch++) {
    for (rk = 0; rk < MAX_RANKS; rk++) {
      if (RunOnThisChannel(MrcData, ch, rk)) {
        MaxElements = MAX(MaxElements, MrcData->MaxDq[ch][rk]);
      }
    }
  }
  return MaxElements;
}

UINT8
GetMaxBits(
IN  OUT   MMRC_DATA     *MrcData
)
{
  UINT8 MaxBits;
  UINT8 ch;
  UINT8 rk;
  MaxBits = 0;
  for (ch = 0; ch < MAX_CHANNELS; ch++) {
    for (rk = 0; rk < MAX_RANKS; rk++) {
      if (RunOnThisChannel(MrcData, ch, rk)) {
        MaxBits = MAX(MaxBits, MrcData->MaxBit[ch][rk]);
      }
    }
  }
  return MaxBits;
}

MMRC_STATUS
MmrcWriteMsr (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT32            MsrAddress,
  IN      UINT32            MsrDataLow,
  IN      UINT32            MsrDataHigh
  )
{
#if !defined SIM && !defined JTAG

  MspDebugPrint ((MSP_DBG_MIN, "MSR[0x%04x]\n", MsrAddress));
  MspDebugPrint ((MSP_DBG_MIN, "  Low:  0x%08x\n", MsrDataLow));
  MspDebugPrint ((MSP_DBG_MIN, "  High: 0x%08x\n", MsrDataHigh));


#ifdef __GNUC__
  asm (
    "wrmsr;"
    :
    :"c" (MsrAddress),
     "a" (MsrDataLow),
     "d" (MsrDataHigh)
  );
#else
  _asm mov edx, MsrDataHigh
  _asm mov eax, MsrDataLow
  _asm mov ecx, MsrAddress
  _asm wrmsr
#endif
#endif
  return MMRC_SUCCESS;
}

UINT64
MmrcReadMsr (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT32            MsrAddress
  )
{
#if !defined SIM && !defined JTAG
  

  MspDebugPrint ((MSP_DBG_MIN, "MSR[0x%04x]\n", MsrAddress));

#ifdef __GNUC__
  UINT64  Data64;

  asm (
    "rdmsr"
    : "=A" (Data64)       // %0
    : "c"  (MsrAddress)   // %1
  );

  return Data64;
#else
  _asm {
    mov     ecx, MsrAddress
    rdmsr
  }
#endif
#endif
}
MMRC_STATUS
StoreDelay (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  UINT32 Value;
  UINT8  Ti;

  for (Ti = 0; Ti < Param->StoreRestore.NumberTimings; Ti++) {
    GetSetDdrioGroup2 (MrcData, Channel,  Rank, Strobe, Param->StoreRestore.ControlIndex[Ti], CMD_GET_REG, &Value);
    //GetSetDdrioGroup2 (MrcData, Channel,  Rank, Strobe, Param->StoreRestore.ControlIndex[Ti], UPD_CACHE, &Value);
    if (Ti == 0) {
      MrcData->CacheRxP[Channel][Rank][Strobe] = (UINT8)Value;
    } else {
      MrcData->CacheRxN[Channel][Rank][Strobe] = (UINT8)Value;
    }
  }
  return MMRC_SUCCESS;
}

MMRC_STATUS
RestoreDelay (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
) {
  UINT32 Value;
  UINT8  Ti;

  for (Ti = 0; Ti < Param->StoreRestore.NumberTimings; Ti++) {
    //GetSetDdrioGroup2 (MrcData, Channel,  Rank, Strobe, Param->StoreRestore.ControlIndex[Ti], CMD_GET_CACHE, &Value);
    if (Ti == 0) {
      Value = MrcData->CacheRxP[Channel][Rank][Strobe];
    } else {
      Value = MrcData->CacheRxN[Channel][Rank][Strobe];
    }
    GetSetDdrioGroup2 (MrcData, Channel,  Rank, Strobe, Param->StoreRestore.ControlIndex[Ti], CMD_SET_VAL_FC_UC, &Value);
  }
  return MMRC_SUCCESS;
}

