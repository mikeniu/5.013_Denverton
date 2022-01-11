/** @file
  SampleCode header file of the library for Intel PCH PEI Debug Policy initialization.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PEI_PCH_POLICY_DEBUG_LIB_H_
#define _PEI_PCH_POLICY_DEBUG_LIB_H_


/**
  This function performs PCH PEI Debug Policy initialization.

  @param[in] PchPolicy       The PCH Policy PPI instance

  @retval EFI_SUCCESS        The PPI is installed and initialized.
  @retval EFI ERRORS         The PPI is not successfully installed.
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicyDebug (
  IN OUT  PCH_POLICY_PPI  *PchPolicy
  );
#endif
