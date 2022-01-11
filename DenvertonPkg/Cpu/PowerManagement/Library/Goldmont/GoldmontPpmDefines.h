/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  GoldmontPpmDefines.h
    
Abstract:

  This file contains define definitions specific to Denverton processor

--*/
#ifndef _GOLDMONT_PPM_DEFINES_H_
#define _GOLDMONT_PPM_DEFINES_H_

//
// Definititons for CPU stepping
//
#define EFI_CPUID_XAPIC_PROC_TOPOLOGY           0xB

//
// Definitions for CPUID
//
#define CPUID_STEPPING                          0x0000000F

//
// Extended Feature Flag definitions (CPUID EAX=1)
//
#define B_CPUID_VERSION_INFO_ECX_MWAIT          BIT3
#define B_CPUID_VERSION_INFO_ECX_EIST           BIT7
#define B_CPUID_VERSION_INFO_ECX_TM2            BIT8
#define B_CPUID_VERSION_INFO_EDX_TM1            BIT29

//
// CPUID 5 definitions (ECX)
//
#define B_CPUID_MONITOR_MWAIT_ECX_EXTENSIONS    BIT0

//
// CPUID 5 definitions (EDX)
//
#define CSTATE_SUPPORTED                        0x1
#define ENHANCED_CSTATE_SUPPORTED               0x2

//
// CPUID 6 definitions (EAX)
//
#define TURBO_MODE_SUPPORTED                    (1 << 1)

//
// MSR definitions
//
#define MSR_IA32_PLATFORM_ID                    0x17
  #define PLATFORM_ID_BITS_MASK                 (0x7 << 18) // Bits 52:50
  #define VID_MAX_PREFLX_BYTE_MASK              0x7F  // Bits 6:0
  #define RATIO_MAX_PFLX_BYTE_MASK              0x3F  // Bits 13:8
  #define RATIO_LOCKED                          (1 << 15)
  #define FB_GV3_DISABLED                       (1 << 17)
  #define FB_THERMAL_THROT_DISABLED             (1 << 19)
#define EFI_MSR_EBC_HARD_POWERON                0x2A
  #define STOP_ENABLE                           (1 << 19)
  
#define MSR_PLATFORM_INFO                       0xCE
#define MSR_IACORE_RATIOS                       0x66a
  #define MAX_RATIO_4C_OFFSET	                24
  #define MAX_RATIO_3C_OFFSET	                16
  #define MAX_RATIO_2C_OFFSET	                8
  #define MAX_RATIO_1C_OFFSET	                0
  #define MAX_RATIO_1C_MASK 		            0x000000ff
  #define MAX_RATIO_2C_MASK 		            0x0000ff00
  #define MAX_RATIO_3C_MASK 		            0x00ff0000
  #define MAX_RATIO_4C_MASK 		            0xff000000
#define MSR_IACORE_VIDS                         0x66b
  #define MAX_VID_4C_OFFSET	                    24
  #define MAX_VID_3C_OFFSET	                    16
  #define MAX_VID_2C_OFFSET	                    8
  #define MAX_VID_1C_OFFSET	                    0
  #define MAX_VID_1C_MASK                       0x000000ff
  #define MAX_VID_2C_MASK                       0x0000ff00
  #define MAX_VID_3C_MASK                       0x00ff0000
  #define MAX_VID_4C_MASK                       0xff000000
#define MSR_IACORE_TURBO_VIDS                   0x66d

