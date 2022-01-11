/** @file
  MemODT.h
  This file contains definitions needed for porting the generic
  implementation of memory ODT.

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MEMODT_H
#define _MEMODT_H

#include "Mmrc.h"

#pragma warning (disable : 4214 )

#define MAX_RANK_DIMM MAX_RANKS_PER_DIMM
#define DDR4_VREF_RANGE1_OFFSET   60
#define DDR4_VREF_RANGE2_OFFSET   45
#define DDR4_VREF_RANGE_STEP      65
#define DDR4_CPU_VREF_OFFSET      63
//
// ODT equates used to program mode registers
//
#define  DRAM_ODT_DIS     0
#define  RTT_WR_ODT_60    BIT9
#define  RTT_WR_ODT_120   BIT10
#define  RTT_WR_ODT_MASK  (BIT10 | BIT9)
#define  RTT_NOM_ODT_60   BIT2
#define  RTT_NOM_ODT_120  BIT6
#define  RTT_NOM_ODT_40   (BIT6 | BIT2)
#define  RTT_NOM_ODT_20   BIT9
#define  RTT_NOM_ODT_30   (BIT9 | BIT2)
#define  RTT_NOM_ODT_MASK (BIT9 | BIT6 | BIT2)

#define  SLOT_NOT_PRESENT  0
#define  EMPTY_DIMM        1
#define  SR_DIMM           2
#define  DR_DIMM           3
#define  QR_DIMM           4
#define  LR_DIMM           5

#define  DIMM0             0
#define  DIMM1             1
#define  DIMM2             2

#define  RANK0             0
#define  RANK1             1
#define  RANK2             2
#define  RANK3             3

#define  MCODT_60          0
#define  MCODT_120         1

#define  MCODT_50          0
#define  MCODT_100         1

#define  MCODT_ACT         1
#define  ODT_ACT           1
#define  NO_ACT            0

#define  RTT_WR_DIS        0
#define  RTT_WR_INF        0
#define  RTT_WR_60         BIT0
#define  RTT_WR_120        BIT1

#define  RTT_NOM_DIS       0
#define  RTT_NOM_INF       0
#define  RTT_NOM_60        BIT0
#define  RTT_NOM_120       BIT1
#define  RTT_NOM_40        (BIT1 | BIT0)
#define  RTT_NOM_20        BIT2
#define  RTT_NOM_30        (BIT2 | BIT0)

#define  DDR3_800          MMRC_800
#define  DDR3_1066         MMRC_1066
#define  DDR3_1333         MMRC_1333
#define  DDR3_1600         MMRC_1600
#define  DDR3_1866         MMRC_1866

//
// DDR4 ODT support
//
// DDR4 Rtt_wr
//
#define  DDR4_RTT_WR_DIS    0
#define  DDR4_RTT_WR_120    BIT0
#define  DDR4_RTT_WR_240    BIT1
#define  DDR4_RTT_WR_INF    (BIT1 | BIT0)
//
// DDR4 Rtt_nom
//
#define  DDR4_RTT_NOM_DIS   0
#define  DDR4_RTT_NOM_INF   0
#define  DDR4_RTT_NOM_60    BIT0
#define  DDR4_RTT_NOM_120   BIT1
#define  DDR4_RTT_NOM_40    (BIT1 | BIT0)
#define  DDR4_RTT_NOM_240   BIT2
#define  DDR4_RTT_NOM_48    (BIT2 | BIT0)
#define  DDR4_RTT_NOM_80    (BIT2 | BIT1)
#define  DDR4_RTT_NOM_34    (BIT2 | BIT1 | BIT0)
//
// DDR4 Rtt_park
//
#define  DDR4_RTT_PARK_DIS   0
#define  DDR4_RTT_PARK_INF   0
#define  DDR4_RTT_PARK_60    BIT0
#define  DDR4_RTT_PARK_120   BIT1
#define  DDR4_RTT_PARK_40    (BIT1 | BIT0)
#define  DDR4_RTT_PARK_240   BIT2
#define  DDR4_RTT_PARK_48    (BIT2 | BIT0)
#define  DDR4_RTT_PARK_80    (BIT2 | BIT1)
#define  DDR4_RTT_PARK_34    (BIT2 | BIT1 | BIT0)

//
// DDR4 MRS Offsets
//
#define CMD_ADDRESS_OFFSET       8
#define RTT_NOM_OFFSET           8
#define RTT_WR_OFFSET            9
#define RTT_PARK_OFFSET          6

typedef struct odtValueStruct{
  UINT16 Config;                                  ///> ODT_VALUE_INDEX
  UINT8  McOdt;                                   ///> bit[0] = 0 for 50 ohms; 1 for 100 ohms
  UINT8  DramOdt[MAX_DIMMS][MAX_RANKS_PER_DIMM];  ///> bits[2:0] = Rtt_nom value matching JEDEC spec
                                                  ///> bits[5:4] = Rtt_wr value matching JEDEC spec
} ODT_VALUE_STRUCT;


typedef struct odtActStruct{
  UINT16 Config;                                  ///> ODT_ACT_INDEX
  UINT16 ActBits[2];                              ///> Bits[3:0]  = D0_R[3:0]
  // Bits[7:4]  = D1_R[3:0]
  // Bits[11:8] = D2_R[3:0]
  // Bit[15] = MCODT
} ODT_ACT_STRUCT;

typedef union {
  struct {
    UINT16 Freq: 4;     // Frequency enum - DDR3_800, 1067, 1333, 1600, 1867, 2133
    UINT16 Rsvd: 3;     // Reserved for now
    UINT16 Slot0: 3;    // DIMM slot 0 enum - Not present, Empty, SR, DR, QR, LR
    UINT16 Slot1: 3;    // DIMM slot 1 enum - Not present, Empty, SR, DR, QR, LR
    UINT16 Slot2: 3;    // DIMM slot 2 enum - Not present, Empty, SR, DR, QR, LR
  } Bits;
  UINT16 Data;
} ODT_VALUE_INDEX;

typedef union {
  struct {
    UINT16 DimmNum: 2;   // Target DIMM number on the channel
    UINT16 RankNum: 2;   // Target Rank number on the DIMM
    UINT16 Rsvd: 3;
    UINT16 Slot0: 3;     // DIMM slot 0 enum - Not present, Empty, SR, DR, QR, LR
    UINT16 Slot1: 3;     // DIMM slot 1 enum - Not present, Empty, SR, DR, QR, LR
    UINT16 Slot2: 3;     // DIMM slot 2 enum - Not present, Empty, SR, DR, QR, LR
  } Bits;
  UINT16 Data;
} ODT_ACT_INDEX;

//
// DDR4 support
//
typedef struct ddr4OdtValueStruct {
   UINT16 config;                            ///> ODT_VALUE_INDEX
   UINT8 mcOdt;                              ///> bit[0] = 0 for 50 ohms; 1 for 100 ohms
   UINT8 mcVref;                             ///> Percent Vddq formula by MC spec
                                             ///> ReadVrefSel = (percent * 192 / Vdd) - 128
   UINT8 dramVref;                           ///> Percent Vddq as defined by JEDEC spec
   UINT8 dramOdt[MAX_DIMMS][MAX_RANK_DIMM*2];  ///> ODT_VALUE_RTT_DDR4
} DDR4_ODT_VALUE_STRUCT;

typedef union {
   struct{
      UINT8 rttNom:3;      // Rtt_nom value matching JEDEC spec
      UINT8 rttPrk:3;      // Rtt_park value matching JEDEC spec
      UINT8 rttWr:2;       // Rtt_wr value matching JEDEC spec
   } Bits;
   UINT8 Data;
} ODT_VALUE_RTT_DDR4;


#endif
