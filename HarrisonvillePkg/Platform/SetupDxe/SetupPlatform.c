
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SetupPlatform.c

Abstract:

  Platform Initialization Driver.

Revision History:

--*/

#include <Library/IoLib.h>

#include "SetupPlatform.h"
#include "Protocol\PciIo.h"
#include <PchAccess.h>
#include <PchPolicyCommon.h>


EFI_GUID                        mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
EFI_GUID                        mAmiTseSetupEnterProtocolGuid = AMITSE_SETUP_ENTER_GUID;
SYSTEM_CONFIGURATION            mSystemDefaults;
SYSTEM_CONFIGURATION            mSystemConfiguration;
// Denverton AptioV Override Start - EIP#180216
UINT32                          Attribute;
// Denverton AptioV Override End - EIP#180216

EFI_HII_HANDLE                  mAdvancedHiiHandle = NULL;
// Denverton Aptiov override start - EIP#217439
/*
EFI_HII_HANDLE                  mSysInfoHiiHandle = NULL;
EFI_HII_HANDLE                  mMainHiiHandle = NULL;
EFI_HII_HANDLE                  mMaintenanceHiiHandle = NULL;
EFI_HII_HANDLE                  mSecurityHiiHandle = NULL;
EFI_HII_HANDLE                  mTpmHiiHandle = NULL;
EFI_HII_HANDLE                  mChipsetHiiHandle = NULL;
*/
// Denverton Aptiov override end - EIP#217439

EFI_HANDLE                      mAdvancedDriverHandle = NULL;
// Denverton Aptiov override start - EIP#217439
/*
EFI_HANDLE                      mSysInfoDriverHandle = NULL;
EFI_HANDLE                      mMainDriverHandle = NULL;
EFI_HANDLE                      mSecurityDriverHandle = NULL;
EFI_HANDLE                      mTpmDriverHandle = NULL;
EFI_HANDLE                      mChipsetDriverHandle = NULL;
*/
// Denverton Aptiov override end - EIP#217439
EFI_HANDLE                      mImageHandle = NULL;

EFI_HII_DATABASE_PROTOCOL        *mHiiDataBase = NULL;
EFI_HII_CONFIG_ROUTING_PROTOCOL  *mHiiConfigRouting = NULL;

// Denverton AptioV Override Start   - EIP#217439
CHAR16      mVariableName[] = L"IntelSetup";
CHAR16      mVarStoreName[] = L"IntelSetup";
// Denverton AptioV Override End - EIP#217439
UINT8       gPwFlag         = VALIDATE_NONE;
UINTN       gRow, gColumn;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mMainConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mAdvancedConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mSysInfoConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mIohConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mEventLogConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mSecurityConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mTpmConfigAccess;
EFI_HII_CONFIG_ACCESS_PROTOCOL  mChipsetConfigAccess;


EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *mPciRootBridgeIo;
EFI_PLATFORM_TYPE_PROTOCOL      *mPlatformType;

STATIC UINT8                    mSmbusRsvdAddresses[PLATFORM_NUM_SMBUS_RSVD_ADDRESSES] = {
  SMBUS_ADDR_CH_A_1,
  SMBUS_ADDR_CH_A_2,
  SMBUS_ADDR_CH_B_1,
  SMBUS_ADDR_CH_B_2
};

CHAR16 mCpuSocketStr[4][5] = {L"CPU0", L"CPU1", L"CPU2", L"CPU3"};
CHAR16 mCpuAssetTagStr[] = L"UNKNOWN";
#define CPU_SOCKET_COUNT  1

CHAR16 mLastBootVarName[] = L"LastBoot";
#define LAST_BOOT_VAR_GUID { 0xc7595589, 0x610c, 0x4229, {0x89, 0xe7, 0x3c, 0x59, 0xeb, 0xf3, 0x99, 0xe2 }}
EFI_GUID mLastBootVariableGuid = LAST_BOOT_VAR_GUID;

EFI_PLATFORM_POLICY_PROTOCOL    mPlatformPolicyData = {
  PLATFORM_NUM_SMBUS_RSVD_ADDRESSES,
  mSmbusRsvdAddresses
};

EFI_BOOT_MODE       mBootMode;

EFI_STATUS
GetTpmState (
     OUT BOOLEAN                   *TpmEnable, OPTIONAL
     OUT BOOLEAN                   *TpmActivated, OPTIONAL
     OUT BOOLEAN                   *PhysicalPresenceLock, OPTIONAL
     OUT BOOLEAN                   *LifetimeLock, OPTIONAL
     OUT BOOLEAN                   *CmdEnable OPTIONAL
  );

CHAR16 						gSetupVariable[]  = L"IntelSetup";
SYSTEM_CONFIGURATION     	            SetupData;

//
// HII Vendor Device Path template for all package lists
// Differentiated by incrementing UniqueId
//
HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE) ),
          (UINT8) ( (sizeof (HII_VENDOR_DEVICE_PATH_NODE) ) >> 8)
        }
      },
      //
      // {396C010D-219A-458a-BA7D-A54D092B4BE5}
      //
      { 0x396c010d, 0x219a, 0x458a, { 0xba, 0x7d, 0xa5, 0x4d, 0x9, 0x2b, 0x4b, 0xe5 } }
    },
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ( (END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};


STATIC
EFI_STATUS
DetectTPMState (
  IN OUT BOOLEAN                   *TpmPresent
);


VOID
SetPlatformCpuPcd (
  VOID
)
{

  UINT32                        CpuSocketCount;
  UINTN                         Index;
  CHAR16                        **CpuSocketNames;
  CHAR16                        **CpuAssetTags;

  //
  // Set the count of CPU sockets on the board.
  //
  CpuSocketCount = CPU_SOCKET_COUNT;

  PcdSet32S (PcdPlatformCpuSocketCount, CpuSocketCount);

  CpuSocketNames = AllocatePool (CpuSocketCount * sizeof (UINTN) );
  ASSERT(CpuSocketNames!=NULL);
  if(CpuSocketNames==NULL)
  {
      return;
  }
  CpuAssetTags = AllocatePool (CpuSocketCount * sizeof (UINTN) );
  ASSERT(CpuAssetTags!=NULL);
  if(CpuAssetTags ==NULL)
  {
      FreePool (CpuSocketNames); // Denverton AptioV override - EIP#265689
      return;
  }

  for (Index = 0; Index < CpuSocketCount; Index++) {
    CpuSocketNames[Index] = mCpuSocketStr[Index];
    CpuAssetTags[Index] = mCpuAssetTagStr;
  }

  PcdSet64S (PcdPlatformCpuSocketNames, (UINT64) (UINTN) CpuSocketNames);
  PcdSet64S (PcdPlatformCpuAssetTags, (UINT64) (UINTN) CpuAssetTags);

  return;
}

EFI_STATUS
EFIAPI
SetupDriverEntry (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
)
/*++

Routine Description:

  This is the standard EFI driver point for the Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

Arguments:

  ImageHandle  -  Handle for the image of this driver.
  SystemTable  -  Pointer to the EFI System Table.

Returns:

  EFI_SUCCESS  -  Policy decisions set.

--*/
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;
  SYSTEM_CONFIGURATION    SetupData;
  UINTN                   SetupVariableSize = sizeof (SYSTEM_CONFIGURATION);
  // Denverton Aptiov override start - EIP#217439
  // BOOLEAN                 TpmPresent; 
  // VOID                    *mEventReg;
  // EFI_EVENT               mEvent;
  // AVN_TODO: EFI_EVENT               PciIoEvent;
  UINT32                  Attributes;

/*
  Status = gRT->GetVariable(
                  L"Setup",
                  &mSystemConfigurationGuid,
                  &Attributes,
                  &SetupVariableSize,
                  &SetupData
                  );
  */
  // Denverton Aptiov override end - EIP#217439
  mImageHandle = ImageHandle;
  //
  // Get current Boot Mode
  //
  mBootMode = GetBootModeHob ();

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &mHiiDataBase);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &mHiiConfigRouting);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
             &gEfiPlatformTypeProtocolGuid,
             NULL,
             &mPlatformType
           );
  ASSERT_EFI_ERROR (Status);

  // Denverton Aptiov override start - EIP#217439
  // Status = DetectTPMState (&TpmPresent);
  // ASSERT_EFI_ERROR (Status);

// Denverton Aptio V override start - EIP#232733
  ParseIfrPackForIntelRcSetup(mImageHandle);
