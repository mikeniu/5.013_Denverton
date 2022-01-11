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

#include "C2fAhci.h"
#include <Base.h>
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <CpuDataStruct.h>
#include <CpuBaseLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "C2fAhciDebug.h"
#include <Platform.h>
#include <Uefi/UefiGpt.h>

#define APIC_BASE (LOCAL_APIC_ADDRESS)
#define AsmPause() { _mm_pause(); }
#define MSI_CAPABILITIES_ID (0x5)

#define DEBUG_PORT (0x80)
#define PORT_SPEC_REGS_SIZE (0x80)
#define BITS_32_63          (0xffffffff00000000)
#define BITS_0_31           (0xffffffff)
#define FOUR_GIG            (0x100000000)

#define REG_H2D_FIS                    (0x27)
#define AHCI_Identify_CMD              (0xEC)
#define AHCI_SET_FEAT_CMD              (0xEF)
#define AHCI_WRITE_DMA_EXT_CMD         (0x35)/*works in Simics*/
#define AHCI_WRITE_DMA_FUA_EXT_CMD     (0x3D)
#define AHCI_WRITE_DMA                 (AHCI_WRITE_DMA_FUA_EXT_CMD)
#define AHCI_READ_DMA_EXT_CMD          (0x25)
#define PxSSTS_DET_MASK                (0xf)
#define PxSSTS_DET_DEVICE_COMM_STARTED (3)

#define CMOS_INDEX 0x70
#define CMOS_DATA 0x71

//
// Copy to Flash GUID used for drive partitions {6868193C-A1E5-46a0-8396-F4A4990F36FD}
//
#define C2F_PARTITION_GUID_VALUE \
  { \
    0x6868193c, 0xa1e5, 0x46a0, 0x83, 0x96, 0xf4, 0xa4, 0x99, 0xf, 0x36, 0xfd \
  }
EFI_GUID  gC2fPartitionGuid = C2F_PARTITION_GUID_VALUE;

CHAR16* gC2fPartitionName = C2F_NAME;

typedef UINT16 volatile * UINT16_REG;
typedef UINT32 volatile * UINT32_REG;

UINT32 configureAHCI(UINTN SataMmioSpace, UINT8 SataPort);
void StartDriveComms(UINT32 abar, UINT8 SataPort);
UINT32 runCommands( UINT32 abar, UINT8 SataPort, struct CommandHeader * CommandHeaderEntry, struct ReceivedFIS * ReceivedFISArea, UINT32 commandMask, C2F_XFER_METHOD Method);
void transferData( UINT32 abar, UINT8 SataPort, UINT64 hostaddr, UINT64 bytes, UINT32 lba, UINT32 sectorsize, UINT8 write, C2F_XFER_METHOD Method);

UINT8 cmos_read(UINT8 loc)
{
  IoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
  return IoRead8(CMOS_DATA);
}

UINT8 c2f_recovery_flag(void)
{
  IoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
  DEBUG (( DEBUG_INFO, "%a() IO(0x%x):0x%x == %x  %x\n", __FUNCTION__, C2F_CMOS_LOC, IoRead8(CMOS_DATA), C2F_DATA_VALID, (C2F_DATA_VALID==IoRead8(CMOS_DATA)) ));
  return ( C2F_DATA_VALID == IoRead8(CMOS_DATA) );
}

VOID c2f_save_started(void)
{
        IoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
        IoWrite8(CMOS_DATA, C2F_DATA_SAVING); // Indicate that a transfer began
        DEBUG ((DEBUG_INFO, "%a() IO(0x%x)=0x%x\n", __FUNCTION__, C2F_CMOS_LOC, IoRead8(CMOS_DATA)));
}

VOID c2f_restore_started(void)
{
        IoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
        IoWrite8(CMOS_DATA, C2F_DATA_RESTORINGING); // Indicate that a transfer began
        DEBUG ((DEBUG_INFO, "%a() IO(0x%x)=0x%x\n", __FUNCTION__, C2F_CMOS_LOC, IoRead8(CMOS_DATA)));
}

VOID c2f_save_completed(void)
{
        IoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
        IoWrite8(CMOS_DATA, C2F_DATA_VALID); // Indicate that a transfer completed
        DEBUG ((DEBUG_INFO, "%a() IO(0x%x)=0x%x\n", __FUNCTION__, C2F_CMOS_LOC, IoRead8(CMOS_DATA)));
}
VOID c2f_data_invalidated(void)
{
        IoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
        IoWrite8(CMOS_DATA, C2F_DATA_EMPTY); // Indicate that a transfer completed
        DEBUG ((DEBUG_INFO, "%a() IO(0x%x)=0x%x\n", __FUNCTION__, C2F_CMOS_LOC, IoRead8(CMOS_DATA)));
}

void spinloop(UINT32 count)
{
	volatile UINT32 i;
	for (i = 0; i < count; i++);
}

UINT32 get_upper32(UINT64 value)
{
	//UINT32 *Dwords = (UINT32*)&value;
	UINT32  upper;

	// causing _aullshr to be pulled in 'upper = (UINT32)((value & BITS_32_63) >> 32);'
	//upper = Dwords[1];
	upper = (UINT32)RShiftU64 (value, 32);

	return upper;
}

UINT32 get_lower32(UINT64 value)
{
	UINT32 lower;
	lower = (UINT32)(value & BITS_0_31);
	return lower;
}

#if 0
//Prints a 64-bit number
void print64( UINT64 number)
{
	if (number >= FOUR_GIG) {
		DEBUG ((EFI_D_ERROR, "0x%x%08x", get_upper32(number),get_lower32(number)));
	} else {
		DEBUG ((EFI_D_ERROR, "0x%08x", get_lower32(number)));
	}
}

// Prints a 64-bit number with a newline at the end
void print64n( UINT64 number)
{
	print64( number);
	DEBUG ((EFI_D_ERROR, "\n"));    
}
#endif

void dumpdata( void * start, UINT32 size)
{
	UINT32 count;
	UINT32 * pointer;

	DEBUG ((EFI_D_ERROR, "Dumping 0x%x bytes starting at 0x%08x\n", size, (UINTN*)start ));

	for (count = 0; count < size; count+=4)
	{
		pointer = (UINT32*)((UINT8*)start + count);
		DEBUG ((EFI_D_ERROR, " 0x%08x : 0x%08x\n",pointer, *pointer));
	}
}

