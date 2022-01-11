/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2006 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  HeciMsgLib.h

@brief
  Header file for Heci Message functionality

**/
#ifndef _HECI_MESSAGE_LIB_H_
#define _HECI_MESSAGE_LIB_H_

#include <PiPei.h>
#include <Protocol/HeciProtocol.h>
#include <Library/CoreBiosMsg.h>
#include <Ppi/MePlatformPolicyPei.h>
#include <BupMsgs.h>
#if AMT_SUPPORT
#include <Protocol/PlatformMeHook.h>
#include <MeBiosPayloadData.h>
#endif //AMT_SUPPORT

//
// Reset Request Origin Codes.
//
#define PEI_HECI_REQ_ORIGIN_BIOS_MEMORY_INIT  0x01
#define PEI_HECI_REQ_ORIGIN_BIOS_POST         0x02
#define PEI_HECI_REQ_ORIGIN_AMTBX_LAN_DISABLE 0x03

//
// End of Post Codes
//
#define HECI_EOP_STATUS_SUCCESS       0x0
#define HECI_EOP_PERFORM_GLOBAL_RESET 0x1
#define MAX_EOP_SEND_RETRIES          0x2

//
// Heci Bus Disable defines
//
#define HECI_BUS_DISABLE_OPCODE     0x0C
#define HECI_BUS_DISABLE_ACK_OPCODE 0x8C
#define ME_HECI_FIXED_ADDRESS       0x0

#define EFI_ME_FW_SKU_VARIABLE_GUID \
  { \
    0xe1a21d94, 0x4a20, 0x4e0e, 0xae, 0x9, 0xa9, 0xa2, 0x1f, 0x24, 0xbb, 0x9e \
  }
//
// Defines the HECI request buffer format for the ICC_MPHY_WRITE_SETTINGS_CMD.
//

#pragma pack(1)

typedef struct _ICC_MPHY_WRITE_SETTINGS_REQ {
   ICC_HEADER  Header;           // Standard ICC HECI Header
   UINT32      Reserved    : 4;  // Reserved for future use
   UINT32      PostedWrite : 1;  // 0-Response returned, 1-No Response returned
   UINT32      Reserved2   : 23; // Reserved for future use
} MPHY_WRITE_SETTINGS_REQ;

typedef union _HBM_COMMAND {
  UINT8 Data;
  struct {
    UINT8 Command    : 7;
    UINT8 IsResponse : 1;
  } Fields;
} HBM_COMMAND;

typedef struct _HECI_BUS_DISABLE_CMD {
  HBM_COMMAND Command;
  UINT8       Reserved[3];
} HECI_BUS_DISABLE_CMD;

typedef struct _HECI_BUS_DISABLE_CMD_ACK {
  HBM_COMMAND Command;
  UINT8       Status;
  UINT8       Reserved[2];
} HECI_BUS_DISABLE_CMD_ACK;


typedef struct {
  UINT32  MeEnabled : 1;          // [0]     ME enabled/Disabled
  UINT32  Reserved : 2;           // [2:1]   Reserved, must set to 0
  UINT32  IntelAmtFw : 1;         // [3]     Intel AMT FW support
  UINT32  IntelAmtFwStandard : 1; // [4]     Intel AMT Standard FW support
  UINT32  IntelSmallBusiness : 1; // [5]     Intel Small Business Technology support
  UINT32  Reserved1 : 7;          // [12:6]  Reserved
  UINT32  AtSupported : 1;        // [13]    AT Support
  UINT32  IntelKVM : 1;           // [14]    Intel KVM supported
  UINT32  LocalWakeupTimer : 1;   // [15]    Local Wakeup Timer support
  UINT32  Reserved2 : 16;         // [31:16] Reserved, must set to 0
  UINT32  MeMinorVer : 16;        // [47:32] ME FW Minor Version.
  UINT32  MeMajorVer : 16;        // [63:48] ME FW Major Version.
  UINT32  MeBuildNo : 16;         // [79:64] ME FW Build Number.
  UINT32  MeHotFixNo : 16;        // [95:80] ME FW Hotfix Number
} ME_CAP;

