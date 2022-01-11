/** @file
  Header file for IqatIp Policy.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

#ifndef _IQAT_IP_POLICY_H_
#define _IQAT_IP_POLICY_H_

///
/// The IQAT_IP_CONFIG block describes the expected configuration of the Intel QuickAssist
/// Technology controller.
///
typedef struct {
  ///
  /// This member describes whether or not the Intel QuickAssist Technology controller should be enabled.
  ///
  BOOLEAN                 Enable;
  UINT8  SetFuseCtl;    // 0: Disable; 1: Enable the setting of FUSECTL
  UINT8  FuseCtlValue;  // FUSECTL value to set
  UINT8  Set64BMrrMpl;  // 0: Disable; 1: Enable the setting of 64B MRR/MPL
} IQAT_IP_CONFIG;

//Global IQAT structure
struct IQAT_IP_GLOBAL_POLICY {
	  ///
	  /// This member describes the Intel QuickAssist Technology configuration
	  ///
	  IQAT_IP_CONFIG              IqatIpConfig;
};

#endif //_IQAT_IP_POLICY_H_
