/** @file
  Dump whole ME_PLATFORM_POLICY_PPI and serial out.

@copyright
 Copyright (c) 2012 - 2015 Intel Corporation. All rights reserved
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

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include "Ppi/MePlatformPolicyPei.h"

/**
  Dump PEI ME Platform Policy

  @param[in] PeiMePlatformPolicy The Me Platform Policy instance
**/
VOID
PeiMePolicyDebugDump (
  IN  PEI_ME_PLATFORM_POLICY_PPI *MePlatformPolicyPpi
  )
{

  DEBUG ((DEBUG_INFO, "\n---------------------- MePlatformPolicyPpi Dump Begin -----------------\n"));
  DEBUG ((DEBUG_INFO, " Revision : 0x%x\n", MePlatformPolicyPpi->Revision));
  DEBUG ((DEBUG_INFO, "MeConfig ---\n"));

  //
  // Byte 1 Ptt Solution Method Selection
  // DEBUG ((DEBUG_INFO, " PttSwitch : 0x%x\n", MePlatformPolicyPpi->PttSwitch));

  DEBUG ((DEBUG_INFO, " DidEnabled  : 0x%x\n", MePlatformPolicyPpi->DidEnabled));
  DEBUG ((DEBUG_INFO, " DidInitStat : 0x%x\n", MePlatformPolicyPpi->DidInitStat));
  DEBUG ((DEBUG_INFO, " HeciCommunication1 : 0x%x\n", MePlatformPolicyPpi->HeciCommunication1));
  DEBUG ((DEBUG_INFO, " HeciCommunication2 : 0x%x\n", MePlatformPolicyPpi->HeciCommunication2));
  DEBUG ((DEBUG_INFO, " HeciCommunication3 : 0x%x\n", MePlatformPolicyPpi->HeciCommunication3));
  DEBUG ((DEBUG_INFO, " SolEnabled : 0x%x\n", MePlatformPolicyPpi->SolEnabled));
  DEBUG ((DEBUG_INFO, " IderEnabled : 0x%x\n", MePlatformPolicyPpi->IderEnabled));
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
  DEBUG ((DEBUG_INFO, " WaitResetWarningAck : 0x%x\n", MePlatformPolicyPpi->WaitResetWarningAck));
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
#if defined(SPS_SUPPORT) && SPS_SUPPORT
  DEBUG ((DEBUG_INFO, " EnableMePreDidReset : 0x%x\n", MePlatformPolicyPpi->EnableMePreDidReset));
#endif // SPS_SUPPORT
  DEBUG ((DEBUG_INFO, "\n---------------------- MePlatformPolicyPpi Dump End -------------------\n"));
}