#define MAX_ASSET_TABLE_ALLOCATED_SIZE  0x3000
#define HECI_HWA_CLIENT_ID              11
#define HWA_TABLE_PUSH_CMD              0

typedef enum _HWAI_TABLE_TYPE_INDEX
{
  HWAI_TABLE_TYPE_INDEX_FRU_DEVICE  = 0,
  HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE,
  HWAI_TABLE_TYPE_INDEX_SMBIOS,
  HWAI_TABLE_TYPE_INDEX_ASF,
  HWAI_TABLE_TYPE_INDEX_MAX         = 4,
} HWAI_TABLE_TYPE_INDEX;

typedef struct _SINGLE_TABLE_POSITION {
  UINT16  Offset;
  UINT16  Length;
} SINGLE_TABLE_POSITION;

typedef struct _TABLE_PUSH_DATA {
  SINGLE_TABLE_POSITION Tables[HWAI_TABLE_TYPE_INDEX_MAX];
  UINT8                 TableData[1];
} TABLE_PUSH_DATA;

typedef union {
  UINT32  Data;
  struct {
    UINT32  MessageLength      : 16;
    UINT32  Command            : 4;  // only supported command would be HWA_TABLE_PUSH=0;
    UINT32  FRUTablePresent    : 1;
    UINT32  MediaTablePresent  : 1;
    UINT32  SMBIOSTablePresent : 1;
    UINT32  ASFTablePresent    : 1;
    UINT32  Reserved           : 8;
  } Fields;
} AU_MESSAGE_HEADER;

typedef struct _AU_TABLE_PUSH_MSG {
  AU_MESSAGE_HEADER Header;
  TABLE_PUSH_DATA   Data;
} AU_TABLE_PUSH_MSG;

typedef enum {
  SEV_NO_ERROR                      = 0,
  SEV_LOW_ERROR,
  SEV_HIGH_ERROR,
  SEV_CRITICAL_ERROR
} MDES_SEVERITY_LEVEL;

typedef enum {
  EFI_STATUS_CODE                   = 1,
  RAW_BUFFER,
  ASCII_DATA,
} MDES_BIOS_PAYLOAD_TYPE;

#pragma pack()

/**

  @brief
  Send Hardware Asset Tables to Firmware

  @param[in] Handle               A handle for this module
  @param[in] AssetTableData       Hardware Asset Table Data
  @param[in] TableDataSize        Size of Asset table

  @retval EFI_SUCCESS             Table sent
  @retval EFI_ABORTED             Could not allocate Memory

**/
EFI_STATUS
HeciAssetUpdateFwMsg (
  IN  EFI_HANDLE      Handle,
  IN  TABLE_PUSH_DATA *AssetTableData,
  IN  UINT16          TableDataSize
  )
;

/**

  @brief
  Send Core BIOS Reset Request Message through HECI.

  @param ResetOrigin - Reset source
  @param ResetType   - Global or Host reset

    @retval EFI_STATUS

**/
EFI_STATUS
HeciSendCbmResetRequest (
  IN  UINT8                             ResetOrigin,
  IN  UINT8                             ResetType
  )
;

/**

  @brief
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku            Return Data from Get Firmware Capabilities MKHI Request

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function

**/
EFI_STATUS
HeciGetFwCapsSkuMsg (
  IN OUT MEFWCAPS_SKU             *FwCapsSku
  )
;

