/*
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SimicsDebug.h

Abstract:

  Definitions for using SIMICS

*/

#ifndef SIMICSDEBUG_H_
#define SIMICSDEBUG_H_

#define SIMICS_BUS          0
#define SIMICS_DEV          0
#define SIMICS_FUN          0
#define SIMICS_REGISTER     0xDC

#define DXE_IN_SIMICS 	(PciRead32( PCI_LIB_ADDRESS( SIMICS_BUS, SIMICS_DEV, SIMICS_FUN, SIMICS_REGISTER ) ) == 1)
#define PEI_IN_SIMICS 	(MmioRead32( ( UINTN ) ( MmPciBase( SIMICS_BUS, SIMICS_DEV, SIMICS_FUN ) + SIMICS_REGISTER ) ) == 1)

#define DXE_IN_EMULATION 	(PciRead32( PCI_LIB_ADDRESS( SIMICS_BUS, SIMICS_DEV, SIMICS_FUN, SIMICS_REGISTER ) ) == 2)
#define PEI_IN_EMULATION 	(MmioRead32( ( UINTN ) ( MmPciBase( SIMICS_BUS, SIMICS_DEV, SIMICS_FUN ) + SIMICS_REGISTER ) ) == 2)

#endif /* SIMICSDEBUG_H_ */
