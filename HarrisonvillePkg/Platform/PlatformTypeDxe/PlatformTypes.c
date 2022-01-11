/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformTypes.c
  
Abstract:

  Platform type driver to identify different platform.  

--*/

//
// Statements that include other files
//
#include "PlatformTypes.h"
#include <Library/GpioLib.h>
#include <GpioPinsSklH.h>
#include <Register/PchRegsGpio.h>

//
// Instantiation of Driver's private data.
//
EFI_PLATFORM_DATA_DRIVER_PRIVATE    mPrivatePlatformData;

VOID
SetAllMtrrsValid (
  VOID
)
{
  UINT32  Index;
  UINT64  MtrrValue;
  UINT32  VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64(EFI_IA32_MTRR_CAP) & B_EFI_IA32_MTRR_VARIABLE_SUPPORT);

  //
  // Check all Mtrrs to see if it is valid
  //
  for (Index = EFI_CACHE_VARIABLE_MTRR_BASE + 1; Index <= (EFI_CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit*2 - 1); Index+=2) {
    if (AsmReadMsr64 (Index - 1) == 0xFE000000) {
      AsmWriteMsr64 (Index - 1, 0);
      AsmWriteMsr64 (Index    , 0);
    } else {
      MtrrValue = AsmReadMsr64 (Index);
      if (MtrrValue && !(MtrrValue & B_EFI_CACHE_MTRR_VALID)) {
        AsmWriteMsr64 (Index, MtrrValue | B_EFI_CACHE_MTRR_VALID);
      }
    }
  } 
}
/*    
EFI_STATUS
GetPDRUUID (
)
/*++

Routine Description:
  This routine reads the PDR information - 64KB size to memory buffer, 
  then copy UUID to platform info data structure.

Arguments:
    
Returns:
  EFI_SUCCESS  -  Function has completed successfully.

--*/
/*
{
  EFI_STATUS            Status;
  EFI_CPU_IO2_PROTOCOL  *CpuIo;
  EFI_SPI_PROTOCOL      *SpiProtocol;
  UINT8                 *MemSourcePtr;
  UINT32                PDRAddress;
  EFI_PLATFORM_INFO 	*pPIT;

  Status = gBS->LocateProtocol (
                    &gEfiCpuIoProtocol2Guid,
                    NULL,
                    &CpuIo
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpuIo->Mem.Read (
              CpuIo,
              EfiCpuIoWidthUint32,
              (SB_RCBA + R_PCH_SPI_FREG4_PLATFORM_DATA),
              1,
              &PDRAddress
              );

  PDRAddress = (PDRAddress & B_PCH_SPI_FREG4_BASE_MASK) << 12;

  Status = gBS->LocateProtocol (
                  &gEfiSpiProtocolGuid,
                  NULL,
                  &SpiProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MemSourcePtr = NULL;

  Status = gBS->AllocatePool( EfiBootServicesData,
                              BLOCK_SIZE_64KB,
                              &MemSourcePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (MemSourcePtr, BLOCK_SIZE_64KB);

  pPIT = (EFI_PLATFORM_INFO *)MemSourcePtr;

  //
  // copy PDR data (64KB) to memory buffer
  //
  Status = SpiProtocol->Execute (
                          SpiProtocol,
                          SPI_OPCODE_READ_INDEX,  // OpcodeIndex
                          0,                      // PrefixOpcodeIndex
                          TRUE,                   // DataCycle
                          TRUE,                   // Atomic
                          FALSE,                  // ShiftOut
                          PDRAddress,             // Address, offset from descriptor region
                          BLOCK_SIZE_64KB,        // Data Number
                          (UINT8 *)MemSourcePtr,
                          EnumSpiRegionAll        // EnumSpiRegionPlatformData
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error during PDR read\n"));
  }

  //
  // update PDR from memory buffer
  //
  CopyMem (
      &mPrivatePlatformData.PlatformType.SystemUuid,
      pPIT->SystemUuid,
      sizeof (EFI_GUID)
      );
  gBS->FreePool (MemSourcePtr);

  return Status;
}
*/

