//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AcpiModeEnable.c
    Provide functions to enable and disable ACPI mode

*/

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>

#include <Protocol/DevicePath.h>
#include "AcpiModeEnable.h"
#include <SbElinks.h>

// Denverton Aptiov Override Start - EIP#221424
#include <PchAccess.h> 
#include <Guid\Rtc.h>
// Denverton Aptiov Override End - EIP#221424

#define AMI_SMM_SW_DISPATCH_PROTOCOL EFI_SMM_SW_DISPATCH2_PROTOCOL
#define AMI_SMM_SW_DISPATCH_CONTEXT  EFI_SMM_SW_REGISTER_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS   EFI_SUCCESS

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

ACPI_DISPATCH_LINK  *gAcpiEnDispatchHead = 0, *gAcpiEnDispatchTail = 0;
ACPI_DISPATCH_LINK  *gAcpiDisDispatchHead = 0, *gAcpiDisDispatchTail = 0;

typedef VOID (ACPI_MODE_CALLBACK) (
    IN EFI_HANDLE                   DispatchHandle,
    IN AMI_SMM_SW_DISPATCH_CONTEXT  *DispatchContext
);

extern ACPI_MODE_CALLBACK ACPI_ENABLE_CALL_BACK_LIST EndOfList;
ACPI_MODE_CALLBACK* AcpiEnableCallbackList[] = { ACPI_ENABLE_CALL_BACK_LIST NULL };

extern ACPI_MODE_CALLBACK ACPI_DISABLE_CALL_BACK_LIST EndOfList;
ACPI_MODE_CALLBACK* AcpiDisableCallbackList[] = { ACPI_DISABLE_CALL_BACK_LIST NULL };

/**
    Create and add link to specified list.

    @param Size - The size of the Link
    @param Head - Pointer to the head of the Link
    @param Tail - Pointer to the tail of the Link

    @retval VOID Pointer

    @note  Here is the control flow of this function:
              1. Allocate Link in SMM Pool.
              2. Add Link to end.
              3. Return Link address.

**/
VOID * AddLink(
    IN UINT32       Size,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    VOID *Link;

    if (pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, &Link) != EFI_SUCCESS) return 0;

    ((GENERIC_LINK*)Link)->Link = 0;
    if (!*Head)
    {
        *Head = *Tail = Link;
    }
    else
    {
        ((GENERIC_LINK*)*Tail)->Link = Link;
        *Tail = Link;
    }

    return Link;
}

/**
    Remove link from specified list.

    @param Handle - EFI Handle
    @param Head - Pointer to the head of the Link
    @param Tail - Pointer to the tail of the Link

    @retval BOOLEAN
            TRUE if link was removed. FALSE if link not in the list.

    @note  Here is the control flow of this function:
              1. Search link list for Link.
              2. Remove link from list.
              3. Free link.

**/
BOOLEAN RemoveLink(
    IN EFI_HANDLE   Handle,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    GENERIC_LINK *PrevLink, *Link;

    PrevLink = *Head;

    // Is link first. Link address is the same as the Handle.
    if (((GENERIC_LINK*)*Head) == Handle)
    {
        if (PrevLink == *Tail) *Tail = 0; // If Tail = Head, then 0.
        *Head = PrevLink->Link;
        pSmst->SmmFreePool(PrevLink);
        return TRUE;
    }

    // Find Link.
    for (Link = PrevLink->Link; Link; PrevLink = Link, Link = Link->Link)
    {
        if (Link == Handle)     // Link address is the same as the Handle.
        {
            if (Link == *Tail) *Tail = PrevLink;
            PrevLink->Link = Link->Link;
            pSmst->SmmFreePool(Link);
            return TRUE;
        }
    }

    return FALSE;
}

/**
    Register a Link on ACPI enable SMI.

    @param This - Pointer to ACPI dispatch protocol
    @param Function - A function to be dispatched.
    @param Handle - Pointer to EFI Handle

    @retval EFI Handle & EFI_STATUS

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable SMI Source.

**/
EFI_STATUS EfiAcpiEnRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      &gAcpiEnDispatchHead, \
                      &gAcpiEnDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

/**
    Unregister a Link on ACPI enable SMI.

    @param This - Pointer to ACPI dispatch protocol
    @param Handle - EFI Handle

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/
EFI_STATUS EfiAcpiEnUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, &gAcpiEnDispatchHead, &gAcpiEnDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

/**
    Register a Link on ACPI disable SMI.

    @param This - Pointer to ACPI dispatch protocol
    @param Function - A function to be dispatched.
    @param Handle - EFI Handle

    @retval EFI Handle & EFI_STATUS

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable SMI Source.

**/
EFI_STATUS EfiAcpiDisRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      &gAcpiDisDispatchHead, \
                      &gAcpiDisDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

