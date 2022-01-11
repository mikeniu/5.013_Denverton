/**

This file contains a 'Sample Driver' and is licensed as such
under the terms of your license agreement with Intel or your
vendor.  This file may be modified by the user, subject to
the additional terms of the license agreement

**/

/**
Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file AmtSetup.h

  Header file for ME Setup.

**/
#ifndef __ME_SETUP_H__
#define __ME_SETUP_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define   SETUP_HECI_DEVICE_DISABLED        0
#define   SETUP_HECI_DEVICE_ENABLED         1
#define   SETUP_HECI_DEVICE_AUTO            2
#define   SETUP_HECI_DEVICE_HIDE            3

#define   SETUP_SSC_DISABLED                0
#define   SETUP_SSC_ENABLED                 1
#define   SETUP_SSC_AUTO                    2

#define   SETUP_HECI_IN_ME_OFF              0
#define   SETUP_HECI_IN_ME_HIDE             1
#define   SETUP_HECI_IN_ME_DISABLE          2

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __ME_SETUP_H__