/*
VOID
LocateBoardUuid (
   VOID
   )
{
  EFI_STATUS        Status;
  VOID              *HobList;
 // UINTN             Size;
  UINT8             *Buffer;

//  Status = ManageMfgModeVariable();
  //
  // Create a variable to be used for the version command
  //  
/*  Status = gRT->SetVariable (
    L"FWVERSIONDATA",
    &gSetupVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof(FW_VERSION),
    &FwVersionData
    );
  ASSERT_EFI_ERROR(Status);*/

 // Size = sizeof (EFI_GUID);
/*
  //
  // Get Hob list
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  //
  // Check for MFG UUID HOB
  //
  Buffer= GetNextGuidHob (&gEfiMfgUUIDHobGuid, HobList);

  // if (! (Size != sizeof (EFI_GUID) || EFI_ERROR (Status))) {
  if (Buffer != NULL) {

    //
    // If the MFG UUID Hob is present, then delete variable
    //
    Status = gRT->SetVariable (
                      VarMfgUuid,
                      &gSetupVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
  }

  Status = GetPDRUUID();

}
*/
VOID
EFIAPI
NotifyVariableEventCallback (
  IN  EFI_EVENT Event,
  IN  VOID      *Context
  )
/*++

Routine Description:
  This notification function is invoked when an instance of the
  Variable protocol is produced.

  This routine attempts to locate the board UUID in NV storage.  It should
  only be there if programmed by MFG and it will only be written to flash
  if the system is in MFG mode as controlled by the HW.

Arguments:
  Event - The event that occured
  Context - For EFI compatiblity.  Not used.

Returns:
  Nothing

--*/
{
//LocateBoardUuid ();
}

//
// System configuration (setup) information
//
SYSTEM_CONFIGURATION      mSystemConfiguration;

VOID
SendBiosCompletionToBmc (
  IN EFI_EVENT        Event,
  IN VOID             *Context
)
{
  EFI_HOB_GUID_TYPE   *GuidHob;
  PLATFORM_INFO_HOB   *PlatformInfoHobData = NULL;

  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  GuidHob = GetFirstGuidHob (&gPlatformInfoHobGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR: PlatformTypeInit () - gPlatformInfoHobGuid not found.\n"));
    return;
  }
  PlatformInfoHobData  = GET_GUID_HOB_DATA (GuidHob);
  ASSERT (PlatformInfoHobData != NULL);
  if (PlatformInfoHobData == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR: PlatformTypeInit () - PlatformInfoHobData is a NULL pointer.\n"));
    return;
  }

  if(PlatformInfoHobData->BoardId == BOARD_ID_CormorantLake){
    GpioSetOutputValue (SOUTH_GROUP1_GPIO_3, V_PCH_GPIO_TX_STATE_HIGH);
    DEBUG ((EFI_D_INFO, "\tBIOS completion notification was sent to BMC\n"));
  }
}

