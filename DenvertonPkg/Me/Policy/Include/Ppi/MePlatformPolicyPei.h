/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  2010 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MePlatformPolicyPei.h

@brief:

  ME Platform Policy for ME PEIMs

**/
#ifndef _PEI_ME_PLATFORM_POLICY_PPI_H_
#define _PEI_ME_PLATFORM_POLICY_PPI_H_

///
/// Include other definitions
///
///
/// Define the PPI GUID
///
#define PEI_ME_PLATFORM_POLICY_PPI_GUID \
  { \
    0x9f685891, 0x4e6f, 0x445c, 0xbb, 0x9e, 0xe5, 0x7a, 0x28, 0xfa, 0x53, 0xa0 \
  }

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gPeiMePlatformPolicyPpiGuid;

///
/// Revision
///
#define PEI_ME_PLATFORM_POLICY_PPI_REVISION_1 1
#define PEI_ME_PLATFORM_POLICY_PPI_REVISION_2 2
#define PEI_ME_PLATFORM_POLICY_PPI_REVISION_3 3

///
/// PPI definition
///
#pragma pack(1)
typedef struct _PEI_ME_PLATFORM_POLICY_PPI {
  UINT8   Revision;                          // [0]
  UINT8   MeTimeout;                         // [1]
  UINT8   Byte1Reserved;                     // [2]
  UINT8   DidEnabled         : 1;            // [3]
  UINT8   DidInitStat        : 4;
  UINT8   MeEndOfPostEnabled : 1;
  UINT8   HeciCommunication1 : 2;
  UINT8   HeciCommunication2 : 2;            // [4]
  UINT8   SolEnabled         : 2;
  UINT8   IderEnabled        : 2;
  UINT8   HeciCommunication3 : 2;


#if SPS_SUPPORT
  UINT8   NmPwrOptBootOverride;                     // [5]
  UINT8   NmPwrOptBoot;                             // [6]
  UINT8   NmCores2DisableOverride;                  // [7]
  UINT8   NmCores2Disable;                          // [8]
  UINT8   EnableMePreDidReset           : 1;        // [9]
  UINT8   SetFiaMuxConfigEnabled        : 1;
  UINT8   SpsIccClkSscSettingSupported  : 1;
  UINT8   SpsIccClkSscSetting           : 1;
  UINT8   MeShutdownEnabled             : 1;
  UINT8   MeHmrfpoLockEnabled           : 1;
  UINT8   Byte2Reserved                 : 2;
  UINT8   Heci1HideInMe                 : 2;        // [10] 0: do not set, 1 - hide, 2 disable
  UINT8   Heci2HideInMe                 : 2;
  UINT8   Heci3HideInMe                 : 2;
  UINT8   Byte3Reserved0                : 2;

  UINT8   Byte3Reserved1[33];                // [11-46]
#else
  UINT8   Byte3Reserved1[41];                // [5-46]
#endif // SPS_SUPPORT

#ifdef AMT_SUPPORT
  // Byte 47
  UINT8   AmtEnabled         : 1;            // [47]
  UINT8   WatchDog           : 1;
  UINT8   WatchDogOs         : 1;
  UINT8   AsfEnabled         : 1;
  UINT8   ManageabilityMode  : 1;  ///< 0: Off, 1:On
  UINT8   Byte4Reserved      : 3;
  UINT16  WatchDogTimerOs;                   // [48]
  UINT16  WatchDogTimerBios;                 // [50]
  UINT8   FWProgress;                        // [52]
  UINT8   Byte5Reserved[8];                 // [53-61]
#else
  UINT8   Byte5Reserved[12];                // [47-61]
#endif // AMT_SUPPORT

  UINT8   HsioEnableMessaging : 1;           // [62]
  UINT8   Byte6Reserved       : 7;

#if ME_TESTMENU_FLAG
  UINT8   TestDisableD0I3SettingForHeci  : 1;  // [63]
  UINT8   WaitResetWarningAck            : 1;
  UINT8   FiaMuxMessageEnabled           : 1;
  UINT8   Byte7Reserved1                 : 5;
#else
  UINT8   Byte7Reserved2;                     // [63]
#endif // ME_TESTMENU_FLAG
} PEI_ME_PLATFORM_POLICY_PPI;
#pragma pack()

#endif
