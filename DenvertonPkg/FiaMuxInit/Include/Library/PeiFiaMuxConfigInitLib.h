/** @file
  Header file for FIA MUX configuration Init PEI library.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

@par Specification
**/

#ifndef _PEI_FIA_MUX_CONFIG_INIT_LIB_H_
#define _PEI_FIA_MUX_CONFIG_INIT_LIB_H_

#include <Ppi/FiaMuxPolicyPpi.h>
#include <Guid/FiaMuxHobGuid.h>

/**
Initialize FIA MUX configuration.

Arguments:

@param[out] *GlobalResetRequired - On success, the pointer to have reset flag.

Returns:
@retval EFI_SUCCESS 				- The function completed successfully
@retval RETURN_INVALID_PARAMETER 	- invalid parameter
@retval EFI_NOT_READY Returned if this API is called before ME signals init
complete, or after END_OF_POST was sent.
@retval EFI_DEVICE_ERROR Returned if HECI transport is not available.
@retval EFI_PROTOCOL_ERROR Unexpected protocol error returned by ME.
**/
EFI_STATUS
PeiFiaMuxConfigInit(
  OUT BOOLEAN *GlobalResetRequired
);

#endif  //_PEI_FIA_MUX_CONFIG_INIT_LIB_H_
