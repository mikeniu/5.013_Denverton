/** @file
  The GUID definition for SaDataHob

@copyright
 Copyright (c) 2012 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _SA_DATA_HOB_H_
#define _SA_DATA_HOB_H_

#include <SaAccess.h>
#include <Base.h>


extern EFI_GUID gSaDataHobGuid;


//!!!This macro has the copy with same purpose @/HarrisonvillePkg/Include/SetupVariable.h and @/DenvertonPkg/SystemAgent/Include/PPI/SaPlatformConfig.h
#define COS_CAT_AGENT_COUNT	16

#define SA_VTD_ENGINE_NUMBER            1
///
/// The data elements should be initialized by a Platform Module.
/// The data structure is for VT-d driver initialization
///
typedef struct {

  BOOLEAN       VtdEnable;                         ///< This field is used to describe whether or not the VT-d function should be enabled
  BOOLEAN       X2ApicOptOut;                      ///< This field is used to enable the X2APIC_OPT_OUT bit in the DMAR table. <b>1=Enable/Set</b> and 0=Disable/Clear
  BOOLEAN       InterruptRemappingSupport;         ///< This field is used to enable/disable  VT-d Interrupt remapping
  UINT32        BaseAddress[SA_VTD_ENGINE_NUMBER]; ///< This field is used to describe the base addresses for VT-d function
  UINT32        RmrrUsbBaseAddress;
  UINT32        RmrrUsbLimitAddress;
} SA_VTD_CONFIGURATION_HOB;

typedef struct {
  UINT8   EnableSsaClockGating;              ///< Used to get clock gating policy value from set up option
  UINT8   MsiRedirAlgorithm;                 ///< MSI Algorithm
  UINT8   SchedulerLatency;					 ///< Scheduler Latency value
  UINT8   CosCatAgent[COS_CAT_AGENT_COUNT];
  UINT8   BestEffortMaxLat;
} SA_PLATFORM_CONFIGURATION_HOB;


///
/// System Agent Data Hob
///
typedef struct {
  EFI_HOB_GUID_TYPE              EfiHobGuidType;                    ///< GUID Hob type structure
  SA_VTD_CONFIGURATION_HOB       VtdData;                           ///< VT-d Data HOB
  SA_PLATFORM_CONFIGURATION_HOB  SaPlatform;
} SA_DATA_HOB;

#endif
