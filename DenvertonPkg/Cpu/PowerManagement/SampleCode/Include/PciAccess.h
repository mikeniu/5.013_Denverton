//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//

/*++

Copyright (c)  1999 - 2008 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PciAccess.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/

#ifndef _PCIACCESS_H_INCLUDED_
#define _PCIACCESS_H_INCLUDED_

#define PCIEX_BASE_ADDRESS  0xE0000000
#define PCI_EXPRESS_BASE_ADDRESS ((VOID *) (UINTN) PCIEX_BASE_ADDRESS)

#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)PCI_EXPRESS_BASE_ADDRESS + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )


//
// UINT32
//

#define MmPci32Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT32 *) MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci32( Segment, Bus, Device, Function, Register ) \
  *MmPci32Ptr( Segment, Bus, Device, Function, Register )

#define MmPci32Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci32( Segment, Bus, Device, Function, Register ) = \
    (UINT32) ( \
      MmPci32( Segment, Bus, Device, Function, Register ) | \
      (UINT32)(OrData) \
    )

#define MmPci32And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci32( Segment, Bus, Device, Function, Register ) = \
    (UINT32) ( \
      MmPci32( Segment, Bus, Device, Function, Register ) & \
      (UINT32)(AndData) \
    )

#define MmPci32AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci32( Segment, Bus, Device, Function, Register ) = \
    (UINT32) ( \
      ( MmPci32( Segment, Bus, Device, Function, Register ) & \
          (UINT32)(AndData) \
      ) | \
      (UINT32)(OrData) \
    )

//
// UINT16
//

#define MmPci16Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT16 *)MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci16( Segment, Bus, Device, Function, Register ) \
  *MmPci16Ptr( Segment, Bus, Device, Function, Register )

#define MmPci16Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci16( Segment, Bus, Device, Function, Register ) = \
    (UINT16) ( \
      MmPci16( Segment, Bus, Device, Function, Register ) | \
      (UINT16)(OrData) \
    )

#define MmPci16And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci16( Segment, Bus, Device, Function, Register ) = \
    (UINT16) ( \
      MmPci16( Segment, Bus, Device, Function, Register ) & \
      (UINT16)(AndData) \
    )

#define MmPci16AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci16( Segment, Bus, Device, Function, Register ) = \
    (UINT16) ( \
      ( MmPci16( Segment, Bus, Device, Function, Register ) & \
          (UINT16)(AndData) \
      ) | \
      (UINT16)(OrData) \
    )

//
// UINT8
//

#define MmPci8Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT8 *)MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci8( Segment, Bus, Device, Function, Register ) \
  *MmPci8Ptr( Segment, Bus, Device, Function, Register )

#define MmPci8Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci8( Segment, Bus, Device, Function, Register ) = \
    (UINT8) ( \
      MmPci8( Segment, Bus, Device, Function, Register ) | \
      (UINT8)(OrData) \
    )

#define MmPci8And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci8( Segment, Bus, Device, Function, Register ) = \
    (UINT8) ( \
      MmPci8( Segment, Bus, Device, Function, Register ) & \
      (UINT8)(AndData) \
    )

#define MmPci8AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci8( Segment, Bus, Device, Function, Register ) = \
    (UINT8) ( \
      ( MmPci8( Segment, Bus, Device, Function, Register ) & \
          (UINT8)(AndData) \
        ) | \
      (UINT8)(OrData) \
    )

#endif
