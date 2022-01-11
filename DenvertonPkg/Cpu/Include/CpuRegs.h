/*++

Copyright (c) 2004 - 2016, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  CpuRegs.h

Abstract:

--*/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

//
// Local APIC defines
//
#define APIC_REGISTER_LOCAL_ID_OFFSET          0x00000020
#define APIC_REGISTER_APIC_VERSION_OFFSET      0x00000030
#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET   0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET           0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET          0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET      0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET      0x00000360

#define BROADCAST_MODE_SPECIFY_CPU             0x00
#define BROADCAST_MODE_ALL_INCLUDING_SELF      0x01
#define BROADCAST_MODE_ALL_EXCLUDING_SELF      0x02

#define EFI_CACHE_UNCACHEABLE                  0
#define EFI_CACHE_WRITECOMBINING               1
#define EFI_CACHE_WRITETHROUGH                 4
#define EFI_CACHE_WRITEPROTECTED               5
#define EFI_CACHE_WRITEBACK                    6

//
// CPU Family, Model, Stepping values
//
#define EFI_CPUID_FAMILY_NEHALEM_UARCH         6
#define EFI_CPUID_MODEL_BLOOMFIELD             0x1A
#define EFI_CPUID_MODEL_GAINESTOWN             0x1A
#define EFI_CPUID_MODEL_PINEVIEW               0x1C
#define EFI_CPUID_MODEL_ATOM                   0x1C
#define EFI_CPUID_MODEL_CLARKSFIELD            0x1E
#define EFI_CPUID_MODEL_LYNNFIELD              0x1E
#define EFI_CPUID_MODEL_AUBURNDALE             0x1F
#define EFI_CPUID_MODEL_HAVENDALE              0x1F
#define EFI_CPUID_MODEL_CLARKDALE              0x25 // Havendale 32nm
#define EFI_CPUID_MODEL_NEHALEM_EX             0x2E
#define EFI_CPUID_MODEL_CEDARVIEW              0x36
#define EFI_CPUID_MODEL_VALLEYVIEW             0x37
#define EFI_CPUID_MODEL_AVOTON                 0x4D // Avoton and Rangeley have the same Processor Signature.

#define EFI_CPUID_STEPPING_PINEVIEW_MIN        0x07
#define EFI_CPUID_STEPPING_PINEVIEW_MAX        0x0F
#define EFI_CPUID_STEPPING_CEDARVIEW_MIN       0x00
#define EFI_CPUID_STEPPING_CEDARVIEW_MAX       0x0F
#define EFI_CPUID_STEPPING_VALLEYVIEW_MIN      0x00
#define EFI_CPUID_STEPPING_VALLEYVIEW_MAX      0x0F

//
// CPUID defines
//
#define EFI_CPUID_SIGNATURE                    0x0

#define EFI_CPUID_VERSION_INFO                 0x1
#define   B_EFI_CPUID_VERSION_INFO_EAX_MASK                            0x0FFF0FFF
#define   B_EFI_CPUID_VERSION_INFO_EAX_FULL_FAMILY_MODEL_MASK          0x0FFF0FF0
#define   B_EFI_CPUID_VERSION_INFO_EAX_EXT_FAMILY_ID_MASK              0x0FF00000
#define   B_EFI_CPUID_VERSION_INFO_EAX_EXT_MODEL_ID_MASK               0x000F0000
#define   N_EFI_CPUID_VERSION_INFO_EAX_EXT_FAMILY_ID                   20
#define   N_EFI_CPUID_VERSION_INFO_EAX_EXT_MODEL_ID                    16
#define   N_EFI_CPUID_VERSION_INFO_EAX_TYPE                            12
#define   N_EFI_CPUID_VERSION_INFO_EAX_FAMILY_ID                       8
#define   N_EFI_CPUID_VERSION_INFO_EAX_MODEL                           4
#define   N_EFI_CPUID_VERSION_INFO_EAX_STEPPING_ID                     0
#define   B_EFI_CPUID_VERSION_INFO_EBX_DEFAULT_APIC_ID                 (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_CPUID_VERSION_INFO_EBX_LOGICAL_CPU_PER_PACKAGE         (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_CPUID_VERSION_INFO_EBX_CLFLUSH_CHUNK_COUNT             (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_CPUID_VERSION_INFO_ECX_AES                             BIT25
#define   B_EFI_CPUID_VERSION_INFO_ECX_XAPIC                           BIT21
#define   B_EFI_CPUID_VERSION_INFO_ECX_SSE4_2                          BIT20
#define   B_EFI_CPUID_VERSION_INFO_ECX_SSE4_1                          BIT19
#define   B_EFI_CPUID_VERSION_INFO_ECX_DCA                             BIT18
#define   B_EFI_CPUID_VERSION_INFO_ECX_XTPR_UPDATE                     BIT14
#define   B_EFI_CPUID_VERSION_INFO_ECX_CMPXCHG16B                      BIT13
#define   B_EFI_CPUID_VERSION_INFO_ECX_L1_CONTEXT_ID                   BIT10
#define   B_EFI_CPUID_VERSION_INFO_ECX_SUP_SSE3                        BIT9
#define   B_EFI_CPUID_VERSION_INFO_ECX_TM2                             BIT8
#define   B_EFI_CPUID_VERSION_INFO_ECX_EIST                            BIT7
#define   B_EFI_CPUID_VERSION_INFO_ECX_SME                             BIT6
#define   B_EFI_CPUID_VERSION_INFO_ECX_VME                             BIT5
#define   B_EFI_CPUID_VERSION_INFO_ECX_QPL                             BIT4
#define   B_EFI_CPUID_VERSION_INFO_ECX_MWAIT                           BIT3
#define   B_EFI_CPUID_VERSION_INFO_ECX_SSE3                            BIT0
#define   B_EFI_CPUID_VERSION_INFO_EDX_PBE                             BIT31
#define   B_EFI_CPUID_VERSION_INFO_EDX_THERMAL_CLOCK_CONTROL           BIT29
#define   B_EFI_CPUID_VERSION_INFO_EDX_HT                              BIT28
#define   B_EFI_CPUID_VERSION_INFO_EDX_SELF_SNOOP                      BIT27
#define   B_EFI_CPUID_VERSION_INFO_EDX_SSE2                            BIT26
#define   B_EFI_CPUID_VERSION_INFO_EDX_SSE                             BIT25
#define   B_EFI_CPUID_VERSION_INFO_EDX_FAST_SAVE_RESTORE               BIT24
#define   B_EFI_CPUID_VERSION_INFO_EDX_MMX                             BIT23
#define   B_EFI_CPUID_VERSION_INFO_EDX_ACPI_SUPPORT                    BIT22
#define   B_EFI_CPUID_VERSION_INFO_EDX_DEBUG_TRACE_STORE               BIT21
#define   B_EFI_CPUID_VERSION_INFO_EDX_CLFLUSH_INTR                    BIT19
#define   B_EFI_CPUID_VERSION_INFO_EDX_CPU_SERIAL_NUMBER               BIT18
#define   B_EFI_CPUID_VERSION_INFO_EDX_PSE                             BIT17
#define   B_EFI_CPUID_VERSION_INFO_EDX_PAT                             BIT16
#define   B_EFI_CPUID_VERSION_INFO_EDX_CON_MOVE_INTR                   BIT15
#define   B_EFI_CPUID_VERSION_INFO_EDX_MCA                             BIT14
#define   B_EFI_CPUID_VERSION_INFO_EDX_PGE                             BIT13
#define   B_EFI_CPUID_VERSION_INFO_EDX_MTRR                            BIT12
#define   B_EFI_CPUID_VERSION_INFO_EDX_SEP                             BIT11
#define   B_EFI_CPUID_VERSION_INFO_EDX_ON_CHIP_APIC                    BIT9
#define   B_EFI_CPUID_VERSION_INFO_EDX_CMPXCHG8                        BIT8
#define   B_EFI_CPUID_VERSION_INFO_EDX_MCE                             BIT7
#define   B_EFI_CPUID_VERSION_INFO_EDX_PAE                             BIT6
#define   B_EFI_CPUID_VERSION_INFO_EDX_MSR                             BIT5
#define   B_EFI_CPUID_VERSION_INFO_EDX_TIME_STAMP_COUNTER              BIT4
#define   B_EFI_CPUID_VERSION_INFO_EDX_PAGE_SIZE_EXT                   BIT3
#define   B_EFI_CPUID_VERSION_INFO_EDX_DEBUG_EXT                       BIT2
#define   B_EFI_CPUID_VERSION_INFO_EDX_VME_8086                        BIT1
#define   B_EFI_CPUID_VERSION_INFO_EDX_FP_386                          BIT0

