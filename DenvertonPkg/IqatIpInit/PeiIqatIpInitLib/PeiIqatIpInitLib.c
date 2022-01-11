/** @file
  This file is a library for IQAT IP PEI specific Initalization.

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

@par Specification Reference:

**/

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/IqatIpPolicyPpi.h>
#include <PeiIqatIpInitLib.h>
#include <IqatIpInitLib.h>
#include <pchpmcstpg.h>

#ifdef FSP_FLAG
#include <Protocol/PciEnumerationComplete.h>
#endif

#ifdef FSP_FLAG
EFI_STATUS
EFIAPI
FspIqatInitOnPciEnumCompleteEvent (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  );

//
// Notify Descriptor for gEfiEndOfDxeEventGroup
//   and gEfiPciEnumerationCompleteProtocolGuid.
//
STATIC
EFI_PEI_NOTIFY_DESCRIPTOR  mFspIqatIpInitNotifyList[] = {
  {
	(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPciEnumerationCompleteProtocolGuid,
    FspIqatInitOnPciEnumCompleteEvent
  }
};

EFI_STATUS
EFIAPI
FspIqatInitOnPciEnumCompleteEvent (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  )
/*++

Routine Description:

  IqatInit callback after PCI enumeration completed.

Arguments:

  PeiServices          General purpose services available to every PEIM.
  NotifyDescriptor     The notification structure this PEIM registered on install.
  Ppi                  The memory discovered PPI.  Not used.

Returns:

  EFI_SUCCESS       The function completed successfully

  --*/
{
  //
  // IQAT Initialization
  //
  IqatIpInit();

  return EFI_SUCCESS;
}

#endif  //FSP_FLAG


/**
  Initialize IQAT IP in PEI

  Arguments:
  NONE

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
  @retval Others 	- Error
**/
EFI_STATUS
PeiIqatIpInit (
  VOID
)
{
  IQAT_IP_GLOBAL_POLICY_PPI  *IqatIpGlobalPolicy;
  EFI_STATUS  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "PeiIqatIpInit Start\n"));

  //
  // Locate installed IQAT Policy PPI
  //
  Status = PeiServicesLocatePpi (
		 &gIqatIpPolicyPpiGuid,
		 0,
		 NULL,
		 (VOID **) &IqatIpGlobalPolicy
		 );
  ASSERT_EFI_ERROR(Status);

  if (IqatIpGlobalPolicy->IqatIpConfig.Enable == FALSE) {
    DEBUG ((DEBUG_INFO, "Disable IQAT device!!\n"));
    Status = PmcStPgDisableFunction(PMC_ST_PG_NCPM);
    ASSERT_EFI_ERROR (Status);
  } else {
    DEBUG ((DEBUG_INFO, "Enabled IQAT device!!\n"));
    Status = PmcStPgEnableFunction(PMC_ST_PG_NCPM);
    ASSERT_EFI_ERROR (Status);

#ifdef FSP_FLAG
    //
    // Register events to configure Silicon.
    //
    Status = PeiServicesNotifyPpi (&mFspIqatIpInitNotifyList[0]);
    ASSERT_EFI_ERROR (Status);
#endif
  }

  DEBUG ((DEBUG_INFO, "PeiIqatIpInit End\n"));

  return Status;
}
