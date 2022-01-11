/** @file
  Header file for IqatIp Policy PPI.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

#ifndef _IQAT_IP_POLICY_PPI_H_
#define _IQAT_IP_POLICY_PPI_H_

#include <IqatIpPolicy.h>

extern EFI_GUID gIqatIpPolicyPpiGuid;

typedef struct IQAT_IP_GLOBAL_POLICY IQAT_IP_GLOBAL_POLICY_PPI;			//< global policy IQAT IP PPI

#endif //_IQAT_IP_POLICY_PPI_H_
