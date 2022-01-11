/**  @file
  MrcFunc.h
  This file the include all the external MRC Function.

  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MRCFUNC_H_
#define _MRCFUNC_H_

#include "Mrc.h"

UINT8
BitScanForward8 (
  IN        UINT8   Input
)
;

INT8
BitScanReverse32 (
  IN        UINT32   Input
)
;

MMRC_STATUS
FillFastBootDataStructure (
  IN  OUT   MMRC_DATA   *MrcData
)
;

MMRC_STATUS
FillOutputStructure (
  IN  OUT   MMRC_DATA   *MrcData
)
;
#endif

