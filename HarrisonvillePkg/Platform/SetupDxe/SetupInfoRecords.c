/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    SetupInfoRecords.c

Abstract:

    This is the filter driver to retrieve data hub entries.

Revision History

--*/
#include "SetupPlatform.h"
#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include "MmrcData.h"
// #include <DxePcieIpInitLib.h> // Denverton AptioV override
#include <PcieIpPlatform.h>
#include "Guid/HiiSetupEnter.h" // Denverton AptioV override - EIP#250584

#pragma optimize( "", off )
extern EFI_GUID gSignalBeforeEnterSetupGuid;

EFI_PLATFORM_TYPE_PROTOCOL    *mPlatformType;
SYSTEM_CONFIGURATION          mSystemDefaults;


CPU_SETUP_INFO                mCpuSetupInfo;

EFI_STATUS
FindDataRecords (
  VOID
  )
/*++

Routine Description:

  Register event handler to find setup data records in Data Hub.

Arguments:

  None.

Returns:

  EFI_SUCCESS  -  The data hub filter event handler was registered.

--*/
{
  EFI_STATUS                  Status;
  EFI_DATA_HUB_PROTOCOL       *DataHub;
  EFI_EVENT                   FilterEvent;
  EFI_EVENT                   DiskInfoEvent;
  VOID                        *DiskInfo;
  VOID                        *FilterDummyRegistration;

  DisplayMemTopology ();
  //
  // Get the Data Hub Protocol. Assume only one instance
  // of Data Hub Protocol is availabe in the system.
  //
  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, &DataHub);
  ASSERT_EFI_ERROR (Status);

  //
  // Register our Setup Data Filter Function.
  // This function is notified at EFI_TPL_CALLBACK.
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SetupDataFilter,
                  NULL,
                  &FilterEvent
                  );
  ASSERT_EFI_ERROR (Status);
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSetupEnterGuid, // Denverton AptioV override - EIP#250584
                  FilterEvent,
                  &FilterDummyRegistration
                  );
  ASSERT_EFI_ERROR (Status);

  Status = DataHub->RegisterFilterDriver (
                      DataHub,
                      FilterEvent,
                      TPL_APPLICATION,
                      EFI_DATA_RECORD_CLASS_DATA,
                      NULL
                      );
  ASSERT_EFI_ERROR (Status);
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    IdeCallback,
                    NULL,
                    &DiskInfoEvent
                    );

    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                    &gEfiDiskInfoProtocolGuid,
                    DiskInfoEvent,
                    &DiskInfo
                    );

  return EFI_SUCCESS;
}

// Denverton AptioV Override Start - EIP#291342
/**
    This function  updates the AhciString reference corresponding to the SATA controller number
	 and port number.
	
    @param  ControllerNo           SATA Controller Number
    @param  UpdateIndex            SATA Port Number
    @param  NameToUpdateDevInfo    Reference to SATA device name string
    @param  NameToUpdateDevSize    Reference to SATA device size string

**/
VOID
UpdateAhciString (
  IN  UINT8     ControllerNo,
  IN  UINTN     UpdateIndex,
  OUT EFI_STRING_ID *NameToUpdateDevInfo,
  OUT EFI_STRING_ID *NameToUpdateDevSize  
  )
{
  if(ControllerNo == PCI_DEVICE_NUMBER_PCH_SATA) {
    switch (UpdateIndex) {
      case 0:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT0_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT0_SIZE;
              break;
      case 1:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT1_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT1_SIZE;
              break;
      case 2:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT2_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT2_SIZE;
              break;
      case 3:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT3_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT3_SIZE;
              break;
      case 4:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT4_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT4_SIZE;
              break;
      case 5:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT5_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT5_SIZE;
              break;
      case 6:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT6_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT6_SIZE;
              break;
      case 7:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT7_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT7_SIZE;
              break;
      default:
              *NameToUpdateDevInfo = 0;
              *NameToUpdateDevSize = 0;
    }
  } else if(ControllerNo == PCI_DEVICE_NUMBER_PCH_SATA2) {
    switch (UpdateIndex) {
      case 0:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT0_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT0_SIZE;
              break;
      case 1:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT1_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT1_SIZE;
              break;
      case 2:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT2_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT2_SIZE;
              break;
      case 3:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT3_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT3_SIZE;
              break;
      case 4:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT4_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT4_SIZE;
              break;
      case 5:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT5_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT5_SIZE;
              break;
      case 6:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT6_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT6_SIZE;
              break;
      case 7:
              *NameToUpdateDevInfo = (STRING_REF) STR_SATA1_PORT7_NAME;
              *NameToUpdateDevSize = (STRING_REF) STR_SATA1_PORT7_SIZE;
              break;
      default:
              *NameToUpdateDevInfo = 0;
              *NameToUpdateDevSize = 0;
    }
  }
}
// Denverton AptioV Override End - EIP#291342

VOID
EFIAPI
IdeCallback(
  IN EFI_EVENT              Event,
  IN VOID                   *Context
  )
