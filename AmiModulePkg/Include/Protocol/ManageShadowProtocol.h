//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

//****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//****************************************************************************

//<AMI_FHDR_START>
//-----------------------------------------------------------------------------
//
//  Name:           ManageShadowProtocol.h
//
//  Description:    Defined Manage Shadow Ram Protocol GUID and Functions.
//
//-----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef __MANAGE_SHADOW_PROTOCOL_H__
#define __MANAGE_SHADOW_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif
#pragma	pack(1)
#define MANAGE_SHADOW_RAM_PROTOCOL_GUID \
	{0x1fa319e, 0xc36c, 0x4f19, 0x9d, 0x9d, 0xd0, 0x29, 0xd, 0xbe, 0xe9, 0x28}
//extern EFI_GUID gManageShdowRamProtocolGuid;

typedef struct{
	UINTN	BuffAddress;
	UINTN	UsedLength;
}DATA_BUFF_STRUC;

typedef
EFI_STATUS
(EFIAPI *HEAP_TO_E000) (
	UINT8	*pData,
	UINT32	Align,
	UINTN	Length,
	DATA_BUFF_STRUC	*pData2
);

typedef
EFI_STATUS
(EFIAPI *HEAP_TO_F000) (
	UINT8	*pData,
	UINT32	Align,
	UINTN	Length,
	DATA_BUFF_STRUC	*pData2
);

typedef struct {
	HEAP_TO_E000	HeapToE000;
	HEAP_TO_F000	HeapToF000;
} MANAGE_SHADOW_RAM_PROTOCOL;


//elink struct type define
typedef VOID (UPDATE_E000_SHDOW_RAM_HOOK)(
    IN UINT32*	pShadowRam,
	IN UINTN	Length
);

typedef VOID (UPDATE_F000_SHDOW_RAM_HOOK)(
    IN UINT32*	pShadowRam,
	IN UINTN	Length
);

typedef EFI_STATUS (CSM_ON_LINK_HOOK)(VOID);

typedef VOID (ENABLE_RUNTIME_SHADOW_WRITE_HOOK)(
    IN BOOLEAN Enable
);

#pragma	pack()
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
