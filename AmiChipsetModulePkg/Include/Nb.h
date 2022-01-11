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

/** @file Nb.h
    This file contains North Bridge or Host Bridge specific structures 
    and macro definitions.

*/

#ifndef  _NB_H   //To Avoid this header get compiled twice
#define  _NB_H


// DO NOT REMOVE THE DEFINITION OF THIS STRUCTURE. THIS IS USED BY CSM ALSO

/**
  Fields:  Type    ParameterName   Description
  UINT32  rbUID Root bridge unique ID
  UINT8   rbDevFunc Corresponding Root Bridge device/function number

  This data structure contains the information that is used to 
  map Root bridge UID to corresponding device/function number

*/
typedef struct ROOT_BRIDGE_MAPPING_TABLE {
    UINT32  rbUID;
    UINT8   rbDevFunc;
} ROOT_BRIDGE_MAPPING_ENTRY;

#ifndef BIT0
#define BIT0                0x01
#define BIT1                0x02
#define BIT2                0x04
#define BIT3                0x08
#define BIT4                0x10
#define BIT5                0x20
#define BIT6                0x40
#define BIT7                0x80
#define BIT8                0x100
#define BIT9                0x200
#endif

#define NB_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
    ((VOID*)(UINTN)(PcdGet64 (PcdPciExpressBaseAddress) + ((UINT8)(bus & 0xff) << 20) + ((UINT8)(dev & 0x1f) << 15) + ((UINT8)(func & 0x07) << 12) + (reg & 0xfff)))

#define NB_EXTENDED_REGISTER(extreg)    (((UINT64)extreg) << 32)

//
// North bridge Device Function and Register Definitions
//
#define NB_BUS                  0x00    // North Bridge Bus Number
#define NB_DEV                  0x00    // North Bridge Device Number
#define NB_FUN                  0x00    // North Bridge Function Number

// Define all the North bridge specific equates and structures in this file
#define NB_BUS_DEV_FUN          EFI_PCI_ADDRESS(0, 0, 0, 0)


EFI_STATUS 
NbDxeBoardInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable 
);

#endif 

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
