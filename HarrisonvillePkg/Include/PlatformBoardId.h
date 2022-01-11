//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**
@copyright
Copyright (c) 2011-2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Platform.h

Abstract:
**/

#ifndef _PLATFORM_BOARD_ID_H_
#define _PLATFORM_BOARD_ID_H_

#define PLATFORM_INFO_HOB_REVISION          0x1
#define PILOT3_CONFIG_INDEX        0x2E
#define PILOT3_CONFIG_DATA         0x2F
#define PILOT3_ENTERCFG_VALUE      0x55
#define PILOT3_DEV_ID_REG          0xAA
#define PILOT3_DEV_ID              0xBB
#define PILOT3_EXITCFG_VALUE       0x02

#define FlavorUnknown                       0x0
#define FlavorMobile                        0x1
#define FlavorDesktop                       0x2
#define FlavorUpServer                      0x3
#define FlavorEmbedded                      0x4
#define FlavorMicroserver                   0x5
#define FlavorStorage                       0x6
#define FlavorPlatformMax                   0x7

#define TypeUnknown                         0x0
#define TypeTrad                            0x1
#define TypeUlt                             0x2

#define GenLpt                     0x1
#define GenWpt                     0x2
#define GenUnknown                 0x3

//
// Board Id 0x00 - 0xFF
//
#define BoardIdOstrichBay                   0x50
#define BoardIdCormorantLake                0x51
#define BoardIdHarcuvar                     0x52
#define BoardIdAspenCove                    0x53
#define BoardIdMinnehaha                    0x54
#define BoardIdPineLake                     0x55
#define BoardIdCarCreekModule               0x56
#define BoardIdBigBugCreek                  0x57
#define BoardIdPineLakeB                    0x58
#define BoardIdCarCreekModule2              0x59
#define BoardIdNano                         0x5A
#define BoardIdBigBugCreek2                 0x5B
#define BoardIdDelphi						0x5C



#define BoardIdUnknown1                     0xffff
#define BoardIdPlatformMax                  0xff
#define BOARD_ID_MASK_5BIT                  0x1f
#define BOARD_ID_MASK_8BIT                  0xff
#define FAB_ID_MASK                         0x7
#define GENERATION_MASK                     0x100

#endif

