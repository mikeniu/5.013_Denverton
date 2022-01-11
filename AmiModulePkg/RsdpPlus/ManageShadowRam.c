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

/** @file ManageShadowRam.c 
	Installing protocol functions for other module used.

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Protocol/ConsoleControl.h>
#include <token.h>
#include <AmiCspLib.h>
#include <Protocol/ManageShadowProtocol.h>
#include "ShadowRamProtocol.h"
#include <RsdpPlusElink.h>	//Define in RsdpPlus.mak file.

//#include <Protocol\Variable.h>
#include <protocol\AcpiSupport.h>
#include <ImageAuthentication.h>

#ifndef CSM_SUPPORT
#define CSM_SUPPORT 0
#endif

#if CSM_SUPPORT
#include <Setup.h>
#endif

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
EFI_STATUS HeapToE000(IN UINT8 *pData, UINT32 Align, IN UINTN Length, IN OUT DATA_BUFF_STRUC	*pData2 OPTIONAL);
EFI_STATUS HeapToF000(IN UINT8 *pData, UINT32 Align, IN UINTN Length, IN OUT DATA_BUFF_STRUC	*pData2 OPTIONAL);
VOID UpdateShadowBeforEfiBoot(VOID);
VOID EraseShadowAfterEfiBoot(VOID);
//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
EFI_HANDLE	gShadowRameHandle = NULL;
EFI_HANDLE	gManageShadowRamHandle = NULL;
UINT8 	*gE000HeapPtr = NULL;
UINT8 	*gF000HeapPtr = NULL;
UINT8	*gESegStore = NULL;
UINT8	*gFSegStore = NULL;
UINT8	EsegUserCount = 0, FsegUserCount = 0;
UINT32	gE000BuffLength = 0;
UINT32	gF000BuffLength = 0;
UINT32	gEsegBufferSize = 0x10000;
UINT32	gFsegBufferSize = 0xE000; // Preserve 0x2000 byte for Legacy VGA OpRom

#if CSM_SUPPORT
SETUP_DATA	gSetupData;
VOID *gLegacyRegionAddress = NULL;
#endif

// GUID Definition(s)
#define BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID \
    {0xdbc9fd21, 0xfad8, 0x45b0, 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93}

EFI_GUID gAllDriverConnectGuid = BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID;	
EFI_GUID gShdowRamProtocolGuid = SHADOW_RAM_PROTOCOL_GUID;
EFI_GUID ManageShdowRamProtocolGuid = MANAGE_SHADOW_RAM_PROTOCOL_GUID;
EFI_GUID gGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
// Protocol Definition(s)
SHADOW_RAM_PROTOCOL gShadowRamProtocol = 
{
	UpdateShadowBeforEfiBoot,
	EraseShadowAfterEfiBoot
};

MANAGE_SHADOW_RAM_PROTOCOL gManageShadowRamProtocol =
{
	HeapToE000,
	HeapToF000
};

// External Declaration(s)
extern UPDATE_E000_SHDOW_RAM_HOOK UPDATE_E000_SHADOW_RAM_HOOK_LIST EndOfUpdateE000ShadowRamHookList;
extern UPDATE_F000_SHDOW_RAM_HOOK UPDATE_F000_SHADOW_RAM_HOOK_LIST EndOfUpdateF000ShadowRamHookList;
extern CSM_ON_LINK_HOOK CSM_ON_LINK_HOOK_LIST EndOfCsmOnLinkHookList;
extern ENABLE_RUNTIME_SHADOW_WRITE_HOOK ENABLE_RUNTIME_SHADOW_WRITE_HOOK_LIST EndOfEnableRuntimeShadowWriteHookList;

// Function Definition(s)
UPDATE_E000_SHDOW_RAM_HOOK* UpdateE000ShdowRamHookList[] = {UPDATE_E000_SHADOW_RAM_HOOK_LIST NULL};
UPDATE_F000_SHDOW_RAM_HOOK* UpdateF000ShdowRamHookList[] = {UPDATE_F000_SHADOW_RAM_HOOK_LIST NULL};
CSM_ON_LINK_HOOK* CsmOnLinkHookList[] = {CSM_ON_LINK_HOOK_LIST NULL};
ENABLE_RUNTIME_SHADOW_WRITE_HOOK* EnableRuntimeShadowWriteHookList[] = {ENABLE_RUNTIME_SHADOW_WRITE_HOOK_LIST NULL};

//---------------------------------------------------------------------------    

/**
    Hook for update Eseg.
	@param  pShadowRam		A pointer to shadow ram area.
	@param  UsageLength		Used data size.

**/
VOID
OemUpdateE000ShdowRamHook(
    IN UINT32*	pShadowRam,
	IN UINTN	UsageLength
)
{
    UINTN i;
    
    for (i = 0; UpdateE000ShdowRamHookList[i] != NULL; i++) 
        UpdateE000ShdowRamHookList[i](pShadowRam,UsageLength);
}

