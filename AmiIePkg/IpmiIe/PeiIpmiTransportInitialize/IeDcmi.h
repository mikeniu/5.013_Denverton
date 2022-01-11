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

/** @file IeDcmi.h
    Baseboard Event Log functions.

**/

#ifndef __IE_DCMI__H__
#define __IE_DCMI__H__

#include <Library/IeHeciCoreLib.h>
#include <Include/IeHeciRegs.h>

//
// DCMI-HI Protocol GUID
//
#define EFI_DCMI_HI_GUID \
  { \
    0x7519B383, 0x48fc, 0x43e5, 0xa5, 0xeb, 0x59, 0x59, 0xcb, 0x58, 0x10, 0x00 \
  }

typedef struct _HECI_MESSAGE_HEADER
{
UINT32 MEAddress:8;
UINT32 HostAddress:8;
UINT32 Length:9;
UINT32 Reserved:6;
UINT32 MessageComplete:1;
} HECI_MESSAGE_HEADER; 

typedef struct _IE_HECI_PPI_EXT
{
  PEI_IE_HECI_PPI    Ppi;
  IE_HECI_DEVICE     Heci;
  HECI_MSG_HEADER    MsgBuf[HECI_MSG_MAXLEN / sizeof(HECI_MSG_HEADER)];
} IE_HECI_PPI_EXT;
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
