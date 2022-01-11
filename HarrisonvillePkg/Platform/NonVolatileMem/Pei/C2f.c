/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

@copyright
  Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement


Module Name:

  C2f.c

Abstract:

  Saves or restores DIMM data from flash

--*/
#include "C2F.h"
#include <Library/DebugLib.h>
#include <Include/SetupVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/C2fAhciLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include "PlatformDefinitions.h"
#include <Pi/PiHob.h>
#include <Library/ResetSystemLib.h>
#include <PlatformDefinitions.h>
#include <Platform.h>
#include <Library/AdrCfgLib.h>

#define EXTRA_DEBUG

#define INT_NUM		  0xB
#define VECTOR_NUM	  0x20
#define IOAPIC_BASE   (LOCAL_APIC_ADDRESS)
#define IOAPIC_AID	  2
#define IOAPIC_INDEX  0x0
#define IOAPIC_DATA	  0x10

#define CPLD_CONFIG_PORT               (0x50C)
#define AC_GOOD_LATCHED_BIT_MSK        (1<<12)

UINT8 c2f_entry_flag(void);


UINT8 c2f_entry_flag(void)
{
  // Check AC_GOOD_LATCHED GPIO12
  if (0 == AdrCfg_Active()) {
    return 0;
  } else {
    AdrCfg_Deactivate();
    return 1;
  }
}

void shutdown_platform()
{
  UINT32 PMBASE = PCH_ACPI_BASE_ADDRESS;
  IoWrite32(PMBASE+0x30,(IoRead32(PMBASE+0x30) & (~(UINT32)(1<<4)))); // Make sure this won't generate an SMI
  IoWrite32(PMBASE+0x4,((7 << 10) | (1 << 13)));
  while(1);
}

extern void isr();
__declspec( naked ) void isr(void)
{
	__asm push eax
	__asm push ebx
	__asm push edx

	AhciInterruptHandler();
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "C2F.c::isr()\n"));
#endif
	__asm pop edx
	__asm pop ebx
	__asm pop eax
   __asm iretd
}


void HookInterrupt()
{

	UINT8   buffer[16];
	struct  InterruptGate32 * intstruct;
	UINT16  MaxVector;
	UINT64  isrloc;
	UINT8 * pointer8;
	UINT32* pointer32;
	UINT16  SegmentSelector = 0;
//	UINT32 data32;
//  UINT8 vectornum;


	ZeroMem(buffer,sizeof(buffer));
	DEBUG ((EFI_D_ERROR, "IOAPIC_BASE=0x%08x\n", (UINT32)(UINTN)IOAPIC_BASE));
	pointer8 = (UINT8*)(IOAPIC_BASE + IOAPIC_INDEX);
	pointer32 = (UINT32*)(IOAPIC_BASE + IOAPIC_DATA);

	// Set the APIC ID for Patsburg APIC incase it isn't set yet.
	*pointer8 = 0;
	if (0 == (*pointer32>>24)){
		 DEBUG ((EFI_D_ERROR, "IOAPIC ID 0x%x <= %x\n", (*pointer32>>24), IOAPIC_AID));
		*pointer32 = (IOAPIC_AID << 24);
	}
	DEBUG ((EFI_D_ERROR, "IOAPIC ID 0x%x\n", (*pointer32>>24)));

#if 0 //INT_NUM
#endif

	__sidt(buffer);

#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "SIDT results\n"));
	dumpdata(buffer,16);
#endif
	intstruct = *((INTERRUPT_GATE32 **)(&buffer[2]));
	ASSERT(NULL != intstruct);
	if (intstruct == NULL) {
      DEBUG ((EFI_D_ERROR, "HookInterrupt() - intstruct pointer is null!\n"));
      return;
    }
	MaxVector = ((buffer[1]<<8)|(buffer[0]+1))>>3;
	DEBUG ((EFI_D_ERROR, "\nidt location=0x%08x  0x%x of 0x%x vectors\n", (UINT32)(UINTN)intstruct, VECTOR_NUM, MaxVector));
	ASSERT(VECTOR_NUM <= MaxVector);
	//
	// get the code segment from the first IDT entry
	//
	SegmentSelector = intstruct->Segment;
	ASSERT(SegmentSelector != 0);
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Dumping Div by zero fault IDT[%x]\n", 0));
	dumpdata(intstruct,sizeof(*intstruct));
	DEBUG ((EFI_D_ERROR, "Code Segment Selector: %x\n", SegmentSelector));
#endif
	//
	// setup the IDT entry for the vector in the SATA MSI write to call the ISR function
	//
	intstruct+= VECTOR_NUM;
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Dumping original Interrupt Gate IDT[%x]\n", VECTOR_NUM));
	dumpdata(intstruct,sizeof(*intstruct));
#endif
	isrloc = ((UINTN)intstruct->MiddleOffset << 16) + intstruct->LowerOffset;
	DEBUG ((EFI_D_ERROR, "Current Interrupt Loccation = 0x%08lx\n",isrloc));
	isrloc = (UINTN)&isr;
	DEBUG ((EFI_D_ERROR, "New Interrupt Loccation = 0x%08lx\n",isrloc));

	intstruct->LowerOffset = (UINT32)isrloc & 0xFFFF;
	// Denverton Aptiov override start
	//intstruct->MiddleOffset = ((UINT32)isrloc >> 16) & 0xFFFF;
	intstruct->MiddleOffset = RShiftU64 (isrloc, 16 ) & 0xFFFF;
	// Denverton Aptiov override start
	intstruct->Segment = SegmentSelector;
	intstruct->Misc = (1 << 9) | (1 << 10) | (1 << 11) | (1 << 15);

#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Dumping AHCI Interrupt Gate\n"));
	dumpdata(intstruct,sizeof(*intstruct));
#endif
}

