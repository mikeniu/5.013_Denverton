//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/VirtualSerialDevice/VirtualSerial.h 2     4/13/10 12:36a Rameshr $
//
// $Revision: 2 $
//
// $Date: 4/13/10 12:36a $
//**********************************************************************

//<AMI_FHDR_START>
//--------------------------------------------------------------------------
//
// Name: 	 VirtualSerial.h	
//
// Description: Header file for the Virtual Serial Device.
//
//--------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _EFI_VIRTUAL_SERIAL_H_
#define _EFI_VIRTUAL_SERIAL_H_

#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol\AmiSio.h>
#include <Protocol\SuperIo.h>
#include <AcpiRes.h>

typedef struct {
    UINT16      BaseAddress;
    UINT8       Irq;
    UINT32      Uid;
} VIRTUAL_SERIAL_DETAILS;

typedef struct _VIRTUAL_DEV_DATA {
    AMI_SIO_PROTOCOL    AmiSio;
    EFI_SIO_PROTOCOL    EfiSio;
    UINT16              DevBase;
    UINT8               DevIrq;
    UINT8               DevDma;
} VIRTUAL_DEV_DATA;

//
// AMI SIO protocol API
//
EFI_STATUS 
EFIAPI
VirtualSerialRegister(
    IN AMI_SIO_PROTOCOL *This,
    IN BOOLEAN          Write,
    IN BOOLEAN          ExitCfgMode,
    IN UINT8           	Register,
    IN OUT UINT8       	*Value
);

EFI_STATUS 
EFIAPI
VirtualSerialCRS(
    IN AMI_SIO_PROTOCOL *This,
    IN BOOLEAN          Set,
    IN OUT T_ITEM_LIST  **Resources
);

EFI_STATUS 
EFIAPI
VirtualSerialPRS(
    IN AMI_SIO_PROTOCOL     *This,
    IN BOOLEAN              Set,
    IN OUT T_ITEM_LIST      **Resources
);

//
// EFI SIO protocol API
//
EFI_STATUS
EFIAPI 
VirtualEfiSioRegisterAccess(
  IN   CONST  EFI_SIO_PROTOCOL  *This,
  IN          BOOLEAN           Write,
  IN          BOOLEAN           ExitCfgMode,
  IN          UINT8             Register,
  IN OUT      UINT8             *Value);


EFI_STATUS
EFIAPI 
VirtualEfiSioGetResources( 
  IN  CONST EFI_SIO_PROTOCOL            *This,
  OUT       ACPI_RESOURCE_HEADER_PTR    *ResourceList
);

EFI_STATUS 
EFIAPI
VirtualEfiSioSetResources(
  IN CONST  EFI_SIO_PROTOCOL        *This,
  IN        ACPI_RESOURCE_HEADER_PTR ResourceList
);

EFI_STATUS 
EFIAPI
VirtualEfiSioPossibleResources(
  IN  CONST EFI_SIO_PROTOCOL         *This,
  OUT       ACPI_RESOURCE_HEADER_PTR *ResourceCollection
);

EFI_STATUS 
EFIAPI
VirtualEfiSioModify(
  IN CONST EFI_SIO_PROTOCOL         *This,
  IN CONST EFI_SIO_REGISTER_MODIFY  *Command,
  IN       UINTN                    NumberOfCommands
);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
