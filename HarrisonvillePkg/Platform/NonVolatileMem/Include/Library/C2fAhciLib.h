/******************************************************************************
Copy To Flash Sample Code

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
#ifndef __C2F_AHCI_LIB_H__
#define __C2F_AHCI_LIB_H__
#include <Uefi.h>

typedef struct InterruptGate32
{
	UINT16 LowerOffset;
	UINT16 Segment;
	UINT16 Misc;
	UINT16 MiddleOffset;
} INTERRUPT_GATE32;

typedef struct InterruptGate64
{
	UINT16 LowerOffset;
	UINT16 Segment;
	UINT16 Misc;
	UINT16 MiddleOffset;
	UINT32 UpperOffset;
	UINT32 Rsvd;
} INTERRUPT_GATE64;


typedef enum {
  C2F_FLASH_TO_RAM = 0,
  C2F_RAM_TO_FLASH = 1,
  C2F_INACTIVE = 4
} C2F_XFER_DIRECTION;

typedef enum {
  C2F_POLLED = 0,
  C2F_INTERRUPT = 1
} C2F_XFER_METHOD;

typedef enum {
  C2F_FIXED = 0,
  C2F_PARTITION = 1
} C2F_LBA_LOC_METHOD;

typedef struct C2fMemoryRegion {
	UINT32 LowStart;
	UINT32 LowSize;
	UINT64 HighStart;
	UINT64 HighSize;
} C2F_MEMORY_REGION;

typedef struct C2fStorageDevice {
	UINT32             AhciMmio;
	UINT8              SataPort;
	C2F_XFER_METHOD    Method;
	UINT16             Vector;
	C2F_LBA_LOC_METHOD LbaLoc;
	UINT32             LBA;
} C2F_STORAGE_DEVICE;


typedef enum {
  C2F_DATA_EMPTY = 0xC0,
  C2F_DATA_SAVING = 0xC1,
  C2F_DATA_RESTORINGING = 0xC2,
  C2F_DATA_VALID = 0xC3
} C2F_SAVED_DATA_STATE;

UINT8 c2f_recovery_flag(void);
VOID c2f_save_started(void);
VOID c2f_save_completed(void);
VOID c2f_data_invalidated(void);
VOID c2f_restore_started(void);
UINT8 cmos_read(UINT8 loc);
#define C2F_CMOS_LOC 0x53
#define c2f_data_in_ssd (C2F_DATA_VALID == cmos_read(C2F_CMOS_LOC))

EFI_STATUS c2f_entry( C2F_MEMORY_REGION *MemoryRange, C2F_XFER_DIRECTION Direction, C2F_STORAGE_DEVICE *StorageDevice );

VOID AhciInterruptHandler (VOID);
void dumpdata( void * start, UINT32 size);


#endif //__C2F_AHCI_LIB_H__
