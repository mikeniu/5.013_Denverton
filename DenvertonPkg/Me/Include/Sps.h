/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file Sps.h

 SPS definitions common for various SPS modules.

**/
#ifndef _SPS_H_
#define _SPS_H_

#include <Library/HeciMsgLib.h>
#include <Library/MeFiaMuxLib.h>

#define ME_FUNCTIONAL               0x1
#define ME_NONFUNCTIONAL            0x2
#define ME_NOT_SPS                  0xF
#define ME_DEV_NOT_PRESENT          0x4
#define LPC_DEV_NOT_PRESENT         0x8
#define ME_AVAILABLE_INVALID_STATUS 0x10

#define SPS_MAX_NUMBER_OF_RESEND_MESSAGE 3

#define SpsMaxNumberOfMsgResendReached(NumOfMsgResend) ((NumOfMsgResend) > SPS_MAX_NUMBER_OF_RESEND_MESSAGE)

/*****************************************************************************
 * SPS  HECI registers addresses
 */
#define SPS_REG_MEFS1    0x40   // HECI-1
#define SPS_REG_MEFS2    0x48   // HECI-1
#define SPS_REG_NMFS     0x40   // HECI-2
#define SpsHeciPciReadMefs1()                                                   \
        PciRead32(PCI_LIB_ADDRESS(ME_BUS, ME_DEV, ME_FUN_HECI1, SPS_REG_MEFS1))
#define SpsHeciPciReadMefs2()                                                   \
        PciRead32(PCI_LIB_ADDRESS(ME_BUS, ME_DEV, ME_FUN_HECI1, SPS_REG_MEFS2))
#define SpsHeciPciReadNmfs()                                                    \
        PciRead32(PCI_LIB_ADDRESS(ME_BUS, ME_DEV, ME_FUN_HECI2, SPS_REG_NMFS))

/******************************************************************************
 * SPS interface on HECI status registers
 */
//
// SPS ME Firmware Status #1 Register
// This register is located in HECI-1 PCI config space at offset 40h.
// Most fields map to HECI_FWS_REGISTER in client firmware.
//
typedef union
{
  UINT32   UInt32;
  struct
  {
    UINT32 CurrentState      : 4,  // 0:3   Current ME firmware state
           ManufacturingMode : 1,  // 4     Platform is in Manufacturing Mode
           FptBad            : 1,  // 5     Flash Partition Table or factory defaults bad
           OperatingState    : 3,  // 6:8   ME operatiing state
           InitComplete      : 1,  // 9     Set when firmware finished initialization
           FtBupLdFlr        : 1,  // 10    ME is not able to load BRINGUP from recovery code
           UpdateInprogress  : 1,  // 11
           ErrorCode         : 4,  // 12:15 ME is not running because of fatal error
           OperatingMode     : 4,  // 16:19 This field describes the current ME operating mode
           MeResetCounter    : 4,  // 20:23 Counter of ME Resets
           FlDscRegVerStatus : 1,  // 24    Flash Descriptor Region Verification status
           Reserved1         : 7;  // 25:31
  } Bits;
} SPS_MEFS1;

//
// SPS_MEFS1::CurrentState Values
// This field describes the current operation state of the firmware.
// The values are shared with client ME firmware.
//
#define MEFS1_CURSTATE_RESET        0  // ME is in reset, will exit this state within 1 milisecond
#define MEFS1_CURSTATE_INIT         1  // ME is initializing, will exit this state within 2 seconds
#define MEFS1_CURSTATE_RECOVERY     2  // ME is in recovery mode, check other bits to determine cause
#define MEFS1_CURSTATE_DISABLED     4  // ME functionality has been disabled - not used with SPS
#define MEFS1_CURSTATE_NORMAL       5  // ME is in normal operational state
#define MEFS1_CURSTATE_DISABLE_WAIT 6  // Not used with SPS
#define MEFS1_CURSTATE_TRANSITION   7  // ME is transitioning to a new Operating State

//
// SPS_MEFS1::OperatingState Values
// This field describes the current operating state of ME.
//
#define MEFS1_OPSTATE_PREBOOT       0  // ME in pre-boot
#define MEFS1_OPSTATE_M0_UMA        1  // ME runs using UMA - not used with SPS
#define MEFS1_OPSTATE_M3            4  // ME runs without UMA, host in Sx
#define MEFS1_OPSTATE_M0            5  // ME runs without UMA, host in S0 -normal state for SPS
#define MEFS1_OPSTATE_BRINGUP       6  // ME in bringup
#define MEFS1_OPSTATE_M0_ERROR      7  // M0 without UMA but with error

