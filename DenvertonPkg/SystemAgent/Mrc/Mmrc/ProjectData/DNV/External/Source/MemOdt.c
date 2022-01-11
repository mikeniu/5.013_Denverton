/**  @file
  MemODT.c
  This file contain functions for parsing the DIMM population and
  define the DRAM and CPU ODT settings.

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

//
// Disable warning for unsued input parameters
//
#pragma warning(disable : 4100)

#include "MemOdt.h"
#include "DnvOdtValueTable.h"
#include "DnvOdtActivationTable.h"
#include "Ddr4OdtValueTable.h"
#include "Ddr4OdtActivationTable.h"


#if ODT_TABLE

ODT_VALUE_INDEX GetOdtValueIndex (
  IN  OUT   MMRC_DATA   *MrcData,
  UINT8    socket,
  UINT8    Channel,
  UINT8    freqLimit
  )
{
  ODT_VALUE_INDEX Config;
  UINT8 Slot;
  UINT8 Dimm;

  Slot = EMPTY_DIMM;
  //
  // Construct config index
  //
  Config.Data = 0;
  Config.Bits.Freq = MrcData->MrcParameters.DdrFrequency;
  if (Config.Bits.Freq > freqLimit) {
    Config.Bits.Freq = freqLimit;
  }
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {

    if (MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
      //
      // Currently no support for load reduced DIMMs
      //
      if (0) {
        Slot = LR_DIMM;
      } else {
        //
        // How many ranks are we dealing with for this DIMM?
        //
        switch (MrcData->D_Ranks[Channel][Dimm]) {
        case 0:
          Slot = SR_DIMM;
          break;
        case 1:
          Slot = DR_DIMM;
          break;
        case 3:
          Slot = QR_DIMM;
          break;
        default:
          MrcData->ErrorCode = MrcErrInvalidRankNumber;
          MrcDeadLoop();
        }
      }
    } else {
      Slot = EMPTY_DIMM;
    }

    switch (Dimm) {
    case 0:
      Config.Bits.Slot0 = Slot;
      break;
    case 1:
      Config.Bits.Slot1 = Slot;
      break;
    case 2:
      Config.Bits.Slot2 = Slot;
      break;
    default:
      MrcData->ErrorCode = MrcErrInvalidDimmNumber;
      MrcDeadLoop();
    }
  }

  return Config;
}

ODT_VALUE_STRUCT *
LookupOdtValue (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8                 Node,
  IN        UINT8        Channel
)
{
  ODT_VALUE_INDEX   Config;
  ODT_VALUE_STRUCT  *OdtValueTablePtr;
  UINT8             Dimm;
  UINT32            i;
  UINT8             Rank;


  OdtValueTablePtr = (ODT_VALUE_STRUCT *) OdtValueTable;
  //
  // DDR3 has max frequency of 1866
  //
  Config = GetOdtValueIndex (MrcData, Node, Channel, MMRC_1866);
  //
  // Now lookup the config...
  //
  for (i = 0; i < sizeof(OdtValueTable) / sizeof(struct odtValueStruct) ; i++) {
    if (OdtValueTablePtr[i].Config == Config.Data) {
      break;
    }
  }
  //
  // Check for error
  //
  if (i >= sizeof(OdtValueTable) / sizeof(struct odtValueStruct)) {
    return NULL;
  }
  MspDebugPrint ((MSP_DBG_MAX, "Channel %d\n", Channel));
  MspDebugPrint ((MSP_DBG_MAX, "Config.Freq %d\n", Config.Bits.Freq));
  MspDebugPrint ((MSP_DBG_MAX, "Config.Slot0 %d\n", Config.Bits.Slot0));
  MspDebugPrint ((MSP_DBG_MAX, "Config.Slot1 %d\n", Config.Bits.Slot1));
  MspDebugPrint ((MSP_DBG_MAX, "Config.Slot2 %d\n", Config.Bits.Slot2));
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
      MspDebugPrint ((MSP_DBG_MAX, "DramOdt[%d][%d] = %02x\n", Dimm, Rank, OdtValueTable[i].DramOdt[Dimm][Rank]));
    }
  }
  //
  // Return pointer
  //
  return &OdtValueTable[i];
}

struct ddr4OdtValueStruct *
LookupDdr4OdtValue (
  IN  OUT   MMRC_DATA   *MrcData,
  UINT8    socket,
  UINT8    ch
  )
{
  ODT_VALUE_INDEX config;
  UINT32 i;
  UINT16 ddr4OdtValueStructSize;
  struct ddr4OdtValueStruct* ddr4OdtValueStructPtr;

  ddr4OdtValueStructPtr = &ddr4OdtValueTable[0];
  ddr4OdtValueStructSize = sizeof (ddr4OdtValueTable);

  //OemLookupDdr4OdtTable(host, &ddr4OdtValueStructPtr, &ddr4OdtValueStructSize);
  //
  // Construct config index. DDR4 has max speed of 2400
  //
  config = GetOdtValueIndex (MrcData, socket, ch, MMRC_2666);
  //
  // Now lookup the config...
  //
  for (i = 0; i < ddr4OdtValueStructSize / sizeof(struct ddr4OdtValueStruct); i++) {
    if (ddr4OdtValueStructPtr->config == config.Data) {
      break;
    }
    ddr4OdtValueStructPtr++;
  }
  //
  // Check for error. Unsupported configuration
  //
  if (i >= ddr4OdtValueStructSize / sizeof(struct ddr4OdtValueStruct)) {
    //OutputWarning (host, WARN_DIMM_COMPAT, WARN_CHANNEL_CONFIG_NOT_SUPPORTED, socket, ch, 0xFF, 0xFF);
    return NULL;
  }
  //
  // Return pointer
  //
  return ddr4OdtValueStructPtr;
}


ODT_ACT_STRUCT *
LookupOdtAct (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Node,
  IN        UINT8        Channel,
  IN        UINT8        ThisDimm,
  IN        UINT8        ThisRank
)
{
  ODT_ACT_INDEX   Config;
  ODT_ACT_STRUCT  *OdtActTablePtr;
  UINT8           Dimm;
  UINT8           Slot;
  UINT32          i;
  UINT32          odtActTableSize; 

  Slot = EMPTY_DIMM;
  OdtActTablePtr = (ODT_ACT_STRUCT *)OdtActTable;
  odtActTableSize = sizeof(OdtActTable);

  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    OdtActTablePtr = (ODT_ACT_STRUCT *) ddr4OdtActTable;
    odtActTableSize = sizeof(ddr4OdtActTable);
  }

  //
  // Construct config index
  //
  Config.Data = 0;
  Config.Bits.DimmNum = ThisDimm;
  Config.Bits.RankNum = ThisRank;
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {

    if (MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
      //
      // Currently no support for low reduced dimms
      //
      if (0) {
        Slot = LR_DIMM;
        Config.Bits.RankNum = 0;
      } else {
        switch (MrcData->D_Ranks[Channel][Dimm]) {
        case 0:
          Slot = SR_DIMM;
          break;
        case 1:
          Slot = DR_DIMM;
          break;
        case 2:
          Slot = QR_DIMM;
          break;
        default:
          MrcData->ErrorCode = MrcErrInvalidRankNumber;
          MrcDeadLoop();
        }
      }
    } else {
      Slot = EMPTY_DIMM;
    }

    switch (Dimm) {
    case 0:
      Config.Bits.Slot0 = Slot;
      break;
    case 1:
      Config.Bits.Slot1 = Slot;
      break;
    case 2:
      Config.Bits.Slot2 = Slot;
      break;
    default:
      MrcData->ErrorCode = MrcErrInvalidDimmNumber;
      MrcDeadLoop();
    }
  }
  //
  // Now lookup the config...
  //
  for (i = 0; i < odtActTableSize / sizeof(struct odtActStruct); i++) {
    if (OdtActTablePtr[i].Config == Config.Data) {
      break;
    }
  }
  //
  // Check for error
  //
  if (i >= odtActTableSize / sizeof(struct odtActStruct)) {
    return NULL;
  } else {
    //
    // Return pointer
    //
    return &OdtActTable[i];
  }
}

UINT32
IsTermRank (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8                 Node,
  IN        UINT8        Channel,
  IN        UINT8                 ThisDimm,
  IN        UINT8                 ThisRank
)
{
  ODT_ACT_STRUCT  *OdtActPtr;
  UINT8           Dimm;
  UINT8           Rank;
  //
  // Check for non-target termination on this rank. Currently no support for load reduced.
  //
  if ((ThisRank < 2) && (0 /* load reduced present */) ) {
    return 1;
  }
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    for (Rank = 0; Rank < MAX_RANK_DIMM; Rank++) {

      if (!MrcData->MrcParameters.RankPresent[Channel][ (2 * Dimm) + Rank]) {
        continue;
      }
      //
      // Skip target rank
      //
      if ((Dimm == ThisDimm) && (Rank == ThisRank) ) {
        continue;
      }
      //
      // Lookup ODT activations for this config
      //
      OdtActPtr = LookupOdtAct (MrcData, Node, Channel, Dimm, Rank);

      if (OdtActPtr != NULL) {
        if ((OdtActPtr->ActBits[0] & (BIT0 << ((ThisDimm * MAX_RANK_DIMM) + ThisRank) ) ) ||
             (OdtActPtr->ActBits[1] & (BIT0 << ((ThisDimm * MAX_RANK_DIMM) + ThisRank) ) ) ) {
          return 1;
        }
      }
    }
  }

  return 0;
}

