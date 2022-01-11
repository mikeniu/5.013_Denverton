
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

/** @file SbSmm.c
    This file contains code for SMM control - the
    protocol defined by Framework

*/

// Module specific Includes
#include <Efi.h>
#include <token.h>
#include <HOB.h>
#include <DXE.h>
#include <AmiLib.h>
#include <AmiCspLib.h>

// Used Protocols
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SmmAccess2.h>
// Produced Protocols
#include <Protocol/SmmControl2.h>

#if defined SB_SMM_CONTROL2_PROTOCOL_SUPPORT && SB_SMM_CONTROL2_PROTOCOL_SUPPORT == 1
// Function Prototypes
EFI_STATUS SbSmmClearSmi2 (
    IN CONST EFI_SMM_CONTROL2_PROTOCOL  *This,
    IN BOOLEAN                          Periodic OPTIONAL
);

EFI_STATUS SbSmmTriggerSmi2(
    IN CONST EFI_SMM_CONTROL2_PROTOCOL  *This,
    IN OUT UINT8                        *CommandPort       OPTIONAL,
    IN OUT UINT8                        *DataPort          OPTIONAL,
    IN BOOLEAN                          Periodic           OPTIONAL,
    IN UINTN                            ActivationInterval OPTIONAL
);
#endif
// GUID Definitions

// Global variable declarations
EFI_SMM_ACCESS2_PROTOCOL            *gSmmAccess2;
#if defined SB_SMM_CONTROL2_PROTOCOL_SUPPORT && SB_SMM_CONTROL2_PROTOCOL_SUPPORT == 1
EFI_SMM_CONTROL2_PROTOCOL gEfiSmmControl2Protocol =
{
    SbSmmTriggerSmi2,
    SbSmmClearSmi2,
    0
};

#endif

EFI_S3_SAVE_STATE_PROTOCOL        *gBootScript;;

// Portable Constants

// Function Definition

#if defined SB_SMM_CONTROL2_PROTOCOL_SUPPORT && SB_SMM_CONTROL2_PROTOCOL_SUPPORT == 1
/**
    This function programs the SB chipset registers to enable
    S/W SMI generation

    @param VOID

    @retval EFI_SUCCESS Always

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS SbSmmEnableSwSmi(
    VOID
)
{
//  UINT8           Value;

//  SbSmmClearSmi2 (&gEfiSmmControl2Protocol,FALSE);

/* Porting Required.  Include code to enable S/W SMI generation
    Value = IoRead8 (
                    PM_BASE_ADDRESS + AMD_8111_PM_SMI_NABL);
    IoWrite8 (
            PM_BASE_ADDRESS + AMD_8111_PM_SMI_NABL, 
            (UINT8)(Value | (1 << 7)));             //Enable Software SMI.

    Value = IoRead8 (
                    PM_BASE_ADDRESS + AMD_8111_PM_GLBL_CTRL);
    IoWrite8 (
            PM_BASE_ADDRESS + AMD_8111_PM_GLBL_CTRL, 
            (Value | 1));                           //Enable global SMIs.
 */

    return EFI_SUCCESS;
}

/**
    This function generates a software SMI by writing the
    provided 2 byte values Command & Data into the software SMI
    generation registers.

    @param Command - The value written to the command port.
    @param Data - The value written to the data port. (Optional)

    @retval VOID

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
VOID GenerateSwSmi (
    IN UINT8            Command,
    IN UINT8            Data )
{
/* Porting Required.  Include code to generate S/W SMI
    SET_IO32_PM(ACPI_IOREG_SMI_EN, 0x21); // Enable S/W SMI
    WRITE_IO8(SW_SMI_IO_DATA_ADDRESS, Data); // Write SMI data port
    WRITE_IO8(IO_DELAY_PORT, Data);   // I/O delay
    WRITE_IO8(SW_SMI_IO_ADDRESS, Command); // This triggers SMI

    Data = READ_IO8(SW_SMI_IO_DATA_ADDRESS);   // I/O delay is necessary
    WRITE_IO8(IO_DELAY_PORT, Data);   // I/O delay is necessary
    Data = READ_IO8(SW_SMI_IO_DATA_ADDRESS);   // I/O delay is necessary
    WRITE_IO8(IO_DELAY_PORT, Data);   // I/O delay is necessary
Porting End */

}