/**
    Hook for update Fseg.
	@param  pShadowRam		A pointer to shadow ram area.
	@param  UsageLength		Used data size.

**/
VOID
OemUpdateF000ShdowRamHook(
    IN UINT32*	pShadowRam,
	IN UINTN	UsageLength
)
{
    UINTN i;
    
    for (i = 0; UpdateF000ShdowRamHookList[i] != NULL; i++) 
        UpdateF000ShdowRamHookList[i](pShadowRam,UsageLength);
}

/**
    Hook for external CSM On feature.
	@retval Status	Depending on the link function. 

**/
EFI_STATUS
ExecuteCsmOnLinkHook(VOID)
{
 	EFI_STATUS	Status;   
	UINTN i;
    
    for (i = 0; CsmOnLinkHookList[i] != NULL; i++) 
        Status = CsmOnLinkHookList[i]();

	return Status;	
}

/**
    Hook for Enable/Disable runtime shadow ram write.
	@param  Enable		Enable/Disable runtime shadow ram write.

**/
VOID
EnableRuntimeShadowWriteHook(
	IN BOOLEAN Enable
)
{
    UINTN i;
    
    for (i = 0; EnableRuntimeShadowWriteHookList[i] != NULL; i++) 
	    EnableRuntimeShadowWriteHookList[i](Enable);
}

/**
    This is "BeforeEfiBootLaunchHook" elink function.
	It will store original data of Shadow ram and then copy shadow buff's data to shadow ram. 

**/
VOID
UpdateShadowBeforEfiBoot(VOID)
{
	EFI_STATUS	Status;
	UINT32 E000Offset = 0xE0000;
	UINT32 F000Offset = 0xF0000;

	TRACE((-1,"Entry Update Shadow Ram!!!\n"));	
	TRACE((-1,"RsdpPlus (UpdateShadowBeforEfiBoot): gE000BuffLength [0x%x] \n",gE000BuffLength));
	TRACE((-1,"RsdpPlus (UpdateShadowBeforEfiBoot): gF000BuffLength [0x%x] \n",gF000BuffLength));

	if(gE000BuffLength == 0 && gF000BuffLength == 0) return;

	//unlock shadow ram
	//OemRuntimeShadowRamWrite(TRUE);
	EnableRuntimeShadowWriteHook(TRUE);


	if(gE000BuffLength != 0){
		Status = pBS->AllocatePool(
					EfiBootServicesData,
					gE000BuffLength,
					&gESegStore
					);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status))
			return;
		
		pBS->CopyMem(gESegStore,(UINT32*)E000Offset,(UINTN)gE000BuffLength);
		MemSet((VOID*)E000Offset,gE000BuffLength, 0);
		pBS->CopyMem((UINT32*)E000Offset,gE000HeapPtr,(UINTN)gE000BuffLength);
		TRACE((-1,"E000 Info : Data length %d bytes, There are %d data in Shadow Ram!!!\n",gE000BuffLength, EsegUserCount));
		OemUpdateE000ShdowRamHook((UINT32*)E000Offset, (UINTN)gE000BuffLength);
	}

	
	if(gF000BuffLength != 0){
		Status = pBS->AllocatePool(
					EfiBootServicesData,
					gF000BuffLength,
					&gFSegStore
					);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status))
			return;
		
		pBS->CopyMem(gFSegStore,(UINT32*)F000Offset,(UINTN)gF000BuffLength);
		MemSet((VOID*)F000Offset,gF000BuffLength, 0);
		pBS->CopyMem((UINT32*)F000Offset,gF000HeapPtr,(UINTN)gF000BuffLength);
		TRACE((-1,"F000 Info : Data length %d bytes, There are %d data in Shadow Ram!!!\n",gF000BuffLength, FsegUserCount));
		OemUpdateF000ShdowRamHook((UINT32*)F000Offset, (UINTN)gF000BuffLength);
	}

	//Clear Warm Reset Flash in 0FFFF0H (BYTE)
	MemSet((VOID*)0xFFFF0,0x1, 0);	

	//Lock shadow ram
	//OemRuntimeShadowRamWrite(FALSE);
	EnableRuntimeShadowWriteHook(FALSE);

	return;
}

