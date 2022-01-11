/** @file
  Header file for FIA Mux Policy.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
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

#ifndef _FIA_MUX_POLICY_H_
#define _FIA_MUX_POLICY_H_

#include <Library/MeFiaMuxLib.h>

#pragma pack(1)
///
/// The FIA_MUX_CONFIG block describes the expected configuration of
/// FIA MUX configuration.
///
typedef struct {
  UINT32         SkuNumLanesAllowed;   // Platform view of Num Lanes allowed
  ME_FIA_CONFIG  FiaMuxConfig;         // Current Platform FIA MUX Configuration
  ME_FIA_CONFIG  FiaMuxConfigRequest;  // FIA MUX Configuration Requested
} FIA_MUX_CONFIG;

///
/// The FIA_MUX_CONFIG_STATUS describes the status of configuring
/// FIA MUX configuration.
///
typedef struct {
	UINT64     FiaMuxConfigGetStatus;  // Status returned from FiaMuxConfigGet, if not EFI_SUCCESS, then error occurred and user can decide on next steps
	UINT64     FiaMuxConfigSetStatus;  // Status returned from FiaMuxConfigSet, if not EFI_SUCCESS, then error occurred and user can decide on next steps
	BOOLEAN    FiaMuxConfigSetRequired;  // Boolean: true - a FiaMuxConfigSet was required, false otherwise
} FIA_MUX_CONFIG_STATUS;

//Global FIA MUX configure structure
typedef struct FIA_MUX_GLOBAL_POLICY {
  FIA_MUX_CONFIG              FiaMuxConfig;
  FIA_MUX_CONFIG_STATUS       FiaMuxConfigStatus;
};

#pragma pack()

#endif //_FIA_MUX_POLICY_H_
