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

/** @file RsdpPlus.c 
	Entry point for RsdpPlus initialization.

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Acpi30.h>
#include <AmiCspLib.h>
#include <Protocol/ManageShadowProtocol.h>
#include <RsdpPlusElink.h>	//Define in RsdpPlus.mak file.

#ifndef CSM_SUPPORT
#define CSM_SUPPORT 0
#endif

#if CSM_SUPPORT
#include <Setup.h>
#include <ImageAuthentication.h>
#endif

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
RSDT_PTR_20 *RSDP = NULL;
DATA_BUFF_STRUC	DataInfo = {0,0};

// GUID Definition(s)
EFI_GUID gManageShadowRamProtocolGuid = MANAGE_SHADOW_RAM_PROTOCOL_GUID;
EFI_GUID gAcpi20TableGuid		= ACPI_20_TABLE_GUID;
EFI_GUID gAcpi11TAbleGuid 	    = ACPI_10_TABLE_GUID;


// Protocol Definition(s)

// External Declaration(s)
extern ENABLE_RUNTIME_SHADOW_WRITE_HOOK ENABLE_RUNTIME_SHADOW_WRITE_HOOK_LIST EndOfEnableRuntimeShadowWriteHookList;

// Function Definition(s)
ENABLE_RUNTIME_SHADOW_WRITE_HOOK* EnableRuntimeShadowWriteHookList[] = {ENABLE_RUNTIME_SHADOW_WRITE_HOOK_LIST NULL};

//---------------------------------------------------------------------------

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

#if CSM_SUPPORT
/**
    Elink function, Delete RSDP structure data before legacy boot.

**/

VOID EraseRsdpInFseg(VOID)
{
	EFI_STATUS	Status;
	EFI_GUID	SetupGuid	= SETUP_GUID;
    UINTN		VariableSize;
    UINT8		CurrentCsmState;
    SETUP_DATA	SetupData;
    UINTN		*Buff = (UINTN*)DataInfo.BuffAddress;
	
    TRACE((-1,"Dbg(EraseRsdpInFseg) : EraseRsdpInFseg \n"));
	TRACE((-1,"Dbg(EraseRsdpInFseg) : Buff address is 0x%x \n",Buff));
	if(Buff == NULL)
		return;

	VariableSize = sizeof(UINT8);
	Status = pRS->GetVariable(EFI_SECURE_BOOT_NAME, &gEfiGlobalVariableGuid, NULL, &VariableSize, &CurrentCsmState);
	TRACE((-1,"Dbg(EraseRsdpInFseg) : Get Secure boot Global variable %r, CurrentCsmState [%d] \n",Status, CurrentCsmState));
	
	VariableSize = sizeof (SETUP_DATA);;
    Status = pRS->GetVariable (L"Setup", &SetupGuid, NULL, &VariableSize, &SetupData);  
	TRACE((-1,"Dbg(EraseRsdpInFseg) : Get Setup Variable %r, CsmLaunchPolicy = %d.\n",Status, SetupData.CsmSupport));
	
	if(!EFI_ERROR(Status) && (SetupData.CsmSupport == 1 && !CurrentCsmState)){
		EnableRuntimeShadowWriteHook(TRUE);
		pBS->SetMem((VOID*)Buff, DataInfo.UsedLength, 0x0);
		EnableRuntimeShadowWriteHook(FALSE);
		return;
	}	
	
	return;
}
#endif
/**
    Elink function for creating RSDP in Shadow Ram when UEFI boot.
    
	@param  ImageHandle		The firmware allocated handle for the EFI image.
	@param  SystemTable		A pointer to the EFI System Table.

	@retval EFI_SUCCESS   	The constructor always returns EFI_SUCCESS.
	
**/
EFI_STATUS CreateRsdpInFseg(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS	Status = EFI_NOT_FOUND;
	BOOLEAN		ExitToSetupFlag;
	MANAGE_SHADOW_RAM_PROTOCOL	*ManageShadowRamProtocol;
	UINTN DataSize;     
	
	InitAmiLib(ImageHandle,SystemTable);

	TRACE((-1,"Dbg(CreateRsdpInFseg) : CreateRsdpInFseg \n"));
	RSDP = GetEfiConfigurationTable(pST,&gAcpi20TableGuid);
    if (!RSDP) 
    {
        RSDP = GetEfiConfigurationTable(pST,&gAcpi11TAbleGuid);
    }
    if (!RSDP) return EFI_NOT_FOUND; 
	
    TRACE((-1,"Rsdp Ptr [0x%x] \n",RSDP));
	TRACE((-1,"Rsdp Sig [%lx] \n",RSDP->Signature));
	TRACE((-1,"Rsdp XSDT addr [0x%x] \n",RSDP->XsdtAddr));
	TRACE((-1,"Rsdp RSDT addr [0x%x] \n",RSDP->RsdtAddr));

	Status = pBS->LocateProtocol(&gManageShadowRamProtocolGuid,NULL,&ManageShadowRamProtocol);
	ASSERT_EFI_ERROR(Status);

	DataSize = sizeof(ExitToSetupFlag);
	Status = pRS->GetVariable(
	             L"Exitflag",
	             &gManageShadowRamProtocolGuid,
	             NULL,
	             &DataSize,
	             &ExitToSetupFlag );

	TRACE((-1,"Dbg (CreateRsdpInFseg): Get Variable Exitflag %r.\n",Status));
	if(EFI_ERROR(Status)){
		Status = ManageShadowRamProtocol->HeapToF000((UINT8*)RSDP,16,(UINTN)sizeof(RSDT_PTR_20), &DataInfo);
		TRACE((-1,"Dbg (CreateRsdpInFseg): HeapToF000 %r \n",Status));
		return Status; //Preserved F seg too small, if halt here, that's mean preserve space less then 0x90 bytes.
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
