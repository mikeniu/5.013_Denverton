/*++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Bdat.h

Abstract:

  This file describes the contents of the BDAT ACPI.

--*/

#ifndef _ACPI_BDAT_H_
#define _ACPI_BDAT_H_

//
// Statements that include other files
//
#include <IndustryStandard\Acpi.h>

//
// Ensure proper structure formats
//
#pragma pack(1)

#define EFI_BDAT_TABLE_SIGNATURE          SIGNATURE_32('B','D','A','T')

//
// BIOS Data ACPI structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  BdatGas;
} EFI_BDAT_ACPI_DESCRIPTION_TABLE;

typedef struct {
  UINT8     BiosDataSignature[8]; // "BDATHEAD"
  UINT32    BiosDataStructSize;   // sizeof BDAT_STRUCTURE
  UINT16    Crc16;                // 16-bit CRC of BDAT_STRUCTURE (calculated with 0 in this field)
  UINT16    Reserved;
  UINT16    PrimaryVersion;       // Primary version
  UINT16    SecondaryVersion;     // Secondary version
  UINT32    OemOffset;            // Optional offset to OEM-defined structure
  UINT32    Reserved1;
  UINT32    Reserved2;
} BDAT_HEADER_STRUCTURE;

typedef struct {
  UINT16  SchemaListLength;
  UINT16  Reserved;
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
  UINT8   Reserved1;
  UINT32  Schemas[1];
} BDAT_SCHEMA_LIST_STRUCTURE;

typedef struct {
  BDAT_HEADER_STRUCTURE       BdatHeader;
  BDAT_SCHEMA_LIST_STRUCTURE  BdatSchemas;
} BDAT_STRUCTURE;

//
// BIOS Data Parameter Region Generic Address
//
#define EFI_BDAT_ACPI_POINTER             0x0

#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)

//
// This is copied from Include\Acpi.h
//
#define CREATOR_ID_INTEL 0x4C544E49  //"LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

//#define EFI_ACPI_TABLE_VERSION_ALL  (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_2_0|EFI_ACPI_TABLE_VERSION_3_0)  Denverton AptioV override: Used Definition from AmiProtocol.h

#pragma pack()

#endif
