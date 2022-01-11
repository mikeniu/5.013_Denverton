/** @file
    SklPchH Ax HSIO Header File

@copyright
 Copyright (c) 2014 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _PCH_H_HSIO_A0_H_
#define _PCH_H_HSIO_A0_H_

#define PCH_H_HSIO_VER_Ax         0x11

#define PCH_HSIO_TABLE_A0_SIZE                          50
#define PCH_CHIPSET_INIT_A0_SIZE						1404
extern UINT8                                            PchHChipsetInitTable_A0[PCH_CHIPSET_INIT_A0_SIZE];
extern PCH_SBI_HSIO_TABLE_STRUCT                        PchHHsio_A0[PCH_HSIO_TABLE_A0_SIZE];

#define PCH_CHIPSET_INIT_B0_SIZE						2484
extern UINT8                                            PchHChipsetInitTable_B0[PCH_CHIPSET_INIT_B0_SIZE];
extern PCH_SBI_HSIO_TABLE_STRUCT                        PchHHsio_B0[1];



#endif //_PCH_H_HSIO_A0_H_
