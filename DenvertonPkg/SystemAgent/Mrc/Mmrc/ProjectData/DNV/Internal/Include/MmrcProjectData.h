/** @file
  MmrcProjectData.h
  This file contains data structures for AVN silicon.
  These structures are fed to MmrcLibraries.c, which use them to initialize
  memory.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MMRCPROJECTDATA_H_
#define _MMRCPROJECTDATA_H_

#include "MmrcProjectDataGenerated.h"
#include "MmrcProjectDefinitions.h"
#include "MmrcData.h"
#include "Printf.h"

#define MAX_FAILURE_LIST  50
#define PASS_GATE_DEBUG_MSG

#ifndef MAX_EYEMASKS
#define CMD_NUM_EYEMASKS 2
#define MAX_EYEMASKS MAX(MAX(RT_NUM_EYEMASKS, WT_NUM_EYEMASKS), CMD_NUM_EYEMASKS)
#endif

#define MAX_MA_GROUPS_DDR3    3
#define MAX_MA_SIGNALS_DDR3   7

#define MAX_MA_GROUPS_DDR4    3
#define MAX_MA_SIGNALS_DDR4   7

#define MAX_SKEW_SIGNAL       2

#define READ_TRAININ_PN       1

extern       RMT_SIGNAL       RmtTable[MaxRmtElements];

extern MMRC_STATUS SimSetDunitConfigValid (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
#if defined(DDR3_SUPPORT) && DDR3_SUPPORT==1
//
// DDR3L victim group assignments
//
extern CONST UINT8            CadbMaGroupsDdr3[MAX_MA_GROUPS_DDR3][MAX_MA_SIGNALS_DDR3];
#endif
#if defined(DDR4_SUPPORT) && DDR4_SUPPORT==1
extern CONST UINT8            CadbMaGroupsDdr4[MAX_MA_GROUPS_DDR4][MAX_MA_SIGNALS_DDR4];
#endif

#ifdef VALID_C0_D0
extern CONST UINT16 SmbusAddressToClttTable[MAX_CHANNELS][MAX_DIMMS][2];
#endif

#if PPR_SUPPORT
extern UINT16 DynamicDmapBitLocations [MAX_RANK_BG_BA_ROW_BITS];
extern UINT16 MaxGuardkeyPatterns [MAX_GUARDKEY_PATTERNS];
#endif // PPR_SUPPORT
#endif

