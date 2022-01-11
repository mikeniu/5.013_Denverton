/** @file
  Mrc.h
  This file include external MRC data.

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MRC_H_
#define _MRC_H_

#include "DataTypes.h"
#include "Mmrc.h"

#if !defined SIM && !defined JTAG
#ifndef MINIBIOS
#include <Library\DebugLib.h>
#include <Library\HobLib.h>
#include <Library\PeiServicesLib.h>
#include <Ppi\ReadOnlyVariable2.h>
#include <Guid\MemoryConfigData.h>
#include <Library\BaseMemoryLib.h>
#include <Library\SerialPortLib.h>
#include <Library\PrintLib.h>
#include <Guid\AcpiVariable.h>
#include <Guid\GlobalVariable.h>
#endif
#endif 

#if defined SIM || defined JTAG
#include <string.h>
#else
#include "Printf.h"
#endif

#endif

