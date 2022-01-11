//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file RsdpPlusCsmOn.c 
	Support Legacy BIOS protocol to locate shadow ram when CSM on.

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <token.h>
#include <Protocol/ManageShadowProtocol.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosExt.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
EFI_STATUS CsmOnHeapToE000(UINT8 *pData, UINT32 Align, UINTN Length, DATA_BUFF_STRUC	*pData2 OPTIONAL);
EFI_STATUS CsmOnHeapToF000(UINT8 *pData, UINT32 Align, UINTN Length, DATA_BUFF_STRUC	*pData2 OPTIONAL);

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
EFI_LEGACY_BIOS_PROTOCOL	*gLegacyBiosProtocol = NULL;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)
extern EFI_BOOT_SERVICES    *pBS;
extern EFI_RUNTIME_SERVICES *pRS;
extern VOID *gLegacyRegionAddress;
extern EFI_GUID		ManageShdowRamProtocolGuid;
extern EFI_HANDLE	gManageShadowRamHandle;
extern VOID OemUpdateE000ShdowRamHook(UINT32*	pShadowRam, UINTN	UsageLength);
extern VOID OemUpdateF000ShdowRamHook(UINT32*	pShadowRam, UINTN	UsageLength);	
extern UPDATE_E000_SHDOW_RAM_HOOK* UpdateE000ShdowRamHookList[];
extern UPDATE_F000_SHDOW_RAM_HOOK* UpdateF000ShdowRamHookList[];
extern MANAGE_SHADOW_RAM_PROTOCOL gManageShadowRamProtocol;

// Function Definition(s)

//--------------------------------------------------------------------------- 

/**
    This is protocol function.
	According to input data and copy data to E-seg via legacy BIOS protocol.

	@param[in]	*pData		Pointer to context to save
	@param[in]	Align		Base alignment.
	@param[in]	Length		the length of the restored confidential information
	@param		*pData2		Pointer to stored data
	
	@retval EFI_SUCCESS				Copy Data successfully.
	
**/
EFI_STATUS
CsmOnHeapToE000(
	IN UINT8 *pData,
	IN UINT32 Align,
	IN UINTN Length,
	IN OUT DATA_BUFF_STRUC *pData2 OPTIONAL)
{
	EFI_STATUS 	Status;

	TRACE((-1,"Dbg (CsmOn): HeapToE000.\n"));

    Status = gLegacyBiosProtocol->GetLegacyRegion(gLegacyBiosProtocol,
                                        Length,
                                        E0000_BIT,
                                        (UINTN)Align,
                                        &gLegacyRegionAddress);
    if(EFI_ERROR(Status))
        return Status;

    Status = gLegacyBiosProtocol->CopyLegacyRegion(gLegacyBiosProtocol,
                                        Length,
                                        gLegacyRegionAddress,
                                        (VOID *)pData);
   if(EFI_ERROR(Status))
        return Status;

	if(pData2 != NULL){
		pData2->BuffAddress = (UINTN)gLegacyRegionAddress;
		pData2->UsedLength = Length;
		TRACE((-1,"Dbg : Locate Fseg addr [0x%lx], Data Length 0x%x\n", pData2->BuffAddress, pData2->UsedLength));
	}	

    return Status;		
}
		
/**
    This is protocol function.
	According to input data and copy data to F-seg via legacy BIOS protocol.

	@param[in]	*pData		Pointer to context to save
	@param[in]	Align		Base alignment.
	@param[in]	Length		the length of the restored confidential information
	@param		*pData2		Pointer to stored data
	
	@retval EFI_SUCCESS				Copy Data successfully.
	
**/
EFI_STATUS
CsmOnHeapToF000(
	IN UINT8 *pData,
	IN UINT32 Align,
	IN UINTN Length,
	IN OUT DATA_BUFF_STRUC *pData2 OPTIONAL)
{
	EFI_STATUS 	Status;

	TRACE((-1,"Dbg (CsmOn): HeapToF000.\n"));

    Status = gLegacyBiosProtocol->GetLegacyRegion(gLegacyBiosProtocol,
                                        Length,
                                        F0000_BIT,
                                        (UINTN)Align,
                                        &gLegacyRegionAddress);
    if(EFI_ERROR(Status))
        return Status;

    Status = gLegacyBiosProtocol->CopyLegacyRegion(gLegacyBiosProtocol,
                                        Length,
                                        gLegacyRegionAddress,
                                        (VOID *)pData);
   if(EFI_ERROR(Status))
        return Status;

	if(pData2 != NULL){
		pData2->BuffAddress = (UINTN)gLegacyRegionAddress;
		pData2->UsedLength = Length;
		TRACE((-1,"Dbg : Locate Fseg addr [0x%lx], Data Length 0x%x\n", pData2->BuffAddress, pData2->UsedLength));
	}

    return Status;
}

/**
    Locate Legacy Bios Protocol and Install Shadow Ram Protocol.
    
	@param[in]	*Event		signal event
	@param[in]	Context		pointer to event context
	
**/
VOID
LocateLegacyBiosProCallBack(
	IN EFI_EVENT Event,
	IN VOID *Context
	)
{
	EFI_STATUS	Status;
	
	pBS->CloseEvent(Event);
    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid,
                                 NULL,
                                 &gLegacyBiosProtocol);
	TRACE((-1,"Dbg :(CallBack) Locate LegacyBios Protocol %r.\n",Status));
	if(EFI_ERROR(Status))
		return;

	gManageShadowRamProtocol.HeapToE000 = CsmOnHeapToE000;
	gManageShadowRamProtocol.HeapToF000 = CsmOnHeapToF000;
	Status = pBS->InstallProtocolInterface(
					&gManageShadowRamHandle,
					&ManageShdowRamProtocolGuid,
					EFI_NATIVE_INTERFACE,
					&gManageShadowRamProtocol
					);
	TRACE((-1,"Dbg : (CallBack) Install Manage ShdowRam Protocol %r.\n",Status));		
	return;
}

/**
    Elink function for RsdpPlus CSM On initialization.
    Install Manage Shadow Ram protocol that support LegacyBios protocol.
	
	@retval EFI_SUCCESS		Install protocol successfully
	
**/
EFI_STATUS RsdpPlusCsmOnEntryPoint(VOID)
{
	EFI_STATUS 	Status;
	EFI_EVENT	Event;	
	VOID		*Registration;

	TRACE((-1,"Dbg : RsdpPlusCsmOnEntryPoint.\n"));

    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid,
                                 NULL,
                                 &gLegacyBiosProtocol);
	TRACE((-1,"Dbg : Locate LegacyBios Protocol %r.\n",Status));	
	if(EFI_ERROR(Status)){
	   	Status = RegisterProtocolCallback(
					&gEfiLegacyBiosProtocolGuid,
					LocateLegacyBiosProCallBack,
					NULL,   // Context
					&Event,
					&Registration
					);
		return Status;		
	}
	
	gManageShadowRamProtocol.HeapToE000 = CsmOnHeapToE000;
	gManageShadowRamProtocol.HeapToF000 = CsmOnHeapToF000;
	Status = pBS->InstallProtocolInterface(
					&gManageShadowRamHandle,
					&ManageShdowRamProtocolGuid,
					EFI_NATIVE_INTERFACE,
					&gManageShadowRamProtocol
					);
	TRACE((-1,"Dbg : Install Manage ShdowRam Protocol %r.\n",Status));
	return Status;
}

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
