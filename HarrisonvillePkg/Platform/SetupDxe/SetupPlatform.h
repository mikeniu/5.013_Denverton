//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SetupPlatform.h

Abstract:

  Header file for Platform Initialization Driver.

Revision History:

--*/

#ifndef _SETUP_PLATFORM_H_
#define _SETUP_PLATFORM_H_

#include <FrameworkDxe.h>

#include "Platform.h"
#include "Configuration.h"
#include "SetupDxeStrDefs.h"
#include "Flashmap.h"
//#include <SouthCluster/PchInit/Dxe/PchInit.h>

#include <Library/PcdLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>

#include <Protocol/DataHub.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
// Denverton AptioV Override Start - EIP#217439
#include <Protocol/HiiPackageList.h>
// Denverton AptioV Override End - EIP#217439
#include <Protocol/IdeControllerInit.h>

#include <Protocol/PlatformType.h>
//#include <Protocol/TcgService.h> // Denverton AptioV override - EIP262802
#include <Protocol/CpuIo2.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/IsaIo.h>
#include <Protocol/Ps2Policy.h>
#include <Protocol/PlatformPolicy.h>
#include <Protocol/EfiLegacyBootMarker.h>


#include <Protocol/UncorePlatformPolicy.h>

#include <Guid/CpuHtCapable.h>
#include <Guid/DataHubRecords.h>
#include <Guid/HobList.h>

#include <Guid/MemorySetupData.h>
#include <Include/SetupVariable.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Guid/MemoryConfigData.h>

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
#include <Library/MeSetupLib.h>
#endif // ME_SUPPORT_FLAG

#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <PchAccess.h>
#include <PcieRegs.h>
#include "Protocol/PciIo.h"
#include <Library/BiosIdLib.h>
#include <Library/PchInfoLib.h>
#include "SaRegs.h"

#define AMITSE_SETUP_ENTER_GUID \
  { 0x71202EEE, 0x5F53, 0x40d9, 0xAB, 0x3D, 0x9E, 0x0C, 0x26, 0xD9, 0x66, 0x57 }

// EDK_TODO
#define EFI_CPUID_VERSION_INFO      0x1


#define DXE_DEVICE_ENABLED  1
#define DXE_DEVICE_DISABLED 0

#define KBC_DATA_PORT                   0x60
#define KBC_CMD_STS_PORT                0x64
#define ONE_SECOND                      10000000
#define VALIDATE_NONE                   0x00
#define VALIDATE_ADMIN                  0x01
#define VALIDATE_USER                   0x02
#define VALIDATE_BOTH                   0x03

#define AUTHENTICATION_PASSED           0x00
#define AUTHENTICATION_NOT_PASSED       0xFF

#define PW_SET                          0x01
#define PW_NOT_SET                      0x00

#define SIZE_STRING_LEN                 0x200
#define SIZE_STRING_PARAMETER           0x14

#define MAX_STRING_LEN                  0x100
#define CharIsUpper(c)                  ((c >= L'A') && (c <= L'Z'))
#define CharIsLower(c)                  ((c >= L'a') && (c <= L'z'))
#define CharIsAlpha(c)                  (CharIsUpper(c) || CharIsLower(c))
#define CharIsNumeric(c)                ((c >= L'0') && (c <= L'9'))
#define CharIsAlphaNumeric(c)           (CharIsAlpha(c) || CharIsNumeric(c))

typedef EFI_STATUS  STATUS;

typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT16  Handle;
} SMBIOS_HEADER;

typedef struct {
  UINT32  Id;
  UINT32  Freq;
  UINT32  Ucode;
  UINT32  L1Cache;
  UINT32  L2Cache;
  UINT32  L3Cache;
  CHAR16  *Version;
} CPU_SETUP_INFO;

#pragma pack(1)

typedef struct _SB_PCIE_BRIDGE
{
	UINT8	Bus;
	UINT8	Dev;
	UINT8	Fun;
} SB_PCIE_BRIDGE;

#pragma pack()

typedef enum {
  EfiUserPassword,
  EfiAdminPassword
} EFI_PASSWORD_TYPE;

