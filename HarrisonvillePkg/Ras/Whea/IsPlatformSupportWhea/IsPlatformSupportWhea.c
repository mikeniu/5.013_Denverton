/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
  IsPlatformSupportWhea.c

Abstract:
  IsPlatformSupportWhea Protocol driver, should be ported for platform.

---------------------------------------------------------------------------------*/

#include <Library\Debuglib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <Library\UefiRuntimeServicesTableLib.h>
#include <Library\HobLib.h>
#include <Protocol\IsPlatformSupportWhea.h>
#include <IncludePrivate/PchPolicyHob.h>
#include <RasPolicy.h>


BOOLEAN
IsSystemSupportWhea (
  IN EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL          *This
  );

STATIC
EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL  IsPlatformSupportWheaInst = \
{
    WheaNotSupported,
    0,
    &IsSystemSupportWhea
};


EFI_STATUS
InstallIsPlatformSupportWhea(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description: 

  This function will install IsPlatformSupportWhea Protocol if the platform supports Whea.
  The IsPlatformSupportWhea will be consumed by core whea module 

Arguments:  

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

Returns:  

  EFI_SUCCESS:              Driver initialized successfully
  EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

--*/
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_HANDLE                  Handle;
  struct RAS_GLOBAL_POLICY    *RasPolicy;
  EFI_PEI_HOB_POINTERS        HobPtr;
  
  IsPlatformSupportWheaInst.WheaSupport = WheaNotSupported;

  //
  // Get the SMM WHEA policy protocol.
  //
  //If Whea Support is not enabled, just return EFI_SUCCESS;
  //If yes, install EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL for WHEA core module to consume
  //
  HobPtr.Guid   = GetFirstGuidHob (&gRasPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  RasPolicy = GET_GUID_HOB_DATA (HobPtr.Guid);

  //
  // if Whea is disabled, return Success
  //

  if ( RasPolicy->WheaSupport == 0) { 
    return EFI_SUCCESS;
  } else if (RasPolicy->WheaSupport == 1) {
    //
    // WHEA EINJ 5.0 setup
    //
    if(RasPolicy->WheaEinj_05Extn)	IsPlatformSupportWheaInst.WheaAcpi5Support  = TRUE;
    else    IsPlatformSupportWheaInst.WheaAcpi5Support  = FALSE;

    IsPlatformSupportWheaInst.WheaSupport = WheaSupported;
    Handle = NULL;

    Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &gEfiIsPlatformSupportWheaProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &IsPlatformSupportWheaInst
                      );
    DEBUG((DEBUG_ERROR, "IsPlatformSupportWhea. InstallProtocolInterface.Status = %x \n",Status));
  }

  return Status;
}

BOOLEAN
IsSystemSupportWhea (
  IN EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL          *This
  )
{
  if(This->WheaSupport == WheaNotSupported) {
      return FALSE;
  }
  
  return TRUE;
}
