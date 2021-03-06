//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//**********************************************************************//
// $Header:
//
// $Revision:
//
// $Date:
//**********************************************************************//
//**********************************************************************//

#ifndef _SmbiosDMIEditSsi_DRIVER_H
#define _SmbiosDMIEditSsi_DRIVER_H

#include <Token.h>

extern EFI_BOOT_SERVICES    *pBS;

#define DMIEDIT_SSI_GUID    {0x39cda386, 0x3988, 0x4073, { 0xa6, 0x59, 0x3a, 0x26, 0x84, 0x8e, 0xe5, 0x6f}}

#pragma pack(1)

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:  DATA_STRUC
//
// Description: AMI SMM Substitute Interface data structure
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct {
    UINT8   SsiFun;             //Function#: 50, 51, 52, etc.
    UINT64  BuffAddr;           //Buffer address of GET_SMBIOS_INFO / GET_SMBIOS_STRUCTURE / SET_SMBIOS_STRUCTURE
} SSI_DMI_CTRL;

#pragma pack()

#endif                          // _SmbiosDMIEditSsi_DRIVER_H

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
