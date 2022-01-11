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

/** @file NbSmm.c
    This file contains code for SMM access II protocols.

*/

// Module specific Includes
#include "Efi.h"
#include "token.h"
#include <HOB.h>
#include <DXE.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

// Used Protocols
#include <Protocol/PciRootBridgeIo.h>

// Produced Protocols
#include <Protocol/SmmAccess2.h>


// GUID Definitions

// Global variable declarations
static  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;
EFI_S3_SAVE_STATE_PROTOCOL              *gBootScriptSave;
EFI_HOB_HANDOFF_INFO_TABLE              *pHIT;

#if defined NB_SMM_ACCESS2_PROTOCOL_SUPPORT && NB_SMM_ACCESS2_PROTOCOL_SUPPORT == 1
EFI_SMRAM_DESCRIPTOR gSmramMap[] = {
    {
    0xA0000,                            //PhysicalStart
    0xA0000,                            //CpuStart
    128*1024,                           //PhysicalSize
    EFI_ALLOCATED | EFI_SMRAM_CLOSED    //RegionState   <--Is this the correct initial state?
    },
    {
    0,                                  //PhysicalStart <--To be filled in.
    0,                                  //CputStart
    TSEG_SIZE,                          //PhysicalSize
    EFI_ALLOCATED | EFI_SMRAM_CLOSED    //RegionState   <--Is this the correct initial state?
    }
};

#define SMRAM_MAP_NUM_DESCRIPTORS (sizeof(gSmramMap)/sizeof(EFI_SMRAM_DESCRIPTOR))

#endif

// Portable Constants

// Function Prototypes

// Function Definition

#if defined NB_SMM_ACCESS2_PROTOCOL_SUPPORT && NB_SMM_ACCESS2_PROTOCOL_SUPPORT == 1
/**
    This function programs the NB chipset registers to enable
    appropriate SMRAM area.  

    @param VOID

    @retval EFI_SUCCESS Always

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS
NbSmmEnableSmmAddress (VOID)
{
    UINT64          qTsegAddress;

    // Currently TSEG address is assumed to be TOM - TSEG_SIZE
    // THIS CODE SHOULD BE CHANGED TO GET THE TSEG LOCATION FROM CPU INFO HOB
    qTsegAddress = pHIT->EfiMemoryTop;
#if NB_DEBUG_MESSAGE
    TRACE((TRACE_ALWAYS, "TSEG Address %x.\n", qTsegAddress));
#endif
    gSmramMap[1].PhysicalStart  = gSmramMap[1].CpuStart = qTsegAddress;
    gSmramMap[1].PhysicalSize   = TSEG_SIZE;

// Porting Required - Enable SMM area in Chipset (both TSEG & 0xA0000)  

// Porting ENDS
    return EFI_SUCCESS;
}

/**
    This function programs the NB chipset registers to open
    the SMRAM area.

    @param DescriptorIndex - Index of SMM Descriptor

    @retval EFI_STATUS
            EFI_SUCCESS             Opened the SMM area.
            EFI_INVALID_PARAMETER   Descriptor doesn't exist.
            EFI_ACCESS_DENIED       SMM area locked

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
              Here is the control flow of this function:
                  1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                  2. Read NB SMM register.
                  3. If locked, return EFI_ACCESS_DENIED
                  4. Set Value for register to Open SMM area (0a0000-bffff)
                  5. Write Register.
                  6. Set OpenState to TRUE.
                  7. Set the RegionState to EFI_SMRAM_OPEN.
                  8. Return EFI_SUCCESS.
**/
EFI_STATUS NbSmmCommonOpenSmram (
    IN UINTN                        DescriptorIndex )
{
    if (DescriptorIndex >= SMRAM_MAP_NUM_DESCRIPTORS) 
        return EFI_INVALID_PARAMETER;

    if (gSmramMap[0].RegionState == EFI_SMRAM_LOCKED ) 
            /* Write code to check SMM lock */
        return EFI_ACCESS_DENIED;   //If device is locked, return error.

// Porting Required - Open SMM area in Chipset (both TSEG & 0xA0000)    

//#### SET_PCI8_NB(NB_REG_SMRAM, 0x08); // 0x9D
//#### RESET_PCI8_NB(NB_REG_SMRAM, 0x20); // 0x9D
//#### SET_PCI8_NB(NB_REG_SMRAM, 0x40); // 0x9D

// Porting ENDS

    // Update appropriate flags
    gSmramMap[0].RegionState = EFI_SMRAM_OPEN;
    gSmramMap[1].RegionState = EFI_SMRAM_OPEN;

    return EFI_SUCCESS;
}