/**
    Unregister a Link on ACPI Disable SMI.

    @param This - Pointer to ACPI dispatch protocol
    @param Handle - EFI Handle

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/
EFI_STATUS EfiAcpiDisUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, &gAcpiDisDispatchHead, &gAcpiDisDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiEnDispatchProtocol = \
        {EfiAcpiEnRegister, EfiAcpiEnUnregister};

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiDisDispatchProtocol = \
        {EfiAcpiDisRegister, EfiAcpiDisUnregister};


/**
    This function enable ACPI mode by clearing all SMI and
    enabling SCI generation
    This routine is also called on a S3 resume for special ACPI
    programming.
    Status should not be cleared on S3 resume. Enables are
    already taken care of.

    @param DispatchHandle - SMI dispatcher handle
    @param DispatchContext - Points to an optional S/W SMI context
    @param CommBuffer - Points to the optional communication buffer
    @param CommBufferSize - Points to the size of the optional
                            communication buffer

    @retval EFI_STATUS if the new SMM PI is applied.

**/
EFI_STATUS EnableAcpiMode (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ACPI_DISPATCH_LINK      *Link;
    UINTN					Index;
    // Denverton Aptiov Override start - EIP#221424
    UINT32 Data32;
 
    //
    // Disable SW SMI Timer
    //
    Data32 = IoRead32 (PcdGet16 (PcdAcpiIoPortBaseAddress) + R_PCH_SMI_EN);
    Data32 &= B_PCH_SMI_EN_INTEL_USB2 |
          B_PCH_SMI_EN_LEGACY_USB2 |
          B_PCH_SMI_EN_APMC |
          B_PCH_SMI_EN_ON_SLP_EN |
          B_PCH_SMI_EN_GBL_SMI;
    IoWrite32(PcdGet16 (PcdAcpiIoPortBaseAddress) + R_PCH_SMI_EN, Data32);
  
    //
    // Disable PM sources except power button
    //
    IoWrite16 (PcdGet16 (PcdAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_EN, 0x0000);
  
    //
    // Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
    //
    IoWrite8 (RTC_INDEX_REG, RTC_REG_D_INDEX);
    IoWrite8 (RTC_DATA_REG, 0);
  
    //
    // Enable SCI
    //
    IoOr32 (PcdGet16 (PcdAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT, B_PCH_ACPI_PM1_CNT_SCI_EN); 
    // Denverton Aptiov Override End - EIP#221424

    for (Link = gAcpiEnDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }

    for (Index = 0; AcpiEnableCallbackList[Index] != NULL; Index++) 
    {
    	AcpiEnableCallbackList[Index](DispatchHandle, DispatchContext);    	
    }
    
    IoWrite8(0x80, SW_SMI_ACPI_ENABLE);

    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function disables ACPI mode by enabling SMI generation

    @param DispatchHandle - SMI dispatcher handle
    @param DispatchContext - Points to an optional S/W SMI context
    @param CommBuffer - Points to the optional communication buffer
    @param CommBufferSize - Points to the size of the optional
                            communication buffer

    @retval EFI_STATUS if the new SMM PI is applied.

**/
EFI_STATUS DisableAcpiMode (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ACPI_DISPATCH_LINK      *Link;
    UINTN					Index;

    for (Link = gAcpiDisDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }

    for (Index = 0; AcpiDisableCallbackList[Index] != NULL; Index++) 
    {
    	AcpiDisableCallbackList[Index](DispatchHandle, DispatchContext);    	
    }

    // Denverton Aptiov Override start - EIP#221424
    //
    // Disable SCI
    //
    IoAnd32 (PcdGet16 (PcdAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT, (UINT32)(~B_PCH_ACPI_PM1_CNT_SCI_EN));
    // Denverton Aptiov Override End - EIP#221424
    
    IoWrite8(0x80, SW_SMI_ACPI_DISABLE);

    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function is part of the ACPI mode enable/disable
    driver and invoked during SMM initialization.  As the name
    suggests this function is called from SMM

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS AcpiModeEnableInSmmFunction(
    IN  EFI_HANDLE          ImageHandle, 
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS                   Status;
    EFI_HANDLE                   Handle = NULL;
    AMI_SMM_SW_DISPATCH_PROTOCOL *SwDispatch = NULL;
    EFI_HANDLE                   DummyHandle = NULL;
    AMI_SMM_SW_DISPATCH_CONTEXT  AcpiEnableContext = {SW_SMI_ACPI_ENABLE};
    AMI_SMM_SW_DISPATCH_CONTEXT  AcpiDisableContext = {SW_SMI_ACPI_DISABLE};

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    Status = pSmst->SmmLocateProtocol( &gEfiSmmSwDispatch2ProtocolGuid , \
                                       NULL, \
                                       &SwDispatch );
    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch->Register( SwDispatch, \
                                   EnableAcpiMode, \
                                   &AcpiEnableContext, \
                                   &Handle );
    if (EFI_ERROR(Status)) return Status;   

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiEnDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiEnDispatchProtocol
                      );
    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch->Register( SwDispatch, \
                                   DisableAcpiMode, \
                                   &AcpiDisableContext,\
                                   &Handle );
    if (EFI_ERROR(Status)) return Status;   

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiDisDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiDisDispatchProtocol
                      );

    return Status;
}

/**
    This function is the entry point for the ACPI mode enable/disable
    driver. This function is called twice: first time by the DXE 
    dispatcher and the next time when it is loaded into the SMRAM.

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS 
AcpiModeEnableInit(
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    InitAmiLib(ImageHandle,SystemTable);

    return InitSmmHandler(ImageHandle, 
                          SystemTable, 
                          AcpiModeEnableInSmmFunction, 
                          NULL);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
