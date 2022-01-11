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

/** @file NbDxeBoard.c
    This file contains DXE stage board component code for
    Template NB

*/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AmiMemInfo.h>
#include <Library/UefiLib.h>
#include <Library/SmbusLib.h>


#if SMBIOS_SUPPORT
#include <Protocol/SmbiosDynamicData.h>
#endif
#include <Nb.h>

// GUID Definitions
#define EFI_MEMORY_CONFIG_DATA_GUID \
    {0x80dbd530, 0xb74c, 0x4f11, 0x8c, 0x03, 0x41, 0x86, 0x65, 0x53, 0x28, 0x31}

// Constants
#define             MAX_BR      2
#define             MAX_CH      2
#define             MAX_RANK    4

// Host enumerated modes
#define             BR_SEQ      0
#define             BR_INTER    1
#define             BR_MIRROR   2
#define             SINGLE_CH   3

typedef struct {
//  UINT8               Error;
    UINT8           InterleaveDepth;
    UINT16          TotalWidth;
    UINT16          Size;
} DIMM_DATA;
typedef struct {
//  UINT8               Error;
    UINT8           Mode;
    UINT8           NumBranches;
    UINT8           NumChannels;
    UINT16          Frequency;
    UINT16          TypeDetail;
    DIMM_DATA       Dimm[MAX_BR][MAX_RANK];
} MEMINIT_CONFIG_DATA;
typedef struct {
    EFI_HOB_GUID_TYPE       EfiHobGuidType;
    MEMINIT_CONFIG_DATA     MemConfigData;
} MEM_CONFIG_HOB;

// Produced Protocols
EFI_GUID    EfiMemoryConfigDataGuid  = EFI_MEMORY_CONFIG_DATA_GUID;

// Portable Constants

// Function Prototypes

// PPI interface definition

// Protocols that are installed

// Function Definition

/**
    This function installs AmiMemoryInfo Protocol

    @param  Event   Pointer to this event
    @param  Context Event Handler private data 
    
    @retval None
**/
VOID
EFIAPI
InstallAmiMemoryInfoProtocol (
    IN EFI_EVENT  Event,
    IN VOID       *Context
  )
{
    /**** PORTING REQUIRED ****
    EFI_STATUS                  Status;
    EFI_SMBUS_HC_PROTOCOL       *Smbus = NULL;
    UINT8                       SpdAddr[] = {0xA0, 0xA2};
    EFI_SMBUS_DEVICE_ADDRESS    SmbusDeviceAddress;
    UINT8                       Offset = 7;
    UINTN                       Length = 1;
    UINT8                       Data8;
    AMI_MEMORY_INFO             AmiMemoryInfoStruct;
    UINTN                       MemoryArrayIndex;
    UINTN                       SlotIndex;
    EFI_HANDLE                  Handle = 0;
    
    Status = pBS->LocateProtocol (
                    &gEfiSmbusHcProtocolGuid,
                    NULL,
                    &Smbus
                    );
    if (EFI_ERROR (Status)) {
      return ;
    }
    
    for (MemoryArrayIndex = 0; MemoryArrayIndex < MEMORY_ARRAY_NUM; MemoryArrayIndex++) {
      AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].RamType     = DDR3;
      AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].MaxCapacity = 0x2000; // 8GB. Unit: MB.
      AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Speed       = 1066;   // Unit: Mhz
      AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].TotalMemory = 0;      // Initial value should be 0
      
      for (SlotIndex = 0; SlotIndex < DIMM_SLOT_NUM; SlotIndex++) {
        AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Slot[SlotIndex].Size = 0; // Unit: MB.
        AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].TotalMemory += \
            AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Slot[SlotIndex].Size; // Unit: MB.
        AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Slot[SlotIndex].Ecc = FALSE;
        AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Slot[SlotIndex].SpdAddr = SpdAddr[MemoryArrayIndex*DIMM_SLOT_NUM + SlotIndex];
        SmbusDeviceAddress.SmbusDeviceAddress = SMBUS_LIB_SLAVE_ADDRESS (SpdAddr[MemoryArrayIndex*DIMM_SLOT_NUM + SlotIndex]);
        //
        // If the value of Bits 5 ~ 3 of SPD BYTE 7 is greater than 000,
        // then set DoubleSide to "TRUE"
        //
        Smbus->Execute(
                Smbus,
                SmbusDeviceAddress,
                Offset,
                EfiSmbusReadByte,
                FALSE,
                &Length,
                &Data8
                );
        if ((Data8 & (UINT8) (BIT5 | BIT4 | BIT3)) != 0) {
          AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Slot[SlotIndex].DoubleSide = TRUE;
        } else {
          AmiMemoryInfoStruct.MemoryArray[MemoryArrayIndex].Slot[SlotIndex].DoubleSide = FALSE;
        }
      }
    }
    
    Status = pBS->InstallMultipleProtocolInterfaces(
                    &Handle,
                    &gAmiMemoryInfoGuid,
                    &AmiMemoryInfoStruct,
                    NULL
                    );

    TRACE((TRACE_ALWAYS, "Install gAmiMemoryInfoGuid  status = %x\n", Status));
    **** PORTING REQUIRED ****/
}