void memclear(void * start, UINT32 size)
{
	UINT32 count;
	UINT8 * volatile pointer; //Use volatile to avoid intrinsic memset substitution by compiler
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Clearing 0x%x bytes starting at 0x%08x\n",size,start));
#endif
	for (count = 0; count < size; count++)
	{
		pointer = ((UINT8*)start + count);
		*pointer = 0;
	}
}

void buildIdentifyCommand( struct CommandHeader * CommandHeaderEntry, struct CommandTable * command, UINTN destination)
{
	void *dest = (void *)(UINTN) destination;
	DEBUG ((EFI_D_ERROR, "Building IDENTIFY descriptor\n"));

	memclear(command,sizeof(struct CommandTable));
	memclear(CommandHeaderEntry,sizeof(struct CommandHeader));
	memclear(dest,512);

	command->cfis.FISType = REG_H2D_FIS; //Register H2D FIS
	command->cfis.MiscFlags = 0x80;	// Set "C" Bit
	command->cfis.Command = AHCI_Identify_CMD; // Identify

	command->prdt[0].DBA = (UINT32)(UINTN)dest;
	command->prdt[0].DBAU = 0;
	command->prdt[0].Reserved = 0;
	command->prdt[0].DBC = (512 - 1);  // 0 Based

	CommandHeaderEntry->Misc = 5;
	CommandHeaderEntry->PRDTL = 1;
	CommandHeaderEntry->PRDBC = 0;
	CommandHeaderEntry->CTBA = (UINT32)(UINTN)(command);
//	CommandHeaderEntry->CTBAU = (UINT32)((UINT64)command >> 32);
	CommandHeaderEntry->CTBAU = 0;
}

void buildReadCommand( struct CommandHeader * CommandHeaderEntry, struct CommandTable * command, UINTN destination)
{
	void *dest = (void *)(UINTN) destination;
	DEBUG ((EFI_D_ERROR, "Building Read descriptor\n"));

	memclear(command,sizeof(struct CommandTable));
	memclear(CommandHeaderEntry,sizeof(struct CommandHeader));
	memclear(dest,512);

	command->cfis.FISType = REG_H2D_FIS; //Register H2D FIS
	command->cfis.MiscFlags = 0x80;	// Set "C" Bit
	command->cfis.Command = AHCI_Identify_CMD; // Identify

	command->prdt[0].DBA = (UINT32)(UINTN)dest;
	command->prdt[0].DBAU = 0;
	command->prdt[0].Reserved = 0;
	command->prdt[0].DBC = (512 - 1);  // 0 Based

	CommandHeaderEntry->Misc = 5;
	CommandHeaderEntry->PRDTL = 1;
	CommandHeaderEntry->PRDBC = 0;
	CommandHeaderEntry->CTBA = (UINT32)(UINTN)(command);
//	CommandHeaderEntry->CTBAU = (UINT32)((UINT64)command >> 32);
	CommandHeaderEntry->CTBAU = 0;
}

void buildSetDMACommand( struct CommandHeader * CommandHeaderEntry, struct CommandTable * command, UINT8 DMAMode)
{
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Building Set DMA Mode descriptor, new mode=%d\n",DMAMode));
#endif
	memclear(command,sizeof(struct CommandTable));
	memclear(CommandHeaderEntry,sizeof(struct CommandHeader));

	command->cfis.FISType = REG_H2D_FIS; //Register H2D FIS
	command->cfis.MiscFlags = 0x80;	// Set "C" Bit
	command->cfis.Command = AHCI_SET_FEAT_CMD; // SET FEATURES
	command->cfis.Features = 0x3;
	command->cfis.SectorCount = (0x40 | DMAMode);

	CommandHeaderEntry->Misc = 5;
	CommandHeaderEntry->PRDTL = 0;
	CommandHeaderEntry->PRDBC = 0;
	CommandHeaderEntry->CTBA = (UINT32)(UINTN)command;
	CommandHeaderEntry->CTBAU = 0;
}

void buildDMACommand( struct CommandHeader * CommandHeaderEntry, struct CommandTable * command, UINT64 hostaddr, UINT32 lba, UINT32 sectorcount, UINT32 sectorsize, UINT8 intflag, UINT8 write)
{
	UINT32 bytesleft;
	UINT16 prdtcount;

#ifdef EXTRA_DEBUG
	if (write) {
		DEBUG ((EFI_D_ERROR, "Building DMA Write descriptor\n"));
	} else {
		DEBUG ((EFI_D_ERROR, "Building DMA Read descriptor\n"));
	}
        DEBUG ((EFI_D_ERROR, "[H:%x,T:%x]Host Address=%lx", CommandHeaderEntry, command, hostaddr));
	DEBUG ((EFI_D_ERROR, " LBA=0x%08x sectorcount=%d,sectorsize=%d,intflag=%d\n",lba,sectorcount,sectorsize,intflag));
#endif

        memclear(command,sizeof(struct CommandTable));
        memclear(CommandHeaderEntry,sizeof(struct CommandHeader));

	command->cfis.FISType = 0x27; //Register H2D FIS
	command->cfis.MiscFlags = 0x80;	// Set "C" Bit

	if (write) {
                command->cfis.Command = AHCI_WRITE_DMA; // Write DMA Ext
	} else {
		command->cfis.Command = AHCI_READ_DMA_EXT_CMD; // Read DMA Ext
	}
	// Denverton Aptiov override start
	command->cfis.Features = 0;	// ?
	command->cfis.FeaturesExp = 0; // ?
	command->cfis.LBALow = (lba & 0xFF);
	// command->cfis.LBAMid = ((lba >> 8) & 0xFF);
	// command->cfis.LBAHigh = ((lba >> 16) & 0xFF);
	// command->cfis.LBALowExp = ((lba >> 24) & 0xFF);
	command->cfis.LBAMid = (RShiftU64 (lba, 8 ) & 0xFF);
	command->cfis.LBAHigh = (RShiftU64 (lba, 16 ) & 0xFF);
	command->cfis.LBALowExp = (RShiftU64 (lba, 24 ) & 0xFF);
	command->cfis.LBAMidExp = 0;
	command->cfis.LBAHighExp = 0;
	// command->cfis.Device = (1 << 6) | (1 << 4); 
	command->cfis.Device = (UINT8)(LShiftU64 ( 1, 6 ) | LShiftU64 ( 1, 4 ));
	command->cfis.SectorCount = (sectorcount & 0xFF);
	// command->cfis.SectorCountExp = ((sectorcount >> 8) & 0xFF);
	command->cfis.SectorCountExp = (RShiftU64 (sectorcount, 8 ) & 0xFF);
	command->cfis.Reserved = 0;
	command->cfis.Control = 0;
	// Denverton Aptiov override end
	bytesleft = sectorcount * sectorsize;
	prdtcount = 0;

	while (bytesleft && (prdtcount < MAX_PRDTs))
	{
		command->prdt[prdtcount].DBA = get_lower32(hostaddr);
		command->prdt[prdtcount].DBAU = get_upper32(hostaddr);
		command->prdt[prdtcount].Reserved = 0;
		if (bytesleft > (MAX_PRDT_BYTECOUNT))
		{
			command->prdt[prdtcount].DBC = ((MAX_PRDT_BYTECOUNT) - 1);	// 0 Based
			bytesleft -= MAX_PRDT_BYTECOUNT;
			hostaddr += MAX_PRDT_BYTECOUNT;
		} else
		{
			command->prdt[prdtcount].DBC = (bytesleft - 1);	 // 0 Based
			bytesleft = 0;
		}
#ifdef EXTRA_DEBUG
		DEBUG((EFI_D_ERROR,"    PRDT %d: DBA=0x%x, DBAU=0x%x, DBC=0x%x\n",prdtcount,command->prdt[prdtcount].DBA,command->prdt[prdtcount].DBAU,command->prdt[prdtcount].DBC));
#endif
		prdtcount++;
	}
        ASSERT((prdtcount <= MAX_PRDTs) && (0 == bytesleft));

	if (intflag && (prdtcount > 0))
    {
		command->prdt[prdtcount-1].DBC |= (1 << 31);
	}

	CommandHeaderEntry->Misc = (write << 6) | 5;
	CommandHeaderEntry->PRDTL = prdtcount;
	CommandHeaderEntry->PRDBC = 0;
	CommandHeaderEntry->CTBA = (UINT32)(UINTN)command;
//	CommandHeaderEntry->CTBAU = (UINT32)((UINT64)command >> 32);
	CommandHeaderEntry->CTBAU = 0;
}