//
// SPS_MEFS1::ErrorCode Values
// If set to non zero value the ME firmware has encountered a fatal error and stopped normal operation.
//
#define MEFS1_ERRCODE_NOERROR       0
#define MEFS1_ERRCODE_UNKNOWN       1
#define MEFS1_ERRCODE_DISABLED      2  // Not used with SPS
#define MEFS1_ERRCODE_IMAGE_FAIL    3  // No valid ME firmware image found in ME region

//
// SPS_MEFS1::OperatingMode Values
// This field describes the current operating mode of ME.
//
#define MEFS1_OPMODE_NORMAL         0  // Client firmware is running in ME
#define MEFS1_OPMODE_IGNITION       1  // Ignition firmware is running in ME
#define MEFS1_OPMODE_ALT_DISABLED   2  // ME is disabled
#define MEFS1_OPMODE_TEMP_DISABLE   3  //
#define MEFS1_OPMODE_SECOVR_JMPR    4  // Security Override activated with jumper
#define MEFS1_OPMODE_SECOVR_MSG     5  // Security Override activated with HMRFPO_ENABLE request
#define MEFS1_OPMODE_SPS            15 // SPS firmware is running in ME

//
// SPS ME Firmware Status #2 Register
// This register is located in HECI-1 PCI config space at offset 48h.
//
typedef union
{
  UINT32   UInt32;
  struct
  {
    UINT32 Reserved1              : 1,  // 0
           IccProgStatus          : 2,  // 1:2   ICC Programming Status
           Reserved2              : 3,  // 3:5
           MfsFailure             : 1,  // 6     ME detected error in its configuration
           WarmResetRequest       : 1,  // 7     ME informs that host warm reset is necessary
           RecoveryCause          : 3,  // 8:10  If MEFS1.CurrentState says recovery here is reason
           EopStatus              : 1,  // 11    ME notion of EOP status
           MeTargetImageBootFault : 1,  // 12    ME could not boot active image, backup is running
           FirmwareHeartbeat      : 3,  // 13:15 ME heartbeat, changes each second
           ExtendedStatusData     : 8,  // 16:23
           PmEvent                : 4,  // 24:27
           PhaseCode              : 4;  // 38:31
  } Bits;
} SPS_MEFS2;

//
// SPS_MEFS2::RecoveryCause Values
// If SPS_MEFS1::CurrentState bits indicate that ME firmware is running in recovery mode
// these values provide the cause of this mode.
//
#define SPS_RCAUSE_RCVJMPR  0  // ME recovery jumper asserted
#define SPS_RCAUSE_MFGJMPR  1  // Security strap override jumper asserted (aka mfg jumper)
#define SPS_RCAUSE_IPMICMD  2  // Recovery forced with IPMI command
#define SPS_RCAUSE_FLASHCFG 3  // Invalid flash configuration (see ME-BIOS spec. for details)
#define SPS_RCAUSE_MEERROR  4  // ME internal error, check SEL
#define SPS_RCAUSE_UMAERROR 5  // UMA memory configuration error returned by HECI DID message

//
// SPS_MEFS2::PmEvent Values
// Power management transition status.
#define SPS_PMEVENT_CMOFF_CMX                0x00 // Clean CMoff->CMx wake
#define SPS_PMEVENT_CMOFF_CMX_AFTER_ERROR    0x01 // CMoff->CMx wake after an error
#define SPS_PMEVENT_CLEAN_GLOBAL_RESET       0x02 // Clean global reset
#define SPS_PMEVENT_GLOBAL_RESET_AFTER_ERROR 0x03 // Global reset after an error
#define SPS_PMEVENT_CLEAN_ME_RESET           0x04 // Clean ME reset
#define SPS_PMEVENT_RESET_DUE_EXCEPTION      0x05 // ME reset due to exception
#define SPS_PMEVENT_PSEUDO_GLOBAL_RESET      0x06 // Pseudo-global reset
#define SPS_PMEVENT_S0_SX                    0x07 // S0/CM0->Sx/CM3
#define SPS_PMEVENT_SX_S0                    0x08 // Sx/CM3->S0/CM0
#define SPS_PMEVENT_NON_POWER_CYCLE_RESET    0x09 // Non-power cycle reset
#define SPS_PMEVENT_POWER_CYCLE_RESET_CM3    0x0A // Power cycle reset through CM3
#define SPS_PMEVENT_POWER_CYCLE_RESET_CMOFF  0x0B // Power cycle reset through CMoff
#define SPS_PMEVENT_SX_SX                    0x0C // Sx/CMx->Sx/CMoff
#define SPS_PMEVENT_CM0_CM0_PG               0x0D // CM0->CM0_PG
#define SPS_PMEVENT_CM3_CM3_PG               0x0E // CM3->CM3_PG
#define SPS_PMEVENT_CM0_PG_CM0               0x0F // CM0_PG->CM0

