//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name: RtErrorLogWheaNullLib.c
//
// Description:	
// 
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogWhea.h"

BOOLEAN						mWheaLogStarted = FALSE;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LogPciErrorToWhea
//
// Description: This function logs pci error to Whea
//
// Input:
//  UINT8    Bus
//  UINT8    Dev
//  UINT8    Func
//  BOOLEAN  IsBridge
//  UINT16   SecondaryStatus
//  UINT8    PcieOffset
//  UINT16   PcieDeviceStatus
//  UINT16   AerOffset
//  UINT32   UncorrErrStatus
//  UINT32   CorrErrStatus
//  UINT32   RootErrStatus
//
// Output: EFI_STATUS - EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogPciErrorToWhea(
	IN	  UINT8       ErrorSource,
	IN	  UINT8       ErrorType,
	IN	  UINT8       Segment,
	IN	  UINT8       Bus,
	IN	  UINT8       Device,
	IN	  UINT8       Function)
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
LogMemErrorToWhea(
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
 )
/*++

Routine Description:
    This function modifies the Uncorrectable and correctable error block entries for status
    Error severity 

Arguments:
  ErrorType - The type of error that needs to be injected

Returns:
	None 	

--*/
{
  return EFI_UNSUPPORTED;
}



//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