/**
    This is "AfterEfiBootLaunchHook" elink function.
	It will restore original data to Shadow ram. 

**/
VOID
EraseShadowAfterEfiBoot(VOID)
{
	UINT32 E000Offset = 0xE0000;
	UINT32 F000Offset = 0xF0000;
	BOOLEAN ExitToSetup = TRUE;	
	EFI_STATUS 	Status;

	TRACE((-1,"Entry Update Shadow Ram!!!\n"));	

#if CSM_SUPPORT
	if(gSetupData.CsmSupport == 1){

		if(gLegacyRegionAddress == NULL) return;

		Status = pRS->SetVariable(
						L"Exitflag",
						&ManageShdowRamProtocolGuid,
						EFI_VARIABLE_BOOTSERVICE_ACCESS \
						|EFI_VARIABLE_RUNTIME_ACCESS,
						sizeof(ExitToSetup),
						&ExitToSetup);
		TRACE((-1,"Dbg (CsmOn): Set ExitFlag variable %r \n",Status));
		ASSERT_EFI_ERROR (Status);

		return;		
	}
#endif

	TRACE((-1,"RsdpPlus (UpdateShadowBeforEfiBoot): gE000BuffLength [0x%x] \n",gE000BuffLength));
	TRACE((-1,"RsdpPlus (UpdateShadowBeforEfiBoot): gF000BuffLength [0x%x] \n",gF000BuffLength));
		
	if(gE000BuffLength == 0 && gF000BuffLength == 0) return;
	
	TRACE((-1,"Store Shadow Ram to default!!!\n"));	
	//OemRuntimeShadowRamWrite(TRUE);
	EnableRuntimeShadowWriteHook(TRUE);

 	if(gE000BuffLength != 0){
		pBS->CopyMem((UINT32*)E000Offset,gESegStore,gE000BuffLength);
		pBS->FreePool(gESegStore);
	}

	if(gF000BuffLength != 0){
		pBS->CopyMem((UINT32*)F000Offset,gFSegStore,gF000BuffLength);
		Status = pBS->FreePool(gFSegStore);
	}
	
	//OemRuntimeShadowRamWrite(FALSE);
	EnableRuntimeShadowWriteHook(FALSE);

	Status = pRS->SetVariable(
					L"Exitflag",
					&ManageShdowRamProtocolGuid,
					EFI_VARIABLE_BOOTSERVICE_ACCESS \
					|EFI_VARIABLE_RUNTIME_ACCESS,
					sizeof(ExitToSetup),
					&ExitToSetup);
	TRACE((-1,"Dbg : Set ExitFlag variable %r \n",Status));
	ASSERT_EFI_ERROR (Status);
	//TRACE((-1,"Rsdp Dbg : Set ExitFlg variable %r \n",Status));

	return;
}

/**
    This is protocol function.
	According to input data and copy data to E-seg temporary buffer.

	@param[in]	*pData		Pointer to context to save
	@param[in]	Align		Base alignment.
	@param[in]	Length		the length of the restored confidential information
	@param		*pData2		Pointer to stored data
	
	@retval	EFI_INVALID_PARAMETER	error for input data is NULL and length is 0
	@retval EFI_BUFFER_TOO_SMALL	error for Input data length is greater than gEsegBufferSize.
	@retval EFI_SUCCESS				Copy Data successfully.
	
**/
EFI_STATUS
HeapToE000(
	IN UINT8 *pData,
	IN UINT32 Align,
	IN UINTN Length,
	IN OUT DATA_BUFF_STRUC *pData2 OPTIONAL)
{

	UINT8	*DataPtr = NULL;
	UINT8	*EndOfDataPtr = NULL;

	if(pData == NULL || Length == 0)
		return EFI_INVALID_PARAMETER;

	if(((UINTN)gE000BuffLength + Length) > gEsegBufferSize)
		return EFI_BUFFER_TOO_SMALL;

	if(Align != 0){
		DataPtr = (UINT8*)(( (UINTN)(gE000HeapPtr + gE000BuffLength)& ~((UINTN)Align - 1)) + (UINTN)Align);
	}else{
		DataPtr = gE000HeapPtr + gE000BuffLength;
	}

	if(pData2 != NULL){
		pData2->BuffAddress = (UINTN)gE000HeapPtr;
		//pData2->UsedLength = gE000BuffLength;
		pData2->UsedLength = (UINTN)(DataPtr - gE000HeapPtr);

	}

	EndOfDataPtr = (UINT8*)(DataPtr + Length);
	TRACE((-1,"RsdpPlus : Align [0x%x],DataPtr [0x%lx],EndOfDataPtr [0x%lx] \n",Align,DataPtr,EndOfDataPtr));
	
	if(EndOfDataPtr > (gE000HeapPtr + gEsegBufferSize))
		return EFI_BUFFER_TOO_SMALL;

	pBS->CopyMem(DataPtr, pData, Length);

	//gE000BuffLength = gE000BuffLength + (UINTN)(EndOfDataPtr - DataPtr);
	gE000BuffLength = (UINT32)(EndOfDataPtr - gE000HeapPtr);
	TRACE((-1,"RsdpPlus (HeapToE000): gE000BuffLength [0x%x] \n",gE000BuffLength));

	EsegUserCount++;

	return EFI_SUCCESS;		
}
		
