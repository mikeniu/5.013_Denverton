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
  Common file for the DXE driver. Contains the main code for FlashSmiDxe module.
  
  FlashSmi module will hook FLASH_PROTOCOL functions to genrate SMI to use
  functions in SMM.
  
**/

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <Uefi.h>

#include <Protocol/FlashProtocol.h>
#include <Protocol/SmmCommunication.h>

#include <Library/AmiCriticalSectionLib.h>

#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#include "FlashSmi.h"

EFI_GUID    mFlashSmiGuid = FLASH_SMI_GUID;
EFI_GUID    mBdsConnectDriversProtocolGuid = BDS_CONNECT_DRIVERS_PROTOCOL_GUID;

EFI_SMM_COMMUNICATE_HEADER      *mCommunicateHeader = NULL;
FLASH_SMI_UPDATE                *mFlashSmiBuffer = NULL;
FLASH_PROTOCOL                  *mFlash = NULL;
EFI_SMM_COMMUNICATION_PROTOCOL  *mSmmCommunicate = NULL;

FLASH_ERASE                     mSavedFlashErase;
FLASH_READ_WRITE                mSavedFlashWrite;
FLASH_READ_WRITE                mSavedFlashUpdate;
FLASH_WRITE_ENABLE_DISABLE      mSavedWriteEnable;
FLASH_WRITE_ENABLE_DISABLE      mSavedWriteDisable;

CRITICAL_SECTION                mFlashSmiCs;