// Denverton Aptio V override end - EIP#232733
/*
  if (TpmPresent == TRUE) {
    //
    // Add TPM configuration form
    //
    InstallHiiDataAndGetSettings (
      (EFI_HII_PACKAGE_HEADER *) PlatformStrings,
      (EFI_HII_PACKAGE_HEADER *) BootOptionsBin,
      (EFI_HII_PACKAGE_HEADER *) SecurityBin,
      (EFI_HII_PACKAGE_HEADER *) AdvancedBin,
      (EFI_HII_PACKAGE_HEADER *) SysInfoSetupScreenBin,
      (EFI_HII_PACKAGE_HEADER *) MainBin,
      (EFI_HII_PACKAGE_HEADER *) TpmBin,
      (EFI_HII_PACKAGE_HEADER *) ChipsetBin,
      NULL
    );
  } else {
    //
    // TPM is not present
    // Don't display TPM configuration form
    //
    InstallHiiDataAndGetSettings (
      (EFI_HII_PACKAGE_HEADER *) PlatformStrings,
      (EFI_HII_PACKAGE_HEADER *) BootOptionsBin,
      (EFI_HII_PACKAGE_HEADER *) SecurityBin,
      (EFI_HII_PACKAGE_HEADER *) AdvancedBin,
      (EFI_HII_PACKAGE_HEADER *) SysInfoSetupScreenBin,
      (EFI_HII_PACKAGE_HEADER *) MainBin,
      (EFI_HII_PACKAGE_HEADER *) ChipsetBin,
      NULL
    );
  }
  */
  // Denverton Aptiov override end - EIP#217439
  Status = SetIdeMode ();


  //
  // Process Processor Setup
  //
  Status = ProcessorHtMode ();


  ////EDKII Fix It.
  //
  // Process dynamic entries
  //
  Status = FindDataRecords ();

  //
  // Update PCDs about platform cpu setting.
  //
  SetPlatformCpuPcd();

  // Create a notification and register callback function on the
  // PciIo protocol installation to enable PCIe Bridge and PacketHub
  // MMIO and IO
  /*
      Status = gBS->CreateEvent (
                       EVT_NOTIFY_SIGNAL,
                       TPL_CALLBACK,
                       UpdateGpioConfiguration,
                       NULL,
                       &PciIoEvent
  					 );

  	if (EFI_ERROR (Status))
          return Status;

      Status = gBS->RegisterProtocolNotify (
                     &pEfiPciIoProtocolGuid,
                     PciIoEvent,
                     &gPciIoNotifyReg);

  	if (EFI_ERROR (Status))
          return Status;
  */

//DNV_TODO  SetupPlatformPolicies ();
  // Denverton Aptiov override Start - EIP#254421
  // Denverton Aptiov override Start - EIP#217439
  // If BootMode is Manufacturing Mode, clear Admin and User passwords
  //
  if (mBootMode == BOOT_WITH_MFG_MODE_SETTINGS) {
    // ZeroMem (&mSystemConfiguration.AdminPassword, SHA256_DIGEST_SIZE);
    // ZeroMem (&mSystemConfiguration.UserPassword, SHA256_DIGEST_SIZE);
    Status = gRT->GetVariable (
               mVariableName,
               &mSystemConfigurationGuid,
               &Attributes,
               &SetupVariableSize,
               &mSystemConfiguration
             ); 
    
    if (!EFI_ERROR (Status) ) {
      mSystemConfiguration.MfgMode = 1;
        
      Status = gRT->SetVariable (
                 mVariableName,
                 &mSystemConfigurationGuid,
                 Attributes,
                 SetupVariableSize,
                 &mSystemConfiguration
               ); 
    }
  }

  // SetPasswordState (&mAdminPassword);
  // SetPasswordState (&mUserPassword);
  // Denverton Aptiov override End - EIP#217439
  // Denverton Aptiov override End - EIP#254421
  //
  // Install protocol to to allow access to this Policy.
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
             &Handle,
             &gEfiPlatformPolicyProtocolGuid,
             &mPlatformPolicyData,
             NULL
           );
  ASSERT_EFI_ERROR (Status);

  if (PcdGetBool (PcdUefiOptimizedBoot) ) {

    //
    // Locate the setup variable
    //
    SetupVariableSize = sizeof (SYSTEM_CONFIGURATION);
    // Denverton Aptiov override Start - EIP#217439
    Status = gRT->GetVariable (L"IntelSetup",
                               &mSystemConfigurationGuid,
                               NULL,
                               &SetupVariableSize,
                               &SetupData);
    // Denverton Aptiov override End - EIP#217439
    if (EFI_ERROR (Status) ) {
      DEBUG ( (EFI_D_ERROR, "We can't get the setup variable!!!  \n") );
      ASSERT_EFI_ERROR (Status);
      //
      // By returning success we will still boot to the EFI shell, but not
      // provide CSM support which is not a requirement at this time.
      //
      return EFI_SUCCESS;
    }



    //
    // We now need to see if we must hack the INT10 vector for Windows UEFI.
    //
    if (SetupData.EfiWindowsInt10Workaround == 0) {
      //
      // The UEFI Video ROM is dependent upon the following protocol.  If we install it, the UEFI
      // video ROM will take control based upon the EFI over Legacy priority
      //
      Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gEfiWindowsInt10Workaround,
                 EFI_NATIVE_INTERFACE,
                 NULL // Not required as we are just using this as a dummy marker
               );
    } else {
      //
      // Here we are doing an EFI boot, but need the legacy ROM.  As such we need to also signal
      // the legacy boot path.  This has the effect of allowing the legacy infrastructure to be
      // dispatched.  However, all selections of ROM priorities etc are based upon only the deep boot
      // marker
      //
      Handle = NULL;
      Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gEfiLegacyBootMarkerGuid,
                 EFI_NATIVE_INTERFACE,
                 NULL  // Not required as we are just using this as a dummy marker
               );
    }
  } else {
    //
    // If we reached here, then we are not doing the setup option of EFI
    // Optimized boot.  As such we need to install the legacy BIOS marker protocol guid.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gEfiLegacyBootMarkerGuid,
               EFI_NATIVE_INTERFACE,
               NULL  // Not required as we are just using this as a dummy marker
             );

  }
  // Denverton AptioV override Start - EIP#217439
  /*
  // TpmAdmin should be disabled on all boot.
  if (mSystemConfiguration.TpmAdmin) {
     mSystemConfiguration.TpmAdmin = FALSE;

     Status = gRT->SetVariable (
              mVariableName,
              &mSystemConfigurationGuid,
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
              sizeof (SYSTEM_CONFIGURATION),
              &mSystemConfiguration
             );
     if (EFI_ERROR (Status) ) {
        DEBUG ( (EFI_D_ERROR, "Unable to set TPM Admin setup variable!!! \n") );
     }
  }
  //
  // Register Notification for password check before entering into setup
  //
  Status = gBS->CreateEvent (
             EVT_NOTIFY_SIGNAL,
             TPL_CALLBACK,
             CheckForPassword,
             NULL,
             &mEvent);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
             &mAmiTseSetupEnterProtocolGuid,
             mEvent,
             &mEventReg
           );
  ASSERT_EFI_ERROR (Status);
  */
  // Denverton AptioV override End - EIP#217439
  //
  // TBD or do in other places.
  // 1. Clear BIS Credentials.  - Wait for Jan's fw protocol
  // 2. Chassis Intrusion, Date/Time, EventLog, Pnp O/S,      - Do we have to do this?
  // 3. Boot (BBS, USB, PXE, VGA) - Jim's code.
  //
  return EFI_SUCCESS;
}
// Denverton Aptiov override Start - EIP#217439
/*
#define TPM_PP_USER_ABORT           ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE         ((TPM_RESULT)(-0x0f))

#define H2NS(x) ((((x) << 8) | ((x) >> 8)) & 0xffff)
#define H2NL(x) (H2NS ((x) >> 16) | (H2NS ((x) & 0xffff) << 16))

STATIC
TPM_RESULT
TpmCommandNoReturnData (
  IN     EFI_TCG_PROTOCOL          *TcgProtocol,
  IN     TPM_COMMAND_CODE          Ordinal,
  IN     UINTN                     AdditionalParameterSize,
  IN     VOID                      *AdditionalParameters
)
{
  EFI_STATUS                       Status;
  TPM_RQU_COMMAND_HDR              *TpmRqu;
  TPM_RSP_COMMAND_HDR              TpmRsp;
  UINT32                           Size;

  TpmRqu = (TPM_RQU_COMMAND_HDR *) AllocateZeroPool (
             sizeof (*TpmRqu) + AdditionalParameterSize
           );
  ASSERT (TpmRqu != NULL);
  if (TpmRqu == NULL) {
    return TPM_PP_BIOS_FAILURE;
  }

  TpmRqu->tag       = H2NS (TPM_TAG_RQU_COMMAND);
  Size              = (UINT32) (sizeof (*TpmRqu) + AdditionalParameterSize);
  TpmRqu->paramSize = H2NL (Size);
  TpmRqu->ordinal   = H2NL (Ordinal);
  gBS->CopyMem (TpmRqu + 1, AdditionalParameters, AdditionalParameterSize);

  Status = TcgProtocol->PassThroughToTpm (
             TcgProtocol,
             Size,
             (UINT8 *) TpmRqu,
             (UINT32) sizeof (TpmRsp),
             (UINT8 *) &TpmRsp
           );
  ASSERT_EFI_ERROR (Status);
  ASSERT (TpmRsp.tag == H2NS (TPM_TAG_RSP_COMMAND) );
  return H2NL (TpmRsp.returnCode);
}

STATIC
EFI_STATUS
EnableTPMDevice (
  IN     BOOLEAN                   TpmEnable
)
{
  EFI_STATUS                       Status;
  EFI_TCG_PROTOCOL                 *TcgProtocol;

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Enable/Disable TPM
  //
  if (TpmEnable == TRUE) {
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalEnable,
               0,
               NULL
             );
    DEBUG ((EFI_D_INFO, "[TPM] EnableTPMDevice Status=%04X!\n", Status));
    if (EFI_ERROR (Status) ) {
      DEBUG ( (EFI_D_ERROR, "[TPM] Fail to enable TPM device!\n") );
      return Status;
    }
  } else {
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalDisable,
               0,
               NULL
             );

    DEBUG ((EFI_D_INFO, "[TPM] DisableTPMDevice Status=%04X!\n", Status));
    if (EFI_ERROR (Status) ) {
      DEBUG ( (EFI_D_ERROR, "[TPM] Fail to disable TPM device!\n") );
      return Status;
    }
  }

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
EnableTPMPhysicalPresence (
  VOID
  )
{
  EFI_STATUS                       Status;
  EFI_TCG_PROTOCOL                 *TcgProtocol;
  TPM_PHYSICAL_PRESENCE            TpmPp;

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  ASSERT_EFI_ERROR (Status);

  // Here we assume TPM_PHYSICAL_PRESENCE_CMD_ENABLE already set ok at TcgPei driver.
  // If some one likes to check that again, please refer to SecurityCallback.c, TpmPhysicalPresence routine 
  TpmPp = H2NS (TPM_PHYSICAL_PRESENCE_PRESENT);
  Status = TpmCommandNoReturnData (
               TcgProtocol, 
               TSC_ORD_PhysicalPresence,
               sizeof (TpmPp),
               &TpmPp
               );

    DEBUG ((EFI_D_INFO, "[TPM] EnablePhysicalPresence Status=%04X!\n", Status));
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to set Physical Presence!\n"));
      return Status;
    }
   
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
LockTPMPhysicalPresence (
  VOID
  )
{
  EFI_STATUS                       Status;
  EFI_TCG_PROTOCOL                 *TcgProtocol;
  TPM_PHYSICAL_PRESENCE            TpmPp;

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  ASSERT_EFI_ERROR (Status);

  TpmPp = H2NS (TPM_PHYSICAL_PRESENCE_LOCK);
  Status = TpmCommandNoReturnData (
               TcgProtocol, 
               TSC_ORD_PhysicalPresence,
               sizeof (TpmPp),
               &TpmPp
               );

    DEBUG ((EFI_D_INFO, "[TPM] LockPhysicalPresence Status=%04X!\n", Status));
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to Lock Physical Presence!\n"));
      return Status;
    }
   
  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
ActivateTPMDevice (
  IN     BOOLEAN                   TpmActive
)
{
  EFI_STATUS                       Status;
  EFI_TCG_PROTOCOL                 *TcgProtocol;
  BOOLEAN                          BoolVal;

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Activate/Deactivate TPM
  //
  if (TpmActive == TRUE) {
    BoolVal = FALSE;
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalSetDeactivated,
               sizeof (BoolVal),
               &BoolVal
             );
    DEBUG ((EFI_D_INFO, "[TPM] ActivateTPMDevice return code: %04X!\n", Status));
    if (EFI_ERROR (Status) ) {
      DEBUG ( (EFI_D_ERROR, "[TPM] Fail to activate TPM device!\n") );
      return Status;
    }
  } else {
    BoolVal = TRUE;
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalSetDeactivated,
               sizeof (BoolVal),
               &BoolVal
             );

    DEBUG ((EFI_D_INFO, "[TPM] DeActivateTPMDevice return code: %04X!\n", Status));
    if (EFI_ERROR (Status) ) {
      DEBUG ( (EFI_D_ERROR, "[TPM] Fail to deactivate TPM device!\n") );
      return Status;
    }
  }

  return EFI_SUCCESS;
}
*/

