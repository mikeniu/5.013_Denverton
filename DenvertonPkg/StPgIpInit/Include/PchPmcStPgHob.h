/** @file
  Header file for PCH PMC ST PG HOB structure.

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

#ifndef _PCH_PMC_ST_PG_HOB_STRUCTURE_H_
#define _PCH_PMC_ST_PG_HOB_STRUCTURE_H_

#pragma pack(1)
/**
  Static Power Gating Related Function Disable Register 1
**/
typedef union {
  struct
  {
    UINT32 Gbe0FdisPmc  : 1,
           Gbe1FdisPmc  : 1,
           Sata0FdisPmc : 1,
           Sata1FdisPmc : 1,
           Pcie0FdisPmc : 1,
           Pcie1FdisPmc : 1,
           NcpmFdisPmc  : 1,
           EmmcFdisPmc  : 1,
           XhciFdisPmc  : 1,
           MeFdisPmc    : 1,
           IeFdisPmc    : 1,
           NpkFdisPmc   : 1,
           ExiFdisPmc   : 1,
           Reserved     : 18,
           StFdisLk     : 1;
  }        Ip;
  UINT32   Data;
} ST_PG_FDIS_PMC_1_REG;

/**
  Static Power Gating Library HOB
**/
typedef struct _EFI_PCH_PMC_ST_PG_HOB {
  BOOLEAN               ResetRequired;
  ST_PG_FDIS_PMC_1_REG  StPgFdisPmc1;
} EFI_PCH_PMC_ST_PG_HOB;
#pragma pack()
#endif //_PCH_PMC_ST_PG_HOB_STRUCTURE_H_