/**
    This function programs the NB chipset registers to close
    the SMRAM area.

    @param DescriptorIndex     Index of SMM Descriptor

    @retval EFI_STATUS
            EFI_SUCCESS             Closed the SMM area.
            EFI_INVALID_PARAMETER   Descriptor doesn't exist.
            EFI_ACCESS_DENIED       SMM area locked

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
              Here is the control flow of this function:
                  1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                  2. Read NB SMM register.
                  3. If locked, return EFI_ACCESS_DENIED
                  4. Set Value for register to close SMM area (0a0000-bffff)
                  5. Write Register.
                  6. Set OpenState to FALSE.
                  7. Set the RegionState to EFI_SMRAM_CLOSED & EFI_ALLOCATED
                  8. Return EFI_SUCCESS.
**/
EFI_STATUS NbSmmCommonCloseSmram (
    IN UINTN                        DescriptorIndex )
{
    if (DescriptorIndex >= SMRAM_MAP_NUM_DESCRIPTORS) 
        return EFI_INVALID_PARAMETER;

    if (gSmramMap[0].RegionState == EFI_SMRAM_LOCKED ) 
            /* Write code to check SMM lock */
        return EFI_ACCESS_DENIED;   //If device is locked, return error.

// Porting Required - Close SMM area in Chipset (both TSEG & 0xA0000)   

//#### RESET_PCI8_NB(NB_REG_SMRAM, 0x40); // 0x9D

// Porting ENDS

    // Update appropriate flags
    gSmramMap[0].RegionState = EFI_SMRAM_CLOSED | EFI_ALLOCATED;
    gSmramMap[1].RegionState = EFI_SMRAM_CLOSED | EFI_ALLOCATED;

    return EFI_SUCCESS;
}

/**
    This function programs the NB chipset registers to lock
    the SMRAM area from opening/closing.  Only system level reset
    can unlock the SMRAM lock.

    @param DescriptorIndex     Index of SMM Descriptor

    @retval EFI_STATUS
            EFI_SUCCESS             Locked the SMM area.
            EFI_INVALID_PARAMETER   Descriptor doesn't exist.
            EFI_DEVICE_ERROR        SMM area is opened, need to be
                                    closed first before locking

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
              Here is the control flow of this function:
                  1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                  2. Read NB SMM register.
                  3. If opened, return EFI_ACCESS_DENIED
                  4. Set Value for register to lock SMM area (0a0000-bffff)
                  5. Write Register.
                  6. Set the RegionState to EFI_SMRAM_CLOSED & EFI_ALLOCATED
                  7. Return EFI_SUCCESS.
**/
EFI_STATUS NbSmmCommonLockSmram (
    IN UINTN                        DescriptorIndex )
{
    //Both regions must open and close at the same time.
    if (DescriptorIndex >= SMRAM_MAP_NUM_DESCRIPTORS) 
        return EFI_INVALID_PARAMETER;

    if (gSmramMap[0].RegionState == EFI_SMRAM_OPEN ) 
            /* Write code to check SMM lock */
        return EFI_ACCESS_DENIED;   //If SMM is opened, return error.

// Porting Required - Lock SMM area in Chipset (both TSEG & 0xA0000)    

//#### SET_PCI8_NB(NB_REG_SMRAM, 0x10); // 0x9D

// Porting ENDS

    // Update appropriate flags
    gSmramMap[0].RegionState = EFI_SMRAM_LOCKED;
    gSmramMap[1].RegionState = EFI_SMRAM_LOCKED;

    return EFI_SUCCESS;
}

/**
    This function returns the current SMRAM area map information
    such as number of regions and its start address and size

    @param SmramMapSize       Size of the SMRAM map buffer provided
    @param SmramMap           Buffer to copy the SMRAM map information

    @retval EFI_STATUS
            EFI_SUCCESS             SMRAM Map copied into buffer
            EFI_BUFFER_TOO_SMALL    Indicates that provided buffer is
                                    not enough                   
    @return SmramMapSize Filled with required/updated size
    @retval SmramMap Updated SMRAM map  

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
              Here is the control flow of this function:
                  1.If SMRAM Map Size less than the actual map size, set 
                    the map size and return EFI_BUFFER_TOO_SMALL.
                  2.Copy the SMRAM Map descriptors into the supplied buffer.
                  3.Set the map size in *SmramMapSize, just in case is
                    larger than the actual buffer.
                  4.Return EFI_SUCCESS.
**/
EFI_STATUS NbSmmCommonGetCapabilities (
    IN OUT UINTN                    *SmramMapSize,
    OUT EFI_SMRAM_DESCRIPTOR        *SmramMap )
{
    UINTN       i;

    if (*SmramMapSize < sizeof(gSmramMap)) { // If input map size to small
        *SmramMapSize = sizeof(gSmramMap);   // report the correct map size
        return EFI_BUFFER_TOO_SMALL;         // and return error. 
    }

    for (i = 0; i < SMRAM_MAP_NUM_DESCRIPTORS; ++i) 
        SmramMap[i] = gSmramMap[i];

    *SmramMapSize = sizeof(gSmramMap);      // Set the correct map size in
    return EFI_SUCCESS;                     // case too large.
}