EFI_STATUS
ParseIfrPackForIntelRcSetup(
IN EFI_HANDLE        ImageHandle
)
{
    EFI_STATUS                            Status = EFI_SUCCESS;
    EFI_HANDLE                            DriverHandle;
    EFI_HII_HANDLE                        *HiiHandle;
    EFI_GUID                              *FormsetGuid;
    HII_VENDOR_DEVICE_PATH_NODE           *VendorDevicePath;
    UINTN                                 BufferSize;
    BOOLEAN                               ActionFlag;
    BOOLEAN                               SetDefaultFlag;
    EFI_STRING                            ConfigRequestHdr;
    EFI_HII_PACKAGE_LIST_HEADER           *PackageList;
    EFI_HII_DATABASE_PROTOCOL             *HiiDatabase;
    UINT16                                Offset;
    EFI_IFR_OP_HEADER                     *IfrOpHdr;
    EFI_FORM_ID                           FormId = VFR_FORMID_INTELRCSETUP;
    BOOLEAN                               GetFormSet = FALSE;
	BOOLEAN                               GetForm = FALSE;
    EFI_HII_PACKAGE_HEADER                *IfrPackagePtr;
    UINT32                                Attribute;


    ZeroMem (&mSystemConfiguration, sizeof (mSystemConfiguration));
    ZeroMem (&mSystemDefaults, sizeof (mSystemDefaults));

    ConfigRequestHdr = NULL;
    ActionFlag       = FALSE;
    SetDefaultFlag   = FALSE;
    BufferSize       = sizeof (mSystemConfiguration);
    Status           = gRT->GetVariable (mVariableName, &gSetupVariableGuid, &Attribute, &BufferSize, &mSystemConfiguration);
    if (EFI_ERROR (Status)) {
        SetDefaultFlag = TRUE;
        Status = gRT->SetVariable (
                                    mVariableName,
                                    &gSetupVariableGuid,
                                    Attribute, 
                                    sizeof(SYSTEM_CONFIGURATION),
                                    &mSystemDefaults
                                  );
        ASSERT_EFI_ERROR (Status);
    }

    Status = gBS->OpenProtocol (
                                ImageHandle,
                                &gEfiHiiPackageListProtocolGuid,
                                (VOID **) &PackageList,
                                ImageHandle,
                                NULL,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                                );
    if (EFI_ERROR (Status)) {
            return Status;
    }

    Status = gBS->LocateProtocol (
                                &gEfiHiiDatabaseProtocolGuid,
                                NULL,
                                &HiiDatabase
                                 );
    if (EFI_ERROR (Status)) {
        return Status;
    }


    for (IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(PackageList+1);
        IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)PackageList+PackageList->PackageLength);
        IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length)) {
        if (IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {
            break;
        }
    }

    IfrOpHdr   = (EFI_IFR_OP_HEADER *)((UINT8 *) IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER));
    Offset     = sizeof (EFI_HII_PACKAGE_HEADER);
    
    while (Offset < IfrPackagePtr->Length) {
        // Check if the current formset is Intel RC formset
        if (IfrOpHdr->OpCode == EFI_IFR_FORM_SET_OP) {
            if (CompareGuid((GUID *)(VOID *)&((EFI_IFR_FORM_SET *) IfrOpHdr)->Guid, &mSystemConfigurationGuid)) {
                GetFormSet = TRUE;
            }
            else {
                GetFormSet = FALSE;
            }
        }
        // Check if the current form is Intel RC form
        else if (IfrOpHdr->OpCode == EFI_IFR_FORM_OP || IfrOpHdr->OpCode == EFI_IFR_FORM_MAP_OP) {
            if (CompareMem (&((EFI_IFR_FORM *) IfrOpHdr)->FormId, &FormId, sizeof (EFI_FORM_ID)) == 0) {
                GetForm = TRUE;
            }
            else {
                GetForm = FALSE;
            }
        }

        if (GetFormSet && GetForm) {
            DEBUG((EFI_D_ERROR, "INTELRCSETUP page FOUND in IFRPACK \n"));
            mHiiVendorDevicePathTemplate.Node.UniqueId++;
            VendorDevicePath = AllocateCopyPool (sizeof (HII_VENDOR_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
            ASSERT (VendorDevicePath != NULL);
            DriverHandle = NULL;
            Status = gBS->InstallMultipleProtocolInterfaces (
                            &DriverHandle,
                            &gEfiDevicePathProtocolGuid,
                            VendorDevicePath,
                            NULL
                          );
            ASSERT_EFI_ERROR (Status);
            mAdvancedDriverHandle = DriverHandle;
            FormsetGuid = &gAdvancedPkgListGuid;
            HiiHandle = &mAdvancedHiiHandle;
            InstallAdvancedCallbackRoutine (mAdvancedDriverHandle, &mAdvancedConfigAccess);
            break;
        }
        //
        // Go to the next Op-Code
        //
        Offset   += IfrOpHdr->Length;
        IfrOpHdr = (EFI_IFR_OP_HEADER *) ((CHAR8 *) (IfrOpHdr) + IfrOpHdr->Length);
    }
    if (Offset >= IfrPackagePtr->Length)
    {
        DEBUG((EFI_D_ERROR, "INTELRCSETUP page not found in IFRPACK \n"));
        ASSERT_EFI_ERROR (Status);
    }
    Status = HiiDatabase->NewPackageList(HiiDatabase, PackageList, DriverHandle, HiiHandle);
    ConfigRequestHdr = HiiConstructConfigHdr (&mSystemConfigurationGuid, mVarStoreName, DriverHandle);
    ASSERT (ConfigRequestHdr != NULL);
    if (SetDefaultFlag) {
          ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);
          ASSERT (ActionFlag);
    } else if (!SetDefaultFlag) {
         ActionFlag = HiiValidateSettings (ConfigRequestHdr);
         DEBUG((EFI_D_ERROR, "HiiValidateSettings: %r \n", ActionFlag ));
         ASSERT (ActionFlag);
    }
    FreePool (ConfigRequestHdr);
    return Status;
}
/*
VOID
EFIAPI
InstallHiiDataAndGetSettings (
  IN EFI_HII_PACKAGE_HEADER      *StringPack,
  ...
)
/*++

Routine Description:

  Installs a string and ifr pack set.

Arguments:

  Hii         -  pointer to an HII database protocol structure.
  StringPack  -  string pack to store in database and associate with IfrPack.
  IrfPack     -  ifr pack to store in database (it will use StringPack data).

Returns:

  None.

--
{
  EFI_STATUS                            Status;
  VA_LIST                               Args;
  EFI_HANDLE                            DriverHandle;
  EFI_HII_HANDLE                        *HiiHandle;
  EFI_HII_PACKAGE_HEADER                *IfrPack;
  EFI_GUID                              *FormsetGuid;
  HII_VENDOR_DEVICE_PATH_NODE           *VendorDevicePath;

  UINT8 					      LastBoot;
  UINTN                                               LastBootVarSize;

  BOOLEAN                         ModifiedBootMode = FALSE;
  BOOLEAN                         ResetRequired = FALSE;
  UINTN                           BufferSize;
  BOOLEAN                         ActionFlag;
  BOOLEAN                         SetDefaultFlag;
  EFI_STRING                      ConfigRequestHdr;

  ZeroMem (&mSystemConfiguration, sizeof (mSystemConfiguration) );
  ZeroMem (&mSystemDefaults, sizeof (mSystemDefaults) );

  ConfigRequestHdr = NULL;
  ActionFlag       = FALSE;
  SetDefaultFlag   = FALSE;
  BufferSize       = sizeof (mSystemConfiguration);
  LastBootVarSize  = sizeof (LastBoot);

  Status           = gRT->GetVariable (mVariableName, &gSetupVariableGuid, NULL, &BufferSize, &mSystemConfiguration);
  if (EFI_ERROR (Status) ) {
    SetDefaultFlag = TRUE;
    //
    // Write the inital zero configuration, the default setting will be set in the following.
    // MrcDefaultSettings (&mSystemDefaults);

    //
    //    UpdateIioDefaultConfigViaBoard(&mSystemDefaults);
    //
    Status = gRT->SetVariable (
               mVariableName,
               &gSetupVariableGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
               sizeof (SYSTEM_CONFIGURATION),
               &mSystemDefaults
             );
    ASSERT_EFI_ERROR (Status);
  }



  //
  //Check if reset nvram is required
  //
  if(GetBootModeHob() == BOOT_WITH_DEFAULT_SETTINGS)
  {
    DEBUG ((EFI_D_INFO,"Reset NVRAM\n"));
    SetDefaultFlag = TRUE;
	
	// Clear BIOS passwords
	ZeroMem (&mSystemConfiguration.AdminPassword, SHA256_DIGEST_SIZE );
    ZeroMem (&mSystemConfiguration.UserPassword, SHA256_DIGEST_SIZE );
    
	Status = gRT->SetVariable (
               mVariableName,
               &mSystemConfigurationGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
               sizeof (SYSTEM_CONFIGURATION),
               &mSystemConfiguration
             );
  }



  HiiHandle = NULL;
  FormsetGuid = NULL;

  IfrPack = NULL;
  VA_START (Args, StringPack);
  while ( (IfrPack = VA_ARG (Args, EFI_HII_PACKAGE_HEADER *) ) != NULL) {
    //
    // Increment UniqueID to distinguish from different device paths
    //
    mHiiVendorDevicePathTemplate.Node.UniqueId++;

    VendorDevicePath = AllocateCopyPool (sizeof (HII_VENDOR_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
    ASSERT (VendorDevicePath != NULL);

    DriverHandle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
               &DriverHandle,
               &gEfiDevicePathProtocolGuid,
               VendorDevicePath,
               NULL
             );
    ASSERT_EFI_ERROR (Status);

    if (IfrPack == (EFI_HII_PACKAGE_HEADER *) MainBin) {
      mMainDriverHandle = DriverHandle;
      FormsetGuid = &gMainPkgListGuid;
      HiiHandle = &mMainHiiHandle;
      InstallDefaultCallbackRoutine (mMainDriverHandle, &mMainConfigAccess);
    } else if (IfrPack == (EFI_HII_PACKAGE_HEADER *) AdvancedBin) {
      mAdvancedDriverHandle = DriverHandle;
      FormsetGuid = &gAdvancedPkgListGuid;
      HiiHandle = &mAdvancedHiiHandle;
      InstallAdvancedCallbackRoutine (mAdvancedDriverHandle, &mAdvancedConfigAccess); //hgm
    } else if (IfrPack == (EFI_HII_PACKAGE_HEADER *) SysInfoSetupScreenBin) {
      mSysInfoDriverHandle = DriverHandle;
      FormsetGuid = &gSysInfoPkgListGuid;
      HiiHandle = &mSysInfoHiiHandle;
      InstallDefaultCallbackRoutine (mSysInfoDriverHandle, &mSysInfoConfigAccess);
    } else if (IfrPack == (EFI_HII_PACKAGE_HEADER *) TpmBin) {
      mTpmDriverHandle = DriverHandle;
      FormsetGuid = &gTpmPkgListGuid;
      HiiHandle = &mTpmHiiHandle;
      InstallTpmCallbackRoutine (mTpmDriverHandle, &mTpmConfigAccess);
    } else if (IfrPack == (EFI_HII_PACKAGE_HEADER *) SecurityBin) {
      mSecurityDriverHandle = DriverHandle;
      FormsetGuid = &gSecurityPkgListGuid;
      HiiHandle = &mSecurityHiiHandle;
      InstallSecurityCallbackRoutine (mSecurityDriverHandle, &mSecurityConfigAccess);
    } else if (IfrPack == (EFI_HII_PACKAGE_HEADER *) ChipsetBin) {
      mChipsetDriverHandle = DriverHandle;
      FormsetGuid = &gChipsetPkgListGuid;
      HiiHandle = &mChipsetHiiHandle;
      InstallChipsetCallbackRoutine (mChipsetDriverHandle, &mChipsetConfigAccess);
    } else {
      /*
        if IfrPack is BootOptionsBin, ExitBin, LanguageBin, etc:
        These Vfr/Ifr Bins will not contribute to the default image, mSystemDefaults.
        So we "continue".
      
      continue;
    }

    *HiiHandle = HiiAddPackages (
                   FormsetGuid,
                   DriverHandle,
                   IfrPack,
                   StringPack,
                   NULL
                 );
    ASSERT (*HiiHandle != NULL);

    ConfigRequestHdr = HiiConstructConfigHdr (&mSystemConfigurationGuid, mVarStoreName, DriverHandle);
    ASSERT (ConfigRequestHdr != NULL);


    Status = gRT->GetVariable(   mLastBootVarName,
                                 &mLastBootVariableGuid,
                                 NULL,
                                 &LastBootVarSize,
                                 &LastBoot);
    if (EFI_ERROR(Status))
    {
      LastBoot = BOOT_WITH_FULL_CONFIGURATION;
    }




    if((GetBootModeHob() == BOOT_WITH_MFG_MODE_SETTINGS) && (LastBoot != BOOT_WITH_MFG_MODE_SETTINGS))
    {
      //Current boot is MFG and last boot was not MFG
      ModifiedBootMode = TRUE;
    }
    else if ((GetBootModeHob() != BOOT_WITH_MFG_MODE_SETTINGS) && (LastBoot == BOOT_WITH_MFG_MODE_SETTINGS))
    {
      //Current boot is not MFG and last Boot was MFG
      ModifiedBootMode = TRUE;
    }
    else
    {
      //Current and last boot where not MFG
      ModifiedBootMode = FALSE;
    }


    if(SetDefaultFlag && (GetBootModeHob () == BOOT_WITH_MFG_MODE_SETTINGS))
    {
      DEBUG ((EFI_D_INFO,"MFG mode ignores clear CMOS...\n"));
      SetDefaultFlag = FALSE;
    }

    if (SetDefaultFlag || ModifiedBootMode) {
      //
      //Boot is first or boot mode is being modified and needs to reinstall variables
      //



      //
      //Install default variables
      //
      ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);
      ASSERT (ActionFlag);

      if(GetBootModeHob () == BOOT_WITH_MFG_MODE_SETTINGS)
      {
          //
          //Install MFG variables
          //
    	  FreePool (ConfigRequestHdr);
    	  ConfigRequestHdr = HiiConstructConfigHdr (&mSystemConfigurationGuid, mVarStoreName, DriverHandle);
          ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_MANUFACTURING);
          ASSERT (ActionFlag);
          PcdSet16S (PcdPlatformBootTimeOut, 0);
      }

      ResetRequired = TRUE;

    }
    else
    {

      //
      //Boot was not modified and no isntall of default values required
      //

      //
      //Validate setup data
      //
      ActionFlag = HiiValidateSettings (ConfigRequestHdr);
      ModifiedBootMode = FALSE;
      ASSERT (ActionFlag);
      ResetRequired = FALSE;
    }
    FreePool (ConfigRequestHdr);
  }

  VA_END (Args);

  //
  //Update last boot value
  //
        LastBoot = (UINT8)GetBootModeHob();
        Status = gRT->SetVariable(mLastBootVarName,
                                    &mLastBootVariableGuid,
                                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                    (UINTN)sizeof(UINT8),
                                    &LastBoot);

  if(ResetRequired)
  {
    //
    //Reset system to reboot with new setup option values
    //
    DEBUG ((EFI_D_INFO,"Reset system to apply new setup values settings...\n"));

    gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
    ASSERT(0);
  }

}

STATIC
EFI_STATUS
DetectTPMState (
  IN OUT BOOLEAN                   *TpmPresent
)
{
  EFI_STATUS                       Status;
  UINTN                            VarSize;
  EFI_TCG_PROTOCOL                 *TcgProtocol;
  TCG_EFI_BOOT_SERVICE_CAPABILITY  TcgCapability;
  UINT8                            TpmDisable;
  UINT8                            TpmDeactivated;
  BOOLEAN                          BoolVal;
  BOOLEAN                          TpmHWEnable;
  BOOLEAN                          TpmHWActivated;
  BOOLEAN                          State=FALSE;

  //
  // TPM update
  // Detect current TPM state and update TPM configuration variable before display the form
  //

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  if (EFI_ERROR (Status) ) {
    *TpmPresent = FALSE;
    return EFI_SUCCESS;
  }

  ZeroMem (&mSystemConfiguration, sizeof (mSystemConfiguration) );

  // Get TPM capability
  ZeroMem (&TcgCapability, sizeof (TcgCapability) );
  TcgCapability.Size = sizeof (TcgCapability);
  Status = TcgProtocol->StatusCheck (TcgProtocol, &TcgCapability, NULL, NULL, NULL);
  if (EFI_ERROR (Status) ) {
    DEBUG ( (EFI_D_ERROR, "[TPM] In file " __FILE__ ":\n    " __FUNCTION__ "    Line %d \n", __LINE__) );
    DEBUG ( (EFI_D_ERROR, "[TPM] TCG Status Check Error!(0x%x) \n", Status) );
    return Status;
  }

  DEBUG ( (EFI_D_ERROR, "[TPM] --> TCG capability\n") );
  DEBUG ( (EFI_D_ERROR, "[TPM]        HashAlgorithmBitmap = 0x%x\n", TcgCapability.HashAlgorithmBitmap) );
  DEBUG ( (EFI_D_ERROR, "[TPM]        TPMPresentFlag = 0x%x\n", TcgCapability.TPMPresentFlag) );
  DEBUG ( (EFI_D_ERROR, "[TPM]        TPMDeactiveFlag = 0x%x\n", TcgCapability.TPMDeactivatedFlag) );
  DEBUG ( (EFI_D_ERROR, "[TPM] <-- TCG capability\n") );

  // Detect TPM device
  if (TcgCapability.TPMPresentFlag == 0x0) {
    // TPM not present
    DEBUG ( (EFI_D_ERROR, "[TPM] TPM device is not present!\n") );
    *TpmPresent = FALSE;
    return EFI_SUCCESS;
  } else {
    // TPM does present
    DEBUG ( (EFI_D_ERROR, "[TPM] TPM device is present!\n") );
    *TpmPresent = TRUE;
  }

  // Get TPM configuration variable
  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status  = gRT->GetVariable (mVariableName, &mSystemConfigurationGuid, NULL, &VarSize, &mSystemConfiguration);
  if (EFI_ERROR (Status) ) {
    DEBUG ( (EFI_D_ERROR, "[TPM] In file " __FILE__ ":\n    " __FUNCTION__ "    Line %d \n", __LINE__) );
    DEBUG ( (EFI_D_ERROR, "[TPM] TPM NV Variable not found!(0x%x) \n", Status) );
    //
    // Here means this is the first time to boot machine, the NVRAM is not allocated yet
    // Use default value instead
    //
    return EFI_SUCCESS;
  }


  //
  // In TPMDeactivatedFlag:
  // 0 bit represents TPM activate state (0- activated, 1- deactivated)
  // 1 bit represents TPM enable state (1-disable, 0 - enable)
  //
  TpmDeactivated = (UINT8) (TcgCapability.TPMDeactivatedFlag) & 0x01;
  //TpmDisable = ( (UINT8) (TcgCapability.TPMDeactivatedFlag) & 0x02) >> 1;

  // If TPM is HW-Disabled but user wants to enable TPM, do it here and reset
  // If TPM is HW-Deactivated but user wants to activate TPM, do it here and reset

  // Get the TPM hardware state
  Status = GetTpmState (&TpmHWEnable, &TpmHWActivated, NULL, NULL, NULL);
  DEBUG ((EFI_D_ERROR, "[TPM] Get TPM FlagPerm: TpmHWEnable=(0x%04x) TpmHWActivated=(0x%04x)\n", TpmHWEnable,TpmHWActivated));
  TpmDisable = (TpmHWEnable==0) ? 1 : 0;

  DEBUG ( (EFI_D_ERROR, "[TPM] TPM State TpmEnable: %d  TpmState: %d\n", mSystemConfiguration.TpmEnable, mSystemConfiguration.TpmState));
  DEBUG ( (EFI_D_ERROR, "[TPM] TPM State TpmDisable: %d  \n", TpmDisable));
  DEBUG ( (EFI_D_ERROR, "[TPM] TpmAdmin: %d  \n", mSystemConfiguration.TpmAdmin));

  // Verify if Admin Password is Installed
  State = (UINT8) (PasswordSet(SetupData.AdminPassword) ? PW_SET : PW_NOT_SET);
		
  DEBUG ( (EFI_D_ERROR, "[TPM] TPM Admin Password: %d  \n", State));
  
  if (State == PW_SET ) {
     if (mSystemConfiguration.TpmAdmin == TRUE) {
           if (((TpmHWEnable==0x00) && (mSystemConfiguration.TpmEnable==TRUE))
              ||((TpmHWActivated==0x00) && (mSystemConfiguration.TpmState==TRUE))  
             ||((TpmHWEnable==0x01) && (mSystemConfiguration.TpmEnable==FALSE))
              ||((TpmHWActivated==0x01) && (mSystemConfiguration.TpmState==FALSE)) ) {
        
         //
              // Assert TPM physical Presence
              //
              Status = EnableTPMPhysicalPresence();     // set physical present

              // Enable TPM
              if (TpmHWEnable == 0x00 && mSystemConfiguration.TpmEnable == TRUE) {
		         DEBUG ((EFI_D_INFO, "[TPM] Enable TPM device!\n"));
			     BoolVal = TRUE;
			     Status = EnableTPMDevice(BoolVal);
			     if (EFI_ERROR (Status)) {
		            DEBUG ((EFI_D_ERROR, "[TPM] Fail to Enable TPM device!\n"));
		         }
		      }
    	
		      // Activate TPM
		      if (TpmHWActivated == 0x00 && mSystemConfiguration.TpmState == TRUE) {
			     DEBUG ((EFI_D_INFO, "[TPM] Activate TPM device!\n"));
			     BoolVal = TRUE; 
			     Status = ActivateTPMDevice(BoolVal);
			     if (EFI_ERROR (Status)) {
				    DEBUG ((EFI_D_ERROR, "[TPM] Fail to activate TPM device!\n"));
			     }
		      }
	
		      // Disable TPM
		      if (TpmHWEnable == 0x01 && mSystemConfiguration.TpmEnable == FALSE) {
			     DEBUG ((EFI_D_INFO, "[TPM] Disable TPM device!\n"));
			     BoolVal = FALSE;
			     Status = EnableTPMDevice(BoolVal);
			     if (EFI_ERROR (Status)) {
				    DEBUG ((EFI_D_ERROR, "[TPM] Fail to Disable TPM device!\n"));
			     }
		      }

		      // Deactivate TPM
		      if (TpmHWActivated == 0x01 && mSystemConfiguration.TpmState == FALSE) {
			     DEBUG ((EFI_D_INFO, "[TPM] Deactivate TPM device!\n"));
			     BoolVal = FALSE;
			     Status = ActivateTPMDevice(BoolVal);
			     if (EFI_ERROR (Status)) {
				    DEBUG ((EFI_D_ERROR, "[TPM] Fail to Deactivate TPM device!\n"));
			     }
		      }

		      mSystemConfiguration.TpmAdmin = FALSE;
		      Status = gRT->SetVariable (
                       mVariableName,
                       &mSystemConfigurationGuid,
                       EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                       sizeof (SYSTEM_CONFIGURATION),
                       &mSystemConfiguration
                       );
		      if (EFI_ERROR (Status) ) {
			     DEBUG ( (EFI_D_ERROR, "Failed to reset TPM Admin setup variable!!! \n") );
		      }
		      DEBUG ((EFI_D_ERROR, "[TPM] Resetting after TPM BIOS Setup Changes!!!\n"));
		      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
		      CpuDeadLoop();
	       }
	  }
  }

  //
  // Lock TPM physical Presence
  //
  Status = LockTPMPhysicalPresence();    // Lock physical presence due to TCG PEI driver no longer locks this

  if ( (TpmDisable == 0x01 && mSystemConfiguration.TpmEnable == TRUE)
       || (TpmDisable == 0x00 && mSystemConfiguration.TpmEnable == FALSE)
       || (TpmDeactivated == 0x01 && mSystemConfiguration.TpmState == TRUE)
       || (TpmDeactivated == 0x00 && mSystemConfiguration.TpmState == FALSE) ) {
    //
    // TPM state in NVRAM differs from current TPM hardware state
    // Update NVRAM TPM variable
    //
    mSystemConfiguration.TpmEnable = (TpmDisable == 0x01) ? FALSE : TRUE;
    mSystemConfiguration.TpmState  = (TpmDeactivated == 0x01) ? FALSE : TRUE;
    DEBUG ( (EFI_D_ERROR, "[TPM] TPM state inconsistent! Set TPM State to %d : %d\n", mSystemConfiguration.TpmEnable, mSystemConfiguration.TpmState) );

    // update TPM variable
    Status = gRT->SetVariable (
               mVariableName,
               &mSystemConfigurationGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
               sizeof (SYSTEM_CONFIGURATION),
               &mSystemConfiguration
             );
    if (EFI_ERROR (Status) ) {

      DEBUG ( (EFI_D_ERROR, "[TPM] In file " __FILE__ ":\n    " __FUNCTION__ "    Line %d \n", __LINE__) );
      DEBUG ( (EFI_D_ERROR, "[TPM] Fail to update TPM NV Variable!(0x%x)\n", Status) );
      return Status;
    }
  }

  return EFI_SUCCESS;
}
*/
// Denverton Aptiov override End - EIP#217439

