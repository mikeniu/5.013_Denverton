//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Parkway, Norcross, GA 30093              **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file SdioSetup.c
    Sdio driver setup related functions implementation.

**/
//----------------------------------------------------------------------

#include <AmiLib.h>
#include <AmiDxeLib.h>
#if EFI_SPECIFICATION_VERSION>0x20000
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#else
#include <Protocol/Hii.h>
#endif

#include <Setup.h>
#include <SdioElink.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/SdioBus.h>

//----------------------------------------------------------------------

typedef struct{
    UINT16  ManufactureId;
    UINT16  ManufactureCode;
    CHAR8   VendorString[30];
}SDIO_MANUFACTURE_DETAILS;

//
// SD I/O device Vendor List.
//
SDIO_MANUFACTURE_DETAILS ManuFactureDetails[] = {
    SDIO_MANUFACTURE_DEVICE_LIST
    {0xFFFF, 0xFFFF, "Unknown Vendor"}
};
#pragma pack(1)
typedef struct{
    UINT8  SdEmul;
    UINT64 SdMmcAddress;
}SDMMC_EMULATION;

typedef struct {
    UINT8           SdioMode;
    UINT8           SdioMassDevNum;
    UINT8           IoDeviceType[8];
    SDMMC_EMULATION SdioEmu[8];
} SDIO_DEV_CONFIGURATION;
#pragma pack()


/**
    This function is eLink'ed with the chain executed right before
    the Setup.

**/