/**
  ACHI Interrupt Handler

  @param[in] InterruptType        The type of interrupt that occured
  @param[in] SystemContext        A pointer to the system context when the interrupt occured

  @retval None
**/
VOID
EFIAPI
AhciInterruptHandler (
//  IN EFI_EXCEPTION_TYPE   InterruptType,
//  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
	INTERRUPT_BLOCK* volatile IntBlock = (INTERRUPT_BLOCK*)(UINTN)MONITOR_LOC;
	UINT32 Data32;

	IoWrite8(DEBUG_PORT, IntBlock->PortVal++);
	// Clear the PxIS register
	Data32  = MmioRead32 (IntBlock->abar + AHCI_ABAR_PxIS + (IntBlock->SataPort * PORT_SPEC_REGS_SIZE));
	MmioWrite32 ((IntBlock->abar + AHCI_ABAR_PxIS + (IntBlock->SataPort * PORT_SPEC_REGS_SIZE)), Data32);

	// Clear the Master IS register
	Data32  = MmioRead32 (IntBlock->abar + AHCI_ABAR_IS_MASTER);
	MmioWrite32 ((IntBlock->abar + AHCI_ABAR_IS_MASTER), Data32);

	// Write the EOI register in the local APIC
	Data32  = MmioRead32 (APIC_BASE + APIC_EOI);
	MmioWrite32 ((APIC_BASE + APIC_EOI), Data32);

	IntBlock->monitorloc = 1;
	IoWrite8(DEBUG_PORT, IntBlock->PortVal++);
	IntBlock->monitorloc = 1; //signal that the AHCI controller is done with the last transfer
}

void sipiroutine(void)
{
	while (1)
	{
		_mm_monitor(SIPI_MONITOR_LOC,0,0);
		_mm_mwait(0x0,0x1);
	}
}

UINT32
GetApicID (
  VOID
)
{
  EFI_CPUID_REGISTER  CpuidRegisters;

  //dnvtodo - use this EfiCpuid(); instead of AsmCpuid();
  AsmCpuid (EFI_CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  // Denverton Aptiov override start
  // return (UINT32)(CpuidRegisters.RegEbx >> 24);
  return (UINT32)(RShiftU64 (CpuidRegisters.RegEbx, 24));
  // Denverton Aptiov override end
}
void DisableMSI(UINTN SataMmioSpace)
{
	volatile UINT8 *pointer8;
	volatile UINT16 *pointer16;
	volatile UINT32 *pointer32;
	UINT16 MsgCtrl = 0;
	UINT8 CapPtr;

	// Uninstall the interrupt handler
	//mCpu->RegisterInterruptHandler (mCpu, VECTOR_NUM, NULL);
	//dnvtodo - move registering of handler to C2F PEIM driver (outside of AHCI library)

	pointer8 = (UINT8*)(UINTN)(SataMmioSpace | AHCI_DEV_CAP);
	CapPtr = *pointer8;

	// Disable MSI
	DEBUG((EFI_D_ERROR, "Clear Generation of MSI\n"));
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | CapPtr + 2);
	*pointer16 = 0;

	// Clear MSI msg register
	DEBUG((EFI_D_ERROR, "Clear MSI Message Register\n"));
	pointer32 = (UINT32*)(UINTN)(SataMmioSpace | CapPtr + 4);
	*pointer32 = 0;

	// Clear MSI Data register
	DEBUG((EFI_D_ERROR, "Clear MSI Data Register\n"));
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | CapPtr + 8);
	*pointer16 = 0;


	// Enable PCI Interrupt and disable bus master
	DEBUG((EFI_D_ERROR, "Enable PCI Interrupts and disable bus master\n"));
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | 04);
	MsgCtrl = *pointer16;
	MsgCtrl &= ~(1 << 10 | 1 << 2) ; // Interrupt Enable. Bus master disable
	*pointer16 = MsgCtrl;

}

