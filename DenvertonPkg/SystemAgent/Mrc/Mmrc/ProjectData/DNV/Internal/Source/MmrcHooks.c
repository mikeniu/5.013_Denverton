/** @file
  MmrcHooks.c 
  Modular MRC calls functions in this file which perform tasks 
  which are outside the realm of DDRIO initialization. Things 
  like print routines, DUNIT routines which the MMRC needs, 
  JEDEC init, etc, will be placed here. The routines in this 
  file should NOT reference any host structure besides MMRC_DATA 
  since this file is build in the MMRC standalone mode which 
  does not include any MRC dependencies outside the MMRC. 
 
  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#include "MmrcHooks.h"
#include "MmrcProjectData.h"
#include "MmrcLibraries.h"
#include "PassGate.h"
#include "MSysProg.h"
#include "Mailbox.h"
#include "NvdimmHooks.h"

UINT16
SearchSubtaskName (
  IN            UINT16    CurrentTaskOffset
);


/**
  Configures the Slice Channel Hash Function 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ProgSliceChannelHash (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if !defined SIM && !defined JTAG
  UINT32                                                     LowData;
  UINT32                                                     HighData;

  LowData   = (UINT32) MrcData->MspData.DynamicVars[0][A_SLICE_CHANNEL_HASH];
  HighData  = (UINT32) (MrcData->MspData.DynamicVars[0][A_SLICE_CHANNEL_HASH] >> 32);
   
#ifdef __GNUC__
  asm (
    "wrmsr;"
    :
    :"c" (SLICE_CHANNEL_HASH_MSR),
     "a" (LowData),
     "d" (HighData)
  );
#else
  _asm mov edx, dword ptr[HighData]
  _asm mov eax, dword ptr[LowData]
  _asm mov ecx, SLICE_CHANNEL_HASH_MSR
  _asm wrmsr
#endif
#endif
  return MMRC_SUCCESS;
}

UINT8 
ReturnFreqPad(
UINT8 Freq
)
{

  switch (Freq){
  case MMRC_1600:
  case MMRC_1866:
    return 2;
  case MMRC_2133:
  case MMRC_2400:
    return 3;
  case MMRC_2666:
    return 4;
    break;
  default:
    return 0;
  }

}
/**
  Increase Tcl for frequency = 1600

  @param[in, out]  Value         Value to check and increase if it is valid.

**/
UINT32
ModifyTclOne(
UINT32 Value
)
{

  switch (Value){
  case 10:
  case 11:
    return 12;
  default:
    return Value;
  }
}
/**
  Increase Tcl for frequency = 1866

  @param[in, out]  Value         Value to check and increase if it is valid.

**/
UINT32
ModifyTclTwo(
UINT32 Value
)
{

  switch (Value){
  case 9:
    return 10;
  case 12:
  case 13:
  case 14:
    return 14;
  default:
    return Value;
  }
}
/**
  Increase Tcl for frequency = 2133

  @param[in, out]  Value         Value to check and increase if it is valid.

**/
UINT32
ModifyTclThree(
UINT32 Value
)
{

  switch (Value){
  case 9:
  case 10:
  case 11:
    return 12;
  case 13:
  case 14:
  case 15:
    return 16;
  default:
    return Value;
  }
}
/**
  Increase Tcl for frequency = 2400

  @param[in, out]  Value         Value to check and increase if it is valid.

**/
UINT32
ModifyTclFour(
UINT32 Value
)
{

  switch (Value){
  case 10:
  case 11:
    return 12;
  case 13:
    return 14;
  case 15:
  case 16:
  case 17:
    return 18;
  default:
    return Value;
  }
}
/**
  Receive enable exit

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.

**/
VOID
ReceiveEnableExit (
  IN    OUT   MMRC_DATA         *MrcData,
  IN          UINT8             Channel
)
{
  UINT32  Rcv2xVal;
  UINT32  RdCmd2rdvldValue;
  UINT32  TclIncrease;
  UINT32  NextNibbleRcv2xVal;
  UINT32  MaxRcv2xVal[MAX_CHANNELS];
  UINT32  MaxRcv2xValPerStrobe[MAX_CHANNELS][MAX_STROBES];
  UINT32  MinRcv2xVal[MAX_CHANNELS][MAX_RANKS][MAX_STROBE_X8_DEVICES];
  UINT32  RecEnDelaySmallest[MAX_STROBES];
  BOOLEAN FlagForDevicex4;
  UINT8   Rk;
  UINT8   St;
  UINT8   Ch;

  UINT32  DiffAmpLenTemp;

  UINT32  Nibble0;
  UINT32  Nibble1;

  UINT32  TempNb0;
  UINT32  TempNb1;

  UINT32  RdCmdLatencyTemp;

  UINT8   ReadRk2RkPerformance;

  FlagForDevicex4 = FALSE;
  DiffAmpLenTemp = 0;
  ReadRk2RkPerformance = 0;
  TclIncrease = 0;
  Ch = Channel;
  if ((MrcData->MrcParameters.BootMode == S0) && (MrcData->SocStepping == DNV_B0_STEP)) {
    return;
  }
  MspDebugPrint((MSP_DBG_MAX, "%s\n", __FUNCTION__));
  MspDebugPrint((MSP_DBG_MAX, "ChRkSt   _2x\n"));
  MmrcMemset(MaxRcv2xVal, 0, MAX_CHANNELS * sizeof(UINT32));
  MmrcMemset(MaxRcv2xValPerStrobe, 0, MAX_CHANNELS * MAX_STROBES * sizeof(UINT32));
  MmrcMemset(MinRcv2xVal, 0xFF, MAX_CHANNELS * MAX_RANKS * MAX_STROBE_X8_DEVICES * sizeof(UINT32));
  MmrcMemset(RecEnDelaySmallest, 0xFF, MAX_STROBES * sizeof(UINT32));

  for (Rk = 0; Rk < MAX_RANKS; Rk++) {
    if (!RunOnThisChannel(MrcData, Ch, Rk)) {
      continue;
    }
    for (St = 0; St < MAX_STROBE_X8_DEVICES; St++) {
      if (!IsStrobeValid(MrcData, Ch, Rk, St, FALSE, NULL)) {
        continue;
      }
      //
      // Read the original 2x value from the Receive Enable.
      //
      GetSetDdrioGroup2(MrcData, Ch, Rk, St, RecEnDelay, CMD_GET_REG, &Rcv2xVal);
      //
      // Determine the 2x value by dividing by the 2x size and rounding down.
      //
      Rcv2xVal = Rcv2xVal / GetHalfClk(MrcData, Ch);

      if (MrcData->MaxBit[Ch][Rk] <= STROBE_BITS_X4_DEVICES) {

        GetSetDdrioGroup2(MrcData, Ch, Rk, (St + MAX_STROBE_X8_DEVICES), RecEnDelay, CMD_GET_REG, &NextNibbleRcv2xVal);
        NextNibbleRcv2xVal = NextNibbleRcv2xVal / GetHalfClk(MrcData, Ch);

        if (Rcv2xVal < NextNibbleRcv2xVal) {
          MinRcv2xVal[Ch][Rk][St] = Rcv2xVal;
        } else {
          MinRcv2xVal[Ch][Rk][St] = NextNibbleRcv2xVal;
        }

        GetSetDdrioGroup2(MrcData, Ch, Rk, St, OdtSegEnOff, CMD_SET_VAL_FC_UC, &MinRcv2xVal[Ch][Rk][St]);
        GetSetDdrioGroup2(MrcData, Ch, Rk, St, OdtEnOff, CMD_SET_VAL_FC_UC, &MinRcv2xVal[Ch][Rk][St]);
        MspDebugPrint((MSP_DBG_MAX, "OdtEn Ch%02d:Rk%02d:St%02d   Rcv2xVal=%03d:x4DeviceMin=%d\n", Ch, Rk, St, Rcv2xVal, MinRcv2xVal[Ch][Rk][St]));

        if (Rcv2xVal > NextNibbleRcv2xVal) {
          MaxRcv2xVal[Ch] = Rcv2xVal;
        } else if (MaxRcv2xVal[Ch] < NextNibbleRcv2xVal) {
          MaxRcv2xVal[Ch] = NextNibbleRcv2xVal;
        }

      } else {

        GetSetDdrioGroup2(MrcData, Ch, Rk, St, OdtSegEnOff, CMD_SET_VAL_FC_UC, &Rcv2xVal);
        GetSetDdrioGroup2(MrcData, Ch, Rk, St, OdtEnOff, CMD_SET_VAL_FC_UC, &Rcv2xVal);
        MspDebugPrint((MSP_DBG_MAX, "OdtEn Ch%02d:Rk%02d:St%02d   Rcv2xVal=%03d:x8DeviceMin=%d\n", Ch, Rk, St, Rcv2xVal, Rcv2xVal));

        if (Rcv2xVal > MaxRcv2xVal[Ch]) {
          MaxRcv2xVal[Ch] = Rcv2xVal;
        }
      } // MaxBit <= STROBE_BITS_X4_DEVICES ...
    } // St loop ...
  } // Rk loop ....
  MspDebugPrint((MSP_DBG_MAX, "Ch:%02d  MaxRcv2xVal=%d\n", Ch, MaxRcv2xVal[Ch]));

  for (St = 0; St < MAX_STROBE_X8_DEVICES; St++) {
    for (Rk = 0; Rk < MAX_RANKS; Rk++) {
      if (!RunOnThisChannel(MrcData, Ch, Rk)) {
        continue;
      }
      //
      // Read the original 2x value from the Receive Enable.
      //
      GetSetDdrioGroup2(MrcData, Ch, Rk, St, RecEnDelay, CMD_GET_REG, &Rcv2xVal);
      //
      // Determine the 2x value by dividing by the 2x size and rounding down.
      //
      Rcv2xVal = Rcv2xVal / GetHalfClk(MrcData, Ch);

      if (MrcData->MaxBit[Ch][Rk] <= STROBE_BITS_X4_DEVICES) {

        GetSetDdrioGroup2(MrcData, Ch, Rk, (St + MAX_STROBE_X8_DEVICES), RecEnDelay, CMD_GET_REG, &NextNibbleRcv2xVal);
        NextNibbleRcv2xVal = NextNibbleRcv2xVal / GetHalfClk(MrcData, Ch);

        if (Rcv2xVal > NextNibbleRcv2xVal) {
          if (MaxRcv2xValPerStrobe[Ch][St] < Rcv2xVal) {
            MaxRcv2xValPerStrobe[Ch][St] = Rcv2xVal;
          }
        } else if (MaxRcv2xValPerStrobe[Ch][St] < NextNibbleRcv2xVal) {
          MaxRcv2xValPerStrobe[Ch][St] = NextNibbleRcv2xVal;
        }
      } else {

        if (Rcv2xVal > MaxRcv2xValPerStrobe[Ch][St]) {
          MaxRcv2xValPerStrobe[Ch][St] = Rcv2xVal;
        }
      } // MaxBit <= STROBE_BITS_X4_DEVICES ...
    } // Rank loop ...
    MspDebugPrint((MSP_DBG_MAX, "Ch:%02d:St:%02d  MaxRcv2xValForDlane:%03d\n", Ch, St, MaxRcv2xValPerStrobe[Ch][St]));
  } // Strobe loop ...

  for (St = 0; St < MAX_STROBES; St++) {
    //
    // Read the original Diff Amp Len.
    //
    GetSetDdrioGroup2(MrcData, Ch, 0, St, DiffAmpLen, CMD_GET_REG, &DiffAmpLenTemp);

    DiffAmpLenTemp += MaxRcv2xValPerStrobe[Ch][St] / 2;
    if (MaxRcv2xValPerStrobe[Ch][St] % 2) {
      DiffAmpLenTemp += 1;
    }
    //
    // Set the Diff Amp Len.
    //
    GetSetDdrioGroup2(MrcData, Ch, 0, St, DiffAmpLen, CMD_SET_VAL_FC_UC, &DiffAmpLenTemp);
  }

  for (St = 0; St < MAX_STROBE_X8_DEVICES; St++) {
    for (Rk = 0; Rk < MAX_RANKS; Rk++) {
      if (!RunOnThisChannel(MrcData, Ch, Rk)) {
        continue;
      }
      //
      // Set rd2rd rank changes (per nibble)
      //
      GetSetDdrioGroup2(MrcData, Ch, Rk, St, RecEnDelay, CMD_GET_REG, &Nibble0);
      Nibble0 = Nibble0 / GetHalfClk(MrcData, Ch);

      if (Nibble0 < RecEnDelaySmallest[St]) {
        RecEnDelaySmallest[St] = Nibble0;
        MspDebugPrint((MSP_DBG_MAX, "Ch%02d:Rk%02d:St%02d   RecEnDelaySmallestNb0=%03d\n", Ch, Rk, St, RecEnDelaySmallest[St]));
      }

      if (MrcData->MaxBit[Ch][Rk] <= STROBE_BITS_X4_DEVICES) {
        GetSetDdrioGroup2(MrcData, Ch, Rk, (St + MAX_STROBE_X8_DEVICES), RecEnDelay, CMD_GET_REG, &Nibble1);
        Nibble1 = Nibble1 / GetHalfClk(MrcData, Ch);
        FlagForDevicex4 = TRUE;

        if (Nibble1 < RecEnDelaySmallest[(St + MAX_STROBE_X8_DEVICES)]) {
          RecEnDelaySmallest[(St + MAX_STROBE_X8_DEVICES)] = Nibble1;
          MspDebugPrint((MSP_DBG_MAX, "Ch%02d:Rk%02d:St%02d   RecEnDelaySmallestNb1=%03d:x4Dev\n", Ch, Rk, St, RecEnDelaySmallest[(St + MAX_STROBE_X8_DEVICES)]));
        }
      }
    }
  }

  switch (MrcData->MrcParameters.DdrFrequency) {
  case MMRC_2666:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      ReadRk2RkPerformance = 2;
    } else {
      ReadRk2RkPerformance = 2;
    }
    break;
  case MMRC_2400:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      ReadRk2RkPerformance = 2;
    } else {
      ReadRk2RkPerformance = 2;
    }
    break;
  case MMRC_2133:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      ReadRk2RkPerformance = 2;
    } else {
      ReadRk2RkPerformance = 2;
    }
    break;
  case MMRC_1866:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      ReadRk2RkPerformance = 2;
    } else {
      ReadRk2RkPerformance = 2;
    }
    break;
  case MMRC_1600:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      ReadRk2RkPerformance = 2;
    } else {
      ReadRk2RkPerformance = 2;
    }
    break;
  default:
    break;
  }

  for (St = 0; St < MAX_STROBE_X8_DEVICES; St++) {
    TempNb0 = ((RecEnDelaySmallest[St] - (ReadRk2RkPerformance)) % 2);

    GetSetDdrioGroup2(MrcData, Ch, 0, St, RdCmdLatency, CMD_GET_REG, &RdCmdLatencyTemp);
    MspDebugPrint((MSP_DBG_MAX, "Ch%02d:St%02d   RdCmdLatencyTempNb0=%03d\n", Ch, St, RdCmdLatencyTemp));
    RdCmdLatencyTemp = (((2 * RdCmdLatencyTemp) + RecEnDelaySmallest[St] - (ReadRk2RkPerformance)) / 2);
    MspDebugPrint((MSP_DBG_MAX, "Ch%02d:St%02d   NewRdCmdLatencyTempNb0=%03d\n", Ch, St, RdCmdLatencyTemp));
    GetSetDdrioGroup2(MrcData, Ch, 0, St, RdCmdLatency, CMD_SET_VAL_FC_UC, &RdCmdLatencyTemp);

    GetSetDdrioGroup2(MrcData, Ch, 0, St, RxDqClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);
    GetSetDdrioGroup2(MrcData, Ch, 0, St, RdEnClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);
    GetSetDdrioGroup2(MrcData, Ch, 0, St, RecEnClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);
    GetSetDdrioGroup2(MrcData, Ch, 0, St, RxDqsClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);

    if (FlagForDevicex4) {
      TempNb1 = ((RecEnDelaySmallest[(St + MAX_STROBE_X8_DEVICES)] - (ReadRk2RkPerformance)) % 2);

      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RdCmdLatency, CMD_GET_REG, &RdCmdLatencyTemp);
      MspDebugPrint((MSP_DBG_MAX, "Ch%02d:St%02d   RdCmdLatencyTempNb1=%03d:x4Dev\n", Ch, St, RdCmdLatencyTemp));
      RdCmdLatencyTemp = (((2 * RdCmdLatencyTemp) + RecEnDelaySmallest[(St + MAX_STROBE_X8_DEVICES)] - (ReadRk2RkPerformance)) / 2);
      MspDebugPrint((MSP_DBG_MAX, "Ch%02d:St%02d   NewRdCmdLatencyTempNb1=%03d:x4Dev\n", Ch, St, RdCmdLatencyTemp));
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RdCmdLatency, CMD_SET_VAL_FC_UC, &RdCmdLatencyTemp);

      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RxDqClkPushOut, CMD_SET_VAL_FC_UC, &TempNb1);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RdEnClkPushOut, CMD_SET_VAL_FC_UC, &TempNb1);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RecEnClkPushOut, CMD_SET_VAL_FC_UC, &TempNb1);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RxDqsClkPushOut, CMD_SET_VAL_FC_UC, &TempNb1);

    } else {

      MspDebugPrint((MSP_DBG_MAX, "Ch%02d:St%02d   NewRdCmdLatencyTempNb1=%03d:x8Dev\n", Ch, St, RdCmdLatencyTemp));
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RdCmdLatency, CMD_SET_VAL_FC_UC, &RdCmdLatencyTemp);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RxDqClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RdEnClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RecEnClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);
      GetSetDdrioGroup2(MrcData, Ch, 0, (St + MAX_STROBE_X8_DEVICES), RxDqsClkPushOut, CMD_SET_VAL_FC_UC, &TempNb0);

    } // MaxBit <= STROBE_BITS_X4_DEVICES ...
  } // Strobe loop ...

  FlagForDevicex4 = FALSE;

  //
  // Read a command group to update the Rdcmd2rdvld
  //
  if (Ch == 0) {
    RdCmd2rdvldValue = (UINT32)MrcData->MspData.DynamicVars[Ch][RL0];
  } else {
    RdCmd2rdvldValue = (UINT32)MrcData->MspData.DynamicVars[Ch][RL1];
  }

  RdCmd2rdvldValue = RdCmd2rdvldValue * 10;
  RdCmd2rdvldValue = (RdCmd2rdvldValue / 2) - 10;
  RdCmd2rdvldValue += ((MaxRcv2xVal[Ch] * 10) / 4);
  RdCmd2rdvldValue += (ReturnFreqPad(MrcData->MrcParameters.DdrFrequency) * 10) / 2;
  if ((RdCmd2rdvldValue % 10) != 0) {
    RdCmd2rdvldValue = RdCmd2rdvldValue / 10;
    RdCmd2rdvldValue += 1;
  } else {
    RdCmd2rdvldValue = RdCmd2rdvldValue / 10;
  }
  RdCmd2rdvldValue += 1;
  if (MrcData->TclIdle == 1) {
    if ((MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) || (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR3_SODIMM)) {
      TclIncrease = RdCmd2rdvldValue;
      MspDebugPrint((MSP_DBG_MAX, "Ch:%02d  RdCmd2rdvldValue before TCL increase :%03d\n", Ch, RdCmd2rdvldValue));

	  switch (MrcData->MrcParameters.DdrFrequency){
	  case MMRC_1600:
        RdCmd2rdvldValue = ModifyTclOne(TclIncrease);
        break;
	  case MMRC_1866:
        RdCmd2rdvldValue = ModifyTclTwo(TclIncrease);
        break;
	  case MMRC_2133:
        RdCmd2rdvldValue = ModifyTclThree(TclIncrease);
        break;
	  case MMRC_2400:
        RdCmd2rdvldValue = ModifyTclFour(TclIncrease);
	    break;
	  default:
	    break;
	  }
    }
  }
  if (Ch == Channel) {
    for (St = 0; St < MAX_STROBES; St++) {
      MspDebugPrint((MSP_DBG_MAX, "Ch:%02d  RdCmd2rdvldValue:%03d\n", Ch, RdCmd2rdvldValue));
      GetSetDdrioGroup2(MrcData, Ch, 0, St, rdcmd2rdvldsig, CMD_SET_VAL_FC_UC, &RdCmd2rdvldValue);
    } // St loop ...
  }

  MspDebugPrint((MSP_DBG_MAX, "ReadRk2RkPerformance:%d\n", ReadRk2RkPerformance));
}