VOID InitSdioStrings(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
    UINT16  SdioDev[8] = {
        STRING_TOKEN(STR_SDIO_DEVICE1),
        STRING_TOKEN(STR_SDIO_DEVICE2),
        STRING_TOKEN(STR_SDIO_DEVICE3),
        STRING_TOKEN(STR_SDIO_DEVICE4),
        STRING_TOKEN(STR_SDIO_DEVICE5),
        STRING_TOKEN(STR_SDIO_DEVICE6),
        STRING_TOKEN(STR_SDIO_DEVICE7),
        STRING_TOKEN(STR_SDIO_DEVICE8)
    };

    UINT16  SdioDevDetails[8] = {
        STRING_TOKEN(STR_SDIO_DEVICE1_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE2_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE3_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE4_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE5_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE6_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE7_DETAILS),
        STRING_TOKEN(STR_SDIO_DEVICE8_DETAILS)
    };
    
    UINT16  IoDevName[8] = {
        STRING_TOKEN(STR_IO_DEVICE1_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE2_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE3_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE4_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE5_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE6_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE7_DETAILS),
        STRING_TOKEN(STR_IO_DEVICE8_DETAILS)
    };

    UINT16  SdioDevLocation[8] = {
        STRING_TOKEN(STR_SDIO_DEVICE1_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE2_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE3_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE4_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE5_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE6_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE7_LOCATION),
        STRING_TOKEN(STR_SDIO_DEVICE8_LOCATION)
    };

    EFI_STATUS              Status;
    UINTN                   HandleCount;
    EFI_HANDLE              *HandleBuffer;
    AMI_SDIO_BUS_PROTOCOL   *SdioBusInterface;
    DLINK                   *dlink = NULL;
    SDIO_DEVICE_INTERFACE   *SdioDevInterface = NULL;
    UINT8                   SdioDevIndex=0;
    CHAR16                  sName[55];
    CHAR16                  CardSize[30];
    UINT8                   Index;
    UINT8                   i,j;
    CHAR8                   *NewString;
    CHAR16                  *NewStringTemp;
    UINTN                   seg;
    UINTN                   bus;
    UINTN                   dev;
    UINTN                   func;
    EFI_GUID                gSetupGuid = SETUP_GUID;
    UINTN                   VariableSize;
    SDIO_DEV_CONFIGURATION  SdioConfiguration;
    UINT64                  TotalBytes;
    UINT32                  DriveSizeInGB;
    UINTN                   NumTenthsOfGB;
    UINTN                   RemainderInBytes;
    UINT16                  ManufactureId=0;

    if (Class!=ADVANCED_FORM_SET_CLASS) return;

    //
    // Assume no line strings is longer than 256 bytes.
    //
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, (VOID**)&NewString);
    ASSERT_EFI_ERROR(Status);

    //
    // Assume no line strings is longer than 256 bytes.
    //
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, (VOID**)&NewStringTemp);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->LocateHandleBuffer (
                                      ByProtocol,
                                      &gSdioBusInitProtocolGuid,
                                      NULL,
                                      &HandleCount,
                                      &HandleBuffer
                                      );
    if (EFI_ERROR(Status)) HandleCount = 0;
    
    VariableSize = sizeof(SDIO_DEV_CONFIGURATION);
    Status = pRS->GetVariable( \
                    L"SdioDevConfiguration", \
                    &gSetupGuid, \
                    NULL, \
                    &VariableSize, \
                    &SdioConfiguration );

    if ( EFI_ERROR(Status) ) {
        //
        // If Error, Set default values and save "SdioConfiguration" variable.
        //
        pBS->SetMem(&SdioConfiguration, sizeof(SDIO_DEV_CONFIGURATION), 0);
    }

    for (Index = 0; Index < HandleCount; Index++) {
        Status = pBS->HandleProtocol (
                                    HandleBuffer[Index],
                                    &gSdioBusInitProtocolGuid,
                                    (VOID**)&SdioBusInterface
                                    );

        ASSERT_EFI_ERROR(Status);

        dlink = SdioBusInterface->SdioDeviceList.pHead;
        if (!dlink) {
            continue;
        }
        do {
            SdioDevInterface = OUTTER(dlink, SdioDeviceLink, SDIO_DEVICE_INTERFACE);
            dlink = dlink-> pNext;

            if(SdioDevInterface->MassStorageDevice == TRUE) {
                for(i=0;i<27;i++) {
                    sName[i]=(CHAR16)SdioDevInterface->SdioDevInfo->PNM[i];
                }
                
                //
                // The toal byes is the multiplication of block size and 
                // maximum number of block.
                //
                TotalBytes = Mul64(((UINT64)SdioDevInterface->SdioDevInfo->dMaxLBA),\
                                SdioDevInterface->SdioDevInfo->wBlockSize);
                
                DriveSizeInGB = (UINT32) Div64(TotalBytes, 1000000000, &RemainderInBytes);
                
                //
                //Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
                //
                NumTenthsOfGB = RemainderInBytes / 100000000;
                
                //
                // Print the Size in GB
                //
                pBS->SetMem(CardSize,60,0);
                Swprintf(&CardSize[0], L"(%d.%dGB)", DriveSizeInGB, NumTenthsOfGB);
                InitString(HiiHandle, SdioDev[SdioDevIndex], L"%s%s", &sName[0],&CardSize[0]);
                SdioConfiguration.SdioEmu[SdioDevIndex].SdMmcAddress = SdioDevInterface->SdioDevInfo->SdioBaseAddress;
                SdioConfiguration.IoDeviceType[SdioDevIndex] = 0;
            } else {

                //
                // SD slot has an IO device. Map the Manufacture ID into the table
                // and get the Vendor name and display it in Setup
                //

                ManufactureId=(UINT16)((UINT16)(SdioDevInterface->SdioDevInfo->SdIOManufactureId[1] << 8)
                                        + ((UINT16)SdioDevInterface->SdioDevInfo->SdIOManufactureId[0]));

                for(j=0;ManuFactureDetails[j].ManufactureId != 0xFFFF;j++) {
                    if(ManuFactureDetails[j].ManufactureId == ManufactureId) {
                        for(i=0;i<30;i++) {
                            sName[i]=(CHAR16)ManuFactureDetails[j].VendorString[i];
                        }
                        break;
                    }
                }
                //
                // Manufacture Id not Found. Initialize to Unknown Vendor.
                //
                if(ManuFactureDetails[j].ManufactureId == 0xFFFF) {
                    for(i=0;i<30;i++) {
                        sName[i]=(CHAR16)ManuFactureDetails[j].VendorString[i];
                    }
                }
                InitString(HiiHandle, IoDevName[SdioDevIndex], L"IO - %s", &sName[0]);
                SdioConfiguration.IoDeviceType[SdioDevIndex] = 1;
            }

            //
            // Get the Device Pci Location
            //
            Status = SdioBusInterface->PciIO->GetLocation(SdioBusInterface->PciIO,&seg,&bus,&dev,&func);
            pBS->SetMem(NewString,0x100,0);
            Sprintf(NewString, "Bus %x Dev %x Func %x", bus,dev,func);
            for(i=0;i<50;i++) {
                NewStringTemp[i]=(CHAR16)NewString[i];
            }
            InitString(HiiHandle, SdioDevLocation[SdioDevIndex], L"%s", NewStringTemp);

            pBS->SetMem(NewString,0x100,0);
            Sprintf(NewString, "Sdio Device %d Details:", SdioDevIndex+1);
            for(i=0;i<50;i++) {
                NewStringTemp[i]=(CHAR16)NewString[i];
            }
            InitString(HiiHandle, SdioDevDetails[SdioDevIndex], L"%s", NewStringTemp);
           
            SdioDevIndex++;

        }while (dlink);

    }

     //
     // Update "SdioMassDevNum" setup variable according to the number
     // of installed mass storage devices 
     //
    SdioConfiguration.SdioMassDevNum = SdioDevIndex;
    Status = pRS->SetVariable( \
                 L"SdioDevConfiguration", &gSetupGuid, \
                 EFI_VARIABLE_NON_VOLATILE | \
                 EFI_VARIABLE_BOOTSERVICE_ACCESS, \
                 VariableSize, &SdioConfiguration );
    ASSERT_EFI_ERROR(Status);

    if (HandleBuffer) {
        pBS->FreePool (HandleBuffer);
    }

    pBS->FreePool (NewString);

    return;
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Parkway, Norcross, GA 30093              **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