/**
  Set MCODT value to 60 or 120 ohms

  @param[in, out]  MrcData          Host structure for all data related to MMRC.
  @param[in]       Node             Socket number
  @param[in]       Channel          Channel number (0-based)
  @param[in]       McOdt            0 = 60 ohms; 1 = 120 ohms (HW default)
  @retval          NONE
**/
VOID
SetMcOdtValue (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Node,
  IN        UINT8       Channel,
  IN        UINT8       McOdt
  )
{
  if (McOdt == MCODT_60) {
    //MrcData->MspData.DynamicVars[Channel][PULLUP_EXT] = 0x1A;
  } else {
    //MrcData->MspData.DynamicVars[Channel][PULLUP_EXT] = 0x25;
  }
  //MrcData->MspData.DynamicVars[Channel][PULLDOWN_INT] = 0xC;
}

/**
  DramVrefToEncode: Coverts the Dram Vref % to the corresponding encoding described on the DDR4 Jedec
  79-4A

@param[in]       InVref           Vref in % 
@retval          OutVref          Corresponding Vref encoding [A6:A0]
**/
UINT8
DramVrefToEncode (
IN  UINT8  InVref,
OUT UINT8  *Range
)
{
  UINT8 OutVref;
  if (InVref < DDR4_VREF_RANGE1_OFFSET) {
    //
    //range 2
    //
    InVref -= DDR4_VREF_RANGE2_OFFSET;
    *Range  = 1;
  } else {
    //
    //range 1
    //
    InVref -= DDR4_VREF_RANGE1_OFFSET;
    *Range  = 0;
  }
  OutVref = (((InVref * 100) / DDR4_VREF_RANGE_STEP) + 1);

  return ((OutVref > 50) ? 50 : OutVref);
}

