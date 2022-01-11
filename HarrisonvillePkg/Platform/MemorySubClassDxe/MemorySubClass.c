//
// This file contains an 'Intel Peripheral Driver' and is        
// licensed for Intel CPUs and chipsets under the terms of your  
// license agreement with Intel or your vendor.  This file may   
// be modified by the user, subject to additional terms of the   
// license agreement                                             
//
/** @file
  This is the driver that locates the MemoryConfigurationData Variable, if it
  exists, add the related SMBIOS tables by PI SMBIOS protocol.  

  Copyright (c) 2014 - 2016 Intel Corporation.  All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "MemorySubClass.h"
#include "Platform.h"
#include <Guid/PlatformInfo.h>
#include <Library/HobLib.h>
#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include "MmrcData.h"

// Denverton AptioV Override Start - EIP#244496
// Added more manufacture ids referring AMI`s smbios.c
MEMORY_MODULE_MANUFACTURE_LIST MemoryModuleManufactureList[] =
{
     { 0, 0x01, L"AMD" },
     { 0, 0x04, L"Fujitsu" },
     { 0, 0x07, L"Hitachi" },
     { 0, 0x89, L"Intel" },
     { 0, 0x10, L"NEC" },
     { 0, 0x97, L"Texas Instrument" },
     { 0, 0x98, L"Toshiba" },
     { 0, 0x1c, L"Mitsubishi" },
     { 0, 0x1f, L"Atmel" },
     { 0, 0x20, L"STMicroelectronics" },
     { 0, 0xa4, L"IBM" },
     { 0, 0x2c, L"Micron Technology" },
     { 0, 0xad, L"SK Hynix" },
     { 0, 0xb0, L"Sharp" },
     { 0, 0xb3, L"IDT" },
     { 0, 0x3e, L"Oracle" },
     { 0, 0xbf, L"SST" },
     { 0, 0x40, L"ProMos/Mosel" },
     { 0, 0xc1, L"Infineon" },
     { 0, 0xc2, L"Macronix" },
     { 0, 0x45, L"SanDisk" },
     { 0, 0xce, L"Samsung" },
     { 0, 0xda, L"Winbond" },
     { 0, 0xe0, L"LG Semi" },
     { 0, 0x62, L"Sanyo" },
     { 1, 0x98, L"Kingston" },
     { 1, 0xba, L"PNY" },
     { 1, 0x4f, L"Transcend" },
     { 1, 0x7a, L"Apacer" },
     { 1, 0xad, L"Hynix"},
     { 2, 0x9e, L"Corsair" },
     { 2, 0xfe, L"Elpida" },
     { 3, 0x0b, L"Nanya" },
     { 3, 0x94, L"Mushkin" },
     { 3, 0x25, L"Kingmax" },
     { 4, 0xb0, L"OCZ" },
     { 4, 0xcb, L"A-DATA" },
     { 4, 0xcd, L"G Skill" },
     { 4, 0xef, L"Team" },
     { 5, 0x02, L"Patriot" },
     { 5, 0x9b, L"Crucial" },
     { 5, 0x51, L"Qimonda" },
     { 5, 0x57, L"AENEON" },
     { 5, 0xf7, L"Avant" },
     { 6, 0x34, L"Super Talent" },
     { 6, 0xd3, L"Silicon Power" },
     {0xff, 0xff, L"Undefined"}
};
// Denverton AptioV Override End - EIP#244496

EFI_STRING_ID mDimmToDevLocator[] = {
    STRING_TOKEN(STR_MEMORY_SUBCLASS_DEVICE_LOCATOR_0), STRING_TOKEN(STR_MEMORY_SUBCLASS_DEVICE_LOCATOR_1),
    STRING_TOKEN(STR_MEMORY_SUBCLASS_DEVICE_LOCATOR_2), STRING_TOKEN(STR_MEMORY_SUBCLASS_DEVICE_LOCATOR_3),
    STRING_TOKEN(STR_MEMORY_SUBCLASS_DEVICE_LOCATOR_4), STRING_TOKEN(STR_MEMORY_SUBCLASS_DEVICE_LOCATOR_5)
};

EFI_STRING_ID mDimmToBankLocator[] = {
    STRING_TOKEN(STR_MEMORY_SUBCLASS_BANK_LOCATOR_0), STRING_TOKEN(STR_MEMORY_SUBCLASS_BANK_LOCATOR_1),
    STRING_TOKEN(STR_MEMORY_SUBCLASS_BANK_LOCATOR_2), STRING_TOKEN(STR_MEMORY_SUBCLASS_BANK_LOCATOR_3)
 };

EFI_GUID mEfiMemorySubClassDriverGuid = EFI_MEMORY_SUBCLASS_DRIVER_GUID;


EFI_STATUS
EFIAPI
MemorySubClassEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{

  EFI_HII_HANDLE                HiiHandle;
  EFI_STATUS                    Status;
  EFI_HOB_GUID_TYPE             *GuidHob;
  UINT8                         Dimm;
  BOOLEAN                       DimmPresent;
  BOOLEAN                       ChEnabled;
  EFI_SMBIOS_PROTOCOL           *Smbios;
  SMBIOS_TABLE_TYPE16           *Type16Record;
  SMBIOS_TABLE_TYPE17           *Type17Record;
  SMBIOS_TABLE_TYPE19           *Type19Record;
  SMBIOS_TABLE_TYPE20           *Type20Record;
  EFI_SMBIOS_HANDLE             MemArraySmbiosHandle;
  EFI_SMBIOS_HANDLE             MemArrayMappedAddrSmbiosHandle;
  EFI_SMBIOS_HANDLE             MemDevSmbiosHandle;
  EFI_SMBIOS_HANDLE             MemDevMappedAddrSmbiosHandle;
  UINTN                         TableSize;
  UINT8                         NumSlots = 0; //Denverton AptioV Override Start - EIP#282920
  UINT64                        TotalMemorySize;
  UINT16                        MemorySpeed;
  CHAR8                         *OptionalStrStart;
  UINTN                         DeviceLocatorStrLen;
  UINTN                         BankLocatorStrLen;
  UINTN                         ManufactureStrLen;
  UINTN                         SerialNumberStrLen;
  UINTN                         AssertTagStrLen;
  UINTN                         PartNumberStrLen;
  EFI_STRING_ID                 DeviceLocator;
  EFI_STRING_ID                 BankLocator;
  EFI_STRING_ID                 Manufacturer;
  EFI_STRING_ID                 SerialNumber;
  EFI_STRING_ID                 AssetTag;
  EFI_STRING_ID                 PartNumber;
  EFI_STRING                    DeviceLocatorStr;
  EFI_STRING                    BankLocatorStr;
  EFI_STRING                    ManufactureStr;
  EFI_STRING                    SerialNumberStr;
  EFI_STRING                    AssertTagStr;
  EFI_STRING                    PartNumberStr;
  UINT8                         FormFactor;
  PLATFORM_INFO_HOB             *PlatformInfoHob;
  UINTN                         StringBufferSize;
  EFI_STRING                    StringBuffer;
  UINT8                         Channel;
  UINT8                         Width=0;
  UINT32                        TotalMem;
  MRC_PARAMETERS	               *MrcParameters;
  VOID                          *HobList;
  UINT16                        Index;         //Denverton AptioV Override Start - EIP#283284
  EFI_STRING                    StringBuffer1;
  UINTN                         IdListIndex;
  UINT16                        MaxMemorySpeed;
  UINT8                         AssetTagString = 0;
  // Denverton AptioV Override Start - EIP#244496
  UINT32                        Type20StartingAddr = 0;
  UINT8                         SPDPartnum = 0;
  // Denverton AptioV Override Start - EIP#265102(Type 19 Partition Width fix)
  UINT8				            PartitionWidth = 0;
  UINT8				            Type;
  EFI_SMBIOS_TABLE_HEADER       *Record;
  EFI_SMBIOS_HANDLE             SmbiosHandle[2]  = {SMBIOS_HANDLE_PI_RESERVED, SMBIOS_HANDLE_PI_RESERVED};
  UINT8                         Node = 0;
  // Denverton AptioV Override End - EIP#265102(Type 19 Partition Width fix) 
  // Denverton AptioV Override End - EIP#244496
  UINT32                        SpdBufferIndex;
  UINT32                        SpdBufferIndex2;
  UINT8                         MemoryType;
  UINT8                         NominalVoltage;
  UINT8                         Attributes;
  UINT8                         DimmsPerCh = 0; //Denverton AptioV Override - EIP#312471
  
  //
  // Allocate Buffers
  //
  StringBufferSize = (sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH;
  StringBuffer = AllocateZeroPool (StringBufferSize);
  ASSERT (StringBuffer != NULL);
  StringBuffer1 = AllocateZeroPool (StringBufferSize);
  ASSERT (StringBuffer1 != NULL);
  // Denverton AptioV Override Start - EIP#343554
  if (StringBuffer1 == NULL){
	gBS->FreePool (StringBuffer);
    return EFI_OUT_OF_RESOURCES;
	}
  // Denverton AptioV Override End - EIP#343554
  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  HobList = GetHobList ();
  ASSERT (HobList != NULL);

  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob = GetNextGuidHob (&gEfiMemoryConfigDataGuid, HobList);
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, " GUID not found\n"));
	// Denverton AptioV Override Start - EIP#343554
	gBS->FreePool (StringBuffer);
	gBS->FreePool (StringBuffer1);
	// Denverton AptioV Override End - EIP#343554
    return EFI_NOT_FOUND;
  }

  MrcParameters = (MRC_PARAMETERS *) GET_GUID_HOB_DATA (GuidHob);
 
  
  //
  // Locate dependent protocols
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, &Smbios);
  ASSERT (!EFI_ERROR (Status));

  //
  // Add our default strings to the HII database. They will be modified later.
  //
  HiiHandle = HiiAddPackages (
                &mEfiMemorySubClassDriverGuid,
                NULL,
                MemorySubClassDxeStrings,
                NULL,
                NULL
                );

  ASSERT (!EFI_ERROR (Status));
  //Denverton AptioV Override Start - EIP#282920
  GuidHob = GetFirstGuidHob (&gPlatformInfoHobGuid);
  ASSERT (GuidHob != NULL);
	  if (GuidHob == NULL) {
	    DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
		// Denverton AptioV Override Start - EIP#343554
		gBS->FreePool (StringBuffer);
		gBS->FreePool (StringBuffer1);
		// Denverton AptioV Override End - EIP#343554
	    return EFI_NOT_READY;
	  }
  PlatformInfoHob = GET_GUID_HOB_DATA (GuidHob);
  ASSERT (PlatformInfoHob);
  //Denverton AptioV Override Start - EIP#312471 
  // Adding Dimms Per Channel info to generate Type 17 structures only if Dimm Slot is present 
  if(PlatformInfoHob->BoardId == BOARD_ID_CormorantLake)
  {
    NumSlots = 2;
    DimmsPerCh = 1;
  }
  else
  {       
    NumSlots = 4;
    DimmsPerCh = 2;
  }
  //Denverton AptioV Override End - EIP#312471
  
  //Denverton AptioV Override End - EIP#282920
  TotalMemorySize = 0;
  
  //
  // Convert Total Memory size from MB to Bytes
  //
  TotalMemorySize = (UINT64) LShiftU64 (MrcParameters->SystemMemorySize, 20);
  DEBUG ((EFI_D_INFO, "Total Memory Size = %dMB\n", MrcParameters->SystemMemorySize));

  //
  // Report top level Physical Memory Array to Type 16 SMBIOS Record
  //

  Type16Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE16) + 1 + 1);
  ASSERT(Type16Record != NULL); 
  // Denverton AptioV Override Start - EIP#343554
  if (Type16Record == NULL) {
    gBS->FreePool (StringBuffer);
	gBS->FreePool (StringBuffer1);
  return EFI_OUT_OF_RESOURCES;
  }
  // Denverton AptioV Override End - EIP#343554
  ZeroMem(Type16Record, sizeof (SMBIOS_TABLE_TYPE16) + 1 + 1);
  
  Type16Record->Hdr.Type = EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;
  Type16Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE16);
  Type16Record->Hdr.Handle = 0;
  Type16Record->Location = MemoryArrayLocationSystemBoard;
  Type16Record->Use = MemoryArrayUseSystemMemory;

  //Denverton AptioV Override Start - EIP#282920 
  Type16Record->MemoryErrorCorrection = MemoryErrorCorrectionMultiBitEcc;
 
  for (Index = 0; Index <  MAX_CHANNELS ; Index++) {
    if (MrcParameters->EccEnabled[Index] != TRUE)
      Type16Record->MemoryErrorCorrection = MemoryErrorCorrectionNone;
    }  
  //Denverton AptioV Override End - EIP#282920

//  if (MrcParameters->SmbiosEccInfo) {
// Type16Record->MemoryErrorCorrection = MemoryErrorCorrectionSingleBitEcc;
//  } else { 
// Type16Record->MemoryErrorCorrection = MemoryErrorCorrectionNone;
//  }
  Type16Record->MaximumCapacity = MAX_MEM_CAPACITY; // in KB
  Type16Record->MemoryErrorInformationHandle = 0xFFFE;
  Type16Record->NumberOfMemoryDevices = (UINT16) NumSlots;

  //
  // Don't change it. This handle will be referenced by Type 17 and Type 19 SMBIOS Records
  //
  MemArraySmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (Smbios, NULL, &MemArraySmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type16Record);
  FreePool (Type16Record);
  ASSERT_EFI_ERROR (Status);
  // Denverton AptioV override start - EIP#256053
  // Type 19 SMBIOS record should be published before Type 20 SMBIOS record
  // since the Handle information will be referred in it.
  //
  // Report Memory Array Mapped Address to Type 19 SMBIOS Record
  //

  Type19Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
  ASSERT(Type19Record != NULL); 
  if (Type19Record == NULL) 
   return EFI_OUT_OF_RESOURCES;  
  ZeroMem (Type19Record, sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
  Type19Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
  Type19Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE19);
  Type19Record->Hdr.Handle = 0;
  Type19Record->StartingAddress = 0;
  Type19Record->EndingAddress = (UINT32) (RShiftU64 (TotalMemorySize, 10) - 1); // in KB
  Type19Record->MemoryArrayHandle = MemArraySmbiosHandle;
  // Denverton AptioV Override Start - EIP#265102(Type 19 Partition Width fix)
  //  Type19Record->PartitionWidth = NumSlots;
  // Denverton AptioV Override End - EIP#265102(Type 19 Partition Width fix)

  DEBUG((EFI_D_ERROR, "TotalMemorySize: 0x%08X", TotalMemorySize >> 32));
  DEBUG((EFI_D_ERROR, "%08X\n", TotalMemorySize));

  //
  // Don't change it. This handle will be referenced by Type 20 SMBIOS Records
  //
  MemArrayMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (Smbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type19Record);
  FreePool (Type19Record);
  ASSERT_EFI_ERROR (Status);
  // Denverton AptioV override end - EIP#256053

  //
  // For each socket whether it is populated or not
  // generate Type 17 structures
  //
  for (Channel = 0 ; Channel < 2; Channel++) {
    if (MrcParameters->ChEnabled[Channel]) {
      ChEnabled = TRUE;
    } else {
      ChEnabled = FALSE;
    }
    for (Dimm = 0 ; Dimm < DimmsPerCh; Dimm++) {    // Denverton AptioV Override - EIP#312471
      if (MrcParameters->DimmPresent[Channel][Dimm]== TRUE) {
        DimmPresent = TRUE;
        if ((MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_TYPE] & SPD_DDR3_MTYPE_MASK) == SPD_DDR4) {
          MemoryType = MemoryTypeDdr4;		
        //	
        // Denverton AptioV Override Start - EIP#265102(Type 19 Partition Width fix)
        //  This will be added in Type 19 structure 
        //
        PartitionWidth++;
       // Denverton AptioV Override End - EIP#265102(Type 19 Partition Width fix)
        } else {
          MemoryType = MemoryTypeDdr3;
        }
      } else {
        DimmPresent = FALSE;
        MemoryType = MemoryTypeUnknown;
      }

      DeviceLocator = mDimmToDevLocator[Channel]; //Zero based index
      BankLocator = mDimmToBankLocator[Dimm]; //Zero based index
      // Denverton AptioV Override Start - EIP#244496
      // Asset tag will be updated along with DIMM and BANK information
      /*
      if (DimmPresent) {
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"");
        AssetTagString++;
        UnicodeValueToString (StringBuffer1, PREFIX_ZERO, AssetTagString, 2);
        StrCatS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, StringBuffer1);
        AssetTag = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      } else {
        AssetTag = STRING_TOKEN (STR_MEMORY_SUBCLASS_UNKNOWN);
      }
      */
      // Denverton AptioV Override End - EIP#244496
      if (DimmPresent) {
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"");
        SpdBufferIndex = (MemoryType == MemoryTypeDdr4) ? SPD_DDR4_MODULESN : SPD_DDR3_SERIAL_NUMBER_1;
        SpdBufferIndex2 = SpdBufferIndex + 4;
        for ( ; SpdBufferIndex <  SpdBufferIndex2; SpdBufferIndex++) {
          UnicodeSPrint(StringBuffer1, 6, L"%02X", MrcParameters->SpdData[Channel][Dimm].Buffer[SpdBufferIndex]);
          StrCatS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, StringBuffer1);
        }
        SerialNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      } else {
        // Denverton AptioV Override Start - EIP#244496
        //SerialNumber = STRING_TOKEN (STR_MEMORY_SUBCLASS_UNKNOWN);
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"NO DIMM");
        SerialNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
        // Denverton AptioV Override End - EIP#244496
      }

      if (DimmPresent) {
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"");
        SpdBufferIndex = (MemoryType == MemoryTypeDdr4) ? DDR4_MODULEPN_START : START_OFFSET;
        SpdBufferIndex2 = (MemoryType == MemoryTypeDdr4) ? DDR4_MODULEPN_END : END_OFFSET;
        for ( ; SpdBufferIndex <=  SpdBufferIndex2; SpdBufferIndex++) {
          UnicodeSPrint(StringBuffer1, 4, L"%c", MrcParameters->SpdData[Channel][Dimm].Buffer[SpdBufferIndex]);
          StrCatS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, StringBuffer1);
        }
        PartNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      } else {
        // Denverton AptioV Override Start - EIP#244496
        //PartNumber = STRING_TOKEN (STR_MEMORY_SUBCLASS_UNKNOWN);
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"NO DIMM");
        PartNumber = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
        // Denverton AptioV Override End - EIP#244496
      }

      if (DimmPresent) {
        SpdBufferIndex = SpdBufferIndex = (MemoryType == MemoryTypeDdr4) ? SPD_DDR4_ORG : SPD_DDR3_ORG;
        Attributes = (MrcParameters->SpdData[Channel][Dimm].Buffer[SpdBufferIndex] >> 3) & 0x7;
        Attributes += 1;  // Add 1 since in SPD Data: 000 = 1 Rank, 001 = 2 Ranks, 010 = 3 Ranks, etc.
      } else {
        Attributes = 0;   // Unknown value
      }

      //
      // Get strings from HII database
      //

      DeviceLocatorStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, DeviceLocator, NULL);
      ASSERT (DeviceLocatorStr != NULL);
      if (DeviceLocatorStr == NULL) {
        DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
        return EFI_NOT_READY;
      }
      DeviceLocatorStrLen = StrLen (DeviceLocatorStr);
      ASSERT (DeviceLocatorStrLen <= SMBIOS_STRING_MAX_LENGTH);

      BankLocatorStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, BankLocator, NULL);
      ASSERT (BankLocatorStr != NULL);
      if (BankLocatorStr == NULL) {
        DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
        return EFI_NOT_READY;
      }
      BankLocatorStrLen = StrLen (BankLocatorStr);
      ASSERT (BankLocatorStrLen <= SMBIOS_STRING_MAX_LENGTH);
      
      // Denverton AptioV Override Start - EIP#244496
      // Asset tag updated along with Bank and Dimm information
      if (DimmPresent) {
        StrCpyS (StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"");
        StrCpyS (StringBuffer1, SMBIOS_STRING_MAX_LENGTH, L"");
        StrCatS (StringBuffer1, SMBIOS_STRING_MAX_LENGTH, BankLocatorStr);
        StrCatS (StringBuffer1, SMBIOS_STRING_MAX_LENGTH, L" ");
        StrCatS (StringBuffer1, SMBIOS_STRING_MAX_LENGTH, DeviceLocatorStr);
        StrCatS (StringBuffer1, SMBIOS_STRING_MAX_LENGTH, L" AssetTag");
        StrCatS (StringBuffer, SMBIOS_STRING_MAX_LENGTH, StringBuffer1);
        AssetTag = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      } else {
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"NO DIMM");
        AssetTag = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      }
      // Denverton AptioV Override End - EIP#244496
      if (DimmPresent) {
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"");
        SpdBufferIndex = (MemoryType == MemoryTypeDdr4) ? SPD_DDR4_MMIDH : SPD_DDR3_MANUFACTURER_ID_HI;
        for (IdListIndex=0; MemoryModuleManufactureList[IdListIndex].Index != 0xff; IdListIndex++){
          if (MemoryModuleManufactureList[IdListIndex].ManufactureId == MrcParameters->SpdData[Channel][Dimm].Buffer[SpdBufferIndex]){
            StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, MemoryModuleManufactureList[IdListIndex].ManufactureName);
            break;
          }
        }
        Manufacturer = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
        DEBUG((EFI_D_ERROR, "DimmPresent:%x\n", Dimm));
      } else {
        StrCpyS(StringBuffer, SMBIOS_STRING_MAX_LENGTH, L"NO DIMM");
        Manufacturer = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
        DEBUG((EFI_D_ERROR, "NoDimmPresent:%x\n", Dimm));
      }

      ManufactureStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, Manufacturer, NULL);
      ASSERT (ManufactureStr != NULL);
      if (ManufactureStr == NULL) {
        DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
        return EFI_NOT_READY;
      }
      ManufactureStrLen = StrLen (ManufactureStr);
      ASSERT (ManufactureStrLen <= SMBIOS_STRING_MAX_LENGTH);

      SerialNumberStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, SerialNumber, NULL);
      ASSERT (SerialNumberStr != NULL);
      if (SerialNumberStr == NULL) {
        DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
        return EFI_NOT_READY;
      }
      SerialNumberStrLen = StrLen (SerialNumberStr);
      ASSERT (SerialNumberStrLen <= SMBIOS_STRING_MAX_LENGTH);

      AssertTagStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, AssetTag, NULL);
      ASSERT (AssertTagStr != NULL);
      if (AssertTagStr == NULL) {
        DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
        return EFI_NOT_READY;
      }
      AssertTagStrLen = StrLen (AssertTagStr);
      ASSERT (AssertTagStrLen <= SMBIOS_STRING_MAX_LENGTH);

      PartNumberStr = HiiGetPackageString (&mEfiMemorySubClassDriverGuid, PartNumber, NULL);
      ASSERT (PartNumberStr != NULL);
      if (PartNumberStr == NULL) {
        DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
        return EFI_NOT_READY;
      }
      PartNumberStrLen = StrLen (PartNumberStr);
      ASSERT (PartNumberStrLen <= SMBIOS_STRING_MAX_LENGTH);

      TableSize = sizeof (SMBIOS_TABLE_TYPE17) + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1 + SerialNumberStrLen + 1 + AssertTagStrLen + 1 + PartNumberStrLen + 1 + 1;
      Type17Record = AllocatePool (TableSize);
      ASSERT(Type17Record != NULL);
      if (Type17Record == NULL) {
        DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocateZeroPool() returned NULL pointer.\n"));
        return EFI_OUT_OF_RESOURCES;
      }
      ZeroMem (Type17Record, TableSize);
      Type17Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
      Type17Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE17);
      Type17Record->Hdr.Handle = 0;
      Type17Record->MemoryArrayHandle = MemArraySmbiosHandle;
      Type17Record->MemoryErrorInformationHandle = 0xFFFE;

      switch (MrcParameters->DdrFrequency) {
        case MMRC_800:
          MemorySpeed = 800;
          break;      
        case MMRC_1066:
          MemorySpeed = 1066;
          break;      
        case MMRC_1333:
          MemorySpeed = 1333;
          break;      
        case MMRC_1600:
          MemorySpeed = 1600;
          break;      
        case MMRC_1866:
          MemorySpeed = 1866;
          break;
        case MMRC_2133:
          MemorySpeed = 2133;
          break;
        case MMRC_2400:
          MemorySpeed = 2400;
          break;      
        case MMRC_2666:
          MemorySpeed = 2666;
          break;
        case MMRC_2933:
          MemorySpeed = 2933;
          break;
        case MMRC_3200:
          MemorySpeed = 3200;
          break;		  
        default:
          MemorySpeed = 0;
          break;
      }

      SpdBufferIndex = (MemoryType == MemoryTypeDdr4) ? SPD_DDR4_MIN_TCLK : SPD_DDR3_TCLK;
      switch (MrcParameters->SpdData[Channel][Dimm].Buffer[SpdBufferIndex]) {
        case MAX_DDRFREQ_800:
          MaxMemorySpeed = 800;
          break;      
        case MAX_DDRFREQ_1066:
          MaxMemorySpeed = 1066;
          break;      
        case MAX_DDRFREQ_1333:
          MaxMemorySpeed = 1333;
          break;      
        case MAX_DDRFREQ_1600:
          MaxMemorySpeed = 1600;
          break;      
        case MAX_DDRFREQ_1866:
          MaxMemorySpeed = 1866;
          break; 
        case MAX_DDRFREQ_2133:
          MaxMemorySpeed = 2133;
          break; 
        case MAX_DDRFREQ_2400:
          MaxMemorySpeed = 2400;
          break;
        case MAX_DDRFREQ_2666:
          MaxMemorySpeed = 2666;
          break;
        case MAX_DDRFREQ_3200:
          MaxMemorySpeed = 3200;
          break;
        default:
          MaxMemorySpeed = 0;
          break;
      }

      switch (MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_WIDTH]) {
        case DATAWIDTH_8_BITS:
          Width = 8;
          break;      
        case DATAWIDTH_16_BITS:
          Width = 16;
          break;      
        case DATAWIDTH_32_BITS:
          Width = 32;
          break;      
        case DATAWIDTH_64_BITS:
          Width = 64;
          break;
        case DATAWIDTH_72_BITS:
          Width = 72;
          break;
        default:
          break;
      }

      //
      // Update Memory Formfactor based on BoardID.
      //
