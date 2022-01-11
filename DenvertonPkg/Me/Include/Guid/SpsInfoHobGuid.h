//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SpsInfoHobGuid.h

  SPS info HOB definitions.

**/

#ifndef _SPS_INFO_HOB_GUID_H_
#define _SPS_INFO_HOB_GUID_H_

#include <SpsMisc.h>

//
// Definition of ME FW SPS Features Set - according to the
// Intel ME-BIOS Interface Specification (revision 1.0.3) - chapter 4.5.1
//
typedef union
{
  UINT64   Data;
  struct
  {
    UINT64 NodeManager                 :1,    // 0
           PeciProxy                   :1,    // 1
           ICC                         :1,    // 2
           MeStorageServices           :1,    // 3
           BootGuard                   :1,    // 4
           PTT                         :1,    // 5
           OemDefinedCpuDebugPolicy    :1,    // 6
           ResetSuppression            :1,    // 7
           PmBusProxy                  :1,    // 8
           CpuHotPlug                  :1,    // 9
           MicProxy                    :1,    //10
           MctpProxy                   :1,    //11
           ThermalReportVolumeAirflow  :1,    //12
           SocThermalReporting         :1,    //13
           DualBIOSSupport             :1,    //14
           MPHYSurvivabilityProgramming:1,    //15
           PECIOverDMI                 :1,    //16
           PCHDebug                    :1,    //17
           PowerThermalUtilitySupport  :1,    //18
           FiaMuxConfiguration         :1,    //19
           PchThermalSensorInit        :1,    //20
           DeepSx                      :1,    //21
           DualMeFwImage               :1,    //22
           DirectFwUpdate              :1,    //23
           MctpInfrastructure          :1,    //24
           CUPS                        :1,    //25
           FlashDescRegionVerification :1,    //26
           Hotham                      :1,    //27
           TurboStateLimiting          :1,    //28
           TelemetryHub                :1,    //29
           MeShutdown                  :1,    //30
           ASA                         :1,    //31
           WarmResetNotificationSubFlow:1,    //32
           Reserved                    :31;   //33-63

  } Bits;
} SPS_FEATURE_SET;

typedef enum
{
  INVALID_MODE,
  SIEN_MODE,
  NM_MODE,
} ME_SKU;

typedef struct MeDiscoveryMode_iface_t
{
  ME_SKU              Mode;
  SPS_FEATURE_SET     FeatureSet;
  UINT8               WorkFlow;
} ME_FW_DISCOVERY_MODE;

typedef struct _PLATFORM_INFORMATION_HOB_ {
    SPS_FEATURE_SET   FeatureSet;
    UINT8             WorkFlow;
    BOOLEAN           PwrOptBoot;    // NM may request power reduction at boot
    UINT8             Cores2Disable; // NM may request disabling processor cores
} EFI_HOB_TYPE_SPS_INFO;

//
// Definitions for SPS Info HOB
//
#define EFI_SPS_INFO_HOB_GUID \
        {0x489d2a71, 0xba4a, 0x444c, {0x9f, 0xe2, 0xa6, 0xb7, 0xe5, 0xcd, 0x78, 0x47}}

extern EFI_GUID gEfiSpsInfoHobGuid;

#endif // _SPS_INFO_HOB_GUID_H_
