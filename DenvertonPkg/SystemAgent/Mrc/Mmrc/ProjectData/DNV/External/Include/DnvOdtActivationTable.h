/** @file
  DnvOdtActivationTable.h

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef AVN_ODT_ACTIVATION_TABLE_H
#define AVN_ODT_ACTIVATION_TABLE_H

#include "MemOdt.h"

#if ODT_TABLE

ODT_ACT_STRUCT OdtActTable[] = {
  //
  // Config index
  //
  (DIMM0 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (SR_DIMM << 7)           | // slot0
  (EMPTY_DIMM << 10)       | // slot1
  (EMPTY_DIMM << 13),        // slot2
  //
  // ODT activation matrix
  //
  {
    //
    // Read
    //
    MCODT_ACT << 15  |
    0,
    //
    // Write
    //
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (EMPTY_DIMM << 10)       | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK1 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (EMPTY_DIMM << 10)       | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (SR_DIMM << 7)           | // slot0
  (SR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM1 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (SR_DIMM << 7)           | // slot0
  (SR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK1 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM1 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM1 << 0)             | // target dimm
  (RANK1 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (SR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK1 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (SR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM1 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (DR_DIMM << 7)           | // slot0
  (SR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM0 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (SR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM1 << 0)             | // target dimm
  (RANK0 << 2)             | // target rank
  (SR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
    0,
  },
  // Config index
  (DIMM1 << 0)             | // target dimm
  (RANK1 << 2)             | // target rank
  (SR_DIMM << 7)           | // slot0
  (DR_DIMM << 10)          | // slot1
  (EMPTY_DIMM << 13),        // slot2

  // ODT activation matrix
  {
    // read
    MCODT_ACT << 15  |
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    0,
    // write
    ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
    ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
    0,
  },
};

#endif // ODT_TABLE

#endif // AVN_ODT_ACTIVATION_TABLE_H
