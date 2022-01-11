/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

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

  MemInfo.h
    
Abstract:

  This protocol provides the memory information data, such as
  total physical memory size, memory frequency, memory size 
  of each dimm and rank. 

  This protocol is EFI compatible.

--*/

#ifndef _MEM_INFO_PROTOCOL_H_
#define _MEM_INFO_PROTOCOL_H_

//
// Define the  protocol GUID
//
#define MEM_INFO_PROTOCOL_GUID \
  { \
    0x6f20f7c8, 0xe5ef, 0x4f21, 0x8d, 0x19, 0xed, 0xc5, 0xf0, 0xc4, 0x96, 0xae \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                   gMemInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _MEM_INFO_PROTOCOL MEM_INFO_PROTOCOL;

//
// Protocol definitions
//

#define CH_NUM    1
#define DIMM_NUM  2
#define RANK_NUM  2

#pragma pack(1)
typedef struct {
  UINT32  memSize;
  UINT8   ddrFreq;
  UINT8   ddrType;
  BOOLEAN EccSupport;
  UINT16  dimmSize[CH_NUM * DIMM_NUM];
  UINT8   reserved;
  UINT16   reserved2;
} MEMORY_INFO_DATA;
#pragma pack()

/*++
Data definition:

  memSize         Total physical memory size
  ddrFreq         DDR Frequency
  EccSupport      ECC Support
  dimmSize        Dimm Size
  DimmExist       Dimm Present or not
  RankInDimm      No. of ranks in a dimm
  
--*/

//
// Protocol definition
//
struct _MEM_INFO_PROTOCOL {
  MEMORY_INFO_DATA  MemInfoData;
};

#endif