UINT32 EnableMSI(UINTN SataMmioSpace, UINT16 VectorNum)
{
	volatile UINT16 *pointer16;
	volatile UINT8 *pointer8;
	volatile UINT32 *pointer32;

	UINT8 CapPtr;
	UINT32 MsgAdr = APIC_BASE;
	UINT16 MsgData = 0;
	UINT16 MsgCtrl = 0;
	
	// 1.Read the capabilities List Pointer to find the location of the first capabilities structure
	pointer8 = (UINT8*)(UINTN)(SataMmioSpace | AHCI_DEV_CAP);
	CapPtr = *pointer8;

	DEBUG((EFI_D_ERROR, "Read Capabilities list Pointer addr = 0x%x, value = 0x%x\n", pointer8, CapPtr));
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | CapPtr);
	if(((*pointer16) & 0xFF) != MSI_CAPABILITIES_ID) // Not MSI Capable
	{
		DEBUG((EFI_D_ERROR, "Device not MSI capable\n"));
		return 0;
	}
	MsgAdr |= ((UINT8)GetApicID()) << 12;
	DEBUG((EFI_D_ERROR, "Destination ID = 0x%x\n", MsgAdr));
	
	pointer32 = (UINT32*)(UINTN)(SataMmioSpace | CapPtr + 4);
	*pointer32 = MsgAdr;
	DEBUG((EFI_D_ERROR, "Program MsgAdr Register = 0x%x, Value = 0x%x\n", pointer32, MsgAdr));

	MsgData |= VectorNum; // All other fields are zero, fixed mode, edge triggered
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | CapPtr + 8);
	*pointer16 = MsgData;
	DEBUG((EFI_D_ERROR, "Program MsgData Register = 0x%x, Value = 0x%x\n", pointer16, MsgData));
	
	// Enable MSI enable bit
	//
	DEBUG((EFI_D_ERROR, "Enable MSI\n"));
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | CapPtr + 2);
	MsgCtrl = *pointer16;
	MsgCtrl |= 1;
	*pointer16 = MsgCtrl;

	DEBUG((EFI_D_ERROR, "Disable Interrupts in PCI Command register\n"));
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | 04);
	MsgCtrl = *pointer16;
	MsgCtrl |= 1 << 10 ; // Interrupt Disable. Bus master enable
	DEBUG((EFI_D_ERROR, "Addr = 0x%x, Value = 0x%x\n", pointer16, MsgCtrl));
	*pointer16 = MsgCtrl;
	MsgCtrl = *pointer16;
	MsgCtrl |= 1 << 2;
	*pointer16 = MsgCtrl;
	DEBUG((EFI_D_ERROR, "Readback Value = 0x%x\n", *pointer16));
	return 1;
}

VOID AhciCleanup(UINTN SataMmioSpace, UINT32 abar, UINT8 SataPort)
{
	volatile UINT32 *pointer32;
	
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCLB + (SataPort * 0x80));
	*pointer32 = 0;
	
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCLBU + (SataPort * 0x80));
	*pointer32 = 0;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxFB + (SataPort * 0x80));
	*pointer32 = 0;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxFBU + (SataPort * 0x80));
	*pointer32 = 0;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_GHC);
	*pointer32 = 1; // Reset the controller

	pointer32 = (UINT32*)(UINTN)(SataMmioSpace | AHCI_REG_ABAR);
	*pointer32 = 0;	// Clear ABAR
}




// Returns Pointer to abar
UINT32 configureAHCI(UINTN SataMmioSpace, UINT8 SataPort)
{
	volatile UINT16 * pointer16;
	volatile UINT32 * pointer32;
	UINT16 data16;
	UINT32 data32;
	UINT32 capregister;
	UINT32 abar;

#ifdef EXTRA_DEBUG
	DEBUG ((DEBUG_INFO, "%a()\n",__FUNCTION__));
	DEBUG ((DEBUG_INFO, "using %x - VID/DID %x\n", SataMmioSpace, *(UINT32*)(UINTN)(SataMmioSpace)));
#endif
	//Verify that this devices is a storage device
	data16 = *(UINT16*)(UINTN)(SataMmioSpace | AHCI_REG_CC);
	if ( (data16 == 0xffff) || ((data16>>AHCI_REG_CC_BC_POS) != 1) ) {
		DEBUG ((EFI_D_ERROR, "Mass storage device not found at MMIO 0x%x Exiting\n", SataMmioSpace));
		return 0;
	}

	// Program ABAR
	pointer32 = (UINT32*)(UINTN)(SataMmioSpace | AHCI_REG_ABAR);
	abar = *pointer32;
	if ((abar != 0xffffffff) && (abar > 0x1))
	{
		DEBUG ((EFI_D_ERROR, "ABAR already programmed to 0x%08x\n", abar));
	} else
	{
		abar = ABAR_LOC;
		*pointer32 = abar;
		DEBUG ((EFI_D_ERROR, "ABAR not programmed yet so programmed to 0x%08x\n", abar));
	}

	// Set the PCI Command register
	pointer16 = (UINT16*)(UINTN)(SataMmioSpace | AHCI_REG_PCICMD);
	*pointer16 |= 0x6;

  //
	// Enable SATA ports in PCI config space
  // copy the port present bits (PxP) to the port enable bits (PxE)
  //
  DEBUG ((EFI_D_ERROR, "PCS:0x%x\n",*(UINT32_REG)(SataMmioSpace | AHCI_REG_PCS)));
  pointer32 = (UINT32_REG)(SataMmioSpace | AHCI_REG_PCS);
  data32 = *pointer32;
  data32 &= 0x3F0000;
  data32 = data32 >> 16;
  data16 = data32 & 0xff; //save PCS.PxE bits to set ABAR.PI
  *(UINT16_REG)pointer32 = data16; // set just PxE bits in PCS
  DEBUG ((EFI_D_ERROR, "PCS:0x%x\n",*(UINT32_REG)(SataMmioSpace | AHCI_REG_PCS)));

	//Enable ports in AHCI space
  pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PI);
  data32 = *pointer32; // required fencing around PI access 1 - read before
  *pointer32 = data16;
  spinloop(100000);
  data32 += *pointer32; // required fencing 2 - read after
  spinloop(100000);
  data32 += *pointer32; // required fencing 3 - read after
  spinloop(100000);
	DEBUG ((EFI_D_ERROR, "ABAR.PI:0x%x\n",*pointer32));

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_CAP);
	capregister = *pointer32;
	capregister &= ~(AHCI_ABAR_CAP_SIS | AHCI_ABAR_CAP_SSS | AHCI_ABAR_CAP_SALP | 
					 AHCI_ABAR_CAP_PMS | AHCI_ABAR_CAP_SSC | AHCI_ABAR_CAP_PSC |
					 AHCI_ABAR_CAP_SXS | AHCI_ABAR_CAP_ISS_MASK);

	capregister |= (AHCI_ABAR_CAP_SCLO | AHCI_ABAR_CAP_PMD | (AHCI_ABAR_CAP_ISS_3_0_G << AHCI_ABAR_CAP_ISS));
	pointer32    = (UINT32*)(UINTN)(abar + AHCI_ABAR_CAP);
	*pointer32   = capregister;
	DEBUG ((EFI_D_ERROR, "CAP:0x%x\n",*pointer32));


	// Clear CAP2 Bits
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_CAP2);
	data32 = *pointer32;
	data32 |= (1 << AHCI_CAP2_APST_POS);
	*pointer32 = data32;
	DEBUG ((EFI_D_ERROR, "CAP2:0x%x\n",*pointer32));

	pointer32 = (UINT32*)(UINTN)(SataMmioSpace | AHCI_REG_PCS);
	data32 = *pointer32;
	if (data32 & (1 << (SataPort+16)))
	{
		DEBUG ((EFI_D_ERROR, "Device present on port %d, PCS=0x%x\n",SataPort,data32));
	} else
	{
		DEBUG ((EFI_D_ERROR, "No Device present on port %d, PCS=0x%x\n",SataPort,data32));
		DEBUG ((EFI_D_ERROR, "Exiting\n"));
		return 0;
	}

	// Step 5 Clear any previous pending interrupts
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxIS + (SataPort * 0x80));
	data32 = *pointer32;
	*pointer32 = data32;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_IS_MASTER);
	data32 = *pointer32;
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Master IS = 0x%08x\n",data32));
#endif
	*pointer32 = data32;

	// Step 6: Set GHC.31 to 1(Section 14.4.1.2 of Ibex EDS) and enable global interrupt flag
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_GHC);
	*pointer32 |= (1 << AHCI_ABAR_GHC_AE_POS) | ( 1<< AHCI_ABAR_GHC_IE_POS);


	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
	// APSTE set to 0 or CAP2.APST
	data32 = (1 << AHCI_PXCMD_APSTE_POS);
	*pointer32 = data32;

	//
	// Start drive connection if needed
	//
        pointer32 = (UINT32*)(UINTN)(UINTN)(abar + AHCI_ABAR_PxSSTS + (SataPort * 0x80));
	DEBUG ((EFI_D_ERROR, " PxSSTS:0x%x\n",*pointer32));
	if (PxSSTS_DET_DEVICE_COMM_STARTED != (*pointer32 & PxSSTS_DET_MASK)) {
	  StartDriveComms(abar, SataPort);
	}
	return abar;
}

