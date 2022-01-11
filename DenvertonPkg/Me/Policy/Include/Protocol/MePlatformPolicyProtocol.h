/** @file
  Interface definition details between ME and platform drivers during DXE phase.

@copyright
  Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _ME_PLATFORM_POLICY_PROTOCOL_H_
#define _ME_PLATFORM_POLICY_PROTOCOL_H_

typedef struct _ME_POLICY_PROTOCOL DXE_ME_POLICY_PROTOCOL;

#include <Library/MePolicyDebugDumpDxe.h>

//
// ME policy provided by platform for DXE phase
// This protocol provides an interface to get Intel ME Configuration information
//
#ifdef ECP_FLAG
#define DXE_PLATFORM_ME_POLICY_GUID \
  { \
    0x903cc4d4, 0xd77e, 0x4eb9, 0xb5, 0x53, 0x47, 0xfd, 0x2e, 0x59, 0x1c, 0xdf \
  }
#endif

//
// Initial Revision
//
#define DXE_PLATFORM_ME_POLICY_PROTOCOL_REVISION_1  1

#pragma pack(1)
#ifdef AMT_SUPPORT

#define AMT_BITMASK   0x00000004
#define NFC_BITMASK   0x80000000

#define CLEAR_FEATURE_BIT 0x00000000

typedef struct {
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT8     MeFwDownGrade       : 1;      // 0: Disabled; 1: Enabled
  UINT8     MeLocalFwUpdEnabled : 1;      // 0: Disabled; 1: Enabled
  UINT8     MbpSecurity         : 1;      // 0: Disabled; 1: Enabled
  UINT8     MdesCapability      : 1;      // 0: Disabled; 1: Enabled
  UINT8     SvtForPchCap        : 1;      // 0: Disabled; 1: Enabled
  UINT8     MdesForBiosState    : 1;      // 0: Disabled; 1: Enabled
  UINT8     MeStateControl      : 1;      // 0: Disabled; 1: Enabled
  UINT8     RemoteSessionActive : 1;      // 0: Disabled; 1: Enabled
  //
  // Byte 1, bit definition for functionality enable/disable
  //
  UINT8     CoreBiosDoneEnabled    : 1;  ///< 0: Disabled; <b>1: Enabled</b>
  UINT8     Byte1Reserved          : 7;

  UINT8     PreviousMeStateControl : 1;   // byte 2
  UINT8     Byte2Reserved          : 7;
  UINT8     PreviousLocalFwUpdEnabled;    // byte 3

  UINT32    MeMajor;                      // byte 4 - 7
  UINT32    MeMinor;                      // byte 8 - 11
  UINT32    MeHotFix;                     // byte 12- 15
  UINT32    MeBuildNo;                    // byte 16- 19
  UINT32    MeFileSystem;                 // byte 20- 23
  UINT8     MeImageType;                  // byte 24

  //
  // Byte 25, bit definition for functionality enable/disable
  //
  UINT8     AsfEnabled         : 1;       //< 0: Disabled; 1: Enabled
  UINT8     AmtEnabled         : 1;       //< 0: Disabled; 1: Enabled
  UINT8     AmtbxPasswordWrite : 1;       //< 0: Disabled; 1: Enabled
  UINT8     WatchDog           : 1;       //< 0: Disabled; 1: Enabled
  UINT8     CiraRequest        : 1;       //< 0: No CIRA request; 1: Trigger CIRA request
  UINT8     ManageabilityMode  : 1;       //< 0: Disabled; 1:AMT
  UINT8     UnConfigureMe      : 1;       //< 0: No; 1: Un-configure ME without password
  UINT8     MebxDebugMsg       : 1;       //< 0: Disabled; 1: Enabled
  //
  // Byte 26, bit definition for functionality enable/disable
  //
  UINT8     ForcMebxSyncUp           : 1; //< 0: No; 1: Force MEBX execution
  UINT8     UsbrEnabled              : 1; //< 0: Disabled;  1: Enabled
  UINT8     UsbLockingEnabled        : 1; //< 0: Disabled;  1: Enabled
  UINT8     HideUnConfigureMeConfirm : 1; //< 0: Don't hide; 1: Hide Un-configure ME Confirmation Prompt
  UINT8     USBProvision             : 1; //< 0: Disabled; 1: Enabled
  UINT8     FWProgress               : 1; //< 0: Disabled; 1: Enabled
  UINT8     AmtbxHotkeyPressed       : 1; //< 0: Disabled; 1: Enabled
  UINT8     AmtbxSelectionScreen     : 1; //< 0: Disabled; 1: Enabled
  //
  // Byte 27, bit definition for functionality enable/disable
  //
  UINT8     AtEnabled      : 1;           //< 0: Disabled; 1: Enabled
  UINT8     Byte27Reserved : 6;
  UINT8     WatchDogOs     : 1;           //< 0: Disabled; 1: Enabled
  UINT16    WatchDogTimerOs;              //< Byte 28-29 OS WatchDog Timer
  UINT16    WatchDogTimerBios;            //< Byte 30-31 BIOS WatchDog Timer
  //
  // Byte 32 CIRA Timeout: Client Initiated Remote Access Timeout
  //                      OEM defined timeout for MPS connection to be established.
  // 0: 60 seconds by default, 0xFF: Wait until the connection succeeds.
  // 1~0xFE: Values in seconds
  //
  UINT8     CiraTimeout;

  //
  // Byte 33 CPU Replacement Timeout
  // 0: 10 seconds; 1: 30 seconds; 2~5: Reserved; 6: No delay; 7: Unlimited delay
  //
  UINT8     CpuReplacementTimeout;

  //
  // Byte 34-36 OemResolutionSettings
  //
  UINT16    MebxNonUiTextMode : 4;        //< 0: Auto; 1: 80x25; 2: 100x31
  UINT16    MebxUiTextMode : 4;           //< 0: Auto; 1: 80x25; 2: 100x31
  UINT16    MebxGraphicsMode : 4;         //< 0: Auto; 1: 640x480; 2: 800x600; 3: 1024x768
  UINT16    OemResolutionSettingsRsvd : 4;

  //
  // Byte 37-40 Pointer to a list which contain on-board devices bus/device/fun number
  //
  UINT32    PciDeviceFilterOutTable;

  //
  // Byte 41-49 Reserved and make AMT_CONFIG as 32 bit alignment
  //
#ifdef TESTMENU_FLAG
  UINT8     MebxLaunchTimeout;            //< 1: 1 minutes; 5: 5 minutes; 10: 10 minutes; others: No delay
  UINT8     ByteReserved[8];
#else
  UINT8     ByteReserved[9];
#endif
} ME_AMT_CONFIG;
#endif // AMT_SUPPORT

typedef struct {
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT8     EndOfPostEnabled      : 1;  // 0: Disabled; 1: Enabled
  UINT8     EndOfPostDone         : 1;  // 0: Not yet;  1: Done
  UINT8     SpsMeShutdownEnabled  : 1;  // 0: Disabled; 1: Enabled
  UINT8     Byte0Reserved         : 5;
  //
  // Byte 1, bit definition for functionality enable/disable
  //
  UINT8     Byte1Reserved       : 8;

  //
  // Byte 2, bit definition for functionality enable/disable
  //
  UINT8     HeciCommunication1       : 2;    // 0: Disabled; 1: Enabled; 2: Auto 3: Hide
  UINT8     HeciCommunication2       : 2;    // 0: Disabled; 1: Enabled; 2: Auto 3: Hide
  UINT8     MeIderEnabled            : 1;
  UINT8     MeKtEnabled              : 1;
  UINT8     MeGrPromotionEnabled     : 1;

  UINT8     MeFirmwareMode;           // byte 3
  UINT8     HeciCommunication3       : 2;    // 0: Disabled; 1: Enabled; 2: Auto; 3: Hide
  UINT8     Heci1HideInMe            : 2;    // 0: do not set, 1 - hide, 2 disable
  UINT8     Heci2HideInMe            : 2;
  UINT8     Heci3HideInMe            : 2;
  UINT8     BytesReservedN[14];       // Byte 5-19 Reserved for other bit definitions in the future
} ME_CONFIG;
#pragma pack()

//
// ME ERROR Message ID
//
typedef enum {
  MSG_EOP_ERROR             = 0,
  MSG_ME_FW_UPDATE_FAILED,
  MSG_ASF_BOOT_DISK_MISSING,
  MSG_KVM_TIMES_UP,
  MSG_KVM_REJECTED,
  MSG_HMRFPO_LOCK_FAILURE,
  MSG_HMRFPO_UNLOCK_FAILURE,
  MSG_ME_FW_UPDATE_WAIT,
  MSG_ILLEGAL_CPU_PLUGGED_IN,
  MSG_KVM_WAIT,
  MSG_PLAT_DISABLE_WAIT,
  MAX_ERROR_ENUM
} ME_ERROR_MSG_ID;

/**
  Show Me Error message. This is to display localized message in
  the console. This is used to display message strings in local
  language. To display the message, the routine will check the
  message ID and ConOut the message strings. For example, the
  End of Post error displayed in English will be:
  gST->ConOut->OutputString (gST->ConOut, L"Error sending End Of
  Post message to ME\n"); It is recommended to clear the screen
  before displaying the error message and keep the message on
  the screen for several seconds.
  A sample is provided, see ShowMeReportError () to retrieve
  details.

  @param[in] MsgId                Me error message ID for displaying on screen message

**/
typedef
VOID
(EFIAPI *ME_REPORT_ERROR) (
  IN ME_ERROR_MSG_ID              MsgId
  );

