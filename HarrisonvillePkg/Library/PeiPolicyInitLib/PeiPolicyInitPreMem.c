/** @file
  This file is SampleCode for Intel PEI Platform Policy initialization.

@copyright
 Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved
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

#include "PeiPolicyInit.h"

/**
  Initialize Intel PEI Platform Policy

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.
**/
VOID
EFIAPI
PeiPolicyInitPreMem (
  IN UINT8                     FirmwareConfiguration
  )
{
  EFI_STATUS                   Status;

  //
  // FIA MUX PEI Policy Initialization
  //
  Status = PeiFiaMuxPolicyInit(FirmwareConfiguration);
  DEBUG((DEBUG_INFO, "FIA MUX PEI Policy Initialization Done \n"));
  ASSERT_EFI_ERROR(Status);
  //
  // Silicon PEI Policy Initialization
  //
  Status = PeiSiPolicyInit (FirmwareConfiguration);
  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Done \n"));
  ASSERT_EFI_ERROR (Status);

  //
  // PCH PEI Policy Initialization
  //
  Status = PeiPchPolicyInitPreMem (FirmwareConfiguration);
  DEBUG ((DEBUG_INFO, "PCH PEI Policy Initialization Done in Pre-Memory\n"));
  ASSERT_EFI_ERROR (Status);



  //
  // SystemAgent Pei Platform Policy Initialization
  //
  Status = PeiSaPolicyInit (FirmwareConfiguration);
  DEBUG ((DEBUG_INFO, "SystemAgent PEI Platform Policy Initialization Done \n"));
  ASSERT_EFI_ERROR (Status);
  //
  // PCIE IP PEI Policy Initialization
  //
  Status = PeiPcieIpPolicyInit (FirmwareConfiguration);
  DEBUG ((DEBUG_INFO, "PCIE IP PEI Policy Initialization Done \n"));
  ASSERT_EFI_ERROR (Status);

  //
  // IQAT IP PEI Policy Initialization
  //
  Status = PeiIqatIpPolicyInit (FirmwareConfiguration);
  DEBUG ((DEBUG_INFO, "IQAT IP PEI Policy Initialization Done \n"));
  ASSERT_EFI_ERROR (Status);

  //
  //RAS Pei Policy Initialization
  //
  Status = PeiRasPolicyInit (FirmwareConfiguration);
  DEBUG ((DEBUG_INFO, "RAS PEI Policy Initialization Done \n"));
  ASSERT_EFI_ERROR (Status);
}
