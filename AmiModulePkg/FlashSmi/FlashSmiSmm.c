//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file
  Common file for the SMM driver. Contains the main code for FlashSmiSmm module.
  
  FlashSmi module will hook FLASH_PROTOCOL functions to genrate SMI to use
  functions in SMM.
  
**/

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <Uefi.h>

#include <Protocol/SmmBase.h>
#include <Protocol/FlashProtocol.h>

#include <Library/AmiBufferValidationLib.h>

#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#include "FlashSmi.h"

EFI_GUID                    mFlashSmiGuid = FLASH_SMI_GUID;

FLASH_PROTOCOL              *mFlashSmm = NULL;

FLASH_ERASE                 mSavedFlashEraseSmm;
FLASH_READ_WRITE            mSavedFlashWriteSmm;
FLASH_READ_WRITE            mSavedFlashUpdateSmm;
FLASH_WRITE_ENABLE_DISABLE  mSavedWriteEnableSmm;
FLASH_WRITE_ENABLE_DISABLE  mSavedWriteDisableSmm;

//----------------------------------------------------------------------------
// Function Definitions
/**
    Main SMI handler function.

    @param DispatchHandle The unique handle assigned to this handler by SmiHandlerRegister()
    @param Context Points to an optional handler context which was specified when the handler was registered
    @param CommBuffer A pointer to memory will be conveyed from a non-SMM environment into an SMM environment
    @param CommBufferSize The size of the CommBuffer

    @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
FlashSmiHandler(
    IN      EFI_HANDLE  DispatchHandle,
    IN      CONST VOID  *Context        OPTIONAL,
    IN  OUT VOID        *CommBuffer     OPTIONAL,
    IN  OUT UINTN       *CommBufferSize OPTIONAL
)
{
    FLASH_SMI_UPDATE    *SmmFlashUpdate;
    EFI_STATUS          Status;

    if( CommBuffer == NULL )
        return EFI_SUCCESS;

    SmmFlashUpdate = (FLASH_SMI_UPDATE*)CommBuffer;

    Status = AmiValidateMemoryBuffer(
                (VOID*)SmmFlashUpdate,
                sizeof(FLASH_SMI_UPDATE) );
    if( EFI_ERROR(Status) )
        return EFI_SUCCESS;

    if( SmmFlashUpdate->Signature != FSMI_SIGNATURE )
        return EFI_SUCCESS;

    switch( SmmFlashUpdate->Subfunction )
    {
        case 'Fu':
        case 'Fw':
        {
            Status = AmiValidateMemoryBuffer(
                        (VOID*)(SmmFlashUpdate->DataBuffer),
                        SmmFlashUpdate->Size );
            if( EFI_ERROR(Status) )
            {
                SmmFlashUpdate->Status = Status;
                return EFI_SUCCESS;
            }
        }
        case 'Fe':
        {
            Status = AmiValidateMmioBuffer(
                        (VOID*)(SmmFlashUpdate->FlashAddress),
                        SmmFlashUpdate->Size );
            if( EFI_ERROR(Status) )
            {
                SmmFlashUpdate->Status = Status;
                return EFI_SUCCESS;
            }
        }
        break;
    }

    if( mFlashSmm == NULL )
        return EFI_SUCCESS;

    switch( SmmFlashUpdate->Subfunction )
    {
        // FlashUpdate call
        case 'Fu':
        {
            Status = mSavedFlashUpdateSmm(
                        (VOID*)(SmmFlashUpdate->FlashAddress),
                        SmmFlashUpdate->Size,
                        (VOID*)(SmmFlashUpdate->DataBuffer) );
        }
        break;

        // FlashErase call
        case 'Fe':
        {
            Status = mSavedFlashEraseSmm(
                        (VOID*)(SmmFlashUpdate->FlashAddress),
                        SmmFlashUpdate->Size );
        }
        break;

        // FlashWrite call
        case 'Fw':
        {
            Status = mSavedFlashWriteSmm(
                        (VOID*)(SmmFlashUpdate->FlashAddress),
                        SmmFlashUpdate->Size,
                        (VOID*)(SmmFlashUpdate->DataBuffer));
        }
        break;

        // WriteEnable call
        case 'We':
        {
            Status = mSavedWriteEnableSmm();
        }
        break;

        // WriteDisable call
        case 'Wd':
        {
            Status = mSavedWriteDisableSmm();
        }
        break;
        
        default:
        {
            SmmFlashUpdate->Status = EFI_UNSUPPORTED;
            return EFI_SUCCESS;
        }
    }

    SmmFlashUpdate->Status = Status;
    SmmFlashUpdate->Subfunction = 0;
    // Invalidate Flash SMI Buffer.
    SmmFlashUpdate->Signature = ~FSMI_SIGNATURE;

    return EFI_SUCCESS;
}
/**
    Main function in SMM.

    @param ImageHandle image handle
    @param SystemTable pointer to EFI_SYSTEM_TABLE

    @return VOID

**/
EFI_STATUS
FlashSmiSmmFunction(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  Handle = NULL;

    // Locate SmmFlashProtocol.
    Status = pBS->LocateProtocol( &gFlashSmmProtocolGuid, NULL, &mFlashSmm );
    if( EFI_ERROR(Status) )
        mFlashSmm = NULL;

    if( mFlashSmm != NULL )
    {
        mSavedFlashEraseSmm = mFlashSmm->Erase;
        mSavedFlashWriteSmm = mFlashSmm->Write;
        mSavedFlashUpdateSmm = mFlashSmm->Update;
        mSavedWriteEnableSmm = mFlashSmm->DeviceWriteEnable;
        mSavedWriteDisableSmm = mFlashSmm->DeviceWriteDisable;
    }

    // Register Flash Software SMI.
    Status = pSmst->SmiHandlerRegister(
                        FlashSmiHandler,
                        &mFlashSmiGuid,
                        &Handle );

    return Status;
}
/**
    Module entry point.

    @param ImageHandle image handle
    @param SystemTable pointer to EFI_SYSTEM_TABLE

    @return VOID

**/
EFI_STATUS
EFIAPI
FlashSmiSmmEntry(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_BOOT_MODE   Bootmode;

    InitAmiLib( ImageHandle, SystemTable );

    Bootmode = GetBootMode();
    if( (Bootmode == BOOT_ON_FLASH_UPDATE) ||
        (Bootmode == BOOT_IN_RECOVERY_MODE) )
    {
        return InitSmmHandlerEx(
                    ImageHandle,
                    SystemTable,
                    FlashSmiSmmFunction,
                    NULL );
    }

    return EFI_UNSUPPORTED;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************