/**
  Write Trainig exit

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.

**/
VOID
WriteTrainingExit (
  IN    OUT   MMRC_DATA         *MrcData,
  IN    OUT   UINT8             Channel
  ) {

  UINT32 TxDqDelayTemp;
  UINT32 TxDqDelaySmallest[MAX_STROBES];
  UINT32 WrCmdLatencyTemp;
  UINT32 Temp1;

  UINT8   Rk;
  UINT8   St;
  UINT8   Ch;

  UINT8   WriteRk2RkPerformance;

  Ch = Channel;
  WriteRk2RkPerformance = 0;

  MmrcMemset(TxDqDelaySmallest, 0xFF, MAX_STROBES * sizeof(UINT32));
  if ((MrcData->MrcParameters.BootMode == S0) && (MrcData->SocStepping == DNV_B0_STEP)) {
    return;
  }
  for (St = 0; St < MAX_STROBES; St++) {
    for (Rk = 0; Rk < MAX_RANKS; Rk++) {
      if (!RunOnThisChannel(MrcData, Ch, Rk)) {
        continue;
      }
      GetSetDdrioGroup2(MrcData, Ch, Rk, St, TxDqDelay, CMD_GET_REG, &TxDqDelayTemp);
      TxDqDelayTemp = TxDqDelayTemp / GetHalfClk(MrcData, Ch);
      MspDebugPrint((MSP_DBG_MAX, "TxDqDelayTemp[%03d]:%03d\n", St, TxDqDelayTemp));

      if (TxDqDelayTemp < TxDqDelaySmallest[St]) {
        TxDqDelaySmallest[St] = TxDqDelayTemp;
        MspDebugPrint((MSP_DBG_MAX, "Ch:%02d:Rk%02d:St%02d  TxDqDelaySmallest:%03d\n", Ch, Rk, St, TxDqDelaySmallest[St]));
      }
    }
  }

  switch (MrcData->MrcParameters.DdrFrequency) {
  case MMRC_2666:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      WriteRk2RkPerformance = 1;
    } else {
      WriteRk2RkPerformance = 1;
    }
    break;
  case MMRC_2400:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      WriteRk2RkPerformance = 1;
    } else {
      WriteRk2RkPerformance = 1;
    }
    break;
  case MMRC_2133:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      WriteRk2RkPerformance = 1;
    } else {
      WriteRk2RkPerformance = 1;
    }
    break;
  case MMRC_1866:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      WriteRk2RkPerformance = 1;
    } else {
      WriteRk2RkPerformance = 1;
    }
    break;
  case MMRC_1600:
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      WriteRk2RkPerformance = 1;
    } else {
      WriteRk2RkPerformance = 1;
    }
    break;
  default:
    break;
  }

  for (St = 0; St < MAX_STROBES; St++) {
    //
    // Set wr2wr rank changes (per nibble)
    //
    GetSetDdrioGroup2(MrcData, Ch, 0, St, WrCmdLatency, CMD_GET_REG, &WrCmdLatencyTemp);
    MspDebugPrint((MSP_DBG_MAX, "Ch:%02d:St%02d  WrCmdLatency:%03d\n", Ch, St, WrCmdLatencyTemp));
    MspDebugPrint((MSP_DBG_MAX, "Ch:%02d:St%02d  NewTxDqDelaySmallest:%03d\n", Ch, St, TxDqDelaySmallest[St]));
    WrCmdLatencyTemp = (((2 * WrCmdLatencyTemp) + TxDqDelaySmallest[St] - (WriteRk2RkPerformance)) / 2);
    MspDebugPrint((MSP_DBG_MAX, "Ch:%02d:St%02d  NewWrCmdLatency:%03d\n", Ch, St, WrCmdLatencyTemp));
    GetSetDdrioGroup2(MrcData, Ch, 0, St, WrCmdLatency, CMD_SET_VAL_FC_UC, &WrCmdLatencyTemp);

    Temp1 = ((TxDqDelaySmallest[St] - (WriteRk2RkPerformance)) % 2);
    MspDebugPrint((MSP_DBG_MAX, "Ch:%02d:St%02d  NewClkPushOut:%03d\n", Ch, St, Temp1));
    GetSetDdrioGroup2(MrcData, Ch, 0, St, IoDrvClkPushOut, CMD_SET_VAL_FC_UC, &Temp1);
    GetSetDdrioGroup2(MrcData, Ch, 0, St, TxDqsClkPushout, CMD_SET_VAL_FC_UC, &Temp1);
    GetSetDdrioGroup2(MrcData, Ch, 0, St, TxDqClkPushout, CMD_SET_VAL_FC_UC, &Temp1);
  } // St loop ...

  MspDebugPrint((MSP_DBG_MAX, "WriteRk2RkPerformance:%d\n", WriteRk2RkPerformance));
}

/**
  Configures a checkpoint to halt the CPU at any point in the MRC code, this
  function is used for debug, and never should be left in the code after debug
  is completed.
  This always halts where the function is placed, in order to continue developer
  must write a '0' into the SSKPD4_DUNIT_REG register

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Name            Pointer with the Name of the Breakpoint.
  @retval          NONE
**/
VOID
BreakpointMrc (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *Name
  )
{
  UINT32  Data32;
  REGISTER_ACCESS     Register;

  Register.Offset = SSKPD4_DUNIT_COMMON_REG;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  MspDebugPrint ((MSP_DBG_MIN, "BN: '%a'. Write '0' to SSKPD4 in order to continue\n", Name));

  MemRegWrite (DUNIT_COMMON, 0, 0, Register, 0x80, 0xf);
  do {
    Data32 = (UINT32) MemRegRead (DUNIT_COMMON, 0, 0, Register);
  } while (Data32 != 0);
  MemRegWrite (DUNIT_COMMON, 0, 0, Register, 0, 0xf);
}

MMRC_STATUS 
MrcDelay (
  IN        UINT8   Type,
  IN        UINT32  Delay
  )
{
#if !defined SIM  && !defined JTAG
  UINT32  TimeCounter;

  switch (Type) {
  case MILLI_DEL:
    for (TimeCounter = 0; TimeCounter < Delay; TimeCounter++) {
      McStall (1000);
    }
    break;
  case MICRO_DEL:
    McStall (Delay);
    break;  
  case NANO_DEL:
    McStallNanoSecond (Delay);
    break;
  default:
    McStall (Delay);
    break;
  }
#endif  
  return MMRC_SUCCESS;
}

/**
  Enables parity check in the Aunit and Bunit through MSR's

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ProgramParityCheck (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
  UINT64 ReadMsrData;
  UINT32 MsrAddress;
  B_CR_BMCMODE_LOW_BUNIT_MCHBAR_STRUCT BmcMode;

  PRINT_FUNCTION_INFO

  if (MrcData->BramParityEnable != 2) { //2 is "Auto" in Setup, meaning don't touch BMCMODE; 1 or 0 is "Enable" or "Disable", respectively
      BmcMode.Data = Mmio32Read(MrcData->MchBar + B_CR_BMCMODE_LOW_BUNIT_MCHBAR_REG);
      if (MrcData->BramParityEnable == 0) {
        BmcMode.Bits.mc_signal_mode = 0;
      } else {
        BmcMode.Bits.mc_signal_mode = 1;
      }
      Mmio32Write(MrcData->MchBar + B_CR_BMCMODE_LOW_BUNIT_MCHBAR_REG, (UINT32)BmcMode.Data);
    }

  if (MrcData->EnableParityCheck) {
    MspDebugPrint ((MSP_DBG_MIN, "Parity Check Enabled\n"));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Parity Check Disabled\n"));
    return MMRC_SUCCESS;
  }

  //
  // Read the MSR
  // Turn off bits 7 -> 8, 
  // and if Parity Check is enabled SET all of them to '1'
  //
  MsrAddress = B_CR_MCI_CTL_MSR;
  ReadMsrData = MmrcReadMsr (MrcData, MsrAddress);
  MspDebugPrint ((MSP_DBG_MIN, "cR[0x%03x] = 0x%08x%08x\n", MsrAddress, (UINT32) (ReadMsrData >> 32), (UINT32) (ReadMsrData & 0xFFFFFFFF)));
  ReadMsrData &= ~0x180;
  MspDebugPrint ((MSP_DBG_MIN, "mR[0x%03x] = 0x%08x%08x\n", MsrAddress, (UINT32) (ReadMsrData >> 32), (UINT32) (ReadMsrData & 0xFFFFFFFF)));
  if (MrcData->MciIehN) {
    ReadMsrData |= 0x180;
  }
  MmrcWriteMsr (MrcData, MsrAddress, (UINT32) (ReadMsrData)   , (UINT32) ((ReadMsrData >> 32) & 0xFFFFFFFF));
  ReadMsrData = MmrcReadMsr (MrcData, MsrAddress);
  MspDebugPrint ((MSP_DBG_MIN, "vR[0x%03x] = 0x%08x%08x\n", MsrAddress, (UINT32) (ReadMsrData >> 32), (UINT32) (ReadMsrData & 0xFFFFFFFF)));
  //
  // Read the MSR
  // Turn off bits 16 -> 20, 
  // and if Parity Check is enabled SET all of them to '1'
  //
  MsrAddress = A_CR_MCI_CTL_MSR;
  ReadMsrData = MmrcReadMsr (MrcData, MsrAddress);
  MspDebugPrint ((MSP_DBG_MIN, "cR[0x%03x] = 0x%08x%08x\n", MsrAddress, (UINT32) (ReadMsrData >> 32), (UINT32) (ReadMsrData & 0xFFFFFFFF)));
  ReadMsrData &= ~0x1F0000;
  MspDebugPrint ((MSP_DBG_MIN, "mR[0x%03x] = 0x%08x%08x\n", MsrAddress, (UINT32) (ReadMsrData >> 32), (UINT32) (ReadMsrData & 0xFFFFFFFF)));
  if (MrcData->MciIehN) {
    ReadMsrData |= 0x1F0000;
  }
  MmrcWriteMsr (MrcData, MsrAddress, (UINT32) (ReadMsrData)   , (UINT32) ((ReadMsrData >> 32) & 0xFFFFFFFF));
  ReadMsrData = MmrcReadMsr (MrcData, MsrAddress);
  MspDebugPrint ((MSP_DBG_MIN, "vR[0x%03x] = 0x%08x%08x\n", MsrAddress, (UINT32) (ReadMsrData >> 32), (UINT32) (ReadMsrData & 0xFFFFFFFF)));

  return MMRC_SUCCESS;
}

/**
  Enables Dunit MCI

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MciDunit (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  REGISTER_ACCESS   Register;
  MCI_CTL2_DUNIT_STRUCT MciCtl2;

  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = MCI_CTL2_DUNIT_REG;
  MspDebugPrint ((MSP_DBG_MAX, "CorrectErrThreshold: %d\n", MrcData->CorrectErrThreshold));
  MciCtl2.Data = (UINT32) MemRegRead  (DUNIT, Channel, 0, Register);
  MciCtl2.Bits.correct_err_thrshold       = MrcData->CorrectErrThreshold;
  MciCtl2.Bits.enable_mc_corrected_signal = 1;
  MemRegWrite (DUNIT, Channel, 0, Register, MciCtl2.Data,0xF);

  return MMRC_SUCCESS;
}

#if PPR_SUPPORT == 1
/**
  Set each test case for the specified input parameters

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       Rank            Rank under test
  @param[in]       BankGroup       Bank group with the Row 
                                   failure
  @param[in]       BankAddress     Bank with the Row failure 
  @param[in]       DramDevice      Dram device with the Row 
                                   failure
  @retval          None
**/
VOID
TestCasePpr (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Channel,
  IN        UINT8       Rank,
  IN        UINT8       BankGroup,
  IN        UINT32      RowAddress,
  IN        UINT8       BankAddress,
  IN        UINT8       DramDevice
)
{
  MrcData->PprParameters.Repair[Channel][Rank][BankGroup]      = TRUE;
  MrcData->PprParameters.RowAddress[Channel][Rank][BankGroup]  = RowAddress;
  MrcData->PprParameters.BankAddress[Channel][Rank][BankGroup] = BankAddress;
  MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup]  = DramDevice;
}

/**
  Performs a test for PPR 
  It forces the sPPR sequence even when the dimms do not support 
  the feature, this will be helpful when verifying waveforms in 
  the SIM-RTL environment.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       SoftPpr         Indicates if method is sPPR
  @retval          None
**/
VOID 
ExecutePprTestCase (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      BOOLEAN           SoftPpr
)
{
#ifndef SIM
  MrcData->MrcParameters.RankPresent[Channel][0]  = TRUE;
  MrcData->MrcParameters.RankPresent[Channel][1]  = TRUE;
  MrcData->MrcParameters.RankPresent[Channel][2]  = TRUE;
  MrcData->MrcParameters.RankPresent[Channel][3]  = TRUE;
                                     
  MrcData->MrcParameters.DimmPresent[Channel][0]  = TRUE;
  MrcData->MrcParameters.DimmPresent[Channel][1]  = TRUE;

  MrcData->MrcParameters.DramType[Channel]        = TypeDdr4;

  MrcData->PprParameters.DeviceDensity[Channel][0] = 2;
  MrcData->PprParameters.DeviceDensity[Channel][1] = 2;
#endif 

  MrcData->PprParameters.DataWidth[Channel][0] = 1; //x8
  MrcData->PprParameters.DataWidth[Channel][1] = 0; //x4

  // 
  // Rank 0
  //
  TestCasePpr (MrcData, Channel, 0, 0, 0x0012, 0, 0);

  // 
  // Rank 1
  //
  TestCasePpr (MrcData, Channel, 1, 1, 0x2100, 1, 6);

  // 
  // Rank 2
  //
  TestCasePpr (MrcData, Channel, 2, 2, 0x0330, 2, 11);

  // 
  // Rank 3
  //
  TestCasePpr (MrcData, Channel, 3, 3, 0x7700, 3, 10);

  MrcData->PprParameters.PprSupport[Channel][0]     = TRUE;
  MrcData->PprParameters.PprSoft[Channel][0]        = SoftPpr;
  MrcData->PprParameters.PprSupport[Channel][1]     = TRUE;
  MrcData->PprParameters.PprSoft[Channel][1]        = !SoftPpr;

  MrcData->PprParameters.GuardKey[Channel][0]       = TRUE;
  MrcData->PprParameters.GuardKey[Channel][1]       = TRUE;
}


/**
  Send the Guard key protection to the specified channel and 
  rank 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       Rank            Current Rank
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SendGuardKeyPattern (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Rank
)
{
  DRAM_CMD_DDR4     DramCmdDdr4;
  UINT8             Pattern;
  //
  // Fill up MR0 command
  //
  DramCmdDdr4.Data                = 0;
  DramCmdDdr4.Bits.command        = 0;
  DramCmdDdr4.Bits.act_n          = 1;
  DramCmdDdr4.Bits.bankaddress    = 0;
  DramCmdDdr4.Bits.bankgroup      = 0;
  DramCmdDdr4.Bits.rankselect     = Rank;
  DramCmdDdr4.Bits.ba             = 0;
  DramCmdDdr4.Bits.mpr            = 0;
  DramCmdDdr4.Bits.prea           = 0;

  for (Pattern = 0; Pattern < MAX_GUARDKEY_PATTERNS; Pattern++) {
    //
    // - Issue a MR0 command
    // 
    DramCmdDdr4.Bits.memoryaddress  = MaxGuardkeyPatterns[Pattern];
    WriteDramCommandSideWrap (MrcData, Channel, Rank, DramCmdDdr4.Data);
    //
    // Wait tMOD (15ns for all frequencies)
    // No need to add delay function
    //
  }
  return MMRC_SUCCESS;
}
/**
  Get the address bit position from the DMAP configuration for 
  the Rank, BG, BA, and Row

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetRowBankLocation (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel
  )
{
  UINT8   Offset;

  for (Offset = 0; Offset < MAX_RANK_BG_BA_ROW_BITS; Offset++) {
    if (MrcData->MspData.DynamicVars[Channel][DynamicDmapBitLocations[Offset]] != 0x1F) {
      MrcData->PprParameters.RsBgBaRowBitPosition[Offset] = (UINT8) MrcData->MspData.DynamicVars[Channel][DynamicDmapBitLocations[Offset]];
      if (Offset >= (MAX_ROW_BITS_DDR4 + MAX_BANK_BITS_DDR4 + MAX_BANK_GROUP_BITS_DDR4)) {
        //
        // RS0 and RS1 equation is A[13 + RSx], instead of A[6+field]
        //
        MrcData->PprParameters.RsBgBaRowBitPosition[Offset] += (13 - 6);
      }
    } else {
      MrcData->PprParameters.RsBgBaRowBitPosition[Offset] = 0xFF;
    }

    MspDebugPrint ((MSP_DBG_MAX, "    RsBgBaRowBitPosition[%02d] at Cpgc_Addr[%02d]\n", Offset, MrcData->PprParameters.RsBgBaRowBitPosition[Offset]));
  }
  return MMRC_SUCCESS;
}

/**
  Calculates the right masks depending in the Data Width in the DRAM device


  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       Rank            Current Rank
  @param[out]      *DqMask         Returns the mask for all DQ's per DRAM device
  @param[out]      *DqPins         Returns the number of DQ pins per DRAM device
  @param[out]      *MaxMask        Returns the maximum number of DqMask's in a 32b Word

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetPprMask (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT16            *DqMask,
  IN      UINT8             *DqPins,
  IN      UINT8             *MaxMask
)
{
  UINT8   Dimm;

  Dimm  = Rank / MAX_RANKS_PER_DIMM;
  switch (MrcData->PprParameters.DataWidth[Channel][Dimm]) {
  case 0:
    *DqMask   = 0x0F;
    *DqPins   = 4;
    *MaxMask  = 8;
    break;
  case 1:
    *DqMask   = 0xFF;
    *DqPins   = 8;
    *MaxMask  = 4;
    break;
  case 2:
    *DqMask   = 0xFFFF;
    *DqPins   = 16;
    *MaxMask  = 2;
    break;
  default:
    MrcData->ErrorCode = MrcErrPprInputParameter;
    return MrcErrPprInputParameter;
    break;
  }

  return MMRC_SUCCESS;
}

/**
  Calculates the composite address for the Rank, BG, BA, and
  Row, determined by the current DMAP configuration.


  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank
  @param[in]       BankGroup       Current Bank Group

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PprSetupAddress (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT8             BankGroup
  )
{
  UINT32  SourceAddress;
  UINT32  TargetAddress;
  UINT32  TempAddress;
  UINT8   Offset;
  UINT16  DqMask;
  UINT8   DqPins;
  UINT8   MaxMask;
  UINT8   Dimm;
  MMRC_STATUS Status;

  TargetAddress   = 0;
  Dimm            = Rank / MAX_RANKS_PER_DIMM;
  Status          = MMRC_SUCCESS;

  SourceAddress   = MrcData->PprParameters.RowAddress[Channel][Rank][BankGroup];
  SourceAddress  |= (MrcData->PprParameters.BankAddress[Channel][Rank][BankGroup] << MAX_ROW_BITS_DDR4);
  SourceAddress  |= (BankGroup << (MAX_ROW_BITS_DDR4 + MAX_BANK_BITS_DDR4));
  SourceAddress  |= (Rank << (MAX_ROW_BITS_DDR4 + MAX_BANK_BITS_DDR4 + MAX_BANK_GROUP_BITS_DDR4));

  for (Offset = 0; Offset < MAX_RANK_BG_BA_ROW_BITS; Offset++) {
    if (MrcData->PprParameters.RsBgBaRowBitPosition[Offset] != 0xFF) {
      TempAddress     = (SourceAddress & (1 << Offset)) >> Offset;
      TargetAddress  |= TempAddress << MrcData->PprParameters.RsBgBaRowBitPosition[Offset];
    }
    MspDebugPrint ((MSP_DBG_TST, "    TargetAddress[%d] = 0x%08x\n", Offset, TargetAddress));
  }
  MrcData->MspData.DynamicVars[Channel][PPR_FAIL_ADDRESS] = TargetAddress;

  Status = GetPprMask(MrcData, Channel, Rank, &DqMask, &DqPins, &MaxMask);

  MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_LOW]  = 0xFFFFFFFF;
  MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_HIGH] = 0xFFFFFFFF;

  if (MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup] >= MaxMask) {
    DqPins = DqPins * (MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup] - MaxMask);
    MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_HIGH] &= ~(DqMask << DqPins);
  } else {
    DqPins = DqPins * (MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup]);
    MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_LOW]  &= ~(DqMask << DqPins);
  } 
  MspDebugPrint ((MSP_DBG_TST, "    PPR_INV_DC_MASK_LOW = 0x%08x%08x\n", 
                  (UINT32) ((MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_LOW] >> 32) & 0xFFFFFFFF), 
                  (UINT32) ((MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_LOW] >> 0)  & 0xFFFFFFFF)
                ));
  MspDebugPrint ((MSP_DBG_TST, "    PPR_INV_DC_MASK_HIGH = 0x%08x%08x\n", 
                  (UINT32) ((MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_HIGH] >> 32) & 0xFFFFFFFF), 
                  (UINT32) ((MrcData->MspData.DynamicVars[Channel][PPR_INV_DC_MASK_HIGH] >> 0)  & 0xFFFFFFFF)
                ));

  return Status;
}

/**
  Verify if the PPR sequence got succeed


  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank
  @param[in]       BankGroup       Current Bank Group

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
VerifyPprResult (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT8             BankGroup
  )
{
  UINT16  DqMask;
  UINT8   DqPins;
  UINT8   MaxMask;
  UINT8   Dimm;
  UINT32  Results;
  UINT32  LaneErrorHigh;
  UINT32  LaneErrorLow;
  UINT32  LaneError;
  MMRC_STATUS Status;

  Dimm            = Rank / MAX_RANKS_PER_DIMM;
  Status          = MMRC_SUCCESS;
  LaneErrorHigh   = 0xDEADBEEF;
  LaneErrorLow    = 0xDEADBEEF;
  //
  // Read a basic pattern to DRAM device
  // Pattern will be set as 0x55
  //
  MmrcExecuteTask (MrcData, PPR_BASIC_READ, NO_PRINT, Channel);
  CpgcExecuteSingleTest (MrcData, Channel, 0, &Results, &LaneErrorHigh, &LaneErrorLow);

  Status = GetPprMask(MrcData, Channel, Rank, &DqMask, &DqPins, &MaxMask);

  MspDebugPrint ((MSP_DBG_TST, "    LaneErrorHigh = 0x%08x\n", LaneErrorHigh));
  MspDebugPrint ((MSP_DBG_TST, "    LaneErrorLow  = 0x%08x\n", LaneErrorLow));

  if (MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup] >= MaxMask) {
    DqPins    = DqPins * (MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup] - MaxMask);
    LaneError = LaneErrorHigh & (DqMask << DqPins);
  } else {
    DqPins    = DqPins * (MrcData->PprParameters.DramDevice[Channel][Rank][BankGroup]);
    LaneError = LaneErrorLow & (DqMask << DqPins);
  }
  MspDebugPrint ((MSP_DBG_TST, "    LaneError     = 0x%08x\n", LaneError));
  if (LaneError) {
    MspDebugPrint ((MSP_DBG_MIN, "    PPR sequence successfully executed\n"));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "    PPR sequence failed\n"));
  }

  return Status;
}

/**
  Run PPR. Decides if PPR needs to be executed

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          ExecutePpr      TRUE/FALSE
**/
BOOLEAN
RunPpr(
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
) {
  BOOLEAN ExecutePpr;
  UINT8   Rank;
  UINT8   Dimm;
  UINT8   BankGroup;
  ExecutePpr = FALSE;

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    Dimm = Rank / MAX_RANKS_PER_DIMM;

    for (BankGroup = 0; BankGroup < MAX_BANK_GROUPS_DDR4; BankGroup++) {
      if (MrcData->PprParameters.Repair[Channel][Rank][BankGroup]) {
        ExecutePpr = TRUE;
      }
    } // BankGroup loop ...
  } // Rank loop ...
  return ExecutePpr;
}

#endif // PPR_SUPPORT

