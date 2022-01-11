//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SleepSmi.c
    Provide functions to register and handle Sleep SMI
    functionality.  

*/

#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSxDispatch2.h>

#define AMI_SMM_SX_DISPATCH_PROTOCOL EFI_SMM_SX_DISPATCH2_PROTOCOL
#define AMI_SMM_SX_DISPATCH_CONTEXT  EFI_SMM_SX_REGISTER_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS   EFI_SUCCESS


// Function declarations

VOID ChipsetSleepWorkaround(
    VOID
);

// Function Definitions

/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S1.

    @param DispatchHandle - SMI dispatcher handle
    @param DispatchContext - Points to an optional Sx SMI context
    @param CommBuffer - Points to the optional communication buffer
    @param CommBufferSize - Points to the size of the optional
                            communication buffer

    @retval EFI_STATUS

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/
EFI_STATUS S1SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    return SMM_CHILD_DISPATCH_SUCCESS;
}


/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S3.

    @param DispatchHandle - SMI dispatcher handle
    @param DispatchContext - Points to an optional Sx SMI context
    @param CommBuffer - Points to the optional communication buffer
    @param CommBufferSize - Points to the size of the optional
                            communication buffer

    @retval EFI_STATUS
     
    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/
EFI_STATUS S3SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ChipsetSleepWorkaround();
    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S4.

    @param DispatchHandle - SMI dispatcher handle
    @param DispatchContext - Points to an optional Sx SMI context
    @param CommBuffer - Points to the optional communication buffer
    @param CommBufferSize - Points to the size of the optional
                            communication buffer

    @retval EFI_STATUS

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/
EFI_STATUS S4SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ChipsetSleepWorkaround();
    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S1.

    @param DispatchHandle - SMI dispatcher handle
    @param DispatchContext - Points to an optional Sx SMI context
    @param CommBuffer - Points to the optional communication buffer
    @param CommBufferSize - Points to the size of the optional
                            communication buffer

    @retval EFI_STATUS

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/
EFI_STATUS S5SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ChipsetSleepWorkaround();
    return SMM_CHILD_DISPATCH_SUCCESS;
}


/**
    This function executes chipset workaround that is needed.  It is 
    necessary for the system to go to S3 - S5.

    @param VOID

    @retval VOID
**/
VOID ChipsetSleepWorkaround( 
    VOID 
)   
{
/** CHIPSET PORTING IF NEEDED.
    UINT16 Value;
    UINT32 PciRegister = 0x800000D4;
    
    IoWrite32(0xCF8,PciRegister);
    Value = IoRead16(0xCFC);
    Value |= BIT14;
    IoWrite16(0xCFC,Value);
 **/
}

/** @file
    Install Sleep SMI Handlers for south bridge.

    @param ImageHandle - Image handle
    @param SystemTable - Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                      Status;
    EFI_HANDLE                      hS1Smi;
    EFI_HANDLE                      hS3Smi;
    EFI_HANDLE                      hS4Smi;
    EFI_HANDLE                      hS5Smi;
    AMI_SMM_SX_DISPATCH_PROTOCOL    *SxDispatch;
    AMI_SMM_SX_DISPATCH_CONTEXT     S1DispatchContext = {SxS1, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S3DispatchContext = {SxS3, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S4DispatchContext = {SxS4, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S5DispatchContext = {SxS5, SxEntry};

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;
    Status = pSmst->SmmLocateProtocol( &gEfiSmmSxDispatch2ProtocolGuid , \
                                       NULL, \
                                       &SxDispatch );

    if (EFI_ERROR(Status)) return Status;

    // Register Sleep SMI Handlers
    Status = SxDispatch->Register( SxDispatch, \
                                   S1SleepSmiOccurred, \
                                   &S1DispatchContext, \
                                   &hS1Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S3SleepSmiOccurred, \
                                   &S3DispatchContext, \
                                   &hS3Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S4SleepSmiOccurred, \
                                   &S4DispatchContext, \
                                   &hS4Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S5SleepSmiOccurred, \
                                   &S5DispatchContext, \
                                   &hS5Smi );
    return Status;
}
/**
    This function Registers Sleep SMI functionality.

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS InitSleepSmi(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    InitAmiLib( ImageHandle, SystemTable );

    return InitSmmHandler( ImageHandle, SystemTable, InSmmFunction, NULL );
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
