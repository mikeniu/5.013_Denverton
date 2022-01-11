/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MmioAccess.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/

#ifndef _MMIOACCESS_H_INCLUDED_
#define _MMIOACCESS_H_INCLUDED_

#define MmioAddress( BaseAddr, Register ) \
  ( (UINTN)BaseAddr + \
    (UINTN)(Register) \
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

//
// UINT8
//

#define Mmio8Ptr( BaseAddr, Register ) \
  ( (volatile UINT8 *)MmioAddress( BaseAddr, Register ) )

#define Mmio8( BaseAddr, Register ) \
  *Mmio8Ptr( BaseAddr, Register )


#endif
