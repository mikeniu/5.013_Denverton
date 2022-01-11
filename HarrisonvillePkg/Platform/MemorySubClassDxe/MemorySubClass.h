//
// This file contains an 'Intel Peripheral Driver' and is        
// licensed for Intel CPUs and chipsets under the terms of your  
// license agreement with Intel or your vendor.  This file may   
// be modified by the user, subject to additional terms of the   
// license agreement                                             
//
/** @file
  Header file for MemorySubClass Driver.

  Copyright (c) 1999 - 2015 Intel Corporation.  All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MEMORY_SUBCLASS_DRIVER_H
#define _MEMORY_SUBCLASS_DRIVER_H

#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmbusHc.h>
#include <IndustryStandard/Smbus.h>

#include "Platform.h"
#include <Guid/MemoryConfigData.h>

//DataWidth
#define DATAWIDTH_8_BITS      0
#define DATAWIDTH_16_BITS     1
#define DATAWIDTH_32_BITS    10
#define DATAWIDTH_64_BITS    11 
#define DATAWIDTH_72_BITS  1011

//
// maximum capable memory speed
// Values taken from Jedec Spec
//
#define MAX_DDRFREQ_800      0x14  
#define MAX_DDRFREQ_1066     0x0F
#define MAX_DDRFREQ_1333     0x0C
#define MAX_DDRFREQ_1600     0x0A
#define MAX_DDRFREQ_1866     0x09
#define MAX_DDRFREQ_2133     0x08
#define MAX_DDRFREQ_2400     0x07
#define MAX_DDRFREQ_2666     0x06
#define MAX_DDRFREQ_3200     0x05

//
// SMBUS
//
#define START_OFFSET              128  //Module Part Number Start
#define END_OFFSET                145  //Module Part Number End
#define DDR4_MODULEPN_START       329  //DDR4 Module Part Number Start
#define DDR4_MODULEPN_END         348  //DDR4 Module Part Number End
#define DIMM0_SPD_ADDRESS_CHANNEL_0     0xA0 //Channel 0
#define DIMM0_SPD_ADDRESS_CHANNEL_1     0xA4 //Channel 1

//
// DDR3 Values for VDD
// Nominal Voltage
//
#define DDR3_VDD_DDRS             1500
#define DDR3_VDD_DDRL             1350
#define DDR3_VDD_DDRU             1250

//
// Row configuration data structure
//
typedef struct {
  EFI_PHYSICAL_ADDRESS          BaseAddress;
  UINT64                        RowLength;  // Size of Row in bytes
} DDR_ROW_CONFIG;

typedef struct {
  UINT8 Index;
  UINT8 ManufactureId;
  CHAR16* ManufactureName;
} MEMORY_MODULE_MANUFACTURE_LIST;
//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//

#define EFI_MEMORY_SUBCLASS_DRIVER_GUID \
  { \
    0xef17cee7, 0x267d, 0x4bfd, 0xa2, 0x57, 0x4a, 0x6a, 0xb3, 0xee, 0x85, 0x91 \
  }

extern UINT8 MemorySubClassDxeStrings[];

#endif

