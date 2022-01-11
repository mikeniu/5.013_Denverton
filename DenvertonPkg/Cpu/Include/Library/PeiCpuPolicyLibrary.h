/** @file
  Prototype of the PeiCpuPolicy library.

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PEI_CPU_POLICY_LIBRARY_H_
#define _PEI_CPU_POLICY_LIBRARY_H_

#include <Ppi/CpuPolicy.h>

/**
  CpuCreatePolicyDefaults creates the default setting of Cpu Policy.
  It allocates and zeros a buffer, and fills in the Intel default settings.

  @param[out] SiCpuPolicyPpi            The pointer to get PEI Cpu Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
CpuCreatePolicyDefaults (
  OUT  SI_CPU_POLICY_PPI                **SiCpuPolicyPpi
  );

/**
  CpuInstallPolicyPpi installs SiCpuPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiCpuPolicyPpi             The pointer to PEI Cpu Policy PPI instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
CpuInstallPolicyPpi (
  IN  SI_CPU_POLICY_PPI                 *SiCpuPolicyPpi
  );

#endif // _PEI_CPU_POLICY_LIBRARY_H_
