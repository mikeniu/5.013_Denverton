/**  @file
  Bdat.c
  This file contains Initialization of the BDAT data structures for both
  v1.0 and v2.0 taken from v0.6 of "A Compatible Method to Access BIOS
  Data Structures"

  Copyright (c) 2005-2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "Bdat.h"
#include "DetectDimms.h"
#include "MmrcLibraries.h"

#if BDAT
/**
  This function prints both v1.0 and v2.0 of the BDAT data structure per
  v0.6 of "A Compatible Method to Access BIOS Data Structures".

  @param[in] BdatMemoryData2Structure:   The BDAT_MEMORY_DATA_2_STRUCTURE structure to be initialized. 
**/
VOID 
PrintMemoryData2Structure (
  IN        BDAT_MEMORY_DATA_2_STRUCTURE    *BdatMemoryData2Structure
  )
{
  UINT8     Node;
  UINT8     Channel;
  UINT8     Dimm;
  UINT8     Rank;
  UINT16    SpdElement;
  UINT8     Strobe;
  UINT8     DramDevice;
  UINT8     ModeRegister;
  UINT8     Dq;
  UINT8     TempValue8;
  UINT8     *PtrValue8;

  BDAT_SOCKET_2_STRUCTURE     *BdatSocket;
  BDAT_CHANNEL_2_STRUCTURE    *BdatChannel;
  BDAT_DIMM_2_STRUCTURE       *BdatDimm;
  BDAT_RANK_2_STRUCTURE       *BdatRank;
  BDAT_SPD_2_STRUCTURE        *BdatSpd;

  MspDebugPrint ((MSP_DBG_DEBUG, "Printing BDAT Memory Schema 2 Structure\n"));

  MspDebugPrint ((MSP_DBG_DEBUG, "-schemaHeader\n"));

  MspDebugPrint ((MSP_DBG_DEBUG, "  SchemaId = {"));
  PtrValue8 = (UINT8 *)&BdatMemoryData2Structure->schemaHeader.SchemaId;

  TempValue8 = 0;
  while (TempValue8 < sizeof (EFI_GUID)) {
    if (TempValue8 == 0) {
      MspDebugPrint ((MSP_DBG_DEBUG, "%08x, ", ((UINT32 *) PtrValue8)[TempValue8]));
      TempValue8 += 4;
    } else if (TempValue8 == 4 || TempValue8 == 6) {
      MspDebugPrint ((MSP_DBG_DEBUG, "%04x, ", ((UINT16 *) PtrValue8)[TempValue8]));
      TempValue8 += 2;
    } else {
      MspDebugPrint ((MSP_DBG_DEBUG, "%02x, ", ((UINT8 *) PtrValue8)[TempValue8]));
      TempValue8 += 1;
    }
  }
  MspDebugPrint ((MSP_DBG_DEBUG, "}\n"));

  MspDebugPrint ((MSP_DBG_DEBUG, "  DataSize = %02d\n", BdatMemoryData2Structure->schemaHeader.DataSize));
  MspDebugPrint ((MSP_DBG_DEBUG, "  Crc16    = %02d\n", BdatMemoryData2Structure->schemaHeader.Crc16));

  MspDebugPrint ((MSP_DBG_DEBUG, "refCodeRevision = 0x%08x\n", BdatMemoryData2Structure->refCodeRevision));
  MspDebugPrint ((MSP_DBG_DEBUG, "maxNode         = %02d\n", BdatMemoryData2Structure->maxNode));
  MspDebugPrint ((MSP_DBG_DEBUG, "maxCh           = %02d\n", BdatMemoryData2Structure->maxCh));
  MspDebugPrint ((MSP_DBG_DEBUG, "maxDimm         = %02d\n", BdatMemoryData2Structure->maxDimm));
  MspDebugPrint ((MSP_DBG_DEBUG, "maxRankDimm     = %02d\n", BdatMemoryData2Structure->maxRankDimm));
  MspDebugPrint ((MSP_DBG_DEBUG, "maxStrobe       = %02d\n", BdatMemoryData2Structure->maxStrobe));
  MspDebugPrint ((MSP_DBG_DEBUG, "maxDq           = %02d\n", BdatMemoryData2Structure->maxDq));
  MspDebugPrint ((MSP_DBG_DEBUG, "marginLoopCount = %02d\n", BdatMemoryData2Structure->marginLoopCount));

  for (Node = 0; Node < BDAT_MAX_NODES; Node++) {
    MspDebugPrint ((MSP_DBG_DEBUG, "-socketList[%d]\n", Node));
    BdatSocket = (BDAT_SOCKET_2_STRUCTURE *) &BdatMemoryData2Structure->socketList[Node].imcEnabled;
    
    MspDebugPrint ((MSP_DBG_DEBUG, "  imcEnabled = %02d\n", BdatSocket->imcEnabled));
    MspDebugPrint ((MSP_DBG_DEBUG, "  imcDid     = 0x%04x\n", BdatSocket->imcDid));
    MspDebugPrint ((MSP_DBG_DEBUG, "  imcRid     = 0x%02x\n", BdatSocket->imcRid));
    MspDebugPrint ((MSP_DBG_DEBUG, "  ddrFreq    = %05d\n", BdatSocket->ddrFreq));
    MspDebugPrint ((MSP_DBG_DEBUG, "  ddrVoltage = %04d\n", BdatSocket->ddrVoltage));
    MspDebugPrint ((MSP_DBG_DEBUG, "  rxVrefStep = %04d\n", BdatSocket->rxVrefStep));
    MspDebugPrint ((MSP_DBG_DEBUG, "  txVrefStep = %04d\n", BdatSocket->txVrefStep));
    MspDebugPrint ((MSP_DBG_DEBUG, "  caVrefStep = %04d\n", BdatSocket->caVrefStep));
    MspDebugPrint ((MSP_DBG_DEBUG, "  recvenStep = %02d\n", BdatSocket->recvenStep));
    MspDebugPrint ((MSP_DBG_DEBUG, "  wrLevStep  = %02d\n", BdatSocket->wrLevStep));
                                                
    for (Channel = 0; Channel < BDAT_MAX_CHANNELS; Channel++) {
      MspDebugPrint ((MSP_DBG_DEBUG, "  -channelList[%d]\n", Channel));
      BdatChannel = (BDAT_CHANNEL_2_STRUCTURE *) &BdatSocket->channelList[Channel].chEnabled;

      MspDebugPrint ((MSP_DBG_DEBUG, "    chEnabled   = %02d\n", BdatChannel->chEnabled));
      MspDebugPrint ((MSP_DBG_DEBUG, "    numDimmSlot = %02d\n", BdatChannel->numDimmSlot));
      //                              
      // Skip if channel is disabled  
      //                              
      if (BdatChannel->chEnabled == 0) {
        continue;
      }
      
      for (Dimm = 0; Dimm < BDAT_MAX_DIMM; Dimm++) {
        MspDebugPrint ((MSP_DBG_DEBUG, "    -dimmList[%d]\n", Dimm));
        BdatDimm  = (BDAT_DIMM_2_STRUCTURE *) &BdatChannel->dimmList[Dimm].dimmEnabled;

        MspDebugPrint ((MSP_DBG_DEBUG, "      dimmEnabled = %02d\n", BdatDimm->dimmEnabled));
        //
        // Skip if no DIMM present
        //
        if (BdatDimm->dimmEnabled == 0) {
          continue;
        }

        MspDebugPrint ((MSP_DBG_DEBUG, "      -spdBytes\n"));
        BdatSpd = (BDAT_SPD_2_STRUCTURE *) &BdatDimm->spdBytes.valid[0];

        for (SpdElement = 0; SpdElement < MAX_SPD_BYTE_512; SpdElement++) {
          if ((SpdElement % 8) == 0) {
            MspDebugPrint ((MSP_DBG_DEBUG, "        valid = 0x%02x\n", BdatSpd->valid[SpdElement / 8]));
          }
          if ((BdatSpd->valid[SpdElement / 8] & (1 << (SpdElement % 8))) == 0) {
            continue;  // There are a few values in the "SPD" table that do not actually exist in the SPD.
          }
          MspDebugPrint ((MSP_DBG_DEBUG, "        spdData[%03d] = 0x%02x\n", SpdElement, BdatSpd->spdData[SpdElement]));
        }

        for (Rank = 0; Rank < BDAT_MAX_RANK_DIMM; Rank++) {
          MspDebugPrint ((MSP_DBG_DEBUG, "      -rankList[%d]\n", Rank));
          BdatRank = (BDAT_RANK_2_STRUCTURE *) &BdatDimm->rankList[Rank].rankEnabled;

          MspDebugPrint ((MSP_DBG_DEBUG, "        rankEnabled       = %02d\n", BdatRank->rankEnabled));
          MspDebugPrint ((MSP_DBG_DEBUG, "        rankMarginEnabled = %02d\n", BdatRank->rankMarginEnabled));
          MspDebugPrint ((MSP_DBG_DEBUG, "        dqMarginEnabled   = %02d\n", BdatRank->dqMarginEnabled));
          //
          // Skip if no Rank or if it is mapped out
          //
          if (BdatRank->rankEnabled == 0) {
            continue;
          }

          MspDebugPrint ((MSP_DBG_DEBUG, "        -rankMargin\n"));
          MspDebugPrint ((MSP_DBG_DEBUG, "          rxDqLeft    = %02d\n", (INT8) BdatRank->rankMargin.rxDqLeft));
          MspDebugPrint ((MSP_DBG_DEBUG, "          rxDqRight   = %02d\n", (INT8) BdatRank->rankMargin.rxDqRight));
          MspDebugPrint ((MSP_DBG_DEBUG, "          txDqLeft    = %02d\n", (INT8) BdatRank->rankMargin.txDqLeft));
          MspDebugPrint ((MSP_DBG_DEBUG, "          txDqRight   = %02d\n", (INT8) BdatRank->rankMargin.txDqRight));
          MspDebugPrint ((MSP_DBG_DEBUG, "          cmdLeft     = %02d\n", (INT8) BdatRank->rankMargin.cmdLeft));
          MspDebugPrint ((MSP_DBG_DEBUG, "          cmdRight    = %02d\n", (INT8) BdatRank->rankMargin.cmdRight));
          MspDebugPrint ((MSP_DBG_DEBUG, "          recvenLeft  = %02d\n", (INT8) BdatRank->rankMargin.recvenLeft));
          MspDebugPrint ((MSP_DBG_DEBUG, "          recvenRight = %02d\n", (INT8) BdatRank->rankMargin.recvenRight));
          MspDebugPrint ((MSP_DBG_DEBUG, "          wrLevLeft   = %02d\n", (INT8) BdatRank->rankMargin.wrLevLeft));
          MspDebugPrint ((MSP_DBG_DEBUG, "          wrLevRight  = %02d\n", (INT8) BdatRank->rankMargin.wrLevRight));
          MspDebugPrint ((MSP_DBG_DEBUG, "          rxVrefLow   = %02d\n", (INT8) BdatRank->rankMargin.rxVrefLow));
          MspDebugPrint ((MSP_DBG_DEBUG, "          rxVrefHigh  = %02d\n", (INT8) BdatRank->rankMargin.rxVrefHigh));
          MspDebugPrint ((MSP_DBG_DEBUG, "          txVrefLow   = %02d\n", (INT8) BdatRank->rankMargin.txVrefLow));
          MspDebugPrint ((MSP_DBG_DEBUG, "          txVrefHigh  = %02d\n", (INT8) BdatRank->rankMargin.txVrefHigh));
          MspDebugPrint ((MSP_DBG_DEBUG, "          cmdVrefLow  = %02d\n", (INT8) BdatRank->rankMargin.cmdVrefLow));
          MspDebugPrint ((MSP_DBG_DEBUG, "          cmdVrefHigh = %02d\n", (INT8) BdatRank->rankMargin.cmdVrefHigh));

          if (BdatRank->dqMarginEnabled) {
            for (Dq = 0; Dq < BDAT_MAX_DQ; Dq++) {
              MspDebugPrint ((MSP_DBG_DEBUG, "        -dqMargin[%d]\n", Dq));
              MspDebugPrint ((MSP_DBG_DEBUG, "          rxDqLeft   = %02d\n", BdatRank->rankMargin.rxDqLeft));
              MspDebugPrint ((MSP_DBG_DEBUG, "          rxDqRight  = %02d\n", BdatRank->rankMargin.rxDqRight));
              MspDebugPrint ((MSP_DBG_DEBUG, "          txDqLeft   = %02d\n", BdatRank->rankMargin.txDqLeft));
              MspDebugPrint ((MSP_DBG_DEBUG, "          txDqRight  = %02d\n", BdatRank->rankMargin.txDqRight));
              MspDebugPrint ((MSP_DBG_DEBUG, "          rxVrefLow  = %02d\n", BdatRank->rankMargin.cmdLeft));
              MspDebugPrint ((MSP_DBG_DEBUG, "          rxVrefHigh = %02d\n", BdatRank->rankMargin.cmdRight));
              MspDebugPrint ((MSP_DBG_DEBUG, "          txVrefLow  = %02d\n", BdatRank->rankMargin.recvenLeft));
              MspDebugPrint ((MSP_DBG_DEBUG, "          txVrefHigh = %02d\n", BdatRank->rankMargin.recvenRight));
            }
          }

          MspDebugPrint ((MSP_DBG_DEBUG, "        -rankTraining\n"));
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            MspDebugPrint ((MSP_DBG_DEBUG, "          recEnDelay[%02d] = %04d\n", 
              Strobe, BdatRank->rankTraining.recEnDelay[Strobe]
              ));
          } // Strobe loop ...
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            MspDebugPrint ((MSP_DBG_DEBUG, "          wlDelay[%02d]    = %04d\n", 
              Strobe, BdatRank->rankTraining.wlDelay[Strobe]
              ));
          } // Strobe loop ...
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            MspDebugPrint ((MSP_DBG_DEBUG, "          rxDqDelay[%02d]  = %02d\n", 
              Strobe, BdatRank->rankTraining.rxDqDelay[Strobe]
              ));
          } // Strobe loop ...
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            MspDebugPrint ((MSP_DBG_DEBUG, "          txDqDelay[%02d]  = %02d\n", 
              Strobe, BdatRank->rankTraining.txDqDelay[Strobe]
              ));
          } // Strobe loop ...

          MspDebugPrint ((MSP_DBG_DEBUG, "          clkDelay  = %03d\n", BdatRank->rankTraining.clkDelay));
          MspDebugPrint ((MSP_DBG_DEBUG, "          ctlDelay  = %03d\n", BdatRank->rankTraining.ctlDelay));
          MspDebugPrint ((MSP_DBG_DEBUG, "          cmdDelay  = %03d : %03d : %03d\n", 
            BdatRank->rankTraining.cmdDelay[0], BdatRank->rankTraining.cmdDelay[1], BdatRank->rankTraining.cmdDelay[2]
            ));
          MspDebugPrint ((MSP_DBG_DEBUG, "          ioLatency = %03d\n", BdatRank->rankTraining.ioLatency));
          MspDebugPrint ((MSP_DBG_DEBUG, "          roundtrip = %03d\n", BdatRank->rankTraining.roundtrip));

          for (DramDevice = 0; DramDevice < MAX_DRAM_DEVICE; DramDevice++) {
            MspDebugPrint ((MSP_DBG_DEBUG, "        -rankMRS[%d]\n", DramDevice));
            for (ModeRegister = 0; ModeRegister < 4; ModeRegister++) {
              MspDebugPrint ((MSP_DBG_DEBUG, "          modeRegister[%d] = 0x%04x\n", 
                ModeRegister, BdatRank->rankMRS[DramDevice].modeRegister[ModeRegister]
                ));
            }
          } // DramDevice loop
        } // Rank loop
      } // Dimm loop
    } // Channel loop
  } // Node loop
}

