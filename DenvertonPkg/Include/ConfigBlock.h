/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _CONFIG_BLOCK_H_
#define _CONFIG_BLOCK_H_
//
// Defining Config Block ID structure
//
typedef enum {
  EnumPlatformConfigId,
  EnumMemConfigId,
  EnumNvMemConfigId,
  EnumVtdConfigId,
  EnumConfigBlockIdMax
} CONFIG_BLOCK_ID;
//
// Define the Number of Config Blocks. 
// Change the value once new config blocks are added.
//
#define MAX_CONFIG_BLOCKS   EnumConfigBlockIdMax

//
// Config Block Header
//
typedef struct _CONFIG_BLOCK_HEADER{
  EFI_GUID   Guid;
  UINT8      Revision;
  UINT8      BlockCount;
  UINT32     Size;
} CONFIG_BLOCK_HEADER;

//
// Config Block Offset Table
//
typedef struct _CONFIG_BLOCK_OFFSET_TABLE{
  UINT32                BlockOffset[MAX_CONFIG_BLOCKS]; ///< This field is used to describe the Block Offset 
} CONFIG_BLOCK_OFFSET_TABLE;

//
//  IP Block Template
//
typedef struct _IP_BLOCK_STRUCT{
  UINT16   BlockId;
  UINT8    BlockRevision;
  UINT16   BlockSize;
} IP_BLOCK_STRUCT;

//
// CONFIG_BLOCK_STRUCT
//
typedef struct _CONFIG_BLOCK_STRUCT {
  CONFIG_BLOCK_HEADER            Header;
  CONFIG_BLOCK_OFFSET_TABLE      OffsetTable;


//Indiviual IP Config Block Structures are added here in memory as part of AddConfigBlock()

}CONFIG_BLOCK_STRUCT;

#endif // _CONFIG_BLOCK_H_