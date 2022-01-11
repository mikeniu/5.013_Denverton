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
#ifndef __C2FAHCI_H__
#define __C2FAHCI_H__

#include <Library\C2fAhciLib.h>

#define EXTRA_DEBUG 1

// Constant Defines
#define AHCI_DEV_CAP	0x34

#define AHCI_REG_PCICMD		0x04
#define AHCI_REG_CC   		0x0A
#define AHCI_REG_ABAR		0x24
#define AHCI_REG_MAP		0x90
#define AHCI_REG_PCS		0x94
#define AHCI_REG_SMS_AHCIMODE	(1 << 6)
#define AHCI_REG_SCLKCG		0x94
#define AHCI_REG_TM2	0x98
#define AHCI_REG_SIRD	0xA0
#define AHCI_REG_STRD	0xA4

#define AHCI_REG_CC_BC_POS   (8)
#define AHCI_REG_PCS_PXP_POS (16)

#define AHCI_ABAR_CAP	0x00
#define AHCI_ABAR_CAP_SIS             BIT28 // Supports Interlock Switch
#define AHCI_ABAR_CAP_SSS             BIT27 // Supports Stagger Spin-up
#define AHCI_ABAR_CAP_SALP            BIT26
#define AHCI_ABAR_CAP_PMS             BIT17 // Supports Port Multiplier
#define AHCI_ABAR_CAP_SSC             BIT14
#define AHCI_ABAR_CAP_PSC             BIT13
#define AHCI_ABAR_CAP_SXS             BIT5  // External SATA is supported
#define AHCI_ABAR_CAP_ISS_MASK        (BIT23 | BIT22 | BIT21 | BIT20)
#define AHCI_ABAR_CAP_SCLO            BIT24 // Supports Command List Override
#define AHCI_ABAR_CAP_PMD             BIT15 // PIO Multiple DRQ Block
#define AHCI_ABAR_CAP_ISS_6_0_G       0x03
#define AHCI_ABAR_CAP_ISS_3_0_G		0x03
#define AHCI_ABAR_CAP_ISS             20    // Interface Speed Support

#define AHCI_ABAR_GHC	0x04
#define AHCI_ABAR_IS_MASTER	0x8
#define AHCI_ABAR_PI	0xC
#define AHCI_ABAR_CAP2	0x24
#define AHCI_ABAR_VSP	0xA0
#define AHCI_ABAR_PxCLB		0x100
#define AHCI_ABAR_PxCLBU	0x104
#define AHCI_ABAR_PxFB		0x108
#define AHCI_ABAR_PxFBU		0x10C
#define AHCI_ABAR_PxIS		0x110
#define AHCI_ABAR_PxIE		0x114
#define AHCI_ABAR_PxCMD		0x118
#define AHCI_ABAR_PxTFD		0x120
#define AHCI_ABAR_PxSSTS	0x128
#define AHCI_ABAR_PxSCTL	0x12C
#define AHCI_ABAR_PxSERR	0x130
#define AHCI_ABAR_PxCI		0x138
#define AHCI_ABAR_PxFBS		0x140

#define AHCI_ABAR_GHC_AE_POS (31)
#define AHCI_ABAR_GHC_IE_POS (1)
#define AHCI_PXCMD_ATAPI_POS (24)
#define AHCI_PXCMD_APSTE_POS (23)
#define AHCI_PXCMD_POD_POS   (2)
#define AHCI_PXCMD_SUD_POS   (1)
#define AHCI_CAP2_APST_POS   (2)
#define AHCI_PXSCTL_DET_POS  (0)

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
#define MWAIT_HINT	0x20
#define ABAR_LOC	0x91000000
//#define IOAPIC_AID	32
#define IOAPIC_AID	2

#define SIPI_AREA		0x3000
#define SIPI_MONITOR_LOC	0x4000

typedef struct CommandHeader
{
	UINT16 Misc;
	UINT16 PRDTL;
	UINT32 PRDBC;
	UINT32 CTBA;
	UINT32 CTBAU;
	UINT32 pad[4];
} COMMAND_HEADER;


typedef struct ReceivedFIS
{
	UINT8 DSFIS[0x20];
	UINT8 PSFIS[0x20];
	UINT8 RFIS[0x18];
	UINT8 SDBFIS[0x8];
	UINT8 UFIS[0x40];
	UINT8 Reserved[0x60];
} RECEIVED_FIS;


typedef struct CFIS
{
	UINT8	FISType;
	UINT8	MiscFlags;
	UINT8	Command;
	UINT8	Features;
	UINT8	LBALow;
	UINT8	LBAMid;
	UINT8	LBAHigh;
	UINT8	Device;
	UINT8	LBALowExp;
	UINT8	LBAMidExp;
	UINT8	LBAHighExp;
	UINT8	FeaturesExp;
	UINT8	SectorCount;
	UINT8	SectorCountExp;
	UINT8	Reserved;
	UINT8	Control;
	UINT8	Padding[48];
} CIFS;

typedef struct PRDT
{
	UINT32 DBA;
	UINT32 DBAU;
	UINT32 Reserved;
	UINT32 DBC;
} PRDT;

typedef struct ACMD
{
	UINT8 pad[0x10];
} ACMD;

#define MAX_PRDTs (8) // Need 8 4MB prdts to do 32MB max transfer size(64k sectors * 512bytes) of SATA controller
typedef struct CommandTable
{
	struct CFIS cfis;
	struct ACMD acmd;
	UINT8  pad[0x30];
	struct PRDT prdt[MAX_PRDTs]; 
} COMMAND_TABLE;

typedef	struct SIRPAIR_ {
	UINT8	Address;
	UINT32	Data;
} SIRPAIR;

/*
typedef struct MSIMSGCTRL_{
	UINT16 MSIEn : 1;
	UINT16 MultMsgCap : 3;
	UINT16 MultMsgEn : 3;
	UINT16 64BitCap : 1;
	UINT16 MaskCap : 1;
	UINT16 Rsvd : 7;
}MSIMSGCTRL;

typedef struct MSICAP_{
	UINT8  CapId;
	UINT8  NextPtr;
	struct MSIMSGCTRL MsgCtrl;
	UINT32 MsgAdrL;
	UINT32 MsgAdrH;
	UINT16 MsgData;
}MSICAP;
*/

//
// Block of data needed to communicate between the interrupt handler and runCommands
//
typedef struct INTERRUPT_BLOCK {
	UINT32 monitorloc;
	UINT32 abar;
	UINT8  SataPort;
	UINT8  PortVal;
} INTERRUPT_BLOCK;

// Because we are still operating out of cache as RAM, we need to use fixed addresses that
// are in RAM for the various hardware queues, and not auto allocated variables
#define COMMAND_HEADER_LIST     (struct CommandHeader *)(UINTN)0x200000
#define COMMAND_LIST            (struct CommandTable *)(UINTN)0x201000
#define RECEIVED_FIS_AREA       (struct ReceivedFIS *)(UINTN)0x203000
#define MONITOR_LOC              (UINTN)0x204000
#define SCRATCH_PAD_AREA         (UINTN)0x205000


extern EFI_GUID gC2fPartitionGuid;
extern CHAR16*  gC2fPartitionName;
#define C2F_PARTITION_GUID        gC2fPartitionGuid
#define C2F_PARTITION_NAME        gC2fPartitionName
#define C2F_NAME                  (CHAR16 *)L"C2F "
#define C2F_PARTITION_NAME_LENGTH (4)


#endif //__C2FAHCI_H__
