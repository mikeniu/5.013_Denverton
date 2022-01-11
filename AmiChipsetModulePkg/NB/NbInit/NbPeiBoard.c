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

/** @file NbPeiBoard.c
    This file contains PEI stage board component code for
    Template NB

*/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include <Nb.h>
#include <Library/NBCspLib.h>
#include <PPI/CspLibPpi.h>

// Produced PPIs

// GUID Definitions

// Portable Constants

// Function Prototypes

// PPI interface definition
AMI_PEI_PCI_INIT_TABLE_STRUCT stNBH2P_PCIInitTable [] = {
    { 0x00, 0xFF, 0x00 },
    { 0xFF, 0xFF, 0xFF }
//  { Register, AND Mask, OR Mask },
//  { Register, AND Mask, OR Mask}
};
UINT16      wNBH2P_PCIInitTableSize = sizeof(stNBH2P_PCIInitTable)/sizeof(AMI_PEI_PCI_INIT_TABLE_STRUCT);

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