/**

  @brief
  Send Get Firmware Version Request to ME

  @param[in] MsgGenGetFwVersionAckData  Return the message of FW version

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
HeciGetFwVersionMsg (
  IN OUT GEN_GET_FW_VER_ACK_DATA      *MsgGenGetFwVersionAckData
  )
;

/**

  @brief
  Sends a message to ME to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE MEI message needs to be sent after all OROMs finish their initialization.

  @param[in] Nonce                Nonce received in previous HMRFPO_ENABLE Response Message
  @param[in] Result               HMRFPO_ENABLE response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciHmrfpoEnable (
  IN  UINT64                          Nonce,
  OUT UINT8                           *Result
  )
;

/**

  @brief
  Send HMRFPO disable MKHI to CSME.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoDisable (
  VOID
  )
;

/**

  @brief
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce               Random number generated by Ignition ME FW. When BIOS
                                  want to unlock region it should use this value
                                  in HMRFPO_ENABLE Request Message
  @param[out] FactoryDefaultBase  The base of the factory default calculated from the start of the ME region.
                                  BIOS sets a Protected Range (PR) register¡¦s "Protected Range Base" field with this value
                                  + the base address of the region.
  @param[out] FactoryDefaultLimit The length of the factory image.
                                  BIOS sets a Protected Range (PR) register¡¦s "Protected Range Limit" field with this value
  @param[out] Result              Status report

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciHmrfpoLock (
  OUT UINT64                          *Nonce,
  OUT UINT32                          *FactoryDefaultBase,
  OUT UINT32                          *FactoryDefaultLimit,
  OUT UINT8                           *Result
  )
;

/**

  @brief
  System BIOS sends this message to get status for HMRFPO_LOCK message.

  @param[out] Result              HMRFPO_GET_STATUS response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciHmrfpoGetStatus (
  OUT UINT8                           *Result
  )
;

/**

  @brief
  KVM support.

  @param[in] QueryType            0 - Query Request
                                  1 - Cancel Request
  @param[out] ResponseCode        1h - Continue, KVM session established.
                                  2h - Continue, KVM session cancelled.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciQueryKvmRequest (
  IN  UINT32                         QueryType,
  OUT UINT32                         *ResponseCode
  )
;

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to query the local firmware update interface status.

  @param[out] RuleData            1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciGetLocalFwUpdate (
  OUT UINT8         *RuleData
  )
;

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to enable or disable the local firmware update interface.
  The firmware allows a single update once it receives the enable command

  @param[in] RuleData             1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciSetLocalFwUpdate (
  IN UINT8         RuleData
  )
;

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to enable the ME State.
  The firmware allows a single update once it receives the enable command

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME enabled message sent

**/
EFI_STATUS
HeciSetMeEnableMsg (
  IN VOID
  )
;

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to disable the ME State.
  The firmware allows a single update once it receives the disable command

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME is disabled

**/
EFI_STATUS
HeciSetMeDisableMsg (
  VOID
  )
;

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  Consumer or Corporate size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA   *RuleData
  )
;

/**

  @brief
  This message is sent by the BIOS on the boot where the host wants to get the firmware provisioning state.
  The firmware will respond to AMT BIOS SYNCH INFO message even after the End of Post.

  @param[out] RuleData            Bit [2:0] Reserved
                                  Bit [4:3] Provisioning State
                                    00 - Pre -provisioning
                                    01 - In -provisioning
                                    02 - Post ¡Vprovisioning
                                  Bit [31:5] Reserved

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Firmware provisioning state returned

**/
EFI_STATUS
HeciAmtBiosSynchInfo (
  OUT UINT32         *RuleData
  )
;

/**

  @brief
  The firmware will respond to GET OEM TAG message even after the End of Post (EOP).

  @param[in] RuleData             Default is zero. Tool can create the OEM specific OEM TAG data.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciGetOemTagMsg (
  OUT UINT32         *RuleData
  )
;

/**

  @brief
  Sets or reads Lock mask on ICC registers.
  @param[in] AccessMode           0 - set, 1 - get
  @param[in] ResponseMode         0 - firmware will answer, 1 - firmware will not answer
  @param[in][out] LockRegInfo     bundle count info and mask of registers to become (for 'set' mode) or are
                                  (for 'get' mode) locked. Each bit represents a register. 0=lock, 1=don't lock

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_INVALID_PARAMETER   ResponseMode or pointer of Mask is invalid value
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet

**/
EFI_STATUS
HeciLockIccRegisters (
  IN UINT8       AccessMode,
  IN UINT8       ResponseMode,
  IN OUT ICC_LOCK_REGS_INFO  *LockRegInfo
  )
;

