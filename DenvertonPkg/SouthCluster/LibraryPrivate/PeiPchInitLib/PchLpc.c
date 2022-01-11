/** @file
  Initializes PCH LPC Device in PEI

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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

#include "PchInitPei.h"

/**
  Configure LPC device on early PEI.
**/
VOID
ConfigureLpcOnEarlyPei (
  VOID
  )
{
#if defined(FSP_FLAG) && defined(SERVER_BIOS_FLAG)
  PCH_POLICY_PPI     *PchPolicyPpi;
  EFI_STATUS         Status;
#endif

  DEBUG ((DEBUG_INFO, "ConfigureLpcOnEarlyPei() \n"));

  //
  // Enhance port 8xh LPC decoding.
  // The can be disable by policy EnhancePort8xhDecoding.
  //
#if defined(FSP_FLAG) && defined(SERVER_BIOS_FLAG)
  Status = PeiServicesLocatePpi (
             &gPchPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&PchPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Do not enable port80 decode (LGIRx) if disable port80 PCD/policy is set
  if (PchPolicyPpi->LpcConfig.Port80RouteDisable == 1)
      return;
#endif
//  PchLpcGenIoRangeSet (0x80, 0x10);  // Denverton AptioV override  -  Comment unnecessary programming of GENx_DEC Registers in RC code.
}

/**
  Configure LPC device on Policy callback.

  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
ConfigureLpcOnPolicy (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  PCH_LPC_GEN_IO_RANGE_LIST             LpcGenIoRangeList;
  UINTN                                 LpcBase;
  UINTN                                 Index;

  DEBUG ((DEBUG_INFO, "ConfigureLpcOnPolicy()\n"));
  //
  // If EnhancePort8xhDecoding is disabled, clear LPC and DMI LPC general IO range register.
  //
  if (PchPolicyPpi->LpcConfig.EnhancePort8xhDecoding == 0) {
    DEBUG ((DEBUG_INFO, "Disable EnhancePort8xhDecoding\n"));
    SetMem (&LpcGenIoRangeList, sizeof (PCH_LPC_GEN_IO_RANGE_LIST), 0);
    PchLpcGenIoRangeGet (&LpcGenIoRangeList);
    for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
      if ((LpcGenIoRangeList.Range[Index].BaseAddr == 0x80) &&
          (LpcGenIoRangeList.Range[Index].Length   == 0x10))
      {
        //
        // Clear the LPC general IO range register and DMI LPC general IO range register.
        //
        LpcBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_LPC,
                    PCI_FUNCTION_NUMBER_PCH_LPC
                    );
        MmioWrite32 (
          LpcBase + R_PCH_LPC_GEN1_DEC + Index * 4,
          0
          );
        break;
      }
    }
  }
}
