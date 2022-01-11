/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c) 2015-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PeiPcieIpInitLib.h

Abstract:

  Driver for Pcie IP Initialization library.

Revision History

--*/

#ifndef _DXE_PCIE_IP_INIT_LIB_H_
#define _DXE_PCIE_IP_INIT_LIB_H_


//
// RCEC Definitions - for For RAS RCEC Error Enable to IEH pre/post PCIe Enum
//

#define  RCEC_BUS                            0x00     //Root complex event collector Bus number
#define  RCEC_DEVICE                         0x05     //Root complex event collector Device number
#define  RCEC_FUNC                           0x00     //Root complex event collector function number
#define  R_RCEC_DEVSTS                       0x4A
#define  B_RCEC_DEVSTS_CED                   BIT0
#define  B_RCEC_DEVSTS_NFED                  BIT1
#define  B_RCEC_DEVSTS_FED                   BIT2
#define  B_RCEC_DEVSTS_URD                   BIT3
#define  R_RCEC_ERRUNCSTS                    0x104
#define  B_RCEC_ERRUNCSTS_URE                BIT20
#define  R_RCEC_ERRCORSTS                    0x110
#define  B_RCEC_ERRCORSTS_ANFE               BIT13
#define  R_RCEC_ERRUNCMSK                    0x108
#define  B_RCEC_ERRUNCMSK_UREM               BIT20


extern EFI_GUID gPcieIpPolicyHobGuid;

EFI_STATUS
DxePcieIpInit ( VOID );

UINTN
DxePcieIpGetRootPortBase (
  IN  UINT8  RpIndex
);

#endif /* _DXE_PCIE_IP_INIT_LIB_H_ */
