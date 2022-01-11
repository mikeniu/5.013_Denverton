//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
    AcpiVariable.h
    
Abstract:

    GUIDs used for ACPI variables.

--*/

#ifndef _ACPI_VARIABLE_H_
#define _ACPI_VARIABLE_H_

#define EFI_ACPI_VARIABLE_GUID \
  { \
    0xaf9ffd67, 0xec10, 0x488a, 0x9d, 0xfc, 0x6c, 0xbf, 0x5e, 0xe2, 0x2c, 0x2e \
  }

#define ACPI_GLOBAL_VARIABLE  L"AcpiGlobalVariable"

//
// The following structure combine all ACPI related variables into one in order
// to boost performance
//
#pragma pack (1)
typedef struct {
  UINT16              Limit;
  UINTN               Base;
} PSEUDO_DESCRIPTOR;
#pragma pack()

typedef struct {
  BOOLEAN                         APState;
  BOOLEAN                         S3BootPath;
  EFI_PHYSICAL_ADDRESS            WakeUpBuffer;
  EFI_PHYSICAL_ADDRESS            GdtrProfile;
  EFI_PHYSICAL_ADDRESS            IdtrProfile;
  EFI_PHYSICAL_ADDRESS            CpuPrivateData;
  EFI_PHYSICAL_ADDRESS            StackAddress;
  EFI_PHYSICAL_ADDRESS            MicrocodePointerBuffer;
  
  EFI_PHYSICAL_ADDRESS            SmramBase;
  EFI_PHYSICAL_ADDRESS            SmmStartImageBase;
  UINT32                          SmmStartImageSize;
  UINT32                          NumberOfCpus;
  UINT32                          ApInitDone;
} ACPI_CPU_DATA;

typedef struct {
  //
  // Acpi Related variables
  //
  EFI_PHYSICAL_ADDRESS  AcpiReservedMemoryBase;
  UINT32                AcpiReservedMemorySize;
  EFI_PHYSICAL_ADDRESS  S3ReservedLowMemoryBase;
  EFI_PHYSICAL_ADDRESS  AcpiBootScriptTable;
  EFI_PHYSICAL_ADDRESS  RuntimeScriptTableBase;
  EFI_PHYSICAL_ADDRESS  AcpiFacsTable;
  UINT64                SystemMemoryLength;
  ACPI_CPU_DATA         AcpiCpuData;
} ACPI_VARIABLE_SET;

extern EFI_GUID gEfiAcpiVariableGuid;

#endif
