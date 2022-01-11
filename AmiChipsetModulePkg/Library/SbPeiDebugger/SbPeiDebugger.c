//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbPeiDebugger.c
    This file contains PEI stage debugger code for SB template

*/

// Files included from EDKII
#include <PCI.h>
#include <Sb.h>
#include <Library/SmBusCommon.h>
#include <AmiDebugPort.h>
#include <token.h>
#include <AmiChipsetIoLib.h>
#include <AmiCspLib.h>

// Definitions
#define PM_STATUS_CTRL          0x8000
#define DBG_PRT_CAP_ID          0xA
#define TIMER_CONTROL_PORT      0x43
#define TIMER0_COUNT_PORT       0x40
#define DBG_EHCI_PCI_ADDRESS    EFI_PCI_ADDRESS(DEBUG_EHCI_BUS_NUMBER, \
                                                DEBUG_EHCI_DEVICE_NUMBER, \
                                                DEBUG_EHCI_FUNCTION_NUMBER, \
                                                0)

// Function Prototypes
VOID   Pei8259WriteMask (IN UINT16 Mask, IN UINT16 EdgeLevel);
VOID   Pei8259SetVectorBase (IN UINT8 MasterBase, IN UINT8 SlaveBase);
VOID   Program8254Timer0 (IN UINT16 Count);

#if USB_DEBUG_TRANSPORT == 1
EFI_STATUS SBPEIDBG_InitUsbEhci (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
);
#endif

#if USB_3_DEBUG_SUPPORT == 1
EFI_STATUS SbPeiDbgInitXhci (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
);
#endif

