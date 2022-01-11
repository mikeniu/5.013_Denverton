/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MemoryErrorHandler.h

Abstract:


------------------------------------------------------------------*/

#ifndef _MEM_ERROR_HANDLER_H
#define _MEM_ERROR_HANDLER_H

//
//DUNIT Register
//
#define R_DECCCTRL               0x180
#define   B_ECCEN                BIT0

#define R_UCELOG                 0x64
#define   B_UCEBNK               (BIT16|BIT17|BIT18|BIT19)
#define   B_UCERNK               (BIT20|BIT21)
#define R_UCEADDR                0x68
#define R_SBELOG                 0x6C
#define   B_SBEBNK               (BIT16|BIT17|BIT18|BIT19)
#define   B_SBERNK               (BIT20|BIT21)
#define R_SBEADDR                0x70

//Channel 0 and 1 Error status register
#define R_DERRSTS                0x124
#define   B_SBE                 BIT0
#define   B_UCE                 BIT1
#define   B_WPE                 BIT2
#define   B_APE                 BIT3
#define   B_UCEPAT               BIT4
#define   B_ALERT                BIT5
#define   B_CAP_ERR_STATE        BIT6
//Channel 0 and 1 Error Mask and severity
#define R_DERRMSKSEV             0x128
#define   B_SEV_UCE              BIT1
#define   B_SEV_WDBR             BIT2
#define   B_SEV_APE0             BIT3
#define   B_SEV_UCEPAT           BIT4
#define   B_MSK_SBE              BIT16
#define   B_MSK_UCE              BIT17
#define   B_MSK_WDBW             BIT18
#define   B_MSK_APE              BIT19
#define   B_MSK_UCEPAT           BIT20

#define DUNIT_CH0                0
#define DUNIT_CH1                1
#define MAX_CHANNELS             2

VOID
EnableElogDUnit(
);

BOOLEAN
ElogMemoryErrorHandler 
(
  );

VOID
ClearDUnitErrors(
  )
/*++

Routine Description:

  Clear Dunit errors before enabling them.

Arguments:
  
  None 

Returns:

  None

--*/
;
#endif

