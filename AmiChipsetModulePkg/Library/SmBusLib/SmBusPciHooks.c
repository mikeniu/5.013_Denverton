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

/** @file SmBusPciHooks.c
    This file contains PCI initialized hooks for SMBus porting.

    @note  Porting required if SMBus early DXE supported

*/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <PciBus.h>

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

// Local variable

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    This function is called by PCI Bus Driver before configuring
    or disabling any PCI device. This function should examine the
    Vendor/Device ID or PCI Bus, Device and Function numbers to
    make sure it is not a south bridge device or any other device
    which should no be configured by PCI Bus Driver.

    @param PciDevice - Pointer to PCI Device Info structure.
        
    @retval EFI_SUCCESS - SKIP this device, do not touch
                          PCI Command register.
    @retval EFI_UNSUPPORTED - DON'T SKIP this device do complete
                              enumeration as usual.
**/
EFI_STATUS SmBusProtectedPciDevice (
    IN PCI_DEV_INFO             *PciDevice )
{
/* 
if ((PciDevice->Address.Addr.Bus == SMBUS_BUS) && \
    (PciDevice->Address.Addr.Device == SMBUS_DEV) && \
    (PciDevice->Address.Addr.Function == SMBUS_FUN)) {

    return EFI_SUCCESS;
}
*/
    return EFI_UNSUPPORTED;
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