/**
    This function programs the NB chipset registers to open
    the SMRAM area.

    @param This - Pointer to the SMM access II protocol

    @retval EFI_STATUS
            EFI_SUCCESS             Opened the SMM area.
            EFI_INVALID_PARAMETER   Descriptor doesn't exist.
            EFI_ACCESS_DENIED       SMM area locked

    @note  No Porting Required
**/
EFI_STATUS NbSmmOpenSmram2 (
    IN EFI_SMM_ACCESS2_PROTOCOL     *This )
{
    EFI_STATUS      Status;

    Status = NbSmmCommonOpenSmram( 0 );

    if (!EFI_ERROR(Status)) This->OpenState = TRUE; 

    return Status;
}

/**
    This function programs the NB chipset registers to close
    the SMRAM area.

    @param This - Pointer to the SMM access II protocol

    @retval EFI_STATUS
            EFI_SUCCESS             Closed the SMM area.
            EFI_INVALID_PARAMETER   Descriptor doesn't exist.
            EFI_ACCESS_DENIED       SMM area locked

    @note  No Porting Required
**/
EFI_STATUS NbSmmCloseSmram2 (
    IN EFI_SMM_ACCESS2_PROTOCOL     *This )
{
    EFI_STATUS      Status;

    Status = NbSmmCommonCloseSmram( 0 );

    if (!EFI_ERROR(Status)) This->OpenState = FALSE;

    return Status;
}

/**
    This function programs the NB chipset registers to lock
    the SMRAM area from opening/closing.  Only system level reset
    can unlock the SMRAM lock.

    @param This - Pointer to the SMM access II protocol

    @retval EFI_STATUS
            EFI_SUCCESS             Locked the SMM area.
            EFI_INVALID_PARAMETER   Descriptor doesn't exist.
            EFI_DEVICE_ERROR        SMM area is opened, need to be
                                    closed first before locking

    @note  No Porting Required
**/
EFI_STATUS NbSmmLockSmram2 (
    IN EFI_SMM_ACCESS2_PROTOCOL     *This )
{
    EFI_STATUS      Status;

    Status = NbSmmCommonLockSmram( 0 );

    if (!EFI_ERROR(Status)) This->LockState = TRUE;

    return Status;
}

/**
    This function returns the current SMRAM area map information
    such as number of regions and its start address and size

    @param This               Pointer to the SMM access II protocol
    @param SmramMapSize       Size of the SMRAM map buffer provided
    @param SmramMap           Buffer to copy the SMRAM map information

    @retval EFI_STATUS
            EFI_SUCCESS             SMRAM Map copied into buffer
            EFI_BUFFER_TOO_SMALL    Indicates that provided buffer is
                                    not enough
    @return SmramMapSize Filled with required/updated size
    @retval SmramMap Updated SMRAM map  
    
    @note  No Porting Required
**/
EFI_STATUS NbSmmGetCapabilities2 (
    IN CONST EFI_SMM_ACCESS2_PROTOCOL *This,
    IN OUT UINTN                      *SmramMapSize,
    OUT EFI_SMRAM_DESCRIPTOR          *SmramMap )
{
    return NbSmmCommonGetCapabilities( SmramMapSize, SmramMap );
}

EFI_SMM_ACCESS2_PROTOCOL gSmmAccess2 = {
    NbSmmOpenSmram2,
    NbSmmCloseSmram2,
    NbSmmLockSmram2,
    NbSmmGetCapabilities2,
    FALSE,
    FALSE
};

#endif

/**
    This function is invoked from NB DXE to initialize SMM
    related stuff in NorthBridge and install appropriate
    SMM protocols such as SMM Access & SMM Control

    @param ImageHandle  - Image handle
    @param SystemTable  - Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

**/
EFI_STATUS
NbSmmInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    //Report Progress code
    PROGRESS_CODE (DXE_NB_SMM_INIT);

    pHIT = GetEfiConfigurationTable(SystemTable, &gEfiHobListGuid);

    Status = pBS->LocateProtocol(
                        &gEfiPciRootBridgeIoProtocolGuid,
                        NULL,
                        &gPciRootBridgeIo);
    if (EFI_ERROR(Status)) 
        return Status;

    Status = pBS->LocateProtocol(
                        &gEfiS3SaveStateProtocolGuid,
                        NULL,
                        &gBootScriptSave);
    if (EFI_ERROR(Status)) 
        return Status;

#if defined NB_SMM_ACCESS2_PROTOCOL_SUPPORT && NB_SMM_ACCESS2_PROTOCOL_SUPPORT == 1
    // Enable SMM address range by programming appropriate chipset registers
    NbSmmEnableSmmAddress ();

    return pBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gEfiSmmAccess2ProtocolGuid,
                  &gSmmAccess2,
                  NULL,
                  NULL
                  );
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