void StartDriveComms(UINT32 abar, UINT8 SataPort){
        volatile UINT32 * pointer32;
        UINT32 data32;

        DEBUG ((EFI_D_ERROR, " PxTFD:0x%x\n",*(UINT32*)(UINTN)(abar + AHCI_ABAR_PxTFD + (SataPort * 0x80))));
        pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
        data32 = *pointer32;
        if (!(data32 | (1<<AHCI_PXCMD_POD_POS))) {
                *pointer32 |= (1<<AHCI_PXCMD_POD_POS);
                data32 = *pointer32;
                DEBUG ((EFI_D_ERROR, "set POD - P:%x PxCMD:0x%x\n", SataPort, *pointer32));
        }

        if (!(data32 | (1<<AHCI_PXCMD_SUD_POS))) {
                *pointer32 |= (1<<AHCI_PXCMD_SUD_POS);
                data32 = *pointer32;
                DEBUG ((EFI_D_ERROR, "set SUD - PxCMD:0x%x\n", *pointer32));

        }

        pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxSCTL + (SataPort * 0x80));
        if (PxSSTS_DET_DEVICE_COMM_STARTED != (*pointer32 & PxSSTS_DET_MASK)) {
                DEBUG ((EFI_D_ERROR, "DET - PxSCTL:0x%x\n",*pointer32));
                data32 = *pointer32;
                *pointer32 = (data32 & ~(UINT32)(0xf<<AHCI_PXSCTL_DET_POS)) | (UINT32)(1<<AHCI_PXSCTL_DET_POS);
                *pointer32 = (*pointer32 & ~(UINT32)(0xf<<AHCI_PXSCTL_DET_POS));
                DEBUG ((EFI_D_ERROR, "set/clear DET - PxSCTL:0x%x\n",*pointer32));
        }


        pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxSSTS + (SataPort * 0x80));
        DEBUG ((EFI_D_ERROR, " PxSSTS:0x%x\n",*pointer32));
        if (0 == *pointer32) {
                DEBUG ((EFI_D_ERROR, "Device communications not established PxSSTS:0x%x\n",*pointer32));
                //return 0;
        }
}

UINT32 runCommands( UINT32 abar, UINT8 SataPort, struct CommandHeader * CommandHeaderEntry, struct ReceivedFIS * ReceivedFISArea, UINT32 commandMask, C2F_XFER_METHOD Method)
{
	volatile UINT32 * pointer32;
	volatile UINT64 * pointer64;
	UINT32 data32;
	UINT8 lastcommand;
	volatile int a = 5;
	INTERRUPT_BLOCK* volatile IntBlock = (INTERRUPT_BLOCK*)(UINTN)MONITOR_LOC;

	UINT8 i;

	IntBlock->monitorloc = 0;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
	data32 = *pointer32;

	if (data32 & ( 1 << 15))
	{
		DEBUG ((EFI_D_ERROR, "Waiting for CMD.CR to be clear\n"));
		while ((*pointer32 & (1 << 15)));
	}

	if (data32 & ( 1 << 14))
	{
		DEBUG ((EFI_D_ERROR, "Waiting for CMD.FR to clear\n"));
		while ((*pointer32 & (1 << 14)));
	}

	// Set Command Base Address
	pointer64 = (UINT64*)(UINTN)(abar + AHCI_ABAR_PxCLB + (SataPort * 0x80));
	*pointer64 = (UINT64)CommandHeaderEntry;

	// Set Received FIS Base Address
	pointer64 = (UINT64*)(UINTN)(abar + AHCI_ABAR_PxFB + (SataPort * 0x80));
	*pointer64 = (UINT64)ReceivedFISArea;


	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxSERR + (SataPort * 0x80));
	data32 = *pointer32;
	if (data32)
	{
		DEBUG ((EFI_D_ERROR, "Clearing errors in SERR Regsiter(0x%08x)\n",data32));
		*pointer32 = data32;
		data32 = *pointer32;
		DEBUG ((EFI_D_ERROR, "SERR Register After Error Clearing = 0x%08x\n",data32));
	}


	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxIS + (SataPort * 0x80));
	data32 = *pointer32;
	if (data32)
	{
		DEBUG ((EFI_D_ERROR, "Clearing errors in PxIS Regsiter(0x%08x)\n",data32));
		*pointer32 = data32;
		data32 = *pointer32;
		DEBUG ((EFI_D_ERROR, "SERR Register After Error Clearing = 0x%08x\n",data32));
	}


    if (C2F_INTERRUPT == Method) {
    	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxIE + (SataPort * 0x80));
    	*pointer32 |= (1 << 5);
