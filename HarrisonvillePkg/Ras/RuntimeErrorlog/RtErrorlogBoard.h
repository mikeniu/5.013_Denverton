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
//
// $Header: 
//
// $Revision: 4 $
//
// $Date: 12/15/11 7:26p $
//
//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogBoard.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _RT_ERRORLOG_BOARD_H_
#define _RT_ERRORLOG_BOARD_H_

#include  <RtErrorlogElink.h>

#include <Ras\Smm\SmmErrorlog\CommonErrorHandlerIncludes.h>

//
// PCI Error Status Bits
//
#define PCI_STS_MSTR_DATA_PERR      BIT8
#define PCI_STS_SIG_SERR            BIT14
#define PCI_STS_DET_PERR            BIT15

typedef struct {
  UINT8   				Socket;
  UINT8   				Branch;
  UINT8   				FbdChannel;
  UINT8   				DdrChannel;
  UINT8   				Dimm;
} MEM_ERROR_LOCATION;

extern BOOLEAN						mWheaLogStarted;	

#endif

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
