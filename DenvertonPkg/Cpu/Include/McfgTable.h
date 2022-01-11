/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  McfgTable.h

Abstract:

  ACPI Memory mapped configuration space base address Description Table
  definition, based on PCI Firmware Specification Revision 3.0 final draft,
  downloadable at http://www.pcisig.com/home

--*/

#ifndef _MCFG_TABLE_H_
#define _MCFG_TABLE_H_

//
// Include files
//
#include <PiDxe.h>

//
// Ensure proper structure formats
//
#pragma pack(1)
//
// MCFG Revision (defined in spec)
//
#define EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_REVISION  0x01

//
// MCFG Structure Definitions
//
//
// Memory Mapped Enhanced Configuration Base Address Allocation
// Structure Definition
//
typedef struct {
  UINT64  BaseAddress;
  UINT16  PciSegmentGroupNumber;
  UINT8   StartBusNumber;
  UINT8   EndBusNumber;
  UINT32  Reserved;
} EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_BASE_ADDRESS_STRUCTURE;

//
// MCFG Table header definition.  The rest of the table
// must be defined in a platform specific manner.
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT64                      Reserved;
} EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER;

#pragma pack()

#endif // _MCFG_TABLE_H