#ifdef EXTRA_DEBUG
	   DEBUG ((EFI_D_ERROR, "Setting PxIE.DPE to 1 %x:0x%x\n", pointer32, *pointer32));
#endif
    }

#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Setting CMD.FRE to 1\n"));
#endif
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
	*pointer32 |= (1 << 4);

#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Waiting for CMD.FR to set\n"));
#endif
	while (!(*pointer32 & (1 << 14)));


	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxTFD + (SataPort * 0x80));
	data32 = *pointer32;
	if (data32 & ((1 << 7) | (1 << 3)))
	{
		DEBUG ((EFI_D_ERROR, "TFD.STS.BSY or TFD.STS.DRQ is set to 1, setting CLO(PxTFD=0x%x)\n", data32));
		pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
		DEBUG ((EFI_D_ERROR, " PxCMD=0x%x\n", *pointer32));
		*pointer32 |= (1<<3);
#ifdef EXTRA_DEBUG
		DEBUG ((EFI_D_ERROR, "Waiting for CMD.CLO to clear\n"));
#endif
		while ((*pointer32 & (1 << 3)));
	}

#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Setting CMD.ST to 1\n"));
#endif
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
	*pointer32 |= 1;

DEBUG ((EFI_D_ERROR, " PxSSTS%d:0x%x\n",SataPort,*(UINT32*)(UINTN)(abar + AHCI_ABAR_PxSSTS + (SataPort * 0x80))));

	if (C2F_INTERRUPT == Method) {
		IntBlock->monitorloc = 0;
	}
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Setting CI to 0x%08x\n",commandMask));
#endif
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCI + (SataPort * 0x80));
	*pointer32 = commandMask;


	lastcommand = 0;
	for (i = 0; i < 32; i++)
	{
		if (commandMask & (1 << i))
			lastcommand=i;
	}
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Last Command=%d\n",lastcommand));
#endif

	do {
		if(a == 1) {
			DEBUG ((EFI_D_ERROR, "Looping...\n"));
			a = 0;
		}
	} while ( a == 0);

#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Waiting for PxCI to clear\n"));
#endif

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCI + (SataPort * 0x80));
	while (*pointer32 != 0)
	{
		if (C2F_INTERRUPT == Method) {
			if (lastcommand > 0)
			{
				_mm_monitor(&IntBlock->monitorloc,0,0);
//				while (IntBlock->monitorloc == 0)
//				{
					DEBUG ((EFI_D_ERROR, "About to go into c-state(hint=0x%x), monitorloc=0x%08x, PxCI=0x%08x\n",MWAIT_HINT,IntBlock->monitorloc, *pointer32));
					EnableInterrupts();
					_mm_mwait(0x1,MWAIT_HINT);
					DisableInterrupts();
					DEBUG ((EFI_D_ERROR, "Woke up from c-state, monitorloc=0x%08x\n",IntBlock->monitorloc));
//				}
			}
		} else {
			AsmPause();
			spinloop(1000000);
		}
#ifdef EXTRA_DEBUG
		DEBUG ((EFI_D_ERROR, "PxCI=0x%08x ",*pointer32));
		pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxTFD + (SataPort * 0x80));
		DEBUG ((EFI_D_ERROR, "PxTFD=0x%x\n",*pointer32));
		pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCI + (SataPort * 0x80));
#endif
	}

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxTFD + (SataPort * 0x80));
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "TFD=0x%x\n",*pointer32));
#endif

	// Clear the Start and FIS recieve bits
	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
	*pointer32 &= ~((1 << 4) | (1 << 0));

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxIS + (SataPort * 0x80));
	data32 = *pointer32;
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "PxIS=0x%x\n",data32));    
#endif
	*pointer32 = data32;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxSERR + (SataPort * 0x80));
	data32 = *pointer32;
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "SERR Register=0x%08x\n",data32));
#endif
	*pointer32 = data32;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_IS_MASTER);
	data32 = *pointer32;
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "Master IS=0x%x\n",data32));
#endif
	*pointer32 = data32;

	pointer32 = (UINT32*)(UINTN)(abar + AHCI_ABAR_PxCMD + (SataPort * 0x80));
	data32 = *pointer32;
	data32 &= ~((1<<0) | (1<<4));
	*pointer32 = data32;

	return 0;
}


void transferData( UINT32 abar, UINT8 SataPort, UINT64 hostaddr, UINT64 bytes, UINT32 lba, UINT32 sectorsize, UINT8 write, C2F_XFER_METHOD Method)
{
	UINT32 commandmask;
	UINT32 commandnum;
	UINT64 bytesleft;
	UINT32 bytestransfered;
	UINT32 bytesqueued;

	struct CommandHeader * CommandHeaderList  = COMMAND_HEADER_LIST;
	struct CommandTable * CommandList = COMMAND_LIST;   

	int i;

	if (write) {
		DEBUG((EFI_D_ERROR, "Writing 0x%llx bytes from 0x%llx to lba 0x%x\n", bytes, hostaddr, lba));
	} else {
		DEBUG((EFI_D_ERROR, "Reading 0x%llx bytes from lba 0x%x to 0x%llx\n", bytes, lba, hostaddr));
        }
	bytesleft = bytes;
	while (bytesleft)
	{
		DEBUG((EFI_D_ERROR, "Starting New Command Round hostaddr=0x%llx\n",hostaddr));
		ZeroMem(COMMAND_HEADER_LIST,sizeof(struct CommandHeader) * COMMAND_SLOTS);
		commandmask = 0;
		bytesqueued = 0;
		for (commandnum = 0; commandnum < COMMAND_SLOTS; commandnum++)
		{
			if (bytesleft > MAX_COMMAND_BYTECOUNT)
			{
				buildDMACommand( &CommandHeaderList[commandnum], &CommandList[commandnum], hostaddr, lba, MAX_SECTOR_COUNT, sectorsize,(commandnum == (COMMAND_SLOTS - 1)), write);
				commandmask |= (1 << commandnum);
				hostaddr += MAX_COMMAND_BYTECOUNT;
				bytesleft -= MAX_COMMAND_BYTECOUNT;
				bytesqueued += MAX_COMMAND_BYTECOUNT;
				lba += MAX_SECTOR_COUNT;
			} else
			{
				buildDMACommand( &CommandHeaderList[commandnum], &CommandList[commandnum], hostaddr, lba, ((UINT32)(bytesleft)/sectorsize), sectorsize, 1, write);
				commandmask |= (1 << commandnum);
				hostaddr += MAX_COMMAND_BYTECOUNT;
				lba += (((UINT32)bytesleft)/sectorsize);
				bytesqueued += (UINT32)bytesleft;
				bytesleft = 0;
				break;
			}
		}
		if (runCommands( abar, SataPort, &CommandHeaderList[0], RECEIVED_FIS_AREA, commandmask, Method))
		{
			DEBUG ((EFI_D_ERROR, "Error Running Command\n"));
			return;
		}

		bytestransfered = 0;

		for (i = 0; i < COMMAND_SLOTS; i++) {
			bytestransfered += CommandHeaderList[i].PRDBC;
		}

		if (bytestransfered != bytesqueued)
		{
			DEBUG ((EFI_D_ERROR, "ERROR, not all bytes transfered\n"));
			for (i=0; i < COMMAND_SLOTS; i++) {
				DEBUG ((EFI_D_ERROR, "Command[%d] Bytes Transfered=0x%08x\n", i, CommandHeaderList[i].PRDBC));
			}
			while (1);

		} else
		{
			DEBUG ((EFI_D_ERROR, "0x%x bytes successfully transfered\n",bytestransfered));
		}
	}
#ifdef EXTRA_DEBUG
	DEBUG ((EFI_D_ERROR, "DMA Command(s) Complete\n"));
#endif
}