/**
  Performs PPR

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PostPackageRepairEntry (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
#if PPR_SUPPORT == 1
  UINT8    Rank;
  UINT8    Dimm;
  UINT8    BankGroup;
  UINT8    DramType;
  UINT32   Failures;
  MMRC_STATUS   Status;

  DramInitDDR4EMR2  Mrs0Command;
  DramInitDDR4EMR2  Mrs2Command;
  DramInitDDR4EMR3  Mrs3Command;
  DramInitDDR4EMR4  Mrs4Command;
  DramInitDDR4EMR5  Mrs5Command;

  DRAM_CMD_DDR4     DramCmdDdr4;
  //
  // PPR Only applies for DDR4
  //
  Failures = 0;
  //
  //Save MRS for the channel
  //Save RCW if we have RDIMM present in current channel
  //
  SaveMrs(MrcData, Channel);
  if ((MrcData->MspData.DynamicVars[Channel][RDIMM_EN_0] == 1) || (MrcData->MspData.DynamicVars[Channel][RDIMM_EN_1] == 1)) {
    SaveRcw(MrcData, Channel);
  }

  DramType  = MrcData->MrcParameters.DramType[Channel];
  MspDebugPrint ((MSP_DBG_MAX, "DramType: %d\n", DramType));
  if (DramType != TypeDdr4 || !RunPpr(MrcData, Channel)) {
    MspDebugPrint ((MSP_DBG_MIN, "Ppr not running\n"));
    return MMRC_SUCCESS;
  }
  //
  // Disables Scrambler, refreshes and page policy timer
  //
  MmrcExecuteTask (MrcData, PPR_ENTRY, 0, Channel);
  //
  // Cpgc configuration
  //
  MmrcExecuteTask (MrcData, PPR_STATIC_SETUP, 0, Channel);
  GetRowBankLocation (MrcData, Channel);

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    Dimm = Rank / MAX_RANKS_PER_DIMM;

    Mrs0Command.Data  = ReadMrsCommand (MrcData, Channel, Rank, 0);
    Mrs2Command.Data  = ReadMrsCommand (MrcData, Channel, Rank, 2);
    Mrs3Command.Data  = ReadMrsCommand (MrcData, Channel, Rank, 3);
    Mrs4Command.Data  = ReadMrsCommand (MrcData, Channel, Rank, 4);
    Mrs5Command.Data  = ReadMrsCommand (MrcData, Channel, Rank, 5);

    MspDebugPrint ((MSP_DBG_MAX, "*** Rank %d ***\n", Rank));
    MspDebugPrint ((MSP_DBG_MAX, "RP:  %d DP:  %d\n", MrcData->MrcParameters.RankPresent[Channel][Rank], MrcData->MrcParameters.DimmPresent[Channel][Dimm]));
    if (!MrcData->MrcParameters.RankPresent[Channel][Rank] || !MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
      continue;
    }
    MspDebugPrint ((MSP_DBG_MAX, "PPR: %d S/H: %d\n",  MrcData->PprParameters.PprSupport[Channel][Dimm], MrcData->PprParameters.PprSoft[Channel][Dimm]));
    if (!MrcData->PprParameters.PprSupport[Channel][Dimm] || !MrcData->PprParameters.PprSoft[Channel][Dimm]) {
      continue;
    }
    MspDebugPrint ((MSP_DBG_MAX, "DataWidth: %d (0: x4, 1: x8 and 2: x16)\n", MrcData->PprParameters.DataWidth[Channel][Dimm]));
    if (MrcData->PprParameters.DataWidth[Channel][Dimm]) {

    }
    MspDebugPrint ((MSP_DBG_MAX, "DeviceDensity: %d (0: 2Gb, 1: 4Gb, 2: 8Gb, 3: 16Gb 4: 32Gb)\n", MrcData->PprParameters.DeviceDensity[Channel][Dimm]));
    if (MrcData->PprParameters.DeviceDensity[Channel][Dimm] < 2) {
      MspDebugPrint ((MSP_DBG_MAX, "PPR Only applies for devices >= 8Gb\n"));
      continue; 
    }
    MspDebugPrint ((MSP_DBG_MAX, "WriteCRC: %d\nWL_crcdm: %d\nWritePream: %d\nDM: %d\nReadDBI: %d\nWriteDBI: %d\n",
                    Mrs2Command.Bits.writeCRC,
                    Mrs3Command.Bits.WL_crcdm,
                    Mrs4Command.Bits.WritePream,
                    Mrs5Command.Bits.DM,
                    Mrs5Command.Bits.ReadDBI,
                    Mrs5Command.Bits.WriteDBI
                  ));

    for (BankGroup = 0; BankGroup < MAX_BANK_GROUPS_DDR4; BankGroup++) {
      MspDebugPrint ((MSP_DBG_MAX, "Repair[%d][%d][%d]: %d \n", Channel, Rank, BankGroup, MrcData->PprParameters.Repair[Channel][Rank][BankGroup]));
      if (MrcData->PprParameters.Repair[Channel][Rank][BankGroup]) {
        MspDebugPrint ((MSP_DBG_MAX, "  Repairing Row: 0x%08x Bank: %d\n", MrcData->PprParameters.RowAddress[Channel][Rank][BankGroup], MrcData->PprParameters.BankAddress[Channel][Rank][BankGroup]));
        //
        // Fill up the data for a PRE Command
        //
        DramCmdDdr4.Data                = 0;
        DramCmdDdr4.Bits.command        = 2;
        DramCmdDdr4.Bits.act_n          = 1;
        DramCmdDdr4.Bits.bankaddress    = MrcData->PprParameters.BankAddress[Channel][Rank][BankGroup];
        DramCmdDdr4.Bits.bankgroup      = BankGroup;
        DramCmdDdr4.Bits.memoryaddress  = 0;
        DramCmdDdr4.Bits.rankselect     = Rank;
        DramCmdDdr4.Bits.ba             = 0;
        DramCmdDdr4.Bits.mpr            = 0;
        DramCmdDdr4.Bits.prea           = 0;
        //
        // Setup the address with the current DMAP configuration
        // and fill the CPGC registers
        //
        Status = PprSetupAddress (MrcData, Channel, Rank, BankGroup);
        if (Status != MMRC_SUCCESS) {
          return Status;
        }
        //
        // Write a basic pattern to DRAM device
        // Pattern will be set as 0x55
        //
        MmrcExecuteTask (MrcData, PPR_BASIC_WRITE, NO_PRINT, Channel);

        CpgcExecuteSingleTest (MrcData, Channel, 0, &Failures, NULL, NULL);

        if (Failures != 0) {
          MspDebugPrint ((MSP_DBG_MAX, "  BasicW: 0x%08x\n  Aborting PPR\n", Failures));
          if ((MrcData->SkipOptions & SKIP_TEST) == 0) {
            return MMRC_FAILURE;
          }
        }

        //
        // Init PPR Mode
        // - Precharge all banks
        //
        MmrcExecuteTask (MrcData, PPR_MODE, NO_PRINT, Channel);
        PrechargeAll (MrcData, Channel, Rank);
        // 
        // - Enable sPPR. (MR4.sPPR)
        // - Enable  PPR. (MR4.PPR)
        //
        if (MrcData->PprParameters.PprSoft[Channel][Dimm]) {
          Mrs4Command.Bits.SPPR = 1;
        } else {
          Mrs4Command.Bits.PPR  = 1;  
        }
        WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs4Command.Data);
        //
        // Wait for TMOD Minimum = MAX(15ns,24nCK)
        // No need to add delay function
        //

        if (MrcData->PprParameters.GuardKey[Channel][Dimm]) {
          SendGuardKeyPattern (MrcData, Channel, Rank);
        }
        // 
        // - Issue an ACT command to the Fail Row Address (FRA)
        // - Issue a WR with Valid address and set to '0' the Data Word(x16), 
        // Byte(x8) or Nibble (x4) to select the right DRAM in the DIMM.
        // 
        CpgcExecuteSingleTest (MrcData, Channel, 0, &Failures, NULL, NULL);

        //
        // - For sPPR: Wait tWR (15ns for all frequencies <= 2400) after sending the PRE to the bank 
        // - For PPR:  Wait tPGM (min - 200ms, max - 250ms for all freqs) to allow DRAM repair target row address
        //
        if (MrcData->PprParameters.PprSoft[Channel][Dimm]) {
          //
          // No need to add delay function
          //
        } else {
#ifdef SIM
          //
          // For SIM we only want to make sure there is no PRE, REF or any other command after 
          // WRA/WR command is issued + 200ms, we need to comply only tRAS (ACT to PRE command)
          // range from 35ns -> 7.8us * 9 (tREFI * 9) || 3.9us * 9 (tREFI * 9)
          //
          MySimStall (1000);
#else
          if ((MrcData->SkipOptions & SKIP_DELAY) == 0) {
            MspDelay (MILLI_DEL, 200);
          }
#endif
        }
        //
        // - Issue a PRE
        // 
        WriteDramCommandSideWrap (MrcData, Channel, Rank, DramCmdDdr4.Data);
        // 
        // - For PPR: Wait tPGM_Exit (15ns)
        // - For sPPR: Wait 20ns
        // *No need to add delay function
        //

        // 
        // - Disable sPPR. (MR4.sPPR~)
        // - Disable  PPR. (MR4.PPR~)
        // 
        if (MrcData->PprParameters.PprSoft[Channel][Dimm]) {
          Mrs4Command.Bits.SPPR = 0;
        } else {
          Mrs4Command.Bits.PPR  = 0;
        }
        WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs4Command.Data);
        // 
        // - Wait (tPGMPST 50us) 
        //
#ifdef SIM
        MySimStall (50);
#else
        MspDelay (NANO_DEL, 50000);
#endif
        MrcData->MrcParameters.SaveRestore.AlreadyRepaired[Channel][Rank][BankGroup] = TRUE;

        //
        // In case the GuardKey sequence was not properly executed
        // Restore back the MR0 with the right value, so we do not
        // get fake results during the Read verification
        //
        if (MrcData->PprParameters.GuardKey[Channel][Dimm]) {
          WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs0Command.Data);
        }

        VerifyPprResult (MrcData, Channel, Rank, BankGroup);
      } // MrcData->PprParameters.Repair[Channel][Rank][BankGroup]
    } // Bank Group loop ...
  } // Rank loop ...

  MmrcExecuteTask (MrcData, PPR_EXIT, NO_PRINT, Channel);

#endif // PPR_SUPPORT

  return MMRC_SUCCESS;
}

/**
  Overrides Vref 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CapsuleStartIndex 
  @param[in]       StringIndex 
  @param[in]       Channel 
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
VrefOverride (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
  if (MrcData->VrefOverrideEnable) {
    //
    // Override Vref with Setup Option
    //
    WriteDimmVref (MrcData, Channel, MrcData->VrefOverrideValue, OVERRIDE);
    MspDebugPrint ((MSP_DBG_MED, "DIMM Vref Override Val: %d\n", MrcData->VrefOverrideValue));
  }
  return MMRC_SUCCESS;
}

/**
  Sets write VREF prior to start all MRC training to a known value in order to
  avoid failures during early trainings

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CapsuleStartIndex 
  @param[in]       StringIndex 
  @param[in]       Channel 
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
EarlySetWriteVref (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
  UINT8   Strobe;
  UINT8   Rank;
  UINT32  TempValue;

  PRINT_FUNCTION_INFO_MAX;

  if (MrcData->MrcParameters.BootMode == S5) {
    MspDebugPrint ((MSP_DBG_MED, "CH%d Vref Set for all strobes: 0x40\n", Channel));
    //
    // 32 is the linear value which corresponds to 0x40 in the linear to physical array.
    //
    TempValue = 32;
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxVref, CMD_SET_VAL_FC_UC, &TempValue);
      } // Strobe loop ...
    } // Rank loop ...
  } else {
    MspDebugPrint ((MSP_DBG_MED, "CH%d Vref per strobe\n", Channel));
    for (Rank = 0; Rank < MAX_RANKS; Rank++) {
      if (!MrcData->RankEnabled[Channel][Rank]) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_MED, "-- R%d --\n", Rank));
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MED, "  S%d  ", Strobe));
      } // Strobe loop ...
      MspDebugPrint ((MSP_DBG_MED, "\n"));

      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        TempValue = ConvertPhysicalToLinearValue (MrcData, Channel, TxVref, MrcData->MrcParameters.SaveRestore.DimmVrefWrite[Channel][Rank][Strobe]);
        MspDebugPrint ((MSP_DBG_MED, " 0x%02x ", MrcData->MrcParameters.SaveRestore.DimmVrefWrite[Channel][Rank][Strobe]));
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxVref, CMD_SET_VAL_FC_UC, &TempValue);
      } // Strobe loop ...
      MspDebugPrint ((MSP_DBG_MED, "\n"));
    } // Rank loop ...
  }
  return MMRC_SUCCESS;
}

/**
  This routine attempts to acquire the SMBus

  @param[in]       SmbusBase       SMBUS Base Address
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MrcAcquireBus (
  IN      UINT16    SmbusBase
  )
{
  UINT8 StsReg;

  StsReg  = 0;
  StsReg  = (UINT8) IoIn8 (SmbusBase + R_PCH_SMBUS_HSTS);
  if (StsReg & B_PCH_SMBUS_IUS) {
    return MMRC_FAILURE;
  } else if (StsReg & B_PCH_SMBUS_HBSY) {
    //
    // Clear Status Register and exit
    //
    // Wait for HSTS.HBSY to be clear
    do {
      StsReg = (UINT8) IoIn8 (SmbusBase + R_PCH_SMBUS_HSTS);
    } while ((StsReg & B_PCH_SMBUS_HBSY) != 0);

    // Clear all status bits
    IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, 0xFE);
    return MMRC_SUCCESS;
  } else {
    //
    // Clear out any odd status information (Will Not Clear In Use)
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, StsReg);
    return MMRC_SUCCESS;
  }
}

/**
  This routine reads SysCtl registers

  @param[in]       SmbusBase    SMBUS Base Address
  @param[in]       Page         Defines which page has to be 
                                selected
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SetPageAddress (
  IN        UINT16    SmbusBase,
  IN        UINT8     Page
  )
{
#if SIM

#else
  MMRC_STATUS   Status;
  UINT8         StsReg;
  UINT8         Retry;

  Retry = 3;

  //
  // See if its ok to use the bus based upon INUSE_STS bit.
  //
  Status = MrcAcquireBus (SmbusBase);
  if (Status == MMRC_FAILURE) {
    return Status;
  }

  while (Retry--) {
    //
    // Dummy data
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_HD0, 0);
    //
    // Set Auxiliary Control register
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_AUXC, 0);
    //
    // Reset the pointer of the internal buffer
    //
    IoIn8 (SmbusBase + R_PCH_SMBUS_HCTL);
    //
    // Set SMBus slave address for the device to write
    // Page 0 -> 0x6c
    // Page 1 -> 0x6e
    //
    if (Page == 0) {
      IoOut8 (SmbusBase + R_PCH_SMBUS_TSA, 0x6c);
    } else if (Page == 1) {
      IoOut8 (SmbusBase + R_PCH_SMBUS_TSA, 0x6e);
    }
    //
    //
    // Set Command register for the offset to read
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_HCMD, 0);
    //
    // Set Control Register to Set "operation command" protocol and start bit
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_HCTL, (UINT8) (V_PCH_SMBUS_SMB_CMD_BYTE_DATA | B_PCH_SMBUS_START) );
    //
    // Wait for IO to complete
    //
    do {
      StsReg = (UINT8) IoIn8 (SmbusBase + R_PCH_SMBUS_HSTS);
    } while ((StsReg & (BIT4 | BIT3 | BIT2 | BIT1) ) == 0);

    if (StsReg & B_PCH_SMBUS_DERR) {
      //
      // Clear Status Registers and exit
      //
      IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
      IoOut8 (SmbusBase + R_PCH_SMBUS_AUXS, B_PCH_SMBUS_CRCE);
      IoOut8 (SmbusBase + R_PCH_SMBUS_AUXC, 0);

      return MMRC_FAILURE;

    } else if (StsReg & B_PCH_SMBUS_BERR) {
      //
      // Clear Status Registers
      //
      IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
      IoOut8 (SmbusBase + R_PCH_SMBUS_AUXS, B_PCH_SMBUS_CRCE);
      continue;
    } else {
      break;
    }
  } // While (Retry--) loop ...
  //
  // Clear Status Registers and exit
  //
  IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
  IoOut8 (SmbusBase + R_PCH_SMBUS_AUXS, B_PCH_SMBUS_CRCE);
  IoOut8 (SmbusBase + R_PCH_SMBUS_AUXC, 0);
#endif

  return MMRC_SUCCESS;
}

/**
  This routine reads SysCtl registers

  @param[in]       SmbusBase    SMBUS Base Address
  @param[in]       SlvAddr      Targeted Smbus Slave device address
  @param[in]       Operation    Which SMBus protocol will be used
  @param[in]       Offset       Offset of the register
  @param[in]       Length       Number of bytes
  @param[in]       Buffer       Buffer contains values read from registers
  @retval          MMRC_SUCCESS As Passed
  @retval         !MMRC_SUCCESS As Failed
**/
MMRC_STATUS
MrcSmbusExec (
  IN        UINT16    SmbusBase,
  IN        UINT8     SlvAddr,
  IN        UINT8     Operation,
  IN        UINT16    Offset,
  IN        UINT8     *Length,
  IN        UINT8     *Buffer
  )
{
  MMRC_STATUS  Status = MMRC_SUCCESS; // Denverton AptioV override - EIP#265689
  UINT8       SmbusOperation = 0;
#if (SIM == 0 && SUSSW == 0) || CAR
  UINT8       AuxcReg;
  UINT8       StsReg;
  UINT8       SlvAddrReg;
  UINT8       Index;
  UINT8       HostCmdReg;
  UINT8       *CallBuffer;
  BOOLEAN     BufferTooSmall;
#endif
  UINT8       BlockCount = 0;
  UINT8       RetryCount = BUS_TRIES;
  UINT8       AdjustOffset;

  AdjustOffset = (UINT8) (Offset & 0xFF);
  if (Offset >= 0x100) {
    AdjustOffset = (UINT8) (Offset - 0x100);
    SetPageAddress (SmbusBase, 1);
  }

  //
  // MrcSmbusExec supports byte and block read.
  // Only allow Byte or block access
  //
  if (! ((*Length  == VF_SC_BYTE_LEN) || (*Length == VF_SC_BLOCK_LEN) ) ) {
    return MMRC_FAILURE;
  }

#if SIM
  *Buffer = myReadSPDByte (SmbusBase, SlvAddr, AdjustOffset);
  if (AdjustOffset == 0x2 && *Buffer == 0) {
    return MMRC_FAILURE;
  } else {
    return MMRC_SUCCESS;
  }
#elif defined(SUSSW) && !defined(CAR)
  *Buffer = ispd8Read(SmbusBase, SlvAddr, AdjustOffset);
#else
  //
  // See if its ok to use the bus based upon INUSE_STS bit.
  //
  Status = MrcAcquireBus (SmbusBase);
  if (Status == MMRC_FAILURE) {
    return Status;
  }

  CallBuffer = Buffer;

  //
  //SmbStatus Bits of interest
  //[6] = IUS (In Use Status)
  //[4] = FAIL
  //[3] = BERR (Bus Error = transaction collision)
  //[2] = DERR (Device Error = Illegal Command Field, Unclaimed Cycle, Host Device Timeout, CRC Error)
  //[1] = INTR (Successful completion of last command)
  //[0] = HOST BUSY
  // This is the main operation loop.  If the operation results in a Smbus
  // collision with another master on the bus, it attempts the requested
  // transaction again at least BUS_TRIES attempts.
  //
  while (RetryCount--) {
    //
    // Operation Specifics (pre-execution)
    //
    Status          = MMRC_SUCCESS;
    SlvAddrReg      = SlvAddr;
    HostCmdReg      = AdjustOffset;
    AuxcReg         = 0;

    switch (Operation) {

    case SmbusWriteByte:
      IoOut8 (SmbusBase + R_PCH_SMBUS_HD0, CallBuffer[0]);
      SmbusOperation = V_PCH_SMBUS_SMB_CMD_BYTE_DATA;
      break;

    case SmbusReadByte:
      SmbusOperation = V_PCH_SMBUS_SMB_CMD_BYTE_DATA;
      SlvAddrReg |= B_PCH_SMBUS_RW_SEL_READ;
      if (*Length < 1) {
        Status = MMRC_FAILURE;
      }
      *Length = 1;
      break;

    case SmbusWriteBlock:
      SmbusOperation  = V_PCH_SMBUS_SMB_CMD_BLOCK;
      IoOut8 (SmbusBase + R_PCH_SMBUS_HD0, * (UINT8 *) Length);
      BlockCount = (UINT8) (*Length);
      if ((*Length < 1) || (*Length > 32) ) {
        Status = MMRC_FAILURE;
        break;
      }
      AuxcReg |= B_PCH_SMBUS_E32B;
      break;

    case SmbusReadBlock:
      SmbusOperation = V_PCH_SMBUS_SMB_CMD_BLOCK;
      SlvAddrReg |= B_PCH_SMBUS_RW_SEL_READ;
      if ((*Length < 1) || (*Length > 32) ) {
        Status = MMRC_FAILURE;
        break;
      }
      AuxcReg |= B_PCH_SMBUS_E32B;
      break;

    default:
      Status = MMRC_FAILURE;
      break;
    }

    //
    // Set Auxiliary Control register
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_AUXC, AuxcReg);

    //
    // Reset the pointer of the internal buffer
    //
    IoIn8 (SmbusBase + R_PCH_SMBUS_HCTL);

    //
    // Now that the 32 byte buffer is turned on, we can write th block data
    // into it
    //
    if (Operation == SmbusWriteBlock) {
      for (Index = 0; Index < BlockCount; Index++) {
        //
        // Write next byte
        //
        IoOut8 (SmbusBase + R_PCH_SMBUS_HBD, CallBuffer[Index]);
      }
    }

    //
    // Set SMBus slave address for the device to read
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_TSA, SlvAddrReg);

    //
    //
    // Set Command register for the offset to read
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_HCMD, HostCmdReg);

    //
    // Set Control Register to Set "operation command" protocol and start bit
    //
    IoOut8 (SmbusBase + R_PCH_SMBUS_HCTL, (UINT8) (SmbusOperation + B_PCH_SMBUS_START) );

    //
    // Wait for IO to complete
    //
    do {
      StsReg = (UINT8) IoIn8 (SmbusBase + 0);
    } while ((StsReg & (BIT4 | BIT3 | BIT2 | BIT1) ) == 0);

    if (StsReg & B_PCH_SMBUS_DERR) {
      Status = MMRC_FAILURE;
      break;
    } else if (StsReg & B_PCH_SMBUS_BERR) {
      //
      // Clear the Bus Error for another try
      //
      Status = MMRC_FAILURE;
      IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_BERR);
      //
      // Clear Status Registers
      //
      IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
      IoOut8 (SmbusBase + R_PCH_SMBUS_AUXS, B_PCH_SMBUS_CRCE);

      continue;
    }

    //
    // successfull completion
    // Operation Specifics (post-execution)
    //
    switch (Operation) {

    case SmbusReadByte:
      CallBuffer[0] = (UINT8) (IoIn8 (SmbusBase + R_PCH_SMBUS_HD0) );
      break;

    case SmbusWriteBlock:
      IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_BYTE_DONE_STS);
      break;

    case SmbusReadBlock:
      BufferTooSmall = FALSE;
      //
      // Find out how many bytes will be in the block
      //
      BlockCount = (UINT8) (IoIn8 (SmbusBase + R_PCH_SMBUS_HD0) );
      if (*Length < BlockCount) {
        BufferTooSmall = TRUE;
      } else {
        for (Index = 0; Index < BlockCount; Index++) {
          //
          // Read the byte
          //
          CallBuffer[Index] = (UINT8) IoIn8 (SmbusBase + R_PCH_SMBUS_HBD);
        }
      }

      *Length = BlockCount;
      if (BufferTooSmall) {
        Status = MMRC_FAILURE;
      }
      break;

    default:
      break;
    };

    if ((StsReg & B_PCH_SMBUS_BERR) && (Status != MMRC_FAILURE)) {
      //
      // Clear the Bus Error for another try
      //
      Status = MMRC_FAILURE;
      IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_BERR);

      continue;
    } else {
      break;
    }
  }

  //
  // Clear Status Registers and exit
  //
  IoOut8 (SmbusBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
  IoOut8 (SmbusBase + R_PCH_SMBUS_AUXS, B_PCH_SMBUS_CRCE);
  IoOut8 (SmbusBase + R_PCH_SMBUS_AUXC, 0);
  if (Offset >= 0x100) {
    SetPageAddress(SmbusBase, 0);
  }
#endif
  return Status;
}

