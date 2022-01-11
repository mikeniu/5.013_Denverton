/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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
#ifndef _VTD_CONFIG_H_
#define _VTD_CONFIG_H_

#define SA_VTD_ENGINE_NUMBER        1

#define VTD_CONFIG_REVISION 1
///
/// The data elements should be initialized by a Platform Module.
/// The data structure is for VT-d driver initialization
///
typedef struct _VTD_CONFIG{
  UINT16        BlockId;
  UINT8         BlockRevision;
  UINT16        BlockSize;
  UINT32        VtdEnable                  : 1;                        ///< This field is used to describe whether or not the VT-d function should be enabled
  UINT32        X2ApicOptOut               : 1;                        ///< This field is used to enable the X2APIC_OPT_OUT bit in the DMAR table. <b>1=Enable/Set</b> and 0=Disable/Clear
  UINT32        InterruptRemappingSupport  : 8;                        ///< This field is used to enable/disable  VT-d Interrupt remapping
  UINT32        Reserved                   : 22;                       ///< Reserved bits for future use
  UINT32        BaseAddress[SA_VTD_ENGINE_NUMBER];                    ///< This field is used to describe the base addresses for VT-d function
  UINT32        RmrrUsbBaseAddress;
  UINT32        RmrrUsbLimitAddress;
} VTD_CONFIG;

#endif   //  _VTD_CONFIG_H_
