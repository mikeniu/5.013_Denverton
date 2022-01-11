//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file WdatBoard.c 
    Entry point for WDAT initialization. 

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Wdat.h>
#include <Token.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)
/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming
**/
#define PCH_PCR_ADDRESS(Pid, Offset)    (0xFD000000 | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

#define TCO_RLD_TMP     1
#define TCO_TMR_TMP     2
#define TCO1_CNT_TMP    3
#define TCO2_CNT_TMP    4
#define TCO2_STS_TMP    5
#define GCS_ADDR_TMP    6

#define TCO_RLD_OFFSET      0x0
#define TCO_TMR_OFFSET      0x12
#define TCO1_CNT_OFFSET     0x8
#define TCO2_CNT_OFFSET     0xA
#define TCO2_STS_OFFSET     0x6
#define GCS_ADDR            PCH_PCR_ADDRESS(0xC6, 0x0C) // R_PCH_PCR_SMBUS_GC

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
///WDAT Header
///WDAT_HDR  WdatHdr = {*HdrLength,PciSeg    ,PciBus ,PciDev ,PciFun ,Reserved   ,TimerPeriod,MaxCount   ,MinCount   ,WdFlag                                 ,Reserved[3],ActionTableCount   };
 WDAT_HDR    WdatHdr = {0         ,PCI_SEG   ,PCI_BUS,PCI_DEV,PCI_FUN,0,0,0      ,0x258      ,0x3ff      ,0x2        ,WD_ENABLED|WD_STOPPED_IN_SLEEP_STATE   ,0,0,0      ,0   };

///Action Instruction Table
///      {WatchdogAction                    ,InstructionFlags                       ,Reserved1  ,AddrSpcID  ,RegBitWidth,RegBitOffs ,AccessSize ,Address        ,Value  ,Mask   };
WDAT_INST_ENTRY ActionTable[] = {
         {WDA_RESET                         ,WDI_WRITE_VALUE                        ,0          ,GAS_SYS_IO ,10         ,0          ,2          ,TCO_RLD_TMP    ,0x04   ,0x3ff  },
         {WDA_QUERY_CURRENT_COUNTDOWN_PERIOD,WDI_READ_COUNTDOWN                     ,0          ,GAS_SYS_IO ,10         ,0          ,2          ,TCO_RLD_TMP    ,0x0    ,0x3ff  },
         {WDA_QUERY_COUNTDOWN_PERIOD        ,WDI_READ_COUNTDOWN                     ,0          ,GAS_SYS_IO ,10         ,0          ,2          ,TCO_TMR_TMP    ,0X0    ,0x3ff  },
         {WDA_SET_COUNTDOWN_PERIOD          ,WDI_WRITE_COUNTDOWN                    ,0          ,GAS_SYS_IO ,10         ,0          ,2          ,TCO_TMR_TMP    ,0x10   ,0x3ff  },
         {WDA_SET_COUNTDOWN_PERIOD          ,WDI_WRITE_COUNTDOWN                    ,0          ,GAS_SYS_IO ,10         ,0          ,2          ,TCO_RLD_TMP    ,0x04   ,0x3ff  },
         {WDA_QUERY_RUNNING_STATE           ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,1          ,11         ,2          ,TCO1_CNT_TMP   ,0x0    ,0x1    },
         {WDA_SET_RUNNING_STATE             ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,1          ,11         ,2          ,TCO1_CNT_TMP   ,0x0    ,0x1    },
         {WDA_QUERY_STOPPED_STATE           ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,1          ,11         ,2          ,TCO1_CNT_TMP   ,0x1    ,0x1    },
         {WDA_SET_STOPPED_STATE             ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,1          ,11         ,2          ,TCO1_CNT_TMP   ,0x1    ,0x1    },
         {WDA_QUERY_REBOOT                  ,WDI_READ_VALUE                         ,0          ,GAS_SYS_MEM,1          ,1          ,3          ,GCS_ADDR_TMP   ,0x0    ,0x1    },
         {WDA_SET_REBOOT                    ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_MEM,1          ,1          ,3          ,GCS_ADDR_TMP   ,0x0    ,0x1    },
         {WDA_QUERY_SHUTDOWN                ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,2          ,4          ,2          ,TCO2_CNT_TMP   ,0x1    ,0x3    },
         {WDA_SET_SHUTDOWN                  ,WDI_WRITE_VALUE                        ,0          ,GAS_SYS_IO ,2          ,4          ,2          ,TCO2_CNT_TMP   ,0x1    ,0x3    },
         {WDA_QUERY_WATCHDOG_STATUS         ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,1          ,1          ,2          ,TCO2_STS_TMP   ,0x1    ,0x1    },
         {WDA_SET_WATCHDOG_STATUS           ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,1          ,1          ,2          ,TCO2_STS_TMP   ,0x1    ,0x1    }};    