UINT32
ConvertLinearToPhysicalValue (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        DimIndex,
  IN        UINT32        LinearValue
)
{
  UINT8   TotalSize;
  UINT8   *LinearTable;
  UINT32  PhysicalValue;
  UINT8   DummyValue;

  TotalSize     = 0;
  DummyValue    = 0;
  PhysicalValue = LinearValue;
  LinearTable   = (UINT8 *)&DummyValue;

  if (GetOffsetIndex (MrcData, DimIndex) == GetOffsetIndex (MrcData, TxVref) && MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    TotalSize = MrcData->OemMrcData.TxVrefTable.SizeOfLtoPTable;
    LinearTable = (UINT8 *)MrcData->OemMrcData.TxVrefTable.LinearToPhysicalTable;
  } else if (GetOffsetIndex (MrcData, DimIndex) == GetOffsetIndex (MrcData, CmdVref)) {
    TotalSize   = MrcData->OemMrcData.CmdVrefTable.SizeOfLtoPTable;
    LinearTable = (UINT8 *)MrcData->OemMrcData.CmdVrefTable.LinearToPhysicalTable;
  } else if ((GetOffsetIndex (MrcData, DimIndex) >= GetOffsetIndex (MrcData, RxVocVal0)) &&
             (GetOffsetIndex (MrcData, DimIndex) <= GetOffsetIndex (MrcData, RxVocVal7))) {
    PhysicalValue = (LinearValue > 7) ? LinearValue : 7 - LinearValue;
//    PhysicalValue = (LinearValue > 7) ? LinearValue - 8 : 15 - LinearValue;
  } else {
    PhysicalValue = LinearValue;
  }

  if (LinearValue >= TotalSize && TotalSize != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Warning!! TotalSize = %d Linear = %d \n", TotalSize, LinearValue));
    MrcData->ErrorCode = MrcErrInvalidLinearValue;
    MrcDeadLoop ();
  } else if (TotalSize != 0) {
    PhysicalValue = LinearTable[LinearValue];
  }

  return PhysicalValue;
}

UINT32
ConvertPhysicalToLinearValue (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        DimIndex,
  IN        UINT32        PhysicalValue
)
{
  UINT8   TotalSize;
  UINT8   *PhysicalTable;
  UINT32  LinearValue;
  UINT8   DummyValue;

  TotalSize     = 0;
  DummyValue    = 0;
  LinearValue   = PhysicalValue;
  PhysicalTable = (UINT8 *)&DummyValue;
  if (GetOffsetIndex (MrcData, DimIndex) == GetOffsetIndex (MrcData, TxVref) && MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All){
    TotalSize = MrcData->OemMrcData.TxVrefTable.SizeOfPtoLTable;
    PhysicalTable = (UINT8 *) MrcData->OemMrcData.TxVrefTable.PhysicalToLinearTable;
  } else if (GetOffsetIndex (MrcData, DimIndex) == GetOffsetIndex (MrcData, CmdVref)) {
    TotalSize     = MrcData->OemMrcData.CmdVrefTable.SizeOfPtoLTable;
    PhysicalTable = (UINT8 *) MrcData->OemMrcData.CmdVrefTable.PhysicalToLinearTable;
  } else if ((GetOffsetIndex (MrcData, DimIndex) >= GetOffsetIndex (MrcData, RxVocVal0)) &&
             (GetOffsetIndex (MrcData, DimIndex) <= GetOffsetIndex (MrcData, RxVocVal7))) {
    LinearValue = (PhysicalValue > 7) ? PhysicalValue : 7 - PhysicalValue;
//    LinearValue = (PhysicalValue > 7) ? 15 - PhysicalValue : PhysicalValue + 8;
  } else {
    LinearValue = PhysicalValue;
  }

  if (PhysicalValue >= TotalSize && TotalSize != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Warning!! TotalSize = %d Physical = %d \n", TotalSize, PhysicalValue));
    MrcData->ErrorCode = MrcErrInvalidPhysicalValue;
    MrcDeadLoop ();
  } else if (TotalSize != 0) {
    LinearValue = PhysicalTable[PhysicalValue];
  }

  return LinearValue;
}


MMRC_STATUS
PdaSequence (
IN  OUT   MMRC_DATA     *MrcData,
IN        UINT8         Socket,
IN        UINT8         Channel,
IN        UINT8         Rank,
IN        UINT8         Strobe,
IN  OUT   UINT32        *Value,
IN        UINT8         Side,
IN        BOOLEAN       PdaEnable
)
{
#if defined(DDR4_SUPPORT) && DDR4_SUPPORT == 1
  DramInitDDR4EMR3  Mrs3Command;
  DramInitDDR4EMR6  Mrs6Command;
  UINT8             MaskOffset;
  UINT8             StrobeMask;
  UINT32            InternalValue;

  if(Rank >= MAX_RANKS){
    MrcData->ErrorCode = MrcErrParameterOutOfBound;
    MrcDeadLoop();
    return MMRC_FAILURE;
  }
  InternalValue = *Value;
  PrechargeAll (MrcData, Channel, Rank);
  //
  // Enable Vref Dq training and set the range
  //
  Mrs6Command.Data = ReadMrsCommand (MrcData, Channel, Rank, 6);
  Mrs6Command.Bits.Vrefdq_Enable = 1;
  if (InternalValue <= 23) {
    Mrs6Command.Bits.Vrefdq_Range = 1;
  } else {
    Mrs6Command.Bits.Vrefdq_Range = 0;
    InternalValue -= 23;
  }
  //
  // Set the Vref Value
  //
  Mrs6Command.Bits.Vrefdq_Value = InternalValue;

  if (PdaEnable == FALSE) {
    WriteDramCommand(MrcData, Channel, Rank, Mrs6Command.Data, Side);
  }
  if (PdaEnable == TRUE) {
    //
    // Pda sequence parameters
    //  
    Mrs3Command.Data = ReadMrsCommand(MrcData, Channel, Rank, 3);
    Mrs3Command.Bits.PDA = 1;
    MirrorMrsAndAssignToDynamicVars(MrcData, Channel, Rank, Mrs3Command.Data, PDA_ENTRY, Side);

    Mrs3Command.Data = ReadMrsCommand(MrcData, Channel, Rank, 3);
    Mrs3Command.Bits.PDA = 0;
    MirrorMrsAndAssignToDynamicVars(MrcData, Channel, Rank, Mrs3Command.Data, PDA_EXIT, Side);
  }

  if (PdaEnable == TRUE) {
    MirrorMrsAndAssignToDynamicVars(MrcData, Channel, Rank, Mrs6Command.Data, PDA_CMD, Side);
    if (MrcData->D_DataWidth[Channel][Rank / MAX_RANKS_PER_DIMM] == 0) {
      MaskOffset = 1;
      StrobeMask = 1;
    } else {
      MaskOffset = 2;
      StrobeMask = 3;
    }
    MrcData->MspData.DynamicVars[Channel][PDA_MASK] = ~(StrobeMask << (Strobe * MaskOffset));

    MmrcExecuteTask(MrcData, PDA_SEQ, NO_PRINT, Channel);
    MmrcExecuteTask(MrcData, PDA_SEQ, NO_PRINT, Channel);
    //
    // Disable  Vref Dq training
    //
    Mrs6Command.Bits.Vrefdq_Enable = 0;
    MirrorMrsAndAssignToDynamicVars(MrcData, Channel, Rank, Mrs6Command.Data, PDA_CMD, Side);
    MmrcExecuteTask(MrcData, PDA_SEQ, NO_PRINT, Channel);
  } else {
    //
    // Set the same Vref on all the Dram devices
    //
    WriteDramCommand(MrcData, Channel, Rank, Mrs6Command.Data, Side);
  }
  //
  // Disable Vref Dq training
  //
  if (PdaEnable == FALSE) {
    Mrs6Command.Bits.Vrefdq_Enable = 0;
    WriteDramCommand(MrcData, Channel, Rank, Mrs6Command.Data, Side);
  }

#endif
  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetDimmVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  )
{
  switch (MrcData->MrcParameters.DramType[Channel]){
  case TypeDdr3:
  case TypeDdr3L:
  case TypeDdr3U:
  case TypeDdr3All:
    if ((Cmd & RD_REG) != 0) {
      *Value = MrcData->MrcParameters.SaveRestore.DimmVrefWrite[Channel][0][0];
    } else {
      WriteDimmVref (MrcData, Channel, *Value, OVERRIDE);
      MrcData->MrcParameters.SaveRestore.DimmVrefWrite[Channel][0][0] = (UINT8)*Value;
    }    
  break;
  case TypeDdr4:
#if defined(DDR4_SUPPORT) && DDR4_SUPPORT == 1
    if ((Cmd & RD_REG) != 0) {
      *Value = MrcData->CacheTxVref[Channel][Rank][Strobe];
    } else {
      //
      // For PdaEnable == FALSE using only the value for strobe 0 will set all devices but always update the cache
      //  
      MrcData->CacheTxVref[Channel][Rank][Strobe] = (UINT8) *Value;
      if (((MrcData->PdaEnable == FALSE) && (Strobe > 0)) || 
        ((MrcData->MaxDqCache[Channel][Rank] == MAX_STROBE_X8_DEVICES) && (Strobe > MAX_LOOP_STROBE_X8_DEVICES))) {
        return MMRC_SUCCESS;
      }
      PdaSequence(MrcData, Socket, Channel, Rank, Strobe, Value, SIDE_A, MrcData->PdaEnable);
      //
      // Rdimm's side B
      //
#ifndef SIM
      if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
        PdaSequence(MrcData, Socket, Channel, Rank, Strobe, Value, SIDE_B, MrcData->PdaEnable);
      }
#endif
    }
#endif
    break;
  }
  return MMRC_SUCCESS;
}

/**
  Convert Value into RxVref fields (RangeSel and Ctrl)

  The valid values are in the range 0 - 113.
  If the value is in the range 0 < Value <= 50:
    rxvref_b0_vrefrangesel = 0
    rxvref_b0_vrefctrl     = Value
  If the value is in the range 50 < Value <= 113:
    rxvref_b0_vrefrangesel = 1
    rxvref_b0_vrefctrl     = Value - 50

  @param[in]       Value           Value to be set into RangeSel and Ctrl
  @param[out]      RangeSel        Value of rxvref_b0_vrefrangesel
  @param[out]      Ctrl            Value of rxvref_b0_vrefctrl
  @retval          MMRC_SUCCESS    RangeSel and Ctrl were set successfully
  @retval          MMRC_FAILURE    Invalid Value was received
**/
MMRC_STATUS
Value2RxVrefRegs (
  IN        UINT32        Value,
  IN  OUT   UINT32        *RangeSel,
  IN  OUT   UINT32        *Ctrl
  )
{
  const UINT32  Offset = 50;

  if (Value <= Offset) {
    *RangeSel = 0;
    *Ctrl = Value;
  } else if (Value <= RX_VREF_MAX) {
    *RangeSel = 1;
    *Ctrl = Value - Offset;
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Error: Invalid Value (0x%x). It should be in range: 0x0 - 0x%x\n", Value, RX_VREF_MAX));
    return MMRC_FAILURE;
  }
  return MMRC_SUCCESS;
}

/**
  Convert RxVref fields (RangeSel and Ctrl) into Value

  @param[out]      Value           Value to be constructed from RangeSel and Ctrl
  @param[in]       RangeSel        Value of rxvref_b0_vrefrangesel
  @param[in]       Ctrl            Value of rxvref_b0_vrefctrl
  @retval          MMRC_SUCCESS    No errors
**/
MMRC_STATUS
RxVrefRegs2Value (
  IN  OUT   UINT32        *Value,
  IN        UINT32        RangeSel,
  IN        UINT32        Ctrl
  )
{
  const UINT32  Offset = 50;

  *Value = RangeSel * Offset + Ctrl;
  return MMRC_SUCCESS;
}

/**
  This routine sets or gets the RxVref register fields rxvref_b0_vrefrangesel and rxvref_b0_vrefctrl

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Socket
  @param[in]       Channel         Channel being examined
  @param[in]       Dimm            DIMM being examined
  @param[in]       Rank            Rank being examined
  @param[in]       Strobe          Strobe being examined
  @param[in]       Bit             Bit being examined
  @param[in]       FrequencyIndex
  @param[in]       IoLevel
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in][out]  Value           Pointer to the Read/Write Value
  @retval          Status          MMRC_SUCCESS or MMRC_FAILURE
**/
MMRC_STATUS
GetSetRxVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  )
{
  UINT32        RangeSel;
  UINT32        Ctrl;
  UINT32        NewValue = 0;
  MMRC_STATUS   Status = MMRC_SUCCESS;

  switch (Cmd) {
  case CMD_GET_CACHE:
  case CMD_GET_REG:
  case CMD_GET_REG_UC:
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefRangeSel, Cmd, &RangeSel);
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefCtrl, Cmd, &Ctrl);
    RxVrefRegs2Value (Value, RangeSel, Ctrl);
    break;
  case CMD_SET_OFFSET_UC:
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefRangeSel, CMD_GET_CACHE, &RangeSel);
    GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefCtrl, CMD_GET_CACHE, &Ctrl);
    RxVrefRegs2Value (&NewValue, RangeSel, Ctrl);
  case CMD_SET_VAL_FC_UC:
    NewValue += *Value;
    Status = Value2RxVrefRegs (NewValue, &RangeSel, &Ctrl);
    if (Status == MMRC_SUCCESS) {
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefRangeSel, CMD_SET_VAL_FC_UC, &RangeSel);
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefCtrl, CMD_SET_VAL_FC_UC, &Ctrl);
    }
    break;
  case CMD_SET_VAL_FC:
    Status = Value2RxVrefRegs (*Value, &RangeSel, &Ctrl);
    if (Status == MMRC_SUCCESS) {
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefRangeSel, Cmd, &RangeSel);
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVrefCtrl, Cmd, &Ctrl);
    }
    break;
  default:
    MspDebugPrint ((MSP_DBG_MIN, "Error: Invalid Cmd (0x%x).\n", Cmd));
    Status = MMRC_FAILURE;
    break;
  }

  return Status;
}

MMRC_STATUS
GetSetWrLvlSmp (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{

  UINT32  Nibble0;
  UINT32  Nibble1;
  UINT8   TargetStrobe;
  UINT8   MaxDq;

#if NIBBLE_TRAINING_SUPPORT == 0
  MaxDq = MrcData->MaxDq[Channel][Rank];
#else
  MaxDq = MrcData->MaxDqCache[Channel][Rank];
#endif

  TargetStrobe = Strobe;
  if (Strobe >= MAX_STROBE_X8_DEVICES) {
    TargetStrobe = Strobe - MAX_STROBE_X8_DEVICES;
  }

  GetSetDdrioGroup2 (MrcData, Channel, Rank, TargetStrobe, WrLvlSmpReg, CMD_GET_REG, &Nibble0);
  GetSetDdrioGroup2 (MrcData, Channel, Rank, TargetStrobe + 9, WrLvlSmpReg, CMD_GET_REG, &Nibble1);

  if (MaxDq <= MAX_STROBE_X8_DEVICES) {
    *Value = Nibble0 | Nibble1;
  } else if (Strobe < MAX_STROBE_X8_DEVICES) {
    *Value = Nibble0;
  } else {
    *Value = Nibble1;
  }

  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetTxDqDelayGExt (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{
#if NIBBLE_TRAINING_SUPPORT == 0
  GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, 0xFF, TxDqDelay,    Cmd, Value);
  GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, 0xFF, TxDqDrvDelay, Cmd, Value);

  if (MrcData->MaxDq[Channel][Rank] <= 9) {
    GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe + 9, Bit, FrequencyIndex, 0xFF, TxDqDelay,    Cmd, Value);
    GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe + 9, Bit, FrequencyIndex, 0xFF, TxDqDrvDelay, Cmd, Value);
  }
#endif
  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetWrLvlDelay (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{
#if NIBBLE_TRAINING_SUPPORT == 0

  GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, 0xFF, TxDqsDelay,   Cmd, Value);
  GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, 0xFF, TxDqDelay,    Cmd, Value);
  GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe, Bit, FrequencyIndex, 0xFF, TxDqDrvDelay, Cmd, Value);

  if (MrcData->MaxDq[Channel][Rank] <= 9) {
    GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe + 9, Bit, FrequencyIndex, 0xFF, TxDqsDelay,   Cmd, Value);
    GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe + 9, Bit, FrequencyIndex, 0xFF, TxDqDelay,    Cmd, Value);
    GetSetDdrioGroup (MrcData, Socket, Channel, Dimm, Rank, Strobe + 9, Bit, FrequencyIndex, 0xFF, TxDqDrvDelay, Cmd, Value);
  }
#endif
  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetRxDqsTether (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{

  GetSetDdrioGroup2 (
    MrcData,
    Channel, Rank, Strobe,
    RxDqsPDelay,
    Cmd,
    Value
  );

  if (Cmd == CMD_GET_REG) {
    return MMRC_SUCCESS;
  }

  if (Cmd == CMD_SET_VAL_FC_UC) {
    *Value += MrcData->RxDqsTetherVal[Strobe];
  }

  GetSetDdrioGroup2 (
    MrcData,
    Channel, Rank, Strobe,
    RxDqsNDelay,
    Cmd,
    Value
  );

  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetCmdVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{
  if ((Cmd & RD_REG) != 0) {
    *Value = MrcData->MrcParameters.SaveRestore.CmdVrefWrite[Channel][0][0];
  } else {
    WriteCmdVref (MrcData, Channel, *Value);
    MrcData->MrcParameters.SaveRestore.CmdVrefWrite[Channel][0][0] = (UINT8)*Value;
  }

  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetTxEq (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{
  if ((Cmd & RD_REG) != 0) {
    //
    // If RD command, restore Txeq Value for the channel
    //
    *Value = MrcData->MrcParameters.SaveRestore.Txeq[Channel];
  } else {
    if ((*Value <= 0) || (*Value >= 9)) {
      MrcData->MspData.DynamicVars[Channel][TX_EQ_EN] = 0;
      MrcData->MspData.DynamicVars[Channel][TX_EQ_COEF] = 0;
    } else {
      MrcData->MspData.DynamicVars[Channel][TX_EQ_EN] = 1;
      MrcData->MspData.DynamicVars[Channel][TX_EQ_COEF] = *Value -1;
      MrcData->MrcParameters.SaveRestore.Txeq[Channel] = *Value;
    }
    MmrcExecuteTask (MrcData, TxEqSet, NO_PRINT, Channel);
  }

  return MMRC_SUCCESS;
}
MMRC_STATUS
GetSetCtle (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{
  UINT32 Bias = 0;
  UINT32 Cap = 0;
  UINT32 Index = *Value;
  UINT32 Ctle_enabled = 0;
  UINT32 TempValue = 0;
  if ((Cmd & RD_REG) != 0) {

    if ((*Value) == 0) {
      *Value = MrcData->MrcParameters.SaveRestore.CtleBias[Channel][Strobe];
    } else {
      *Value = MrcData->MrcParameters.SaveRestore.CtleCap[Channel][Strobe];
    }


  } else {

    if (((*Value) >= 0) &&  ((*Value) <= 0x7)){
      GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Ctle_bias, CMD_SET_VAL_FC_UC, &Index);
      GetSetDdrioGroup2(MrcData, Channel, Rank, Strobe, Ctle_bias, CMD_GET_REG, &Bias);
    } else if ((*Value) == 0x8){

      //modify ampoffset
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        //
        // Check if this is for the VOC sweep, if so, then we need to set all the VOC Vrefs, where were not set in the create1dsweep
        // call.
        //
        for (Bit = 0; Bit < STROBE_BITS_X4_DEVICES; Bit++) {
#ifdef RxVocVal0
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVocVal0+Bit, CMD_GET_REG, &TempValue);
          if ((TempValue >= 0) && (TempValue <= 1)) {
          TempValue = 5;
          } else if ((TempValue >= 2) && (TempValue <= 3)) {
            TempValue = 6;
          } else if ((TempValue >= 4) && (TempValue <= 5)) {
            TempValue = 7;
          } else if ((TempValue >= 6) && (TempValue <= 9)) {
            TempValue = 8;
          } else if ((TempValue >= 10) && (TempValue <= 11)) {
            TempValue = 9;
          } else if ((TempValue >= 12) && (TempValue <= 13)) {
            TempValue = 10;
          } else if ((TempValue >= 14) && (TempValue <= 15)) {
            TempValue = 11;
          }
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVocVal0+Bit, CMD_SET_VAL_FC_UC, &TempValue);
          GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, RxVocVal0+Bit, CMD_GET_REG, &TempValue);
#endif
        }
        //  MspDelay (MICRO_DEL, 1);
      }
      Ctle_enabled = 1;
      Bias = 0;
      Cap = 0;
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Ctle_bias, CMD_SET_VAL_FC_UC, &Bias);
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Ctle_en, CMD_SET_VAL_FC_UC, &Ctle_enabled);
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Ctle_cap, CMD_SET_VAL_FC_UC, &Cap);

    } else if (((*Value) > 0x8) && ((*Value) <= 0x27)) {
      Bias = ((*Value-8)/4);
      Cap = ((*Value-8)%4);
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Ctle_bias, CMD_SET_VAL_FC_UC, &Bias);
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, Ctle_cap, CMD_SET_VAL_FC_UC, &Cap);
    }
   // MrcData->MspData.DebugMsgLevel = BackupMsg;
  }

  return MMRC_SUCCESS;
}

