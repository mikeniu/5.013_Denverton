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

/** @file RriorDma.asl
  This file is for SB/LPC I/O and DMA route/release.
   
  This file contains set of ACPI control methods implementation for
  South Bridge or LPC I/O and DMA route/release functions. It should
  be included in the PCI device of SB/LPC controller.
  
*/

/**
  Convert UART port address to the decoded value in SB/LPC bridge. 
  
  This function is used for converting UART port address to the 
  decoded value in SB/LPC registers.
  
  @param Arg0   Port to Route/Release
  
  @return UART port decoded value in the LPC bridge
  
**/

    Method(UXDV, 1) {
        Store(0xff, Local0) // Unknown
        Switch (Add(Arg0, 0)) {
            Case (0x3F8) { Store(0, Local0) }
            Case (0x2F8) { Store(1, Local0) }
            Case (0x220) { Store(2, Local0) }
            Case (0x228) { Store(3, Local0) }
            Case (0x238) { Store(4, Local0) }
            Case (0x2E8) { Store(5, Local0) }
            Case (0x338) { Store(6, Local0) }
            Case (0x3E8) { Store(7, Local0) }
        }
        return (Local0)
    }

/**
  Route/Release I/O ports to/from SB/LPC external I/O bus  
  
  This function is for route/release I/O ports to/from SB/LPC 
  external I/O bus
  
  @param Arg0   Device Category
                  0x00 - Serial A (Modem)
                  0x01 - Serial B (IR)
                  0x02 - LPT
                  0x03 - FDD
                  0x04 - SB16 Audio
                  0x05 - Midi
                  0x06 - Mss Audio
                  0x07 - Adlib sound (FM synth)
                  0x08 - Game 1 port
                  0x09 - Game 2 port
                  0x0A - KBC 60 & 64
                  0x0B - EC 62 & 66
                  0x0C - CNF1 2e & 2f (static location)
                  0x0D - CNF2 4e & 4f (static location)
                  0x0E - RESERVED
                  0x0F - RESERVED
                  0x10 - Generic I/O Range 1 // IRDA
                  0x11 - Generic I/O Range 2
                  0x12 - Generic I/O Range 3
                  0x13 - Generic I/O Range 4
  @param Arg1   1/0 Route/Release Mode
  @param Arg2   Port to Route/Release 
  @param Arg3   Port SIZE to Route (Optional) 
  
  @return Nothing
  
**/

    Method(RRIO, 4) {
/*
        Switch (Add(Arg0, 0)) {
            Case (0) {  // UART 0 or 2
                Store(0, CALE) // Disable the decoding
                Store (UXDV(Arg2), Local0)
                If (LNotEqual(Local0, 0xff)) { Store(Local0, CADR)}
                If (Arg1) { Store(1, CALE) }
            }
            Case (1) {  // UART 1 or 3
                Store(0, CBLE) // Disable the decoding
                Store (UXDV(Arg2), Local0)
                If (LNotEqual(Local0, 0xff)) { Store(Local0, CBDR)}
                If (Arg1) { Store(1, CBLE) }
            }
            Case (2) {  // LPT
                Store(0, LTLE) // Disable the decoding
                If (LEqual(Arg2, 0x378)) { Store(0, LTDR) }
                If (LEqual(Arg2, 0x278)) { Store(1, LTDR) }
                If (LEqual(Arg2, 0x3BC)) { Store(2, LTDR) }
                If (Arg1) { Store(1, LTLE) }
            }
            Case (3) {  // FDD
                Store(0, FDLE) // Disable the decoding
                If (LEqual(Arg2, 0x3F0)) { Store(0, FDDR) }
                If (LEqual(Arg2, 0x370)) { Store(1, FDDR) }
                If (Arg1) { Store(1, FDLE) }
            }
            Case (8) {  // Game 1
                If (LEqual(Arg2, 0x200)) {
                    If (Arg1) { Store(1, GLLE) }
                    Else { Store(0, GLLE) }
                }
                If (LEqual(Arg2, 0x208)) {
                    If (Arg1) { Store(1, GHLE) }
                    Else { Store(0, GHLE) }
                }
            }
            Case (9) {  // Game 2
                If (LEqual(Arg2, 0x200)) {
                    If (Arg1) { Store(1, GLLE) }
                    Else { Store(0, GLLE) }
                }
                If (LEqual(Arg2, 0x208)) {
                    If (Arg1) { Store(1, GHLE) }
                    Else { Store(0, GHLE) }
                }
            }
            Case (10) {  // Keyboard Controller
                If (LOr(LEqual(Arg2, 0x60), LEqual(Arg2, 0x64))) {
                    If (Arg1) { Store(1, KCLE) }
                    Else { Store(0, KCLE) }
                }
            }
            Case (11) {  // MicroController
                If (LOr(LEqual(Arg2, 0x62), LEqual(Arg2, 0x66))) {
                    If (Arg1) { Store(1, MCLE) }
                    Else { Store(0, MCLE) }
                }
            }
            Case (12) {  // Super I/O Enable 1
                If (LEqual(Arg2, 0x2E)) {
                    If (Arg1) { Store(1, C1LE) }
                    Else { Store(0, C1LE) }
                }
                If (LEqual(Arg2, 0x4E)) {
                    If (Arg1) { Store(1, C2LE) }
                    Else { Store(0, C2LE) }
                }
            }
            Case (13) {  // Super I/O Enable 2
                If (LEqual(Arg2, 0x2E)) {
                    If (Arg1) { Store(1, C1LE) }
                    Else { Store(0, C1LE) }
                }
                If (LEqual(Arg2, 0x4E)) {
                    If (Arg1) { Store(1, C2LE) }
                    Else { Store(0, C2LE) }
                }
            }

        }
*/        
    }

/**
  Route DMA Channels to SB/LPC external I/O bus  
  
  This function is for route DMA Channels to SB/LPC 
  external I/O bus
  
  @param Arg0   Device Category
  @param Arg1   1/0 Route/Release Mode
  @param Arg2   DMA channel to route (1 based) 
  
  @return Nothing
  
**/

    Method(rDMA, 3) {
    // Porting If needed.
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