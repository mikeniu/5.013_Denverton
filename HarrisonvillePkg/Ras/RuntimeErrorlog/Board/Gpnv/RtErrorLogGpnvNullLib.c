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
// Name:		RtErrorLogGpnvNullLib.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogGpnv.h"


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LogMemErrorToGpnv
//
// Description: This function logs Memory error to Gpnv
//
// Input:
//  UINT8    Socket
//  UINT8    Channel
//  UINT8    Dimm
//  UINT8    Type
//
// Output: EFI_STATUS - EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogMemErrorToGpnv (
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
  )
{
  return EFI_UNSUPPORTED;
 }

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LogPciErrorToGpnv
//
// Description: This function logs pci error to Gpnv
//
// Input:
//  UINT8    ErrorSource
//  UINT8    ErrorType
//  UINT8    Segment
//  UINT8    Bus
//  UINT8    Device
//  UINT8    Function
//
// Output: EFI_STATUS - EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogPciErrorToGpnv (
   IN	  UINT8       ErrorSource,
   IN	  UINT8       ErrorType,
   IN	  UINT8       Segment,
   IN	  UINT8       Bus,
   IN	  UINT8       Device,
   IN	  UINT8       Function
 )
{
	  return EFI_UNSUPPORTED;
}
    