/**
    This is protocol function.
	According to input data and copy data to F-seg temporary buffer.

	@param[in]	*pData		Pointer to context to save
	@param[in]	Align		Base alignment.
	@param[in]	Length		the length of the restored confidential information
	@param		*pData2		Pointer to stored data
	
	@retval	EFI_INVALID_PARAMETER	error for input data is NULL and length is 0
	@retval EFI_BUFFER_TOO_SMALL	error for Input data length is greater than gEsegBufferSize
	@retval EFI_SUCCESS				Copy Data successfully
	
**/
EFI_STATUS
HeapToF000(
	IN UINT8 *pData,
	IN UINT32 Align,
	IN UINTN Length,
	IN OUT DATA_BUFF_STRUC *pData2 OPTIONAL)
{
	UINT8	*DataPtr = NULL;
	UINT8	*EndOfDataPtr = NULL;


	if(pData == NULL || Length == 0)
		return EFI_INVALID_PARAMETER;

	if(((UINTN)gF000BuffLength + Length) > gFsegBufferSize)
		return EFI_BUFFER_TOO_SMALL;

	if(Align != 0){
		DataPtr = (UINT8*)(( (UINTN)(gF000HeapPtr + gF000BuffLength)& ~((UINTN)Align - 1)) + (UINTN)Align);		
	}else{
		DataPtr = gF000HeapPtr + gF000BuffLength;
	}

	if(pData2 != NULL){
		pData2->BuffAddress = (UINTN)gF000HeapPtr;
		//pData2->UsedLength = gF000BuffLength;
		pData2->UsedLength = (UINTN)(DataPtr - gF000HeapPtr);
	}

	EndOfDataPtr = (UINT8*)(DataPtr + Length);
	TRACE((-1,"RsdpPlus : Align [0x%x],DataPtr [0x%lx],EndOfDataPtr [0x%lx] \n",Align,DataPtr,EndOfDataPtr));
	
	if(EndOfDataPtr > (gF000HeapPtr + gFsegBufferSize))
		return EFI_BUFFER_TOO_SMALL;

	pBS->CopyMem(DataPtr, pData, Length);

	//gF000BuffLength = gF000BuffLength + (UINTN)(EndOfDataPtr - DataPtr);
	gF000BuffLength = (UINT32)(EndOfDataPtr - gF000HeapPtr);
	TRACE((-1,"RsdpPlus (HeapToF000): gF000BuffLength [0x%x] \n",gF000BuffLength));

	FsegUserCount++;

	return EFI_SUCCESS;		
}

/**
    Install Shadow Ram Protocol.

	@param[in]	*Event		signal event
	@param[in]	Context		pointer to event context
	
	@retval EFI_SUCCESS		Install ShdowRam Protocol successfully
	
**/
EFI_STATUS
ShadowRamCallBack(
	IN EFI_EVENT Event,
	IN VOID *Context
	)
{
	EFI_STATUS	Status;
	
	pBS->CloseEvent(Event);
	Status = pBS->InstallProtocolInterface(
								&gShadowRameHandle,
								&gShdowRamProtocolGuid,
								EFI_NATIVE_INTERFACE,
								&gShadowRamProtocol
								);
	return Status;
}

