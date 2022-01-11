/******************************************************************************
ADR configuration of CPLD Sample Code

Copyright 2014 Intel Corporation All Rights Reserved.

The source code, information and material ("Material") contained herein
is owned by Intel Corporation or its suppliers or licensors, and title
to such Material remains with Intel Corporation or its suppliers or licensors.
The Material contains proprietary information of Intel or its suppliers and 
licensors. The Material is protected by worldwide copyright laws and treaty 
provisions. No part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed or disclosed in any way
without Intel's prior express written permission. No license under any patent,
copyright or other intellectual property rights in the Material is granted to
or conferred upon you, either expressly, by implication, inducement, estoppel
or otherwise. Any license under such intellectual property rights must be 
express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or alter this
notice or any other notice embedded in Materials by Intel or Intel's suppliers
or licensors in any way.

******************************************************************************/
#ifndef __ADR_CFG_LIB_H__
#define __ADR_CFG_LIB_H__
#include <base.h>

typedef enum {
	ADR_BBU = 0,
	C2F = 1,
	SMI_ONLY = 2,
	NVDIMM = 3,
	NVM_DISABLE
} CPLD_OP_MODES;

typedef union {
  struct {
	// [4:0] R/W C2F Config bits 0..3
	CPLD_OP_MODES c2f_config : 4;
	//   [4] W   MCU_INIT
	UINT8 mcu_init   : 1;
	//   [5] R   DC/AC good
	UINT8 dc_ac_good : 1;
	//   [6] W   ADR_STATUS_CLR
	UINT8 status_clr : 1;
	//   [7] R   ADR_STATUS_RD
	UINT8 adr_status : 1;
  } fld;
  UINT8 data;
} ADR_CONFIG_STATUS;

UINT8 AdrCfg_boardID(void);
UINT8 AdrCfg_CPLDversion(void);

// VREG 0=off / 1=On
// [0] -> VREG_EN_A(Channel 0) ... [7] -> VREG_EN_H(Channel 7)
UINT8 AdrCfg_VREGread(void);
void AdrCfg_VREGwrite(UINT8);

ADR_CONFIG_STATUS AdrCfg_ConfigRead(void);
void AdrCfg_ConfigWrite(ADR_CONFIG_STATUS config);
void AdrCfg_Arm(CPLD_OP_MODES mode);
UINT8 AdrCfg_Active(void);
void AdrCfg_Deactivate(void);

//
// PMC registers used when using internal ADR status
//
#define PCH_PM_STS  0x1C
#define ADR_RST_STS   16

#endif //__ADR_CFG_LIB_H__
