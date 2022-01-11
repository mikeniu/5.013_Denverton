//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 1999 - 2008 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformClock.h

--*/

#ifndef _PLATFORM_DEFINITIONS_CLOCK_H_
#define _PLATFORM_DEFINITIONS_CLOCK_H_

//
// Clock controller SMBUS address
//
#define CLOCK_CONTROLLER_SMBUS_ADDRESS 0xD2

//
// Redfort platform CK505 configuration bytes 0~12
// (Reference:  Table 21, CK505 Clock Synthesizer Specification Revision 1.0
// October 2006 Intel)
//
// BIT7       BIT6       BIT5       BIT4       BIT3       BIT2       BIT1       BIT0
// =================================================================================
// Byte 0 
// ------
// FSC(r)     FSB(r)     FSA(r)     MT_EN      Reserved   SRC_MAIN   SATA_SEL   PD_RESTORE
//                                                        _SEL
// 0          0          0          0           0         1          0          1 
//
// Byte 1
// ------
// SRC0_SEL   PLL1_SSC   PLL3_SSC   PLL3_CFB3  PLL3_CFB2  PLL3_CFB1  PLL3_CFB0  PCI_SEL
//            _DC        _DC
// 1          0          0          0          0          0          1          1
// 
// Byte 2
// ------
// REF_OE     USB_OE     PCIF5_OE   PCI4_OE    PCI3_OE    PCI2_OE    PCI1_OE    PCI0_OE
// 1          0          0          0          0          0          0          0
// 
// Byte 3
// ------
// SRC11_OE   SRC10_OE   SRC9_OE    SRC8/ITP   SRC7_OE    SRC6_OE    SRC5_OE    SRC4_OE
//                                  _OE
// 0          0          0          0          0          0          0          0
//
// Byte 4
// ------
// SRC3_OE    SATA/SRC2  SRC1_OE    SRC0       CPU1_OE    CPU0_OE    PLL1_SSC   PLL3_SSC_EN   
//            _OE                   /DOT96_OE                         _EN
// 0          1          1          1          0          1          0          0
// 
// Byte 5
// ------
// CR#_A_EN   CR#_A_SEL  CR#_B_EN   CR#_B_SEL  CR#_C_EN   CR#_C_SEL  CR#_D_EN   CR#_D_SEL
// 0          0          0          0          0          0          0          0 
//
// Byte 6
// ------
// CR#_E_EN   CR#_F_EN   CR#_G_EN   CR#_H_EN   Reserved   Reserved   SSCD_STP   SRC_STP_CRTL
//                                                                   _CRTL
// 0          0          0          0          0          0          0          0
// 
// Byte 7
// ------
// RC Bit3(r) RC Bit2(r) RC Bit1(r) RC Bit0(r) VID Bit3(r)VID Bit2(r)VID Bit1(r)VID Bit0(r)
// 0          0          0          0          0          0          0          0
// 
// Byte 8
// ------
// DID3(r)    DID2(r)    DID1(r)    DID0(r)    Reserved   Reserved   SE1_OE     SE2_OE
// 0          0          0          0          0          0          0          0
//
// Byte 9
// ------
// PCIF5_STP  TME_STRAP  REF_DRIVE  TEST_MODE  TEST_MODE  IO_VOUT2   IO_VOUT1   IO_VOUT0
// _CTRL      (r)                   _SEL       _ENTRY
// 0          0          1          0          0          1          0          1
//
// Byte 10
// -------
// SRC5_EN    PLL3_EN    PLL2_EN    SRC_DIV_EN PCI_DIV_EN CPU_DIV_EN CPU1_STOP  CPU0_STOP
// _STRAP(r)                                                         _EN        _EN
// 1          1          1          0          0          0          0          0
// 
// Byte 11
// -------
// PCI3_CFG1  PCI3_CFG0  25MHZ_EN   PLL4_EN    CPU2_AMT   CPU1_AMT   PCI-E_GEN2 CPU2_STOP
// (r)        (r)        _SE1                  _EN        _EN        (r)        _EN
// 0          1          0          1          0          0          1          0 
//
// Byte 12
// -------
// Reserved   Reserved   BC5        BC4        BC3        BC2        BC1        BC0
// 0          0          0          0          1          1          0          1
//

#define MOBILE_CLOCK_CONTROLLER_SETTINGS {0x05, 0x83, 0x80, 0, 0x77, 0x00, 0x0, 0x00, 0, 0x25, 0xe0, 0x52, 0xd}
#define DESKTOP_CLOCK_CONTROLLER_SETTINGS {0x35, 0x83, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x26, 0x03, 0x25, 0xFD, 0x56}

#endif
