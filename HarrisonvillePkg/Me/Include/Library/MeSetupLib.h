/**

This file contains a 'Sample Driver' and is licensed as such
under the terms of your license agreement with Intel or your
vendor.  This file may be modified by the user, subject to
the additional terms of the license agreement

**/

/**
Copyright (c)  2016 Intel Corporation. All rights reserved
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
#ifndef __ME_SETUP_LIB_H__
#define __ME_SETUP_LIB_H__

#include "SetupVariable.h"

#define ME_RC_CONFIGURATION SYSTEM_CONFIGURATION

EFI_STATUS
GetMeSetupConfig (ME_RC_CONFIGURATION * MeRcConfig);

EFI_STATUS
SetMeSetupConfig (ME_RC_CONFIGURATION * MeRcConfig);

#endif /* __ME_SETUP_LIB_H__ */