//
// Define platform type check
//
EFI_STATUS
EFIAPI
PlatformTypeInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
/*++

Routine Description:
  Entry point for the driver.

  This routine reads the PlatformType GPI on FWH and produces a protocol 
  to be consumed by the chipset driver to effect those settings.

Arguments:

  ImageHandle  -  Image Handle.
  SystemTable  -  EFI System Table.
  
Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  
--*/
{
  EFI_STATUS                        Status;
  EFI_HOB_GUID_TYPE                 *GuidHob;
  PLATFORM_INFO_HOB                 *PlatformInfoHobData = NULL;
  VOID                        	    *VarArch;
  UINTN                             DataSize;
  UINT32                            Attributes;
  EFI_EVENT                         ReadyToBootEvent;
  Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  DataSize = sizeof (SYSTEM_CONFIGURATION);
  // Denverton Aptiov override Start - EIP#337909
  /*
  Status = gRT->GetVariable (
                                  L"IntelSetup",
                                  &gSetupVariableGuid,
                                  &Attributes,
                                  &DataSize,
                                  &mSystemConfiguration
                                  );
  ASSERT_EFI_ERROR (Status);
  */
  // Denverton Aptiov override End - EIP#337909


  //
  // Initialize driver private data.
  // Only one instance exists
  //
  ZeroMem (&mPrivatePlatformData, sizeof (mPrivatePlatformData));
  mPrivatePlatformData.Signature            = EFI_PLATFORM_TYPE_DRIVER_PRIVATE_SIGNATURE;

  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  GuidHob       = GetFirstGuidHob (&gPlatformInfoHobGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR: PlatformTypeInit () - gPlatformInfoHobGuid not found.\n"));
    return EFI_NOT_FOUND;
  }
  PlatformInfoHobData  = GET_GUID_HOB_DATA (GuidHob);
  ASSERT (PlatformInfoHobData);
  if (PlatformInfoHobData == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR: PlatformTypeInit () - PlatformInfoHobData is a NULL pointer.\n"));
    return EFI_NOT_FOUND;
  }


  mPrivatePlatformData.PlatformType.BoardId     = PlatformInfoHobData->BoardId;
  mPrivatePlatformData.PlatformType.CpuType     = 0; //PlatformInfoHobData->CpuType;
  mPrivatePlatformData.PlatformType.CpuStepping = 0; //PlatformInfoHobData->CpuStepping;
  
  // Denverton Aptiov override Start - EIP#337909
  // Denverton Aptiov override Start - EIP#217439
  Status = gRT->GetVariable (
                                  L"IntelSetup",
                                  &gSetupVariableGuid,
                                  &Attributes,
                                  &DataSize,
                                  &mSystemConfiguration
                                  );
  // Denverton Aptiov override End - EIP#217439
  //ASSERT_EFI_ERROR (Status);
  if(!EFI_ERROR(Status)){
      mSystemConfiguration.BoardId =  PlatformInfoHobData->BoardId;

  // Denverton Aptiov override Start - EIP#217439
  Status = gST->RuntimeServices->SetVariable( L"IntelSetup",
                                              &gSetupVariableGuid,
                                              Attributes,
                                              DataSize,
                                              &mSystemConfiguration);
  }
  // Denverton Aptiov override End - EIP#217439
  // Denverton Aptiov override End - EIP#337909
  //
  // Check if platform type recorded in HOB by PEI
  //
  switch (PlatformInfoHobData->BoardId) {
    case BOARD_ID_AspenCove:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"AspenCove");
      break;
    case BOARD_ID_CormorantLake:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"CormorantLake");
      break;
    case BOARD_ID_Harcuvar:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"Harcuvar");
      break;
    case BOARD_ID_PineLake:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"PineLake");
      break;
    case BOARD_ID_PineLakeB:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"Pinelake-B");
      break;
    case BOARD_ID_CarCreekModule:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"CarCreekModule");
      break;
    case BOARD_ID_BigBugCreek:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"BigBugCreek");
      break;
	case BOARD_ID_Delphi:
	  StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"Delphi A1");
	  break;
    case BOARD_ID_Nano:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"Nano");
      break;
    default:
      StrCpyS(mPrivatePlatformData.PlatformType.PlatformName, sizeof (mPrivatePlatformData.PlatformType.PlatformName)/sizeof (CHAR16), L"Unknown");
      break;
  }
  DEBUG ((DEBUG_ERROR, "%s platform is detected!\n", mPrivatePlatformData.PlatformType.PlatformName));

  //
  // Prepare the signal response on the "Ready to Boot" event
  //
  if(PlatformInfoHobData->BoardId == BOARD_ID_CormorantLake){
    Status = EfiCreateEventReadyToBootEx (TPL_CALLBACK, SendBiosCompletionToBmc, NULL, &ReadyToBootEvent);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, "\tRegister ReadyToBoot Event to send BIOS completion notification to BMC\n"));
  }

  Status = gBS->LocateProtocol (
                   &gEfiVariableArchProtocolGuid,
                   NULL,
                   &VarArch
                   );

  /*if ( !EFI_ERROR (Status) ){
      LocateBoardUuid ();
  }*/
  
  //
  // Register to be notified when the variable write protocol has been produced.
  // Check for UUID in NV area.  If not found or error, then use UUID in Hob.
  //

  if ( EFI_ERROR (Status) ) {
    Status = gBS->CreateEvent (
                   EVT_NOTIFY_SIGNAL,
                   TPL_CALLBACK,
                   NotifyVariableEventCallback,
                   NULL,
                   &mEfiVariableProtocolEvent
                   );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                  &gEfiVariableArchProtocolGuid,
                  mEfiVariableProtocolEvent,
                  &mEfiVariableProtocolNotifyReg
                  );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Install the PlatformType policy.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPrivatePlatformData.Handle,
                  &gEfiPlatformTypeProtocolGuid,
                  &mPrivatePlatformData.PlatformType,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
}