UINT32	ActionTableLen = sizeof(ActionTable) / sizeof(WDAT_INST_ENTRY);

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------   
/**
  This procedure will get PCIE address
  
  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number

  @retval PCIE address
**/
UINTN
MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
  )
{
  ASSERT ((Bus <= 0xFF) && (Device <= 0x1F) && (Function <= 0x7));

  return ((UINTN) PCIEX_BASE_ADDRESS + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) (Function << 12));
}

/**
  Get PCH TCO base address.

  @param[in] Address                    Address of TCO base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
PchTcoBaseGet (
  UINT16                                *Address
  )
{
  UINTN                                 SmbusBase;

  if (Address == NULL) {
    //DEBUG((DEBUG_ERROR, "PchTcoBaseGet Error. Invalid pointer.\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  SmbusBase = MmPciBase (
                0, 	// DEFAULT_PCI_BUS_NUMBER_PCH,
                31,	// PCI_DEVICE_NUMBER_PCH_SMBUS,
                4	// PCI_FUNCTION_NUMBER_PCH_SMBUS
                );
  if (MmioRead16 (SmbusBase) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  //*Address = MmioRead16 (SmbusBase + R_PCH_SMBUS_TCOBASE) & B_PCH_SMBUS_TCOBASE_BAR;
  *Address = MmioRead16 (SmbusBase + 0x50) & 0x0000FFE0;
  return EFI_SUCCESS;
}

/**
  Patch ActionTable register address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
PatchActionTable (
    VOID
  )
{
  UINTN     i;
  UINTN     error = 0;
  UINT16    TcoBaseAddress;
  
  PchTcoBaseGet(&TcoBaseAddress);
  
  for(i = 0; i <ActionTableLen; i++) {
      switch (ActionTable[i].RegisterRegion.Address)
      {
      case TCO_RLD_TMP:
          ActionTable[i].RegisterRegion.Address = TcoBaseAddress + TCO_RLD_OFFSET;
          break;
      case TCO_TMR_TMP:
          ActionTable[i].RegisterRegion.Address = TcoBaseAddress + TCO_TMR_OFFSET;
          break;
      case TCO1_CNT_TMP:
          ActionTable[i].RegisterRegion.Address = TcoBaseAddress + TCO1_CNT_OFFSET;
          break;
      case TCO2_CNT_TMP:
          ActionTable[i].RegisterRegion.Address = TcoBaseAddress + TCO2_CNT_OFFSET;
          break;
      case TCO2_STS_TMP:
          ActionTable[i].RegisterRegion.Address = TcoBaseAddress + TCO2_STS_OFFSET;
          break;
      case GCS_ADDR_TMP:
          ActionTable[i].RegisterRegion.Address = GCS_ADDR;
          break;
      default:
          error = 1;
          break;          
      }
  }
  
  if (error)
      return EFI_INVALID_PARAMETER;
  
  return EFI_SUCCESS;

}

/**
    Override the default Wdat Header and Action instruction table
    
    @retval EFI_SUCCESS Always returns EFI_SUCCESS.  
    
**/
EFI_STATUS WdatHeaderAndActionTableOverride(IN WDA_TABLE *pWdatTable)
{
	UINT64	WdatInstTableLen;
	UINT8	*ActInsEtnry = (UINT8*)pWdatTable + sizeof(ACPI_HDR) + sizeof(WDAT_HDR);
	
	PatchActionTable();
	
	WdatInstTableLen = ActionTableLen * sizeof(WDAT_INST_ENTRY);
	WdatHdr.WatchdogHeaderLength = sizeof(WDAT_HDR)+ (UINT32)WdatInstTableLen;
	WdatHdr.NumberWactchdogInstructionEntries = ActionTableLen;
	MemCpy(&pWdatTable->WatchdogHeader, &WdatHdr, sizeof(WDAT_HDR));
	MemCpy(ActInsEtnry, ActionTable,(UINTN)WdatInstTableLen);

	return EFI_SUCCESS;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