MMRC_STATUS
GetSetTxSwingB (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
)
{
  UINT32 TempValue;
  if ((Cmd & RD_REG) != 0) {
    // ToDo do we need to do something here?
  } else {
    if ((*Value) < 0x40) {
      MrcData->MspData.DynamicVars[Channel][TX_SWING_BIAS_CTL] = (((*Value) & 0x3F) >> 4);
      MrcData->MspData.DynamicVars[Channel][TX_IMODE_COEFF] = ((*Value) & 0xF);
      MmrcExecuteTask (MrcData, TxSwingBoostSet, NO_PRINT, Channel);
      //
      // Set the right bit using the enable for Dq and Dqs
      //
      TempValue = 1;
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxImodeDqsEn, CMD_SET_VAL_FC_UC, &TempValue);
      TempValue = 0xF;
      GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TxImodeDqEn, CMD_SET_VAL_FC_UC, &TempValue);
    }
  }
  return MMRC_SUCCESS;
}

#if ENABLE_CMD_OVERRIDE == 1

/**
  Access the Delay per Group, Rank, Strobe

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel being examined
  @param[in]       SweepAllGroups  if TRUE it examines all the Groups related with this Delay
  @param[in]       Delay           Delay to be incremented/decremented
  @param[in]       ComandType      Command to read/write from register/cache.
  @param[in]       DelayValue      Pointer to the Read/Write Value
  @param[in]       Offset          Value that delay needs to be incremented/decremented
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReadWriteDelay (
  IN    OUT       MMRC_DATA         *MrcData,
  IN              UINT8             Channel,
  IN              BOOLEAN           SweepAllGroups,
  IN              UINT16            Delay,
  IN              UINT8             ComandType,
  IN    OUT       UINT32            (*DelayValue)[MAX_RANKS][MAX_STROBES],
  IN    OUT       UINT32            OffsetValue
)
{
  UINT8   MaxGroups;
  UINT8   Rank;
  UINT8   Strobe;
  UINT8   GroupOffset;
  UINT8   GroupIndex;
  UINT16  TrainingDelay;

  TrainingDelay  = GetOffsetIndex (MrcData, Delay);
  if (SweepAllGroups) {
    MaxGroups = SignalInfo[GetSignalInfoIndex (TrainingDelay)].NumSignalGroups;
  } else {
    MaxGroups = 1;
  }

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    GroupOffset  = 0;
    for (GroupIndex = 0; GroupIndex < MaxGroups; GroupIndex++) {
      for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
        if (!IsStrobeValid(MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
          continue;
        }
        GetSetDdrioGroup2 (MrcData, Channel, Rank, Strobe, TrainingDelay + GroupOffset, ComandType, &DelayValue[GroupIndex][Rank][Strobe]);
        DelayValue[GroupIndex][Rank][Strobe] += OffsetValue;
      } // Strobe loop ...
      GroupOffset += SignalInfo[GetSignalInfoIndex (TrainingDelay + GroupOffset)].FrequencyOffset;
    } // GroupIndex loop ...
  } // Rank loop ...

  GroupOffset  = 0;
  for (GroupIndex = 0; GroupIndex < MaxGroups; GroupIndex++) {
    DisplayOrRestoreTrainingResults (MrcData, Channel, TrainingDelay + GroupOffset, FALSE);
    GroupOffset += SignalInfo[GetSignalInfoIndex (TrainingDelay + GroupOffset)].FrequencyOffset;
  } // GroupIndex loop ...

  return MMRC_SUCCESS;
}


/**
  Overrides an specific Delay

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel being examined
  @param[in]       SweepAllGroups  if TRUE it examines all the Groups related with this Delay
  @param[in]       Delay           Delay to be incremented/decremented
  @param[in]       Offset          Value that delay needs to be incremented/decremented
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
OverrideDelay (
  IN    OUT       MMRC_DATA         *MrcData,
  IN              UINT8             Channel,
  IN              BOOLEAN           SweepAllGroups,
  IN              UINT16            Delay,
  IN              UINT32            OffsetValue
)
{
  UINT32  DelayValue[MAX_GROUPS][MAX_RANKS][MAX_STROBES];
  UINT8   BackupMsg;

  BackupMsg     = MrcData->MspData.DebugMsgLevel;

  MrcData->MspData.DebugMsgLevel = 0;
  MspDebugPrint ((MSP_DBG_MIN, "\nDefault:\n"));
  ReadWriteDelay (MrcData, Channel, SweepAllGroups, Delay, CMD_GET_REG_UC,    DelayValue, OffsetValue);
  MspDebugPrint ((MSP_DBG_MIN, "\nOverride:\n"));
  ReadWriteDelay (MrcData, Channel, SweepAllGroups, Delay, CMD_SET_VAL_FC_UC, DelayValue, 0);
  if (GetOffsetIndex (MrcData, Delay) == GetOffsetIndex (MrcData, TxDqDelay)) {
    ReadWriteDelay (MrcData, Channel, SweepAllGroups, TxDqDrvDelay, CMD_SET_VAL_FC_UC, DelayValue, 0);
  }
  MrcData->MspData.DebugMsgLevel = BackupMsg;
  return MMRC_SUCCESS;
}
#endif

/**
  Overrides the settings for CMD/CTL/CLK

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CapsuleStartIndex 
  @param[in]       StringIndex 
  @param[in]       Channel 
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CmdOverrideSettings (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  )
{
#if ENABLE_CMD_OVERRIDE == 1
  OverrideDelay(MrcData, Channel, TRUE,  CmdGrp0, MrcData->OverrideClocks.CmdOffsetValue);
  OverrideDelay(MrcData, Channel, FALSE, CtlGrp0, MrcData->OverrideClocks.CsOffsetValue);
  OverrideDelay(MrcData, Channel, FALSE, CtlGrp1, MrcData->OverrideClocks.OdtOffsetValue);
  OverrideDelay(MrcData, Channel, FALSE, CtlGrp2, MrcData->OverrideClocks.CkeOffsetValue);
  OverrideDelay(MrcData, Channel, FALSE, CkAll,   MrcData->OverrideClocks.ClkOffsetValue);

  OverrideDelay(MrcData, Channel, FALSE, RecEnDelay,   MrcData->OverrideClocks.ClkOffsetValue);

  DnvNibbleTrainingHook (MrcData, 0, 0, Channel);
  OverrideDelay(MrcData, Channel, FALSE, TxDqsDelay,   MrcData->OverrideClocks.ClkOffsetValue);
  OverrideDelay(MrcData, Channel, FALSE, TxDqDelay,    MrcData->OverrideClocks.ClkOffsetValue);
//  OverrideDelay(MrcData, Channel, FALSE, TxDqDrvDelay, MrcData->OverrideClocks.ClkOffsetValue);
  DnvNibbleTrainingExitHook (MrcData, 0, 0, Channel);
#endif
  return MMRC_SUCCESS;
}

/**
DisableChannel sends the disabled unused channel to the deepest power management suspend state

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex
@param[in]       StringIndex
@param[in]       Channel
@retval          MMRC_SUCCESS
**/
MMRC_STATUS
DisableChannel (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
)
{
  REGISTER_ACCESS                Register;
  DSCH_DUNIT_COMMON_STRUCT       Dsch;
  DRAM_SR_CMD_DUNIT_STRUCT       DramSrCmd;
  DPMC0_DUNIT_STRUCT             Dpmc0;
  MISC_CHANNEL_CFG_DUNIT_STRUCT  MiscChannelCfg;

  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->MrcParameters.ChEnabled[Channel] == TRUE){
      Register.Offset = MISC_CHANNEL_CFG_DUNIT_REG;
      MiscChannelCfg.Data = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);
      MiscChannelCfg.Bits.channel_clkgtdis = 1;
      MemRegWrite (DUNIT, Channel, 0, Register, MiscChannelCfg.Data, 0xF);
      continue;
    }
    Register.Offset = DSCH_DUNIT_COMMON_REG;
    Dsch.Data = (UINT32)MemRegRead (DUNIT_COMMON, 0, 0, Register);
    Register.Offset = DRAM_SR_CMD_DUNIT_REG;
    DramSrCmd.Data = (UINT32)MemRegRead (DUNIT, Channel, 0, Register);
    Register.Offset = DPMC0_DUNIT_REG;
    Dpmc0.Data = (UINT32)MemRegRead (DUNIT, Channel, 0, Register);
    //
    // Dunit Wake
    //
    Dsch.Bits.coldwake = 1;
    Register.Offset = DSCH_DUNIT_COMMON_REG;
    MemRegWrite (DUNIT_COMMON, 0, 0, Register, Dsch.Data, 0xF);
    DramSrCmd.Bits.wake = 1;
    Register.Offset = DRAM_SR_CMD_DUNIT_REG;
    MemRegWrite(DUNIT, Channel, 0, Register, DramSrCmd.Data, 0xF);
    // Poll for DramSrCmd.Bits.wake == 0
    do {
      Register.Offset = DRAM_SR_CMD_DUNIT_REG;
      DramSrCmd.Data = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);
      MspDelay(NANO_DEL, 100);
    } while ((DramSrCmd.Bits.wake != 0) && (MrcData->MspData.CurrentPlatform[0] != P_SIMICS));

    Dsch.Bits.coldwake = 0;
    Register.Offset = DSCH_DUNIT_COMMON_REG;
    MemRegWrite(DUNIT_COMMON, 0, 0, Register, Dsch.Data, 0xF);
    //
    // Deepest PM code message
    //
    Dpmc0.Bits.susp_pmop = 6;
    Register.Offset = DPMC0_DUNIT_REG;
    MemRegWrite(DUNIT, Channel, 0, Register, Dpmc0.Data, 0xF);
    //
    // Suspend the unused Dunit
    //
    DramSrCmd.Bits.suspend = 1;
    Register.Offset = DRAM_SR_CMD_DUNIT_REG;
    MemRegWrite(DUNIT, Channel, 0, Register, DramSrCmd.Data, 0xF);
    // Poll for DramSrCmd.Bits.suspend == 0
    do{
      Register.Offset = DRAM_SR_CMD_DUNIT_REG;
      DramSrCmd.Data = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);
      MspDelay(NANO_DEL, 100);
    } while ((DramSrCmd.Bits.suspend != 0) && (MrcData->MspData.CurrentPlatform[0] != P_SIMICS));
  }

  return MMRC_SUCCESS;
}

/**
  Decreases the latency across ALL delay group to the minimum possible,
  in order to increase the performance

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel being examined
  @param[in]       MaxElements     Number of elements
  @param[in]       Delay           Delay Type
  @param[in]       DelayValue      Pointer to the Read Value
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetMin2xPIPerRank (
  IN    OUT       MMRC_DATA         *MrcData,
  IN              UINT8             Channel,
  IN              UINT8             Rank,
  IN              UINT8             MaxElements,
  IN              UINT8             *Name,
  IN              UINT16            Delay,
  IN    OUT       UINT32            *Minimum,
  IN    OUT       UINT32            *GlobalMinimum
)
{
  UINT8   Element;
  UINT8   MaxLength;
  UINT8   DelayIndex;
  UINT32  Value32;
  UINT32  LinearDelay;

  *Minimum  = 0xFFFFFFFF;
  MaxLength = 0;

  Delay = GetOffsetIndex (MrcData, Delay);

  Delay -= ALGO_REG_INDEX;
  Delay *= NumberOfElementsPerAlgo;

  while (Name != NULL && *Name != '\0') {
    if (*(Name + MaxLength) == '\0') {
      break;
    } else {
      MaxLength++;
    }
  }

  MaxLength = 9 - MaxLength;
  if (Name != NULL && *Name != '\0') {
    MspDebugPrint ((MSP_DBG_MIN, "%s", Name));
  }
  while (MaxLength--) {
    MspDebugPrint ((MSP_DBG_MIN, " "));
  }
  MspDebugPrint ((MSP_DBG_MIN, " "));

  for (Element = 0; Element < MaxElements; Element++) {
    //
    // Get the value of 2x and PI directly from the register,
    // exclude the CC region, so we can read the real minimum value
    //
    LinearDelay = 0;
    for (DelayIndex = 0; DelayIndex < NumberOfDelayElementsPerAlgo; DelayIndex++) {
      if (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][DelayIndex] > GetHalfClk(MrcData, Channel)) {
        continue;
      }
      if (GetSetDdrioGroup2 (MrcData, Channel, Rank, Element, Delay + DelayIndex, CMD_GET_REG, &Value32) == MMRC_SUCCESS) {
        LinearDelay += Value32 * (Granularity[ADdll[MrcData->DigitalDll]][MrcData->MspData.CurrentFrequency[Channel]][DelayIndex]);
      }
    } //  DelayIndex loop ...
    MspDebugPrint ((MSP_DBG_MIN, "%03d ", LinearDelay));
    if (LinearDelay < *Minimum) {
      *Minimum = LinearDelay;
    }
  }
  for (Element = MaxElements; Element < MAX_STROBES; Element++) {
    MspDebugPrint ((MSP_DBG_MIN, "--- "));
  }

  MspDebugPrint ((MSP_DBG_MIN, "%03d ", *Minimum));

  if (*Minimum < *GlobalMinimum) {
    *GlobalMinimum = *Minimum;
    MspDebugPrint ((MSP_DBG_MIN, "%03d Winner\n", *GlobalMinimum));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "%03d\n", *GlobalMinimum));
  }

  return MMRC_SUCCESS;
}


/**
  Decreases the latency across ALL delay group to the minimum possible,
  in order to increase the performance

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex
  @param[in]       StringIndex
  @param[in]       Channel
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PerformanceLatency (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  )
{
  UINT8     Loop;
  UINT8     Rank;
  UINT8     Strobe;
  UINT8     GroupOffset;
  UINT16    TrainingDelay;
  UINT32    Minimum;
  UINT32    GlobalMinimum;
  UINT8     BackupMsg;
  BOOLEAN   BackupEnable;

  BackupEnable  = MrcData->MspData.Enabled[0];
  GlobalMinimum = 0xFFFFFFFF;
  Minimum       = 0xFFFFFFFF;
  BackupMsg     = MrcData->MspData.DebugMsgLevel;

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!RunOnThisChannel(MrcData, Channel, Rank)) {
      continue;
    }
    MspDebugPrint ((MSP_DBG_MIN, "R%02d\n", Rank));
    MspDebugPrint ((MSP_DBG_MIN, "DelayType "));
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
      MspDebugPrint ((MSP_DBG_MIN, "S%02d ", Strobe));
    }
    MspDebugPrint ((MSP_DBG_MIN, "Min Glb\n"));

    GetMin2xPIPerRank (MrcData, Channel, Rank, MrcData->MaxDq[Channel][Rank], "RcvEn", RecEnDelay, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, MAX_STROBES, "TxDqs", TxDqsDelay, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, MAX_STROBES, "TxDq ", TxDqDelay, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, MAX_STROBES, "TxDrv", TxDqDrvDelay, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, 1, "CmdGrp", CmdGrp0, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, 1, "CsGrp", CtlGrp0, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, 1, "OdtGrp", CtlGrp1, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, 1, "CkeGrp", CtlGrp2, &Minimum, &GlobalMinimum);
    GetMin2xPIPerRank (MrcData, Channel, Rank, 1, "CkAll", CkAll, &Minimum, &GlobalMinimum);
  } // Rank loop ...
  MspDebugPrint ((MSP_DBG_MIN, "Pullin # x2/UI: %02d\n", GlobalMinimum / GetHalfClk (MrcData, Channel)));
  GlobalMinimum = (GlobalMinimum / GetHalfClk(MrcData, Channel)) * GetHalfClk(MrcData, Channel);
  MspDebugPrint ((MSP_DBG_MIN, "Pullin # ticks: %03d\n", GlobalMinimum));

  MrcData->OverrideClocks.CmdOffsetValue = 0 - GlobalMinimum;
  MrcData->OverrideClocks.CsOffsetValue  = 0 - GlobalMinimum;
  MrcData->OverrideClocks.OdtOffsetValue = 0 - GlobalMinimum;
  MrcData->OverrideClocks.CkeOffsetValue = 0 - GlobalMinimum;
  MrcData->OverrideClocks.ClkOffsetValue = 0 - GlobalMinimum;

  CmdOverrideSettings (MrcData, 0, 0, Channel);
  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  MrcData->MspData.DebugMsgLevel &= ~MSP_DBG_MAX;
  MrcData->MspData.Enabled[0] = TRUE;
  if (Channel == 0) {
    MmrcExecuteTask(MrcData, PerfLatencyDefaultsCh0, 0, 0);
  } else {
    MmrcExecuteTask(MrcData, PerfLatencyDefaultsCh1, 0, 0);
  }
  MrcData->MspData.Enabled[0] = BackupEnable;
  ReceiveEnableExit (MrcData, Channel);
  WriteTrainingExit (MrcData, Channel);
  MrcData->MspData.DebugMsgLevel = BackupMsg;
  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);

  AverageGroups (MrcData, Channel, RecEnDelay,   FALSE);
#if RDQS_ODDEVENSUPPORT==1
  AverageGroups (MrcData, Channel, RxDqsNDelay,  TRUE);
#else
  AverageGroups (MrcData, Channel, RxDqsDelay,   TRUE);
#endif
  AverageGroups (MrcData, Channel, TxDqsDelay,   FALSE);
  AverageGroups (MrcData, Channel, TxDqDelay,    FALSE);
#ifdef TxDqDrvDelay
  AverageGroups (MrcData, Channel, TxDqDrvDelay, FALSE);
#endif
  //
  // Print out or restore the training results.
  //
  TrainingDelay = GetOffsetIndex (MrcData, CmdGrp0);
  GroupOffset   = 0;
  for (Loop = 0; Loop < MAX_CMDS; Loop++) {
    DisplayOrRestoreTrainingResults (MrcData, Channel, TrainingDelay + GroupOffset, TRUE);
    GroupOffset += SignalInfo[GetSignalInfoIndex (TrainingDelay + GroupOffset)].FrequencyOffset;
  }
  DisplayOrRestoreTrainingResults (MrcData, Channel, CtlGrp0,  TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, CtlGrp1,  TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, CtlGrp2,  TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, CkAll,  TRUE);

  DisplayOrRestoreTrainingResults (MrcData, Channel, RecEnDelay, TRUE);
  DisplayOrRestoreTrainingResultsRxDqsWrapper(MrcData, Channel, TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqsDelay, TRUE);
  DisplayOrRestoreTrainingResults (MrcData, Channel, TxDqDelay,  TRUE);
  DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDrvDelay, FALSE);

  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  if (TestMrcTrainingSteps (MrcData, Channel, CPGC_CONFIG_VA, CPGC_CMDSEQ_WRITE_READ, 0, 8, 6) != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Training Failure, write soc.ddr.dunit0.skpd4=0 to continue\n"));
  }

  return MMRC_SUCCESS;
}
VOID
WriteDimmVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT32        vref,
  IN        UINT8         Override
)
{
  UINT32 txVref;
  UINT8  Length;

#if JTAG || SIM
  return;
#endif

  Length = 1;

  if (Override == OVERRIDE) {
    txVref = vref;
  } else {
    txVref = 0x40;
  }

  if (MrcData->OemMrcData.VrefSmbAddress[Channel] != 0xFF) {
    MrcSmbusExec (
      MrcData->SmbusBar,
      MrcData->OemMrcData.VrefSmbAddress[Channel],
      SmbusWriteByte,
      0,
      &Length,
      (UINT8 *) &txVref
      );
  }
  //
  // Add some time for Vref circuit to settle
  //
  MspDelay (NANO_DEL, 100000);
}

VOID
WriteCmdVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT32        vref
)
{
  UINT8  Length;

#if JTAG || SIM
  return;
#endif

  Length = 1;

  if (MrcData->OemMrcData.CmdVrefSmbAddress[Channel] != 0xFF) {
    MrcSmbusExec (
      MrcData->SmbusBar,
      MrcData->OemMrcData.CmdVrefSmbAddress[Channel],
      SmbusWriteByte,
      0,
      &Length,
      (UINT8 *) &vref
      );
  }
  //
  // Add some time for Vref circuit to settle
  //
  MspDelay (NANO_DEL, 100000);
}

/**
  Configures a checkpoint to halt the CPU in a specific memory 
  training step 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CheckPoint      Specifies the code related to 
                                   an MRC step
  @retval          NONE
**/
VOID
HandleCheckpoint (
  IN        MMRC_DATA *MrcData,
  IN        UINT8      CheckPoint
  )
{
  UINT32  CheckpointCode;
  REGISTER_ACCESS Register;

#if SIM
  MspDebugPrint ((MSP_DBG_MIN, "-%10dns, ", (MyGetTime ())/1000));
#endif
  MspDebugPrint ((MSP_DBG_MIN, "CP %02X\n", (CheckPoint & 0xFF)));
#if !defined JTAG
  if (CheckPoint != 0xFF) {
    CHECKPOINT (CheckPoint);
  }
#endif

  //
  // Bits[31:16] will work as monitoring register
  //
  Register.Offset = SSKPD1_DUNIT_COMMON_REG;
  Register.Mask   = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  CheckpointCode = (UINT32) MemRegRead (DUNIT_COMMON, 0, 0, Register);
  CheckpointCode &= ~0xFFFF0000;
  CheckpointCode |= ((CheckPoint) << 16);
  MemRegWrite (DUNIT_COMMON, 0, 0, Register, CheckpointCode, 0xF);
  //
  // This while-loop halts system as an special option to Enable Memory Configuration Override Checkpoint
  //
  CheckpointCode = (UINT32) MemRegRead (DUNIT_COMMON, 0, 0, Register);
  if (CheckPoint == (CheckpointCode & 0xFFFF) ) {
    MspDebugPrint ((MSP_DBG_MIN, "MRC Checkpoint is reached.... \n"));
    MspDebugPrint ((MSP_DBG_MIN, "Clear/Configure register in order to continue\n"));
  }
  while (CheckPoint == (CheckpointCode & 0xFFFF) ) {
    CheckpointCode = (UINT32) MemRegRead (DUNIT_COMMON, 0, 0, Register);
  }

}
#if EARLY_READ_TRAINING || RECEIVE_ENABLE
/**
  Toggle the MPR mode for an specific rank 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Toggle          Value for the MRS 3 command.
  @retval          NONE
**/
VOID
ToggleMprMode (
  IN  OUT   MMRC_DATA       *MrcData,
  IN        UINT8           Channel,
  IN        UINT8           Rank,
  IN        UINT8           Toggle
  )
{
  DramInitDDR3EMR3    Mrs3Command;
  DramInitDDR4EMR3    Mrs3Ddr4Command;
  BOOLEAN             ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;
  
  //
  // Enable MPR mode
  //
  if (Toggle == 1) {
    MmrcExecuteTask (MrcData, ENABLE_MPR_MODE, NO_PRINT, Channel);
  }

  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    Mrs3Ddr4Command.Data =  ReadMrsCommand (MrcData, Channel, Rank, 3);
    Mrs3Ddr4Command.Bits.MPR = Toggle; 
    WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs3Ddr4Command.Data);
  } else {
    Mrs3Command.Data = ReadMrsCommand (MrcData, Channel, Rank, 3);
    Mrs3Command.Bits.MPR = Toggle;
    WriteDramCommandSideWrap (MrcData, Channel, Rank, Mrs3Command.Data);
  }

  //
  // Disable MPR mode
  //
  if (Toggle == 0) {
    MmrcExecuteTask (MrcData, DISABLE_MPR_MODE, NO_PRINT, Channel);
  }

  MspDelay (NANO_DEL, 10000);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
}
#endif // EARLY_READ_TRAINING