//
// SPS NM Firmware Status Register.
// This register is located in HECI-2 PCI config space at offset 40h.
//
typedef union
{
  UINT32   UInt32;
  struct
  {
    UINT32 PerfOptBoot       : 1,  // 0     Power vs performance optimized boot mode
           Cores2Disable     : 7,  // 1:7   Number of processor cores to disable in each processor
           PwrLimiting       : 1,  // 8     Set to 1 if ME is activly limiting power consumption
           SmartClstUVolt    : 1,  // 9     Set to 1 if under-voltage was seen
           SmartClstOCurr    : 1,  // 10    Set to 1 if over-current was seen
           SmartClstOTemp    : 1,  // 11    Set to 1 if over-temperature was seen
           DirectPtuExecution: 1,  // 12    Set to 1 if PTU execution is requested
           Reserved1         :18,  // 13:30
           NmEnabled         : 1;  // 31    Set to 1 if Node Manager is suported in ME
  } Bits;
} SPS_NMFS;

#define SPS_NMFS_PERF_OPT_BOOT_SET 0x01

/*****************************************************************************
 * SPS interface using HECI messaging
 */
#define SPS_CLIENTID_BIOS 0x00


/*****************************************************************************
 * ME Kernel Host Interface
 *****************************************************************************/
#define SPS_CLIENTID_ME_MKHI 0x07
//
// MKHI protocol supports several groups and several commands in each group
//
#define MKHI_GRP_GEN    0xFF  // Generic group
#define MKHI_GRP_HMRFPO 0x05  // Host ME Region Flash Protection Override
#define MKHI_GRP_NM     0x11  // Node Manager extension to MKHI
#define MKHI_GRP_DFUSE  0x13  // Dynamic Fusing


/******************************************************************************
 * Generic MKHI messages group
 */
#define MKHI_CMD_GET_MKHI_VERSION  0x01
#define MKHI_CMD_GET_FW_VERSION    0x02
#define MKHI_CMD_END_OF_POST       0x0C
#define MKHI_CMD_HECI_STATE_CHANGE 0x0D

//
// MKHI Get Version message structure
// This request provides MKHI protocol definition version.
//
#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
} MKHI_MSG_GET_MKHI_VERSION_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
  UINT16               Minor;
  UINT16               Major;
} MKHI_MSG_GET_MKHI_VERSION_RSP;
#pragma pack()


//
// MKHI Get Firmware Version message structure
// Backup firmware version is optional, response may not contain this field
// if ME does not implement dual-image configuration.
//
//
// ME firmware version numbers structure
//
typedef struct
{
  UINT16  Minor;
  UINT16  Major;
  UINT16  Build;
  UINT16  Patch;
} MKHI_FW_VERSION;

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
} MKHI_MSG_GET_FW_VERSION_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
  MKHI_FW_VERSION      Act;  // Active operatinal firmware
  MKHI_FW_VERSION      Rcv;  // Recovery firmware
  MKHI_FW_VERSION      Bkp;  // Backup operational firmwar (optional)
} MKHI_MSG_GET_FW_VERSION_RSP;
#pragma pack()


//
// MKHI End Of POST message structure
//
#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
} MKHI_MSG_END_OF_POST_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
  UINT32               Action;
} MKHI_MSG_END_OF_POST_RSP;
#pragma pack()


//
// MKHI HECI State Change message structure
//
#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
  UINT64               Nonce;
  UINT8                HeciId;  // HECI ID 1,2,3 - this is not function number
  UINT8                State;   // 1 for hide, or 0 for disable
  UINT16               Reserved;
} MKHI_MSG_HECI_STATE_CHANGE_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER  Mkhi;
  UINT32               Response;
} MKHI_MSG_HECI_STATE_CHANGE_RSP;
#pragma pack()


