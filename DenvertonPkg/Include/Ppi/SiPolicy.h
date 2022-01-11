/** @file
  Silicon Policy PPI is used for specifying platform 
  related Intel silicon information and policy setting. 
  This PPI is consumed by the silicon PEI modules and carried  
  over to silicon DXE modules.

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

#ifndef _SI_POLICY_PPI_H_
#define _SI_POLICY_PPI_H_

#include <SiPolicyStruct.h>

extern EFI_GUID gSiPolicyPpiGuid;

typedef struct _SI_POLICY_STRUCT SI_POLICY_PPI;

#endif // _SI_POLICY_PPI_H_