#if USB_DEBUG_TRANSPORT == 1
/**
    This eLink function is used to initialize the EHCI controller
    debug port for USB PEI Debugging. It also fills the transport
    interface structure with appropriate information

    @param DebugPort Debug transport interface structure

    @retval EFI_STATUS

    @note  PORTING REQUIRED
**/
EFI_STATUS SBPEIDBG_InitUsbEhci (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
)
{
// Denverton AptioV override Start - EIP#239760
// Denverton has Xhci Debug port. Following programming is not required
/*
    UINT8     PwrMgtCapReg;
    UINTN     PciAddress;
    UINT16    CmdReg;
    UINT8     DbgPrtCapReg;
    UINT16    DbgPrtBaseOffset;
    UINT32    EhciBaseAddress = 0;
#if defined(PEI_EHCI_MEM_BASE_ADDRESSES) && defined(PEI_EHCI_PCI_BDFS)
    UINT32    EhciBusDevFun[] = PEI_EHCI_PCI_BDFS;
    UINT32    EhciBaseAddr[] = PEI_EHCI_MEM_BASE_ADDRESSES;
    UINT32    i;
#endif

    if(DebugPort->UsbDebugPort.USBBaseAddr) {
      EhciBaseAddress = DebugPort->UsbDebugPort.USBBaseAddr;
    } else {
      EhciBaseAddress = EHCI_MMIO_BASE_ADDRESS;
#if defined(PEI_EHCI_MEM_BASE_ADDRESSES) && defined(PEI_EHCI_PCI_BDFS)
      for (i = 0; i < (sizeof(EhciBusDevFun) / sizeof(UINT32)); i++) {
        if (EhciBusDevFun[i] == DBG_EHCI_PCI_ADDRESS) {
            EhciBaseAddress = EhciBusDevFun[i];
            break;
        }
      }
#endif
    }
    //
    // First disable the EHCI device
    //
    PciAddress = CSP_PCIE_CFG_ADDRESS (
                  DEBUG_EHCI_BUS_NUMBER,
                  DEBUG_EHCI_DEVICE_NUMBER,
                  DEBUG_EHCI_FUNCTION_NUMBER,
                  0
                  );

    CmdReg = READ_PCI16(DEBUG_EHCI_BUS_NUMBER, \
                        DEBUG_EHCI_DEVICE_NUMBER, \
                        DEBUG_EHCI_FUNCTION_NUMBER, \
                        PCI_COMMAND_REGISTER_OFFSET \
                        );

    if(CmdReg & 2) {
      WRITE_PCI16(DEBUG_EHCI_BUS_NUMBER, \
                  DEBUG_EHCI_DEVICE_NUMBER, \
                  DEBUG_EHCI_FUNCTION_NUMBER, \
                  PCI_COMMAND_REGISTER_OFFSET, \
                  0 \
                  );
    }

    //
    // Assign base address register
    //
    WRITE_PCI32(DEBUG_EHCI_BUS_NUMBER, \
                DEBUG_EHCI_DEVICE_NUMBER, \
                DEBUG_EHCI_FUNCTION_NUMBER, \
                0x10, \
                EhciBaseAddress \
                );

    //
    // Set the Power state of the device to D0
    //
    PwrMgtCapReg = FindCapPtr (
                    PciAddress,
                    PCI_CAP_ID_PMI
                    );

    WRITE_PCI16(DEBUG_EHCI_BUS_NUMBER, \
                DEBUG_EHCI_DEVICE_NUMBER, \
                DEBUG_EHCI_FUNCTION_NUMBER, \
                PwrMgtCapReg + 4, \
                PM_STATUS_CTRL \
                );

    //
    // Enable PCH EHCI register & make it Bus master
    //
    CmdReg = CmdReg | 0x06;
    WRITE_PCI16(DEBUG_EHCI_BUS_NUMBER, \
                DEBUG_EHCI_DEVICE_NUMBER, \
                DEBUG_EHCI_FUNCTION_NUMBER, \
                PCI_COMMAND_REGISTER_OFFSET, \
                CmdReg \
                );

    //
    // Locate the Debug port register Interface location
    //
    DbgPrtCapReg = FindCapPtr (
                    PciAddress,
                    DBG_PRT_CAP_ID
                    );

    DbgPrtBaseOffset = READ_PCI16(DEBUG_EHCI_BUS_NUMBER, \
                                  DEBUG_EHCI_DEVICE_NUMBER, \
                                  DEBUG_EHCI_FUNCTION_NUMBER, \
                                  DbgPrtCapReg + 2 \
                                  ) & 0x1fff;

    if (DebugPort->UsbDebugPort.USBBaseAddr == 0) {
      DebugPort->UsbDebugPort.USBBaseAddr = EhciBaseAddress;
      DebugPort->UsbDebugPort.MemoryMappedIoSpaceSize = EHCI_MMIO_SIZE;
    }
    DebugPort->UsbDebugPort.USBDebugPortStartAddr = EhciBaseAddress + DbgPrtBaseOffset;
    DebugPort->UsbDebugPort.PciBusNumber          = DEBUG_EHCI_BUS_NUMBER;
    DebugPort->UsbDebugPort.PciDeviceNumber       = DEBUG_EHCI_DEVICE_NUMBER;
    DebugPort->UsbDebugPort.PciFunctionNumber     = DEBUG_EHCI_FUNCTION_NUMBER;
    DebugPort->UsbDebugPort.PciBAROffset          = PCI_EHCI_BAR_OFFSET;
    DebugPort->UsbDebugPort.InitUSBEHCI           = SBPEIDBG_InitUsbEhci;
*/
// Denverton AptioV override End - EIP#239760
    return EFI_SUCCESS;
}
#endif