#define MSR_PM_CFG_CTRL                         0xE2
  #define C0_SUB_STATES_MASK                    0x0000000f
  #define C1_SUB_STATES_MASK	                0x000000f0
  #define C2_SUB_STATES_MASK	                0x00000f00
  #define C3_SUB_STATES_MASK	                0x0000f000
  #define C4_SUB_STATES_MASK	                0x000f0000
  #define C5_SUB_STATES_MASK	                0x00f00000
  #define C6_SUB_STATES_MASK	                0x0f000000
  #define C7_SUB_STATES_MASK	                0xf0000000    
  #define CSTATE_LIMIT_MASK                     0xF
  #define CSTATE_LIMIT_NO_LIMIT                 0x0 							
  #define CSTATE_LIMIT_C1                       0x1
  #define CSTATE_LIMIT_C3                       0x2
  #define CSTATE_LIMIT_C6                       0x3
  #define CSTATE_LIMIT_C7                       0x4
  #define CSTATE_LIMIT_C7S                      0x5
  #define CSTATE_LIMIT_C8                       0x6
  #define CSTATE_LIMIT_C9                       0x7
  #define CSTATE_LIMIT_C10                      0x8
  #define B_TIMED_MWAIT_ENABLE                  BIT31
	
  #define B_IO_MWAIT_REDIRECTION_ENABLE         (1 << 10)
  #define B_SINGLE_PCTL                         (1 << 11)
  #define B_C3_AUTO_DEMOTION_ENABLE             (1 << 25)
  #define B_C1_AUTO_DEMOTION_ENABLE             (1 << 26)
  #define B_C3_AUTO_UNDEMOTION_ENABLE           (1 << 27)
  #define B_C1_AUTO_UNDEMOTION_ENABLE           (1 << 28)
  #define B_PKG_CSTATE_DEMOTION_ENABLE          (1 << 29)
  #define B_PKG_CSTATE_UNDEMOTION_ENABLE        (1 << 30)

#define MSR_PMG_IO_CAPTURE_BASE                 0xE4
  #define B_MSR_PMG_CST_RANGE                   (BIT18 | BIT17 | BIT16)
  #define V_IO_CAPT_LVL2                        (0x0 << 16)   ///< C3
  #define V_IO_CAPT_LVL3                        (0x1 << 16)   ///< C6
  #define V_IO_CAPT_LVL4                        (0x2 << 16)   ///< C7
  #define V_IO_CAPT_LVL5                        (0x3 << 16)   ///< C8
  #define V_IO_CAPT_LVL6                        (0x4 << 16)   ///< C9
  #define V_IO_CAPT_LVL7                        (0x5 << 16)   ///< C10
  #define V_IO_CAPT_LVL2_BASE_ADDR_MASK         0xFFFF

  #define B_IO_MWAIT_REDIRECTION_ENABLE         (1 << 10)
  #define B_SINGLE_PCTL                         (1 << 11)

#define MSR_POWER_MISC                          0x120
  #define ENABLE_ULFM_AUTOCM_MASK               (1 << 2)
  #define ENABLE_INDP_AUTOCM_MASK               (1 << 3)
  #define ENABLE_IA_UNTRUSTED_MODE              (1 << 6)
#define MAX_POWER_LIMIT_1_TIME_IN_SECONDS       32767
#define AUTO                                    0
#define END_OF_TABLE                            0xFF

#define MSR_FLEX_RATIO                          0x194
  #define VID_FLEX_BYTE_MASK                    0xFF      // Bits 6:0  (7 --> Reserved)
  #define RATIO_FLEX_BYTE_MASK                  0xFF      // Bits 13:8 (15:14 --> Reserved)
  #define ENABLE_FLEX                           (1 << 16)

#define MSR_IA32_PERF_STS                       0x198
  #define BUS_RATIO_MIN_MASK			              0x1F			//Bits 28:24
#define MSR_IA32_PERF_CTL                       0x199
  #define P_STATE_TARGET_OFFSET                 8
  #define P_STATE_TARGET_MASK                   (0x00FF)
  #define RATIO_BYTE_MASK                       0x1F
  #define P_STATE_MAX_FREQ                      0xFF
  #define B_IA32_PERF_CTRLP_STATE_TARGET        (0x7F << 8)
#define MSR_IA32_CLOCK_MODULATION               0x19A
#define MSR_IA32_MISC_ENABLES                   0x1A0
#define MSR_TURBO_RATIO_LIMIT                   0x1AD
#define MSR_PACKAGE_THERM_INTERRUPT             0x1B2
  #define PROCHOT_INT_ENABLE                    BIT2
#define MSR_POWER_CTL                           0x1FC
  #define FAST_BRK_INTERRUPT_ENABLE               (1 << 4)
  #define FAST_BRK_SNP_ENABLE                     (1 << 3)
  #define B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE (1 << 30)
#define MSR_PACKAGE_POWER_SKU_UNIT              0x606
#define MSR_PACKAGE_POWER_SKU                   0x614

#define MSR_CPU_THERM_SENS_CFG                  0x675

#define MSR_PRIMARY_PLANE_CURRENT_CONFIG        0x601
  #define CURRENT_LIMIT_MASK                    0x1FFF      // Bits 12:0

