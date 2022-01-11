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

/** @file NbDxe.c
    This file contains code for Template North bridge initialization
    in the DXE stage

*/

// Module specific Includes
#include <Efi.h>
#include <Token.h>
#include <DXE.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Nb.h>


// Build flag adjustments
#ifndef     SMM_SUPPORT
#define     SMM_SUPPORT         0
#endif

#if SMBIOS_SUPPORT
extern  VOID    CreateMemoryDataForSMBios(IN EFI_SYSTEM_TABLE   *SystemTable);
#endif

// Produced Protocols

// GUID Definitions

// Portable Constants

// Function Prototypes
#if SMM_SUPPORT
EFI_STATUS
NbSmmInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable);
#endif

// Function Definition

/**
    This function is the entry point for this DXE. This function
    initializes the chipset NB before PCI Bus enumeration.

    @param ImageHandle - Image handle
    @param SystemTable - Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.
**/
EFI_STATUS
NbDxeInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINT64                              PcieBaseAddr;
    DXE_SERVICES                        *DxeSvcTbl;
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR     GcdDescriptor;

    InitAmiLib(ImageHandle, SystemTable);

    //Report Progress code
    PROGRESS_CODE (DXE_NB_INIT);

#if SMM_SUPPORT
    Status = NbSmmInit (ImageHandle, SystemTable);

    if (Status != EFI_SUCCESS) {
        TRACE ((TRACE_ALWAYS, "NbSmmInit() = %r\n", Status));
        goto Init_Error;
    }
#endif

    PcieBaseAddr = (UINT64)PcdGet64(PcdPciExpressBaseAddress);
    
    Status = LibGetDxeSvcTbl(&DxeSvcTbl);
    TRACE ((-1, "LibGetDxeSvcTbl(&DxeSvcTbl)%r.\n", Status));
    
    if (!EFI_ERROR (Status)) {
      Status = DxeSvcTbl->AllocateMemorySpace (
                            EfiGcdAllocateAddress,
                            EfiGcdMemoryTypeMemoryMappedIo,
                            0,
                            PCIEX_LENGTH,
                            &PcieBaseAddr,
                            ImageHandle,
                            NULL
                            );
      
      if (EFI_ERROR (Status)) {
        TRACE ((-1, "Fail to Allocate Memory Space for PciExpressBaseAddress.\n"));
      } else {
        Status = DxeSvcTbl->GetMemorySpaceDescriptor (PcieBaseAddr, &GcdDescriptor);
        if (EFI_ERROR (Status)) {
          TRACE ((-1, "Fail to get PCIEx GcdDescriptor at %x.\n", PcieBaseAddr));
        } else {
          TRACE ((-1, " GcdDescriptor.Attributes%x.\n", GcdDescriptor.Attributes));
          Status = DxeSvcTbl->SetMemorySpaceAttributes (
                                PcieBaseAddr,
                                PCIEX_LENGTH,
                                GcdDescriptor.Attributes | EFI_MEMORY_RUNTIME | EFI_MEMORY_UC
                                );
          if (EFI_ERROR (Status)) {
            TRACE ((-1, "Unable to add EFI_MEMORY_RUNTIME attribute to PCIEx region %r.\n", Status));
          }
        }
      }
    }

    Status = NbDxeBoardInit(ImageHandle, SystemTable);

#if SMBIOS_SUPPORT
    CreateMemoryDataForSMBios(SystemTable);
#endif

Init_Error:
    if (Status != EFI_SUCCESS) {
      //Report Error code
      ERROR_CODE (DXE_NB_ERROR, EFI_ERROR_MAJOR);
    }
    return Status;
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