#if EARLY_READ_TRAINING || RECEIVE_ENABLE
/**
  Toggle the MPR mode for an specific rank side

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Toggle          Value for the MRS 3 command.
  @param[in]       Side            Side
  @retval          NONE
**/
VOID
ToggleMprModeSide (
  IN  OUT   MMRC_DATA       *MrcData,
  IN        UINT8           Channel,
  IN        UINT8           Rank,
  IN        UINT8           Toggle,
  IN        UINT8           Side
  )
{

  DramInitDDR4EMR3    Mrs3Ddr4Command;
  BOOLEAN             ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  //
  // Enable MPR mode
  //
  if (Toggle == 1) {
    MmrcExecuteTask (MrcData, ENABLE_MPR_MODE, NO_PRINT, Channel);
  }

    Mrs3Ddr4Command.Data =  ReadMrsCommand (MrcData, Channel, Rank, 3);
    Mrs3Ddr4Command.Bits.MPR = Toggle;
    WriteDramCommand(MrcData, Channel, Rank, Mrs3Ddr4Command.Data, Side);

  //
  // Disable MPR mode
  //
  if (Toggle == 0) {
    MmrcExecuteTask (MrcData, DISABLE_MPR_MODE, NO_PRINT, Channel);
  }

  MspDelay (NANO_DEL, 10000);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
}
#endif // EARLY_READ_TRAINING

/**
  Mirrors address bits

  @param[in]       data         Data for MRS write 
  @param[in]       DramType     Dram Type 
  @retval                       Mirrored data
**/
UINT32
MirrorAddrBits (
  IN    UINT32  data,
  IN    UINT8  DramType
  )
{
  UINT32  dataNew;

  if (DramType <= TypeDdr3All) {
    dataNew = (data & ~(0x1F8));
  } else {
    dataNew = (data & ~(0x29F8));
  }
  //
  // Swap of bits (3,4), (5,6), (7,8)
  //
  dataNew |= ((data & BIT3) << 1) | ((data & BIT4) >> 1) | ((data & BIT5) << 1) | ((data & BIT6) >> 1) | ((data & BIT7) << 1) | ((data & BIT8) >> 1);

  //
  // Swap (11,13) if the DIMM is DDR4
  //
  if (DramType == TypeDdr4) {
    dataNew |= ((data & BIT11) << 2) | ((data & BIT13) >> 2);
  }

  return dataNew;
} 

/**
  Mirrors bank address bits

  @param[in]       bank         Bank address to be mirrored 
  @retval                       Mirrored bank address
**/
UINT8
MirrorBankBits (
  UINT8 bank
  )
{
  UINT8 bankNew;

  bankNew = bank &~0x3;
  //
  // Swap bits 0 and 1 of the bank address
  //
  bankNew = ((bank & BIT0) << 1) | ((bank & BIT1) >> 1);

  return bankNew;
}

VOID
HandleAddressMirroring (
  IN  OUT   UINT32 *Data,
  IN        UINT8   AddressMirroringSpdByte,
  IN        UINT8   DramDeviceSpdByte,
  IN        UINT8   DramType, 
  IN        UINT8   DimmType,
  IN        UINT8   Side
  )
{
  DramInitMisc     MiscCommandDdr3;
  DramInitDDR4Misc MiscCommandDdr4;
  UINT32           dataNewInv;
  UINT8            rankSelect;

  MiscCommandDdr3.Data = *Data;
  MiscCommandDdr4.Data = *Data;
  if (DramType <= TypeDdr3All) {
    rankSelect = (UINT8) MiscCommandDdr3.Bits.rankSelect;
  } else {
    rankSelect = (UINT8) MiscCommandDdr4.Bits.rankSelect;
  }

  if (AddressMirroringSpdByte & 1) {
    if (rankSelect & BIT0) {
      if (DramType <= TypeDdr3All) {
        MiscCommandDdr3.Bits.multAddress  = (UINT16) MirrorAddrBits ((UINT32) MiscCommandDdr3.Bits.multAddress, DramType);
        MiscCommandDdr3.Bits.bankAddress  = (UINT8) MirrorBankBits ((UINT8) MiscCommandDdr3.Bits.bankAddress);
      } else {
        MiscCommandDdr4.Bits.MemoryAddress  = (UINT32) MirrorAddrBits ((UINT32) MiscCommandDdr4.Bits.MemoryAddress, DramType);
        MiscCommandDdr4.Bits.bankAddress  = (UINT8) MirrorBankBits ((UINT8) MiscCommandDdr4.Bits.bankAddress);
        //
        // x16 only have BG0, don't mirror it
        //
        if ((DramDeviceSpdByte & SPD_DDR4_ORG_WIDTH_MASK) != SPD_DDR4_ORG_WIDTH_X16)
          MiscCommandDdr4.Bits.bankGroup    = (UINT8) MirrorBankBits ((UINT8) MiscCommandDdr4.Bits.bankGroup);
      }
    }
  }

  //
  // Address Inversion for DDR4 Rdimms
  //
  if ((DramType == TypeDdr4) && (DimmType == SPD_DDR_RDIMM) && (Side == SIDE_B)) {
    dataNewInv = 0;
    dataNewInv = ((MiscCommandDdr4.Bits.MemoryAddress & 0x1D407) | ((~MiscCommandDdr4.Bits.MemoryAddress) & (~0x1D407))) & 0x3FFFF;
    MiscCommandDdr4.Bits.MemoryAddress = dataNewInv;
    dataNewInv = 0;
    dataNewInv = ~MiscCommandDdr4.Bits.bankAddress & 0x3;
    MiscCommandDdr4.Bits.bankAddress = dataNewInv;
    dataNewInv = 0;
    dataNewInv = ~MiscCommandDdr4.Bits.bankGroup & 0x3;
    MiscCommandDdr4.Bits.bankGroup = dataNewInv;
  }

  if (DramType <= TypeDdr3All) {
    *Data = MiscCommandDdr3.Data;
  } else {
    *Data = MiscCommandDdr4.Data;
  }
}

VOID
MirrorMrsAndAssignToDynamicVars (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT32        MiscCommand,
  IN        UINT16        DynamicVar,
  IN        UINT8         Side
  )
{
  ConvertMrsDataToMirrored(MrcData, Channel, Rank, &MiscCommand, Side);
  MrcData->MspData.DynamicVars[Channel][DynamicVar] = MiscCommand;
}

VOID
WriteDramCommandSideWrap(
IN  OUT   MMRC_DATA     *MrcData,
IN        UINT8         Channel,
IN        UINT8         Rank,
IN        UINT32        MiscCommand
)
{
  WriteDramCommand(MrcData, Channel, Rank, MiscCommand, SIDE_A);
  //
  // Rdimm's side B
  //
  if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
    WriteDramCommand(MrcData, Channel, Rank, MiscCommand, SIDE_B);
  }
}

VOID
WriteDramCommand (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT32        MiscCommand,
  IN        UINT8         Side
  )
{
  BOOLEAN           ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  MirrorMrsAndAssignToDynamicVars(MrcData, Channel, Rank, MiscCommand, REG_DATA, Side);
  MmrcExecuteTask (MrcData, SEND_DRAM_INIT, NO_PRINT, Channel);

#if SIM
  MySimStall (50);
#else
  MrcDelay (NANO_DEL, 50);
#endif

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
}

VOID
ConvertMrsDataToMirrored(
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN  OUT   UINT32        *MiscCommand,
  IN        UINT8         Side
  )
{
  UINT8             DramType;
  UINT8             DimmType;
  UINT8             Dimm;
  UINT8             SpdAddMap;
  UINT8             SpdDeviceOrg;
  BOOLEAN           ExecuteMode;

  if(Rank >= MAX_RANKS){
    MrcData->ErrorCode = MrcErrParameterOutOfBound;
    MrcDeadLoop();
    return;
  }

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  if (Rank > 1) {
    Dimm = 1;
  } else {
    Dimm = 0;
  }
  DramType  = MrcData->MrcParameters.DramType[Channel];
  DimmType  = MrcData->D_Type[Channel][Dimm];
  if (DramType <= TypeDdr3All) {
    SpdAddMap = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR3_ADD_MAPPING];
    SpdDeviceOrg = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR3_ORG];
  } else {
    SpdAddMap = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR4_ADD_MAPPING];
    SpdDeviceOrg = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR4_ORG];
  }

  HandleAddressMirroring (MiscCommand, SpdAddMap, SpdDeviceOrg, DramType, DimmType, Side);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
}

UINT32
ReadMrsCommand (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Index
  )
{
  DramInitMisc      MiscCommand;
  BOOLEAN           ExecuteMode;

  ExecuteMode = MrcData->MspData.ExecuteThisRoutineInParallel;
  MrcData->MspData.ExecuteThisRoutineInParallel = FALSE;

  MiscCommand.Data = (UINT32) MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][Index]];
  ConvertMrsDataToMirrored(MrcData, Channel, Rank, &MiscCommand.Data, SIDE_A);

  MrcData->MspData.ExecuteThisRoutineInParallel = ExecuteMode;
  return MiscCommand.Data;
}

VOID
DisablePatrolDemandScrubbing(
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  MrcData->MspData.DynamicVars[Channel][PATROL_SCRUB] = 0;
  MrcData->MspData.DynamicVars[Channel][DEMAND_SCRUB] = 0;
  MmrcExecuteTask (MrcData, EnableScrub, NO_PRINT, Channel);
}

VOID
RestorePatrolDemandScrubbing (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  if (MrcData->PatrolScrubEnable) {
    MrcData->MspData.DynamicVars[Channel][PATROL_SCRUB] = 1;
    MrcData->MspData.DynamicVars[Channel][DEMAND_SCRUB] = 1;
  } else {
    MrcData->MspData.DynamicVars[Channel][PATROL_SCRUB] = 0;
    MrcData->MspData.DynamicVars[Channel][DEMAND_SCRUB] = MrcData->DemandScrub;
  }
  MmrcExecuteTask (MrcData, EnableScrub, NO_PRINT, Channel);
}

VOID
UpdateSelfRefreshDelay (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINTX         NewDelay
  )
{
  MrcData->MspData.DynamicVars[Channel][SR_DELAY] = NewDelay;
  MmrcExecuteTask (MrcData, ProgDunitPerfPowerSettings, NO_PRINT, Channel);
}

UINT32
GetAddress (
  MMRC_DATA            *MrcData, 
  UINT8                Channel, 
  UINT8 Rank
  )
{
  return (UINT32)(Rank << RANK_SHIFT_BIT);
}


UINT32
GetAddressBit (
  IN  OUT   MMRC_DATA     *MrcData, 
  IN        UINT8         Channel, 
  IN        UINT8         Rank
  ) 
{
  return RANK_SHIFT_BIT;
}

VOID
CpgcOptionsSetup (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  MrcData->CpgcOptions.BusWidth   = 1; 
  MrcData->CpgcOptions.MinRankBit = RANK_SHIFT_BIT;
  if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    MrcData->CpgcOptions.MinBankBit = BANK_SHIFT_BIT_DDR3;
  } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    MrcData->CpgcOptions.MinBankBit = BANK_SHIFT_BIT_DDR4;
  }
}

/**
  Output a progress data to port 80 for debug purpose
  Could be safely overridden to send checkpoints elsewhere, such
  as port 0x84 or a serial port 

  @param[in]    Content     Hexadecimal value to be sent thru 
                            debug port
  @retval                   NONE
**/
VOID
CheckpointSet (
  IN        UINT16    Content
  )
{
  IoOut32 (0x80, Content);
}

MMRC_STATUS
DumpDdrioRegisters (
  IN  OUT   MMRC_DATA *MrcData,
  IN        UINT8      Channel
  )
{
  UINT16 Offset;
  UINT32 Value;
  UINT8  Dq;
  REGISTER_ACCESS Register;

  Register.Mask   = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  
  MspDebugPrint ((MSP_DBG_MIN, "DDRIO CH%d\n", Channel));
  //
  // DDRDQ
  //
  for (Dq = 0; Dq < 9; Dq ++) {
  for (Offset = 0; Offset < 0x634; Offset += 4) {
    if (Offset == 0x30) Offset = 0x100;
    if (Offset == 0x144) Offset = 0x180;
    if (Offset == 0x22C) Offset = 0x280;
    if (Offset == 0x288) Offset = 0x2B0;
    if (Offset == 0x224) Offset = 0x228;
    if (Offset == 0x388) Offset = 0x3B0;
    if (Offset == 0x454) Offset = 0x458;
    if (Offset == 0x45C) Offset = 0x4B0;
    if (Offset == 0x4B8) Offset = 0x4E0;
    if (Offset == 0x5B8) Offset = 0x5E0;
    Register.Offset = Offset;
      Value = (UINT32) MemRegRead (DDRDQ, Channel, Dq, Register);
      MspDebugPrint((MSP_DBG_MIN, "DQ%d 0x%08x 0x%08x\n", Dq, Register.Offset, Value));
  }
  }
  //
  // DDRPLL
  //
  for (Offset = 0; Offset < 0x38; Offset += 4) {
    if (Offset == 0x1c) Offset = 0x20;
    if (Offset == 0x28) Offset = 0x2c;
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (DDRPLL, Channel, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "PLL 0x%08x 0x%08x\n", Register.Offset, Value));
  }
  //
  // DDRCC0
  //
  for (Offset = 0; Offset < 0x62C; Offset += 4) {
    if (Offset == 0x30) Offset = 0x100;
    if (Offset == 0x120) Offset = 0x180;
    if (Offset == 0x1B0) Offset = 0x280;
    if (Offset == 0x288) Offset = 0x3B0;
    if (Offset == 0x3E0) Offset = 0x4B0;
    if (Offset == 0x4B8) Offset = 0x5E0;
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (DDRCC0, Channel, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "CC0 0x%08x 0x%08x\n", Register.Offset, Value));
  }
  //
  // DDRCC1
  //
  for (Offset = 0; Offset < 0x708; Offset += 4) {
    if (Offset == 0x8C) Offset = 0x100;
    if (Offset == 0x130) Offset = 0x180;
    if (Offset == 0x1A8) Offset = 0x280;
    if (Offset == 0x288) Offset = 0x2B0;
    if (Offset == 0x2F8) Offset = 0x3B0;
    if (Offset == 0x3D8) Offset = 0x4B0;
    if (Offset == 0x4B8) Offset = 0x4E0;
    if (Offset == 0x528) Offset = 0x5E0;
    if (Offset == 0x618) Offset = 0x61c;
    if (Offset == 0x62C) Offset = 0x6E0;
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (DDRCC1, Channel, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "CC1 0x%08x 0x%08x\n", Register.Offset, Value));
  }
  //
  // DDRCC2
  //
  for (Offset = 0; Offset < 0x62C; Offset += 4) {
    if (Offset == 0x34) Offset = 0x100;
    if (Offset == 0x120) Offset = 0x180;
    if (Offset == 0x1B0) Offset = 0x280;
    if (Offset == 0x288) Offset = 0x3B0;
    if (Offset == 0x3E0) Offset = 0x4B0;
    if (Offset == 0x4B8) Offset = 0x5E0;
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (DDRCC0, Channel, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "CC2 0x%08x 0x%08x\n", Register.Offset, Value));
  }

  return MMRC_SUCCESS;  
}

MMRC_STATUS
DumpDunitRegisters (
  IN  OUT   MMRC_DATA *MrcData,
  IN        UINT8      Channel
  )
{
  UINT16 Offset;
  UINT32 Value;
  REGISTER_ACCESS Register;

  Register.Mask   = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  if (Channel == 0) {
    MspDebugPrint((MSP_DBG_MIN, "DUNIT_COMMON%d\n", Channel));
    for (Offset = 0; Offset < 0x140; Offset += 4) {
      if (Offset == 0x28) Offset = 0x30;
      if (Offset == 0x40) Offset = 0x80;
      if (Offset == 0xC8) Offset = 0x100;
      Register.Offset = Offset;
      Value = (UINT32)MemRegRead(DUNIT_COMMON, Channel, 0, Register);
      MspDebugPrint((MSP_DBG_MIN, "0x%08x 0x%08x\n", Register.Offset, Value));
    }
  }
  MspDebugPrint((MSP_DBG_MIN, "DUNIT%d\n", Channel));
  for (Offset = 0x3C; Offset < 0x1A0; Offset += 4) {
    if (Offset == 0x40) Offset = 0x44;
    if (Offset == 0x50) Offset = 0x54;
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (DUNIT, Channel, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "0x%08x 0x%08x\n", Register.Offset, Value));
  }
  return MMRC_SUCCESS;  
}

MMRC_STATUS
DumpCpgcRegisters (
  IN  OUT   MMRC_DATA *MrcData,
  IN        UINT8      Channel
  )
{
  UINT16 Offset;
  UINT32 Value;
  REGISTER_ACCESS Register;

  Register.Mask   = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  
  MspDebugPrint ((MSP_DBG_MIN, "CPGC%d\n", Channel));

  for (Offset = 0x40; Offset < 0x364; Offset += 4) {
    if (Offset == 0x5C) Offset = 0x80;
    if (Offset == 0x1A8) Offset = 0x1AC;
    if (Offset == 0x1DC) Offset = 0x200;
    if (Offset == 0x2A8) Offset = 0x2AC;
    if (Offset == 0x308) Offset = 0x314;
    if (Offset == 0x32C) Offset = 0x35C;
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (CPGC, Channel, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "0x%08x 0x%08x\n", Register.Offset, Value));
  }
  return MMRC_SUCCESS;  
}

MMRC_STATUS
DumpBunitRegisters (
  IN  OUT   MMRC_DATA *MrcData
  )
{
  UINT16 Offset;
  UINT32 Value;
  REGISTER_ACCESS Register;

  Register.Mask   = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  
  MspDebugPrint ((MSP_DBG_MIN, "BUNIT\n"));

  for (Offset = 0; Offset < 0xFF; Offset++) {
    Register.Offset = Offset;
    Value = (UINT32) MemRegRead (BUNIT, 0, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "0x%08x 0x%08x\n", Register.Offset, Value));
  }
  return MMRC_SUCCESS;  
}

MMRC_STATUS
DumpRegs (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 i;
  DumpBunitRegisters (MrcData);
  for (i = 0; i < MAX_DUNITS; i++)
  {
    if (MrcData->MrcParameters.ChEnabled[i])
    {
      DumpDunitRegisters (MrcData, i);
    }
  }
  for (i = 0; i < MAX_CHANNELS; i++) {
    if (MrcData->MrcParameters.ChEnabled[i]) {
      DumpDdrioRegisters (MrcData, i);
      DumpCpgcRegisters (MrcData, i);
    }
  }
  return MMRC_SUCCESS;
}

MMRC_STATUS
BreakStrobeLoop (
  IN   MMRC_DATA    *MrcData,
  IN   UINT16       Dim1Index,
  IN   UINT8        *Strobe
  )
{
#if TRAINING_ALGOS == 1
  //
  // For the CMD group signals break right at the first strobe
  //
  if (Dim1Index >= GSM_GT_INDEX) {
    if ((IsCmdSignalDelay (MrcData, Dim1Index) || Dim1Index == TxVref || IsCtlSignalDelay(MrcData, Dim1Index) || GetOffsetIndex (MrcData, Dim1Index) == GetOffsetIndex (MrcData, CkAll)) && (*Strobe) >= 0) {
      *Strobe = MAX_STROBES;
    }
  } else {
    if ((IsCmdSignalDelay (MrcData, Dim1Index) || Dim1Index == MmrcTxVref || IsCtlSignalDelay(MrcData, Dim1Index) || GetOffsetIndex (MrcData, Dim1Index) == GetOffsetIndex (MrcData, CkAll)) && (*Strobe) >= 0) {
      *Strobe = MAX_STROBES;
    }
  }

  //
  // Different projects can add different conditions per delay that uses the same per strobe loop 
  //
#endif
  return MMRC_SUCCESS;
}

UINT32
SignalUpperBound (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT16       Dim1Index
  )
{
#if TRAINING_ALGOS == 1
  UINT32    WaitTime;
  UINT16    Dim1IndexBackup;
  UINT32    MaxLimit;

  MaxLimit = 0;
  if (Dim1Index == TxDqDelayG) {
    Dim1Index = TxDqDelay2;
  }
  Dim1IndexBackup = Dim1Index;
  Dim1Index = GetOffsetIndex (MrcData, Dim1Index);

  if (Dim1Index > 0) {
    GetDdrioGroupLimits (MrcData, 0, Channel, 0, 0, Dim1Index, &MaxLimit, &WaitTime);
  } else {
    MspDebugPrint((MSP_DBG_MIN, "ERROR! improper value for Dim1Index.\n"));
    MrcDeadLoop();
  }
  //
  // Return the linear value as maximum in case it exists
  //
  switch (Dim1IndexBackup) {
  case TxVref:
    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    MaxLimit = ConvertPhysicalToLinearValue(MrcData, Channel, Dim1IndexBackup, MaxLimit);
    } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      MaxLimit = 72;
    }
    break;
  default:
    break;
  }
  return MaxLimit;
#else
  return 0;
#endif
}