/******************************************************************************
 * Host ME Region Flash Protoction Override MKHI messages group
 */
#define HMRFPO_CMD_MERESET   0x00
#define HMRFPO_CMD_ENABLE    0x01
#define HMRFPO_CMD_LOCK      0x02
#define HMRFPO_CMD_GETSTATUS 0x03

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT64              Nonce;
} MKHI_MSG_HMRFPO_ENABLE_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT32              FactoryBase;
  UINT32              FactoryLimit;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_MSG_HMRFPO_ENABLE_RSP;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
} MKHI_MSG_HMRFPO_LOCK_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT64              Nonce;
  UINT32              FactoryBase;
  UINT32              FactoryLimit;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_MSG_HMRFPO_LOCK_RSP;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
} MKHI_MSG_HMRFPO_GETSTATUS_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_MSG_HMRFPO_GETSTATUS_RSP;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT64              Nonce;
} MKHI_MSG_HMRFPO_MERESET_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_MSG_HMRFPO_MERESET_RSP;
#pragma pack()

/******************************************************************************
 * Dynamic Fusing interface
 */
#define SPS_CLIENTID_ME_DYNFUSE 0x7

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
} SPS_DYNAMIC_FUSING_REQ;
#pragma pack()

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT8               Result;
} SPS_DYNAMIC_FUSING_RSP;
#pragma pack()

typedef enum {
   CPU_DYNAMIC_FUSING_SUCCESS ,
   CPU_DYNAMIC_FUSING_GRST_REQUIRED ,
   CPU_DYNAMIC_FUSING_END_OF_POST_RECEIVED ,
   CPU_DYNAMIC_FUSING_ROL_SOFT_DISABLED
 } DYNAMIC_FUSING_RESPONSE_TYPE ;


/******************************************************************************
 * SPS Node Manager messages
 */
#define NM_CMD_HOSTCFG  0x00
#define MAX_ACPI_PSTATES 16

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  union
  {
    UINT16            Uint16;
    struct
    {
      UINT16          TurboEn   : 1,
                      SmiOptim  : 1,
                      PowerMsmt : 1,
                      HwChange  : 1,
                      Resrerve0 :12;
    } Bits;
  } Capabilities;
  UINT8               PStatesNumber;
  UINT8               TStatesNumber;
  UINT16              MaxPower;
  UINT16              MinPower;
  UINT8               ProcNumber;
  UINT8               ProcCoresNumber;
  UINT8               ProcCoresEnabled;
  UINT8               ProcThreadsEnabled;
  UINT64              Reserved;
  UINT64              PlatformInfo;
  INT32               Altitude;
  UINT8               PStatesRatio[MAX_ACPI_PSTATES];
} MKHI_MSG_NM_HOST_CFG_REQ;

#define STATUS_NM_HOST_CFG_RSP_OK                   0
#define STATUS_NM_HOST_CFG_RSP_WRONG_MSG_FORMAT     1
#define STATUS_NM_HOST_CFG_RSP_ALTITUDE_MISSING     2
#define STATUS_NM_HOST_CFG_RSP_PSTATESRATIO_MISSING 3
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT8               Status;
  UINT8               Reserved1;
  UINT16              Reserved2;
} MKHI_MSG_NM_HOST_CFG_RSP;
#pragma pack()


/*****************************************************************************
 * ICC interface
 *****************************************************************************/
#define SPS_CLIENTID_ME_ICC 0x8

#define ICC_API_4_0 0x00040000
#define ICC_API_3_0 0x00030000
#define ICC_API_2_0 0x00020000
#define ICC_API_1_0 0x00010000

//
// ICC HECI message commands
//
#define ICC_CMD_GET_CLOCK_SETTINGS      0x1A
#define ICC_CMD_SET_CLOCK_SETTINGS      0x1B
#define ICC_CMD_SET_FIA_MUX_CONFIG      0x1E
#define ICC_CMD_GET_FIA_MUX_CONFIG      0x1F

//
// SPS ICC Status Codes
//
#define ICC_STS_SUCCESS                      0x00
#define ICC_STS_INCORRECT_API_VERSION        0x02
#define ICC_STS_INVALID_FUNCTION             0x03
#define ICC_STS_INVALID_BUFFER_LENGTH        0x04
#define ICC_STS_INVALID_PARAMETERS           0x05
#define ICC_STS_ENABLED_CLOCK_MASK_VIOLATION 0x0C
#define ICC_STS_CMD_NOT_SUPPORTED_AFTER_DID  0x40
#define ICC_STS_FIA_MUX_CONFIG_SKU_MISMATCH  0x41
#define ICC_STS_FIA_MUX_NO_CONFIG_FOUND      0x42