#pragma pack(1)
typedef struct {
  //
  // This member determines the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS
  // Type 14 - Group Associations structure - item type. FVI structure uses it as
  // SMBIOS OEM type to provide MEBx, ME FW and reference code version information
  //
  UINT8 FviSmbiosType;
  UINT8 ByteReserved[15];         //< Byte 1-15 Reserved for other bit definitions in future
} ME_MISC_CONFIG;
#pragma pack()

#pragma pack(1)
#ifdef SPS_SUPPORT
typedef struct {
  UINT8   NmPwrOptBoot;
  UINT8   NmPwrOptBootOverride;
  UINT8   NmCores2Disable;
  UINT8   NmCores2DisableOverride;
  UINT8   NmPowerMsmtSupport;
  UINT8   NmHwChangeStatus;
  UINT32  SpsAltitude;
  UINT16  SpsMctpBusOwner;
  UINT8   MeHmrfpoEnableEnabled;
  UINT8   MeHmrfpoLockEnabled;
  UINT8   MeGrLockEnabled;
  UINT8   TurboMode;
  UINT8   TStateEnable; 
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
  UINT8   NmPtuLoadOverride;        // For MROM-less systems only
  UINT8   ByteReserved[12];         //< Byte 1-13 Reserved for other bit definitions in future
#else
  UINT8   ByteReserved[13];         //< Byte 1-13 Reserved for other bit definitions in future
#endif
} ME_NM_CONFIG;
#endif // SPS_SUPPORT
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *EFI_SAVE_ME_POLICY_TO_SETUP_CONFIGURATION) (
   DXE_ME_POLICY_PROTOCOL *This
  )
