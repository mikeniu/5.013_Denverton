/*++

Copyright (c)  1999 - 2009, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CMOSMap.h

Abstract:

 

--*/

#ifndef _CMOSMAP_H
#define _CMOSMAP_H


//
// CMOS usage Upper CMOS bank offsets:
//
#define CMOS_CPU_BSP_SELECT             0x10
#define CMOS_CPU_UP_MODE                0x11
#define CMOS_CPU_RATIO_OFFSET           0x12
#define CMOS_CPU_CORE_HT_OFFSET         0x13
#define CMOS_EFI_DEBUG                  0x14
#define CMOS_CPU_BIST_OFFSET            0x15
#define CMOS_CPU_VMX_OFFSET             0x16
#define CMOS_PORT80_OFFSET              0x17
#define CMOS_PLATFORM_DESIGNATOR        0x18                          // Second bank CMOS location of Platform ID
#define CMOS_VALIDATION_TEST_BYTE       0x19                          // BIT0 - Validation mailbox for UPonDP
#define CMOS_SERIAL_BAUD_RATE           0x1A                          // 0=115200; 1=57600; 2=38400; 3=19200; 4=9600
#define CMOS_DCU_MODE_OFFSET            0x1B                          
#define CMOS_VR11_SET_OFFSET            0x1C                          
#define CMOS_SBSP_TO_AP_COMM            0x20                          // SEC code use ONLY!!!
#define TCG_CMOS_AREA_OFFSET            0x60                          // Also Change in Universal\Security\Tpm\PhysicalPresence\Dxe\PhysicalPresence.c &
                                                                      // Also Change in Platform\IntelEpg\Thurley\Dxe\AcpiTables\Dsdt\Tpm.asi
#define TCG_CMOS_MOR_AREA_OFFSET        (TCG_CMOS_AREA_OFFSET + 0x05) // Also Change in Platform\IntelEpg\Thurley\Dxe\AcpiTables\Dsdt\Tpm.asi


#define EFI_CMOS_START_ADDRESS                      0x40
#define EFI_CMOS_END_ADDRESS                        0x7F

#define EFI_CMOS_CHECKSUM_ADDRESS                   0x4F
#define EFI_CMOS_HYPERBOOT_FLAGS                    0x50
#define   B_CMOS_FIRST_BOOT                         0x01
#define   B_CMOS_BOOT_SUCCESS                       0x02
#define   B_CMOS_HYPERBOOT_STATUS                   0x04
#define   B_CMOS_HYPERBOOT_RECOVERY                 0x08
#define   B_CMOS_BOOT_FAILED                        0x10
#define   B_CMOS_BOOT_LOCK                          0x20
#define EFI_CMOS_BOOT_CFG_FLAGS                     0x51
#define   B_CMOS_BOOT_CFG_EXIST                     0x01
#define   B_CMOS_BOOT_CFG_BOOT_MENU                 0x02
#define   B_CMOS_BOOT_CFG_BOOT_CD                   0x04
#define   B_CMOS_BOOT_CFG_BOOT_FDD                  0x08
#define   B_CMOS_BOOT_CFG_BOOT_PXE                  0x10
#define   B_CMOS_BOOT_CFG_BOOT_USB                  0x20
#define   B_CMOS_BOOT_CFG_USB_FIRST                 0x40
#define   B_CMOS_BOOT_CFG_BOOT_UEFI                 0x80
#define EFI_CMOS_PENDING_ME_BIOS_ACTION             0x6D
#define EFI_CMOS_S4_WAKEUP_FLAG_ADDRESS             0x6E
#define EFI_CMOS_XP_FLAG_ADDRESS                    0x6F
#define EFI_CMOS_CAPSULE_ADDRESS_1                  0x70
#define EFI_CMOS_CAPSULE_ADDRESS_2                  0x71
#define EFI_CMOS_CAPSULE_ADDRESS_3                  0x72
#define EFI_CMOS_CAPSULE_ADDRESS_4                  0x73
#define EFI_CMOS_PERFORMANCE_FLAGS                  0x74
#define   B_CMOS_MEMORY_INIT                          0x01
#define   B_CMOS_FORCED_REBOOT                        0x02
#define   B_CMOS_ALLOW_RESET                          0x04
#define   B_CMOS_WD_RUNNING_FROM_OS                   0x08
#define   B_CMOS_WD_FAILURE_STATUS_TO_OS              0x10
#define   B_CMOS_BIOS_RESET_PERF_SETTINGS_TO_OS       0x20
#define   B_CMOS_TCO_WDT_RESET                        0x40
#define EFI_ACPI_TPM_REQUEST                        0x75
#define EFI_ACPI_TPM_LAST_REQUEST                   0x76
#define EFI_ACPI_TPM_MORD                           0x77
#define EFI_CMOS_UCLK_DEFAULT                       0x78
#define EFI_CMOS_CCLK_DEFAULT                       0x79
#define EFI_CMOS_QCLK_DEFAULT                       0x7A
#define EFI_CMOS_BURN_IN_MODE_FLAGS                 0x7C
#define   B_CMOS_BIM_HANG                             0x01
#define EFI_CMOS_ACPI_TABLE_FLAG_ADDRESS            0x7D
#define   B_CMOS_HPET_ENABLED                        0x01
#define EFI_CMOS_BOOT_FLAG_ADDRESS                  0x7E
#define   B_CMOS_THERMAL_TRIP                         0x01
#define   B_CMOS_FORCE_ENTER_SETUP                    0x02
#define   B_CMOS_FORCE_NETWORK_BOOT                   0x04
#define   B_CMOS_TPM_ENABLED                          0x08
#define EFI_CMOS_SX_STATE_FLAG_ADDRESS              0x7F
#define   B_CMOS_S5_SHUTDOWN                          0x01

#define EFI_CMOS_EOL  0xFFFF
#define EFI_CMOS_CHECKSUM_EXCLUDES {EFI_CMOS_CHECKSUM_ADDRESS, \
                                    EFI_CMOS_XP_FLAG_ADDRESS, \
                                    EFI_ACPI_TPM_REQUEST, \
                                    EFI_ACPI_TPM_LAST_REQUEST, \
                                    EFI_ACPI_TPM_MORD, \
                                    EFI_CMOS_BOOT_FLAG_ADDRESS, \
                                    EFI_CMOS_S4_WAKEUP_FLAG_ADDRESS, \
                                    EFI_CMOS_ACPI_TABLE_FLAG_ADDRESS, \
                                    EFI_CMOS_SX_STATE_FLAG_ADDRESS, \
                                    EFI_CMOS_PERFORMANCE_FLAGS, \
                                    EFI_CMOS_BURN_IN_MODE_FLAGS, \
                                    EFI_CMOS_HYPERBOOT_FLAGS, \
                                    EFI_CMOS_BOOT_CFG_FLAGS, \
                                    EFI_CMOS_EOL }

#endif
