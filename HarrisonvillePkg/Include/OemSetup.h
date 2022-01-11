/** @file
 OEM Specific Setup Variables and Structures

@copyright
 Copyright (c) 2011 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _OEMSETUP_H_
#define _OEMSETUP_H_
#ifdef __cplusplus
extern "C" {
#endif

// OS profile 
#define OS_PROFILE_GUID \
 { 0xE59376D7, 0x2DD9, 0x42A3, 0x9E, 0xC8, 0x1D, 0x71, 0xD5, 0xE3, 0xC1, 0xEC }

// Currently enumerated OSes
#define OsNone                   0x00
#define OsAndroid                0x01
#define OsWinCs                  0x02
#define OsWinNonCs               0x03
#define OsMax                    0x04

#pragma pack(1)

typedef struct _SETUP_VOLATILE_DATA
{
  UINT8  FabId;
  UINT8  BoardBomId;
  UINT16 PlatId;
  UINT8  PlatformFlavor;
  UINT8  PlatformType;
  UINT8  BoardType;
  UINT8  VTdAvailable;
  UINT8  PchGeneration;
  UINT8  PchRevId;
  UINT8  GbeAvailable;
  UINT8  GbePciePortNum;
  UINT8  DimmPresent[4];
  UINT8  PcieControllerCfg1;
  UINT8  PcieControllerCfg2;
  UINT8  PcieControllerCfg3;
  UINT8  PcieControllerCfg4;
  UINT8  PcieControllerCfg5;
  UINT8  PcieSBDE;
  UINT8  EnableDebugSerialIoUartNumber;
  UINT32 AcpiDebugAddress;
  UINT8  PegPresent[3];
  UINT8  RaidAvailable;
  UINT8  SataMpsPresent[8];
  UINT8  MdesEnabled;
  UINT8  UsbSsicEnabled[2];
  UINT8  DockAttached;
} SETUP_VOLATILE_DATA;

//This structure is used for setup.
typedef struct {
  UINT8 XDBitAvailable;
  UINT8 TurboModeAvailable;
  UINT8 XETdcTdpLimitAvailable;
  UINT8 CoreRatioLimitAvailable;
  UINT8 HTAvailable;
  UINT8 MultiCoreAvailable;
  UINT8 VTAvailable;
  UINT8 AESAvailable;
  UINT8 TXTAvailable;
  UINT8 CxAvailable;
  UINT8 C1Available;
  UINT8 C1EAvailable;
  UINT8 C3Available;
  UINT8 C6Available;
  UINT8 C7Available;
  UINT8 C8Available;
  UINT8 C9Available;
  UINT8 C10Available;
  UINT8 EISTAvailable;
  UINT8 HwpAvailable;
  UINT8 NumCores;
  UINT8 MultiSocketAvailable;
  UINT8 MultiSocketPopulated;
  UINT8 LocalX2ApicAvailable;
  UINT8 EnergyEfficientTurboAvailable;
  UINT8 SgxSupported;
  UINT8 C6DramSupported;
  UINT8 ApicIdFlag;
  UINT8 ConfigTdpAvailable;
  UINT8 LpmAvailable;
  UINT8 TimedMwaitSupported;
  UINT8 DebugInterfaceSupported;
  UINT8 ProcTraceSupported;
  UINT8 SmmEnhancementSupported;
  UINT8 SmmTargetedSmiAvailable;
  UINT8 SmmCpuSvrstrAvailable;  
  UINT8 SmmCodeAccessAvailable;
  UINT8 SmmLongFlowAvailable;
  UINT8 HdcSupported;
} SETUP_CPU_FEATURES;

typedef struct {
  UINT8   OsProfile;
  UINT8   PrevOs;
} OS_PROFILE;

#pragma pack()

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