EFI_STATUS
EFIAPI
DefaultExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
)
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  EFI_HANDLE                       Handle;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  UINTN                            Size;
  BOOLEAN                          AllocatedRequest;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress        = Request;
  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if ( (Request != NULL) && !HiiIsConfigHdrMatch (Request, &mSystemConfigurationGuid, mVarStoreName) ) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;
  AllocatedRequest = FALSE;

  //
  // Get Buffer Storage data from EFI variable
  //
  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
             mVariableName,
             &mSystemConfigurationGuid,
             NULL,
             &BufferSize,
             &mSystemConfiguration
           );
  if (EFI_ERROR (Status) ) {
    return Status;
  }

  ConfigRequest = Request;
  if (Request == NULL || (StrStr (Request, L"OFFSET") == NULL) ) {
    //
    // Return all configurable elements
    //
    if (This == &mAdvancedConfigAccess) {
      Handle = mAdvancedDriverHandle;
      // Denverton Aptiov override start - EIP#217439
      /*
    } else if (This == &mTpmConfigAccess) {
      Handle = mTpmDriverHandle;
    } else if (This == &mMainConfigAccess) {
      Handle = mMainDriverHandle;
    } else if (This == &mChipsetConfigAccess) {
      Handle = mChipsetDriverHandle;
      */
      // Denverton Aptiov override end - EIP#217439
    } else {
      return EFI_NOT_FOUND;
    }

    //
    // Request is set to NULL, construct full request string.
    //

    //
    // First Set ConfigRequestHdr string.
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&mSystemConfigurationGuid, mVarStoreName, Handle);
    ASSERT (ConfigRequestHdr != NULL);

    if(ConfigRequestHdr == NULL) {
      DEBUG ((EFI_D_ERROR, "\nERROR!! DefaultExtractConfig() - ConfigRequestHdr variable is NULL\n"));
      return EFI_INVALID_PARAMETER;
    }

    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);

    if(ConfigRequest == NULL) {
      FreePool (ConfigRequestHdr);
      DEBUG ((EFI_D_ERROR, "\nERROR!! DefaultExtractConfig() - ConfigRequest variable is NULL\n"));
      return EFI_INVALID_PARAMETER;
    }

    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64) BufferSize);
    FreePool (ConfigRequestHdr);
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = mHiiConfigRouting->BlockToConfig (
             mHiiConfigRouting,
             ConfigRequest,
             (UINT8 *) &mSystemConfiguration,
             BufferSize,
             Results,
             Progress
           );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = (EFI_STRING) (Request + StrLen (Request) );
  }

  return Status;
}