/*++

Routine Description:

  IDE data filter function.

Arguments:

  None

Returns:

  None.

--*/
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode;
  PCI_DEVICE_PATH                 *PciDevicePath;
  UINTN                           Index;
  UINT8                           Index1;
  UINT32                          BufferSize;
  // Denverton AptioV Override Start - EIP#291342
  // Changing the variable datatype as UINT64 since UINT32 is not sufficient to store 4TB Hard Disk size.
  UINT64                          DriveSize;
  // Denverton AptioV Override End - EIP#291342
  UINT32                          IdeChannel;
  UINT32                          IdeDevice;
  EFI_IDENTIFY_DATA               *IdentifyDriveInfo;
  CHAR16                          *NewString;
  CHAR16                          SizeString[20];
  // Denverton AptioV Override Start - EIP#291342
  EFI_STRING_ID                   NameToUpdateDevInfo;
  EFI_STRING_ID                   NameToUpdateDevSize;
  // Denverton AptioV Override End - EIP#291342
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINT8                           PciBuffer;
  UINT8                           SataMode;

  //
  // Assume no line strings is longer than 256 bytes.
  //
  NewString     = AllocateZeroPool (MAX_STRING_LEN);
  ASSERT(NewString != NULL);
  if(NewString==NULL)
  {
     return;
  }
  PciDevicePath = NULL;

  // Denverton AptioV Override Start - EIP#291342
  NameToUpdateDevInfo = (STRING_REF) STR_SATA0_PORT0_NAME;
  NameToUpdateDevSize = (STRING_REF) STR_SATA0_PORT0_SIZE;
  // Denverton AptioV Override End - EIP#291342
  //
  // Initialize IDE Combined mode
  //
  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);

  Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
                                  EfiPciWidthUint8,
                                  0x1F020A,
                                  1,
                                  &PciBuffer
                                  );
  SataMode = PciBuffer;

  //
  // Fill IDE Information for IDE mode
  //
  Status = gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiDiskInfoProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );
  if (EFI_ERROR(Status)) {
	gBS->FreePool (NewString);     // Denverton AptioV Override - EIP#343554
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    ASSERT_EFI_ERROR (Status);

    DevicePathNode = DevicePath;
    while (!IsDevicePathEnd (DevicePathNode)) {
      if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == HW_PCI_DP)
          ) {
        PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        break;
      }

      DevicePathNode = NextDevicePathNode (DevicePathNode);
    }

    if (PciDevicePath == NULL) {
      continue;
    }
    //
    // Check for onboard SATA: Device(19, 20), Function (0)
    //

    // Denverton AptioV Override Start - EIP#291342
    if ((PciDevicePath->Function == PCI_FUNCTION_NUMBER_PCH_SATA) && (PciDevicePath->Device == PCI_DEVICE_NUMBER_PCH_SATA || PciDevicePath->Device == PCI_DEVICE_NUMBER_PCH_SATA2 )) {
    // Denverton AptioV Override Start - EIP#291342
      Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDiskInfoProtocolGuid, &DiskInfo);
      ASSERT_EFI_ERROR (Status);

      Status = DiskInfo->WhichIde (
                          DiskInfo,
                          &IdeChannel,
                          &IdeDevice
                          );
      // Denverton AptioV Override Start	- EIP#145396					  
      // To solve ASSERT when iSCSI is enabled - EIP#145396 - Start
      //      ASSERT_EFI_ERROR (Status);
            if(EFI_ERROR(Status))
              continue;       // Denverton AptioV Override - EIP#291342
      // To solve ASSERT when iSCSI is enabled - EIP#145396 - End
      // Denverton AptioV Override End	- EIP#145396

      BufferSize = sizeof (EFI_IDENTIFY_DATA);
      IdentifyDriveInfo = AllocatePool (BufferSize);
      ASSERT(IdentifyDriveInfo!=NULL);
      if(IdentifyDriveInfo==NULL)
      {
          return;
      }

      Status = DiskInfo->Identify (
                          DiskInfo,
                          IdentifyDriveInfo,
                          &BufferSize
                          );
      ASSERT_EFI_ERROR (Status);

       // Denverton AptioV Override Start - EIP#291342
       // Code clean-up. Not supporting IDE Mode 
       /*
        if (SataMode == PCI_CLASS_MASS_STORAGE_IDE) {   // IDE MODE
        Status = DiskInfo->WhichIde (
                             DiskInfo,
                             &IdeChannel,
                             &IdeDevice
                             );
        IdeDevice = (IdeChannel * 2) + IdeDevice;
        // Port 0 = Primary Master
        // Port 2 = Primary Slave
        // Port 1 = Secondary Master
        // Port 3 = Secondary Slave
        if (IdeDevice == 1 || IdeDevice == 2) {   // SINDX Serial ATA Index (D31:F2)
          IdeDevice ^= 3;
        }
        if (PciDevicePath->Function == 5) {  //Port 4, 5 in SATA2
          IdeDevice += 4;
        }
        NameToUpdate = UpdateAhciString(IdeDevice);

      } else { // SATA or RAID MODE


        Status = DiskInfo->WhichIde (
                             DiskInfo,
                             &IdeChannel,
                             &IdeDevice
                             );
        */

        UpdateAhciString(
                         PciDevicePath->Device,
                         IdeChannel,
                         &NameToUpdateDevInfo,
                         &NameToUpdateDevSize
                         );
        //}
        // Denverton AptioV Override End - EIP#291342

      SwapEntries ((CHAR8 *) &IdentifyDriveInfo->AtaData.ModelName, 40);
      AsciiToUnicode ((CHAR8 *) &IdentifyDriveInfo->AtaData.ModelName, NewString);
      
      // Denverton AptioV Override Start - EIP#291342
      // Chop it off after 17 characters
      NewString[17] = 0;
      
      HiiSetString(mAdvancedHiiHandle, NameToUpdateDevInfo, NewString, NULL);   //Update Sata Port # page with info

      // Clear string, to update size
      NewString[0] = 0;

      // Denverton AptioV Override End - EIP#291342

      //
      // For HardDisk append the size. Otherwise display atapi
      //
      if ((IdentifyDriveInfo->AtaData.config & 0x8000) == 00) {
        //
        // 48 bit address feature set is supported, get maximum capacity
        //
        if ((IdentifyDriveInfo->AtaData.command_set_supported_83 & 0x0400) == 0) {
          DriveSize =
            (
              (
                (
                  (
                    (IdentifyDriveInfo->AtaData.user_addressable_sectors_hi << 16) +
                    IdentifyDriveInfo->AtaData.user_addressable_sectors_lo
                  ) / 1000
                ) * 512
              ) /
              1000
            );
        } else {
          DriveSize = IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing[0];
          for (Index1 = 1; Index1 < 4; Index1++) {
            DriveSize |= LShiftU64 (
                          IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing[Index1],
                          16 * Index1
                          );
          }
          DriveSize = (UINT32) DivU64x64Remainder (MultU64x32 (DivU64x64Remainder (DriveSize, 1000, 0), 512), 1000, 0);
        }

        // Denverton AptioV Override Start - EIP#291342
        //StrCatS(NewString, MAX_STRING_LEN/sizeof(CHAR16), L" - ");
        UnicodeValueToString (SizeString, LEFT_JUSTIFY, DriveSize / 1000, 5);
        StrCatS(NewString, MAX_STRING_LEN/sizeof(CHAR16), SizeString);
        // StrCatS(NewString, MAX_STRING_LEN/sizeof(CHAR16), L".");
        // UnicodeValueToString (SizeString, LEFT_JUSTIFY, (DriveSize % 1000) / 100, 1);
        //StrCatS(NewString, MAX_STRING_LEN/sizeof(CHAR16), SizeString);
        StrCatS(NewString, MAX_STRING_LEN/sizeof(CHAR16), L" GB");
      } else {
        StrCatS(NewString, MAX_STRING_LEN/sizeof(CHAR16), L"Unknown - ATAPI ");
      }

      HiiSetString(mAdvancedHiiHandle, NameToUpdateDevSize, NewString, NULL);	//Update Sata Port # page with info
      // Denverton AptioV Override End - EIP#291342

      // Denverton Aptiov override start - EIP#217439
	  // HiiSetString(mMainHiiHandle, NameToUpdate, NewString, NULL);		//Update Main page with info as well 
      // Denverton Aptiov override end - EIP#217439
      gBS->FreePool(IdentifyDriveInfo);
    }
  }

  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
  }

  gBS->FreePool (NewString);

  return ;
}

