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
// Name:		RtErrorLogGpnv.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _GPNV_LOG_H
#define _GPNV_LOG_H

#include <Protocol\SmbiosElogSupport.h>
#include <Protocol\GenericElogProtocol.h>


#include <Ras\RuntimeErrorlog\RtErrorLogBoard.h>

#include "Token.h"

#define PCIE_CFG_ADDR(bus,dev,func,reg) \
  ((VOID*) (UINTN)(PCIEX_BASE_ADDRESS + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

#define PCI_STS             0x0006        // PCI Status Register

EFI_STATUS
LogMemErrorToGpnv (
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
);

EFI_STATUS
LogPciErrorToGpnv (
	  IN UINT8    ErrorSource,
	  IN UINT8    ErrType,
	  IN UINT8    Segment,
	  IN UINT8    Bus,
	  IN UINT8    Dev,
	  IN UINT8    Func 
 );

#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2008, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