/**
    This function initializes the board specific component in
    in the chipset north bridge

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS 
NbDxeBoardInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS          Status = EFI_SUCCESS;
    VOID                *Registration;

    EfiCreateProtocolNotifyEvent (
      &gEfiSmbusHcProtocolGuid,
      TPL_CALLBACK,
      InstallAmiMemoryInfoProtocol,
      NULL,
      &Registration
      );

    return Status;
}

#if SMBIOS_SUPPORT
/**
                     ***** PORTING REQUIRED *****
               (if SMBIOS module is part of the project)

    This function gathers the System Memory information and
    saves them in a variable named "SmbiosMemVar". This variable
    with the slot information is needed by the SMBIOS module to
    create the "Type 16, 17, 18, 19" structure.

    @param SystemTable Pointer to the system table

    @retval Creates variable named "SmbiosMemVar" with System Memory
        information

    @note  Refer to SYSTEM_MEM_ARRAY_DYNAMIC_DATA in SmbiosDynamicData.h for
       structure information.

**/
VOID CreateMemoryDataForSMBios(
    IN EFI_SYSTEM_TABLE   *SystemTable 
)
{
  /*
  //
  //  Sample code for Blackford CRB. Needs to be changed for other platforms.
  //
  EFI_STATUS                      Status;
  MEM_CONFIG_HOB                  *SmbiosMemConfigHob;
  SYSTEM_MEM_ARRAY_DYNAMIC_DATA   Memory;
  UINTN                           VarSize;
  UINT32                          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
  UINT8                           PMAIndex, MemDevIndex;
  UINT32                          Type16StartingAddr = 0;
  UINT32                          Type20StartingAddr = 0;

// Parse HOBs to get SmbiosMemConfig HOB.

// Find first HOB
    SmbiosMemConfigHob =  (MEM_CONFIG_HOB*)GetEfiConfigurationTable(SystemTable, &gEfiHobListGuid);
    if (!SmbiosMemConfigHob) {
        TRACE((TRACE_ALWAYS, "NBDXE.Entry()Error:Unable to find SMBIOS Memory Configuration HOB\n"));
        return;
    }

    Status = FindNextHobByGuid(&EfiMemoryConfigDataGuid, (VOID**)&SmbiosMemConfigHob);
    if (EFI_ERROR(Status)) return;

  for (PMAIndex = 0; PMAIndex < NO_OF_PHYSICAL_MEMORY_ARRAY; PMAIndex++) {
    // Type 16
        // Do the next IF if MaxCapacity is known, else set MaxCapacity to 0x80000000
        if (SmbiosMemConfigHob->MemConfigData.Mode == BR_MIRROR) {
        Memory.PhysicalMemArray[PMAIndex].MaxCapacity = (0x800000000 >> 10);        // 32 GB if mode = mirror
        }
        else {
        Memory.PhysicalMemArray[PMAIndex].MaxCapacity = (0x1000000000 >> 10);       // 64GB
        }
    Memory.PhysicalMemArray[PMAIndex].MemErrInfoHandle = 0xFFFF;        // 0xFFFE if not supported
                                                                                                                                        // 0xFFFF if supported but no error

    #if MEMORY_ERROR_INFO
      // Type 18
      Memory.PhysicalMemArray[PMAIndex].ArrayMemoryError.ErrorType = 3;
      Memory.PhysicalMemArray[PMAIndex].ArrayMemoryError.ErrorGranularity = 2;
      Memory.PhysicalMemArray[PMAIndex].ArrayMemoryError.ErrorOperation = 2;
      Memory.PhysicalMemArray[PMAIndex].ArrayMemoryError.MemArrayErrorAddress = 0x80000000;
      Memory.PhysicalMemArray[PMAIndex].ArrayMemoryError.DeviceErrorAddress = 0x80000000;
      Memory.PhysicalMemArray[PMAIndex].ArrayMemoryError.ErrorResolution = 0x80000000;
    #endif

        Type20StartingAddr = Type16StartingAddr;
    for (MemDevIndex = 0; MemDevIndex < NUMBER_OF_MEM_MODULE; MemDevIndex++) {
        // Type 17
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.TotalWidth = \
                SmbiosMemConfigHob->MemConfigData.Dimm[PMAIndex][MemDevIndex].TotalWidth;
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.Size = \
                SmbiosMemConfigHob->MemConfigData.Dimm[PMAIndex][MemDevIndex].Size;
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.DeviceSet = \
                ((MemDevIndex / 2) + (MemDevIndex % 2));
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.Speed = \
                SmbiosMemConfigHob->MemConfigData.Frequency;
#if defined AMI_SMBIOS_MODULE_VERSION && AMI_SMBIOS_MODULE_VERSION >= 100
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.TypeDetail = \
                SmbiosMemConfigHob->MemConfigData.TypeDetail;
#endif

      #if MEMORY_ERROR_INFO
        // Type 18
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type18.ErrorType = 3;
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type18.ErrorGranularity = 2;
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type18.ErrorOperation = 2;
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type18.MemArrayErrorAddress = 0x80000000;
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type18.DeviceErrorAddress = 0x80000000;
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type18.ErrorResolution = 0x80000000;
      #endif

      // Type 20
      if (Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.Size == 0) {
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.StartingAddress = 0;
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.EndingAddress = 0;
      }
      else {
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.StartingAddress = Type20StartingAddr;
                Type20StartingAddr += (Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type17.Size << 10);  // Size in MB, convert to KB
        Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.EndingAddress = Type20StartingAddr - 1;
      }
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.PartitionRowPosition = \
                (MemDevIndex % 2);
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.InterleavePosition = 0xFF;
      Memory.PhysicalMemArray[PMAIndex].MemoryDeviceData[MemDevIndex].Type20.InterleaveDataDepth = \
                SmbiosMemConfigHob->MemConfigData.Dimm[PMAIndex][MemDevIndex].InterleaveDepth;
    }

    // Type 19
        if (Type16StartingAddr == Type20StartingAddr) {
        Memory.PhysicalMemArray[PMAIndex].MemArrayMapAddr.StartingAddress = 0;
        Memory.PhysicalMemArray[PMAIndex].MemArrayMapAddr.EndingAddress = 0;
        }
        else {
        Memory.PhysicalMemArray[PMAIndex].MemArrayMapAddr.StartingAddress = Type16StartingAddr;
            Type16StartingAddr = Type20StartingAddr;
      Memory.PhysicalMemArray[PMAIndex].MemArrayMapAddr.EndingAddress = Type16StartingAddr - 1;
        }
    Memory.PhysicalMemArray[PMAIndex].MemArrayMapAddr.PartitionWidth = 2;
  }
  VarSize = sizeof(SYSTEM_MEM_ARRAY_DYNAMIC_DATA);
  Status = pRS->SetVariable(SmbiosMemVar, &gAmiSmbiosDynamicDataGuid,
             Attributes, VarSize, &Memory);
  */
}
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