/**

  Find the SMBIOS structure with the specified Type and Handle.
  If Type is NULL then only Handle will be matched.

  @param    Handle            The Handle to search for.
  @param    Type              The SMBIOS Type to search for.
  @param    Record            The SMBIOS record if a match is found

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.

**/
EFI_STATUS
SmbiosGetStructure (
  IN      UINT16  Handle,
  IN      EFI_SMBIOS_TYPE         Type,
  OUT     EFI_SMBIOS_TABLE_HEADER **Record
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  SMBIOS_HEADER                     *SmbiosHeader;
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = 0;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &Type, Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    SmbiosHeader = (SMBIOS_HEADER *) *Record;
    if (SmbiosHeader->Handle == Handle) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**

  Acquire the string associated with the Index from SMBIOS structure and return it.
  The caller is responsible for freeing the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.

**/
EFI_STATUS
SmbiosGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  )
{
  UINTN          StrSize;
  UINTN          StrTotal = 0;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart); // size includes null terminator
    StrTotal     += StrSize;
  // SMBIOS strings are NULL terminated, and end of all strings is indicated with NULL
  // loop until at end of all SMBIOS strings (found NULL terminator at string index past string's NULL), and Index != 0
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    return EFI_INVALID_PARAMETER;
  } else {
    AsciiStrToUnicodeStrS(OptionalStrStart, *String, StrTotal );
  }

  return EFI_SUCCESS;
}
// Denverton Aptiov override start - EIP#217439
/*
VOID
UpdateRevisionInfo (
  IN EFI_DATA_HUB_PROTOCOL              *DataHub
  )
{

  UINT8                                           StrIndex;
  CHAR16                                       *NewString;
  EFI_STATUS                                 Status;
  EFI_STRING_ID                            TokenToUpdate;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  EFI_SMBIOS_PROTOCOL              *Smbios;
  SMBIOS_TABLE_TYPE0                *Type0Record;
  EFI_SMBIOS_TABLE_HEADER       *Record;
  UINTN                                             StrSize      = 0x200;
  //
  // Display BIOS revsion
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  NewString    = AllocateZeroPool (StrSize);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
      Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
      StrIndex = Type0Record->BiosVersion;
      SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), StrIndex, &NewString);
      TokenToUpdate = STRING_TOKEN (STR_BIOS_REVISION_VALUE);
      HiiSetString (mMainHiiHandle, TokenToUpdate, NewString, NULL);
      break;
    }

  } while (1);



  return;
}
*/
// Denverton Aptiov override end - EIP#217439