#if PASS_GATE_TEST == 1
/**
  Returns back the mirrored address

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current channel examined 
  @param[in]       Address         Address to be mirrored
  @retval          Mirrored Address 
**/
UINT32
MirrorAddrBitsPassGate (
  IN          MMRC_DATA     *MrcData,
  IN          UINT8         Channel,
  IN          UINT32        Address
  )
{
  return MirrorAddrBits (Address, MrcData->MrcParameters.DramType[Channel]);
} 
/**
  Delay the execution for X number of activates and returns back 
  the number of activates, reads and write 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current channel examined 
  @param[in]       Activations     Number of activates desired 
        per Row
  @param[in, out]  ReadCounter     Reads executed
  @param[in, out]  WriteCounter    Writes executed
  @param[in, out]  ActivateCounter Activates executed
  @retval          None
**/
VOID
DelayForXActivates (
  IN          MMRC_DATA     *MrcData,
  IN          UINT8         Channel,
  IN          UINT32        Activations,
  IN          UINT32        *ReadCounter,
  IN          UINT32        *WriteCounter,
  IN          UINT32        *ActivateCounter
  )
{
  UINT32    ActivatesIn1xSpeed;
  *ActivateCounter = 0;
  *ReadCounter     = 0;
  *WriteCounter    = 0;

  while (1) {
#if SIM
    //
    // Stall between polling passes in simulation so we don't slow down the
    // simulator with a bunch of read requests.
    //
    MySimStall (10);
#else
    //
    // Read the rapl counter every 100 ACTs
    // 100 * 100ns (each act per row) = 10us
    //
    MspDelay (NANO_DEL, 10000);
#endif
    MmrcExecuteTask (MrcData, PASS_GATE_READ_RAPL_COUNTERS, NO_PRINT, Channel);
    *ActivateCounter += ((UINT32) MrcData->MspData.DynamicVars[Channel][PG_ACTIVATE_CNT]);
    *ReadCounter     += ((UINT32) MrcData->MspData.DynamicVars[Channel][PG_READ_CNT]);
    *WriteCounter    += ((UINT32) MrcData->MspData.DynamicVars[Channel][PG_WRITE_CNT]);

    ActivatesIn1xSpeed  = *ActivateCounter / MrcData->PassGateParameters.SpeedSelect;
    if (ActivatesIn1xSpeed >= (Activations - MrcData->PassGateParameters.ActivateAdjust)) {
      break;
    }
  }
}

/**
  Read the MrcData structure and gets the information related 
  with the Dimm 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in, out]  Channel         Current channel examined
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
GetDimmGeometry (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  UINT8   Dimm;
  UINT8   DimmDeviceDensity;
  UINT8   DimmDeviceWidth;
  //
  // Determine if Swizzle or Sequential mode in each Dimm
  // Determine if both Dimms have the same Data Width and Device Density
  //
  MrcData->PassGateParameters.DeviceDataWidth = 0xff;
  MrcData->PassGateParameters.DeviceDensity   = 0xff;
  DimmDeviceWidth     = 0xff;
  DimmDeviceDensity   = 0xff;
  
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    MrcData->PassGateParameters.DimmSwizzle[Dimm] = MrcData->PassGateParameters.SwizzleMode;
    if (MrcData->PassGateParameters.SamsungDram[Channel][Dimm] && MrcData->PassGateParameters.SwizzleMode == 0) {
      MrcData->PassGateParameters.DimmSwizzle[Dimm] = 1;
    }
    if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
      continue;
    }
    DimmDeviceDensity = MrcData->D_Size[Channel][Dimm] + 1;
    DimmDeviceWidth   = MrcData->D_DataWidth[Channel][Dimm];

    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      //
      // DDR3
      // 
      MrcData->PassGateParameters.DimmMirrored[Dimm]  = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR3_ADD_MAPPING] & 1;
    } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      //
      // DDR4
      //
      MrcData->PassGateParameters.DimmMirrored[Dimm]  = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ADD_MAPPING] & 1;
    }
    if (MrcData->PassGateParameters.DeviceDataWidth == 0xff) {
      MrcData->PassGateParameters.DeviceDataWidth = DimmDeviceWidth;
    } else if (MrcData->PassGateParameters.DeviceDataWidth != DimmDeviceWidth) {
      MspDebugPrint ((MSP_DBG_MIN, "All Dimms must be the same Data Width\n"));
      return MrcErrSetupErrorPassGate;
    } 

    if (MrcData->PassGateParameters.DeviceDensity == 0xff) {
      MrcData->PassGateParameters.DeviceDensity = DimmDeviceDensity;
    } else if (MrcData->PassGateParameters.DeviceDensity != DimmDeviceDensity) {
      MspDebugPrint ((MSP_DBG_MIN, "All Dimms must be the same Density\n"));
      return MrcErrSetupErrorPassGate;
    } 
  } // Dimm loop ...

  return MMRC_SUCCESS;
}

/**
  Configures steps prior to execute Pass Gate Test
  Configure the DMAP register according with the Device Density and Data Width

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in, out]  Channel         Current channel examined
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
PassGateTestEntryHooks (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  //
  // Save the content of any register modified during the Pass Gate Test
  //
  MmrcExecuteTask (MrcData, PASS_GATE_SAVE_ENTRY, NO_PRINT, Channel);
  //
  // Enable the CPGC Engine.
  //
  MmrcExecuteTask (MrcData, CPGC_ENABLE, NO_PRINT, Channel);
  //
  // Dunit & Cpgc Configuration
  //
  MrcData->PassGateParameters.BankOffset          -= 6;
  MrcData->PassGateParameters.BankGroupOffset     -= 6;
  MrcData->PassGateParameters.RowOffset           -= 6;
  MrcData->PassGateParameters.RankOffset          -= 6;
  //
  // Initialize dynamic variables to disable state/value
  //
  MrcData->MspData.DynamicVars[Channel][PG_DIMMSEL]       = 0x1F;
  MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]        = 0x1F;
  MrcData->MspData.DynamicVars[Channel][PG_BG1_SEL]       = 0x1F;
  MrcData->MspData.DynamicVars[Channel][PG_BNK_SEL]       = 0x1F;
  MrcData->MspData.DynamicVars[Channel][PG_COL11SEL]      = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL]       = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_17SEL]     = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_16SEL]     = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_15SEL]     = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_14SEL]     = 0x3F;

  //
  // Calculate the right values for the common variables for DDR3/4
  //
  MrcData->MspData.DynamicVars[Channel][PG_BNK_SEL]       = MrcData->PassGateParameters.BankGroupOffset;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL]       = MrcData->PassGateParameters.RowOffset;
  if (MrcData->PassGateParameters.BitsPerRow >= 18) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_17SEL]   = MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL] + 17;
  }
  if (MrcData->PassGateParameters.BitsPerRow >= 17) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_16SEL]   = MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL] + 16;
  }
  if (MrcData->PassGateParameters.BitsPerRow >= 16) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_15SEL]   = MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL] + 15;
  }
  if (MrcData->PassGateParameters.BitsPerRow >= 15) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_14SEL]   = MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL] + 14;
  }
  MrcData->MspData.DynamicVars[Channel][PG_DIMMSEL]       = MrcData->PassGateParameters.RankOffset + 1 - 7;
  MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]        = MrcData->PassGateParameters.RankOffset - 7;

  switch (MrcData->MrcParameters.DramType[Channel]) {
  case TypeDdr3:
  case TypeDdr3L:
  case TypeDdr3U:
  case TypeDdr3All:
    if (MrcData->PassGateParameters.BitsPerColumns == 11) {
      MrcData->MspData.DynamicVars[Channel][PG_COL11SEL]  = 0;
    }
    break;
  case TypeDdr4:
    MrcData->MspData.DynamicVars[Channel][PG_BG1_SEL]     = MrcData->MspData.DynamicVars[Channel][PG_BNK_SEL] + 1;
    break;
  default:
    break;
  }

  //
  // Cpcg configuration
  // 128 Bursts per Subsequences during writes (Ag, Vic0 and Vic1) and reads (only Vic0 and Vic1)
  // 2^(8-1) Bursts per subsequence 
  // LOW_SHIFT should point to the first Bank bit 
  //
  MrcData->MspData.DynamicVars[Channel][PG_BURST]                   = MrcData->PassGateParameters.Burst;
  MrcData->MspData.DynamicVars[Channel][PG_BURST_CHARACTERIZE]      = MrcData->PassGateParameters.SpeedSelect;
  MrcData->MspData.DynamicVars[Channel][PG_VAR_LO_SHIFT]            = MrcData->PassGateParameters.BankGroupOffset - 3 + AntiLogBase2 (MrcData->PassGateParameters.SpeedSelect);
  MrcData->MspData.DynamicVars[Channel][PG_VAR_HI_SHIFT]            = 0;
  MrcData->MspData.DynamicVars[Channel][PG_VAR_LO_SHIFT_AGG_READS]  = MrcData->PassGateParameters.BankGroupOffset;
  MrcData->MspData.DynamicVars[Channel][PG_VAR_HI_SHIFT_AGG_READS]  = 0;
  //
  // CPGC Delay adjustments
  //
  switch (MrcData->MrcParameters.DdrFrequency) {
  case MMRC_2400:
    MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT] = 54;
    break;
  case MMRC_2133:
    MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT] = 47;
    break;
  case MMRC_1866:
    MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT] = 40;
    break;
  case MMRC_1600:
    MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT] = 34;
    MrcData->PassGateParameters.ActivateAdjust    = 90;
    break;
  case MMRC_1333:
    MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT] = 27;
    MrcData->PassGateParameters.ActivateAdjust    = 100;
    break;
  default:
    break;
  }
#if SIM
  MrcData->PassGateParameters.ActivateAdjust      = 0;
#endif
  MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT]  -= (MrcData->PassGateParameters.SpeedSelect - 1);

  //
  // DRFC
  //
  if (MrcData->PassGateParameters.EnablePeriodicSelfRefresh) {
    MrcData->MspData.DynamicVars[Channel][PG_SELF_REFRESH] = 1;
  } else {
    MrcData->MspData.DynamicVars[Channel][PG_SELF_REFRESH] = 0;
  }
  //
  // Specify the Aggressor and Victim Patterns
  // 
  if (MrcData->PassGateParameters.Pattern == 0) {
    MrcData->MspData.DynamicVars[Channel][PG_AGGRESSOR_PATTERN] = 0x00000000;
  } else {
    MrcData->MspData.DynamicVars[Channel][PG_AGGRESSOR_PATTERN] = 0xffffffff;
  }
  if (MrcData->PassGateParameters.TargetPattern == 0) {
    MrcData->MspData.DynamicVars[Channel][PG_VICTIM_PATTERN] = 0x00000000;
  } else {
    MrcData->MspData.DynamicVars[Channel][PG_VICTIM_PATTERN] = 0xffffffff;
  }

#ifdef PASS_GATE_DEBUG_MSG
  MspDebugPrint ((MSP_DBG_MIN, "Pass Gate Variable:\n"));              
  MspDebugPrint ((MSP_DBG_MIN, "DeviceDataWidth     = %d\n", MrcData->PassGateParameters.DeviceDataWidth));
  MspDebugPrint ((MSP_DBG_MIN, "DeviceDensity       = %d\n", MrcData->PassGateParameters.DeviceDensity));
  MspDebugPrint ((MSP_DBG_MIN, "DimmSwizzle         = %d:%d\n", MrcData->PassGateParameters.DimmSwizzle[0], MrcData->PassGateParameters.DimmSwizzle[1]));
  
  MspDebugPrint ((MSP_DBG_MIN, "RowMask             = 0x%x\n", MrcData->PassGateParameters.RowMask));
  MspDebugPrint ((MSP_DBG_MIN, "RowBankTrackerMask  = 0x%x\n", MrcData->PassGateParameters.RowBankTrackerMask));

  MspDebugPrint ((MSP_DBG_MIN, "BitsPerBankColumns  = %d\n", MrcData->PassGateParameters.BitsPerColumns));
  MspDebugPrint ((MSP_DBG_MIN, "BitsPerBank         = %d\n", MrcData->PassGateParameters.BitsPerBank));
  MspDebugPrint ((MSP_DBG_MIN, "BitsPerBankGroup    = %d\n", MrcData->PassGateParameters.BitsPerBankGroup));
  MspDebugPrint ((MSP_DBG_MIN, "BitsPerRow          = %d\n", MrcData->PassGateParameters.BitsPerRow));

  MspDebugPrint ((MSP_DBG_MIN, "BankGroupOffset     = %d\n", MrcData->PassGateParameters.BankGroupOffset));
  MspDebugPrint ((MSP_DBG_MIN, "BankOffset          = %d\n", MrcData->PassGateParameters.BankOffset));
  MspDebugPrint ((MSP_DBG_MIN, "RowOffset           = %d\n", MrcData->PassGateParameters.RowOffset));
  MspDebugPrint ((MSP_DBG_MIN, "RankOffset          = %d\n", MrcData->PassGateParameters.RankOffset));

  MspDebugPrint ((MSP_DBG_MIN, "Burst               = %d\n", MrcData->PassGateParameters.Burst));

  MspDebugPrint ((MSP_DBG_MIN, "ActivateAdjust      = %d\n", MrcData->PassGateParameters.ActivateAdjust));

  MspDebugPrint ((MSP_DBG_MIN, "Pass Gate Cpgc Parameters:\n"));
  MspDebugPrint ((MSP_DBG_MIN, "PG_COL11SEL     = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_COL11SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_BNK_SEL      = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_BNK_SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_BG1_SEL      = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_BG1_SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_ROW_SEL      = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_ROW_SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_ROW_14SEL    = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_ROW_14SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_ROW_15SEL    = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_ROW_15SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_ROW_16SEL    = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_ROW_16SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_ROW_17SEL    = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_ROW_17SEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_RNKSEL       = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_DIMMSEL      = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_DIMMSEL]));


  MspDebugPrint ((MSP_DBG_MIN, "PG_BURST              = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_BURST]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_BURST_CHARACTERIZE = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_BURST_CHARACTERIZE]));

  MspDebugPrint ((MSP_DBG_MIN, "PG_VAR_LO_SHIFT = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_VAR_LO_SHIFT]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_VAR_LO_SHIFTA= %d\n",  MrcData->MspData.DynamicVars[Channel][PG_VAR_LO_SHIFT_AGG_READS]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_VAR_HI_SHIFT = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_VAR_HI_SHIFT]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_VAR_HI_SHIFTA= %d\n",  MrcData->MspData.DynamicVars[Channel][PG_VAR_HI_SHIFT_AGG_READS]));

  MspDebugPrint ((MSP_DBG_MIN, "PG_SUBSEQ_WAIT  = %d\n",  MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_WAIT]));

  MspDebugPrint ((MSP_DBG_MIN, "PG_AGGRESSOR_PATTERN  = 0x%08x\n",  MrcData->MspData.DynamicVars[Channel][PG_AGGRESSOR_PATTERN]));
  MspDebugPrint ((MSP_DBG_MIN, "PG_VICTIM_PATTERN     = 0x%08x\n",  MrcData->MspData.DynamicVars[Channel][PG_VICTIM_PATTERN]));
#endif
  MmrcExecuteTask (MrcData, PASS_GATE_SET_ENTRY, NO_PRINT, Channel);
  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_STATIC_SETUP, NO_PRINT, Channel);

  return MMRC_SUCCESS;
}

/**
  Configures steps after execute Pass Gate Test

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in, out]  Channel         Current channel examined
  @retval          MMRC_SUCCESS    
**/
MMRC_STATUS
PassGateTestExitHooks (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{

  MmrcExecuteTask (MrcData, PASS_GATE_EXIT, NO_PRINT, Channel);

  return MMRC_SUCCESS;
}
#endif // PASS_GATE_TEST == 1

extern MMRC_STATUS
OemClearSelfRefreshClamp (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
);

/**
  This function releases the board's activation of self refresh to the DIMMs

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
ClearSelfRefreshClamp (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  return OemClearSelfRefreshClamp(MrcData, CapsuleStartIndex, StringIndex, Channel);
}

/**
  This function restores any saved NVDIMM data and prepares for the next save

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
RestoreArmNvDimms (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  if (NVDIMM_MODE == MrcData->NonVolMemMode){
    PlatformRestoreNvDimms(MrcData);
    PlatformArmNvDimms(MrcData);
  }
  return MMRC_SUCCESS;
}

/**
  Wrap RxDqs training result may be different across DDR3/DDR4 HIP implementations

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
DisplayOrRestoreTrainingResultsRxDqsWrapper (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        BOOLEAN       FaultyPartProcess
  )
{
  DisplayOrRestoreTrainingResults (MrcData, Channel, RxDqsPDelay, FaultyPartProcess);
  DisplayOrRestoreTrainingResults (MrcData, Channel, RxDqsNDelay, FaultyPartProcess);
  if (MrcData->ReadPerBitEnable) {
    DnvNibbleTrainingHook(MrcData, 0, 0, 0); 
    DisplayOrRestoreTrainingResults (MrcData, Channel, RxDqBitDelay, FaultyPartProcess);
    DnvNibbleTrainingExitHook(MrcData, 0, 0, 0); 
  }

  return MMRC_SUCCESS;
};


/**
  SignalPunitMemInitDone sends the MemInitDone punit mailbox command

  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
  @param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
  @param[in]       Channel             Current Channel being examined.
  @retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SignalPunitMemInitDone(
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
)
{
#if PUNIT_MAILBOX == 1
  PunitMemInitDone(MrcData);
#endif
  return MMRC_SUCCESS;
}

/**
Return the flags required for the Jedec Reset Type for Read Training (basic and advanced)

@retval  x    Bit flags for this training

**/
UINT16
GetJedecResetTypeForRd ()
{
  return NO_ACTION;
}

/**
Return the flags required for the Jedec Reset Type for Write Training (advanced only)

@retval  x    Bit flags for this training

**/
UINT16
GetJedecResetTypeForWr ()
{
  return NO_ACTION;
}

/**
Getx4FloorPlan returns the high logical strobe number that corresponds to the low strobe to support x4 configurations
and siganls that have byte level granularity

@param[in]       Strobe             Strobe
@retval          Strobe             Strobe map or error

**/
UINT8
Getx4FloorPlan (
  IN      UINT8             Strobe
)
{
  //
  // Floorplan layout to map x4 strobes to a byte lane granularity.
  //
  //                            S0  S1   S2   S3   S4   S5   S6   S7   S8       
  UINT8 FloorPlanx4Config[MAX_STROBE_X8_DEVICES] = { 9, 10, 11, 12, 13, 14, 15, 16, 17 };
  if (Strobe < MAX_STROBE_X8_DEVICES) {
    return FloorPlanx4Config[Strobe];
  }

  return 0xFF;
}
/**
Check if ther eis x4 support per project basis

@param[in, out]  MrcData         Host structure for all data related to MMRC.

**/
MMRC_STATUS
CheckX4Support (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             *function,
IN      UINT32            line
)
{
  return MMRC_SUCCESS;
}


/**
Check if the Per bit should be executed per project basis

@param[in, out]  MrcData         Host structure for all data related to MMRC.

**/
MMRC_STATUS
CheckPerBitSupport (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             *function,
IN      UINT32            line
)
{
  return MMRC_SUCCESS;
}

/**
  Indicates if the Dimm under examination should be initialized or not.

  Cases:
  Dimm is V                         - Dimm should be initialized
  Dimm is NV and data is NOT valid  - Dimm should be initialized
  Dimm is NV and data is valid      - Dimm should NOT be initialized

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.

  @retval          TRUE            Only if initialization is required
**/
MMRC_STATUS
IsInitRequired (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Dimm
  )
{
  UINT8 SkipInit = 0;

  EccInitializeNeeded(MrcData, Channel, Dimm, &SkipInit);
  return (0 == SkipInit);
}

/**
  Determines the right values for DMAP configuration according with the rank

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm
  @param[in]       Rank            Current Rank
  @param[out]      RsOffset        Rank Offset
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DetermineRankMap (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Dimm,
  IN      UINT8             Rank,
  IN      UINT8             *RsOffset
)
{

  MrcData->MspData.DynamicVars[Channel][PG_ROW_14SEL] = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_15SEL] = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_16SEL] = 0x3F;
  MrcData->MspData.DynamicVars[Channel][PG_ROW_17SEL] = 0x3F;

  MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]    = 0x12;

  //
  // D_Size 0: 2Gb, 1:4Gb, 2:8Gb 3:16Gb
  // D_DataWidth 0:x4, 1:x8, 2:x16
  //
  if ((MrcData->D_Size[Channel][Dimm] == 0 && MrcData->D_DataWidth[Channel][Dimm] == 0) || (MrcData->D_Size[Channel][Dimm] > 0)) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_14SEL] = 0x19;
    MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]    = 0x13;
  }
  if ((MrcData->D_Size[Channel][Dimm] == 1 && MrcData->D_DataWidth[Channel][Dimm] == 0) || (MrcData->D_Size[Channel][Dimm] > 1)) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_15SEL] = 0x1A;
    MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]    = 0x14;
  }
  if ((MrcData->D_Size[Channel][Dimm] == 2 && MrcData->D_DataWidth[Channel][Dimm] == 0) || (MrcData->D_Size[Channel][Dimm] > 2)) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_16SEL] = 0x1B;
    MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]    = 0x15;
  }
  if ((MrcData->D_Size[Channel][Dimm] == 3 && MrcData->D_DataWidth[Channel][Dimm] == 0) || (MrcData->D_Size[Channel][Dimm] > 3)) {
    MrcData->MspData.DynamicVars[Channel][PG_ROW_17SEL] = 0x1C;
    MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]    = 0x16;
  }

  *RsOffset = (MrcData->MspData.DynamicVars[Channel][PG_RNKSEL] & 0xFF) + 13 - 6;

  MrcData->MspData.DynamicVars[Channel][INIT_DIMMDWID] = MrcData->MspData.DynamicVars[Channel][DIMMDWID];
  if (Dimm == 0) {
    MrcData->MspData.DynamicVars[Channel][INIT_DIMMDDEN] = MrcData->MspData.DynamicVars[Channel][DIMMDDEN0];
  } else {
    MrcData->MspData.DynamicVars[Channel][INIT_DIMMDDEN] = MrcData->MspData.DynamicVars[Channel][DIMMDDEN1];
  }

  MspDebugPrint ((MSP_DBG_MAX, "PG_ROW_14SEL = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][PG_ROW_14SEL]));
  MspDebugPrint ((MSP_DBG_MAX, "PG_ROW_15SEL = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][PG_ROW_15SEL]));
  MspDebugPrint ((MSP_DBG_MAX, "PG_ROW_16SEL = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][PG_ROW_16SEL]));
  MspDebugPrint ((MSP_DBG_MAX, "PG_ROW_17SEL = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][PG_ROW_17SEL]));
  MspDebugPrint ((MSP_DBG_MAX, "PG_RNKSEL    = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][PG_RNKSEL]));

  MspDebugPrint ((MSP_DBG_MAX, "INIT_DIMMDDEN = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][INIT_DIMMDDEN]));
  MspDebugPrint ((MSP_DBG_MAX, "INIT_DIMMDWID = 0x%02x\n", MrcData->MspData.DynamicVars[Channel][INIT_DIMMDWID]));

  MspDebugPrint ((MSP_DBG_MAX, "*RsOffset(Cpgc) = %02d\n", *RsOffset));

  MmrcExecuteTask (MrcData, INIT_DMAP, NO_PRINT, Channel);

  return MMRC_SUCCESS;
}

/**
  Checks if ADR flow needs to be executed

  @param[in, out]  MrcData         Host structure for all data related to MMRC.

  @retval          MMRC_SUCCESS
**/
BOOLEAN
IsAdrFlow (
  IN  OUT MMRC_DATA         *MrcData
  )
{
  if (MrcData->MrcParameters.BootMode == ADR) {
    return TRUE;
  }
  return FALSE;
}

/**
  Initializes the memory on an specific Dimm

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MemInitPerDimm (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel
  )
{
#if ECC_SCRUB == 1
  UINT8     Rank;
  UINT32    RankSize;
  UINT8     Dimm;
  UINT32    RankAddress;
  UINT32    Errors;
  UINT8     RsOffset;
  MMRC_STATUS Status;
  UINT8     Segments;

  if (((MrcData->SkipOptions & SKIP_TEST) != 0) && (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All)) {
    MspDebugPrint ((MSP_DBG_MAX, "DDR3 && SKIP_TEST\n"));
    return MMRC_SUCCESS;
  } else if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    //
    // NVDIMM are NOT supported for DDR3 so all the memory is initialized
    //
    return MemInit(MrcData, Channel, MrcMemInit);
  }  else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    MspDebugPrint ((MSP_DBG_MAX, "Temporal Fix when Scrambler is enabled\n"));
    if (MrcData->OemMrcData.MemoryScrubSegments.MemorySegmentationEnabled) {
      MspDebugPrint ((MSP_DBG_MIN, "Segmentation enabled\n"));
      Status = MMRC_SUCCESS;
      for (Segments = 0; Segments < MrcData->OemMrcData.MemoryScrubSegments.NumberOfSegments; Segments++) {
        MrcData->MspData.DynamicVars[Channel][MEM_LOW_ADDRESS]    = MrcData->OemMrcData.MemoryScrubSegments.ScrubSegment[Segments].Low << 14;
        MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES]  = (MrcData->OemMrcData.MemoryScrubSegments.ScrubSegment[Segments].High << 14) - 1;
        MspDebugPrint ((MSP_DBG_MIN, "Scrubbing L: 0x%08x%08x H: 0x%08x%08x\n",
          (UINT32)(MrcData->MspData.DynamicVars[Channel][MEM_LOW_ADDRESS]>>32) & 0xFFFFFFFF, ((UINT32)MrcData->MspData.DynamicVars[Channel][MEM_LOW_ADDRESS] & 0xFFFFFFFF),
          (UINT32)(MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES]>>32) & 0xFFFFFFFF, ((UINT32)MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES] & 0xFFFFFFFF)
        ));

        Status |= MemInit(MrcData, Channel, MrcMemInit);
      }
      return Status;

    } else {
      MspDebugPrint ((MSP_DBG_MIN, "Scrubbing L: 0x%08x%08x H: 0x%08x%08x\n",
        (UINT32)(MrcData->MspData.DynamicVars[Channel][MEM_LOW_ADDRESS]>>32) & 0xFFFFFFFF, ((UINT32)MrcData->MspData.DynamicVars[Channel][MEM_LOW_ADDRESS] & 0xFFFFFFFF),
        (UINT32)(MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES]>>32) & 0xFFFFFFFF, ((UINT32)MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES] & 0xFFFFFFFF)
      ));
      return MemInit(MrcData, Channel, MrcMemInit);
    }
  }

  MmrcExecuteTask (MrcData, INIT_ENTRY, NO_PRINT, Channel);
  MmrcExecuteTask (MrcData, INIT_STATIC, NO_PRINT, Channel);

  for (Rank = 0; Rank < MAX_RANKS; Rank++) {

    Dimm = Rank / MAX_RANKS_PER_DIMM;

    if (!MrcData->RankEnabled[Channel][Rank]) {
      continue;
    }

    if (!IsInitRequired (MrcData, Channel, Dimm)) {
      continue;
    }

    DetermineRankMap (MrcData, Channel, Dimm, Rank, &RsOffset);

    RankSize = MrcData->MrcParameters.SlotMem[Channel][Dimm] / (MrcData->D_Ranks[Channel][Dimm] + 1);
    MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d Init required, Rank Size = %d\n", Channel, Dimm, Rank, RankSize));

    RankAddress = (Rank << RsOffset);
    MrcData->MspData.DynamicVars[Channel][START_ADDRESS]  = RankAddress;
    MrcData->MspData.DynamicVars[Channel][WRAP_ADDRESS]   = RankAddress | ((RankSize << 14) - 1);
    MspDebugPrint ((MSP_DBG_MAX, "0x%08x -- 0x%08x\n", (UINT32)MrcData->MspData.DynamicVars[Channel][START_ADDRESS], (UINT32) MrcData->MspData.DynamicVars[Channel][WRAP_ADDRESS]));
#ifdef SIM
    //
    // Limit the End - Start to only 8 burst during simulation
    //
    MrcData->MspData.DynamicVars[Channel][START_ADDRESS]  = RankAddress | ((RankSize << 14) - 1) - 8;
#endif

    MmrcExecuteTask (MrcData, INIT_SETUP, 0, Channel);

    CpgcExecuteSingleTest (MrcData, Channel, 0, &Errors, NULL, NULL);

    if (Errors != 0) {
      MspDebugPrint ((MSP_DBG_MIN, "ERRORS detected C%dD%dR%d\n", Channel, Dimm, Rank));
    }
  } // Dimm loop ...

  MmrcExecuteTask (MrcData, INIT_EXIT, NO_PRINT, Channel);
#endif // ECC_SCRUB == 1
  return MMRC_SUCCESS;
}

/**
RxTxSkewCtl Enables the hook for PMO and memory vendors to skew Rx or Tx Dq/Dqs training results
            by  2 ticks to the left or right of the trained value

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
RxTxSkewCtl(
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  ){
  UINT8  ch;
  UINT8  rk;
  UINT8  st;
  UINT32 ValueOffset;
  UINT8  SkewIdx;
  SKEW_CTL_STRUCT SkewStruct[2];

  SkewStruct[0].DqSignal  = RxDqsDelay;
  SkewStruct[0].DqsSignal = RecEnDelay;
  SkewStruct[0].Skew      = MrcData->RxSkewCtl;

  SkewStruct[1].DqSignal  = 0xFFFF;
  SkewStruct[1].DqsSignal = WrLvlDelay;
  SkewStruct[1].Skew      = MrcData->TxSkewCtl;
#if SIM
  if (MrcData->RxSkewCtl != 0) {
    DisplayOrRestoreTrainingResults(MrcData, Channel, RecEnDelay, FALSE);
    DisplayOrRestoreTrainingResults(MrcData, Channel, RxDqsPDelay, FALSE);
    DisplayOrRestoreTrainingResults(MrcData, Channel, RxDqsNDelay, FALSE);
  }
  if (MrcData->TxSkewCtl != 0) {
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqsDelay, FALSE);
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDelay, TRUE);
#ifdef TxDqDrvDelay
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDrvDelay, FALSE);
#endif
  }
#endif

  for (SkewIdx = 0; SkewIdx < MAX_SKEW_SIGNAL; SkewIdx++) {
    for (ch = 0; ch < MAX_CHANNELS; ch++) {
      if (!MrcData->MrcParameters.ChEnabled[ch]) continue;
      if (SkewStruct[SkewIdx].Skew != 0) {
        switch (SkewStruct[SkewIdx].Skew) {
        case 1:
          ValueOffset = 2;
          break;
        case 2:
          ValueOffset = (UINT32) -2;
          break;
        default:
          ValueOffset = 0;
          break;
        }
        for (rk = 0; rk < MAX_RANKS; rk++) {
          if (!MrcData->RankEnabled[ch][rk]) continue;
          for (st = 0; st < MAX_STROBES; st++) {
            if (!IsStrobeValid(MrcData, ch, rk, st, FALSE, NULL)) continue;
            if (SkewStruct[SkewIdx].DqsSignal != 0xFFFF) {
              GetSetDdrioGroup2(MrcData, ch, rk, st, SkewStruct[SkewIdx].DqsSignal, CMD_SET_OFFSET_UC, &ValueOffset);
            }
            if (SkewStruct[SkewIdx].DqSignal != 0xFFFF) {
              GetSetDdrioGroup2(MrcData, ch, rk, st, SkewStruct[SkewIdx].DqSignal, CMD_SET_OFFSET_UC, &ValueOffset);
            }
          }
        }  // for Rank
      }  // if Skew not zero
    }  // for ch
  }  // for SkewIdx
  if (MrcData->RxSkewCtl != 0) {
    DisplayOrRestoreTrainingResults(MrcData, Channel, RecEnDelay, FALSE);
    DisplayOrRestoreTrainingResults(MrcData, Channel, RxDqsPDelay, FALSE);
    DisplayOrRestoreTrainingResults(MrcData, Channel, RxDqsNDelay, FALSE);
  }
  if (MrcData->TxSkewCtl != 0) {
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqsDelay, FALSE);
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDelay, TRUE);
#ifdef TxDqDrvDelay
    DisplayOrRestoreTrainingResults(MrcData, Channel, TxDqDrvDelay, FALSE);
#endif
  }
  return MMRC_SUCCESS;
}

#if DRAM_RAPL == 1
/**
This routine programs DRAM RAPL LIMIT MSR

@param[in, out]  MrcData       Host structure for all data related to MMRC.
**/
MMRC_STATUS
ProgDramRaplLimitMsr(
IN  OUT MMRC_DATA         *MrcData
)
{
#if !defined SIM && !defined JTAG
  UINT32  LowData;
  UINT32  HighData;

  LowData = (UINT32)MrcData->MspData.DynamicVars[0][RAPL_LIMIT_MISC];
  HighData = (UINT32)(MrcData->MspData.DynamicVars[0][RAPL_LIMIT_MISC] >> 32);

#ifdef __GNUC__
  asm(
    "wrmsr;"
    :
  : "c" (RAPL_LIMIT_MSR),
    "a" (LowData),
    "d" (HighData)
    );
#else
  _asm mov edx, dword ptr[HighData]
    _asm mov eax, dword ptr[LowData]
    _asm mov ecx, RAPL_LIMIT_MSR
  _asm wrmsr
#endif
#endif
  return MMRC_SUCCESS;
}

/**
SaveMrs  save MRS values from 0 to 6 in EXTBUF

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       Channel             Ch to save MRS
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SaveMrs (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
)
{
  CPGC_DPAT_EXTBUF0_REG_CPGC_STRUCT ExtBuf0;
  UINT8 Buffer;
  UINT32 Offset;
  UINT32 temp;
  REGISTER_ACCESS     Register;
  Buffer = 0;
  Offset = 0;
  ExtBuf0.Data = 0;
  temp = 0;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = CPGC_DPAT_EXTBUF0_REG_CPGC_REG;

  //
  //Write MRS 0-6 in EXTBUF 0-6
  //Write Bits A23:A8 from MRS# R2 in bits 31:16 from EXTBUF
  //Write Bits A23:A8 from MRS# R0 in bits 15:0 from EXTBUF
  //Increase by 4 Register buffer to jump next ExtBuf address
  //

  for (Buffer = 0; Buffer <= 6; Buffer++) {
    Register.Offset = (CPGC_DPAT_EXTBUF0_REG_CPGC_REG + (Buffer * 4));
    temp = MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][Buffer]] & 0xFFFF00;
    ExtBuf0.Data = (temp << 8) & 0xFFFF0000;
    temp = MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][Buffer]] & 0xFFFF00;
    ExtBuf0.Data |= (temp >> 8) & 0x0000FFFF;
    MemRegWrite (CPGC, Channel, 0, Register, ExtBuf0.Data,0xF);
  }

  return MMRC_SUCCESS;
}
/**
SaveRcw  save RCW in EXTBUF

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       Channel             Ch to save MRS
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SaveRcw (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
)
{
  CPGC_DPAT_EXTBUF7_REG_CPGC_STRUCT ExtBuf;
  UINT8 Buffer;
  UINT32 Offset;
  UINT32 TempH;
  UINT32 TempL;
  REGISTER_ACCESS     Register;
  Buffer = 0;
  Offset = 0;
  ExtBuf.Data = 0;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = CPGC_DPAT_EXTBUF7_REG_CPGC_REG;

  //
  // Write RC00_R2[23:8] into EXTBUF7[31:16] and RC00_R0[23:8] into EXTBUF7[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF7_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC00]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC00]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC01_R2[23:8] into EXTBUF8[31:16] and RC01_R0[23:8] into EXTBUF8[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF8_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC01]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC01]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC02_R2[23:8] into EXTBUF9[31:16] and RC02_R0[23:8] into EXTBUF9[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF9_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC02]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC02]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC03_R2[23:8] into EXTBUF10[31:16] and RC03_R0[23:8] into EXTBUF10[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF10_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC03]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC03]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC04_R2[23:8] into EXTBUF11[31:16] and RC04_R0[23:8] into EXTBUF11[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF11_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC04]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC04]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC05_R2[23:8] into EXTBUF12[31:16] and RC05_R0[23:8] into EXTBUF12[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF12_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC05]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC05]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC06_R2[23:8] into EXTBUF13[31:16] and RC06_R2[23:8] into EXTBUF13[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF13_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC06]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC06]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC08_R2[23:8] into EXTBUF15[31:16] and RC08_R0[23:8] into EXTBUF15[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF15_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC08]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC08]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC09_R2[23:8] into EXTBUF0B[31:16] and RC09_R0[23:8] into EXTBUF0B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF0B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC09]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC09]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC0A_R2[23:8] into EXTBUF1B[31:16] and RC0A_R0[23:8] into EXTBUF1B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF1B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC0A]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC0A]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC0B_R2[23:8] into EXTBUF2B[31:16] and RC0B_R0[23:8] into EXTBUF2B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF2B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC0B]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC0B]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC0C_R2[23:8] into EXTBUF3B[31:16] and RC0C_R0[23:8] into EXTBUF3B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF3B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC0C]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC0C]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC0D_R2[23:8] into EXTBUF4B[31:16] and RC0D_R0[23:8] into EXTBUF4B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF4B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC0D]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC0D]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC0E_R2[23:8] into EXTBUF5B[31:16] and RC0E_R0[23:8] into EXTBUF5B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF5B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC0E]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC0E]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC0F_R2[23:8] into EXTBUF6B[31:16] and RC0F_R0[23:8] into EXTBUF6B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF6B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC0F]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC0F]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC3X_R2[23:8] into EXTBUF9B[31:16] and RC3X_R0[23:8] into EXTBUF9B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF9B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC3X]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC3X]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC4X_R2[23:8] into EXTBUF10B[31:16] and RC4X_R0[23:8] into EXTBUF10B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF10B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC4X]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC4X]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  //
  // Write RC6X_R2[23:8] into EXTBUF12B[31:16] and RC6X_R0[23:8] into EXTBUF12B[15:0]
  //
  Register.Offset = (CPGC_DPAT_EXTBUF12B_REG_CPGC_REG);
  TempH =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[0][RC6X]] & 0xFFFF00) << 8;
  TempL =  (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[2][RC6X]] & 0xFFFF00) >> 8;
  ExtBuf.Data = TempH | TempL;
  MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data, 0xF);

  return MMRC_SUCCESS;
}
/**
SetExtDefault  set EXTBUF to default values

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       Channel             Ch to save MRS
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SetExtDefault (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
)
{
  CPGC_DPAT_EXTBUF0_REG_CPGC_STRUCT ExtBuf;
  UINT8 Buffer;
  UINT32 Offset;
  REGISTER_ACCESS     Register;
  Buffer = 0;
  Offset = 0;
  ExtBuf.Data = 0;
  Register.Mask     = 0xFFFF;
  Register.ShiftBit = 0;
  Register.Offset   = CPGC_DPAT_EXTBUF0_REG_CPGC_REG;

  for (Buffer = 0; Buffer <=30; Buffer++) {
    Register.Offset = (CPGC_DPAT_EXTBUF0_REG_CPGC_REG + (Buffer * 4));
    ExtBuf.Data = 0;
    MemRegWrite (CPGC, Channel, 0, Register, ExtBuf.Data,0xF);
  }
  return MMRC_SUCCESS;
}

/**
Rapl enables the time window and band width scale factors

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
RaplSupport(
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
){
  if (MrcData->DramRaplParameters.PowerLimitEnable == 1) {
    MmrcExecuteTask(MrcData, DramRapl, NO_PRINT, Channel);
    ProgDramRaplLimitMsr (MrcData);
    SetRaplBandWidthScale(MrcData);
  }
  MmrcExecuteTask(MrcData, DramRaplLocked, 0, Channel);

  return MMRC_SUCCESS;
}

/**
  CA Parity Enable

  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
  @param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
  @param[in]       Channel             Current Channel being examined.

  @retval          Status              MMRC_SUCCESS
**/
MMRC_STATUS
CaParityEnable(
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
)
{
  UINT8   Dimm;
  DRAM_CMD_DDR4 CmdDdr4;

  if (MrcData->CommandAddressParity == 1) {
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm] ||
          MrcData->MrcParameters.DramType[Channel] != TypeDdr4 ||
          MrcData->MspData.DynamicVars[Channel][DIMMTYPE] != SPD_DDR_RDIMM
      ) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_MIN, "C%dD%d CA Parity Enable\n", Channel, Dimm));
      //
      // RCE Bits:
      // 0: Parity Enable
      // 1: Reserved
      // 2: Alert_n pulse width
      // 3: Parity checking is re-enabled
      //
      CmdDdr4.Data = ReadMrsCommand(MrcData, Channel, 2*Dimm, 7);
      MspDebugPrint ((MSP_DBG_MIN, "Read 0x%08x\n", CmdDdr4.Data));
      CmdDdr4.Bits.memoryaddress |= BIT0;
      MspDebugPrint ((MSP_DBG_MIN, "Write 0x%08x\n", CmdDdr4.Data));
      WriteDramCommand (MrcData, Channel, 2*Dimm, CmdDdr4.Data, SIDE_A);
    }
  }
  return MMRC_SUCCESS;
}