typedef struct {
  CHAR16            TempPassword[PASSWORD_MAX_SIZE + 1];
  VOID              *PasswordHash;
  VOID              *Salt;
  EFI_PASSWORD_TYPE PasswordType;
} EFI_PASSWORD_DATA;

///
/// HII specific Vendor Device Path Node definition.
///
#pragma pack(1)

typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  UINT16                         UniqueId;
} HII_VENDOR_DEVICE_PATH_NODE;

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  HII_VENDOR_DEVICE_PATH_NODE    Node;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()

#define SETUP_DATA_HOTPLUG_AUTO		3
#define SETUP_DATA_RESETDELAY_20S	1

VOID
UpdatePcds(
  VOID
);

#define PCIE_BRIDGE_VID_DID     0x88008086
#define PACKET_HUB_VID_DID      0x88018086

#define PCI_REG_PCICMD          0x04    // PCI Command Register


//
// Prototypes
//
EFI_STATUS
EfiMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
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
;

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
;

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
;

// Denverton AptioV Override Start - EIP#217439
EFI_STATUS
ParseIfrPackForIntelRcSetup(
IN EFI_HANDLE        ImageHandle
);
// Denverton AptioV Override End - EIP#217439
VOID
EFIAPI
InstallHiiDataAndGetSettings (
  IN EFI_HII_PACKAGE_HEADER           *StringPack,
  ...
  );

/*++

Routine Description:

  Installs a string and ifr pack set.

Arguments:

  Hii         - pointer to an HII database protocol structure.
  StringPack  -  string pack to store in database and associate with IfrPack.
  IrfPack     - ifr pack to store in database (it will use StringPack data).

Returns:

  None.

--*/
;

EFI_STATUS
GetStringFromToken (
  IN      EFI_GUID                  *ProducerGuid,
  IN      STRING_REF                Token,
  OUT     CHAR16                    **String
  )
/*++

Routine Description:

  Acquire the string associated with the ProducerGuid and return it.

Arguments:

  ProducerGuid  -  The Guid to search the HII database for.
  Token         -  The token value of the string to extract.
  String        -  The string that is extracted.

Returns:

  EFI_SUCCESS  -  Must return success on completion.

--*/
;

VOID
EFIAPI
SetupDataFilter (
  IN EFI_EVENT      Event,
  IN VOID           *Context
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
;


VOID
SwapEntries (
  IN  CHAR8 *Data,
  IN  UINT16 Size
  )
/*++

Routine Description:

  Swap order of nearby CHAR8 data.

Arguments:

  Data  -  The data to swap.

Returns:

  None.

--*/
;

VOID
AsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString
  )
/*++

Routine Description:

  Converts an ascii string to unicode string 16 chars at a time.

Arguments:

  AsciiString    -  Address of Ascii string.
  UnicodeString  -  Address of Unicode String.

Returns:

  None.

--*/
;

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

  EFI_SUCCESS  -  Must return success on completion.

--*/
;

EFI_STATUS
EFIAPI
SecurityDriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  );


EFI_STATUS
DisplayPlatformInfo (
  EFI_PLATFORM_TYPE_PROTOCOL            *mPlatformType
  )
/*++

Routine Description:

  Displays the Platform, NorthBridge and SouthBridge info in the Setup.

Arguments:

  mPlatformType.

Returns:

  EFI_SUCCESS  -  Must complete successfully.

--*/
;


EFI_STATUS
SetPasswordState (
  IN  EFI_PASSWORD_DATA               *SecurityData
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SecurityData  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

VOID
EncodePassword (
  IN     CHAR16                      *Password,
  OUT    UINT8                       *Digest,
  IN     BOOLEAN                     GenerateNewSalt,
  IN OUT UINT8                       *Salt
  )
/*++

Routine Description:

  Encode password passed in using SHA-256

Arguments:

  Password         -  The password to encode.
  Digest           -  The 32 byte digest from SHA-256
  GenerateNewSalt  -  If TRUE, a new Salt is generated, if FALSE, existing Salt is used
  Salt             -  The salt value used for the encoded password

Returns:

  None.

--*/
;

BOOLEAN
PasswordSet (
  IN  UINT8                      *Hash
  )
/*++

Routine Description:

  Takes in pointer to 32 byte hash.  If hash is all zeros (unlikely to happen 'accidentally'), return FALSE, else return TRUE.

Arguments:

  Hash  - Pointer to 32 byte UINT8

Returns:

  TRUE  - If any of the bytes pointed to are not zero
  FALSE - If all bytes pointed to are zeros

--*/
;

VOID
UpdateUsbData (
   VOID
   );

VOID
SetAdminIndicator (
  IN BOOLEAN                      State
  );

VOID
SetUserIndicator (
  IN BOOLEAN                      State
  );

EFI_STATUS
EFIAPI
DefaultExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
DefaultRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

EFI_STATUS
InstallDefaultCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  );