#define EFI_CPUID_CACHE_INFO                   0x2
#define EFI_CPUID_SERIAL_NUMBER                0x3

#define EFI_CPUID_CACHE_PARAMS                 0x4
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_MAX_CORES_IN_PACKAGE            (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26)
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_TOTAL_THREADS_SHARE_CACHE       (BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14)
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_FULLY_ASSOCIATIVE_CACHE         BIT9
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_SELF_INITIALIZING               BIT8
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_CACHE_LEVEL                     (BIT7 | BIT6 | BIT5)
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_CACHE_TYPE                      (BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define   B_EFI_CPUID_CACHE_PARAMS_EBX_WAYS_OF_ASSOCIATIVITY           (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23 | BIT22)
#define   B_EFI_CPUID_CACHE_PARAMS_EBX_PHYSICAL_LINE_PARTITIONS        (BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12)
#define   B_EFI_CPUID_CACHE_PARAMS_EBX_SYSTEM_COHERENCY_LINE_SIZE      (BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define   B_EFI_CPUID_CACHE_PARAMS_EDX_PREFETCH_STRIDE                 (BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define   B_EFI_CPUID_CACHE_PARAMS_EDX_CACHE_INCLUSIVE_IN_LOWER_CACHE  BIT1
#define   B_EFI_CPUID_CACHE_PARAMS_EDX_WBINVD_INVD_ON_LOWER_CACHE      BIT0

#define EFI_CPUID_MONITOR_MWAIT_PARAMS         0x5
#define   B_EFI_CPUID_MONITOR_MWAIT_ECX_INTERRUPTS_BREAK_MWAIT         BIT1
#define   B_EFI_CPUID_MONITOR_MWAIT_ECX_MWAIT_SUPPORT                  BIT0
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C7                      (BIT31 | BIT30 | BIT29 | BIT28)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C7                      28
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C6                      (BIT27 | BIT26 | BIT25 | BIT24)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C6                      24
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C5                      (BIT23 | BIT22 | BIT21 | BIT20)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C5                      20
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C4                      (BIT19 | BIT18 | BIT17 | BIT16)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C4                      16
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C3                      (BIT15 | BIT14 | BIT13 | BIT12)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C3                      12
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C2                      (BIT11 | BIT10 | BIT9 | BIT8)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C2                      8
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C1                      (BIT7 | BIT6 | BIT5 | BIT4)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C1                      4
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C0                      (BIT3 | BIT2 | BIT1 | BIT0)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C0                      0

#define EFI_CPUID_POWER_MANAGEMENT_PARAMS      0x6
#define   EFI_CPUID_POWER_MANAGEMENT_EAX_PECI                          BIT0
#define   EFI_CPUID_POWER_MANAGEMENT_EBX_NUM_INT_THRESHOLDS            (BIT3 | BIT2 | BIT1 | BIT0)
#define   EFI_CPUID_POWER_MANAGEMENT_ECX_HW_COORDINATION_FEEDBACK      BIT0

#define EFI_CPUID_REV7                         0x7
#define EFI_CPUID_REV8                         0x8
#define EFI_CPUID_DCA_PARAMS                   0x9
#define EFI_CPUID_ARCH_PERF_MON                0xA
#define EFI_CPUID_CORE_TOPOLOGY                0xB

#define EFI_CPUID_EXTENDED_FUNCTION            0x80000000

#define EFI_CPUID_EXTENDED_FEATURE_BITS        0x80000001
#define EFI_CPUID_EXTENDED_FEATURE_BITS_ECX_LAHF_SAHF                  BIT0
#define EFI_CPUID_EXTENDED_FEATURE_BITS_EDX_XD                         BIT20
#define EFI_CPUID_EXTENDED_FEATURE_BITS_EDX_SYSCALL                    BIT11

//
// This constant defines the maximum length of the CPU brand string. According to the
// IA manual, the brand string is in EAX through EDX (thus 16 bytes) after executing
// the CPUID instructions with EAX as 80000002, 80000003, 80000004.
//
#define MAXIMUM_CPU_BRAND_STRING_LENGTH        48

#define EFI_CPUID_BRAND_STRING1                0x80000002
#define EFI_CPUID_BRAND_STRING2                0x80000003
#define EFI_CPUID_BRAND_STRING3                0x80000004

#define EFI_CPUID_ADVANCED_POWER_MANAGEMENT    0x80000007
#define   EFI_CPUID_ADVANCED_POWER_MANAGEMENT_EDX_TSC_INVARIANCE         BIT8

#define EFI_CPUID_VIRT_PHYS_ADDRESS_SIZE       0x80000008
#define   B_EFI_CPUID_VIRTUAL_ADDRESS_BITS                             (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_CPUID_PHYSICAL_ADDRESS_BITS                            (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

//
// Common MSR
//
//#define EFI_MSR_IA32_PLATFORM_ID               0x00000017
#define   N_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS                  50
#define   B_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS_MASK             (BIT52 | BIT51 | BIT50)
#define   N_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS_MASK_START       50
#define   N_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS_MASK_END         52

#ifndef EFI_MSR_IA32_APIC_BASE
#define EFI_MSR_IA32_APIC_BASE                 0x0000001B
#endif

#define   B_EFI_MSR_IA32_APIC_BASE_APIC_BASE_ADDRESS                   0xFFFFFF000  //For Nehalem, base address can be up to 43 bits but not cover here yet
#define   B_EFI_MSR_IA32_APIC_BASE_APIC_GLOBAL_ENABLE                  BIT11
#define   B_EFI_MSR_IA32_APIC_BASE_M_XAPIC                             BIT10
#define   B_EFI_MSR_IA32_APIC_BASE_BSP                                 BIT8

//
// Local APIC defines, offset from APIC base address
//
#define APIC_REGISTER_LOCAL_ID_OFFSET          0x00000020
#define   N_APIC_REGISTER_LOCAL_ID_OFFSET_XAPIC_ID_MASK                24
#define   B_APIC_REGISTER_LOCAL_ID_OFFSET_XAPIC_ID_MASK                0xFF000000

#define APIC_REGISTER_APIC_VERSION_OFFSET      0x00000030
#define   B_APIC_REGISTER_APIC_VERSION_OFFSET_VERSION_MASK             0xFF

#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET   0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET           0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET          0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET      0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET      0x00000360

//#define EFI_MSR_EBC_FREQUENCY_ID               0x0000002C

#define EFI_MSR_IA32_FEATURE_CONTROL           0x0000003A
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SGE                           BIT15
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SLFE                          (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SMRR                          BIT3
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_EVT                           BIT2
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_ELT                           BIT1
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK                          BIT0
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_VT_SECURE                     0x0000FF02

#ifndef EFI_MSR_IA32_BIOS_UPDT_TRIG
#define EFI_MSR_IA32_BIOS_UPDT_TRIG            0x00000079
#endif
#ifndef EFI_MSR_IA32_BIOS_SIGN_ID
#define EFI_MSR_IA32_BIOS_SIGN_ID              0x0000008B
#endif

#define EFI_MSR_IA32_MCG_CAP                   0x00000179
#define EFI_MSR_IA32_MCG_STATUS                0x0000017A

#define EFI_MSR_PMG_CST_CONFIG                 0x000000E2
#define   B_EFI_MSR_PMG_CST_CONFIG_CST_CONTROL_LOCK                    BIT15
#define   B_EFI_MSR_PMG_CST_CONFIG_IO_MWAIT_REDIRECTION_ENABLE         BIT10
#define   B_EFI_MSR_PMG_CST_CONFIG_PACKAGE_C_STATE_LIMIT               (BIT2 | BIT1 | BIT0)

#define EFI_MSR_PMG_IO_CAPTURE_ADDR            0x000000E4   //For Nehalem Spec: EFI_IA32_PMG_IO_CAPTURE_BASE
#define   N_EFI_MSR_PMG_IO_CAPTURE_ADDR_CST_RANGE                      16
#define   B_EFI_MSR_PMG_IO_CAPTURE_ADDR_LVL_2_BASE_ADDRESS_MASK        0xFFFF

#define EFI_MSR_IA32_MPERF                     0x000000E7
#define EFI_MSR_IA32_APERF                     0x000000E8

#define   N_EFI_PLATFORM_INFO_MIN_RATIO         40
#define   B_EFI_PLATFORM_INFO_RATIO_MASK        0xFF
#define   N_EFI_PLATFORM_INFO_MAX_RATIO         8

#define EFI_MSR_IA32_MTRR_CAP                  0x000000FE
#define   B_EFI_MSR_IA32_MTRR_CAP_EMRR_SUPPORT                         BIT12
#define   B_EFI_MSR_IA32_MTRR_CAP_SMRR_SUPPORT                         BIT11
#define   B_EFI_MSR_IA32_MTRR_CAP_WC_SUPPORT                           BIT10
#define   B_EFI_MSR_IA32_MTRR_CAP_FIXED_SUPPORT                        BIT8
#define   B_EFI_MSR_IA32_MTRR_CAP_VARIABLE_SUPPORT                     (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define EFI_MSR_IA32_FEATURE_CONFIG            0x0000013C
#define   B_IA32_FEATURE_CONFIG_AES_DIS                                 BIT1
#define   B_IA32_FEATURE_CONFIG_LOCK                                    BIT0

#define EFI_MSR_CLOCK_FLEX_MAX                 0x00000194
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_EN                             BIT16
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_RATIO_MASK                     0x1F
#define   N_EFI_MSR_CLOCK_FLEX_MAX_FLEX_RATIO                          8

#define EFI_MSR_IA32_PERF_STS                  0x00000198
#define EFI_MSR_IA32_PERF_CTRL                 0x00000199
//#define EFI_MSR_IA32_CLOCK_MODULATION          0x0000019A

#ifndef EFI_MSR_IA32_THERM_INTERRUPT
#define EFI_MSR_IA32_THERM_INTERRUPT           0x0000019B
#endif

#define   B_EFI_MSR_IA32_THERM_INTERRUPT_VIE                           BIT4

#ifndef EFI_MSR_IA32_THERM_STATUS
#define EFI_MSR_IA32_THERM_STATUS              0x0000019C
#endif

#ifndef EFI_MSR_IA32_MISC_ENABLE
#define EFI_MSR_IA32_MISC_ENABLE               0x000001A0
#endif

#ifndef EFI_MSR_POWER_MISC
#define EFI_MSR_POWER_MISC                     0x00000120
  #define B_EFI_MSR_POWER_MISC_ENABLE_ULFM_AUTOCM_MASK                 BIT2
  #define B_EFI_MSR_POWER_MISC_ENABLE_INDP_AUTOCM_MASK                 BIT3
  #define B_EFI_MSR_POWER_MISC_ENABLE_IA_UNTRUSTED_MODE                BIT6
#endif

#define   B_EFI_MSR_IA32_MISC_ENABLE_XD                                BIT34
#define   B_EFI_MSR_IA32_MISC_ENABLE_CPUID_MAX                         BIT22
#define   B_EFI_MSR_IA32_MISC_ENABLE_MONITOR                           BIT18
#define   B_EFI_MSR_IA32_MISC_ENABLE_EIST                              BIT16
#define   B_EFI_MSR_IA32_MISC_ENABLE_TM1_EN                            BIT3

#define EFI_MSR_SMRR_PHYS_BASE                 0x000001F2 //0x000000A0
#define EFI_MSR_SMRR_PHYS_MASK                 0x000001F3 //0x000000A1

#define EFI_MSR_CACHE_VARIABLE_MTRR_BASE       0x00000200
#define EFI_MSR_CACHE_VARIABLE_MTRR_END        0x0000020F
#define   V_EFI_FIXED_MTRR_NUMBER                                      11

#define EFI_MSR_IA32_MTRR_FIX64K_00000         0x00000250
#define EFI_MSR_IA32_MTRR_FIX16K_80000         0x00000258
#define EFI_MSR_IA32_MTRR_FIX16K_A0000         0x00000259
#define EFI_MSR_IA32_MTRR_FIX4K_C0000          0x00000268
#define EFI_MSR_IA32_MTRR_FIX4K_C8000          0x00000269
#define EFI_MSR_IA32_MTRR_FIX4K_D0000          0x0000026A
#define EFI_MSR_IA32_MTRR_FIX4K_D8000          0x0000026B
#define EFI_MSR_IA32_MTRR_FIX4K_E0000          0x0000026C
#define EFI_MSR_IA32_MTRR_FIX4K_E8000          0x0000026D
#define EFI_MSR_IA32_MTRR_FIX4K_F0000          0x0000026E
#define EFI_MSR_IA32_MTRR_FIX4K_F8000          0x0000026F
#define EFI_MSR_CACHE_IA32_MTRR_DEF_TYPE       0x000002FF
#define   B_EFI_MSR_CACHE_MTRR_VALID                                   BIT11
#define   B_EFI_MSR_GLOBAL_MTRR_ENABLE                                 BIT11
#define   B_EFI_MSR_FIXED_MTRR_ENABLE                                  BIT10
#define   B_EFI_MSR_CACHE_MEMORY_TYPE                                  (BIT2 | BIT1 | BIT0)

#define EFI_MSR_VALID_MASK                     0xFFFFFFFFF
#define EFI_CACHE_VALID_ADDRESS                0xFFFFFF000
#define EFI_SMRR_CACHE_VALID_ADDRESS           0xFFFFF000
#define EFI_CACHE_VALID_EXTENDED_ADDRESS       0xFFFFFFFFFF000

// Leave one MTRR pairs for OS use
#define EFI_CACHE_NUM_VAR_MTRR_PAIRS_FOR_OS   1
#define EFI_CACHE_LAST_VARIABLE_MTRR_FOR_BIOS (EFI_MSR_CACHE_VARIABLE_MTRR_END) - \
        (EFI_CACHE_NUM_VAR_MTRR_PAIRS_FOR_OS * 2)

#define EFI_MSR_NO_EVICT_MODE                  0x000002E0
#define   B_EFI_MSR_NO_EVICT_MODE_RUN                                  BIT1
#define   B_EFI_MSR_NO_EVICT_MODE_SETUP                                BIT0

#define EFI_MSR_IA32_MC0_CTL                   0x00000400
#define EFI_MSR_IA32_MC0_STATUS                0x00000401
#define EFI_MSR_IA32_MC0_ADDR                  0x00000402
#define EFI_MSR_IA32_MC0_MISC                  0x00000403
#define EFI_MSR_IA32_MC8_CTL                   (EFI_IA32_MC0_CTL + (8*4))
#define EFI_MSR_IA32_MC8_STATUS                (EFI_IA32_MC0_STATUS + (8*4))

#define EFI_MSR_APIC_GLOBAL_ENABLE             0x00000800
#define EFI_MSR_EXT_XAPIC_LOGICAL_APIC_ID      0x00000802
#define EFI_MSR_EXT_XAPIC_VERSION              0x00000803
#define   B_EFI_MSR_EXT_XAPIC_VERSION_VERSION                          0xFF
#define EFI_MSR_EXT_XAPIC_SVR                  0x0000080F
#define EFI_MSR_EXT_XAPIC_ICR                  0x00000830
#define EFI_MSR_EXT_XAPIC_LVT_LINT0            0x00000835
#define EFI_MSR_EXT_XAPIC_LVT_LINT1            0x00000836

#define EFI_MSR_IA32_EFER                      0xC0000080
#define   B_EFI_MSR_IA32_EFER_NXE                                      BIT11
#define   B_EFI_MSR_IA32_EFER_LMA                                      BIT10
#define   B_EFI_MSR_IA32_EFER_LME                                      BIT8
#define   B_EFI_MSR_IA32_EFER_SCE                                      BIT0


//
// MSR specific for Pineview
//
#ifndef EFI_MSR_IA32_PLATFORM_ID
#define EFI_MSR_IA32_PLATFORM_ID               0x00000017
#endif

#define   B_EFI_MSR_IA32_PLATFORM_ID_MOBILE_PROCESSOR                  BIT28
#define     V_EFI_MSR_IA32_PLATFORM_ID_MOBILE_PROCESSOR                  BIT28
#define   B_EFI_MSR_IA32_PLATFORM_ID_PRODUCTION                        BIT27
#define   B_EFI_MSR_IA32_PLATFORM_ID_THERMAL_MONITOR_DISABLED          BIT19
#define   B_EFI_MSR_IA32_PLATFORM_ID_EIST_DISABLED                     BIT17
#define   B_EFI_MSR_IA32_PLATFORM_ID_RATIO_LOCKED                      BIT15
#define   N_EFI_MSR_IA32_PLATFORM_ID_MAX_BUS_RATIO                     8
#define   B_EFI_MSR_IA32_PLATFORM_ID_MAX_BUS_RATIO_MASK                (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_IA32_PLATFORM_ID_MAX_VID_MASK                      (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define EFI_MSR_EBL_CR_POWERON                 0x0000002A
#define   B_EFI_MSR_EBL_CR_POWERON_EXECUTEBIST                         BIT9

#ifndef EFI_MSR_IA32_FEATURE_CONTROL
#define EFI_MSR_IA32_FEATURE_CONTROL           0x0000003A
#endif
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_C_STATE_SMI                   BIT16

#define EFI_MSR_IA32_TEMPERATURE_OFFSET        0x0000003F
#define   B_EFI_MSR_IA32_TEMPERATURE_OFFSET_THERMAL_DIODE_OFFSET_MASK  (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE     0x00000080
#define   B_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_LOCK                    BIT0
#define   V_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK                    0x001FFFFF
#define   V_EFI_MSR_TRACE_HUB_STH_ACPIBAR_BASE_OFFSET                  0x00010040

#define EFI_MSR_UCODE_CR_PATCH_SIGN_ID         0x0000008B
#define   B_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_VERSION_MASK                (BIT43 | BIT42 | BIT41 | BIT40 | BIT39 | BIT38 | BIT37 | BIT36 | \
                                                                        BIT35 | BIT34 | BIT33 | BIT32)
#define   N_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_VERSION                     32
#define   V_EFI_MSR_UCODE_CR_PATCH_SIGN_ID_B5073167                    0x00E

#define EFI_MSR_SMM_CST_MISC_INFO              0x000000E1
#define   B_EFI_MSR_SMM_CST_MISC_INFO_PACKAGE_C_STATE_MASK             (BIT31 | BIT30 | BIT29 | BIT28)
#define   B_EFI_MSR_SMM_CST_MISC_INFO_PACKAGE_FREQUENCY_MASK           (BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_MSR_SMM_CST_MISC_INFO_THREAD_C_STATE_MASK              (BIT23 | BIT22 | BIT21 | BIT20)
#define   B_EFI_MSR_SMM_CST_MISC_INFO_L2_SHRINK_STATE_MASK             (BIT19 | BIT18 | BIT17)
#define   B_EFI_MSR_SMM_CST_MISC_INFO_THREAD_FREQUENCY                 BIT16
#define   B_EFI_MSR_SMM_CST_MISC_INFO_BLOCK_SMI                        BIT15
#define   B_EFI_MSR_SMM_CST_MISC_INFO_IO_TYPE_MASK                     (BIT7 | BIT6 | BIT5 | BIT4)
#define   B_EFI_MSR_SMM_CST_MISC_INFO_SC_SMI                           BIT0

#ifndef EFI_MSR_PMG_CST_CONFIG
#define EFI_MSR_PMG_CST_CONFIG                 0x000000E2
#endif
#define   B_EFI_MSR_PMG_CST_CONFIG_L2_SHRINK_THRESHOLD_MASK            (BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_MSR_PMG_CST_CONFIG_EIST_HARWARE_COORDINATION           BIT11
#define   B_EFI_MSR_PMG_CST_CONFIG_STPGNT_ISSUE                        BIT9
#define   B_EFI_MSR_PMG_CST_CONFIG_C4VID_DISABLE                       BIT7
#define   B_EFI_MSR_PMG_CST_CONFIG_CSM_TRIGGER_MASK                    (BIT6 | BIT5 | BIT4)
//#define   B_EFI_MSR_PMG_CST_CONFIG_DYNAMIC_L2_ENABLE                   BIT3

#define EFI_MSR_PMG_IO_BASE_ADDR               0x000000E3
#define   B_EFI_MSR_PMG_IO_BASE_ADDR_PMB1_MASK                         0xFFFF0000
#define   B_EFI_MSR_PMG_IO_BASE_ADDR_PMB0_MASK                         0x0000FFFF

//#define EFI_MSR_PMG_IO_CAPTURE_ADDR            0x000000E4   //For Nehalem Spec: EFI_IA32_PMG_IO_CAPTURE_BASE
#define   B_EFI_MSR_PMG_IO_CAPTURE_ADDR_CST_RANGE                      (BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)

#define EFI_BBL_CR_CTL3                        0x0000011E
#define   B_EFI_BBL_CR_CTL3_L2_WAY_SHRINK_MIN                          (BIT31 | BIT30)
#define   B_EFI_BBL_CR_CTL3_L2_WAY_SHRINK_RATE                         (BIT29 | BIT28)
#define   B_EFI_BBL_CR_CTL3_L2_WAY_CHUNK_SIZE                          (BIT27 | BIT26 | BIT25)
#define   B_EFI_BBL_CR_CTL3_L2_REDUCTION_CONF_LOCK                     BIT24
#define   B_EFI_BBL_CR_CTL3_L2_NOT_PRESENT                             BIT23
#define   B_EFI_BBL_CR_CTL3_SIZE_OF_WAY                                (BIT17 | BIT16 | BIT15 | BIT14 | BIT13)
#define   B_EFI_BBL_CR_CTL3_L2_ENABLED                                 BIT8
#define   B_EFI_BBL_CR_CTL3_NUMBER_OF_WAYS                             (BIT4 | BIT3 | BIT2 | BIT1)
#define   B_EFI_BBL_CR_CTL3_L2_HW_ENABLED                              BIT0

//#define EFI_MSR_CLOCK_FLEX_MAX                 0x00000194
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_RATIO_PINEVIEW                 (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_VID                            (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_IA32_PERF_STS
#define EFI_MSR_IA32_PERF_STS                  0x00000198
#endif

#define   B_EFI_MSR_IA32_PERF_STS_BUS_RATIO_BOOT                       (BIT60 | BIT59 | BIT58 | BIT57 | BIT56)
#define   B_EFI_MSR_IA32_PERF_STS_VID_BOOT                             (BIT53 | BIT52 | BIT51 | BIT50 | BIT49 | BIT48)
#define   B_EFI_MSR_IA32_PERF_STS_BUS_RATIO_MAX                        (BIT44 | BIT43 | BIT42 | BIT41 | BIT40)
#define   B_EFI_MSR_IA32_PERF_STS_VID_MAX                              (BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
#define   N_EFI_MSR_IA32_PERF_STS_BUT_RATIO_MIN                        24
#define   B_EFI_MSR_IA32_PERF_STS_BUT_RATIO_MIN_MASK                   (BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_MSR_IA32_PERF_STS_TS                                   BIT21
#define   B_EFI_MSR_IA32_PERF_STS_CMD_SEEK                             BIT20
#define   B_EFI_MSR_IA32_PERF_STS_THERM_THROT                          BIT19
#define   B_EFI_MSR_IA32_PERF_STS_TT                                   BIT18
#define   B_EFI_MSR_IA32_PERF_STS_VIP                                  BIT17
#define   B_EFI_MSR_IA32_PERF_STS_FIP                                  BIT16
#define   B_EFI_MSR_IA32_PERF_STS_BUS_RATIO_STS                        (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   N_EFI_MSR_IA32_PERF_STS_BUS_RATIO_STS                        8
#define   B_EFI_MSR_IA32_PERF_STS_VID_STS                              (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_IA32_PERF_CTRL
#define EFI_MSR_IA32_PERF_CTRL                 0x00000199
#endif

#define   B_EFI_MSR_IA32_PERF_CTRL_BUS_RATIO_SEL                       (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_IA32_PERF_CTRL_VID_SEL                             (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define EFI_MSR_THERM2_CTL                     0x0000019D
#define   B_EFI_MSR_THERM2_CTL_BUS_RATIO_THROT                         (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_THERM2_CTL_VID_THROT                               (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_IA32_MISC_ENABLE
#define EFI_MSR_IA32_MISC_ENABLE               0x000001A0
#endif
#define   B_EFI_MSR_IA32_MISC_ENABLE_HARD_C4E_EN                       BIT33
#define   B_EFI_MSR_IA32_MISC_ENABLE_C4E_EN                            BIT32
#define   B_EFI_MSR_IA32_MISC_ENABLE_C2E_EN                            BIT26
#define   B_EFI_MSR_IA32_MISC_ENABLE_C1E_EN                            BIT25
#define   B_EFI_MSR_IA32_MISC_ENABLE_FORCEPR_INPUT_EN                  BIT21
#define   B_EFI_MSR_IA32_MISC_ENABLE_EIST_LOCK                         BIT20
#define   B_EFI_MSR_IA32_MISC_ENABLE_BI_DIRECTIONAL_PROCHOT_EN         BIT17
#define   B_EFI_MSR_IA32_MISC_ENABLE_TM2_EN                            BIT13
#define   B_EFI_MSR_IA32_MISC_ENABLE_FERR_MULTIPLEXING_EN              BIT10

#ifndef EFI_MSR_PIC_SENS_CFG
#define EFI_MSR_PIC_SENS_CFG                   0x000001AA
#endif
#define   B_EFI_MSR_PIC_SENS_CFG_LOCK_THERMAL_INT                      BIT22
#define   B_EFI_MSR_PIC_SENS_CFG_LOCK_TM1                              BIT21
#define   B_EFI_MSR_PIC_SENS_CFG_TM1_ON_TM2                            BIT20
#define   B_EFI_MSR_PIC_SENS_CFG_CONVERSION_TIME                       (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_PIC_SENS_CFG_BYPASS_FILTER                         BIT4

#ifndef MSR_PLATFORM_INFO
#define MSR_PLATFORM_INFO                       0xCE
#endif
#define   B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED                        BIT37

#ifndef EFI_MSR_FREQ_LIMIT_RATIOS
#define EFI_MSR_FREQ_LIMIT_RATIOS                                      0x1AD
#endif
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_7                    (BIT63 | BIT62 | BIT61 | BIT60 | BIT59 | BIT58 | BIT57 | BIT56)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_6                    (BIT55 | BIT54 | BIT53 | BIT52 | BIT51 | BIT50 | BIT49 | BIT48)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_5                    (BIT47 | BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_4                    (BIT39 | BIT38 | BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_3                    (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_2                    (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_1                    (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_FREQ_LIMIT_RATIOS_RATIO_LIMIT_0                    (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_FREQ_LIMIT_CORES
#define EFI_MSR_FREQ_LIMIT_CORES                                       0x1AE
#endif
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_7                      (BIT63 | BIT62 | BIT61 | BIT60 | BIT59 | BIT58 | BIT57 | BIT56)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_6                      (BIT55 | BIT54 | BIT53 | BIT52 | BIT51 | BIT50 | BIT49 | BIT48)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_5                      (BIT47 | BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_4                      (BIT39 | BIT38 | BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_3                      (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_2                      (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_1                      (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_FREQ_LIMIT_CORES_CORE_COUNT_0                      (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_PRMRR_BASE
#define EFI_MSR_PRMRR_BASE                                             0x1F4
#endif
#define   B_EFI_MSR_PRMRR_BASE_ADDR_MASK                               0xFFFFF00000 //Bit[39:20]

#ifndef EFI_MSR_PRMRR_MASK
#define EFI_MSR_PRMRR_MASK                                             0x1F5
#endif
#define   B_EFI_MSR_PRMRR_MASK_ADDR_MASK                               0xFFFFF00000 //Bit[39:20]
#define   N_EFI_MSR_PRMRR_MASK                                         20
#define   B_EFI_MSR_PRMRR_MASK_VLD                                     BIT11


#ifndef EFI_MSR_PKG_POWER_SKU_UNIT
#define EFI_MSR_PKG_POWER_SKU_UNIT                                     0x606
#endif
#define   B_EFI_MSR_PKG_POWER_SKU_UNIT_RESISTANCE_UNIT                 (BIT31 | BIT30 | BIT29 | BIT28)
#define   B_EFI_MSR_PKG_POWER_SKU_UNIT_CURRENT_UNIT                    (BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_MSR_PKG_POWER_SKU_UNIT_TIME_UNIT                       (BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_MSR_PKG_POWER_SKU_UNIT_ENERGY_UNIT                     (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_PKG_POWER_SKU_UNIT_POWER_UNIT                      (BIT3 | BIT2 | BIT1 | BIT0)
#define   DEFAULT_PKG_POWER_SKU_UNIT                                   0x000A0505

#ifndef EFI_MSR_PKG_RAPL_LIMIT
#define EFI_MSR_PKG_RAPL_LIMIT                                         0x610
#endif

///
/// PWR_UNIT: Power Units used for power control registers.
/// The actual unit value is calculated by 1 W / Power2PWR_UNIT.
/// The default value of 0110b corresponds to 1/64 W (0.015)
///
#define   N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2                      32
#define   N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME                 17
#define   N_B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1                      0

#define   PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME_MASK                        0x7F
#define   PKG_RAPL_LIMIT_PKG_PWR_LIM1_MASK                             0x7FFF
#define   PKG_RAPL_LIMIT_PKG_PWR_LIM2_MASK                             0x7FFF

#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM_LOCK                    BIT63
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2_TIME                   (BIT55 | BIT54 | BIT53 | BIT52 | BIT51 | BIT50 | BIT49)
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_CLAMP_LIM2                      BIT48
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2_EN                     BIT47
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM2                        (BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40 | \
                                                                        BIT39 | BIT38 | BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1_TIME                   (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17)
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_CLAMP_LIM1                      BIT16
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1_EN                     BIT15
#define   B_EFI_MSR_PKG_RAPL_LIMIT_PKG_PWR_LIM1                        (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | \
                                                                       BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define   DEFAULT_PKG_RAPL_LIMIT                                       0x00158270001d8270

#ifndef EFI_MSR_PKG_TRUBO_POWER_LIMIT
#define EFI_MSR_PKG_TRUBO_POWER_LIMIT                                  0x610
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_PWR_LIM2_TIME            (BIT55 | BIT54 | BIT53 | BIT52 | BIT51 | BIT50 | BIT49)
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_CLAMP_LIM2               BIT48
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_PWR_LIM2_EN              BIT47
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_PWR_LIM2                 (BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40 | \
                                                                        BIT39 | BIT38 | BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_PWR_LIM1_TIME            (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17)
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_CLAMP_LIM1               BIT16
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_PWR_LIM1_EN              BIT15
#define   B_EFI_MSR_PKG_TRUBO_POWER_LIMIT_PKG_PWR_LIM1                 (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | \
                                                                       BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#endif

#ifndef EFI_MSR_PL3_CONTROL
  #define EFI_MSR_PL3_CONTROL                                          0x615
#endif
  #define B_EFI_MSR_PL3_CONTROL_LOCK                                   BIT63
  #define B_EFI_MSR_PL3_CONTROL_PL4_ENABLE                             BIT47
  #define B_EFI_MSR_PL3_CONTROL_PMAX                                   (BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40 | \
                                                                        BIT39 | BIT38 | BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
  #define B_EFI_MSR_PL3_CONTROL_DUTY_CYCLE                             (BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
  #define B_EFI_MSR_PL3_CONTROL_TIME_WINDOW                            (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17)
  #define B_EFI_MSR_PL3_CONTROL_PL3_ENABLE                             BIT15
  #define B_EFI_MSR_PL3_CONTROL_POWER_LIMIT                            (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | \
                                                                       BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_DDR_RAPL_LIMIT
  #define EFI_MSR_DDR_RAPL_LIMIT                                       0x618
#endif
  #define B_EFI_MSR_PDDR_RAPL_LIMIT_LOCKED                             BIT63

#define EFI_MSR_IACORE_RATIOS                                          0x66A
#define   B_EFI_MSR_IACORE_RATIOS_GUAR_RATIO                           (BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_MSR_IACORE_RATIOS_LFM_RATIO                            (BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_IACORE_RATIOS_MIN_RATIO                            (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef MSR_IACORE_RATIOS
#define MSR_IACORE_RATIOS                                              0x66a
#define MSR_IACORE_VIDS                                                0x66b
#define MSR_IACORE_TURBO_RATIOS                                        0x66c
  #define MAX_RATIO_4C_OFFSET	                                       24
  #define MAX_RATIO_3C_OFFSET	                                       16
  #define MAX_RATIO_2C_OFFSET	                                       8
  #define MAX_RATIO_1C_OFFSET	                                       0
  #define MAX_RATIO_1C_MASK 		                                   0x000000ff
  #define MAX_RATIO_2C_MASK 		                                   0x0000ff00
  #define MAX_RATIO_3C_MASK 		                                   0x00ff0000
  #define MAX_RATIO_4C_MASK 		                                   0xff000000
#define MSR_IACORE_TURBO_VIDS                                          0x66d
  #define MAX_VID_4C_OFFSET	                                           24
  #define MAX_VID_3C_OFFSET	                                           16
  #define MAX_VID_2C_OFFSET	                                           8
  #define MAX_VID_1C_OFFSET	                                           0
  #define MAX_RATIO_1C_MASK 		                                   0x000000ff
  #define MAX_RATIO_2C_MASK 		                                   0x0000ff00
  #define MAX_RATIO_3C_MASK 		                                   0x00ff0000
  #define MAX_RATIO_4C_MASK 		                                   0xff000000

#define MSR_PM_CFG_CTRL                                                0xE2
  #define C0_SUB_STATES_MASK                                           0x0000000f
  #define C1_SUB_STATES_MASK	                                       0x000000f0
  #define C2_SUB_STATES_MASK	                                       0x00000f00
  #define C3_SUB_STATES_MASK	                                       0x0000f000
  #define C4_SUB_STATES_MASK	                                       0x000f0000
  #define C5_SUB_STATES_MASK	                                       0x00f00000
  #define C6_SUB_STATES_MASK	                                       0x0f000000
  #define C7_SUB_STATES_MASK	                                       0xf0000000
  #define CSTATE_LIMIT_MASK                                            0xF
  #define CSTATE_LIMIT_NO_LIMIT                                        0x0
  #define CSTATE_LIMIT_C1                                              0x1
  #define CSTATE_LIMIT_C3                                              0x2
  #define CSTATE_LIMIT_C6                                              0x3
  #define CSTATE_LIMIT_C7                                              0x4
  #define CSTATE_LIMIT_C7S                                             0x5
  #define CSTATE_LIMIT_C8                                              0x6
  #define CSTATE_LIMIT_C9                                              0x7
  #define CSTATE_LIMIT_C10                                             0x8
  #define B_TIMED_MWAIT_ENABLE                                         BIT31

  #define B_IO_MWAIT_REDIRECTION_ENABLE                                BIT10
  #define B_SINGLE_PCTL                                                BIT11
  #define B_C3_AUTO_DEMOTION_ENABLE                                    BIT25
  #define B_C1_AUTO_DEMOTION_ENABLE                                    BIT26
  #define B_C3_AUTO_UNDEMOTION_ENABLE                                  BIT27
  #define B_C1_AUTO_UNDEMOTION_ENABLE                                  BIT28
  #define B_PKG_CSTATE_DEMOTION_ENABLE                                 BIT29
  #define B_PKG_CSTATE_UNDEMOTION_ENABLE                               BIT30

//
// Turbo
//
#define MSR_TURBO_RATIO_LIMIT		                                   0x1ad
  #define MAX_RATIO_LIMIT_4C_OFFSET	                                   24
  #define MAX_RATIO_LIMIT_3C_OFFSET	                                   16
  #define MAX_RATIO_LIMIT_2C_OFFSET	                                   8
  #define MAX_RATIO_LIMIT_1C_OFFSET	                                   0
  #define MAX_RATIO_LIMIT_MASK1		                                   0x000000ff
  #define MAX_RATIO_LIMIT_MASK2		                                   0x0000ff00
  #define MAX_RATIO_LIMIT_MASK3		                                   0x00ff0000
  #define MAX_RATIO_LIMIT_MASK4		                                   0xff000000
// MSR_IA32_MISC_ENABLE     0x1a0
  #define TURBO_DISABLE_MASK                                           ((UINT64)1 << 38)
  #define TURBO_MODE_DISABLE_BIT                                       38
//
// P-State
//
#define MSR_FLEX_RATIO                                                 0x194
#define MSR_CLOCK_FLEX_MAX                                             MSR_FLEX_RATIO
  #define ENABLE_FLEX                                                  BIT16
  #define RATIO_FLEX_BYTE_MASK                                         0xFF                  // Bits 15:8

#define MSR_POWER_CTL                                                  0x1fc
#define B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE                        BIT30
#define RATIO_FLEX_CLEAR_MASK                                          0xFFFFFFFFFFFF00FF      // Clear Bits 16:8
#define RATIO_FLEX_EN_MASK                                             0x0000000000010000      // Clear all but bit 16
#define MAX_NON_TURBO_MASK                                             0x000000000000FF00
#define MAX_EFFICIENCY_MASK                                            0x0000FF0000000000
//
// ACPI P-State Coordination Type
//
#define PSD_SW_ALL                                                     0xfc
#define PSD_SW_ANY                                                     0xfd
#define PSD_HW_ALL                                                     0xfe
#endif
//-----------------------------------------------------------------------------
// Thermal Management Registers
//-----------------------------------------------------------------------------
#define EFI_MSR_IA32_CR_THERM_INTERRUPT                                0x19b
  #define EFI_MSR_IA32_THERM_INTERRUPT_THRESHOLD2_INT_EN               BIT23
  #define EFI_MSR_IA32_THERM_INTERRUPT_TEMP_THRESHOLD_2                (BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
  #define EFI_MSR_IA32_THERM_INTERRUPT_THRESHOLD1_INT_EN               BIT15
  #define EFI_MSR_IA32_THERM_INTERRUPT_TEMP_THRESHOLD_1                (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
  #define EFI_MSR_IA32_THERM_INTERRUPT_OUTOFSPEC_INT_EN                BIT4
  #define EFI_MSR_IA32_THERM_INTERRUPT_PROCHOT_INT_EN                  BIT2
  #define EFI_MSR_IA32_THERM_INTERRUPT_LO_TEMP_INT_EN                  BIT1
  #define EFI_MSR_IA32_THERM_INTERRUPT_HI_TEMP_INT_EN                  BIT0
#define EFI_MSR_IA32_CR_THERM_STATUS                                   0x19c
#define EFI_MSR_CPU_THERM_TEMPERATURE                                  0x1a2
  #define B_EFI_MSR_TJ_TARGET_OFFSET                                   (BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define EFI_MSR_CPU_THERM_CFG1                                         0x673
#define EFI_MSR_CPU_THERM_CFG2                                         0x674
  #define B_EFI_MSR_CPU_THERM_CFG_BIDIR_PROCHOT                        (BIT0)
#define EFI_MSR_CPU_THERM_SENS_CFG                                     0x675


#define EFI_MSR_PKGC6_IRTL                                             0x60B
#define EFI_MSR_PKGC7_IRTL                                             0x60C
#define EFI_MSR_TURBO_POWER_LIMIT                                      0x610
#define EFI_MSR_PACKAGE_POWER_SKU                                      0x614
#define EFI_MSR_CPU_THERM_SENS_CFG                                     0x675
#ifndef EFI_MSR_EBC_HARD_POWERON
#define EFI_MSR_EBC_HARD_POWERON                                       0x2A
  #define STOP_ENABLE                                                  BIT19
#endif

#define EFI_MSR_PRIMARY_PLANE_CURRENT_CONFIG                           0x601
  #define CURRENT_LIMIT_MASK                                           0x1FFF      // Bits 12:0

#define EFI_MSR_SECONDARY_PLANE_CURRENT_CONFIG                         0x602

#define EFI_MSR_PRIMARY_PLANE_TURBO_POWER_LIMIT                        0x638
#define EFI_MSR_SECONDARY_PLANE_TURBO_POWER_LIMIT                      0x640
  #define PLANE_POWER_LIMIT_MASK                                       (0x7FFF)
  #define PLANE_POWER_LIMIT_ENABLE                                     BIT15
  #define PLANE_POWER_LIMIT_TIME_MASK                                  (0x7F)
  #define PLANE_POWER_LIMIT_TIME_OFFSET                                17
  #define PLANE_POWER_LIMIT_LOCK                                       BIT31

#endif
