//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbIdeBoard.c
    This file contains DXE stage board component code for IDE


*/

// Module specific Includes
#include "Efi.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/PciIo.h>
#include <protocol/BlockIo.h>
#include <Protocol/PDiskInfo.h>
#include <Protocol/PIDEController.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#include <Protocol/AmiIdeBus.h>

EFI_GUID    gIdeSetupProtocolguid   = IDE_SETUP_PROTOCOL_GUID;

/**
    This function installs the the IDE_SETUP_PROTOCOL which
    would be used in AHCI/IDE bus driver.

    @param ImageHandle - ImageHandle of the loaded driver
    @param SystemTable - Pointer to the System Table

    @retval EFI_SUCCESS

**/
EFI_STATUS 
SbIde_BoardInit (
IN EFI_HANDLE     ImageHandle,
IN EFI_SYSTEM_TABLE   *SystemTable )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle = NULL;
  SETUP_DATA              *SetupData = NULL;
  EFI_GUID                gSetupGuid = SETUP_GUID;
  UINTN                   VariableSize = 0;
  AMI_IDE_SETUP_PROTOCOL  *gIdeSetupProtocol;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->AllocatePool (EfiBootServicesData,
                            sizeof(AMI_IDE_SETUP_PROTOCOL),
                            (VOID**)&gIdeSetupProtocol);
    
    if (EFI_ERROR(Status)){
        return Status;
    }

    Status = GetEfiVariable( L"Setup",
                             &gSetupGuid,
                             NULL,
                             &VariableSize,
                             &SetupData
                           );

#if ACOUSTIC_MANAGEMENT_SUPPORT
  if (!EFI_ERROR(Status)) { 
    gIdeSetupProtocol->AcousticPwrMgmt = SetupData->HDD_AcousticPwrMgmt;
    gIdeSetupProtocol->AcousticLevel   = SetupData->HDD_AcousticMode;
  } else {
    gIdeSetupProtocol->AcousticPwrMgmt = ACOUSTIC_SUPPORT_DISABLE;
    gIdeSetupProtocol->AcousticLevel   = ACOUSTIC_LEVEL_BYPASS;
  }
#endif

#if DiPM_SUPPORT
  if (!EFI_ERROR(Status)) {
        gIdeSetupProtocol->Dipm = SetupData->HDD_DiPM;
  }   
#endif

  // Create a handle for the ArchProtocol and install Watchdog Arch Protocol on the handle
  Status = pBS->InstallProtocolInterface(
                  &Handle,
                  &gIdeSetupProtocolguid,
                  EFI_NATIVE_INTERFACE,
                  gIdeSetupProtocol
                  );

  return Status;


}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
