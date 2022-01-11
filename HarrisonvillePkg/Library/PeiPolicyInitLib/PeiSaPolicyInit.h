/** @file
  Header file for the SaPolicyInitPei PEIM.

@copyright
 Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
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
#ifndef _SA_POLICY_INIT_PEI_H_
#define _SA_POLICY_INIT_PEI_H_

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>
#include <Library/PeiSaPolicyLibrary.h>
#include <Ppi/SaPolicy.h>
#include <SaDataHob.h>
#include <SaAccess.h>
#include <CpuRegs.h>
#include <PeiSaPolicyUpdateLib.h>
#include <FirwmareConfigurations.h>

//
// Functions
//
/**
  <b>This PEIM performs SA PEI Policy initialzation</b> \n
  - <b>Introduction</b> \n
    System Agent PEIM behavior can be controlled by platform policy without modifying reference code directly.
    Platform policy PPI is initialized with default settings in this funciton.
    Some MRC functions will be initialized in this PEIM to provide the capability for customization.
    This policy PPI has to be initialized prior to System Agent initialization PEIM and MRC execution.

  - @pre
    - PEI_READ_ONLY_VARIABLE_PPI

  - @result
    _SA_POLICY_PPI will be installed successfully and ready for System Agent reference code use.

  - <b>Porting Recommendations</b> \n
    Policy should be initialized basing on platform design or user selection (like BIOS Setup Menu)

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the PPI.
  @retval     EFI ERRORS             The PPI is not successfully installed.
**/
EFI_STATUS
PeiSaPolicyInit (
  IN UINT8                     FirmwareConfiguration
  );
#endif