VOID
UpdateCpuSetupInfo (
  IN EFI_DATA_HUB_PROTOCOL              *DataHub
  )
{
  EFI_STATUS                            Status;
  UINTN                                 StrSize;
  EFI_STRING_ID                         TokenToUpdate;
  CHAR16                                *NewString;
  CHAR16                                *VersionString;
  UINTN                                 SocketIndex;
  UINTN                                 BspPackageNumber;
  UINT32                                BspCpuidSignature;
  UINT32                                BspUcodeRevision;
  UINT8                                 StrIndex;

  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  SMBIOS_TABLE_TYPE4                    *SmbiosType4Record;
  SMBIOS_TABLE_TYPE7                    *SmbiosType7Record;
  EFI_SMBIOS_TYPE                       SmbiosType;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord2;

  UINTN                                 NaLen;
  UINT32                                SocketState;
  UINT32                                BspApicId;
  UINT32                                RegEax, RegEbx, RegEcx, RegEdx;
  CHAR16                                *NotAvailable;

  UINTN                                 bclk = 100;

  StrSize      = 0x200;
  NewString    = AllocateZeroPool (StrSize);
  ASSERT(NewString != NULL);
  if (NewString == NULL)
     return;
  NotAvailable = L"   N/A   ";
  NaLen        = StrLen (NotAvailable) ;
  ZeroMem (&mCpuSetupInfo, sizeof (CPU_SETUP_INFO));
  VersionString = AllocateZeroPool (StrSize);

  //
  // Get BSP APIC ID
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  BspCpuidSignature = RegEax;
  BspApicId = (RegEbx >> 24);
  //
  // Get BSP microcode revision
  //
  BspUcodeRevision = (UINT32) RShiftU64 (AsmReadMsr64 (0x8B), 32);

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = 0;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    SmbiosType4Record = (SMBIOS_TABLE_TYPE4 *) SmbiosRecord;
    StrIndex = SmbiosType4Record->Socket;
    if (SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &NewString) == EFI_SUCCESS) {
      // string should be 'CPUxx'
      if (StrnCmp (NewString, L"CPU", 3) != 0) {
        continue;
      }

	  SocketIndex = StrDecimalToUintn(NewString + StrLen(L"CPU"));

      // if Status [6] is set then socket is populated
      if (SocketIndex >= 0){ // && (SmbiosType4Record->Status & 0x40)) {

        mCpuSetupInfo.Id = *(UINT32 *) &SmbiosType4Record->ProcessorId.Signature;
        mCpuSetupInfo.Freq = SmbiosType4Record->CurrentSpeed;
        mCpuSetupInfo.Ucode = BspUcodeRevision;
        bclk = SmbiosType4Record->ExternalClock;

        if (SmbiosGetStructure (SmbiosType4Record->L1CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
          SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
          if ((SmbiosType7Record->CacheConfiguration & 0x7) == 0) { // [2:0] is Cache Level; verify it matches
            // [15] indicates granularity; 0=1 KB, 1=64 KB
            // set cache size in bytes by setting the value to 1024*size
            mCpuSetupInfo.L1Cache = SmbiosType7Record->InstalledSize & 0x7FFF;
            // if granularity is 64 KB, then multiply by 64
			if (SmbiosType7Record->InstalledSize & 0x8000) {
              mCpuSetupInfo.L1Cache *= 64;
            }
          }
        }

        if (SmbiosGetStructure (SmbiosType4Record->L2CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
          SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
          if ((SmbiosType7Record->CacheConfiguration & 0x7) == 1) { // [2:0] is Cache Level; verify it matches
            // [15] indicates granularity; 0=1 KB, 1=64 KB
            // set cache size in bytes by setting the value to 1024*size
            mCpuSetupInfo.L2Cache = SmbiosType7Record->InstalledSize & 0x7FFF;
            // if granularity is 64 KB, then multiply by 64
            if (SmbiosType7Record->InstalledSize & 0x8000) {
              mCpuSetupInfo.L2Cache *= 64;
            }
          }
        }

        if (SmbiosGetStructure (SmbiosType4Record->L3CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
          SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
          if ((SmbiosType7Record->CacheConfiguration & 0x7) == 2) { // [2:0] is Cache Level; verify it matches
            // [15] indicates granularity; 0=1 KB, 1=64 KB
            // set cache size in bytes by setting the value to 1024*size
            mCpuSetupInfo.L3Cache = SmbiosType7Record->InstalledSize & 0x7FFF;
            // if granularity is 64 KB, then multiply by 64
            if (SmbiosType7Record->InstalledSize & 0x8000) {
              mCpuSetupInfo.L3Cache *= 64;
            }
          }
        }

        StrIndex = SmbiosType4Record->ProcessorVersion;
        if (SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &VersionString) == EFI_SUCCESS) {
          mCpuSetupInfo.Version = VersionString;
        }
      }
    }
  }

  BspPackageNumber  = 0;

  if (mCpuSetupInfo.Version) {
      TokenToUpdate = STR_PROCESSOR_VERSION_VALUE;
    StrCpyS(NewString, MAX_STRING_LEN/sizeof(CHAR16), mCpuSetupInfo.Version);
    HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString , NULL);

  }


  SocketState = 0;
  SocketState = (mCpuSetupInfo.Id ? 0 : 1 );


  TokenToUpdate = (STRING_REF) STR_PROCESSOR_ID_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x%c",
    (UINTN)mCpuSetupInfo.Id
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }

  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString, NULL);



  TokenToUpdate = (STRING_REF) STR_PROCESSOR_FREQ_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%1d.%03dGHz ",
    (UINTN) (mCpuSetupInfo.Freq / 1000),
    (UINTN) (mCpuSetupInfo.Freq % 1000)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }

  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString, NULL);


  TokenToUpdate = (STRING_REF) STR_PROCESSOR_BCLK_FREQ_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%3dMHz ",
    (UINTN) bclk
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }

  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_MICROCODE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x  ",
    (UINTN)mCpuSetupInfo.Ucode
    );