/**

  @brief
  retrieves the number of currently used ICC clock profile
  @param[out] Profile             number of current ICC clock profile

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet

**/
EFI_STATUS
HeciGetIccProfile (
  OUT UINT8                       *Profile,
  OUT UINT8                       *NumProfiles,
  OUT ICC_PROFILE_DESC            *ProfileDescriptions
  )
;

/**

  @brief
  Sets ICC clock profile to be used on next and following boots
  @param[in] Profile              number of profile to be used

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet

**/
EFI_STATUS
HeciSetIccProfile (
  IN UINT8 Profile
  )
;

/**

  @brief
  Writes 1 dword of data to the icc register offset specified by RegOffset in the ICC Aux space
  @param[in] RegOffset            Register Offset in ICC Aux Space to write
  @param[in] RegData              Dword ICC register data to write
  @param[in] ResponseMode 0       Wait for response, 1 - skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_INVALID_PARAMETER   ResponseMode is invalid value
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
HeciWriteIccRegDword (
  IN UINT16                       RegOffset,
  IN UINT32                       RegData,
  IN UINT8                        ResponseMode
  )
;


/**

  @brief
  Sends the MKHI Enable/Disable manageability message

  @param[in] EnableState          Enable Bit Mask
  @param[in] DisableState         Disable Bit Mask

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciFwFeatureStateOverride (
  IN UINT32  EnableState,
  IN UINT32  DisableState
  )
;

/**

  @brief
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] RuleData            MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                *RuleData
  )
;

/**

  @brief
  Enables/disables clocks. Used to turn off clocks in unused pci/pcie slots.
  BIOS use this command when it enumerates PCI slots. When PCI slot is found unpopulated, the
  BIOS can disable its clock through this MEI message. It is the BIOS requirement to know which
  slot is controlled by which control bit.

  @param[in] Enables              each bit means corresponding clock should be turned on (1) or off (0)
  @param[in] EnablesMask          each bit means corresponding enable bit is valid (1) or should be ignored (0)
  @param[in] ResponseMode 0       wait for response, 1 - skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_INVALID_PARAMETER   ResponseMode is invalid value
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetIccRegLock (
  OUT UINT8                       *LockState
  )
;

/**

  @brief
  Sets ICC clock profile to be used on next and following boots

  @param[in] LockState              Requested FW LockState

  @exception EFI_UNSUPPORTED      Message attempted after EOP
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccRegLock (
  IN UINT8                        LockState
  )
;

/**

  @brief
  retrieves the number of currently used ICC clock profile

  @param[out] IccClockSettings    ICC Clock Settings of the requested clock

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY            Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetIccClockSettings (
  OUT ICC_GETSET_CLOCK_SETTINGS_REQRSP  *IccClockSettings

  )
;

/**

  @brief
  Sets ICC clock profile to be used on next and following boots

  @param[in] IccClockSettings     Requested ICC Clock Settings
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccClockSettings (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP  *IccClockSettings

  )
;

/**

  @brief
  Retrieves the Clock Range Definition Record

  @param[out] IccClockRangeRecords     Clock Range Definition Records containing 2 single CRDR

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetClockRangeDefinitionRecord (
  OUT ICC_CLK_RANGE_DEF_REC  *IccClockRangeRecords
  )
;

/**

  @brief
  This message is sent by the BIOS to inform ME FW whether or not to take the
  TPM 1.2 Deactivate flow

  @param[in] TpmDeactivate  0 - ME FW should not take the deactivate flow.
                            1 - ME FW should take the deactivate flow.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI interfaces disabled by ME
**/
EFI_STATUS
HeciSendTpmData (
  IN UINT8               TpmDeactivate
  )
;