#if USB_3_DEBUG_SUPPORT == 1
/**
    This eLink function is used to initialize the EHCI controller
    debug port for USB PEI Debugging. It also fills the transport
    interface structure with appropriate information

    @param DebugPort Debug transport interface structure

    @retval EFI_STATUS

    @note  1. xHCI Controllers must be initialized after PEI Memory is installed (Init power state, clock ..etc), ensure USB xHCI Debug Capability is functionality. 
    @note  2. The entries for xHCI controllers in gAmiBadPciDevList of PciBus.c must be removed.
    @note  3. xHCI controllers can't be re-initialized/disabled at DXE.
    @note  PORTING REQUIRED
**/
EFI_STATUS SbPeiDbgInitXhci (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
)
{
    // Denverton AptioV override Start - EIP#239760
    // Denverton has Xhci Debug port. Following programming is not required
    /*
    UINT8     PwrMgtCapReg;
    UINTN     PciAddress;
    UINT16    CmdReg;
    UINT32    XhciBaseAddress = 0;

    if(DebugPort->UsbDebugPort.USBBaseAddr) {
      XhciBaseAddress = DebugPort->UsbDebugPort.USBBaseAddr;
    } else {
      XhciBaseAddress = XHCI_MMIO_BASE_ADDRESS;
    }
    
    //
    // First disable the XHCI device
    //
    PciAddress = CSP_PCIE_CFG_ADDRESS (
                  DEBUG_XHCI_BUS_NUMBER,
                  DEBUG_XHCI_DEVICE_NUMBER,
                  DEBUG_XHCI_FUNCTION_NUMBER,
                  0
                  );

    CmdReg = READ_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                        DEBUG_XHCI_DEVICE_NUMBER, \
                        DEBUG_XHCI_FUNCTION_NUMBER, \
                        PCI_COMMAND_REGISTER_OFFSET \
                        );

    if(CmdReg & 2) {
      WRITE_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                  DEBUG_XHCI_DEVICE_NUMBER, \
                  DEBUG_XHCI_FUNCTION_NUMBER, \
                  PCI_COMMAND_REGISTER_OFFSET, \
                  0 \
                  );
    }

    //
    // Assign base address register
    //
    WRITE_PCI32(DEBUG_XHCI_BUS_NUMBER, \
                DEBUG_XHCI_DEVICE_NUMBER, \
                DEBUG_XHCI_FUNCTION_NUMBER, \
                0x10, \
                XhciBaseAddress \
                );

    //
    // Set the Power state of the device to D0
    //
    PwrMgtCapReg = FindCapPtr (
                    PciAddress,
                    PCI_CAP_ID_PMI
                    );

    WRITE_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                DEBUG_XHCI_DEVICE_NUMBER, \
                DEBUG_XHCI_FUNCTION_NUMBER, \
                PwrMgtCapReg + 4, \
                PM_STATUS_CTRL \
                );

    //
    // Enable PCH XHCI register & make it Bus master
    //
    CmdReg = CmdReg | 0x06;
    WRITE_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                DEBUG_XHCI_DEVICE_NUMBER, \
                DEBUG_XHCI_FUNCTION_NUMBER, \
                PCI_COMMAND_REGISTER_OFFSET, \
                CmdReg \
                );

    if (DebugPort->UsbDebugPort.USBBaseAddr == 0) {
      DebugPort->UsbDebugPort.USBBaseAddr = XhciBaseAddress;
    }
    DebugPort->UsbDebugPort.PciBusNumber          = DEBUG_XHCI_BUS_NUMBER;
    DebugPort->UsbDebugPort.PciDeviceNumber       = DEBUG_XHCI_DEVICE_NUMBER;
    DebugPort->UsbDebugPort.PciFunctionNumber     = DEBUG_XHCI_FUNCTION_NUMBER;
    DebugPort->UsbDebugPort.InitUSBEHCI           = SbPeiDbgInitXhci;
*/
// Denverton AptioV override End - EIP#239760
    return EFI_SUCCESS;
}
#endif

