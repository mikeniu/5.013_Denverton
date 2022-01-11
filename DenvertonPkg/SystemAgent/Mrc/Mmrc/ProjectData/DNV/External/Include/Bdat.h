/** @file 
  Bdat.h
  This file contains Initialization of the BDAT data structures for both
  v1.0 and v2.0 taken from v0.6 of "A Compatible Method to Access BIOS
  Data Structures".

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _BDAT_H_
#define _BDAT_H_

#include "MmrcData.h"
#include "Spd.h"
#include "MmrcProjectDefinitions.h"

#pragma pack(1)

#define BDAT_MEMORY_DATA_2_GUID {0xCE3F6794, 0x4883, 0x492C, {0x8D, 0xBA, 0x2F, 0xC0, 0x98, 0x44, 0x77, 0x10}}

typedef struct bdatSchemaHeader {
  EFI_GUID  SchemaId;
  UINT32    DataSize;
  UINT16    Crc16;
} BDAT_SCHEMA_HEADER_STRUCTURE;

#define   MAX_MODE_REGISTER         7     // Number of mode registers
#define   MAX_DRAM_DEVICE           9     //  Maximum number of memory devices
#define   MAX_SPD_BYTE_512          512   //  Number of bytes in Serial EEPROM

#define   BDAT_MAX_NODES            1
#define   BDAT_MAX_CHANNELS         MAX_CHANNELS
#define   BDAT_MAX_DIMM             MAX_DIMMS
#define   BDAT_MAX_RANK_DIMM        MAX_RANKS_PER_DIMM
#define   BDAT_MAX_DQ               72
#define   BDAT_MAX_STROBES          MAX_STROBES

typedef struct {
  UINT16  modeRegister[MAX_MODE_REGISTER];  ///<  Mode register settings
} BDAT_DRAM_MRS_STRUCTURE;

typedef struct {
  UINT8   rxDqLeft;       ///<  Units = piStep
  UINT8   rxDqRight;
  UINT8   txDqLeft;
  UINT8   txDqRight;
  UINT8   rxVrefLow;      ///<  Units = rxVrefStep
  UINT8   rxVrefHigh;
  UINT8   txVrefLow;      ///<  Units = txVrefStep
  UINT8   txVrefHigh;
} BDAT_DQ_MARGIN_STRUCTURE;

typedef struct {
  UINT8   rxDqLeft;       ///<  Units = piStep
  UINT8   rxDqRight;
  UINT8   txDqLeft;
  UINT8   txDqRight;
  UINT8   cmdLeft;
  UINT8   cmdRight;
  UINT8   recvenLeft;     ///<  Units = recvenStep
  UINT8   recvenRight;
  UINT8   wrLevLeft;      ///<  Units = wrLevStep
  UINT8   wrLevRight;
  UINT8   rxVrefLow;      ///<  Units = rxVrefStep
  UINT8   rxVrefHigh;
  UINT8   txVrefLow;      ///<  Units = txVrefStep
  UINT8   txVrefHigh;
  UINT8   cmdVrefLow;     ///<  Units = caVrefStep
  UINT8   cmdVrefHigh;
} BDAT_RANK_MARGIN_STRUCTURE;

typedef struct {
  UINT16  recEnDelay[BDAT_MAX_STROBES];    ///<  Array of nibble training results per rank
  UINT16  wlDelay[BDAT_MAX_STROBES];
  UINT8   rxDqDelay[BDAT_MAX_STROBES];
  UINT8   txDqDelay[BDAT_MAX_STROBES];
  UINT8   clkDelay;
  UINT8   ctlDelay;
  UINT8   cmdDelay[3];
  UINT8   ioLatency;
  UINT8   roundtrip;
} BDAT_RANK_TRAINING_STRUCTURE;

typedef struct {
  UINT8  rankEnabled;           ///<  0 = Rank disabled
  UINT8  rankMarginEnabled;     ///<  0 = Rank margin disabled
  UINT8  dqMarginEnabled;       ///<  0 = Dq margin disabled
  BDAT_RANK_MARGIN_STRUCTURE    rankMargin;                 ///<  Rank margin data
  BDAT_DQ_MARGIN_STRUCTURE      dqMargin[BDAT_MAX_DQ];      ///<  Array of Dq margin data per rank
  BDAT_RANK_TRAINING_STRUCTURE  rankTraining;               ///<  Rank training settings
  BDAT_DRAM_MRS_STRUCTURE       rankMRS[MAX_DRAM_DEVICE];   ///<  Rank MRS settings
} BDAT_RANK_2_STRUCTURE;

typedef struct {
  UINT8  valid[MAX_SPD_BYTE_512/8];   ///<  Each valid bit maps to SPD byte
  UINT8  spdData[MAX_SPD_BYTE_512];   ///<  Array of raw SPD data bytes
} BDAT_SPD_2_STRUCTURE;

typedef struct {
  UINT8   dimmEnabled;                                   ///<  0 = DIMM disabled
  BDAT_RANK_2_STRUCTURE   rankList[BDAT_MAX_RANK_DIMM];  ///<  Array of ranks per DIMM
  BDAT_SPD_2_STRUCTURE    spdBytes;                      ///<  SPD data per DIMM
} BDAT_DIMM_2_STRUCTURE;

typedef struct {
  UINT8  chEnabled;                                      ///<  0 = Channel disabled
  UINT8  numDimmSlot;                                    ///<  Number of slots per channel on the board
  BDAT_DIMM_2_STRUCTURE   dimmList[BDAT_MAX_DIMM];       ///<  Array of DIMMs per channel
} BDAT_CHANNEL_2_STRUCTURE;

typedef struct {
  UINT8   imcEnabled;   ///<  0 = MC disabled
  UINT16  imcDid;       ///<  MC device Id
  UINT8   imcRid;       ///<  MC revision Id	
  UINT16  ddrFreq;      ///<  DDR frequency in units of MHz / 10
                        ///<  e.g. ddrFreq = 13333 for tCK = 1.5 ns
  UINT16  ddrVoltage;   ///<  Vdd in units of mV
                        ///<  e.g. ddrVoltage = 1350 for Vdd = 1.35 V
  UINT8   piStep;       ///<  Step unit = piStep * tCK / 2048
                        ///<  e.g. piStep = 16 for step = 11.7 ps (1/128 tCK)
  UINT16  rxVrefStep;   ///<  Step unit = rxVrefStep * Vdd / 100
                        ///<  e.g. rxVrefStep = 520 for step = 7.02 mV
  UINT16  txVrefStep;   ///<  Step unit = txVrefStep * Vdd / 100
  UINT16  caVrefStep;   ///<  Step unit = caVrefStep * Vdd / 100
  UINT8   recvenStep;   ///<  Step unit = recvenStep * tCK / 2048
  UINT8   wrLevStep;    ///<  Step unit = wrLevStep * tCK / 2048
  BDAT_CHANNEL_2_STRUCTURE  channelList[BDAT_MAX_CHANNELS];   ///<  Array of channels per socket
} BDAT_SOCKET_2_STRUCTURE;

typedef struct {
  BDAT_SCHEMA_HEADER_STRUCTURE  schemaHeader;
  UINT32  refCodeRevision;    ///<  Matches JKT scratchpad definition
  UINT8   maxNode;            ///<  Max processors per system, e.g. 4
  UINT8   maxCh;              ///<  Max channels per socket, e.g. 4
  UINT8   maxDimm;            ///<  Max DIMM per channel, e.g. 3
  UINT8   maxRankDimm;        ///<  Max ranks per DIMM, e.g. 4
  UINT8   maxStrobe;          ///<  Number of Dqs used by the rank, e.g. 18
  UINT8   maxDq;              ///<  Number of Dq bits used by the rank, e.g. 72
  UINT32  marginLoopCount;    ///<  Units of cache line
  BDAT_SOCKET_2_STRUCTURE   socketList[BDAT_MAX_NODES];    ///<  Array of sockets per system
} BDAT_MEMORY_DATA_2_STRUCTURE;

#pragma pack()

/**
  This function returns the CRC16 of the provided array.

  @param[in] Ptr     The array to be processed.
  @param[in] Count   Number of elements in the array.
  @retval            CRC16 of the provided array.
**/
INT16
Crc16 (
  IN        INT8      *Ptr,
  IN        INT32     Count
  );

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
  );

#endif // _BDAT_H_
