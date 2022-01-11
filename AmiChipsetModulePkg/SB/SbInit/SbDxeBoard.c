//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbDxeBoard.c
    This file contains DXE stage board component code for
    Template SB

*/

// Module specific Includes
#include <Efi.h>
#include <DXE.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <SbDxeInitElink.h>
#include <Setup.h>
#include <Protocol/PciIo.h>
#include <protocol/BlockIo.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#if SB_IDE_SETUP_PROTOCOL_SUPPORT
#include <Protocol/AmiIdeBus.h>



// Produced Protocols

// GUID Definitions
EFI_GUID    gIdeSetupProtocolguid   = IDE_SETUP_PROTOCOL_GUID;
#endif
// Portable Constants

// Function Prototypes

// PPI interface definition


// Protocols that are installed

// Function Definition

typedef VOID (SAVE_RESTORE_CALLBACK)( BOOLEAN Save );

extern SAVE_RESTORE_CALLBACK SAVE_RESTORE_CALLBACK_LIST EndOfList;
SAVE_RESTORE_CALLBACK* SaveRestoreCallbackList[] = { SAVE_RESTORE_CALLBACK_LIST NULL };

/**
    This function calls registered callbacks to save/restore registers
    value in timer interrupt routine

    @param BOOLEAN Save - if TRUE - save registers, FALSE - restore back

    @retval VOID

**/
VOID SaveRestoreRegisters(
    IN BOOLEAN Save
)
{
    UINTN i;
    for(i = 0; SaveRestoreCallbackList[i] != NULL; i++) 
        SaveRestoreCallbackList[i](Save);
}

/**
    This function initializes the board specific component in
    in the chipset South bridge

    @param ImageHandle - Image handle
    @param SystemTable - Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.
**/
EFI_STATUS SbDxeBoardInit (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
)
{
#if SB_IDE_SETUP_PROTOCOL_SUPPORT    
    EFI_STATUS              Status;
    EFI_HANDLE              Handle = NULL;
    SETUP_DATA              *SetupData = NULL;
    EFI_GUID                gSetupGuid = SETUP_GUID;
    UINTN                   VariableSize = 0;
    AMI_IDE_SETUP_PROTOCOL  *gIdeSetupProtocol;
#endif

    InitAmiLib( ImageHandle, SystemTable );

#if SB_IDE_SETUP_PROTOCOL_SUPPORT
    Status = pBS->AllocatePool (EfiBootServicesData,
                            sizeof(AMI_IDE_SETUP_PROTOCOL),
                            (VOID**)&gIdeSetupProtocol);
    
    if (EFI_ERROR(Status)){
        return Status;
    }
    
    Status = GetEfiVariable( L"Setup",
                             &gSetupGuid,
                             NULL,
                             &VariableSize,
                             &SetupData
                           );
    
    #if ACOUSTIC_MANAGEMENT_SUPPORT
    if (!EFI_ERROR(Status)) { 
    gIdeSetupProtocol->AcousticPwrMgmt = SetupData->HDD_AcousticPwrMgmt;
    gIdeSetupProtocol->AcousticLevel   = SetupData->HDD_AcousticMode;
    } else {
    gIdeSetupProtocol->AcousticPwrMgmt = ACOUSTIC_SUPPORT_DISABLE;
    gIdeSetupProtocol->AcousticLevel   = ACOUSTIC_LEVEL_BYPASS;
    }
    #endif
    
    #if DiPM_SUPPORT
    if (!EFI_ERROR(Status)) {
        gIdeSetupProtocol->Dipm = SetupData->HDD_DiPM;
    }   
    #endif
    
    // Create a handle for the ArchProtocol and install Watchdog Arch Protocol on the handle
    Status = pBS->InstallProtocolInterface(
                  &Handle,
                  &gIdeSetupProtocolguid,
                  EFI_NATIVE_INTERFACE,
                  gIdeSetupProtocol
                  );
#endif

    return EFI_SUCCESS;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