/**

  @brief
  This message is sent by the BIOS when it wants to query
  ME Unconfig on RTC Clear Disable state.

  @param[out] RuleData             1 - Unconfig on RTC clear is disabled
                                  0 - Unconfig on RTC clear is not disabled

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciGetUnconfigOnRtcClearDisableMsg (
  OUT UINT32                *RuleData
  )
;

/**

  @brief
  This message is sent by the BIOS when it wants to set
  ME Unconfig on RTC Clear Disable state.

  @param[in] RuleData             1 - Disable Unconfig on RTC clear
                                  0 - Enable Unconfig on RTC clear

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function or EOP was sent
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciSetUnconfigOnRtcClearDisableMsg (
  IN UINT32                RuleData
  )
;



/**
  This message is sent by the BIOS when it wants to notify that the BIOS is starting
  to run 3rd party code.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function or EOP was sent
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/
EFI_STATUS
HeciCoreBiosDoneMsg (
  VOID
)
;

#ifdef AMT_SUPPORT

/**
  Send Get MBP from FW

  @param[in] MbpHeader     MBP header of the response
  @param[in] MbpItems       MBP items of the response

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
PeiHeciGetMbpMsg (
  IN OUT MBP_HEADER            *MbpHeader,
  IN OUT UINT32                *MbpItems,
  IN BOOLEAN                   SkibMbp
  )
;

/**
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku              ME Firmware Capability SKU

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
PeiHeciGetFwCapsSkuMsg (
  IN OUT MEFWCAPS_SKU             *FwCapsSku
  )
;

/**
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] RuleData            MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
**/
EFI_STATUS
PeiHeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                *RuleData
  )
;

/**
  This message is sent by the BIOS or IntelR MEBX. One of usages is to utilize
  this command to determine if the platform runs in Consumer or Corporate SKU
  size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetUsageType

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
PeiHeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  )
;


/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] ResetOrigin         Reset source
  @param[in] ResetType           Global or Host reset

  @exception EFI_UNSUPPORTED     Current ME mode doesn't support this function
  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_NOT_FOUND          No ME present
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT            HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendCbmResetRequest (
  IN  UINT8                      ResetOrigin,
  IN  UINT8                      ResetType
  )
;
#endif // AMT_SUPPORT

/**
  Dump PEI ME Platform Policy

  @param[in] PeiMePlatformPolicy  The Me Platform Policy instance
**/
VOID
PeiMePolicyDebugDump (
  IN  PEI_ME_PLATFORM_POLICY_PPI      *MePlatformPolicyPpi
  );

/**

  @brief
  Send HSIO request through HECI to get the HSIO settings version on CSME side.

  @param[in] BiosCmd              HSIO command: 0 - close interface, 1 - report HSIO version
  @param[out] FwSts               FwSts

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciHsioMsg (
  IN  UINT32                      BiosCmd,
  OUT UINT16                      *Crc
  )
;

/**

  @brief
  Send the required system ChipsetInit Table to ME FW.

  @param[in] ChipsetInitTable     The required system ChipsetInit Table.
  @param[in] ChipsetInitTableLen  Length of the table in bytes

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciChipsetInitSyncMsg (
  IN  UINT8                       *ChipsetInitTable,
  IN  UINT32                      ChipsetInitTableLen
  )
;

/**
  Initialize HECI Interface.

  @param [in] None

  @retval Status Initialization Status
**/

EFI_STATUS
PeiHeciInitialize (
  VOID
  )
;

/**
  Send DRAM init done message through HECI to inform ME of memory initialization done.

  @param[in] DidInput             ME UMA data
  @param [in] WaitDidAck          Wait for DID Ack
  @param[out] BiosAction          ME response to DID

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendDid (
  IN  UINT32                      DidInput,
  OUT UINT8                       *BiosAction
  )
;

/**
  Signal CSME FW BUP to leave the MANUF disabled state.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciClearManufDisableRequest (
  VOID
  )
;

/**
  Send Tracing Enable request to CSME via HECI.


  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciTracingEnableRequest (
  VOID
  )
;

/**
  Send ICC request through HECI to query if CSME FW requires the warm reset flow from a previous boot.

  @param[out] WarmResetRequired   1 - CSME requires a warm reset to complete BCLK ramp en flow

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciIccBclkMsg (
  OUT UINT8    *WarmResetRequired
  )
;

#endif // _HECI_MESSAGE_LIB_H_
