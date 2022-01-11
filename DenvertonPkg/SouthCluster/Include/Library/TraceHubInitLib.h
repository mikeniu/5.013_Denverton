/** @file
  Header file for TraceHub Init Lib.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

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

@par Specification Reference:
**/
#ifndef _TRACE_HUB_INIT_LIB_H_
#define _TRACE_HUB_INIT_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <PchAccess.h>
#include <Ppi/PchPolicy.h>

///
/// The PCH_TRACE_HUB_ENABLE_MODE describes the desired TraceHub mode of operation
///
typedef enum {
  TraceHubModeDisabled       = 0,   ///< Pch TraceHub Disabled
  TraceHubModeHostDebugger   = 2,   ///< Pch TraceHub External Debugger Present
  TraceHubModeMax
} TRACE_HUB_ENABLE_MODE;
//
// TraceHub Supported Destinations
//
typedef enum {
   TraceHubTraceDestMem    = 0,
   TraceHubTraceDestPti    = 1,
   TraceHubTraceDestUsb    = 2,
   TraceHubTraceDestBssb   = 3,
   TraceHubTraceDestMax
} TRACE_HUB_TRACE_DESTINATION;

/**
  The function disables all messages between Punit and TraceHub (NPK).

  @param[in]      None
  @param[in out]  None

  @retval         EFI_INVALID_PARAMETER   Intended address to access is not a valid address
                  EFI_UNSUPPORTED         Command is not supported by the B2P mailbox or the PUnit
                  EFI_SUCCESS             Punit to TraceHub messages are disabled
                  EFI_ERROR               Punit completion acknowledge was not received
**/
EFI_STATUS
TraceHubDisablePunitMgs (
  );

VOID
TraceHubInitialize (
  IN  PCH_POLICY_PPI *PchPolicyPpi
 );
#endif // _TRACE_HUB_INIT_LIB_H_
