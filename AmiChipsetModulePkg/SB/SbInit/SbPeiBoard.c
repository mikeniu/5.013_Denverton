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

/** @file SbPeiBoard.c
    This file contains PEI stage board component code for
    Template SB

*/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiLib.h>
#include <Sb.h>
#include <Ppi/CspLibPpi.h>

// Produced PPIs

// GUID Definitions

// Portable Constants

// Function Prototypes

// PPI interface definition
AMI_PEI_PCI_INIT_TABLE_STRUCT stSBH2P_PCIInitTable [] = {
//  { Register, AND Mask, OR Mask },
    {0x00, 0x00, 0x00 }
//  { Register, AND Mask, OR Mask}
};
UINT16  wSBH2P_PCIInitTableSize = sizeof(stSBH2P_PCIInitTable)/sizeof(AMI_PEI_PCI_INIT_TABLE_STRUCT);

// Function Definition


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

