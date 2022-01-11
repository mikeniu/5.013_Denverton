/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AdrCfgLib.h

Abstract:

  This header file provides common definitions for configuring an ADR capable platform's CPLD.

--*/

#ifndef ADRCFGLIB_H_
#define ADRCFGLIB_H_
#include <Library\AdrCfgLib.h> // bring in the libraries shared interface

#define CPLD_REG_BOARD_IDENTIFICATION (0)
#define CPLD_REG_CPLD_VERSION (2)
#define CPLD_REG_VREG_ENABLES (0xA6)
#define CPLD_REG_ADR_CONFIG_STATUS (0xA8)

#define ADRCFG_READ(addr) (addr|0x1)
#define ADRCFG_WRITE(addr)(~addr&0xfe)

#endif /* ADRCFGLIB_H_ */