;

typedef
EFI_STATUS
(EFIAPI *UPDATE_ME_POLICY_FROM_SETUP) (
  DXE_ME_POLICY_PROTOCOL  *MePlatformPolicyInstance,
  VOID                    *mSetupData,
  BOOLEAN                 *resetRequest
  )
;

typedef
VOID
(EFIAPI *ME_POLICY_DEBUG_DUMP) (
  DXE_ME_POLICY_PROTOCOL *this,
  ME_POLICY_DUMP_TYPE    WhatToDump,
  UINTN                  DebugLevel
  )
;
//
// ME DXE Platform Policy
// This protocol provides information of the current Intel ME feature selection. Information is
// passed from the platform code to the Intel ME Reference code using this structure. There are
// 2 types of information, BIOS setup option and ME status information.
//
typedef struct _ME_POLICY_PROTOCOL {
  //
  // Revision for the protocol structure
  //
  UINT8                           Revision;
  UINT8                           MeType;
  //
  // Intel ME feature selection enable/disable and firmware configuration information
  //
  ME_CONFIG                       MeConfig;
#ifdef SPS_SUPPORT
  //
  // Intel ME Node Manager feature selection enable/disable
  //
  ME_NM_CONFIG                    MeNmConfig;
#else
  UINT8     BytesReserved1[29];
#endif // SPS_SUPPORT
#ifdef AMT_SUPPORT
  //
  // Intel ME AMT feature selection
  //
  ME_AMT_CONFIG                   MeAmtConfig;
#else
  UINT8     BytesReserved2[50];
#endif // AMT_SUPPORT
  //
  // Miscellaneous items
  //
  ME_MISC_CONFIG                  MeMiscConfig;
  //
  // Variables required to Manage changes in Setup
  //
  BOOLEAN                         AnythingChanged; // any change after After EndOfPost requires reset

  //
  // Protocol functions
  //

  // Save ME Policy configuration to System Setup
  EFI_SAVE_ME_POLICY_TO_SETUP_CONFIGURATION  saveMePolicyToSetupConfiguration;

  // Update ME Policy from setup e.g. after configuration change in UI
  UPDATE_ME_POLICY_FROM_SETUP                UpdateMePolicyFromSetup;

  // Support Localization for displaying on screen message
  ME_REPORT_ERROR                            MeReportError;
  BOOLEAN (*EnableRedirection) (BOOLEAN Enable);
  // Dump ME Policy
  ME_POLICY_DEBUG_DUMP                      MePolicyDebugDump;
} DXE_ME_POLICY_PROTOCOL;

extern EFI_GUID gDxePlatformMePolicyProtocolGuid;

#endif
