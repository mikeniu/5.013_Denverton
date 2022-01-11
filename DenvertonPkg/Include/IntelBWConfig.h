/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
	Some configuration of BW Package

Copyright (c) 2010 Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef __INTEL_BW_CONFIG_H__
#define __INTEL_BW_CONFIG_H__

typedef union {
  struct {
    UINT32  PortErrorMask               :8;
    UINT32  SlotImplemented             :1;
    UINT32  Reserved1                   :1;
    UINT32  AspmEnable                  :1;
    UINT32  AspmAutoEnable              :1;
    UINT32  AspmL0sEnable               :2;
    UINT32  AspmL1Enable                :1;
    UINT32  PmeInterruptEnable          :1;    
    UINT32  PhysicalSlotNumber          :13;    
    UINT32  Reserved2                   :1; 
    UINT32  PmSciEnable                 :1;   
    UINT32  HotplugSciEnable            :1;
  } Bits;
  UINT32 Uint32;
} PCIEXP_ROOT_PORT_CONFIGURATION;

typedef union {
  UINT32 Uint32;
  struct {
	  UINT32 Pcie_0     :1;   // 0: Disabled; 1: Enabled*
	  UINT32 Pcie_1     :1;   // 0: Disabled; 1: Enabled*
	  UINT32 Pcie_2     :1;   // 0: Disabled; 1: Enabled*
	  UINT32 Pcie_3     :1;   // 0: Disabled; 1: Enabled*
	  UINT32 Smbus      :1;   // 0: Disabled; 1: Enabled*   
	  UINT32 Rsvd       :27;  // 0
  } Bits;
}BW_DEVICE_ENABLES;

#endif

