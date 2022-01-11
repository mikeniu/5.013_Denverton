/*++
 Copyright (c)  2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.


 Module Name:

 RasPolicy.h

 Abstract:

 Driver for Ras Policy structure.

 Revision History

 --*/

#ifndef _RAS_POLICY_H_
#define _RAS_POLICY_H_

//
// --------------------- RAS Policy Structure ------------------------------
//
struct RAS_GLOBAL_POLICY{
  UINT8  SystemErrorEn;
  UINT8  ErrorCloakingEnable;
  UINT8  MemoryErrorLog;
  UINT8  MemoryCorrErrorLog;
  UINT8  MemoryUnCorrErrorLog;
  UINT8  PCIeErrorLog;
  UINT8  PCIeCorrErrorLog;
  UINT8  PCIeNFErrorLog;
  UINT8  PCIeFatalErrorLog;
  UINT8  PCIeSystemErrEnable;
  UINT8  PCIeParityErrEnable;
  UINT8  RCECErrorLog;
  UINT8  BUnitErrorLog;
  UINT8  WheaSupport;
  UINT8  WheaEinj_05Extn;
  UINT8  WheaLogging;
} ;

extern EFI_GUID gRasPolicyHobGuid;

#endif