// Index of the clock capabilities to "Get" or "Set"
#define REQUESTED_CLOCK_PCIe_Gen2_PLL        0x02

#pragma pack(1)

typedef struct {
  UINT32 ApiVersion;
  UINT32 IccCommand;
  UINT32 IccStatus;
  UINT32 BufferLength;
  UINT32 Reserved;
} ICC_MSG_HEADER;

typedef struct _ICC_SETGET_CLOCK_SETTINGS_REQ {
  ICC_MSG_HEADER  IccHeader;        // ICC Command
  UINT8           RequestedClock;   // Req: Specifies an index of the clock capabilities to "Get" or "Set".
  UINT8           SettingType;      // Req: Selects the source of to retrieve the clock settings.
  UINT8           Reserved1;
  UINT8           ClockMode;        // Rsp: PCH Clocking Mode
  UINT32          Frequency;        // Rsp: Frequency of the clock in Hz
  UINT32          Reserved2;
  UINT32          MaxFrequency;     // Rsp: Specifies the maximum frequency supported by the clock
  UINT32          MinFrequency;     // Rsp: Specifies the minimum frequency supported by the clock
  UINT8           SSCMode;          // Rsp: Spread Spectrum Clocking Mode (Up, Down, Center, None)
  UINT8           SSCPercentage;    // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           MaxSSCPercentage; // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           SSCsetting;       // Req: Spread Spectrum settings for IsCLK
  UINT16          Reserved3;
  UINT16          Reserved4;
} ICC_SETGET_CLOCK_SETTINGS_REQ;

typedef struct _ICC_SETGET_CLOCK_SETTINGS_RSP {
  ICC_MSG_HEADER  IccHeader;        // ICC Command
  UINT8           RequestedClock;   // Req: Specifies an index of the clock capabilities to "Get" or "Set".
  UINT8           SettingType;      // Req: Selects the source of to retrieve the clock settings.
  UINT8           Reserved1;
  UINT8           ClockMode;        // Rsp: PCH Clocking Mode
  UINT32          Frequency;        // Rsp: Frequency of the clock in Hz
  UINT32          Reserved2;
  UINT32          MaxFrequency;     // Rsp: Specifies the maximum frequency supported by the clock
  UINT32          MinFrequency;     // Rsp: Specifies the minimum frequency supported by the clock
  UINT8           SSCMode;          // Rsp: Spread Spectrum Clocking Mode (Up, Down, Center, None)
  UINT8           SSCPercentage;    // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           MaxSSCPercentage; // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           SSCsetting;       // Req: Spread Spectrum settings for IsCLK
  UINT16          Reserved3;
  UINT16          Reserved4;
} ICC_SETGET_CLOCK_SETTINGS_RSP;

typedef union {
  ICC_SETGET_CLOCK_SETTINGS_REQ  Message;
  ICC_SETGET_CLOCK_SETTINGS_RSP  Response;
} ICC_SETGET_CLOCK_SETTINGS_BUFFER;
#pragma pack()

/*****************************************************************************
 * SPS FIA CONFIG messages
 *****************************************************************************/

#define FIA_MUX_CONFIGURATION_SIZE              5
#define FIA_SATA_LANES_CONFIGURATION_SIZE       4
#define FIA_PCIE_ROOT_PORTS_CONFIGURATION_SIZE  5

#pragma pack(1)
typedef struct {
  ICC_MSG_HEADER Header;
  UINT8 Invalidate;
  UINT8 MuxConfiguration[FIA_MUX_CONFIGURATION_SIZE];
  UINT8 SataLaneConfiguration[FIA_SATA_LANES_CONFIGURATION_SIZE];
  UINT8 PcieRootPortsConfiguration[FIA_PCIE_ROOT_PORTS_CONFIGURATION_SIZE];
  UINT8 Reserved[1];
} ICC_MSG_SET_FIA_MUX_CONFIG_REQ;

typedef struct {
  ICC_MSG_HEADER Header;
} ICC_MSG_SET_FIA_MUX_CONFIG_RSP;

typedef struct {
  ICC_MSG_HEADER Header;
} ICC_MSG_GET_FIA_MUX_CONFIG_REQ;