//  if (SocketState & 1) {
  if (mCpuSetupInfo.Ucode == 0) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString, NULL);


  TokenToUpdate = (STRING_REF) STR_PROCESSOR_L1_CACHE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB ",
    (UINTN)mCpuSetupInfo.L1Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString, NULL);



  TokenToUpdate = (STRING_REF) STR_PROCESSOR_L2_CACHE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB  ",
    (UINTN)mCpuSetupInfo.L2Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Display Processor Revision in the Main page
  //
  TokenToUpdate = STR_CPU_BSP_REVISION_VALUE;
  switch (BspCpuidSignature) {

    default:
      UnicodeSPrint (NewString, StrSize, L"%5X", (UINTN)mCpuSetupInfo.Id);
  }
  HiiSetString(mAdvancedHiiHandle , TokenToUpdate, NewString, NULL); // Denverton Aptiov override - EIP#217439

  // Denverton Aptiov override start - EIP#217439
  // STR_PROCESSOR_MICROCODE_VALUE will be displayed in IntelRc page itself
  /*
  //
  // Display Microcode Revision in the Main page
  //
  TokenToUpdate = STR_PROCESSOR_MICROCODE_VALUE;
  UnicodeSPrint (NewString, StrSize, L"%08X", mCpuSetupInfo.Ucode);
  HiiSetString(mMainHiiHandle, TokenToUpdate, NewString, NULL);
  */
  // Denverton Aptiov override start - EIP#217439

  gBS->FreePool (NewString);
    gBS->FreePool (VersionString);

  return;
}

VOID
EFIAPI
SetupDataFilter (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/*++

Routine Description:

  Setup data filter function. This function is invoked when there is data records
  available in the Data Hub.

Arguments:

  Event    -  the event that is signaled.
  Context  -  not used here.

Returns:

  None.

--*/
{

  EFI_STATUS              Status;
  EFI_DATA_HUB_PROTOCOL   *DataHub;
  CHAR16                  *NewString;
  // Denverton Aptiov override start - EIP#217439
  // EFI_STRING_ID                         TokenToUpdate;
  struct memInfoSetup     *memInfo;
  // EFI_HOB_GUID_TYPE                     *GuidHob;
  VOID                    *HobList;
  UINTN                   DataSize;
  // MRC_PARAMS_SAVE_RESTORE				*MrcParameters;
  // Denverton Aptiov override end - EIP#217439
  UINT32                  SataDeviceNumber;
  UINT8                   PortIndex;

  DataSize    = 0;
  NewString   = AllocateZeroPool (SIZE_STRING_LEN);
  ASSERT(NewString!=NULL);
  if(NewString==NULL)
  {
     return;
  }

  ZeroMem(&memInfo, sizeof(struct memInfoSetup));

  Status = gBS->LocateProtocol (
                  &gEfiPlatformTypeProtocolGuid,
                  NULL,
                  &mPlatformType
                  );
  ASSERT_EFI_ERROR (Status);
  // Denverton AptioV override Start - EIP#217439
  // InitSysInfoStrings(mSysInfoHiiHandle);
  // Denverton AptioV override End - EIP#217439
  
  //
  // Get the Data Hub Protocol. Assume only one instance
  //
  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, &DataHub);
  ASSERT_EFI_ERROR (Status);
  //
  //Update Revision Information in Main page
  //
  // UpdateRevisionInfo(DataHub); // Denverton AptioV override - EIP#217439
  //
  //Update CPU information in the Processor Configuration Page
  //
  UpdateCpuSetupInfo (DataHub);

  // Denverton AptioV override - EIP#217439
  // Implementation moved to AptioV main page.
  /*
  GuidHob       = GetFirstGuidHob (&gEfiMemoryConfigDataGuid);
  MrcParameters = (MRC_PARAMS_SAVE_RESTORE *)GET_GUID_HOB_DATA (GuidHob);
  ASSERT (MrcParameters);

  TokenToUpdate = (STRING_REF) STRING_TOKEN (STR_TOTAL_MEMORY_SIZE_VALUE);
  UnicodeValueToString (
                NewString,
                LEFT_JUSTIFY,
                MrcParameters->SystemMemorySize,
                6
                );
  HiiSetString(mMainHiiHandle, TokenToUpdate, NewString, NULL);
  StrCatS(NewString, SIZE_STRING_LEN/sizeof(CHAR16), L" MB");
  TokenToUpdate = (STRING_REF) STR_TOTAL_MEMORY_SIZE_VALUE;
  HiiSetString(mMainHiiHandle, TokenToUpdate, NewString, NULL);
  */
  // Denverton Aptiov override end - EIP#217439
  //
  // Update setup variable for memory display
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  //
  // Diplay Platform Info
  //
  Status = DisplayPlatformInfo (mPlatformType);

  //
  //SATA2 Devices Installed Information
  //
  SataDeviceNumber = PCI_DEVICE_NUMBER_PCH_SATA;
  for (PortIndex = 0; PortIndex < PCH_MAX_SATA_PORTS; PortIndex ++)
	SataDevicesInfo(SataDeviceNumber, PortIndex);

  //
  //SATA3 Device Installed Information
  //
  SataDeviceNumber = PCI_DEVICE_NUMBER_PCH_SATA2;
  for (PortIndex = 0; PortIndex < PCH_MAX_SATA_PORTS; PortIndex ++){
	SataDevicesInfo(SataDeviceNumber, PortIndex);
  }

  // Denverton Aptiov override start - EIP#217439
  // for ( PortIndex=0; PortIndex < GetPchMaxPciePortNum(); PortIndex++) {
  //   PcieLinksInfo(PortIndex);
  // }
  // Denverton Aptiov override end - EIP#217439

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  //
  //Update ME Related Strings
  //
  InitMeSetupStrings ();
#endif
  
#ifdef IE_SUPPORT
  InitIeSetupStrings();
#endif //IE_SUPPORT

  gBS->FreePool (NewString);

  return ;
}