EFI_STATUS DetermineDataSaveRanges(
  IN EFI_PEI_SERVICES                  **PeiServices,
  IN OUT  NON_VOLATILE_MEM_POLICY_PPI  * NonVolPpi
  )
/*++

Routine Description:
  Determines if C2F is enabled and what regions of memory need to be saved/restored

Arguments:
  NonVolPpi    points to the NonVolatile Memory policy

Returns:

  EFI_SUCCESS     The function completed successfully by finding all data needed.

--*/
{
	EFI_STATUS                  Status = EFI_SUCCESS;

	NonVolPpi->C2fLowStart = NonVolPpi->C2fLowLength = 0;
	NonVolPpi->C2fHighStart = NonVolPpi->C2fHighLength = 0;

	if ( NONVOL_C2F_MODE(NonVolPpi->NonVolMemMode) ) {
		EFI_HOB_RESOURCE_DESCRIPTOR 	*ResourceHob;
		EFI_PEI_HOB_POINTERS        	Hob;

		DEBUG ((DEBUG_INFO, "C2F Mode Enabled\n"));
		Status            = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
		while (!END_OF_HOB_LIST (Hob)) {
			if (Hob.Raw != NULL && GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
				DEBUG ((DEBUG_INFO, "Found Resource HOB, Start=0x%lX, Length=0x%lX, Type=0x%08x, Attributes=0x%08x\n", Hob.ResourceDescriptor->PhysicalStart, Hob.ResourceDescriptor->ResourceLength, Hob.ResourceDescriptor->ResourceType, Hob.ResourceDescriptor->ResourceAttribute));
				ResourceHob = Hob.ResourceDescriptor;
				if ( (EFI_RESOURCE_SYSTEM_MEMORY == ResourceHob->ResourceType)
					 && (EFI_RESOURCE_ATTRIBUTE_PERSISTENT == (Hob.ResourceDescriptor->ResourceAttribute & EFI_RESOURCE_ATTRIBUTE_PERSISTENT))
				   ) {
					if (ResourceHob->PhysicalStart < 0x100000000) {
						if(NonVolPpi->C2fLowStart == 0) {//Fill low values is still zero
							NonVolPpi->C2fLowStart = (UINT32)ResourceHob->PhysicalStart;
							NonVolPpi->C2fLowLength += (UINT32)ResourceHob->ResourceLength;
							DEBUG ((DEBUG_INFO, "Found C2F Low region start=0x%08x, length=0x%08x\n", NonVolPpi->C2fLowStart, NonVolPpi->C2fLowLength));

						} else {
							DEBUG ((DEBUG_ERROR, "2nd Low region found start=0x%08x, length=0x%08x\n", (UINT32)ResourceHob->PhysicalStart, (UINT32)ResourceHob->ResourceLength));
						}
					} else {
						if(!NonVolPpi->C2fHighStart) { //Fill high values is still zero
							NonVolPpi->C2fHighStart = ResourceHob->PhysicalStart;
							NonVolPpi->C2fHighLength += ResourceHob->ResourceLength;
							DEBUG ((DEBUG_INFO, "Found C2F High region start=0x%lx, length=0x%lx\n", NonVolPpi->C2fHighStart, NonVolPpi->C2fHighLength));

						} else {
							DEBUG ((DEBUG_ERROR, "2nd High region found start=0x%lx, length=0x%lx\n", ResourceHob->PhysicalStart, ResourceHob->ResourceLength));
						}
					}
				}/* else if (EFI_RESOURCE_SYSTEM_MEMORY == ResourceHob->ResourceType) {
					if (NonVolPpi->LargestFreeblockLength < ResourceHob->ResourceLength) {
						NonVolPpi->LargestFreeblockLength = ResourceHob->ResourceLength;
						NonVolPpi->LargestFreeblock = ResourceHob->PhysicalStart;
						DEBUG ((DEBUG_ERROR, "Free region found start=0x%lx, length=0x%lx\n", ResourceHob->PhysicalStart, ResourceHob->ResourceLength));
					}
				}*/
			}
			Hob.Raw = GET_NEXT_HOB (Hob);
		}
		if (NonVolPpi->C2fLowStart == NonVolPpi->C2fHighStart) {
			DEBUG ((DEBUG_ERROR, "No Non-volatile memory regions were found Low={%x,%x} High={%x,%x}\n",NonVolPpi->C2fLowStart, NonVolPpi->C2fLowLength,NonVolPpi->C2fHighStart, NonVolPpi->C2fHighLength));
			Status = EFI_INVALID_PARAMETER;
		}
	}

	return Status;
}