//Denverton AptioV Override Start - EIP#282920
//Moved earlier in the file to fill in Type16 Structures
/*      GuidHob = GetFirstGuidHob (&gPlatformInfoHobGuid);
      ASSERT (GuidHob != NULL);
	  if (GuidHob == NULL) {
	    DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
	    return EFI_NOT_READY;
	  }
      PlatformInfoHob = GET_GUID_HOB_DATA (GuidHob);
      ASSERT (PlatformInfoHob);*/
//Denverton AptioV Override End - EIP#282920
      FormFactor = MemoryFormFactorDimm;
      
      // Denverton AptioV Override Start - EIP#312471
      // Update the FormFactor irrespective of DIMM presence 
      //if (ChEnabled) {
      //  if (DimmPresent) {
          switch (PlatformInfoHob->BoardId){
            case BOARD_ID_Harcuvar:
            case BOARD_ID_CormorantLake:
              FormFactor = MemoryFormFactorDimm;
              break;
            case BOARD_ID_AspenCove:
              if ((MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_TYPE] & SPD_DDR3_MTYPE_MASK) == SPD_DDR4) {
                if (((MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK) == 3) || ((MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK) == 9)) {
                  FormFactor = MemoryFormFactorSodimm;
                } else {
                  FormFactor = MemoryFormFactorDimm;
                }
              } else {
                if (((MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK) == 3) || ((MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK) == 8)) {
                  FormFactor = MemoryFormFactorSodimm;
                } else {
                  FormFactor = MemoryFormFactorDimm;
                }
              }
              break ;
            case BOARD_ID_OstrichBay:
              FormFactor = MemoryFormFactorDimm;
              break;
            case BOARD_ID_OstrichBayS :
              FormFactor = MemoryFormFactorSodimm;
              break;
            default:
              FormFactor = MemoryFormFactorUnknown;
          }
     //   }
     // }
     // Denverton AptioV Override End - EIP#312471
          
      TotalMem = MrcParameters->SlotMem[Channel][Dimm]; // in MB
      if (!DimmPresent) {
        Width = 0;
        MemorySpeed = 0;
        MaxMemorySpeed = 0;
        TotalMem = 0;
      }
      //
      // Denverton AptioV Override Start - EIP#244496
      //
      // Size field can only hold up to 32GB - 1MB, because setting bit 15
      // indicates the value is in KB rather than MB. If the DIMM size is
      // that large or larger, use the ExtendedSize field.
      //

      if (TotalMem >= 0x7fff) {
        Type17Record->Size = 0x7fff;                    // max value
        Type17Record->ExtendedSize = TotalMem;  // in MB
      } else {
        Type17Record->Size = TotalMem;          // in MB
      }
        
      Type17Record->TotalWidth = Width;
      Type17Record->DataWidth = Width;  
      //Type17Record->Size = (UINT16) TotalMem; // in MB
	 // Denverton AptioV Override Start - EIP#312471
	 // Update the FormFactor irrespective of DIMM presence 
     // if ((ChEnabled) && (DimmPresent))  {    
        Type17Record->FormFactor = FormFactor;
     // }
	 // Denverton AptioV Override End - EIP#312471
      Type17Record->DeviceLocator = 1;
      Type17Record->BankLocator = 2;