#define MSR_TURBO_POWER_LIMIT                   0x610

#define MSR_PRIMARY_PLANE_TURBO_POWER_LIMIT     0x638
  #define PP_PWR_LIM_TIME                       (0 << PP_PWR_LIM_TIME_OFFSET)
  #define PP_PWR_LIM_TIME_MASK                  (0x7F << PP_PWR_LIM_TIME_OFFSET)
  #define PP_PWR_LIM_TIME_OFFSET                17

#define PACKAGE_TDP_POWER_MASK                  (0x7FFF)    // Bits 14:0

#define POWER_LIMIT_ENABLE                      (1 << 15)   // Used as Bit 15 and Bit 47
#define POWER_LIMIT_MASK                        (0x7FFF)    // Bits 14:0 and 46:32
#define POWER_LIMIT_1_TIME_MASK                 (0xFE0000)  // Bits 23:17

#define TM_ENABLE                               (1 << 3)
#define GV3_ENABLE                              (1 << GV3_ENABLE_OFFSET)
#define GV3_ENABLE_OFFSET                       16
#define MONITOR_MWAIT_ENABLE                    (1 << 18)
#define LIMIT_CPUID                             (1 << 22)
#define C1E_ENABLE                              (1 << 1)

#define DISABLE_MASTER_TURBO_MODE               (1 << (38 - 32))

#define PHOT_ENABLE                             (1 << 0)
#define PACKAGE_POWER_UNIT_MASK                 0xF
#define PACKAGE_TIME_UNIT_MASK                  0xF0000

#define PPM_DISABLE                             0

#define EFI_MSR_IA32_PERF_CTL                   MSR_IA32_PERF_CTL

#define FVID_MAX_POWER                          2000
#define FVID_MIN_POWER                          600

#define NOPKG_CSTATES                           0x0
#define NOS0IX                                  0x1
#define PKG_CSTATES_NOLIMIT                     0x2

  #define MEM_THERM_CTRL_MCHBAR                   0x7200
  #define MEM_THERM_CTRL_EN_MEMTRIP               BIT0

  #define MEMHOT_THERM_CONFIG_MCHBAR              0x70DC
  #define MEMHOT_THERM_CONFIG_MEMHOT_THROT_LVL    (BIT1 | BIT0)
  #define MEMHOT_DISABLE                          0
  #define MEMHOT_THRT_MID                         0x1
  #define MEMHOT_THRT_HI                          0x2
  #define MEMHOT_THRT_CRIT                        0x3

  #define PL3_CONTROL_MCHBAR                      0x71F0
  #define B_PL3_CONTROL_LOCK                      BIT63
  #define B_PL3_CONTROL_PL4_ENABLE                BIT47
  #define B_PL3_CONTROL_PMAX                      (BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40 | \
                                                    BIT39 | BIT38 | BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
  #define B_PL3_CONTROL_DUTY_CYCLE                (BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
  #define B_PL3_CONTROL_TIME_WINDOW               (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17)
  #define B_PL3_CONTROL_PL3_ENABLE                BIT15
  #define B_PL3_CONTROL_POWER_LIMIT               (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | \
                                                    BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
  #define N_PL3_CONTROL_TIME_WINDOW               17
  #define N_PL3_CONTROL_DUTY_CYCLE                24
  #define N_PL3_CONTROL_PMAX                      32

  #define MEM_THERM_CTRL_MCHBAR                   0x7200
  #define MEM_THERM_CTRL_EN_MEMTRIP               BIT0

  #define MEMHOT_THERM_CONFIG_MCHBAR              0x70DC
  #define MEMHOT_THERM_CONFIG_MEMHOT_THROT_LVL    (BIT1 | BIT0)
  #define MEMHOT_DISABLE                          0
  #define MEMHOT_THRT_MID                         0x1
  #define MEMHOT_THRT_HI                          0x2
  #define MEMHOT_THRT_CRIT                        0x3

#define BIOS_RESET_CPL_MCHBAR                   0x7078
  #define BIOS_RESET_CPL_RST_CPL0               BIT0

#define DTS_CONFIG3_CFG_MCHBAR                  0x7118
  #define TCONTROL_OFFSET                       ( BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 )
  #define TCONTROL_OFFSET_SIGN                  BIT7
  #define OFFSET_PROGRAMMED                     BIT28
#endif