EFI_STATUS
InstallAdvancedCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  );

EFI_STATUS
InstallSecurityCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  );

EFI_STATUS
InstallTpmCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  );

EFI_STATUS
InstallChipsetCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  );

EFI_STATUS
InstallMainCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  );

VOID
EFIAPI
IdeCallback(
  IN EFI_EVENT              Event,
  IN VOID                   *Context
  );


VOID
EFIAPI
CheckForPassword (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  );

EFI_STATUS
ReadStrings(
  IN CHAR16                   *StringPtr
  );

VOID
EFIAPI
DrawPopUp (
  VOID
  );

EFI_STATUS
ReadKeyStrokes(
  EFI_INPUT_KEY                 *Key
 );

UINT8
CheckIfPasswordInstalled(
  VOID
  );

BOOLEAN
IsPasswordCheck (
  IN BOOLEAN                     Check,
  IN CHAR16                      Key
  );

EFI_STATUS
PromptForPassword(
  VOID
  );

VOID
SetPasswordType (
  UINT8                          Type
  );

VOID
InitSysInfoStrings (
  EFI_HII_HANDLE                HiiHandle
  );

VOID
PcieLinksInfo (
  IN  UINT8                     PortIndex
  );

VOID
SataDevicesInfo (
  IN  UINTN  SataDeviceNumber,
  IN  UINT8  PortIndex
  );

VOID
DisplayMemTopology (
  VOID
  );

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
VOID
InitMeSetupStrings(
VOID
  );

EFI_STATUS
EFIAPI
MeAdvancedRouteConfig (
  IN OUT ME_RC_CONFIGURATION *MeRcConfiguration
);
#endif // ME_SUPPORT_FLAG

#ifdef IE_SUPPORT

VOID
InitIeSetupStrings(
  );

#endif //IE_SUPPORT

//
// Global externs
//
extern UINT8                SetupDxeStrings[];
extern UINT8                MainBin[];
extern UINT8                AdvancedBin[];
extern UINT8                IohSetupBin[];
extern UINT8                PchSetupBin[];
extern UINT8                ExitBin[];
extern UINT8                BootOptionsBin[];
extern UINT8                SysInfoSetupScreenBin[];
extern UINT8                SecurityBin[];
extern UINT8                TpmBin[];
extern UINT8                ChipsetBin[];

extern EFI_HII_DATABASE_PROTOCOL        *mHiiDataBase;
extern EFI_HII_CONFIG_ROUTING_PROTOCOL  *mHiiConfigRouting;

extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mMainConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mAdvancedConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mIohConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mPchConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mEventLogConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mSecurityConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mTpmConfigAccess;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL   mChipsetConfigAccess;

extern SYSTEM_CONFIGURATION mSystemConfiguration;
extern EFI_HII_HANDLE       mMainHiiHandle;
extern EFI_HII_HANDLE       mAdvancedHiiHandle;
extern EFI_HII_HANDLE       mSysInfoHiiHandle;
extern EFI_HII_HANDLE       mSecurityHiiHandle;
extern EFI_HII_HANDLE       mChipsetHiiHandle;

extern EFI_HANDLE           mMainDriverHandle;
extern EFI_HANDLE           mAdvancedDriverHandle;
extern EFI_HANDLE           mTpmDriverHandle;
extern EFI_HANDLE           mSecurityDriverHandle;

extern EFI_GUID             mSystemConfigurationGuid;
extern CHAR16               mVariableName[];
extern EFI_PASSWORD_DATA    mAdminPassword;
extern EFI_PASSWORD_DATA    mUserPassword;
extern EFI_HANDLE           mChipsetDriverHandle;

#endif
