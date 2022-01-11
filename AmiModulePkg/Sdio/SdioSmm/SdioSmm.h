//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SdioSmm.h
    Header file for the SdioSmm

**/

#ifndef _EFI_SDIO_SMM_DRIVER_H_
#define _EFI_SDIO_SMM_DRIVER_H_

//----------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#if PI_SPECIFICATION_VERSION >= 0x1000A
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#define RETURN(status) {return status;}
#else
#include <Protocol/SmmBase.h>
#include <Protocol/SmmSwDispatch.h>
#define RETURN(status) {return ;}
#endif
#include "SdioDef.h"

//----------------------------------------------------------------------

//-----------------------------------------------
//      ERROR CODE REPORTED TO CALLER
//-----------------------------------------------
#define SDIO_WRITE_PROTECT_ERR          0x003   // Write protect error
#define SDIO_TIME_OUT_ERR               0x080   // Command timed out error
#define SDIO_DRIVE_NOT_READY_ERR        0x0AA   // Drive not ready error
#define SDIO_DATA_CORRECTED_ERR         0x011   // Data corrected error
#define SDIO_PARAMETER_FAILED           0x007   // Bad parameter error
#define SDIO_MARK_NOT_FOUND_ERR         0x002   // Address mark not found error
#define SDIO_NO_MEDIA_ERR               0x031   // No media in drive
#define SDIO_READ_ERR                   0x004   // Read error
#define SDIO_UNCORRECTABLE_ERR          0x010   // Uncorrectable data error
#define SDIO_BAD_SECTOR_ERR             0x00A   // Bad sector error
#define SDIO_GENERAL_FAILURE            0x020   // Controller general failure

#define MAX_NUMBLOCK_16BIT              0xFFFF


#if PI_SPECIFICATION_VERSION >= 0x1000A
EFI_STATUS
EFIAPI
SdioSWSMIHandler (
    EFI_HANDLE      DispatchHandle,
    IN  CONST VOID  *DispatchContext OPTIONAL,
    IN  OUT VOID    *CommBuffer      OPTIONAL,
    IN  OUT UINTN   *CommBufferSize  OPTIONAL
);
#else
VOID
EFIAPI
SdioSWSMIHandler (
    EFI_HANDLE                  DispatchHandle,
    EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
);
#endif


VOID
SdMmcAPIRead (
    SDIO_STRUC  *SdioURP
);

VOID
SdMmcAPIWrite (
    SDIO_STRUC  *SdioURP
);

UINT8
GetDevEntry (
    IN  UINT8  DeviceAddress
);

VOID
SdMmcApiReset (
    SDIO_STRUC  *SdioURP
);

VOID
SdMmcApiDeviceGeometry (
    SDIO_STRUC  *SdioURP
);

VOID
SdMmcNotSupported (
    SDIO_STRUC  *SdioURP
);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