UINT8
CpuVrefPercentToEncode(
IN  UINT8  InVref
)
{
  UINT8 OutVref;
  UINT8 Decrement;

  //
  // The RxVref table is mostly linear and all percentages duplicated except 3
  //
  Decrement = 0;
  if (InVref > 65) {
    Decrement++;
  }

  if (InVref > 80) {
    Decrement++;
  }

  if (InVref > 93) {
    Decrement++;
  }

  InVref -= DDR4_CPU_VREF_OFFSET;
  OutVref = (InVref * 2) - Decrement;

  return ((OutVref > 96) ? 96 : OutVref);
}



/**
  Programs the ODT matrix

  @param[in, out]  MrcData          Host structure for all data related to MMRC.
  @param[in]       Node             Socket number
  @param[in]       Channel          Channel number (0-based)
  @retval          NONE
**/
VOID
SetOdtMatrix (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Node,
  IN        UINT8        Channel,
  IN  OUT   UINT32       MrsValue[MAX_RANKS][MR_COUNT]
  )
{
  UINT8                 Dimm;
  UINT8                 Rank;
  UINT8                 TableVal;
  ODT_VALUE_STRUCT      *OdtValuePtr;
  DDR4_ODT_VALUE_STRUCT *ddr4OdtValuePtr;
  ODT_VALUE_RTT_DDR4    ddr4TableVal;
  UINT8                 Range;
  UINT8                 Strobe;

  Strobe = 0;
  switch(MrcData->MrcParameters.DramType[Channel]) {
  case TypeDdr3 :
  case TypeDdr3L:
  case TypeDdr3U:
  case TypeDdr3All:
    //
    // Lookup ODT values for this config
    //
    OdtValuePtr = LookupOdtValue (MrcData, Node, Channel);
    if (OdtValuePtr == NULL){
      MrcData->ErrorCode = MrcErrNullPointer;
      MspDebugPrint ((MSP_DBG_MAX, "DDR3: ODT Ptr is NULL at Node:%d Channel:%d\n", Node, Channel));
      MrcDeadLoop ();
    } else {
    //
    // Set MCODT values for this channel
    //
    SetMcOdtValue (MrcData, Node, Channel, OdtValuePtr->McOdt);
    //
    // Loop for each DIMM
    //
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      //
      // Skip if no DIMM present
      //
      if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        continue;
      }
        
      //
      // Loop for each rank
      //
      for (Rank = 0; Rank < MAX_RANK_DIMM; Rank++) {
        //
        // Skip if no rank
        //
        if (MrcData->MrcParameters.RankPresent[Channel][ (2 * Dimm) + Rank] == 0) {
          continue;
        }
        //
        // Get the ODT values
        //
        TableVal = OdtValuePtr->DramOdt[Dimm][Rank];
    
#ifdef  LRDIMM_SUPPORT
        if (host->nvram.common.imc[node].channelList[ch].lrDimmPresent) {
    
          // LRDIMM uses SPD data for DRAM Rtt_nom and Rtt_wr fields
          if (rank < 2) {
            tableVal = ((*dimmNvList) [dimm].spdLrBuf_MR1_2_RTT >> 2) & 0x37;
          } else {
            // DRAM Rtt_nom must be disabled for ranks > 1
            tableVal = ((*dimmNvList) [dimm].spdLrBuf_MR1_2_RTT >> 2) & 0x30;
          }
        }
#endif  //LRDIMM_SUPPORT
   
        //
        // Set encodings for DRAM Mode Registers
        //
        MrsValue[(2 * Dimm) + Rank][2] = 0;
        if ((TableVal & 0xF0) ==  (RTT_WR_60 << 4)) {
          MrsValue[(2 * Dimm) + Rank][2] |= (RTT_WR_ODT_60 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_WR_60\n", Channel, Dimm, Rank));
        } else if ((TableVal & 0xF0) == (RTT_WR_120 << 4)) {
          MrsValue[(2 * Dimm) + Rank][2] |= (RTT_WR_ODT_120 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_WR_120\n", Channel, Dimm, Rank));
        }
        MrsValue[(2 * Dimm) + Rank][1] = 0;
        if ((TableVal & 0x0F) == RTT_NOM_60) {
          MrsValue[(2 * Dimm) + Rank][1] |= (RTT_NOM_ODT_60 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_NOM_ODT_60 \n", Channel, Dimm, Rank));
        } else if ((TableVal & 0x0F) == RTT_NOM_120) {
          MrsValue[(2 * Dimm) + Rank][1] |= (RTT_NOM_ODT_120 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_NOM_ODT_120 \n", Channel, Dimm, Rank));
        } else if ((TableVal & 0x0F) == RTT_NOM_20) {
          MrsValue[(2 * Dimm) + Rank][1] |= (RTT_NOM_ODT_20 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_NOM_ODT_20 \n", Channel, Dimm, Rank));
        } else if ((TableVal & 0x0F) == RTT_NOM_40) {
          MrsValue[(2 * Dimm) + Rank][1] |= (RTT_NOM_ODT_40 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_NOM_ODT_40 \n", Channel, Dimm, Rank));
        } else if ((TableVal & 0x0F) == RTT_NOM_30) {
          MrsValue[(2 * Dimm) + Rank][1] |= (RTT_NOM_ODT_30 << 6);
          MspDebugPrint ((MSP_DBG_MAX, "C%dD%dR%d RTT_NOM_ODT_30 \n", Channel, Dimm, Rank));
        }
      } // target rank
    } // target dimm
    }
  break;
  case TypeDdr4:
    ddr4OdtValuePtr = LookupDdr4OdtValue(MrcData, Node, Channel);

    if (ddr4OdtValuePtr == NULL) {
      MrcData->ErrorCode = MrcErrNullPointer;
      MspDebugPrint ((MSP_DBG_MAX, "DDR4: ODT Ptr is NULL at Node:%d Channel:%d\n", Node, Channel));
      MrcDeadLoop ();
    } else {
    //
    // Loop for each DIMM
    //
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      //
      // Skip if no DIMM present
      //
      if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        continue;
      }
      //
      // Loop for each rank
      //
      for (Rank = 0; Rank < MAX_RANK_DIMM; Rank++) {
        //
        // Get the ODT values
        //
        ddr4TableVal = *(ODT_VALUE_RTT_DDR4 *)&ddr4OdtValuePtr->dramOdt[Dimm][Rank];
        MrsValue[(2 * Dimm) + Rank][1] = (ddr4TableVal.Bits.rttNom << (CMD_ADDRESS_OFFSET + RTT_NOM_OFFSET));
        MrsValue[(2 * Dimm) + Rank][2] = (ddr4TableVal.Bits.rttWr  << (CMD_ADDRESS_OFFSET + RTT_WR_OFFSET));
        MrsValue[(2 * Dimm) + Rank][5] = (ddr4TableVal.Bits.rttPrk << (CMD_ADDRESS_OFFSET + RTT_PARK_OFFSET));
        MrsValue[(2 * Dimm) + Rank][6] = (0x3F & DramVrefToEncode(ddr4OdtValuePtr->dramVref, &Range)) << CMD_ADDRESS_OFFSET;

        for(Strobe = 0; Strobe <MAX_STROBES; Strobe++) {
          if (Range == 0) {
            MrcData->CacheTxVref[Channel][(2 * Dimm) + Rank][Strobe] = DramVrefToEncode(ddr4OdtValuePtr->dramVref, &Range) + 23;
          } else {
            MrcData->CacheTxVref[Channel][(2 * Dimm) + Rank][Strobe] = DramVrefToEncode(ddr4OdtValuePtr->dramVref, &Range);
          }
        }

        MrsValue[(2 * Dimm) + Rank][6] |= (Range & 1) << 6;
        MrcData->MspData.DynamicVars[Channel][RXVREF] = CpuVrefPercentToEncode(ddr4OdtValuePtr->mcVref);
      } // Rank
    } // Dimm
     }
    break;
  default :
    //
    // This should never happen. Return error or set default value
    //
    break;
  } // switch
}

#endif // ODT_TABLE
