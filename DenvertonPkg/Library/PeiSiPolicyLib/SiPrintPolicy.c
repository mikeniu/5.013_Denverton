/** @file
  This file is PeiSiPolicyLib library creates default settings of RC
  Policy and installs RC Policy PPI.

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
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
#include "PeiSiPolicyLibrary.h"

/**
  Print whole SI_POLICY_PPI and serial out.

  @param[in] SiPolicyPpi The RC Policy PPI instance
**/
VOID
EFIAPI
SiPrintPolicyPpi (
  IN  SI_POLICY_PPI          *SiPolicyPpi
  )
{
#ifdef EFI_DEBUG
  DEBUG ((DEBUG_INFO, "------------------------ Silicon Print Policy Start ------------------------\n"));
  DEBUG ((DEBUG_INFO, " Revision= %x\n", SiPolicyPpi->Revision));
  DEBUG ((DEBUG_INFO, " EcPresent= %x\n", SiPolicyPpi->EcPresent));
  DEBUG ((DEBUG_INFO, " TempPciBusMin= %x\n", SiPolicyPpi->TempPciBusMin));
  DEBUG ((DEBUG_INFO, " TempPciBusMax= %x\n", SiPolicyPpi->TempPciBusMax));
  DEBUG ((DEBUG_INFO, " TempMemBaseAddr= %x\n", SiPolicyPpi->TempMemBaseAddr));
  DEBUG ((DEBUG_INFO, " TempMemSize= %x\n", SiPolicyPpi->TempMemSize));
  DEBUG ((DEBUG_INFO, " TempIoBaseAddr= %x\n", SiPolicyPpi->TempIoBaseAddr));
  DEBUG ((DEBUG_INFO, " TempIoSize= %x\n", SiPolicyPpi->TempIoSize));

  DEBUG ((DEBUG_INFO, "------------------------ Silicon Print Policy End --------------------------\n"));
#endif // EFI_DEBUG
}