EFI_STATUS find_storage_patition( C2F_STORAGE_DEVICE *StorageDevice, UINT32* numberofsectors, UINT32 sectorsize, UINT32 abar )
{
	EFI_PARTITION_TABLE_HEADER *PartitionHeader = (EFI_PARTITION_TABLE_HEADER *)(SCRATCH_PAD_AREA);
	EFI_PARTITION_ENTRY        *PartitionEntry = (EFI_PARTITION_ENTRY *)(SCRATCH_PAD_AREA + sectorsize);
	UINT32                      CurrentLba;
	UINT32                      entry = 0;
	UINT32                      EntryInSector = 0;

	// Read the drive partition table at sector 1 verifying it is EFI
	CurrentLba = 1;
	transferData( abar, StorageDevice->SataPort, (UINT64)PartitionHeader, sectorsize, CurrentLba, sectorsize, C2F_FLASH_TO_RAM, C2F_POLLED /*StorageDevice->Method*/);
	DEBUG ((EFI_D_ERROR, "Header {Signature:%lx  Revision:%x HeaderSize:%x CRC32:%x}\n", PartitionHeader->Header.Signature, PartitionHeader->Header.Revision, PartitionHeader->Header.HeaderSize, PartitionHeader->Header.CRC32));
	DEBUG ((EFI_D_ERROR, "MyLBA:%lx AlternateLBA:%lx FirstUsableLBA:%x\n", PartitionHeader->MyLBA, PartitionHeader->AlternateLBA, PartitionHeader->FirstUsableLBA));
	DEBUG ((EFI_D_ERROR, "PartitionEntryLBA:%lx NumberOfPartitionEntries:%x SizeOfPartitionEntry:%x\n", PartitionHeader->PartitionEntryLBA, PartitionHeader->NumberOfPartitionEntries, PartitionHeader->SizeOfPartitionEntry));

    if ((PartitionHeader->Header.Signature != EFI_PTAB_HEADER_ID) || (PartitionHeader->MyLBA != CurrentLba)) {
    	DEBUG ((DEBUG_ERROR, "Bad GPT: signature error\n"));
		return EFI_NOT_FOUND;
    }
    // Partition CRC validation should be done but will be left for a later date

    ASSERT(PartitionHeader->SizeOfPartitionEntry < sectorsize);

    //
	// Walk the partition list looking for the partition with C2F or the correct UUID
    // read two sectors of data at a time and check each entry in that data
    // if an entry starts in the second sector the second sector will be reread plus one more
    //
	CurrentLba = 0;
    do {
    	UINT32  EntryStart = (PartitionHeader->SizeOfPartitionEntry * entry);
    	UINT32  EntryStartLba = (UINT32)PartitionHeader->PartitionEntryLBA + (EntryStart / sectorsize);
    	UINT32  EntryEndLba = (UINT32)PartitionHeader->PartitionEntryLBA + ((EntryStart + PartitionHeader->SizeOfPartitionEntry) / sectorsize);

    	DEBUG ((EFI_D_ERROR, "EntryStartLba:%x EntryEndLba:%x CurrentLba:%x\n", EntryStartLba, EntryEndLba, CurrentLba));
    	if ( ((CurrentLba+1) < EntryEndLba) || (CurrentLba > EntryStartLba) ) {
    		CurrentLba = EntryStartLba;
    		transferData( abar, StorageDevice->SataPort, (UINT64)PartitionEntry, (sectorsize*2), CurrentLba, sectorsize, C2F_FLASH_TO_RAM, C2F_POLLED /*StorageDevice->Method*/);
    		PartitionEntry = (EFI_PARTITION_ENTRY *)(SCRATCH_PAD_AREA + sectorsize + (EntryStart % sectorsize));
    		EntryInSector = 0;
    	}

    	//
    	// C2F partition has one of C2F_GUID in the partition or partition type fields or a name that starts with a defined string
    	//
    	DEBUG ((EFI_D_ERROR, "LBA:%x PartitionEntry[%x,s:%x]:%x  ", CurrentLba, entry, EntryInSector, &PartitionEntry[EntryInSector]));
    	if ( (0 == CompareGuid( &PartitionEntry[EntryInSector].UniquePartitionGUID, &C2F_PARTITION_GUID))
    		 || (0 == CompareGuid( &PartitionEntry[EntryInSector].PartitionTypeGUID, &C2F_PARTITION_GUID))
			 || (0 == StrnCmp( PartitionEntry[EntryInSector].PartitionName, C2F_PARTITION_NAME, C2F_PARTITION_NAME_LENGTH))
    			) {
           DEBUG ((EFI_D_ERROR, "is C2F partition\n"));

           //
           // These 'if's are always matching the first partition. Denverton-Minnehaha droped C2F so this code is no longer needed for DNV
           // It is left here to be further debugged when a plaform needs to share a drive between OS and C2F.
           // Remove'return EFI_NOT_FOUND;' to restore original code flow
           //
   		   return EFI_NOT_FOUND;

           break;
    	}
    	DEBUG ((EFI_D_ERROR, "\n"));

    	entry++;
    	EntryInSector++;
    } while (entry < PartitionHeader->NumberOfPartitionEntries);

	if (entry < PartitionHeader->NumberOfPartitionEntries) {
		StorageDevice->LBA = (UINT32)PartitionEntry[EntryInSector].StartingLBA;
		*numberofsectors = (UINT32)(PartitionEntry[EntryInSector].EndingLBA - PartitionEntry[EntryInSector].StartingLBA);
	} else {
		return EFI_NOT_FOUND;
	}


	return EFI_SUCCESS;
}

