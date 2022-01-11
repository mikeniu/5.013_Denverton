//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AcpiPlatform.h
  
Abstract:

  This is an implementation of the ACPI platform driver.  Requirements for 
  this driver are defined in the Tiano ACPI External Product Specification,
  revision 0.3.6.

--*/

#ifndef _ACPI_PLATFORM_H_
#define _ACPI_PLATFORM_H_

//
// Statements that include other header files
//

#include <PiDxe.h>
#include <IntelBWConfig.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/MpService.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol\SerialIo.h>
#include <Protocol\LpcPolicy.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard\SerialPortConsoleRedirectionTable.h>
#include <Include/SetupVariable.h>
#include <Include/Guid/PlatformInfo.h>

#include "Madt.h"
#include "AcpiPciUpdate.h"

#pragma pack(1)
typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   OpCode;
  UINT16  Size;                // Hardcode to 16bit width because the table we use is fixed size
  UINT8   NumEntries;
} EFI_ACPI_NAME_COMMAND;

typedef struct {
  UINT8   PackageOp;
  UINT8   PkgLeadByte;
  UINT8   NumEntries;
  UINT8   DwordPrefix0;
  UINT32  CoreFreq;
  UINT8   DwordPrefix1;
  UINT32  Power;
  UINT8   DwordPrefix2;
  UINT32  TransLatency;
  UINT8   DwordPrefix3;
  UINT32  BMLatency;
  UINT8   DwordPrefix4;
  UINT32  Control;
  UINT8   DwordPrefix5;
  UINT32  Status;
} EFI_PSS_PACKAGE;
#pragma pack()


#define AML_NAME_OP               0x08
#define AML_METHOD_OP             0x14
#define AML_OPREGION_OP           0x80
#define AML_PACKAGE_OP            0x12    // Package operator.

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID           "INTEL "
#define EFI_ACPI_OEM_TABLE_ID     0x2020204F4E414954  // "TIANO   "
#define EFI_ACPI_OEM_REVISION     0x00000002
#define EFI_ACPI_CREATOR_ID       0x5446534D          // TBD "MSFT"
#define EFI_ACPI_CREATOR_REVISION 0x01000013          // TBD

#define ACPI_COMPATIBLE_1_0       0
#define ACPI_COMPATIBLE_2_0       1
#define ACPI_COMPATIBLE_3_0       2

/* // Denverton Aptiov override start
// DBG2 ACPI table is installed by "Debug Port Tables" module
//
// ACPI DBG2 - Debug Device Name Space String
//
#define ACPI_DBG2_DEFAULT_NAME_SPACE            "."
#define ACPI_DBG2_SERIALIO_UART0_NAME_SPACE     "\\_SB.PCI0.UA00"
#define ACPI_DBG2_SERIALIO_UART1_NAME_SPACE     "\\_SB.PCI0.UA01"
#define ACPI_DBG2_SERIALIO_UART2_NAME_SPACE     "\\_SB.PCI0.UA02"
*/ // Denverton Aptiov override End
//
// Private Driver Data
//

//
// Define Union of IO APIC & Local APIC structure;
//

typedef union {
  EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC_STRUCTURE     AcpiLocalApic;
  EFI_ACPI_2_0_IO_APIC_STRUCTURE                  AcpiIoApic;
  struct {
    UINT8                                         Type;
    UINT8                                         Length;
  } AcpiApicCommon;
} ACPI_APIC_STRUCTURE_PTR;

extern EFI_GUID gEfiBdatMemoryData2Guid;

#endif
