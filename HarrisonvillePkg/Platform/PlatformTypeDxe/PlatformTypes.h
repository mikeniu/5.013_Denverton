//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
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
  PlatformTypes.h
  
Abstract:
  Platform Type Driver for Harwich.

--*/

#ifndef _PLATFORM_TYPES_H_
#define _PLATFORM_TYPES_H_

#include <PiDxe.h>
#include <Protocol/PlatformType.h>
#include <Library\DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library\HobLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol\Variable.h>
#include <Protocol\CpuIo2.h>
//#include <Protocol\Spi.h>

#include <Guid/HobList.h>
#include <Include/CpuRegs.h>
#include <Include/Platform.h>
#include <Include/SetupVariable.h>

#define BLOCK_SIZE_32KB  				 0x8000
#define BLOCK_SIZE_64KB  				 0x10000
#define SPI_OPCODE_READ_INDEX            4
#define B_PCH_SPI_FREG4_BASE_MASK        0x00001FFF                  // Base, [12:0]  here represents base [24:12]
#define R_PCH_SPI_FREG4_PLATFORM_DATA (R_PCH_RCRB_SPI_BASE + 0x64)  // Flash Region 4(Platform Data)(32bits)
#define SB_RCBA             			 PCH_RCBA_ADDRESS 
#define VarMfgUuid           			 L"MfgSystemUuid"
#define EFI_PLATFORM_TYPE_DRIVER_PRIVATE_SIGNATURE  SIGNATURE_32 ('T', 'Y', 'P', 'P')
#define EFI_IIO_UDS_DRIVER_PRIVATE_SIGNATURE  SIGNATURE_32 ('S', 'D', 'U', 'I')

extern EFI_GUID gEfiMfgUUIDHobGuid;

VOID                              *mEfiVariableProtocolNotifyReg;
EFI_EVENT                         mEfiVariableProtocolEvent;

typedef struct {
  UINTN                               Signature;
  EFI_HANDLE                          Handle;               // Handle for protocol this driver installs on
  EFI_PLATFORM_TYPE_PROTOCOL          PlatformType;         // Policy protocol this driver installs
} EFI_PLATFORM_DATA_DRIVER_PRIVATE;

EFI_STATUS
GetPDRUUID (
  );

#endif
