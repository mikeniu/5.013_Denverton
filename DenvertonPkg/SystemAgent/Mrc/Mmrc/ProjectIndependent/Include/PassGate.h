/** @file
  PassGate.h
  Library specific macros and function declarations used within the Pass Gate Test

  Copyright (c) 2005-2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _PASS_GATE_H_
#define _PASS_GATE_H_

#include "MmrcHooks.h"
#include "MmrcLibraries.h"

#if JTAG
#include <stdio.h>
#endif

UINT8 
AntiLogBase2 (
  IN          UINT32    num
  );

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
;
#endif // _PASS_GATE_H_