EFI_STATUS
EFIAPI
C2FCallback (
	IN EFI_PEI_SERVICES                 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
	IN VOID                       *Ppi
  )
/*++

Routine Description:

  Copy or restore DIMM data from flash

Arguments:

  PeiServices       General purpose services available to every PEIM.
  NotifyDescriptor  Notify that this module published.
  Ppi               PPI that was installed.

Returns:

  EFI_SUCCESS     The function completed successfully.

--*/
{
	EFI_STATUS                   Status = EFI_SUCCESS;
//  C2F_XFER_METHOD const        method = C2F_INTERRUPT;
  C2F_XFER_METHOD const        method = C2F_POLLED;
	C2F_XFER_DIRECTION           Action = C2F_INACTIVE;
	C2F_MEMORY_REGION            MemoryRange;
	C2F_STORAGE_DEVICE           StorageDevice;
	NON_VOLATILE_MEM_POLICY_PPI *NonVolPpi = NULL;
	UINTN const                  AhciMmio = PCIEX_BASE_ADDRESS | PCI_LIB_ADDRESS( 0, 20, 0, 0 );
	UINT8 const                  SataPort = 4;

	DEBUG ((DEBUG_INFO, "%a() - NonVolatile memory service\n",__FUNCTION__));

	Status = (**PeiServices).LocatePpi (
				PeiServices,
				&gNonVolatileMemPpiGuid,
				0,
				NULL,
				(VOID **)&NonVolPpi
				);
        ASSERT_EFI_ERROR(Status);

        if (Status != EFI_SUCCESS) {
          return Status;
        }

  if (c2f_data_in_ssd) {
    DEBUG ((DEBUG_INFO, "C2F Recovery\n"));
    Action = C2F_FLASH_TO_RAM;
  } else if (c2f_entry_flag()) 	{
    DEBUG ((DEBUG_INFO, "C2F Entry\n"));
    Action = C2F_RAM_TO_FLASH;
  }

	if (2 == NonVolPpi->NonVolMemTestMode) Action = C2F_RAM_TO_FLASH;
	if (3 == NonVolPpi->NonVolMemTestMode) Action = C2F_FLASH_TO_RAM;

	if (C2F_INACTIVE != Action) {
		//Post MRC execution is only needed in C2F modes
		Status = DetermineDataSaveRanges(PeiServices, NonVolPpi);
		DEBUG ((DEBUG_INFO,"NonVolatileMemHob {NonVolMemMode:%x, ",NonVolPpi->NonVolMemMode));
		DEBUG ((DEBUG_INFO,"\nC2fLowStart:%x, C2fLowLength:%x, ",NonVolPpi->C2fLowStart,NonVolPpi->C2fLowLength));
		DEBUG ((DEBUG_INFO,"C2fHighStart:%lx, C2fHighLength:%lx, ",NonVolPpi->C2fHighStart,NonVolPpi->C2fHighLength));
		DEBUG ((DEBUG_INFO,"\nLargestFreeblock:%lx, LargestFreeblockLength:%lx}\n",NonVolPpi->LargestFreeblock, NonVolPpi->LargestFreeblockLength));
		ASSERT(NONVOL_C2F_MODE(NonVolPpi->NonVolMemMode));

  	    if (0 != NonVolPpi->NonVolMemTestMode) {
			Status = EFI_SUCCESS;
			DEBUG ((DEBUG_INFO,"C2fLowStart:%x, C2fLowLength:%x, ", NonVolPpi->C2fLowStart, NonVolPpi->C2fLowLength));
			DEBUG ((DEBUG_INFO,"C2fHighStart:%lx, C2fHighLength:%lx\n", NonVolPpi->C2fHighStart, NonVolPpi->C2fHighLength));
			DEBUG ((DEBUG_INFO,"Action:0x%x write:%d\n", Action, (C2F_RAM_TO_FLASH == Action) ));
		}
		ASSERT_EFI_ERROR(Status);

		if (C2F_INTERRUPT == method) {
			DEBUG ((EFI_D_ERROR, "About to Hook the interrupt\n"));
			HookInterrupt();
		}

		if (C2F_RAM_TO_FLASH == Action) {
      c2f_save_started();
    } else {
      c2f_restore_started();
    }
		MemoryRange.LowStart = NonVolPpi->C2fLowStart;
		MemoryRange.LowSize = NonVolPpi->C2fLowLength;
		MemoryRange.HighStart = NonVolPpi->C2fHighStart;
		MemoryRange.HighSize = NonVolPpi->C2fHighLength;
		StorageDevice.AhciMmio = AhciMmio;
		StorageDevice.SataPort = SataPort;
		StorageDevice.LbaLoc = NonVolPpi->NonVolMemC2fLbaLocMethod;
		StorageDevice.LBA = NonVolPpi->NonVolMemC2fLba;
		StorageDevice.Method = method;
		StorageDevice.Vector = VECTOR_NUM;

        Status = c2f_entry(&MemoryRange, Action, &StorageDevice );
		if(Status != EFI_SUCCESS) {
      c2f_data_invalidated();
			DEBUG ((EFI_D_ERROR, "C2F failed \n"));
			shutdown_platform();
		} else if (C2F_FLASH_TO_RAM == Action) {
		  c2f_data_invalidated();
			DEBUG ((DEBUG_INFO, "C2F restore completed\n"));
		} else if (C2F_RAM_TO_FLASH == Action) {
		  c2f_save_completed();
      DEBUG ((DEBUG_INFO, "C2F save completed\n"));
			shutdown_platform();
		}
	} else {
    DEBUG ((DEBUG_INFO, "C2F state is inactive\n"));
	}

   return Status;
}
