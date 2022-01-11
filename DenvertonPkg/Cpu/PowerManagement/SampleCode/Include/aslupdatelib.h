/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to  
  the additional terms of the license agreement               
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

  AslUpdateLib.h

Abstract:

  ASL dynamic update library definitions.

  This library provides dymanic update to various ASL structures.

  There may be different libraries for different environments (PEI, BS, RT, SMM).
  Make sure you meet the requirements for the library (protocol dependencies, use
  restrictions, etc).

--*/

#ifndef _ASL_UPDATE_LIB_H_
#define _ASL_UPDATE_LIB_H_

//
// Include files
//
#include "Tiano.h"
#include "Acpi.h"
#include "Acpi3_0.h"

#include EFI_PROTOCOL_DEPENDENCY (AcpiSupport)

//
// AML parsing definitions
//
#define AML_NAME_OP       0x08
#define AML_SCOPE_OP      0x10
#define AML_PACKAGE_OP    0x12
#define AML_METHOD_OP     0x14
#define AML_OPREGION_OP   0x80
#define AML_DEVICE_OP     0x82
#define AML_PROCESSOR_OP  0x83

//
// Magic number definition for values to be updated
//
#define UINT16_BIT_MAGIC_NUMBER 0xFFFF
#define UINT32_BIT_MAGIC_NUMBER 0xFFFFFFFF

//
// ASL PSS package structure layout
//
#pragma pack(1)
typedef struct {
  UINT8   NameOp;         // 12h ;First opcode is a NameOp.
  UINT8   PackageLead;    // 20h ;First opcode is a NameOp.
  UINT8   NumEntries;     // 06h ;First opcode is a NameOp.
  UINT8   DwordPrefix1;   // 0Ch
  UINT32  CoreFrequency;  // 00h
  UINT8   DwordPrefix2;   // 0Ch
  UINT32  Power;          // 00h
  UINT8   DwordPrefix3;   // 0Ch
  UINT32  TransLatency;   // 00h
  UINT8   DwordPrefix4;   // 0Ch
  UINT32  BMLatency;      // 00h
  UINT8   DwordPrefix5;   // 0Ch
  UINT32  Control;        // 00h
  UINT8   DwordPrefix6;   // 0Ch
  UINT32  Status;         // 00h
} PSS_PACKAGE_LAYOUT;
#pragma pack()

EFI_STATUS
InitializeAslUpdateLib (
  VOID
  )
/*++

Routine Description:

  Initialize the ASL update library state.
  This must be called prior to invoking other library functions.

Arguments:

  None

Returns: 

  EFI_SUCCESS                  - The function completed successfully.

--*/
;

EFI_STATUS
UpdateAslCode (
  IN     UINT32                        AslSignature,
  IN     UINT16                        BaseAddress,
  IN     UINT8                         Length
  )
/*++

Routine Description:

  This function locates an ACPI structure and updates it.
  This function knows how to update operation regions and BUFA/BUFB resource structures.

  This function may not be implemented in all instantiations of this library.

Arguments:

  AslSignature      - The signature of Operation Region that we want to update.
  BaseAddress       - Base address of IO trap.
  Length            - Length of IO address.

Returns: 

  EFI_SUCCESS       - The function completed successfully.

--*/
;

EFI_STATUS
LocateAcpiTableBySignature (
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle,
  IN OUT  EFI_ACPI_TABLE_VERSION        *Version
  )
/*++

Routine Description:

  This function uses the ACPI support protocol to locate an ACPI table using the .
  It is really only useful for finding tables that only have a single instance,
  e.g. FADT, FACS, MADT, etc.  It is not good for locating SSDT, etc.
  Matches are determined by finding the table with ACPI table that has
  a matching signature and version.

Arguments:

  TableId           - Pointer to an ASCII string containing the Signature to match
  Table             - Updated with a pointer to the table
  Handle            - AcpiSupport protocol table handle for the table found
  Version           - On input, the version of the table desired,
                      on output, the versions the table belongs to
                      (see AcpiSupport protocol for details)

Returns: 

  EFI_SUCCESS       - The function completed successfully.

--*/
;

EFI_STATUS
LocateAcpiTableByOemTableId (
  IN      UINT8                         *TableId,
  IN      UINT8                         TableIdSize,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle,
  IN OUT  EFI_ACPI_TABLE_VERSION        *Version
  )
/*++

Routine Description:

  This function uses the ACPI support protocol to locate an ACPI SSDT table.
  The table is located by searching for a matching OEM Table ID field.
  Partial match searches are supported via the TableIdSize parameter.

Arguments:

  TableId           - Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  TableIdSize       - Length of the TableId to match.  Table ID are 8 bytes long, this function
                      will consider it a match if the first TableIdSize bytes match
  Table             - Updated with a pointer to the table
  Handle            - AcpiSupport protocol table handle for the table found
  Version           - See AcpiSupport protocol, GetAcpiTable function for use

Returns: 

  EFI_SUCCESS       - The function completed successfully.

--*/
;

EFI_STATUS
EFI_BOOTSERVICE
AcpiChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  )
/*++

Routine Description: 

  This function calculates and updates an UINT8 checksum.

Arguments:  

  Buffer          Pointer to buffer to checksum
  Size            Number of bytes to checksum
  ChecksumOffset  Offset to place the checksum result in

Returns:  

  EFI_SUCCESS             The function completed successfully.

--*/
;
#endif