/**
    This function generates a software SMI by writing the provided
    byte value into the software SMI generation register

    @param This - Pointer to the SMM control II protocol
    @param CommandPort - The value written to the command port. 
    @param DataPort - The value written to the data port. 
    @param Periodic - Boolean indicating the nature of generation
           TRUE - means periodic generation depending on timing 
                  value provided in the next variable
           CURRENTLY NOT SUPPORTED. EXPECTS FALSE
    @param ActivationInterval - Optional. NOT SUPPORTED

    @retval EFI_STATUS
            EFI_SUCCESS S/W SMI triggered successfully
            EFI_INVALID_PARAMETER If Periodic is TRUE or when
                                  (ArgumentBuffer is not NULL and 
                                  ArgumentBufferSize is not 1)

    @note  No Porting Required
**/
EFI_STATUS SbSmmTriggerSmi2 (
    IN CONST EFI_SMM_CONTROL2_PROTOCOL *This,
    IN OUT UINT8                       *CommandPort OPTIONAL,
    IN OUT UINT8                       *DataPort OPTIONAL,
    IN BOOLEAN                         Periodic OPTIONAL,
    IN UINTN                           ActivationInterval OPTIONAL )
{
    if (Periodic) return EFI_INVALID_PARAMETER;

    GenerateSwSmi( *CommandPort , *DataPort );

    return EFI_SUCCESS;
}

/**
    This function clears software SMI status registers and
    generates End-of-SMI (EOS).

    @param This - Pointer to the SMM control II protocol
    @param Periodic - Boolean indicating the nature of clearing,
           TRUE means periodic SMI clearing.
           CURRENTLY NOT SUPPORTED. EXPECTS FALSE

    @retval EFI_STATUS
            EFI_SUCCESS SMI status successfully cleared
            EFI_INVALID_PARAMETER If Periodic is TRUE

    @note  No Porting Required
**/
EFI_STATUS SbSmmClearSmi2 (
    IN CONST EFI_SMM_CONTROL2_PROTOCOL  *This,
    IN BOOLEAN                          Periodic OPTIONAL )
{
    if (Periodic) return EFI_INVALID_PARAMETER;
    
/* Porting Required.  Include code to clear all SMI status
    // Porting Required.  Include code to clear software SMI status only
    WRITE_IO32_PM(ACPI_IOREG_SMI_STS, 0x20); // 0x34

    // Set EOS
    SET_IO8_PM(ACPI_IOREG_SMI_EN, 0x02); // 0x30
    if ((READ_IO8_PM(ACPI_IOREG_SMI_EN) & 0x02) == 0) {
        // Reset GBL_SMI_EN 
        RESET_IO8_PM(ACPI_IOREG_SMI_EN, 0x01); // 0x30
        // Set EOS Again
        SET_IO8_PM(ACPI_IOREG_SMI_EN, 0x02); // 0x30
        // Set GBL_SMI_EN 
        SET_IO8_PM(ACPI_IOREG_SMI_EN, 0x01); // 0x30
    }
    
Porting end */

    return EFI_SUCCESS;
}

#endif

/**
    This function is invoked from SB DXE to initialize SMM
    related stuff in NorthBridge and install appropriate
    SMM protocols such as SMM Access & SMM Control

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS SbSmmInit(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
#if defined SB_SMM_CONTROL2_PROTOCOL_SUPPORT && SB_SMM_CONTROL2_PROTOCOL_SUPPORT == 1
    UINT8   Value;
    UINT8   Mask;
#endif
    EFI_STATUS  Status = EFI_SUCCESS;

    //Report Progress code
    PROGRESS_CODE (DXE_SB_SMM_INIT);

    Status = pBS->LocateProtocol( &gEfiSmmAccess2ProtocolGuid, \
                                  NULL, \
                                  &gSmmAccess2 );
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->LocateProtocol( &gEfiS3SaveStateProtocolGuid,
                                  NULL,
                                  &gBootScript );
    if (EFI_ERROR(Status)) 
        return Status;

#if defined SB_SMM_CONTROL2_PROTOCOL_SUPPORT && SB_SMM_CONTROL2_PROTOCOL_SUPPORT == 1
    // Enable S/W SMI Generation
    SbSmmEnableSwSmi ();

    Value = 1; Mask = 0xFF;
/** Save in boot script for S3 wakeup
    BootScriptIoReadWriteMacro(gBootScriptSave,
                        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                        EfiBootScriptWidthUint8,
                        PM_BASE_ADDRESS + AMD_8111_PM_GLBL_CTRL,
                        &Value,&Mask);
 **/


    return pBS->InstallMultipleProtocolInterfaces(
                        &ImageHandle,
                        &gEfiSmmControl2ProtocolGuid,
                        &gEfiSmmControl2Protocol,
                        NULL,
                        NULL);
#endif

    return Status;
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