/**
  This function fills in both v1.0 and v2.0 of the BDAT data structure per
  v0.6 of "A Compatible Method to Access BIOS Data Structures".

  @param[in, out] MrcData:                    The Host structure for all data related to MMRC.
  @param[in]      BdatMemoryData2Structure:   The BDAT_MEMORY_DATA_2_STRUCTURE structure to be initialized.
  @retval         NONE
**/
VOID
FillBdatStructure (
  IN  OUT   MMRC_DATA                       *MrcData,
      OUT   BDAT_MEMORY_DATA_2_STRUCTURE    *BdatMemoryData2Structure
  )
{
  UINT8     Node;
  UINT8     Channel;
  UINT8     Dimm;
  UINT8     Rank;
  UINT32    MaxSpdElements;
  UINT16    SpdElement;
  UINT16    *RelevantSpdBytes;
  UINT32    GetValue;
  UINT8     Strobe;
  UINT8     DramDevice;
  UINT8     ModeRegister;
  EFI_GUID  BdatMemorySchemaGuid = BDAT_MEMORY_DATA_2_GUID;
  
  BDAT_SOCKET_2_STRUCTURE     *BdatSocket;
  BDAT_CHANNEL_2_STRUCTURE    *BdatChannel;
  BDAT_DIMM_2_STRUCTURE       *BdatDimm;
  BDAT_RANK_2_STRUCTURE       *BdatRank;
  DIMM_VOLTAGE_FLAGS          *DimmVoltageFlags;

  DimmVoltageFlags = (DIMM_VOLTAGE_FLAGS *) &MrcData->DimmVoltageFlags;

  MspDebugPrint ((MSP_DBG_MIN, "Filling BDAT Memory Schema 2 Structure\n"));

  MmrcMemset (BdatMemoryData2Structure, 0, sizeof (BDAT_MEMORY_DATA_2_STRUCTURE));
  //
  // BDAT_SCHEMA_HEADER_STRUCTURE
  //
  BdatMemoryData2Structure->schemaHeader.SchemaId = BdatMemorySchemaGuid;
  BdatMemoryData2Structure->schemaHeader.DataSize = sizeof (BDAT_MEMORY_DATA_2_STRUCTURE);
  //
  // Crc16 will be updated later after SCHEMA is filled up completely
  //
  BdatMemoryData2Structure->schemaHeader.Crc16    = 0;
  //
  // General data for the Memory Schema 2 structure
  // 
  BdatMemoryData2Structure->refCodeRevision   = MRC_VERSION;
  BdatMemoryData2Structure->maxNode           = BDAT_MAX_NODES;
  BdatMemoryData2Structure->maxCh             = BDAT_MAX_CHANNELS;
  BdatMemoryData2Structure->maxDimm           = BDAT_MAX_DIMM;
  BdatMemoryData2Structure->maxRankDimm       = BDAT_MAX_RANK_DIMM;
  BdatMemoryData2Structure->maxStrobe         = BDAT_MAX_STROBES;
  BdatMemoryData2Structure->maxDq             = BDAT_MAX_DQ;
  BdatMemoryData2Structure->marginLoopCount   = MrcData->MrcTrainingCpgcExpLoopCnt;

  for (Node = 0; Node < BDAT_MAX_NODES; Node++) {
    BdatSocket = (BDAT_SOCKET_2_STRUCTURE *) &BdatMemoryData2Structure->socketList[Node].imcEnabled;
    //
    // Socket Information
    //
    BdatSocket->imcEnabled    = 1;
    BdatSocket->imcDid        = MrcData->CpuType;
    BdatSocket->imcRid        = MrcData->SocStepping;
    //
    // DDR frequency in units of MHz / 10
    // e.g. ddrFreq = 13333 for tCK = 1.5 ns
    //
    switch (MrcData->MrcParameters.DdrFrequency) {
    case MMRC_800:
      BdatSocket->ddrFreq     = 8000;
      break;
    case MMRC_1066:
      BdatSocket->ddrFreq     = 10666;
      break;
    case MMRC_1333:
      BdatSocket->ddrFreq     = 13333;
      break;
    case MMRC_1600:
      BdatSocket->ddrFreq     = 16000;
      break;
    case MMRC_1866:
      BdatSocket->ddrFreq     = 18666;
      break;
    case MMRC_2133:
      BdatSocket->ddrFreq     = 21333;
      break;
    case MMRC_2400:
      BdatSocket->ddrFreq     = 24000;
      break;
    case MMRC_2666:
      BdatSocket->ddrFreq     = 26666;
      break;
    case MMRC_2933:
      BdatSocket->ddrFreq     = 29333;
      break;
    case MMRC_3200:
      BdatSocket->ddrFreq     = 32000;
      break;
    case MMRC_3777:
      BdatSocket->ddrFreq     = 37777;
      break;
    default:
    break;
    }
    //
    // Vdd in units of mV
    // e.g. ddrVoltage = 1350 for Vdd = 1.35 V
    //
    switch (DimmVoltageFlags->FLAGS.RunDimmVoltageAt) {
    case DDR_VOLTAGE_150V:
      BdatSocket->ddrVoltage  = 1500;
      break;
    case DDR_VOLTAGE_135V:
      BdatSocket->ddrVoltage  = 1350;
      break;
    case DDR_VOLTAGE_125V:
      BdatSocket->ddrVoltage  = 1250;
      break;
    default:
      break;
    }
    //
    //           (1/64)* QClock * 2048      (1/64)* QClock * 2048       2048
    // piStep  = ----------------------   =  ----------------------  = -------- = 16
    //                   tCK                       2*QClock             128
    //
    BdatSocket->piStep        = 16; 
    //
    //                step * 100       7.81N * 100
    // rxVrefStep  =  ------------ =  ------------ = 520
    //                    Vdd             1.5
    //
    BdatSocket->rxVrefStep    = 520;
    //
    //                 step * 100       8 * 100
    // txVrefStep  =  ------------ =  ------------ = 533
    //                    Vdd             1.5
    //
    BdatSocket->txVrefStep    = 533;
    BdatSocket->caVrefStep    = 0;
    BdatSocket->recvenStep    = BdatSocket->piStep;
    BdatSocket->wrLevStep     = BdatSocket->piStep;

    for (Channel = 0; Channel < BDAT_MAX_CHANNELS; Channel++) {

      BdatChannel = (BDAT_CHANNEL_2_STRUCTURE *) &BdatSocket->channelList[Channel].chEnabled;
      //
      // Skip if channel is disabled
      //
      if (MrcData->MrcParameters.ChEnabled[Channel] == 0) {
        continue;
      }
      BdatChannel->chEnabled    = 1;
      BdatChannel->numDimmSlot  = BDAT_MAX_DIMM;

      for (Dimm = 0; Dimm < BDAT_MAX_DIMM; Dimm++) {
        BdatDimm  = (BDAT_DIMM_2_STRUCTURE *) &BdatChannel->dimmList[Dimm].dimmEnabled;
        //
        // Skip if no DIMM present
        //
        if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
          continue;
        }
        BdatDimm->dimmEnabled   = 1;

        GetRelevantSpdBytes (MrcData, Channel, &RelevantSpdBytes, &MaxSpdElements);
        if (MaxSpdElements == 0 || RelevantSpdBytes == NULL) {
          MspDebugPrint ((MSP_DBG_MIN, "Skipping fill of SPD data.\n"));
        } else {
          for (SpdElement = 0; SpdElement < MaxSpdElements; SpdElement++) {
            if (RelevantSpdBytes[SpdElement] == 0) {
              continue;  // There are a few values in the "SPD" table that do not actually exist in the SPD.
            }
            BdatDimm->spdBytes.valid[RelevantSpdBytes[SpdElement] / 8] |= 1 << (RelevantSpdBytes[SpdElement] % 8);
            BdatDimm->spdBytes.spdData[RelevantSpdBytes[SpdElement]]    = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[RelevantSpdBytes[SpdElement]];
          }
        }

        for (Rank = 0; Rank < BDAT_MAX_RANK_DIMM; Rank++) {
          BdatRank = (BDAT_RANK_2_STRUCTURE *) &BdatDimm->rankList[Rank].rankEnabled;
          //
          // Skip if no Rank or if it is mapped out
          //
          if (MrcData->MrcParameters.RankPresent[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM] == 0) {
            continue;
          }
          BdatRank->rankEnabled = 1;
          if (MrcData->MrcRmtSupport & RMT_SUPPORT_ENABLED) {
            BdatRank->rankMarginEnabled   = 1;
          }
          BdatRank->dqMarginEnabled       = 0;

          BdatRank->rankMargin.rxDqLeft     = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][RxDqLeft];
          BdatRank->rankMargin.rxDqRight    = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][RxDqRight];
          BdatRank->rankMargin.txDqLeft     = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][TxDqLeft];
          BdatRank->rankMargin.txDqRight    = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][TxDqRight];
          BdatRank->rankMargin.cmdLeft      = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][CmdLeft];
          BdatRank->rankMargin.cmdRight     = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][CmdRight];
          BdatRank->rankMargin.recvenLeft   = 0;
          BdatRank->rankMargin.recvenRight  = 0;
          BdatRank->rankMargin.wrLevLeft    = 0;
          BdatRank->rankMargin.wrLevRight   = 0;
          BdatRank->rankMargin.rxVrefLow    = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][RxVLow];
          BdatRank->rankMargin.rxVrefHigh   = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][RxVHigh];
          BdatRank->rankMargin.txVrefLow    = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][TxVLow];
          BdatRank->rankMargin.txVrefHigh   = (UINT8) MrcData->RmtData[Channel][Rank + Dimm * BDAT_MAX_RANK_DIMM][TxVHigh];
          BdatRank->rankMargin.cmdVrefLow   = 0;
          BdatRank->rankMargin.cmdVrefHigh  = 0;