/**
    This function programs the SB register to enable the serial IRQ

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID EnableSerialIrq (
    VOID
)
{
    // TODO - ** PORTING REQUIRED **  specific to chipset, as shown below.
    //
    // Enable Serial IRQ, set it to continuous mode and set
    // Start Frame Pulse Width to 4 clocks
    //
    //WRITE_PCI8_SB(R_PCH_LPC_SERIRQ_CNT, (UINT8) 0xD0);
}

/**
    This function programs the SB register to enable the LPC IO
    Decoding ranges to enable the programming of SIO and Serial Port.

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID EnableLpcIoDecode (
    VOID
)
{
    UINT32      GEN2_DEC = 0;
    UINT16      AndData = 0;
    UINT16      OrData = 0;

    // TODO - ** PORTING REQUIRED **  specific to chipset, as shown below.
    //
    // Enables SIO Configuration Ports decode
    //
    //GEN2_DEC = READ_PCI32_SB (R_PCH_LPC_GEN2_DEC);
    //GEN2_DEC |= 0xFFFFFFFF & ((0x0C << 16) | ((0x16 << 8) | 0x41));
    //WRITE_PCI32_SB(R_PCH_LPC_GEN2_DEC, GEN2_DEC);

    // TODO - ** PORTING REQUIRED **  specific to chipset, as shown below.
    //
    // Enable COMA/COMB decode
    //
    //AndData = (UINT16) (B_PCH_LPC_COMB_DEC | B_PCH_LPC_COMA_DEC);
    //OrData  = (UINT16) (V_PCH_LPC_COMB_2F8 | V_PCH_LPC_COMA_3F8);
    //RW_PCI16_SB(R_PCH_LPC_IO_DEC, OrData, AndData);
  

    //OrData  = (UINT16) (B_PCH_LPC_ENABLES_COMB_EN | B_PCH_LPC_ENABLES_COMA_EN);
    //SET_PCI16_SB(R_PCH_LPC_ENABLES, OrData);
}

// Denverton AptioV override Start - EIP#273894 - To solve 0xC3 unexpected reboot
/**
    This function programs the SB register to disable
    the TCO timer. If this timer is not disabled the system
    will shutdown or reset as soon as the timer is expired

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID ProgramSmbusDeviceToDisableTco (
    VOID
)
{

    // Disable TCO in SMBUS Device first before changing base address.

    MmioAnd16 ((UINTN)SB_PCIE_CFG_ADDRESS (
	                DEFAULT_PCI_BUS_NUMBER_PCH,
	                PCI_DEVICE_NUMBER_PCH_SMBUS,
	                PCI_FUNCTION_NUMBER_PCH_SMBUS,
	                R_PCH_SMBUS_TCOCTL),
	             (UINT16) ~B_PCH_SMBUS_TCOCTL_TCO_BASE_EN
	             );

    // Program TCO in SMBUS Device
    
    MmioAndThenOr16 ((UINTN)SB_PCIE_CFG_ADDRESS (
	                DEFAULT_PCI_BUS_NUMBER_PCH,
	                PCI_DEVICE_NUMBER_PCH_SMBUS,
	                PCI_FUNCTION_NUMBER_PCH_SMBUS,
	                R_PCH_SMBUS_TCOBASE),
	                (UINT16) (~B_PCH_SMBUS_TCOBASE_BAR),
	                (UINT16) PCH_TCO_BASE_ADDRESS
	                );
	  
    // Enable TCO in SMBUS Device
 
    MmioOr16 ((UINTN)SB_PCIE_CFG_ADDRESS (
	                DEFAULT_PCI_BUS_NUMBER_PCH,
	                PCI_DEVICE_NUMBER_PCH_SMBUS,
	                PCI_FUNCTION_NUMBER_PCH_SMBUS,
	                R_PCH_SMBUS_TCOCTL),
	            (UINT16)B_PCH_SMBUS_TCOCTL_TCO_BASE_EN
	            );

    // Halt the TCO timer

    IoOr16 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, B_PCH_TCO_CNT_TMR_HLT);
    IoOr16 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS, B_PCH_TCO2_STS_SECOND_TO);


}
// Denverton AptioV override End - EIP#273894 - To solve 0xC3 unexpected reboot

/**
    This function programs the onboard clock generator.

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED if needed.
**/
VOID ProgramClockGenerator (VOID)
{

}