EFI_STATUS find_storage_location( C2F_MEMORY_REGION *MemoryRange, C2F_STORAGE_DEVICE *StorageDevice, UINT32* sectorsize, UINT32 abar )
{
	volatile UINT32 * pointer32=0;
	UINT16 * dest = (UINT16*)SCRATCH_PAD_AREA;
	UINT32 sectorcount, capacity;
	UINT32 testmbytes;

	buildIdentifyCommand( COMMAND_HEADER_LIST, COMMAND_LIST, SCRATCH_PAD_AREA);
	DEBUG ((EFI_D_ERROR, "About to issue the Identify command\n"));
	if (runCommands( abar, StorageDevice->SataPort, COMMAND_HEADER_LIST, RECEIVED_FIS_AREA, 1, StorageDevice->Method))
	{
		DEBUG((EFI_D_ERROR, "Error Running Command\n"));
		return (UINT32)-1;
	}

	//
	// determine the sector size of the device
	//
	pointer32 = (UINT32*)&dest[100];
	sectorcount= *pointer32;
	if (dest[106] & (1 << 12))
	{
		pointer32 = (UINT32*)&dest[117];
		*sectorsize = 2 * (*pointer32);
		DEBUG ((EFI_D_ERROR, "[117]Logical Sector Size=%d\n",sectorsize));
	} else
	{
		DEBUG ((EFI_D_ERROR, "Word 106.12 not set, assuming 512 Byte sector\n"));
		*sectorsize = 512;
	}

	buildSetDMACommand( COMMAND_HEADER_LIST, COMMAND_LIST, 6);
	if (runCommands( abar, StorageDevice->SataPort, COMMAND_HEADER_LIST, RECEIVED_FIS_AREA, 1, StorageDevice->Method))
	{
	    return (UINT32)-1;
	}

	//
	// Verify the number of sectors available to use is large enough
	//
	if (StorageDevice->LbaLoc) {
		#if 0
		if (-1 == find_storage_patition(StorageDevice, &sectorcount, *sectorsize, abar)) {
			return (UINT32)-1;
		}
		#else
		StorageDevice->LBA = 0; //just use LBA 0 until the partition code is needed and fixed
		sectorcount -= StorageDevice->LBA;
		#endif
	} else {
		sectorcount -= StorageDevice->LBA;
	}
	capacity = (sectorcount) / (1024 / *sectorsize) / 1024; // FIXE ME: Broken for >1KB sectors
	DEBUG ((EFI_D_ERROR, "Drive Capacity = %dMB\n", capacity));

	DEBUG ((EFI_D_ERROR, "MemoryRange->highsize = 0x%lx\n",MemoryRange->HighSize));
	DEBUG ((EFI_D_ERROR, "testmbytes = (0x%08x>>20) + ((0x%08x << 12) + (0x%08x>>20)\n",MemoryRange->LowSize, get_upper32(MemoryRange->HighSize), get_lower32(MemoryRange->HighSize)));
	testmbytes = (MemoryRange->LowSize >> 20) + ((get_upper32(MemoryRange->HighSize) << 12) + (get_lower32(MemoryRange->HighSize) >> 20));
	DEBUG ((EFI_D_ERROR, "Total transfer size = %dMB or 0x%08x\n",(UINT32)testmbytes));
	if (capacity < testmbytes)
	{
		DEBUG ((EFI_D_ERROR, "FATAL ERROR: Storage Device smaller than test size(%dMB)\n",testmbytes));
		return (UINT32)-1;
	}

	return 0;
}


EFI_STATUS c2f_entry( C2F_MEMORY_REGION *MemoryRange, C2F_XFER_DIRECTION Direction, C2F_STORAGE_DEVICE *StorageDevice )
{
	UINT32 abar;
	UINT32 sectorsize;

	DEBUG ((EFI_D_ERROR, "About to initialize AHCI\n"));
	abar = configureAHCI(StorageDevice->AhciMmio, StorageDevice->SataPort);
	if (abar == 0)
	{
			return(UINT32)-1;
	}

	if (C2F_INTERRUPT == StorageDevice->Method) {
		INTERRUPT_BLOCK* volatile IntBlock = (INTERRUPT_BLOCK*)(UINTN)MONITOR_LOC;
		IntBlock->monitorloc = 0;
		IntBlock->abar = abar;
		IntBlock->SataPort = StorageDevice->SataPort;
		IntBlock->PortVal = 0x50;
		DEBUG ((EFI_D_ERROR, "About to Setup MSI\n"));
		if(!EnableMSI(StorageDevice->AhciMmio, StorageDevice->Vector))
		{
				return(UINT32)-1;
		}
	}

	ZeroMem(COMMAND_HEADER_LIST, sizeof(struct CommandHeader)*COMMAND_SLOTS);
	ZeroMem(RECEIVED_FIS_AREA, sizeof(struct ReceivedFIS));

	if (-1 == find_storage_location(MemoryRange, StorageDevice, &sectorsize, abar))
	{
		return (UINT32)-1;
	}

	if (MemoryRange->LowSize > 0)
	{
		DEBUG ((EFI_D_ERROR, "Starting lower memory copy\n"));
		transferData( abar, StorageDevice->SataPort, MemoryRange->LowStart, MemoryRange->LowSize, StorageDevice->LBA, sectorsize, Direction, StorageDevice->Method);
		StorageDevice->LBA = StorageDevice->LBA + (MemoryRange->LowSize/sectorsize);
		DEBUG ((EFI_D_ERROR, "Finished lower memory copy\n"));
	} else
	{
		DEBUG ((EFI_D_ERROR, "No Low Zone\n"));
	}

	if (MemoryRange->HighSize > 0)
	{
		DEBUG ((EFI_D_ERROR, "Starting high memory copy\n"));
		transferData( abar, StorageDevice->SataPort, MemoryRange->HighStart, MemoryRange->HighSize, StorageDevice->LBA, sectorsize, Direction, StorageDevice->Method);
		DEBUG ((EFI_D_ERROR, "Finished high memory copy\n"));
	} else
	{
		DEBUG ((EFI_D_ERROR, "No High Zone\n"));
	}

	if (C2F_INTERRUPT == StorageDevice->Method) {
		DisableMSI(StorageDevice->AhciMmio);
	}
	AhciCleanup(StorageDevice->AhciMmio, abar, StorageDevice->SataPort);


	return EFI_SUCCESS;
}



