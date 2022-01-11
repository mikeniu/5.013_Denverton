/** @file
  MchRegs.h
  This file include MCH register defination.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
 
**/

#ifndef _MCHREGS_H_
#define _MCHREGS_H_

//
// DEVICE 0 (Memroy Controller Hub)
//
#ifndef MC_BUS
#define MC_BUS                    0x00            //Bus 0
#endif
#define MC_FUN                    0x00            //Function 0
#define MC_DEV_FUN                (MC_DEV << 3)  //Device 0, function 0

//
// NB DEV 0 Vendor and Device IDs
//
#define MC_VID                    0x8086
#define MC_DID_MASK               0xFFF0

#define B_ESMRAMC_TSEG_SZ         0x06

#define REFRESH_DISABLED    0
#define REFRESH_128_CLOCKS  1
#define REFRESH_3_9_US      2
#define REFRESH_7_8_US      3

#endif

