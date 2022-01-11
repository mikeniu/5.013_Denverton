//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file PeiIpmiIeReportPostCode.h
    Header files and function prototypes used in PeiIpmiIeReportPostCode driver
**/

#ifndef _EFI_PEI_IPMI_IE_POSTCODE_H
#define _EFI_PEI_IPMI_IE_POSTCODE_H

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Pi/PiHob.h>
#include <Ppi/MemoryDiscovered.h>
#include <IpmiIeNetFnOtherDefinitions.h>
#include <PostCodeCommon.h>

//
// Function Prototypes
//
EFI_STATUS
EFIAPI
UpdateIpmiTransportPpi (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi );

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
