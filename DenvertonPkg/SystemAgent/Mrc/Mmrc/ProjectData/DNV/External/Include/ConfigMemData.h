/** @file
  ConfigMemData.h
  This file contains extended defination of ConfigMemData.c

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _CONFIGMEMDATA_H_
#define _CONFIGMEMDATA_H_

#include "Mrc.h"
#include "McFunc.h"
#include "MrcFunc.h"

#pragma pack(1)

typedef enum {
  Trwsrl = 0,
  Trwsrs,
  Twrl,
  Twrs,
  Trrdr,
  Twwdr,
  Trwdr,
  Twrdr,
  Trrdd,
  Twwdd,
  Trwdd,
  Twrdd,
  MaxTimingGuardband
} TURNAROUND_TIMINGS;

extern UINT16 mDdrMemoryFrequencyTable[C_MAXDDR];
extern UINT16 mCoreFrequencyTable[C_MAXCOREFREQ];
extern UINT32 mDimmConfigs[MAX_DIMM_CONFIGS];
extern UINT32 mDimmActivate[MAX_ACTIVATE_CONFIGS];
extern UINT32 RaplDimmConfigs[MAX_RAPL_DIMM_CONFIGS];
extern UINT8  TimingGuardbands[MaxTimingGuardband][C_MAXDDR];
extern UINT16 mDdrMaxSreDly[C_MAXDDR];
#pragma pack()

#endif
