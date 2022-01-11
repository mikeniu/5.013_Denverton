/*++
Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PeiRasPolicyPpi.h

Abstract:

  Driver for RAS Policy structure.

Revision History

--*/

#ifndef _PEI_RAS_POLICY_PPI_H_
#define _PEI_RAS_POLICY_PPI_H_

#include <RasPolicy.h>

typedef struct RAS_GLOBAL_POLICY RAS_GLOBAL_POLICY_PPI;			//< global RAS policy

extern EFI_GUID gRasPolicyPpiGuid;

extern EFI_GUID gRasPolicyHobGuid;

#endif
