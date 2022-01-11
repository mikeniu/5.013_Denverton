//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file
  SceHiiInterface SMM driver.
  
**/

#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/AmiBufferValidationLib.h>

#include <Protocol/AmiNvramUpdate.h>


#define AMI_REFLASH_SMM_HANDLER_GUID \
    { 0xe90a7b5e, 0x37b2, 0x43e4,  { 0xb2, 0x81, 0x4c, 0x8e, 0x34, 0x90, 0x45, 0xe9 }}

typedef struct {
    UINT64 Action;
    UINT64 Status;
    UINT64 Data[4];
} AMI_REFLASH_SMM_COMMUNICATE_HEADER;

/**
  Dispatch function for a Software SMI handler.

  Caution: This function may receive untrusted input.
  Communicate buffer and buffer size are external input, so this function will do basic validation.

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
static EFI_STATUS
EFIAPI
ReflashSmmandler (
    IN EFI_HANDLE  DispatchHandle,
    IN CONST VOID  *Context         OPTIONAL,
    IN OUT VOID    *CommBuffer      OPTIONAL,
    IN OUT UINTN   *CommBufferSize  OPTIONAL
    )
{
    AMI_REFLASH_SMM_COMMUNICATE_HEADER *hdr;
    AMI_NVRAM_UPDATE_PROTOCOL *nu;
    EFI_STATUS Status;
    VOID *p;
    
    if (CommBuffer == NULL || CommBufferSize == NULL)
        return EFI_SUCCESS;
    
    Status = AmiValidateMemoryBuffer(CommBuffer, *CommBufferSize);
    if (EFI_ERROR (Status))
    	return EFI_SUCCESS;
    
    hdr = (AMI_REFLASH_SMM_COMMUNICATE_HEADER *)CommBuffer;
    Status = gSmst->SmmLocateProtocol (&gAmiSmmNvramUpdateProtocolGuid, NULL, &nu);
    if (EFI_ERROR (Status)) {
        hdr->Status = (UINT64) Status;
        return EFI_SUCCESS;
    }
    
    if (hdr->Action < 2) {
        p = (VOID *)(UINTN)hdr->Data[0];
        Status = AmiValidateMemoryBuffer(p, (UINTN)hdr->Data[1]);
        if (!EFI_ERROR (Status))
        	Status = nu->UpdateNvram (nu, p, (UINTN)hdr->Data[1], (BOOLEAN)hdr->Action);
    } else {
        Status = nu->MigrateNvram (nu, hdr->Data[0], hdr->Data[1], (UINTN)hdr->Data[2]);
    }
    hdr->Status = (UINT64) Status;
    return EFI_SUCCESS;
}

/**
  Entry Point for SceHiiInterface SMM driver.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  A Pointer to the EFI System Table.

  @retval EFI_SUCEESS     
  @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
ReFlashSmmEntry (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    EFI_STATUS Status;
    EFI_HANDLE DispatchHandle;
    static EFI_GUID AmiReflashSmmHandlerGuid = AMI_REFLASH_SMM_HANDLER_GUID;
    
    Status = gSmst->SmiHandlerRegister (
                    ReflashSmmandler,
                    &AmiReflashSmmHandlerGuid,
                    &DispatchHandle);
    return Status;
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
