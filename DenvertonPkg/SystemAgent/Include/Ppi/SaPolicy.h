/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

@copyright
 Copyright (c) 2010 - 2014 Intel Corporation. All rights reserved
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
#ifndef _SA_POLICY_PPI_H_
#define _SA_POLICY_PPI_H_

#include <Uefi.h>
#include <Library/SmbusLib.h>
#include <SaAccess.h>
#include <ConfigBlock.h>
#include <Ppi/MemoryConfig.h>
#include <Ppi/NvMemoryConfig.h>
#include <Ppi/SaPlatformConfig.h>
#include <Ppi/VtdConfig.h>

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gSiSaPolicyPpiGuid;

#define SA_POLICY_PPI_REVISION_1   1

//
// SI SA Policy PPI
//
typedef struct _SA_INIT_POLICY_PPI {
  CONFIG_BLOCK_HEADER            Header;
  CONFIG_BLOCK_OFFSET_TABLE      OffsetTable;
  UINT32                         Size;
/*
  Indiviual IP Config Block Structures are added here in memory as part of AddConfigBlock()
*/
}SI_SA_POLICY_PPI;
///
/// SI SA Policy PPI

#endif // _SA_POLICY_PPI_H_