/**
    Callback for RsdpPlus initialization.
    Register a All Driver Connect protocol callback function.
    And allocate two buff for Manage Shadow Ram protocol used.
    Install Manage Shadow Ram protocol.			

	@param[in]	*Event		signal event
	@param[in]	Context		pointer to event context

**/
VOID
ManageShadowRamProtocolInitCallback(
	IN EFI_EVENT Event,
	IN VOID *Context
	)
{
	EFI_EVENT	SecondEvent;
	EFI_STATUS	Status;
	VOID		*Registration;
	static	EFI_PHYSICAL_ADDRESS	E000PagePtr;
	static 	EFI_PHYSICAL_ADDRESS	F000PagePtr;
#if CSM_SUPPORT
	EFI_GUID	SetupGuid	= SETUP_GUID;
    UINTN		VariableSize;
    UINT8		CurrentCsmState;
    
   	Status = RegisterProtocolCallback(
				&gAllDriverConnectGuid,
				ShadowRamCallBack,
				NULL,   // Context
				&SecondEvent,
				&Registration
				);
	if(EFI_ERROR(Status)) return;

	VariableSize = sizeof(UINT8);
	Status = pRS->GetVariable(EFI_SECURE_BOOT_NAME, &gGlobalVariableGuid, NULL, &VariableSize, &CurrentCsmState);
	TRACE((-1,"Dbg : Get Secure boot Global variable %r, CurrentCsmState [%d] \n",Status, CurrentCsmState));
	
	VariableSize = sizeof (SETUP_DATA);;
    Status = pRS->GetVariable (L"Setup", &SetupGuid, NULL, &VariableSize, &gSetupData);  
	TRACE((-1,"Dbg : Get Setup Variable %r, CsmLaunchPolicy = %d.\n",Status, gSetupData.CsmSupport));
	
	if(!EFI_ERROR(Status) && (gSetupData.CsmSupport == 1 && !CurrentCsmState)){
		Status = ExecuteCsmOnLinkHook();
		TRACE((-1,"Dbg : ExecuteCsmOnLinkHook %r.\n",Status));
		return;
	}
#endif	
	TRACE((-1,"Dbg : RsdpPlus Non-CSM.\n"));

   	Status = RegisterProtocolCallback(
				&gAllDriverConnectGuid,
				ShadowRamCallBack,
				NULL,   // Context
				&SecondEvent,
				&Registration
				);
	if(EFI_ERROR(Status)) return;

	Status = pBS->AllocatePages(
                 AllocateAnyPages, 
                 EfiBootServicesData, 
                 16,
                 &E000PagePtr);
	ASSERT_EFI_ERROR(Status);
	gE000HeapPtr = (UINT8*)E000PagePtr;
	TRACE((-1,"RsdpPlus : gE000HeapPtr [0x%lx] \n",gE000HeapPtr));

	Status = pBS->AllocatePages(
				AllocateAnyPages, 
                EfiBootServicesData, 
                16,
                &F000PagePtr);	
	ASSERT_EFI_ERROR(Status);
	gF000HeapPtr = (UINT8*)F000PagePtr;
	TRACE((-1,"RsdpPlus : gF000HeapPtr [0x%lx] \n",gF000HeapPtr));

	Status = pBS->InstallProtocolInterface(
					&gManageShadowRamHandle,
					&ManageShdowRamProtocolGuid,
					EFI_NATIVE_INTERFACE,
					&gManageShadowRamProtocol
					);
	TRACE((-1,"Dbg : Install Manage ShdowRam Protocol %r.\n",Status));
	return;
}

/**
    Entry point for RsdpPlus initialization.
    
	@param  ImageHandle		The firmware allocated handle for the EFI image.
	@param  SystemTable		A pointer to the EFI System Table.

	@retval EFI_SUCCESS   	The constructor always returns EFI_SUCCESS.
	
**/
EFI_STATUS ManageShadowRamEntryPoint(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
	//static EFI_GUID VariableArchProtocolGuid = EFI_VARIABLE_ARCH_PROTOCOL_GUID;
	static EFI_GUID	AcpiSupportProtocolGuid = EFI_ACPI_SUPPORT_GUID;
	EFI_STATUS	Status = EFI_SUCCESS;
    EFI_EVENT Event;
    VOID	*Registration;
    VOID    *Protocol;
    
	InitAmiLib(ImageHandle,SystemTable);

    //Status = pBS->LocateProtocol(&VariableArchProtocolGuid,NULL,&Protocol);
    Status = pBS->LocateProtocol(&AcpiSupportProtocolGuid,NULL,&Protocol);
    TRACE((-1,"Dbg : Locate ACPI Support Protocol %r \n",Status));
	if(!EFI_ERROR(Status)){
		ManageShadowRamProtocolInitCallback(NULL,NULL);
	}else{
/*		RegisterProtocolCallback(&VariableArchProtocolGuid, 
								ManageShadowRamProtocolInitCallback, 
								NULL, &Event, &Registration);
*/
		RegisterProtocolCallback(&AcpiSupportProtocolGuid, 
								ManageShadowRamProtocolInitCallback, 
								NULL, &Event, &Registration);
	}
	
	return EFI_SUCCESS;
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