EFI_STATUS
EFIAPI
DefaultRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
)
{
  EFI_STATUS                                 Status;
  UINTN                                      BufferSize;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &mSystemConfigurationGuid, mVarStoreName) ) {
    return EFI_NOT_FOUND;
  }

  //
  // Get Buffer Storage data from EFI variable
  //
  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
             mVariableName,
             &mSystemConfigurationGuid,
             // Denverton AptioV Override Start - EIP#180216
             &Attribute, 
             // Denevrton AptioV Override End - EIP#180216
             &BufferSize,
             &mSystemConfiguration
           );
  if (EFI_ERROR (Status) ) {
    return Status;
  }

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = mHiiConfigRouting->ConfigToBlock (
             mHiiConfigRouting,
             Configuration,
             (UINT8 *) &mSystemConfiguration,
             &BufferSize,
             Progress
           );

  //
  // Store Buffer Storage back to EFI variable
  //
  Status = gRT->SetVariable (
             mVariableName,
             &mSystemConfigurationGuid,
			 // Denverton AptioV Override Start - EIP#180216
             //EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
             Attribute, 
             // Denverton AptioV Override End - EIP#180216
             sizeof (SYSTEM_CONFIGURATION),
             &mSystemConfiguration
           );
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  Status = MeAdvancedRouteConfig (&mSystemConfiguration);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, __FUNCTION__ ": MeAdvancedRouteConfig returns: %r.\n", Status));
  }
