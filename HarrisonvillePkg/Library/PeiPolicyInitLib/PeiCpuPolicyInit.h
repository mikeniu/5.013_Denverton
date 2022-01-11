/** @file
  Header file for the PeiCpuPolicyInit.

@copyright
 Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
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

#ifndef _PEI_CPU_POLICY_INIT_H_
#define _PEI_CPU_POLICY_INIT_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/PchPcieDeviceTable.h>
#include <Library/PeiCpuPolicyLibrary.h>
#include <Library/PeiDxeSmmCpuPlatformLib.h>
#include "PeiCpuPolicyUpdateLib.h"
#include <FirwmareConfigurations.h>

///
/// Functions
///
/**
  This PEIM performs CPU PEI Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).
**/
EFI_STATUS
EFIAPI
PeiCpuPolicyInit (
  IN UINT8                     FirmwareConfiguration
  );
#endif