//----------------------------------------------------------------------------
// Function Definitions
/**
    Event to locate EFI_SMM_COMMUNICATION_PROTOCOL.

    @param Event
    @param Context

    @return VOID

**/
VOID
EFIAPI
SmmCommunicationReady(
    IN  EFI_EVENT   Event, 
    IN  VOID        *Context
)
{
    pBS->CloseEvent( Event );

    pBS->LocateProtocol(
            &gEfiSmmCommunicationProtocolGuid,
            NULL,
            &mSmmCommunicate );
}
/**
    Use EFI_SMM_COMMUNICATION_PROTOCOL to generate SMI.
  
    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS Generate SMI successful

**/
EFI_STATUS
GenerateFlashSmi(
    IN  VOID
)
{
    EFI_STATUS  Status;
    UINTN       SmmCommunicateSize;

    if( (mSmmCommunicate == NULL) || (mCommunicateHeader == NULL) )
        return EFI_UNSUPPORTED;

    SmmCommunicateSize = FLASH_SMI_COMMUNICATE_SIZE;

    Status = mSmmCommunicate->Communicate(
                                mSmmCommunicate,
                                (VOID*)mCommunicateHeader,
                                &SmmCommunicateSize );

    return Status;
}
/**
    Update the region of the flash part with the provided data.
  
    @param FlashAddress Pointer to address of a flash to update
    @param Size Size to update
    @param DataBuffer Pointer to the data to write into the flash part
  
    @return EFI_STATUS
    @retval EFI_SUCCESS The data was written successfully
    @retval EFI_DEVICE_ERROR An error was encountered while writing the data

**/
EFI_STATUS
EFIAPI
FlashDriverUpdateSmi(
    IN  VOID    *FlashAddress,
    IN  UINTN   Size,
    IN  VOID    *DataBuffer
)
{
    EFI_STATUS  Status;

    Status = EFI_NO_RESPONSE;
    if( (mFlashSmiBuffer != NULL) && (mFlash != NULL) )
    {
        if( EFI_ERROR(BeginCriticalSection( mFlashSmiCs )) ) 
            return EFI_DEVICE_ERROR;

        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        mFlashSmiBuffer->Signature = FSMI_SIGNATURE;
        mFlashSmiBuffer->FlashAddress = (UINT32)FlashAddress;
        mFlashSmiBuffer->Size = Size;
        mFlashSmiBuffer->Status = EFI_NO_RESPONSE;
        mFlashSmiBuffer->Subfunction = 'Fu';
        mFlashSmiBuffer->DataBuffer = (UINT32)DataBuffer;
        GenerateFlashSmi();
        Status = mFlashSmiBuffer->Status;
        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        EndCriticalSection( mFlashSmiCs );
    }

    if( Status == EFI_NO_RESPONSE )
        Status = mSavedFlashUpdate( FlashAddress, Size, DataBuffer );

    if( EFI_ERROR(Status) )
        Status = EFI_DEVICE_ERROR;

    return Status;
}
/**
    Write the passed data from DataBuffer into the flash part at FlashAddress

    @param FlashAddress The address in the flash part to write the data
    @param Size The size of the data to write
    @param DataBuffer Pointer to the buffer of data to write into the flash part

    @return EFI_STATUS
    @retval EFI_SUCCESS The data was written successfully
    @retval EFI_DEVICE_ERROR An error was encountered while writing the data

**/
EFI_STATUS
EFIAPI
FlashDriverWriteSmi(
    IN  VOID    *FlashAddress,
    IN  UINTN   Size,
    IN  VOID    *DataBuffer
)
{
    EFI_STATUS          Status;

    Status = EFI_NO_RESPONSE;
    if( (mFlashSmiBuffer != NULL) && (mFlash != NULL) )
    {
        if( EFI_ERROR(BeginCriticalSection( mFlashSmiCs )) ) 
            return EFI_DEVICE_ERROR;

        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        mFlashSmiBuffer->Signature = FSMI_SIGNATURE;
        mFlashSmiBuffer->FlashAddress = (UINT32)FlashAddress;
        mFlashSmiBuffer->Size = Size;
        mFlashSmiBuffer->Status = EFI_NO_RESPONSE;
        mFlashSmiBuffer->Subfunction = 'Fw';
        mFlashSmiBuffer->DataBuffer = (UINT32)DataBuffer;
        GenerateFlashSmi();
        Status = mFlashSmiBuffer->Status;
        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        EndCriticalSection( mFlashSmiCs );
    }

    if( Status == EFI_NO_RESPONSE )
        Status = mSavedFlashWrite( FlashAddress, Size, DataBuffer );

    if( EFI_ERROR(Status) )
        Status = EFI_DEVICE_ERROR;

    return Status;
}
/**
    Erase the flash part starting at the passed Address.

    @param FlashAddress Pointer to the address of the flash to erase
    @param Size Size, in bytes, of the flash to erase

    @return EFI_STATUS
    @retval EFI_SUCCESS The desired portion of the flash was erased
    @retval EFI_DEVICE_ERROR An error was encountered while erasing the flash

**/
EFI_STATUS
EFIAPI
FlashDriverEraseSmi(
    IN  VOID    *FlashAddress,
    IN  UINTN   Size
)
{
    EFI_STATUS          Status;

    Status = EFI_NO_RESPONSE;
    if( (mFlashSmiBuffer != NULL) && (mFlash != NULL) )
    {
        if( EFI_ERROR(BeginCriticalSection( mFlashSmiCs )) ) 
            return EFI_DEVICE_ERROR;

        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        mFlashSmiBuffer->Signature = FSMI_SIGNATURE;
        mFlashSmiBuffer->FlashAddress = (UINT32)FlashAddress;
        mFlashSmiBuffer->Size = Size;
        mFlashSmiBuffer->Status = EFI_NO_RESPONSE;
        mFlashSmiBuffer->Subfunction = 'Fe';
        GenerateFlashSmi();
        Status = mFlashSmiBuffer->Status;
        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        EndCriticalSection( mFlashSmiCs );
    }

    if( Status == EFI_NO_RESPONSE )
        mSavedFlashErase( FlashAddress, Size );

    if( EFI_ERROR(Status) )
        Status = EFI_DEVICE_ERROR;

    return Status;
}
/**
    Enable writing to the flash device.

    @return EFI_STATUS
    @retval EFI_SUCCESS Writing to the flash part has been enabled

**/
EFI_STATUS
EFIAPI
FlashDriverWriteEnableSmi(
    IN  VOID
)
{
    EFI_STATUS          Status;

    Status = EFI_NO_RESPONSE;
    if( (mFlashSmiBuffer != NULL) && (mFlash != NULL) )
    {
        if( EFI_ERROR(BeginCriticalSection( mFlashSmiCs )) ) 
            return EFI_DEVICE_ERROR;

        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        mFlashSmiBuffer->Signature = FSMI_SIGNATURE;
        mFlashSmiBuffer->Status = EFI_NO_RESPONSE;
        mFlashSmiBuffer->Subfunction = 'We';
        GenerateFlashSmi();
        Status = mFlashSmiBuffer->Status;
        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        EndCriticalSection( mFlashSmiCs );
    }

    if( Status == EFI_NO_RESPONSE )
        mSavedWriteEnable ();

    if( EFI_ERROR(Status) )
        Status = EFI_DEVICE_ERROR;

    return Status;
}
/**
    Disable writing to the flash part

    @return EFI_STATUS
    @retval EFI_SUCCESS Writing to the flash part was disabled
    @retval EFI_DEVICE_ERROR There was an error restoring a firmware volume header signature

**/
EFI_STATUS
EFIAPI
FlashDriverWriteDisableSmi(
    IN  VOID
)
{
    EFI_STATUS          Status;

    Status = EFI_NO_RESPONSE;
    if( (mFlashSmiBuffer != NULL) && (mFlash != NULL) )
    {
        if( EFI_ERROR(BeginCriticalSection( mFlashSmiCs )) ) 
            return EFI_DEVICE_ERROR;

        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        mFlashSmiBuffer->Signature = FSMI_SIGNATURE;
        mFlashSmiBuffer->Status = EFI_NO_RESPONSE;
        mFlashSmiBuffer->Subfunction = 'Wd';
        GenerateFlashSmi();
        Status = mFlashSmiBuffer->Status;
        MemSet( mFlashSmiBuffer, sizeof(FLASH_SMI_UPDATE), 0 );
        EndCriticalSection( mFlashSmiCs );
    }

    if( Status == EFI_NO_RESPONSE )
        mSavedWriteDisable ();

    if( EFI_ERROR(Status) )
        Status = EFI_DEVICE_ERROR;

    return Status;
}
/**
    This function will be called when Connect Drivers Protocol is installed
    and will update FlashProtocol function in RunTime.

    @param Event signalled event
    @param Context calling context

    @return VOID

**/
VOID
EFIAPI
FlashSmiConnectDrivers(
    IN  EFI_EVENT   Event, 
    IN  VOID        *Context
)
{
    pBS->CloseEvent( Event );

    if( mFlashSmiBuffer == NULL )
        return;

    if( mFlash != NULL )
    {
        mSavedFlashUpdate = mFlash->Update;
        mSavedFlashErase = mFlash->Erase;
        mSavedFlashWrite = mFlash->Write;
        mSavedWriteEnable = mFlash->DeviceWriteEnable;
        mSavedWriteDisable = mFlash->DeviceWriteDisable;

        mFlash->Update = FlashDriverUpdateSmi;
        mFlash->Erase = FlashDriverEraseSmi;
        mFlash->Write = FlashDriverWriteSmi;
        mFlash->DeviceWriteEnable = FlashDriverWriteEnableSmi;
        mFlash->DeviceWriteDisable = FlashDriverWriteDisableSmi;
    }
}
/**
  This function will be called when Ready To Boot and will restore oringinal
  FlashProtocol function.

  @param Event signalled event
  @param Handle handle

  @return VOID
**/
VOID
EFIAPI
FlashSmiReadyToBoot(
    EFI_EVENT   Event,
    VOID        *Handle
)
{
    pBS->CloseEvent( Event );
    mFlash->Update = mSavedFlashUpdate;
    mFlash->Erase = mSavedFlashErase;
    mFlash->Write = mSavedFlashWrite;
    mFlash->DeviceWriteEnable = mSavedWriteEnable;
    mFlash->DeviceWriteDisable = mSavedWriteDisable;
}
/**
  This function will convert pointer.

  @param Event signalled event
  @param Context calling context

  @return VOID
**/
VOID
EFIAPI
FlashSmiVirtualFixup(
    IN  EFI_EVENT   Event, 
    IN  VOID        *Context
)
{
    pRS->ConvertPointer( 0, &mCommunicateHeader );
    pRS->ConvertPointer( 0, &mFlashSmiBuffer );
}
/**
  Main function in boot time.

  @param ImageHandle image handle
  @param SystemTable pointer to EFI_SYSTEM_TABLE

  @return VOID
**/
EFI_STATUS
FlashSmiDxeFunction(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS  Status;
    UINTN       BufferSize;
    VOID        *Buffer;
    EFI_EVENT   EvtSmmCommunicationReady;
    VOID        *RegSmmCommunicationReady;
    EFI_EVENT   EvtVirtualFixup;
    EFI_EVENT   EvtConnectDrivers;
    VOID        *RegConnectDrivers;
    EFI_EVENT   EvtReadyToBoot;

    Status = CreateCriticalSection( &mFlashSmiCs );
    if( EFI_ERROR(Status) )
        return Status;

    Status = pBS->LocateProtocol(
                    &gEfiSmmCommunicationProtocolGuid,
                    NULL,
                    &mSmmCommunicate );
    if( EFI_ERROR(Status) )
    {
        Status = RegisterProtocolCallback(
                    &gEfiSmmCommunicationProtocolGuid,
                    SmmCommunicationReady,
                    NULL,
                    &EvtSmmCommunicationReady,
                    &RegSmmCommunicationReady );
    }

    // Locate FlashProtocol.
    Status = pBS->LocateProtocol( &gFlashProtocolGuid, NULL, &mFlash );
    if( EFI_ERROR(Status) )
        return Status;

    //Allocate buffer for commnicate prorocol
    BufferSize = FLASH_SMI_COMMUNICATE_SIZE;
    Buffer = NULL;
    Status = pBS->AllocatePool(
                    EfiRuntimeServicesData,
                    BufferSize,
                    &Buffer );
    if( EFI_ERROR(Status) )
        return EFI_SUCCESS;

    MemSet( Buffer, BufferSize, 0 );
    mCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER*)Buffer;
    mFlashSmiBuffer = (FLASH_SMI_UPDATE*)mCommunicateHeader->Data;

    //Prepare Smm communicate header
    MemCpy( &mCommunicateHeader->HeaderGuid, &mFlashSmiGuid, sizeof(EFI_GUID) );
    mCommunicateHeader->MessageLength = sizeof(FLASH_SMI_UPDATE);

    Status = pBS->CreateEvent(
                    EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE,
                    TPL_CALLBACK,
                    FlashSmiVirtualFixup,
                    NULL,
                    &EvtVirtualFixup );
    if( EFI_ERROR(Status) )
        return EFI_SUCCESS;

    RegConnectDrivers = NULL;
    Status = RegisterProtocolCallback(
                &mBdsConnectDriversProtocolGuid,
                FlashSmiConnectDrivers,
                NULL,
                &EvtConnectDrivers,
                &RegConnectDrivers );
    if( EFI_ERROR(Status) )
        return EFI_SUCCESS;

    Status = CreateReadyToBootEvent(
                TPL_CALLBACK,
                FlashSmiReadyToBoot,
                NULL,
                &EvtReadyToBoot );

    return EFI_SUCCESS;
}
/**
  Module entry point.

  @param ImageHandle image handle
  @param SystemTable pointer to EFI_SYSTEM_TABLE

  @return VOID
**/
EFI_STATUS
EFIAPI
FlashSmiDxeEntry(
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
        return FlashSmiDxeFunction( ImageHandle, SystemTable );
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