/**
    This eLink function is used to initialize the South Bridge
    for PEI Debugger support

    @param DebugPort Debug transport interface structure

    @retval EFI_STATUS

    @note  Normally no PORTING REQUIRED
**/
EFI_STATUS SBPEIDBG_Initialize (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
)
{
    //
    // Program the TCO IO to avoid rebooting of the hardware
    //
    ProgramSmbusDeviceToDisableTco(); // Denverton AptioV override: EIP#273894 - To solve 0xC3 unexpected reboot

    //
    // Program Clock Generator
    //
    ProgramClockGenerator();

    //
    // Initialize 8259 Controller
    //
    // Pei8259SetVectorBase (LEGACY_MODE_BASE_VECTOR_MASTER, LEGACY_MODE_BASE_VECTOR_SLAVE ); // Denverton AptioV override - EIP#239760

    //
    // Set all 8259 Interrupts to edge triggered and disabled
    //
    Pei8259WriteMask (0xffff , 0x0000);

    EnableLpcIoDecode ();

    EnableSerialIrq ();

    Program8254Timer0 (0);

    return EFI_SUCCESS;
}


/**
    This function searches the PCI address space for the PCI
    device specified for a particular capability ID and returns
    the offset in the PCI address space if one found

    @param Bus      PCI Bus number
    @param DevFunc  PCI Device and function number
    @param CapId    Capability ID to look for

    @retval Capability ID location if one found
            Otherwise returns 0
**/
UINT8
FindCapPtrDbg(
 IN UINT8 Bus,
 IN UINT8 DevFunc,
 IN UINT8 CapId
)
{
    UINTN     PcieAddress;
    UINT8     Dev;
    UINT8     Func;

    Dev = (DevFunc >> 3) & 0x1F;
    Func = DevFunc & 0x07;
    PcieAddress = CSP_PCIE_CFG_ADDRESS(Bus, Dev, Func, 0);
    return FindCapPtr (PcieAddress, CapId);
}


/**
    Writes PC 8259 interrupt Controller mask register

    @param Mask Mask to write
    @param EdgeLevel Edge/level trigger to be programmed

    @retval VOID

**/
VOID
Pei8259WriteMask (
    IN UINT16 Mask, 
    IN UINT16 EdgeLevel)
{
// Denverton AptioV override Start - EIP#239760
/*
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER, (UINT8)Mask);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,  (UINT8)(Mask >> 8));
    IoWrite8 (LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_MASTER, (UINT8)EdgeLevel);
    IoWrite8 (LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_SLAVE,  (UINT8)(EdgeLevel >> 8));
	*/
// Denverton AptioV override End - EIP#239760	
}


/**
    Set up the 8259 interrupt controller master and slave base values

    @param MasterBase Master base to be programmed
    @param SlaveBase Slave base to be programmed

    @retval VOID

**/
VOID
Pei8259SetVectorBase (
    IN UINT8 MasterBase, 
    IN UINT8 SlaveBase
)
{
// Denverton AptioV override Start - EIP#239760
    /*
	UINT8           Mask;
    
    // Initialize Slave interrupt controller.
    Mask = IoRead8 (LEGACY_8259_MASK_REGISTER_SLAVE);
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_SLAVE, 0x11);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    SlaveBase);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    0x02); 
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    0x01);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    Mask);

    // Initialize Master interrupt controller.
    Mask = IoRead8 (LEGACY_8259_MASK_REGISTER_MASTER);
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_MASTER, 0x11);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    MasterBase);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    0x04);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    0x01); 
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    Mask); 

    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_SLAVE,  LEGACY_8259_EOI);
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_MASTER, LEGACY_8259_EOI);
	*/
// Denverton AptioV override End - EIP#239760
}

/**
    Programs the 8254 Timer channel 0

    @param Count Timer tick count

    @retval VOID

**/
VOID Program8254Timer0(
    IN UINT16 Count
)
{
// Denverton AptioV override Start - EIP#239760
/*
    UINT8   LCntbyte,HCntbyte;

    LCntbyte = (UINT8)Count;
    HCntbyte = (UINT8)(Count >> 8);

    //Write the timer control port to select timer 0 and set to mode 3
    IoWrite8(TIMER_CONTROL_PORT, 0x36);

    //Write the Counter 0 with initial count value
    IoWrite8(TIMER0_COUNT_PORT, LCntbyte);
    IoWrite8(TIMER0_COUNT_PORT, HCntbyte);
*/
// Denverton AptioV override End - EIP#239760
    //Enable the IRQ0 interrupt for this timer 0 in USB Xport module
    //......
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
