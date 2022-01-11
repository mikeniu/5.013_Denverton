/** @file
  Macros to simplify and abstract the interface to PCI configuration.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _SAACCESS_H_
#define _SAACCESS_H_

#include "SaRegs.h"
#include "SaCommonDefinitions.h"

#define MCH_BASE_ADDRESS	0xFED10000

///
/// SystemAgent Base Address definition
///
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND  1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND  1000
#endif

//
// SA DMI configuration
//
#define SA_DMI_MAX_LANE    0x04
#define SA_DMI_MAX_BUNDLE  0x02

//
// SA PCI Express* Port configuration
//
#define SA_PEG_BUS_NUM     0x00
#define SA_PEG_DEV_NUM     0x01
#define SA_PEG10_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG10_FUN_NUM   0x00
#define SA_PEG11_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG11_FUN_NUM   0x01
#define SA_PEG12_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG12_FUN_NUM   0x02
#define SA_PEG_MAX_FUN     0x03
#define SA_PEG_MAX_LANE    0x10
#define SA_PEG_MAX_BUNDLE  0x08

#define SA_SWING_HALF     0x1
#define SA_SWING_FULL     0x2

//
// PCI Express* Port configuration Hardware Strapping value
//
#define SA_PEG_x8_x4_x4  0x00
#define SA_PEG_x8_x8_x0  0x02
#define SA_PEG_x16_x0_x0 0x03

///
/// DMI/OPI macros
///
#define IsPchLinkDmi(CpuFamilyId) \
  ((CpuFamilyId == EnumCpuHsw) || \
   (CpuFamilyId == EnumCpuCrw) || \
  ((CpuFamilyId == EnumCpuBcw) && ((MmioRead8(MmPciBase (0, 0, 0) + R_SA_MC_DEVICE_ID) & BIT4) == BIT4)))

//=============================================================================
//              Standard Access Interfaces
//=============================================================================
//                      MMIO (Memory Mapped I/O registers)
//=============================================================================
//
// Memory Mapped IO access macros used by MSG BUS LIBRARY
//
#define MmioAddress( BaseAddr, Register ) \
  ( (UINTN)BaseAddr + \
    (UINTN)(Register) \
  )

//
// UINT64
//

#define Mmio64Ptr( BaseAddr, Register ) \
  ( (volatile UINT64 *)MmioAddress( BaseAddr, Register ) )

#define Mmio64( BaseAddr, Register ) \
  *Mmio64Ptr( BaseAddr, Register )

#define Mmio64Or( BaseAddr, Register, OrData ) \
  Mmio64( BaseAddr, Register ) = \
    (UINT64) ( \
      Mmio64( BaseAddr, Register ) | \
      (UINT64)(OrData) \
    )

#define Mmio64And( BaseAddr, Register, AndData ) \
  Mmio64( BaseAddr, Register ) = \
    (UINT64) ( \
      Mmio64( BaseAddr, Register ) & \
      (UINT64)(AndData) \
    )

#define Mmio64AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio64( BaseAddr, Register ) = \
    (UINT64) ( \
      ( Mmio64( BaseAddr, Register ) & \
          (UINT64)(AndData) \
      ) | \
      (UINT64)(OrData) \
    )
  
//
// UINT32
//

#define Mmio32Ptr( BaseAddr, Register ) \
  ( (volatile UINT32 *)MmioAddress( BaseAddr, Register ) )

#define Mmio32( BaseAddr, Register ) \
  *Mmio32Ptr( BaseAddr, Register )

#define Mmio32Or( BaseAddr, Register, OrData ) \
  Mmio32( BaseAddr, Register ) = \
    (UINT32) ( \
      Mmio32( BaseAddr, Register ) | \
      (UINT32)(OrData) \
    )

#define Mmio32And( BaseAddr, Register, AndData ) \
  Mmio32( BaseAddr, Register ) = \
    (UINT32) ( \
      Mmio32( BaseAddr, Register ) & \
      (UINT32)(AndData) \
    )

#define Mmio32AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio32( BaseAddr, Register ) = \
    (UINT32) ( \
      ( Mmio32( BaseAddr, Register ) & \
          (UINT32)(AndData) \
      ) | \
      (UINT32)(OrData) \
    )

//
// UINT16
//

#define Mmio16Ptr( BaseAddr, Register ) \
  ( (volatile UINT16 *)MmioAddress( BaseAddr, Register ) )

#define Mmio16( BaseAddr, Register ) \
  *Mmio16Ptr( BaseAddr, Register )

#define Mmio16Or( BaseAddr, Register, OrData ) \
  Mmio16( BaseAddr, Register ) = \
    (UINT16) ( \
      Mmio16( BaseAddr, Register ) | \
      (UINT16)(OrData) \
    )

#define Mmio16And( BaseAddr, Register, AndData ) \
  Mmio16( BaseAddr, Register ) = \
    (UINT16) ( \
      Mmio16( BaseAddr, Register ) & \
      (UINT16)(AndData) \
    )

#define Mmio16AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio16( BaseAddr, Register ) = \
    (UINT16) ( \
      ( Mmio16( BaseAddr, Register ) & \
          (UINT16)(AndData) \
      ) | \
      (UINT16)(OrData) \
    )

//
// UINT8
//

#define Mmio8Ptr( BaseAddr, Register ) \
  ( (volatile UINT8 *)MmioAddress( BaseAddr, Register ) )

#define Mmio8( BaseAddr, Register ) \
  *Mmio8Ptr( BaseAddr, Register )

#define Mmio8Or( BaseAddr, Register, OrData ) \
  Mmio8( BaseAddr, Register ) = \
    (UINT8) ( \
      Mmio8( BaseAddr, Register ) | \
      (UINT8)(OrData) \
    )

#define Mmio8And( BaseAddr, Register, AndData ) \
  Mmio8( BaseAddr, Register ) = \
    (UINT8) ( \
      Mmio8( BaseAddr, Register ) & \
      (UINT8)(AndData) \
    )

#define Mmio8AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio8( BaseAddr, Register ) = \
    (UINT8) ( \
      ( Mmio8( BaseAddr, Register ) & \
          (UINT8)(AndData) \
        ) | \
      (UINT8)(OrData) \
    )

#endif