EFI_STATUS
SetIdeMode (
  VOID
  )
/*++

Routine Description:

  Set Ide's mode.

Arguments:

  None.

Returns:

  EFI_SUCCESS  -  Must return success on return.

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
ProcessorHtMode (
  VOID
  )
/*++

Routine Description:

  Setup process in HT mode.

Arguments:

  None.

Returns:

  EFI_SUCCESS  -  Must complete successfully.

--*/
{
  UINT16              State;
  EFI_STATUS          Status;
  VOID                *HobList;
  UINT32              RegEbx, RegEdx;
  EFI_HOB_GUID_TYPE   *GuidHob;
  State = 0;

  //
  // Get Hob list
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  GuidHob = GetFirstGuidHob (&gEfiCpuHtCapableGuid);
  if (GuidHob != NULL) {
     State = *(UINT16*) GET_GUID_HOB_DATA (GuidHob);
  }

  //
  // Check if processor is HT capable
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, &RegEbx, NULL, &RegEdx);
  State |= (UINT16) (!((RegEdx & 0x10000000) && (((RegEbx >> 16) & 0xFF) <= 1)));

  gRT->SetVariable (
        VAR_EQ_HT_MODE_DECIMAL_NAME,
        &mSystemConfigurationGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS,
        2,
        (VOID *) &State
        );

  return EFI_SUCCESS;
}


EFI_STATUS
DisplayPlatformInfo (
  EFI_PLATFORM_TYPE_PROTOCOL            *mPlatformType
  )
/*++

Routine Description:

  Displays the Platform info in the Setup.
  Can be used to display NorthBridge and SouthBridge info.

Arguments:

  mPlatformType.

Returns:

  EFI_SUCCESS  -  Must complete successfully.

--*/

{
  EFI_STRING_ID  TokenToUpdate;

  //
  // Platform Type Info
  //
  TokenToUpdate = (STRING_REF) STR_PLATFORM_REVISION_VALUE;
  HiiSetString(mAdvancedHiiHandle, TokenToUpdate, mPlatformType->PlatformName, NULL); // Denverton Aptiov override - EIP#217439

  return EFI_SUCCESS;
}


VOID
SataDevicesInfo (
  IN  UINTN  SataDeviceNumber,
  IN  UINT8  PortIndex
)
{
}




UINT8 mMrcTimingOffset[3] = {
  TimingTrcd, TimingTrp, TimingTras
};

VOID
DisplayMemTopology (
  VOID
  )
