//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//*************************************************************************
// Name:        VirtualSerialComPort3.ASL
//
// Description: Define ACPI methods for Virtual ComPort
//
//*************************************************************************
//<AMI_FHDR_END>

Scope(_SB.PCI0) {
    Device(UAR3) {
        Name(_HID, EISAID("PNP0501"))    //PnP Device ID 16550 Type

        Method(_UID, 0) {
            Return(UID3)
        }

        Method(_STA, 0, Serialized) {
            Return(0x000F)
        }

        Method(_CRS, 0, Serialized) {
            Name(BUF0, ResourceTemplate() {
                IO(Decode16, 0, 0, 0x01, 0x8, IOP)
                IRQNoFlags(IRQx) {}
                DMA(Compatibility, NotBusMaster, Transfer8) {}
           })

            CreateWordField(BUF0, IOP._MIN, IOPL)
            CreateWordField(BUF0, IOP._MAX, IOPH)
            CreateWordField(BUF0, IRQx._INT, IRQ)

            Store(VDA3, IOPL )
            Store(VDA3, IOPH )

            ShiftLeft(1,VIR3,IRQ)

            Return(BUF0)
        }
    } // End Of UAR3 
} //End of Scope _SB.PCI0
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************