typedef struct {
  ICC_MSG_HEADER Header;
  UINT8 LanesAllowed;
  UINT8 MuxConfiguration[FIA_MUX_CONFIGURATION_SIZE];
  UINT8 SataLaneConfiguration[FIA_SATA_LANES_CONFIGURATION_SIZE];
  UINT8 PcieRootPortsConfiguration[FIA_PCIE_ROOT_PORTS_CONFIGURATION_SIZE];
  UINT8 Reserved[1];
} ICC_MSG_GET_FIA_MUX_CONFIG_RSP;
#pragma pack()

/*****************************************************************************
 * SPS messages for ME-BIOS interface definition version
 *****************************************************************************/
#define SPS_CLIENTID_ME_SPS   0x20

#define SPS_CMD_GET_MEBIOS_INTERFACE_REQ 1
#define SPS_CMD_GET_MEBIOS_INTERFACE_RSP (0x80 | SPS_CMD_GET_MEBIOS_INTERFACE_REQ)

#define SpsMeBiosRspCommandDataVerify(CommandData) ((CommandData) == SPS_CMD_GET_MEBIOS_INTERFACE_RSP)

#pragma pack(1)
typedef struct {
  HBM_COMMAND       Command;
} SPS_MSG_GET_MEBIOS_INTERFACE_REQ;

typedef struct {
    HBM_COMMAND     Command;
    UINT8           VerMajor;
    UINT8           VerMinor;
    UINT64          FeatureSet;
} SPS_MSG_GET_MEBIOS_INTERFACE_RSP;
#pragma pack()

//
// BIOS must verify the SPS ME-BIOS Interface Specification version to make
// sure BIOS and ME firmware will talk the sam laguage.
// There can be different versions for recovery and operational ME firmware
// after update of operational firmware.
//
#define SPS_MEBIOS_OPR_VERSION_MIN ((0 << 8) | 0)
#define SPS_MEBIOS_OPR_VERSION_MAX ((0xFF << 8) | 0xFF)
#define SPS_MEBIOS_RCV_VERSION_MIN ((0 << 8) | 0)
#define SPS_MEBIOS_RCV_VERSION_MAX ((0xFF << 8) | 0xFF)
#define SpsMeBiosOprVersionVerify(Major, Minor)                      \
        ((((Major) << 8) | (Minor)) >= SPS_MEBIOS_OPR_VERSION_MIN && \
         (((Major) << 8) | (Minor)) <= SPS_MEBIOS_OPR_VERSION_MAX)
#define SpsMeBiosRcvVersionVerify(Major, Minor)                      \
        ((((Major) << 8) | (Minor)) >= SPS_MEBIOS_RCV_VERSION_MIN && \
         (((Major) << 8) | (Minor)) <= SPS_MEBIOS_RCV_VERSION_MAX)

/*****************************************************************************
 * SPS ME SHUTDOWN message
 *****************************************************************************/
#define SPS_CMD_ME_SHUTDOWN_REQ 3
#define SPS_CMD_ME_SHUTDOWN_RSP (0x80 | SPS_CMD_ME_SHUTDOWN_REQ)

#pragma pack(1)
typedef struct {
  HBM_COMMAND       Command;
} SPS_MSG_ME_SHUTDOWN_REQ;

typedef struct {
    HBM_COMMAND     Command;
    UINT8           Result;
} SPS_MSG_ME_SHUTDOWN_RSP;
#pragma pack()

/*****************************************************************************
* MCTP Bus Owner Proxy Configuration
*****************************************************************************/
#define SPS_CLIENTID_ME_MCTP   0x21

#define MCPT_CMD_SET_BUS_OWNER_REQ   0x01
#define MCPT_CMD_SET_BUS_OWNER_RSP (0x80 | MCPT_CMD_SET_BUS_OWNER_REQ)
#define MCPT_CMD_SET_BUS_OWNER_RSP_SUCCESS (0x00)

#pragma pack(1)
typedef struct {
  UINT8           Command;
  UINT8           Reserved0[3];
  UINT16          PCIeAddress;
  UINT8           Location;
  UINT8           Reserved1;
} MCTP_SET_BUS_OWNER_REQ;

typedef struct {
  UINT8           Command;
  UINT8           Reserved0[2];
  UINT8           Result;
} MCTP_SET_BUS_OWNER_RSP;
#pragma pack()


#endif // _SPS_H_
