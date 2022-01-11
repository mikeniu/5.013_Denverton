/******************************************************************************
SMI based Copy To Flash Sample Code 

Copyright 2011-2016 Intel Corporation All Rights Reserved.

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
notice or any other notice embedded in Materials by Intel or Intel’s suppliers
or licensors in any way.

******************************************************************************/

#ifndef _COPY_TO_FLASH_H_
#define _COPY_TO_FLASH_H_

#include <PiSmm.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>

#define MAX_SIR_CONFIGURATION               3     //Number of Sata Initialization registers to be configured

typedef	struct SIRPAIR_ {
	UINT8	addr;
	UINT32	data;
} SIRPAIR;

//#define EXTRA_DEBUG 1


#define MAX_PRDT_BYTECOUNT	(4 * 1024 * 1024)
#define MAX_COMMAND_BYTECOUNT (512 * 0x10000)
#define MAX_SECTOR_COUNT (0x10000)
#define COMMAND_SLOTS 32


#define APIC_EOI	0xB0
#define APIC_ICR_LOW	0x300
#define APIC_ICR_HIGH	0x310


#define IOAPIC_INDEX	0x0
#define IOAPIC_DATA	0x10
#define IOAPIC_EOI	0x40

#define WRITE 1
#define READ  0

// PLATFORM DEPENDENT DEFINES
#define MWAIT_HINT	0x1
#define OTHERCORE_MWAIT_HINT	0x20
#define MMCONFIG_BASE	0xE0000000
#define RCBA		0xFED1C000
#define ABAR_LOC	0xF8000000
#define APIC_BASE	0xFEE00000
#define IOAPIC_BASE	0xFEC00000

#define C2F_PORT_NUM	0	// SATA Port Nubmer with C2F SSD
#define IOAPIC_AID	32

#define SIPI_AREA		0x3000
#define SIPI_MONITOR_LOC	0x4000


#define INT_NUM		19
#define VECTOR_NUM	0xCF

// 
// CMOS ADDRESS FOR C2F_STATUS
// Store in this address represent a state for
// ADR-C2F path
// 0: C2F is not needed
// 1: C2F entry should be executed
// 2: C2F recovery should be executed / skip C2F entry
//
#define CMOS_INDEX 0x70
#define CMOS_DATA 0x71
#define C2F_CMOS_LOC 0x53

//#define monitor __asm _emit 0x0F __asm _emit 0x01 __asm _emit 0xC8
//#define mwait __asm _emit 0x0F __asm _emit 0x01 __asm _emit 0xC9






















#endif
