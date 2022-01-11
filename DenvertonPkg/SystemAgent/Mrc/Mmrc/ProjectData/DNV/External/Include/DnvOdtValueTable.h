/** @file
  DnvOdtValueTable.h

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef AVN_ODT_VALUE_TABLE_H
#define AVN_ODT_VALUE_TABLE_H

#include "MemOdt.h"

#if ODT_TABLE

ODT_VALUE_STRUCT OdtValueTable[] = {
{
  // Config index
  (DDR3_800 << 0)       | // freq
  (SR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_120,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_120,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1066 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_60,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1333 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_60,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1600 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_60,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1866 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_800 << 0)       | // freq
  (DR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_120,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_120,         // rank0
      (RTT_WR_INF << 4)   | RTT_NOM_120          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1066 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_60,          // rank0
      (RTT_WR_INF << 4)   | RTT_NOM_60           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1333 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_60,          // rank0
      (RTT_WR_INF << 4)   | RTT_NOM_60           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1600 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_60,          // rank0
      (RTT_WR_INF << 4)   | RTT_NOM_60           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1866 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (EMPTY_DIMM << 10)    , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_INF << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_INF << 4)   | RTT_NOM_40           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS,         // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_800 << 0)       | // freq
  (SR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_120,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1066 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1333 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1600 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1866 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_800 << 0)       | // freq
  (DR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_120,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1066 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1333 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    }
  }
},
{
  // Config index
  (DDR3_1600 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    }
  }
},
{
  // Config index
  (DDR3_1866 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    }
  }
},
{
  // Config index
  (DDR3_800 << 0)       | // freq
  (DR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_120,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1066 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1333 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1600 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1866 << 0)      | // freq
  (DR_DIMM << 7)        | // slot0
  (SR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    }
  }
},
{
  // Config index
  (DDR3_800 << 0)       | // freq
  (SR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_120,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_40,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1066 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_120 << 4)   | RTT_NOM_30,          // rank0
      (RTT_WR_120 << 4)   | RTT_NOM_INF          // rank1
    }
  }
},
{
  // Config index
  (DDR3_1333 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    }
  }
},
{
  // Config index
  (DDR3_1600 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    }
  }
},
{
  // Config index
  (DDR3_1866 << 0)      | // freq
  (SR_DIMM << 7)        | // slot0
  (DR_DIMM << 10)       , // slot1
  MCODT_60,
  {
    {
      // dimm0
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_DIS << 4)   | RTT_NOM_DIS          // rank1
    },
    {
      // dimm1
      // Rtt_wr           Rtt_nom
      (RTT_WR_60 << 4)   | RTT_NOM_30,           // rank0
      (RTT_WR_60 << 4)   | RTT_NOM_INF           // rank1
    }
  }
},
};

#endif // ODT_TABLE

#endif // AVN_ODT_VALUE_TABLE_H
