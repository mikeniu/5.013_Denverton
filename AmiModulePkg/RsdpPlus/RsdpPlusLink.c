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

/** @file RsdpPlusLink.c 
	Contain E-link functions.

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include "ShadowRamProtocol.h"


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
SHADOW_RAM_PROTOCOL	*gShadowRamProtocol;

// GUID Definition(s)
EFI_GUID gShdowRamProtocolGuid = SHADOW_RAM_PROTOCOL_GUID;

// Protocol Definition(s)

// External Declaration(s)
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;

// Function Definition(s)

//---------------------------------------------------------------------------


/**
    BeforeEfiBootLaunchHook eLink function.
    
**/
VOID UpdateShadow(VOID)
{
	EFI_STATUS	Status;
	
	Status = gBS->LocateProtocol(&gShdowRamProtocolGuid,NULL,&gShadowRamProtocol);
    if(!EFI_ERROR(Status))
	    gShadowRamProtocol->UpdateShadowBeforEfiBoot();
		
	return;
}

/**
    AfterEfiBootLaunchHook eLink function.
    
**/
VOID EraseShadow(VOID)
{
	EFI_STATUS	Status;

	Status = gBS->LocateProtocol(&gShdowRamProtocolGuid,NULL,&gShadowRamProtocol);
    if(!EFI_ERROR(Status))
	    gShadowRamProtocol->EraseShadowAfterEfiBoot();
	
	return;
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
