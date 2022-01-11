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
// Name:		RtErrorlogWhea.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef _WHEA_LOG_H
#define _WHEA_LOG_H

#include <Ras\RuntimeErrorlog\RtErrorLogBoard.h>


EFI_STATUS
LogPciErrorToWhea(
		IN	  UINT8       ErrorSource,
		IN	  UINT8       ErrorType,
		IN	  UINT8       Segment,
		IN	  UINT8       Bus,
		IN	  UINT8       Device,
		IN	  UINT8       Function
    );

EFI_STATUS
LogMemErrorToWhea(
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
);

#endif
