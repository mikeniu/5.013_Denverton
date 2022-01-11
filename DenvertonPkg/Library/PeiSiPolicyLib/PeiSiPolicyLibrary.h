/** @file
  Header file for the PeiSiPolicyLib library.

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
#ifndef _PEI_SI_POLICY_LIBRARY_H_
#define _PEI_SI_POLICY_LIBRARY_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/SiPolicy.h>
#include <Library/SiPolicyLib.h>
#include <PchAccess.h>

#define TEMP_MEM_BASE_ADDRESS 0xFE600000
#define TEMP_IO_BASE_ADDRESS  0xD000

//
// IO/MMIO resource limits
//
#define TEMP_MEM_SIZE         V_PCH_XDCI_MEM_LENGTH
#define TEMP_IO_SIZE          0x10

#endif // _PEI_SI_POLICY_LIBRARY_H_
