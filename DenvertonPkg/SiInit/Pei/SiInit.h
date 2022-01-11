/** @file
  Header file for Silicon Init PEIM.

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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

@par Specification
**/
#ifndef _SI_INIT_MODULE_H_
#define _SI_INIT_MODULE_H_

#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/SiPolicy.h>
#include <IncludePrivate/Library/PchInitLib.h>

#include <PcieIpInit/Include/PeiPcieIpInitLib.h>
#include <PcieIpInit/Include/Ppi/PeiPcieIpPolicyPpi.h>
#include <PcieIpInit/Include/PeiPcieIpPolicyLib.h>
#include <HsuartIpInit/Include/PeiHsuartIpInitLib.h>
#include <IqatIpInit/Include/PeiIqatIpInitLib.h>
#include <IqatIpInit/Include/IqatIpPolicyLib.h>
#include <RasPolicyInit/Include/PeiRasPolicyLib.h>
#include <FiaMuxInit/Include/Library/PeiFiaMuxConfigInitLib.h>
#ifdef ME_SUPPORT_FLAG
#include <Include/Library/MeLibPei.h>
#endif
#include <StPgIpInit/Include/PchPmcStPg.h>
#include <Include/Library/ConfigBlockLib.h>
#include <IncludePrivate/Library/VtdInitLib.h>
#include <Ppi/PchReset.h>
#include <SimicsDebug.h>

//
// Core Disable
//
#define PCH_PCR_CORE_EXISTS_VECTOR_MCHBAR                   0x7164
#define PCH_PCR_CORE_DISABLE_MASK_MCHBAR                    0x7168
#define PCH_PCR_CORE_DISABLE_MASK_VALUE                     (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define PCH_PCR_CORE_DISABLE_MASK_CORE0                     BIT0
#define PCH_PCR_CORE_DISABLE_MASK_CORE1                     BIT1
#define PCH_PCR_CORE_DISABLE_MASK_CORE2                     BIT2
#define PCH_PCR_CORE_DISABLE_MASK_CORE3                     BIT3
#define PCH_PCR_CORE_DISABLE_MASK_CORE4                     BIT4
#define PCH_PCR_CORE_DISABLE_MASK_CORE5                     BIT5
#define PCH_PCR_CORE_DISABLE_MASK_CORE6                     BIT6
#define PCH_PCR_CORE_DISABLE_MASK_CORE7                     BIT7
#define PCH_PCR_CORE_DISABLE_MASK_CORE8                     BIT8
#define PCH_PCR_CORE_DISABLE_MASK_CORE9                     BIT9
#define PCH_PCR_CORE_DISABLE_MASK_CORE10                    BIT10
#define PCH_PCR_CORE_DISABLE_MASK_CORE11                    BIT11
#define PCH_PCR_CORE_DISABLE_MASK_CORE12                    BIT12
#define PCH_PCR_CORE_DISABLE_MASK_CORE13                    BIT13
#define PCH_PCR_CORE_DISABLE_MASK_CORE14                    BIT14
#define PCH_PCR_CORE_DISABLE_MASK_CORE15                    BIT15

#define CORE_EXISTS_VECTOR_DEFAULT_VALUE                    0x00FF
#define MAX_NUM_MODULES                                     8
#define MAX_NUM_CORES_PMODULE                               2

#define ALL_CORES                                           0
#define ALL_CORES_ODDS                                      0xAAAA
#define ALL_CORES_PAIRS                                     0x5555
#define CORE_1                                              0xFFFE
#define CORE_2                                              0xFFFA
#define CORE_3                                              0xFFEA
#define CORE_4                                              0xFFAA
#define CORE_5                                              0xFEAA
#define CORE_6                                              0xFAAA
#define CORE_7                                              0xEAAA
#define CORE_8                                              0xAAAA
#define CORE_9                                              0xAAA8
#define CORE_10                                             0xAAA0
#define CORE_11                                             0xAA80
#define CORE_12                                             0xAA00
#define CORE_13                                             0xA800
#define CORE_14                                             0xA000
#define CORE_15                                             0x8000
#define CORE_16                                             0x0000

//
// GUID Definitions
//
extern EFI_GUID gSiPolicyHobGuid;

//
// Post Mem Performance GUIDs
//
extern EFI_GUID gPerfPchPostMemGuid;
#ifdef ME_SUPPORT_FLAG
extern EFI_GUID gPerfMePostMemGuid;
#endif
#endif // _SI_INIT_MODULE_H_
