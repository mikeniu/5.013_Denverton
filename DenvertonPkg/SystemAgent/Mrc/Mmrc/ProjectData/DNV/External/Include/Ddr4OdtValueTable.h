/** @file 
  Ddr4OdtValueTable.h
  DDR4 ODT Table Definition

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef DDR4_ODT_VALUE_TABLE_H
#define DDR4_ODT_VALUE_TABLE_H

#include "MemOdt.h"
#if ODT_TABLE

struct ddr4OdtValueStruct ddr4OdtValueTable[] = {
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (SR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (SR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (SR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (SR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (SR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10),                 // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (DR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (DR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (DR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (DR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (EMPTY_DIMM << 7)     |             // slot0
      (DR_DIMM << 10),                    // slot1
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_80 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      76,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      76,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      76,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      76,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,        // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS         // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      76,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1600 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (MMRC_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)   ,                 // slot1
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      80,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_120,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
};

#endif // ODT_TABLE

#endif //DDR4_ODT_VALUE_TABLE_H