//Denverton AptioV Override Start - EIP#283284

      if (DimmPresent) {
        Type17Record->TypeDetail.Synchronous = 1;
        Type17Record->TypeDetail.Unbuffered = 1;
      } else {
	    Type17Record->TypeDetail.Unknown = 1;
      }
//Denverton AptioV Override End - EIP#283284
      Type17Record->Speed = MaxMemorySpeed; // in MHZ
      Type17Record->Manufacturer = 3;
      Type17Record->SerialNumber = 4;
      Type17Record->AssetTag = 5;
      Type17Record->PartNumber = 6;
      Type17Record->Attributes = Attributes;
      Type17Record->MemoryType = MemoryType;
      Type17Record->ConfiguredMemoryClockSpeed = MemorySpeed; // in MHZ  Smbios v2.7

      if (ChEnabled) {
        if (DimmPresent) {
        if (MemoryType == MemoryTypeDdr4) {
          Type17Record->MinimumVoltage = 1200;
          Type17Record->MaximumVoltage = 1200;
          Type17Record->ConfiguredVoltage = 1200;
        } else {
          NominalVoltage = MrcParameters->SpdData[Channel][Dimm].Buffer[SPD_DDR3_VDD];
          NominalVoltage ^= 0x1; // Toggle bit 0
          switch (NominalVoltage & 0x7) {
            case SPD_DDRS:
              Type17Record->MinimumVoltage = DDR3_VDD_DDRS;
              Type17Record->MaximumVoltage = DDR3_VDD_DDRS;
              Type17Record->ConfiguredVoltage = DDR3_VDD_DDRS;
              break;
            case SPD_DDRL:
              Type17Record->MinimumVoltage = DDR3_VDD_DDRL;
              Type17Record->MaximumVoltage = DDR3_VDD_DDRL;
              Type17Record->ConfiguredVoltage = DDR3_VDD_DDRL;
              break;
            case SPD_DDRL_DDRS:
              Type17Record->MinimumVoltage = DDR3_VDD_DDRL;
              Type17Record->MaximumVoltage = DDR3_VDD_DDRS;
              Type17Record->ConfiguredVoltage = DDR3_VDD_DDRL;
              break;
            case SPD_DDRU:
              Type17Record->MinimumVoltage = DDR3_VDD_DDRU;
              Type17Record->MaximumVoltage = DDR3_VDD_DDRU;
              Type17Record->ConfiguredVoltage = DDR3_VDD_DDRU;
              break;
            case SPD_DDRU_DDRL:
              Type17Record->MinimumVoltage = DDR3_VDD_DDRU;
              Type17Record->MaximumVoltage = DDR3_VDD_DDRL;
              Type17Record->ConfiguredVoltage = DDR3_VDD_DDRL;
              break;
            default:
              Type17Record->MinimumVoltage = 0;
              Type17Record->MaximumVoltage = 0;
              Type17Record->ConfiguredVoltage = 0;
          }
        }
      }
    }
      OptionalStrStart = (CHAR8 *) (Type17Record + 1);
      UnicodeStrToAsciiStrS (DeviceLocatorStr, OptionalStrStart, DeviceLocatorStrLen + 1 );
      UnicodeStrToAsciiStrS(BankLocatorStr, OptionalStrStart + DeviceLocatorStrLen + 1, BankLocatorStrLen + 1 );
      UnicodeStrToAsciiStrS(ManufactureStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1,ManufactureStrLen + 1 );
      UnicodeStrToAsciiStrS(SerialNumberStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1, SerialNumberStrLen + 1 );
      UnicodeStrToAsciiStrS(AssertTagStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1 + SerialNumberStrLen + 1, AssertTagStrLen + 1 );
      UnicodeStrToAsciiStrS(PartNumberStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1 + SerialNumberStrLen + 1 + AssertTagStrLen + 1, PartNumberStrLen + 1);

      //
      // Report Memory Device to Type 17 SMBIOS Record
      //
      MemDevSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
      Status = Smbios->Add (Smbios, NULL, &MemDevSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type17Record);

      FreePool (DeviceLocatorStr);
      FreePool (BankLocatorStr);
      FreePool (ManufactureStr);
      FreePool (SerialNumberStr);
      FreePool (AssertTagStr);
      FreePool (PartNumberStr);
      FreePool (Type17Record);
      ASSERT_EFI_ERROR (Status);

      if (DimmPresent) {
        //
        // Generate Memory Device Mapped Address info (Type 20)
        //
        Type20Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE20) + 1 + 1);
        ASSERT(Type20Record != NULL); 
        if (Type20Record == NULL)
          return EFI_OUT_OF_RESOURCES;
        ZeroMem (Type20Record, sizeof (SMBIOS_TABLE_TYPE20) + 1 + 1);
        Type20Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
        Type20Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE20);
        Type20Record->Hdr.Handle = 0;
        if(TotalMem){
          // In intel RC, Type 20 table is having incorrect Starting and ending address
          // Type20Record->StartingAddress = 0;
          Type20Record->StartingAddress = Type20StartingAddr;
          // Type20Record->EndingAddress =(UINT32)( MrcParameters->TotalMem[Dimm] - 1);
          Type20StartingAddr += TotalMem << 10;
          Type20Record->EndingAddress = Type20StartingAddr -1;
          Type20Record->PartitionRowPosition = 0xFF;
          Type20Record->InterleavePosition   = 0x0;
          Type20Record->InterleavedDataDepth = 0x0;
		  // Denverton AptioV override start - EIP#256053
          Type20Record->MemoryDeviceHandle = MemDevSmbiosHandle;
          Type20Record->MemoryArrayMappedAddressHandle = MemArrayMappedAddrSmbiosHandle;
		  // Denverton AptioV override end - EIP#256053
          MemDevMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
          Status = Smbios->Add (Smbios, NULL, &MemDevMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type20Record);
          FreePool (Type20Record);
          ASSERT_EFI_ERROR (Status);
        }
        // Denverton AptioV Override End - EIP#244496
      }
    }
  }
  //
  // Denverton AptioV Override Start - EIP#265102(Type 19 Partition Width fix)
  // 
  Type = 19;
  Status = Smbios->GetNext (Smbios, &SmbiosHandle[Node], &Type, &Record, NULL);  //AptioV server override
  Type19Record = (SMBIOS_TABLE_TYPE19 *)Record;
  Type19Record->PartitionWidth = PartitionWidth;
  PartitionWidth = 0;
  //       }
  //
  // Denverton AptioV Override End - EIP#265102(Type 19 Partition Width fix)
  //

  // Denverton AptioV override start - EIP#256053
  /*
  //
  // Report Memory Array Mapped Address to Type 19 SMBIOS Record
  //

  Type19Record = AllocatePool (sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
  ASSERT(Type19Record != NULL); 
  if (Type19Record == NULL) 
   return EFI_OUT_OF_RESOURCES;  
  ZeroMem (Type19Record, sizeof (SMBIOS_TABLE_TYPE19) + 1 + 1);
  Type19Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
  Type19Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE19);
  Type19Record->Hdr.Handle = 0;
  Type19Record->StartingAddress = 0;
  Type19Record->EndingAddress = (UINT32) (RShiftU64 (TotalMemorySize, 10) - 1); // in KB
  Type19Record->MemoryArrayHandle = MemArraySmbiosHandle;
  Type19Record->PartitionWidth = NumSlots; 

  DEBUG((EFI_D_ERROR, "TotalMemorySize: 0x%08X", TotalMemorySize >> 32));
  DEBUG((EFI_D_ERROR, "%08X\n", TotalMemorySize));

  //
  // Don't change it. This handle will be referenced by Type 20 SMBIOS Records
  //
  MemArrayMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (Smbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type19Record);
  FreePool (Type19Record);
  ASSERT_EFI_ERROR (Status);
  */
  // Denverton AptioV override end - EIP#256053
  gBS->FreePool (StringBuffer);
  gBS->FreePool (StringBuffer1);	// Denverton AptioV Override - EIP#343554
  return EFI_SUCCESS;
    }