/**
DnvNibbleTrainingExitHook  switches the training granularity to x4/x8

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
@param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
@param[in]       Channel             Current Channel being examined.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
DnvNibbleTrainingExitHook (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
)
{
#if  NIBBLE_TRAINING_SUPPORT == 1
  UINT8   Rk;
  UINT8   Ch;

  for (Ch = 0; Ch < MAX_CHANNELS; Ch++) {
    for (Rk = 0; Rk < MAX_RANKS; Rk++) {
      if (!MrcData->MrcParameters.ChEnabled[Ch]) {
        continue;
      }
      //
      // Restore back DQ and Bit values
      //
      MrcData->MaxDq[Ch][Rk] = MrcData->MaxDqCache[Ch][Rk];
      MrcData->MaxBit[Ch][Rk] = MrcData->MaxBitCache[Ch][Rk];
    }
  }
#endif
  return MMRC_SUCCESS;
}

/**
DnvNibbleTrainingHook  switches the training granularity to x4 in case there is a x8 dimm

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
@param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
@param[in]       Channel             Current Channel being examined.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
DnvNibbleTrainingHook (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
)
{
#if  NIBBLE_TRAINING_SUPPORT == 1
  UINT8   Rk;
  UINT8   Ch;

  for (Ch = 0; Ch < MAX_CHANNELS; Ch++) {
    for (Rk = 0; Rk < MAX_RANKS; Rk++) {
      if (!MrcData->MrcParameters.ChEnabled[Ch]) {
        continue;
      }
      //
      // Training always as x4 nibble bases
      // During Adv Training algorithms some functions are call recursively and we need to make
      // sure we do NOT override our MaxDqCache with an incorrect value.
      //
      if (MrcData->MaxDqCache[Ch][Rk] == 0) {
        MrcData->MaxDqCache[Ch][Rk] = MrcData->MaxDq[Ch][Rk];
      }
      if (MrcData->MaxBitCache[Ch][Rk] == 0) {
        MrcData->MaxBitCache[Ch][Rk] = MrcData->MaxBit[Ch][Rk];
      }
      MrcData->MaxDq[Ch][Rk] = MAX_STROBE_X4_DEVICES;
      MrcData->MaxBit[Ch][Rk] = STROBE_BITS_X4_DEVICES;
    }
  }
#endif
  return MMRC_SUCCESS;
}

VOID
JedecDllOffToOnFlow (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Dimm
  )
{
  DramInitDDR4MRS0 Mrs0Command;
  DramInitDDR4EMR1 Mrs1Command;
  const UINT8      FistDimmRank = (2 * Dimm);
  const UINT8      NextDimmsRank = (2 * Dimm) + MAX_RANKS_PER_DIMM;
  UINT8            Rank = 0;

  //
  // CKE has to be High when doing the MRS writes
  //
  MmrcExecuteTask(MrcData, RELEASE_CKE, SearchSubtaskName(RELEASE_CKE), Channel);

  for (Rank = FistDimmRank; Rank < NextDimmsRank; Rank++) {
    if (MrcData->MrcParameters.ChEnabled[Channel] && MrcData->RankEnabled[Channel][Rank]) {
      Mrs0Command.Data = ReadMrsCommand(MrcData, Channel, Rank, 0);
      Mrs1Command.Data = ReadMrsCommand(MrcData, Channel, Rank, 1);
      Mrs1Command.Bits.dllEnabled = 1;
      Mrs0Command.Bits.dllReset = 1;

      MspDebugPrint((MSP_DBG_MAX, " %a() C%d-D%d-R%d   MR1<- %x MR0<-%x\n", __FUNCTION__, Channel, Dimm, Rank, Mrs1Command.Data, Mrs0Command.Data));
      //
      // execute the DIMM DLL off to on flow. After recovery NVDIMM's DLL is off
      //
      WriteDramCommandSideWrap(MrcData, Channel, Rank, Mrs1Command.Data);
      WriteDramCommandSideWrap(MrcData, Channel, Rank, Mrs0Command.Data);
    }
  }
}
#endif