#ifdef CkAll
          //
          // Get training results
          // CK
          //
          if (GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, 0, CkAll, CMD_GET_REG, &GetValue) == MMRC_SUCCESS) {
            BdatRank->rankTraining.clkDelay     = (UINT8) GetValue;
          }
#endif
#ifdef CtlGrp0
          //
          // CTL
          //
          if (GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, 0, CtlGrp0, CMD_GET_REG, &GetValue) == MMRC_SUCCESS) {
            BdatRank->rankTraining.ctlDelay     = (UINT8) GetValue;
          }
#endif
#ifdef CmdGrp0
          //
          // CMD
          //
          if (GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, 0, CmdGrp0, CMD_GET_REG, &GetValue) == MMRC_SUCCESS) {
            BdatRank->rankTraining.cmdDelay[0]  = (UINT8) GetValue;
          }
#endif
#ifdef CmdGrp1
          if (GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, 0, CmdGrp1, CMD_GET_REG, &GetValue) == MMRC_SUCCESS) {
            BdatRank->rankTraining.cmdDelay[1]  = (UINT8) GetValue;
          }
#endif
          BdatRank->rankTraining.cmdDelay[2]  = 0xFF;

          BdatRank->rankTraining.ioLatency    = 0;
          BdatRank->rankTraining.roundtrip    = 0;

          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if (!IsStrobeValid (MrcData, Channel, Rank, Strobe, FALSE, NULL)) {
              continue;
            }
            //
            // Receive Enable
            //
            GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, Strobe, RecEnDelay, CMD_GET_REG, &GetValue);
            BdatRank->rankTraining.recEnDelay[Strobe]   = (UINT16) GetValue;
            //
            // Write Leveling
            //
            GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, Strobe, TxDqsDelay, CMD_GET_REG, &GetValue);
            BdatRank->rankTraining.wlDelay[Strobe]      = (UINT16) GetValue;
            //
            // Rx Dq 
            //
            GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, Strobe, RxDqsDelay, CMD_GET_REG, &GetValue);
            BdatRank->rankTraining.rxDqDelay[Strobe]    = (UINT8) GetValue;
            //
            // Tx Dq
            //
            GetSetDdrioGroup2 (MrcData, Channel, Rank + Dimm * BDAT_MAX_RANK_DIMM, Strobe, TxDqDelay, CMD_GET_REG, &GetValue);
            BdatRank->rankTraining.txDqDelay [Strobe]   = (UINT8) GetValue;
          } // Strobe loop ...

          for (DramDevice = 0; DramDevice < MAX_DRAM_DEVICE; DramDevice++) {
            for (ModeRegister = 0; ModeRegister < 4; ModeRegister++) {
              BdatRank->rankMRS[DramDevice].modeRegister[ModeRegister] = (MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank + Dimm * (MAX_RANKS_PER_DIMM)][ModeRegister]] >> 6) & 0xffff;
            }
          }
        } // Rank loop
      } // Dimm loop
    } // Channel loop
  } // Node loop
  //
  // Updating Crc16 for Bdat Memory Schema 2
  //
  BdatMemoryData2Structure->schemaHeader.Crc16    = (UINT16) Crc16 ((INT8 *)BdatMemoryData2Structure, sizeof (BDAT_MEMORY_DATA_2_STRUCTURE));

  PrintMemoryData2Structure (BdatMemoryData2Structure);

} // FillBdatStructure

/**
  This function returns the CRC16 of the provided array.

  @param[in] Ptr     The array to be processed.
  @param[in] Count   Number of elements in the array.
  @retval            CRC16 of the provided array.
**/
INT16
Crc16 (
  IN INT8  *Ptr,
  IN INT32 Count
  )
{
  INT16 crc, i;
  crc = 0;
  while (--Count >= 0) {
    crc = crc ^ (INT16) (int) * Ptr++ << 8;
    for (i = 0; i < 8; ++i) {
      if (crc & 0x8000) {
        crc = crc << 1 ^ 0x1021;
      } else {
        crc = crc << 1;
      }
    }
  }
  return (crc & 0xFFFF);
}
#endif  //  BDAT