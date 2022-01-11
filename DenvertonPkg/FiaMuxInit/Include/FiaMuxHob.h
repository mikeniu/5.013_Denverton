/** @file
  Header file for FIA Mux HOB structure.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
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

#ifndef _FIA_MUX_HOB_STRUCTURE_H_
#define _FIA_MUX_HOB_STRUCTURE_H_

#include <FiaMuxPolicy.h>

#pragma pack(1)

//Global FIA MUX configure structure
typedef struct {
	FIA_MUX_CONFIG              FiaMuxConfig;
	FIA_MUX_CONFIG_STATUS       FiaMuxConfigStatus;
} FIA_MUX_CONFIG_HOB;

#pragma pack()

#endif //_FIA_MUX_HOB_STRUCTURE_H_
