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

  IchAccess.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/

#ifndef _ICHACCESS_H_INCLUDED_
#define _ICHACCESS_H_INCLUDED_

#include "IchTpt.h"
#include "IchCommonDefinitions.h"

#define EFI_P2P_PCI_ADDRESS( Register ) \
  EFI_PCI_ADDRESS(PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, PCI_FUNCTION_NUMBER_ICH_LPC, Register)

#define EFI_LPC_PCI_ADDRESS( Register ) \
  EFI_PCI_ADDRESS(PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, Register)

//
// Ich Controller PCI access macros
//
#define ICH_RCRB_BASE (IchMmio32 (IchPciDeviceMmBase (0, PCI_DEVICE_NUMBER_ICH_LPC, 0), R_ICH_LPC_RCBA) & ~BIT0)

// Device 0x1e, Function 0
         
#define P2pPciCfg32( Register ) \
  IchMmPci32( 0, PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register )

#define P2pPciCfg32Or( Register, OrData ) \
  IchMmPci32Or( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, OrData )

#define P2pPciCfg32And( Register, AndData ) \
  IchMmPci32And( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, AndData )

#define P2pPciCfg32AndThenOr( Register, AndData, OrData ) \
  IchMmPci32AndThenOr( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, AndData, OrData )

#define P2pPciCfg16( Register ) \
  IchMmPci16( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register )

#define P2pPciCfg16Or( Register, OrData ) \
  IchMmPci16Or( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, OrData )

#define P2pPciCfg16And( Register, AndData ) \
  IchMmPci16And( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, AndData )

#define P2pPciCfg16AndThenOr( Register, AndData, OrData ) \
  IchMmPci16AndThenOr( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, AndData, OrData )

#define P2pPciCfg8( Register ) \
  IchMmPci8( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register )

#define P2pPciCfg8Or( Register, OrData ) \
  IchMmPci8Or( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, OrData )

#define P2pPciCfg8And( Register, AndData ) \
  IchMmPci8And( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, AndData )

#define P2pPciCfg8AndThenOr( Register, AndData, OrData ) \
  IchMmPci8AndThenOr( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_HL2P, 0, Register, AndData, OrData )

//
// Device 0x1f, Function 0
//

#define LpcPciCfg32( Register ) \
  IchMmPci32(0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register )

#define LpcPciCfg32Or( Register, OrData ) \
  IchMmPci32Or( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, OrData )

#define LpcPciCfg32And( Register, AndData ) \
  IchMmPci32And( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, AndData )

#define LpcPciCfg32AndThenOr( Register, AndData, OrData ) \
  IchMmPci32AndThenOr( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, AndData, OrData )

#define LpcPciCfg16( Register ) \
  IchMmPci16( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register )

#define LpcPciCfg16Or( Register, OrData ) \
  IchMmPci16Or(  0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, OrData )

#define LpcPciCfg16And( Register, AndData ) \
  IchMmPci16And( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, AndData )

#define LpcPciCfg16AndThenOr( Register, AndData, OrData ) \
  IchMmPci16AndThenOr( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, AndData, OrData )

#define LpcPciCfg8( Register ) \
  IchMmPci8( 0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register )

#define LpcPciCfg8Or( Register, OrData ) \
  IchMmPci8Or(  0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, OrData )

#define LpcPciCfg8And( Register, AndData ) \
  IchMmPci8And(  0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, AndData )

#define LpcPciCfg8AndThenOr( Register, AndData, OrData ) \
  IchMmPci8AndThenOr(  0,PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, 0, Register, AndData, OrData )

//
// Root Complex Register Block
//

#define MmRcrb32( Register ) \
  IchMmio32( ICH_RCRB_BASE, Register )

#define MmRcrb32Or( Register, OrData ) \
  IchMmio32Or( ICH_RCRB_BASE, Register, OrData )

#define MmRcrb32And( Register, AndData ) \
  IchMmio32And( ICH_RCRB_BASE, Register, AndData )

#define MmRcrb32AndThenOr( Register, AndData, OrData ) \
  IchMmio32AndThenOr( ICH_RCRB_BASE, Register, AndData, OrData )

#define MmRcrb16( Register ) \
  IchMmio16( ICH_RCRB_BASE, Register )

#define MmRcrb16Or( Register, OrData ) \
  IchMmio16Or( ICH_RCRB_BASE, Register, OrData )

#define MmRcrb16And( Register, AndData ) \
  IchMmio16And( ICH_RCRB_BASE, Register, AndData )

#define MmRcrb16AndThenOr( Register, AndData, OrData ) \
  IchMmio16AndThenOr( ICH_RCRB_BASE, Register, AndData, OrData )

#define MmRcrb8( Register ) \
  IchMmio8( ICH_RCRB_BASE, Register )

#define MmRcrb8Or( Register, OrData ) \
  IchMmio8Or( ICH_RCRB_BASE, Register, OrData )

#define MmRcrb8And( Register, AndData ) \
  IchMmio8And( ICH_RCRB_BASE, Register, AndData )

#define MmRcrb8AndThenOr( Register, AndData, OrData ) \
  IchMmio8AndThenOr( ICH_RCRB_BASE, Register, AndData, OrData )

#endif