#endif
  return Status;
}

EFI_STATUS
EFIAPI
DefaultCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
)
{
  return EFI_UNSUPPORTED;
}

// Denverton AptioV Override Start - EIP#217439
/*
EFI_STATUS
InstallDefaultCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
)
{
  EFI_STATUS          Status;

  ASSERT (ConfigAccess != NULL);

  if(ConfigAccess == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR!! InstallDefaultCallbackRoutine() - ConfigAccess pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  ConfigAccess->ExtractConfig = DefaultExtractConfig;
  ConfigAccess->RouteConfig   = DefaultRouteConfig;
  ConfigAccess->Callback      = DefaultCallbackRoutine;

  Status = gBS->InstallProtocolInterface (
             &DriverHandle,
             &gEfiHiiConfigAccessProtocolGuid,
             EFI_NATIVE_INTERFACE,
             ConfigAccess
           );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
*/
// Denverton AptioV Override End - EIP#217439

EFI_STATUS
EFIAPI
AdvancedCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
)
{
  EFI_STATUS                                 Status;
  UINTN                                      MyVarSize;

  Status = EFI_SUCCESS;

#ifndef SOFTSVD_FLAG
  switch (Action) {
  case EFI_BROWSER_ACTION_FORM_CLOSE:
    //
    // Do nothing for UEFI CLOSE/OPEN Action
    //
    break;
  case EFI_BROWSER_ACTION_FORM_OPEN:

    break;
  case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
    // Check for PbgTPresent setup option and update it with the current PCH type
    if (QuestionId == 0x2000) {
      MyVarSize = sizeof (SYSTEM_CONFIGURATION);
      //mSystemConfiguration.PbgTPresent = mPlatformType->PchType;
      Status = gRT->SetVariable (
                 mVariableName,
                 &mSystemConfigurationGuid,
				 // Denverton AptioV Override Start - EIP#180216
                 //EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                 Attribute, 
                 // Denverton AptioV Override End - EIP#180216				 
                 MyVarSize,
                 &mSystemConfiguration
               );
      if (EFI_ERROR (Status) ) {
        return Status;
      }
    }
    // Update PchPciePortConfig according to softstraps info
    if (QuestionId == 0x2001) {
      MyVarSize = sizeof (SYSTEM_CONFIGURATION);
      //	       mSystemConfiguration.PciePortConfig1 = mPlatformType->PchPciePortCfg1;
      //         mSystemConfiguration.PciePortConfig2 = mPlatformType->PchPciePortCfg2;
      Status = gRT->SetVariable (
                 mVariableName,
                 &mSystemConfigurationGuid,
				 // Denverton AptioV Override Start - EIP#180216
                 //EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                 Attribute, 
                 // Denverton AptioV Override End - EIP#180216
                 MyVarSize,
                 &mSystemConfiguration
               );
      if (EFI_ERROR (Status) ) {
        return Status;
      }
    }
    break;
  case EFI_BROWSER_ACTION_RETRIEVE:
    break;
  default:
    //
    // Do nothing for other UEFI Action. Only do call back when data is changed.
    //
    Status = EFI_UNSUPPORTED;
    break;
  }
#endif
  return Status;
}
// Denverton AptioV Override Start - EIP#217439
/*
EFI_STATUS
EFIAPI
ChipsetCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
)
{
  return EFI_SUCCESS;
}
*/
// Denverton AptioV Override End - EIP#217439
EFI_STATUS
InstallAdvancedCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
)
{
  EFI_STATUS          Status;

  ASSERT (ConfigAccess != NULL);

  if(ConfigAccess == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR!! InstallAdvancedCallbackRoutine() - ConfigAccess pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  ConfigAccess->ExtractConfig = DefaultExtractConfig;
  ConfigAccess->RouteConfig   = DefaultRouteConfig;
  ConfigAccess->Callback      = AdvancedCallbackRoutine;

  Status = gBS->InstallProtocolInterface (
             &DriverHandle,
             &gEfiHiiConfigAccessProtocolGuid,
             EFI_NATIVE_INTERFACE,
             ConfigAccess
           );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

// Denverton AptioV Override Start - EIP#217439
/*

EFI_STATUS
InstallChipsetCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
)
{
  EFI_STATUS          Status;

  ASSERT (ConfigAccess != NULL);

  if(ConfigAccess == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR!! InstallChipsetCallbackRoutine() - ConfigAccess pointer is NULL \n"));
    return EFI_INVALID_PARAMETER;
  }

  ConfigAccess->ExtractConfig = DefaultExtractConfig;
  ConfigAccess->RouteConfig   = DefaultRouteConfig;
  ConfigAccess->Callback      = ChipsetCallbackRoutine;

  Status = gBS->InstallProtocolInterface (
             &DriverHandle,
             &gEfiHiiConfigAccessProtocolGuid,
             EFI_NATIVE_INTERFACE,
             ConfigAccess
           );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}




VOID
EFIAPI
CheckForPassword (
  IN EFI_EVENT        Event,
  IN VOID             *Context
)
{
  EFI_STATUS          Status;
  CHAR16             *ErrorMsgBuf1;
  CHAR16             *ErrorMsgBuf2;
  UINT8               ReTries;
  EFI_INPUT_KEY       Key;
  UINTN               Index;

  ErrorMsgBuf1   = NULL;
  ErrorMsgBuf2   = NULL;
  Status         = EFI_SUCCESS;

  //This callback needs to run at TPL_APPLICATION so USB driver can poll for keystrokes
  gBS->RestoreTPL(TPL_APPLICATION);

  ErrorMsgBuf1 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16) );
  ASSERT (ErrorMsgBuf1 != NULL);

  ErrorMsgBuf2 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16) );
  ASSERT (ErrorMsgBuf2 != NULL);
  //
  // Check if we need to validate at this point
  //
  gPwFlag = CheckIfPasswordInstalled();

  if (gPwFlag != VALIDATE_NONE ) {
    //
    // User has to be validated
    //
    for (ReTries = 0; ReTries < 3;) {
      Status = PromptForPassword ();
      if (Status == AUTHENTICATION_PASSED) {
        //
        // Passed Authentication done , You are allowed to Enter BIOS Setup
        //
        break;
      } else {
        do {

          CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, L"Invalid Password", NULL, NULL);
        } while ( (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) );

      }
      //
      // Maximum of 3 retries are allowed to enter Wrong password
      //
      ReTries++;
    }
    if (Status == AUTHENTICATION_NOT_PASSED) {
      //
      // Popup a menu to notice user
      //
      do {
        //
        // Retry Limit is over, Halt the Machine
        //
        StrCpy (ErrorMsgBuf1, L" System Halted ! ");
        StrCpy (ErrorMsgBuf2, L" Press CTRL+ALT+DEL to reset Machine");
        CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, ErrorMsgBuf1, ErrorMsgBuf2, NULL);
        gBS->WaitForEvent ( 1, gST->ConIn->WaitForKey, &Index );
        gST->ConIn->ReadKeyStroke ( gST->ConIn, &Key );
      } while (TRUE);
    }
  }

  FreePool (ErrorMsgBuf1);
  FreePool (ErrorMsgBuf2);
  gBS->CloseEvent (Event);

  //Restore back to TPL_CALLBACK
  gBS->RaiseTPL(TPL_CALLBACK);
}

EFI_STATUS
ReadStrings (
  IN CHAR16      *StringPtr
)
{
  UINTN                            Index;
  UINTN                            Count;
  CHAR16                           *TempString;
  CHAR16                           KeyPad[2];
  UINTN                            Maximum;
  EFI_INPUT_KEY                    Key;
  EFI_STATUS                       Status;
  CHAR16                           *BufferedString;

  Maximum = PASSWORD_MAX_SIZE;

  BufferedString = AllocateZeroPool ( (Maximum + 1) * sizeof (CHAR16) );
  ASSERT (BufferedString != NULL);
  TempString = AllocateZeroPool ( (Maximum + 1) * sizeof (CHAR16) );
  ASSERT (TempString != NULL);

  if(BufferedString == NULL) {
	DEBUG ((EFI_D_ERROR, "\nERROR!! ReadStrings() - BufferedString pointer is NULL\n"));
	return EFI_INVALID_PARAMETER;
  }

  if(TempString == NULL) {
	FreePool (BufferedString);
	DEBUG ((EFI_D_ERROR, "\nERROR!! ReadStrings() - TempString pointer is NULL\n"));
	return EFI_INVALID_PARAMETER;
  }

  do {

    Status = ReadKeyStrokes ( &Key);
    ASSERT_EFI_ERROR (Status);

    switch (Key.UnicodeChar) {
    case CHAR_NULL:
      switch (Key.ScanCode) {
      case SCAN_LEFT:
        break;

      case SCAN_RIGHT:
        break;

      case SCAN_ESC:
        FreePool (TempString);
        FreePool (BufferedString);
        return EFI_DEVICE_ERROR;
        break;

      default:
        break;
      }

      break;

    case CHAR_CARRIAGE_RETURN:
      FreePool (TempString);
      FreePool (BufferedString);
      return EFI_SUCCESS;

      break;

    case CHAR_BACKSPACE:
      if (StringPtr[0] != CHAR_NULL) {
        for (Index = 0; StringPtr[Index] != CHAR_NULL; Index++) {
          TempString[Index] = StringPtr[Index];
        }
        //
        // Effectively truncate string by 1 character
        //
        TempString[Index - 1] = CHAR_NULL;
        StrCpy (StringPtr, TempString);
      }

    default:
      //
      // If it is the beginning of the string, don't worry about checking maximum limits
      //
      if ( (StringPtr[0] == CHAR_NULL) && (Key.UnicodeChar != CHAR_BACKSPACE) ) {
        StrnCpy (StringPtr, &Key.UnicodeChar, 1);
        StrnCpy (TempString, &Key.UnicodeChar, 1);
      } else if ( ( (StrLen (StringPtr) ) < Maximum) && (Key.UnicodeChar != CHAR_BACKSPACE) ) {

        KeyPad[0] = Key.UnicodeChar;
        KeyPad[1] = CHAR_NULL;
        StrCat (StringPtr, KeyPad);
        StrCat (TempString, KeyPad);
      }

      for (Count = 0; Count < Maximum; Count++) {
        BufferedString[Count] = L' ';
      }
      gST->ConOut->SetCursorPosition (gST->ConOut, gColumn, gRow);

      for (Count = 0; Count < StrLen (StringPtr); Count++) {
        BufferedString[Count] = L'*';
      }
      gST->ConOut->OutputString (gST->ConOut, BufferedString);
      break;
    }


    gST->ConOut->SetCursorPosition (gST->ConOut, gColumn + StrLen (StringPtr), gRow);
  } while (TRUE);

  return Status;

}


EFI_STATUS
ReadKeyStrokes (
  OUT  EFI_INPUT_KEY            *Key
)
{
  EFI_STATUS                    Status;
  EFI_EVENT                     TimerEvent;
  EFI_EVENT                     WaitList[2];
  UINTN                         Index;

  do {
    do {
      Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);

      //
      // Set a timer event of 1 second expiration
      //
      gBS->SetTimer (
        TimerEvent,
        TimerRelative,
        ONE_SECOND
      );

      //
      // Wait for the keystroke event or the timer
      //
      WaitList[0] = gST->ConIn->WaitForKey;
      WaitList[1] = TimerEvent;
      Status      = gBS->WaitForEvent (2, WaitList, &Index);

      //
      // Check for the timer expiration
      //
      if (!EFI_ERROR (Status) && Index == 1) {
        Status = EFI_TIMEOUT;
      }

      gBS->CloseEvent (TimerEvent);
    } while (Status == EFI_TIMEOUT);

    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
  } while (EFI_ERROR (Status) );
  return Status;
}

VOID
EFIAPI
DrawPopUp (
)
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE      SavedConsoleMode;
  UINTN                            Columns;
  UINTN                            Rows;
  UINTN                            Column;
  UINTN                            Row;
  UINTN                            NumberOfLines;
  UINTN                            MaxLength;
  UINTN                            Length;
  CHAR16                           *Line;
  CHAR16                           *Line1;
  CHAR16                           *StringBuffer;


  MaxLength     = 44;
  NumberOfLines = 3;

  //
  // If the total number of lines in the popup is zero, then ASSERT()
  //
  ASSERT (NumberOfLines != 0);

  //
  // If the maximum length of all the strings is zero, then ASSERT()
  //
  ASSERT (MaxLength != 0);

  //
  // Cache a pointer to the Simple Text Output Protocol in the EFI System Table
  //
  ConOut = gST->ConOut;

  //
  // Save the current console cursor position and attributes
  //
  CopyMem (&SavedConsoleMode, ConOut->Mode, sizeof (SavedConsoleMode) );

  //
  // Retrieve the number of columns and rows in the current console mode
  //
  ConOut->QueryMode (ConOut, SavedConsoleMode.Mode, &Columns, &Rows);

  ConOut->ClearScreen (gST->ConOut);

  //
  // Disable cursor and set the foreground and background colors specified by Attribute
  //
  ConOut->EnableCursor (ConOut, TRUE);
  ConOut->SetAttribute (ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE);

  //
  // Compute the starting row and starting column for the popup
  //
  Row    = (Rows - (NumberOfLines + 3) ) / 2;
  Column = (Columns - (MaxLength + 2) ) / 2;


  //
  // Allocate a buffer for a single line of the popup with borders and a Null-terminator
  //
  Line = AllocateZeroPool ( (MaxLength + 3) * sizeof (CHAR16) );
  ASSERT (Line != NULL);

  if(Line == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR!! DrawPopUp() - Line pointer is NULL\n"));
    return ;
  }
  //
  // Draw top of popup box
  //
  SetMem16 (Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition (ConOut, Column, Row++);
  ConOut->OutputString (ConOut, Line);


  // Draw middle of the popup with strings
  StringBuffer = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16) );
  ASSERT(StringBuffer!=NULL);
  
  if(StringBuffer ==NULL)
  {
      return;
  }

  StrCpy (StringBuffer, L"Enter Password ");
  Length = StrLen (StringBuffer);


  SetMem16 (Line, (MaxLength + 2) * 2, L' ' );
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  CopyMem (Line + 1 + (MaxLength - Length) / 2, StringBuffer , Length * sizeof (CHAR16) );
  ConOut->SetCursorPosition (ConOut, Column, Row++);
  ConOut->OutputString (ConOut, Line);

  //draw vertical lines for popup

  while ( NumberOfLines > 0) {
    SetMem16 (Line, (MaxLength + 2) * 2, L' ');
    Line[0]             = BOXDRAW_VERTICAL;
    Line[MaxLength + 1] = BOXDRAW_VERTICAL;
    Line[MaxLength + 2] = L'\0';
    ConOut->SetCursorPosition (ConOut, Column, Row++);
    ConOut->OutputString (ConOut, Line);
    NumberOfLines--;
  }

  //
  // Draw bottom of popup box
  //
  SetMem16 (Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition (ConOut, Column, Row++);
  ConOut->OutputString (ConOut, Line);

  //
  // Free the allocated line buffer
  //
  FreePool (Line);

  Line1 = AllocateZeroPool ( (PASSWORD_MAX_SIZE + 3) * sizeof (CHAR16) );
  ASSERT (Line1 != NULL);

  if(Line1 == NULL) {
	FreePool (StringBuffer);
    DEBUG ((EFI_D_ERROR, "\nERROR!! DrawPopUp() - Line1 pointer is NULL\n"));
    return ;
  }

  ConOut->SetAttribute (ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //these are for inside popup
  Row = Row - 3;
  Column = Column + 5;

  gRow = Row;
  gColumn = Column;


  //Draw top of inside popup box
  SetMem16 (Line1, (PASSWORD_MAX_SIZE + 2) * 2, L' ');
  Line1[0]              = L' ';
  Line1[PASSWORD_MAX_SIZE + 1] = L' ';
  Line[PASSWORD_MAX_SIZE + 2]  = L'\0';
  ConOut->SetCursorPosition (ConOut, Column, Row);
  ConOut->OutputString (ConOut, Line1);

  //place cursor back at beginning of password input area
  ConOut->SetCursorPosition (ConOut, Column, Row);

  ConOut->EnableCursor (ConOut, TRUE);

  FreePool (Line1);
  FreePool (StringBuffer);

  //
  // Restore the cursor visibility, position, and attributes
  //
  ConOut->EnableCursor      (ConOut, SavedConsoleMode.CursorVisible);
  ConOut->SetAttribute      (ConOut, SavedConsoleMode.Attribute);


}
EFI_STATUS
PromptForPassword (
  VOID
)
{
  EFI_STATUS                      Status;
  CHAR16                          *Password;
  UINTN                           VarSize;
  UINT8                           AccessLevel;
  SYSTEM_CONFIGURATION            SetupData;
  UINT8                           AdminDigest[SHA256_DIGEST_SIZE];
  UINT8                           UserDigest[SHA256_DIGEST_SIZE];

  AccessLevel  = SYSTEM_PASSWORD_ADMIN ;
  Status       = AUTHENTICATION_NOT_PASSED;
  VarSize      = sizeof (SYSTEM_CONFIGURATION);
  Status       = gRT->GetVariable (L"Setup", &mSystemConfigurationGuid, NULL, &VarSize, &SetupData);
  //
  // Draw Popup for password prompt
  //
  DrawPopUp ();
  Password = AllocateZeroPool ( (PASSWORD_MAX_SIZE + 1) * sizeof (CHAR16) );
  ASSERT(Password!=NULL);
  if(Password ==NULL)
  {
      return EFI_OUT_OF_RESOURCES;
  }

  Status  = ReadStrings (Password);

  if (*Password == L' ' || Status != EFI_SUCCESS) {
    Status = AUTHENTICATION_NOT_PASSED;
  }

  EncodePassword (Password, AdminDigest, FALSE, SetupData.AdminPasswordSalt);
  EncodePassword (Password, UserDigest, FALSE, SetupData.AdminPasswordSalt);

  switch (gPwFlag) {

  case VALIDATE_ADMIN:
    if (CompareMem (AdminDigest, SetupData.AdminPassword, SHA256_DIGEST_SIZE) == 0) {
      Status = AUTHENTICATION_PASSED;
      AccessLevel = SYSTEM_PASSWORD_ADMIN;

    } else {
      Status = AUTHENTICATION_NOT_PASSED;
    }

    break;
  case VALIDATE_USER:
    if (CompareMem (UserDigest, SetupData.UserPassword, SHA256_DIGEST_SIZE) == 0) {
      Status = AUTHENTICATION_PASSED;
      AccessLevel = SYSTEM_PASSWORD_USER;
    } else {
      Status = AUTHENTICATION_NOT_PASSED;
    }

    break;
  case VALIDATE_BOTH:
    if (CompareMem (AdminDigest, SetupData.AdminPassword, SHA256_DIGEST_SIZE) == 0) {
      Status = AUTHENTICATION_PASSED;
      AccessLevel = SYSTEM_PASSWORD_ADMIN;
    } else if (CompareMem (UserDigest, SetupData.UserPassword, SHA256_DIGEST_SIZE) == 0) {
      Status = AUTHENTICATION_PASSED;
      AccessLevel = SYSTEM_PASSWORD_USER;
    } else {
      Status = AUTHENTICATION_NOT_PASSED;
    }

    break;
  }
  if (Status == AUTHENTICATION_PASSED ) {
    SetPasswordType ( AccessLevel );
  }

  FreePool (Password);
  return Status;
}


UINT8
CheckIfPasswordInstalled (
  VOID
)
{
  UINT8                   State;
  UINTN                   VarSize;
  EFI_STATUS              Status;
  SYSTEM_CONFIGURATION    SetupData;

  gPwFlag            = VALIDATE_NONE;
  VarSize           = sizeof (SYSTEM_CONFIGURATION);
  Status  = gRT->GetVariable (
              L"Setup",
              &mSystemConfigurationGuid,
              NULL,
              &VarSize,
              &SetupData
            );
  if (!EFI_ERROR (Status) ) {
    //
    // User trying to enter setup , Validate the password here if it is set
    //
    // Verify if Admin Password is Installed
    State = (UINT8) (PasswordSet(SetupData.AdminPassword) ? PW_SET : PW_NOT_SET);
    if (State == PW_SET ) {
      //
      // Admin P/W installed
      //
      gPwFlag |= VALIDATE_ADMIN;
    }
    State = 0;
    State = (UINT8) (PasswordSet(SetupData.UserPassword) ? PW_SET : PW_NOT_SET);
    if (State == PW_SET ) {
      //
      // Yes User P/W installed
      //
      gPwFlag |= VALIDATE_USER;
    }
  }

  return gPwFlag;
}

BOOLEAN
IsPasswordCheck (
  IN  BOOLEAN         Check,
  IN  CHAR16          Key
)
{
  if (Check) {
    if (CharIsAlphaNumeric (Key) ) {
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return TRUE;
  }
}

VOID
SetPasswordType (
  IN UINT8         Type
)
{

  SYSTEM_CONFIGURATION    SetupData;
  UINTN                   VariableSize;
  EFI_STATUS              Status;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->GetVariable (
             L"Setup",
             &gSetupVariableGuid,
             NULL,
             &VariableSize,
             &SetupData
           );
  ASSERT_EFI_ERROR (Status);

  SetupData.Access = Type;
  Status = gRT->SetVariable (
             L"Setup",
             &gSetupVariableGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
             VariableSize,
             &SetupData
           );
  ASSERT_EFI_ERROR (Status);

  return;
}
*/
// Denverton AptioV Override End - EIP#217439
