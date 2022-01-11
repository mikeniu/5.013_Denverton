/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

@copyright
 Copyright (c) 2014-2015 Intel Corporation. All rights reserved
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
#ifndef _NV_MEMORY_CONFIG_H_
#define _NV_MEMORY_CONFIG_H_


#define NV_MEMORY_CONFIG_REVISION 1

//
// NonVolatile Memory configuration parameters
//
typedef struct {
  UINT16  BlockId;
  UINT8   BlockRevision;
  UINT16  BlockSize;
  UINT8   Mode;
  UINT8   Interleaving;
  UINT32  Size;
  UINT8   AdrRecoveryDetected;
  UINT8   eraseArmNVDIMMS;
  UINT8   restoreNVDIMMS;
  UINTN   CkeLatchGpioCtrl;
} NV_MEMORY_CONFIGURATION;

#endif // _NV_MEMORY_CONFIG_H_
