/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

@copyright
  Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement


Module Name:

  IPMIPlatformID.c

Abstract:
  Comrmorant Lake CRB rquest
  Send Platform ID to BMC via IPMI 
  NetFun:0x30 Cmd=0x20 Data=0x76 0x08 0 0 0 0 0 0 0 0
--*/
#include "IPMIPlatformID.h"

EFI_STATUS
IPMIPlatformID_entry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status=EFI_SUCCESS;
  PLATFORM_INFO_HOB               *PlatformInfoHob; 
  PEI_IPMI_TRANSPORT_PPI          *mIpmiTransport;  
  UINT8       commanddata[10]={0x76,0x08,0,0,0,0,0,0,0,0};
  UINT8       commanddatasize=10;
  UINT8                ResponseData[10] = {0};
  UINT8                ResponseLength   = sizeof(ResponseData);
  
  PlatformInfoHob = NULL;

  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }
  
   Status = PeiServicesLocatePpi(
				&gEfiPeiIpmiTransportPpiGuid,	// Denverton AptioV Override
				0,
				NULL,
				(VOID **) &mIpmiTransport
				);
   
  DEBUG((EFI_D_ERROR,"IPMI_PlatformID: Start \n"));
  if(PlatformInfoHob->BoardId == BOARD_ID_CormorantLake){
  DEBUG((EFI_D_ERROR,"IPMI_PlatformID:Send Board ID to BMC: It's Cormorant Lake CRB\n"));
  //Net Fun=0x30
   Status = mIpmiTransport->SendIpmiCommand (
                            mIpmiTransport,
                            BMC_GENERAL_APPLICATION,
                            0,
                            SET_Platform_ID,
                            (UINT8 *)commanddata,
                            commanddatasize,
                            ResponseData,
                            &ResponseLength
                            );
  
   if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR,"IPMI_PlatformID:Send Board ID to BMC(): failed.\n"));
    return Status;
       }
     DEBUG ((EFI_D_ERROR,"IPMI_PlatformID:Send Board ID to BMC(): Success.\n"));  
     return Status;
  }
	DEBUG((EFI_D_ERROR,"IPMIPlatformID:It's not Cormorant Lake CRB \n"));

  return EFI_UNSUPPORTED;
}  

