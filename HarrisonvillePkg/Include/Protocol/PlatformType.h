//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/*++

Copyright (c) 1996 - 2014, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


Module Name:
  PlatformType.h

Abstract:
  This file defines platform policies for Platform Type.

--*/

#ifndef _PLATFORM_TYPE_H_
#define _PLATFORM_TYPE_H_

#include <Guid/PlatformInfo.h>

#define PLATFORM_NAME_MAX_SIZE    0x40

typedef struct _EFI_PLATFORM_TYPE_PROTOCOL {
  UINT8                       SystemUuid[16];     // 16 bytes
  UINT32                      Signature;          // "$PIT" 0x54495024
  UINT32                      Size;               // Size of the table
  UINT16                      Revision;           // Revision of the table
  UINT32                      Type;               // Platform Type
  UINT32                      TypeRevisionId;     // Board Revision ID
  UINT8                       CpuType;            // Cpu Type
  UINT8                       CpuStepping;        // Cpu Stepping
  UINT16                      IioSku;
  UINT16                      IioRevision;
  UINT16                      PchSku;
  UINT16                      PchRevision;
  BOOLEAN                     ExtendedInfoValid;  // If TRUE then below fields are Valid
  UINT8                       Checksum;           // Checksum minus SystemUuid is valid in DXE only.
  CHAR16                      PlatformName[PLATFORM_NAME_MAX_SIZE];
  UINT64                      IioStringPtr;
  UINT64                      PchStringPtr;
  EFI_PLATFORM_PCI_DATA       PciData;
  EFI_PLATFORM_CPU_DATA       CpuData;
  EFI_PLATFORM_MEM_DATA       MemData;
  EFI_PLATFORM_SYS_DATA       SysData;
  BOARD_ID_LIST               BoardId;
  
} EFI_PLATFORM_TYPE_PROTOCOL;

extern EFI_GUID gEfiPlatformTypeProtocolGuid;

#endif
