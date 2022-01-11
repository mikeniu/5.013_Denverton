/** @file
  Header file for the PolicyInitPei PEIM.

@copyright
 Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
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
#ifndef _POLICY_INIT_PEI_H_
#define _POLICY_INIT_PEI_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

#include "PeiPchPolicyInit.h"
#include "PeiSaPolicyInit.h"
#include "PeiSiPolicyInit.h"
#include "PeiPcieIpPolicyInit.h"
#include "PeiIqatIpPolicyInit.h"
#include "PeiRasPolicyInit.h"
#include "PeiFiaMuxPolicyInit.h"

/**
  Initilize Intel PEI Platform Policy after Sv Pei Host interface installed. 
  For the policies which have no dependency with Sv Pei Host interface should be installed already. 
  This is for rest policies to be installed after Sv Pei Host interface is ready.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          Operation completed successfully.

**/
static
EFI_STATUS
PeiHostInterfaceCallback (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
  IN VOID                         *Ppi
  );

#endif