/*++

Routine Description:

  This function displays System Memory Info, such as size, CAS Latencies, tRCD, tRP, TRAS, etc
  Enables setup options required according to Voltage Dimm configuration

Arguments:

  None

Returns:

  None

--*/
{
  SYSTEM_CONFIGURATION    mSystemConfigurationGuid;
  BOOLEAN                 SystemConfigurationFound;
  VOID                    *HobList;
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID                    *HobData;
  UINTN                   BufferSize;
  EFI_STATUS              Status;
  MRC_PARAMETERS          *MrcParameters;
  CHAR16                  StringParameter[SIZE_STRING_PARAMETER];
  CHAR16                  TempStringParameter[20];
  UINT8                   Channel;
  UINT8                   Rank;

  UINT32                  *pMrcVersion;
  UINT32                  MrcVersion = 0;
  UINT32                  Dbuff;

  BufferSize    = 0;
  GuidHob       = NULL;
  HobList       = NULL;
  HobData       = NULL;
  MrcParameters = NULL;
  pMrcVersion   = NULL;

  //
  // Get System Configuration structure
  //
  BufferSize = sizeof (mSystemConfigurationGuid);
  // Denverton Aptiov override Start - EIP#217439
  Status = gRT->GetVariable (
                  L"IntelSetup",
                  &gSetupVariableGuid,
                  NULL,
                  &BufferSize,
                  &mSystemConfigurationGuid
                );
  // Denverton Aptiov override End - EIP#217439

  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "System Configuration found\n"));
    SystemConfigurationFound = TRUE;
    mSystemConfigurationGuid.HideChannel1 = 1;
  } else {
    DEBUG ((EFI_D_INFO, "System Configuration NOT found\n"));
    SystemConfigurationFound = FALSE;
  }

  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  HobList = GetHobList ();
  ASSERT (HobList != NULL);
  //
  // Fetch the MRC Version
  //
  GuidHob = GetNextGuidHob (&gEfiMrcVersionGuid, HobList);
  if (GuidHob != NULL) {
    DEBUG ((EFI_D_INFO, "Memory Version Data found\n"));
    pMrcVersion = (UINT32 *)GET_GUID_HOB_DATA (GuidHob);
    MrcVersion = *pMrcVersion;
    //
    // Convert the MRC Version to the correct format.
    //
    UnicodeValueToString (StringParameter,     LEFT_JUSTIFY, (MrcVersion >> 24) & 0xff, 3);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, L".");
    UnicodeValueToString (TempStringParameter, LEFT_JUSTIFY, (MrcVersion >> 16) & 0xff, 3);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, TempStringParameter);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, L".");
    UnicodeValueToString (TempStringParameter, LEFT_JUSTIFY, (MrcVersion >> 8)  & 0xff, 3);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, TempStringParameter);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, L".");
    UnicodeValueToString (TempStringParameter, LEFT_JUSTIFY, (MrcVersion >> 0)  & 0xff, 3);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, TempStringParameter);
    HiiSetString (mAdvancedHiiHandle, STR_MRC_VERSION_VALUE, StringParameter, NULL); // Denverton Aptiov override - EIP#217439
    HiiSetString (mAdvancedHiiHandle, STR_RC_REVISION_VALUE, StringParameter, NULL); // Denverton Aptiov override - EIP#217439
  } // GuidHob != NULL

  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob = GetNextGuidHob (&gEfiMemoryConfigDataGuid, HobList);

  if (GuidHob != NULL) {
    DEBUG ((EFI_D_INFO, "Memory Config Data found\n"));
    MrcParameters = (MRC_PARAMETERS *)GET_GUID_HOB_DATA (GuidHob);

    UnicodeValueToString (StringParameter, LEFT_JUSTIFY, MrcParameters->SystemMemorySize, 8);
    StrCatS(StringParameter, SIZE_STRING_PARAMETER, L" MB ");
    HiiSetString (mAdvancedHiiHandle, STR_MEMORY_SIZE_VALUE, StringParameter, NULL); // Denverton Aptiov override - EIP#217439

    Dbuff = MmioRead32 (PCH_PCR_ADDRESS (PID_DUNIT, R_DUNIT_DSCH));
    DEBUG ((EFI_D_INFO, "[DSCH]: 0x%08X\n", Dbuff));

    // -----------------------------------------------
    // Since DNV DDR type between channels 
    // is homogeneous (starting from channel 0)
    // it looks for the first enabled channel to read
    // DramType, and breaks loop when found.
    // -----------------------------------------------
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
        if (MrcParameters->ChEnabled[Channel]) {
            DEBUG ((EFI_D_INFO, "Channel[%x] found and used to detect DramType for Memory Information display menu...\n", Channel));
            break;
        } else {
            DEBUG ((EFI_D_INFO, "Channel[%x] not enabled...\n", Channel));
        }
    } // Channel loop

    // ----------------------------------------------------
    // In case of there are no channels enabled, sets 
    // Channel = 0 default, to avoid "out of boundary"
    // when DramType[] structure access.
    // ----------------------------------------------------
    if (Channel == MAX_CHANNELS) {
        DEBUG ((EFI_D_INFO, "There is no Channel[] enabled to read DramType..."));
        Channel = 0;
    }

    // ---------------------------------------------------------
    // Displaying and checking for DDR4 and DDR3L DramTypes,
    // due to DNV POR constraints.
    // ---------------------------------------------------------
    if (MrcParameters->DramType[Channel] == TypeDdr4) {
      mSystemConfigurationGuid.EnableDdr4Options      = 1;
      //
      // Disable the Mitigation feature for DDR4
      //
      mSystemConfigurationGuid.MitigationProbability  = 0;

      switch (MrcParameters->DdrFrequency) {
      case MMRC_1600:
        AsciiToUnicode ((CHAR8 *) "DDR4 - 1600 MHz", StringParameter);
        break;
      case MMRC_1866:
        AsciiToUnicode ((CHAR8 *) "DDR4 - 1866 MHz", StringParameter);
        break;
      case MMRC_2133:
        AsciiToUnicode ((CHAR8 *) "DDR4 - 2133 MHz", StringParameter);
        break;
      case MMRC_2400:
        AsciiToUnicode ((CHAR8 *) "DDR4 - 2400 MHz", StringParameter);
        break;
      case MMRC_2666:
        AsciiToUnicode ((CHAR8 *) "DDR4 - 2666 MHz", StringParameter);
        break;
      default:
        AsciiToUnicode ((CHAR8 *) "Error!!!!", StringParameter);
        break;
      }
    } else if (MrcParameters->DramType[Channel] == TypeDdr3L) {
      mSystemConfigurationGuid.EnableDdr4Options = 0;

      switch (MrcParameters->DdrFrequency) {
      case MMRC_1600:
        AsciiToUnicode ((CHAR8 *) "DDR3 - 1600 MHz", StringParameter);
        break;
      case MMRC_1866:
        AsciiToUnicode ((CHAR8 *) "DDR3 - 1866 MHz", StringParameter);
        break;
      default:
        AsciiToUnicode ((CHAR8 *) "Error!!!!", StringParameter);
        break;
      }
    } // if (MrcParameters->DramType[Channel]

    HiiSetString (mAdvancedHiiHandle, STR_MEMORY_FRQ_VALUE, StringParameter, NULL); // Denverton Aptiov override - EIP#217439
    // HiiSetString (mMainHiiHandle, STR_SYSTEM_MEMORY_SPEED_VALUE, StringParameter, NULL);// Denverton Aptiov override - EIP#217439

    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {

      if (!MrcParameters->ChEnabled[Channel]) {
        continue;
      } // !Channel Enabled

      if (SystemConfigurationFound && (Channel == 1)) {
        mSystemConfigurationGuid.HideChannel1 = 0;
      }
// Denverton AptioV Override Start
      UnicodeValueToString (StringParameter, LEFT_JUSTIFY, MrcParameters->TimingData[Channel][TimingTcl][FreqA], 4);
      if (Channel == 0) {
        HiiSetString (mAdvancedHiiHandle, STR_TCL_TIMING_VALUE_CH0, StringParameter, NULL);
      } else {
        HiiSetString (mAdvancedHiiHandle, STR_TCL_TIMING_VALUE_CH1, StringParameter, NULL);
      }
// Denverton AptioV Override End

      //
      // Allow the user to select which ranks will be used for PG Test
      //
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        if (MrcParameters->RankPresent[Channel][Rank]) {
          if (Channel == 0) {
            mSystemConfigurationGuid.HideRankCh0[Rank] = 0;
          } else {
            mSystemConfigurationGuid.HideRankCh1[Rank] = 0;
          }
        } else {
          if (Channel == 0) {
            mSystemConfigurationGuid.HideRankCh0[Rank] = 1;
          } else {
            mSystemConfigurationGuid.HideRankCh1[Rank] = 1;
          }
        }
      } // Rank loop ...
    } // Channel loop ...

    if (SystemConfigurationFound) {
      mSystemConfigurationGuid.Suppress125VOption = 1;
    } // SystemConfigurationFound

    Dbuff = MmioRead32 (PCH_PCR_ADDRESS (PID_DUNIT, R_DUNIT_DFUSESTAT));

    if ((Dbuff & B_DUNIT_DFUSESTAT_SINGLE_CHANNEL) == B_DUNIT_DFUSESTAT_SINGLE_CHANNEL) {
      mSystemConfigurationGuid.SuppressChannelOption = 1;
      mSystemConfigurationGuid.DdrChannels = 1;
    } else {
      mSystemConfigurationGuid.SuppressChannelOption = 0;
    }

    BufferSize = sizeof (mSystemConfigurationGuid);
    // Denverton Aptiov override Start - EIP#217439
    Status = gRT->SetVariable (
                    L"IntelSetup",
                    &gSetupVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    BufferSize,
                    &mSystemConfigurationGuid
                  );
    // Denverton Aptiov override End - EIP#217439
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "System Configuration was NOT set properly\n"));
    }
  } // GuidHob != NULL

  return;
}
// Denverton Aptiov override start - EIP#217439
/*
VOID
PcieLinksInfo (
  IN  UINT8  PortIndex
)
{

  UINT16 LinkStatus[PCIE_IP_STANDARD_ROOT_PORT] ;
  UINT16 NegotiatedLinkWidth[PCIE_IP_STANDARD_ROOT_PORT] ;
  UINT16 CurrentLinkSpeed [PCIE_IP_STANDARD_ROOT_PORT] ;
  UINTN  RPBase;

  STRING_REF msgNegotiatedLinkWidth[PCIE_IP_STANDARD_ROOT_PORT] =
  {
    STRING_TOKEN(RP1_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP2_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP3_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP4_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP5_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP6_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP7_NEGOTIATED_LINK_WIDTH_VALUE),
    STRING_TOKEN(RP8_NEGOTIATED_LINK_WIDTH_VALUE)
  };

  STRING_REF msgNegotiatedLinkSpeed[PCIE_IP_STANDARD_ROOT_PORT] =
  {
    STRING_TOKEN(RP1_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP2_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP3_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP4_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP5_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP6_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP7_NEGOTIATED_LINK_SPEED_VALUE),
    STRING_TOKEN(RP8_NEGOTIATED_LINK_SPEED_VALUE)
  };

    RPBase = DxePcieIpGetRootPortBase(PortIndex);

    ///
    /// do nothing if rp is not in use
    ///
    if ( (RPBase == (UINTN)INVALID_ROOT_PORT_NUMBER) || (RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT)){
      return ;
    }

    LinkStatus[PortIndex] = MmioRead16(RPBase+R_PCH_PCIE_LSTS);

    NegotiatedLinkWidth[ PortIndex ] = LinkStatus[ PortIndex ] & B_PCIE_LSTS_NLW ;
    CurrentLinkSpeed[ PortIndex ] = LinkStatus[ PortIndex ] & B_PCIE_LSTS_CLS ;

    if (LinkStatus[ PortIndex ] == ( (UINT16)PCI_DEVICE_IS_NOT_PRESENT ) ) {
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"Inactive", NULL);
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkSpeed[PortIndex], L"Inactive", NULL);
      return ;
    }

    // Negotiated Link Width
    switch (NegotiatedLinkWidth[ PortIndex ]) {
    case 0:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"No Negotiated", NULL);
      break;
    case V_PCIE_LSTS_NLW_1:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"x1", NULL);
      break;
    case V_PCIE_LSTS_NLW_2:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"x2", NULL);
      break;
    case V_PCIE_LSTS_NLW_4:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"x4", NULL);
      break;
    case V_PCIE_LSTS_NLW_8:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"x8", NULL);
      break;
    case V_PCIE_LSTS_NLW_16:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"x16", NULL);
      break;
    default:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkWidth[PortIndex], L"xNotSupported", NULL);
    }

    //Negotiated Link Speed
    switch (CurrentLinkSpeed[ PortIndex ]) {
    case V_PCIE_LSTS_CLS_GEN1:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkSpeed[PortIndex], L"2.5 Gb/s", NULL);
      break;
    case V_PCIE_LSTS_CLS_GEN2:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkSpeed[PortIndex], L"5.0 Gb/s", NULL);
      break ;
    case V_PCIE_LSTS_CLS_GEN3:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkSpeed[PortIndex], L"8.0 Gb/s", NULL);
      break;
    default:
      HiiSetString (mChipsetHiiHandle, msgNegotiatedLinkSpeed[PortIndex], L"NotSupported", NULL);
    }


  return ;
}
*/
// Denverton Aptiov override start - EIP#217439
