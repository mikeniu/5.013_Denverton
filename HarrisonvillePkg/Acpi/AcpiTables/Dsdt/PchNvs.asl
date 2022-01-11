
/**@file

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

  //
  // Define PCH NVS Area operatino region.
  //
  
  Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
  Name (PNVL, 0xAA55)      // PCH NVS Length

  OperationRegion(PNVA,SystemMemory,PNVB,PNVL)
  Field(PNVA,AnyAcc,Lock,Preserve)
  {
  Offset(0),      RCRV, 32, // Offset(0),     RC Revision
  Offset(4),      PCHS, 16, // Offset(4),     PCH Series
  Offset(6),      PCHG, 16, // Offset(6),     PCH Generation
  Offset(8),      RPA1, 32, // Offset(8),     Root Port address 1
  Offset(12),     RPA2, 32, // Offset(12),    Root Port address 2
  Offset(16),     RPA3, 32, // Offset(16),    Root Port address 3
  Offset(20),     RPA4, 32, // Offset(20),    Root Port address 4
  Offset(24),     RPA5, 32, // Offset(24),    Root Port address 5
  Offset(28),     RPA6, 32, // Offset(28),    Root Port address 6
  Offset(32),     RPA7, 32, // Offset(32),    Root Port address 7
  Offset(36),     RPA8, 32, // Offset(36),    Root Port address 8
  Offset(40),     RPA9, 32, // Offset(40),    Root Port address 9
  Offset(44),     RPAA, 32, // Offset(44),    Root Port address 10
  Offset(48),     RPAB, 32, // Offset(48),    Root Port address 11
  Offset(52),     RPAC, 32, // Offset(52),    Root Port address 12
  Offset(56),     RPAD, 32, // Offset(56),    Root Port address 13
  Offset(60),     RPAE, 32, // Offset(60),    Root Port address 14
  Offset(64),     RPAF, 32, // Offset(64),    Root Port address 15
  Offset(68),     RPAG, 32, // Offset(68),    Root Port address 16
  Offset(72),     RPAH, 32, // Offset(72),    Root Port address 17
  Offset(76),     RPAI, 32, // Offset(76),    Root Port address 18
  Offset(80),     RPAJ, 32, // Offset(80),    Root Port address 19
  Offset(84),     RPAK, 32, // Offset(84),    Root Port address 20
  Offset(88),     NHLA, 32, // Offset(88),    HD-Audio NHLT ACPI address
  Offset(92),     NHLL, 32, // Offset(92),    HD-Audio NHLT ACPI length
  Offset(96),     ADFM, 32, // Offset(96),    HD-Audio DSP Feature Mask
  Offset(100),    SBRG, 32, // Offset(100),   SBREG_BAR
  Offset(104),    GPEM, 32, // Offset(104),   GPP_X to GPE_DWX mapping
  Offset(108),    PML1, 16, // Offset(108),   PCIE LTR max snoop Latency 1
  Offset(110),    PML2, 16, // Offset(110),   PCIE LTR max snoop Latency 2
  Offset(112),    PML3, 16, // Offset(112),   PCIE LTR max snoop Latency 3
  Offset(114),    PML4, 16, // Offset(114),   PCIE LTR max snoop Latency 4
  Offset(116),    PML5, 16, // Offset(116),   PCIE LTR max snoop Latency 5
  Offset(118),    PML6, 16, // Offset(118),   PCIE LTR max snoop Latency 6
  Offset(120),    PML7, 16, // Offset(120),   PCIE LTR max snoop Latency 7
  Offset(122),    PML8, 16, // Offset(122),   PCIE LTR max snoop Latency 8
  Offset(124),    PML9, 16, // Offset(124),   PCIE LTR max snoop Latency 9
  Offset(126),    PMLA, 16, // Offset(126),   PCIE LTR max snoop Latency 10
  Offset(128),    PMLB, 16, // Offset(128),   PCIE LTR max snoop Latency 11
  Offset(130),    PMLC, 16, // Offset(130),   PCIE LTR max snoop Latency 12
  Offset(132),    PMLD, 16, // Offset(132),   PCIE LTR max snoop Latency 13
  Offset(134),    PMLE, 16, // Offset(134),   PCIE LTR max snoop Latency 14
  Offset(136),    PMLF, 16, // Offset(136),   PCIE LTR max snoop Latency 15
  Offset(138),    PMLG, 16, // Offset(138),   PCIE LTR max snoop Latency 16
  Offset(140),    PMLH, 16, // Offset(140),   PCIE LTR max snoop Latency 17
  Offset(142),    PMLI, 16, // Offset(142),   PCIE LTR max snoop Latency 18
  Offset(144),    PMLJ, 16, // Offset(144),   PCIE LTR max snoop Latency 19
  Offset(146),    PMLK, 16, // Offset(146),   PCIE LTR max snoop Latency 20
  Offset(148),    PNL1, 16, // Offset(148),   PCIE LTR max no snoop Latency 1
  Offset(150),    PNL2, 16, // Offset(150),   PCIE LTR max no snoop Latency 2
  Offset(152),    PNL3, 16, // Offset(152),   PCIE LTR max no snoop Latency 3
  Offset(154),    PNL4, 16, // Offset(154),   PCIE LTR max no snoop Latency 4
  Offset(156),    PNL5, 16, // Offset(156),   PCIE LTR max no snoop Latency 5
  Offset(158),    PNL6, 16, // Offset(158),   PCIE LTR max no snoop Latency 6
  Offset(160),    PNL7, 16, // Offset(160),   PCIE LTR max no snoop Latency 7
  Offset(162),    PNL8, 16, // Offset(162),   PCIE LTR max no snoop Latency 8
  Offset(164),    PNL9, 16, // Offset(164),   PCIE LTR max no snoop Latency 9
  Offset(166),    PNLA, 16, // Offset(166),   PCIE LTR max no snoop Latency 10
  Offset(168),    PNLB, 16, // Offset(168),   PCIE LTR max no snoop Latency 11
  Offset(170),    PNLC, 16, // Offset(170),   PCIE LTR max no snoop Latency 12
  Offset(172),    PNLD, 16, // Offset(172),   PCIE LTR max no snoop Latency 13
  Offset(174),    PNLE, 16, // Offset(174),   PCIE LTR max no snoop Latency 14
  Offset(176),    PNLF, 16, // Offset(176),   PCIE LTR max no snoop Latency 15
  Offset(178),    PNLG, 16, // Offset(178),   PCIE LTR max no snoop Latency 16
  Offset(180),    PNLH, 16, // Offset(180),   PCIE LTR max no snoop Latency 17
  Offset(182),    PNLI, 16, // Offset(182),   PCIE LTR max no snoop Latency 18
  Offset(184),    PNLJ, 16, // Offset(184),   PCIE LTR max no snoop Latency 19
  Offset(186),    PNLK, 16, // Offset(186),   PCIE LTR max no snoop Latency 20
  Offset(188),    U0C0, 32, // Offset(188),   SerialIo Hidden UART0 BAR 0
  Offset(192),    U1C0, 32, // Offset(192),   SerialIo Hidden UART1 BAR 0
  Offset(196),    ADPM, 32, // Offset(196),   HD-Audio DSP Post-Processing Module Mask
  Offset(200),    XHPC, 8,  // Offset(200),   Number of HighSpeed ports implemented in XHCI controller
  Offset(201),    XRPC, 8,  // Offset(201),   Number of USBR ports implemented in XHCI controller
  Offset(202),    XSPC, 8,  // Offset(202),   Number of SuperSpeed ports implemented in XHCI controller
  Offset(203),    XSPA, 8,  // Offset(203),   Address of 1st SuperSpeed port
  Offset(204),    HPTB, 32, // Offset(204),   HPET base address
  Offset(208),    HPTE, 8,  // Offset(208),   HPET enable
  //110-bytes large SerialIo block
  Offset(209),    SMD0, 8,  // Offset(209),   SerialIo controller 0 (sdma) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(210),    SMD1, 8,  // Offset(210),   SerialIo controller 1 (i2c0) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(211),    SMD2, 8,  // Offset(211),   SerialIo controller 2 (i2c1) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(212),    SMD3, 8,  // Offset(212),   SerialIo controller 3 (spi0) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(213),    SMD4, 8,  // Offset(213),   SerialIo controller 4 (spi1) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(214),    SMD5, 8,  // Offset(214),   SerialIo controller 5 (ua00) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(215),    SMD6, 8,  // Offset(215),   SerialIo controller 6 (ua01) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(216),    SMD7, 8,  // Offset(216),   SerialIo controller 7 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(217),    SMD8, 8,  // Offset(217),   SerialIo controller 8 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(218),    SMD9, 8,  // Offset(218),   SerialIo controller 9 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(219),    SMDA, 8,  // Offset(219),   SerialIo controller A (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  Offset(220),    SIR0, 8,  // Offset(220),   SerialIo controller 0 (sdma) irq number
  Offset(221),    SIR1, 8,  // Offset(221),   SerialIo controller 1 (i2c0) irq number
  Offset(222),    SIR2, 8,  // Offset(222),   SerialIo controller 2 (i2c1) irq number
  Offset(223),    SIR3, 8,  // Offset(223),   SerialIo controller 3 (spi0) irq number
  Offset(224),    SIR4, 8,  // Offset(224),   SerialIo controller 4 (spi1) irq number
  Offset(225),    SIR5, 8,  // Offset(225),   SerialIo controller 5 (ua00) irq number
  Offset(226),    SIR6, 8,  // Offset(226),   SerialIo controller 6 (ua01) irq number
  Offset(227),    SIR7, 8,  // Offset(227),   SerialIo controller 7 (shdc) irq number
  Offset(228),    SIR8, 8,  // Offset(228),   SerialIo controller 8 (shdc) irq number
  Offset(229),    SIR9, 8,  // Offset(229),   SerialIo controller 9 (shdc) irq number
  Offset(230),    SIRA, 8,  // Offset(230),   SerialIo controller A (shdc) irq number
  Offset(231),    SB00, 32, // Offset(231),   SerialIo controller 0 (sdma) BAR0
  Offset(235),    SB01, 32, // Offset(235),   SerialIo controller 1 (i2c0) BAR0
  Offset(239),    SB02, 32, // Offset(239),   SerialIo controller 2 (i2c1) BAR0
  Offset(243),    SB03, 32, // Offset(243),   SerialIo controller 3 (spi0) BAR0
  Offset(247),    SB04, 32, // Offset(247),   SerialIo controller 4 (spi1) BAR0
  Offset(251),    SB05, 32, // Offset(251),   SerialIo controller 5 (ua00) BAR0
  Offset(255),    SB06, 32, // Offset(255),   SerialIo controller 6 (ua01) BAR0
  Offset(259),    SB07, 32, // Offset(259),   SerialIo controller 7 (shdc) BAR0
  Offset(263),    SB08, 32, // Offset(263),   SerialIo controller 8 (shdc) BAR0
  Offset(267),    SB09, 32, // Offset(267),   SerialIo controller 9 (shdc) BAR0
  Offset(271),    SB0A, 32, // Offset(271),   SerialIo controller A (shdc) BAR0
  Offset(275),    SB10, 32, // Offset(275),   SerialIo controller 0 (sdma) BAR1
  Offset(279),    SB11, 32, // Offset(279),   SerialIo controller 1 (i2c0) BAR1
  Offset(283),    SB12, 32, // Offset(283),   SerialIo controller 2 (i2c1) BAR1
  Offset(287),    SB13, 32, // Offset(287),   SerialIo controller 3 (spi0) BAR1
  Offset(291),    SB14, 32, // Offset(291),   SerialIo controller 4 (spi1) BAR1
  Offset(295),    SB15, 32, // Offset(295),   SerialIo controller 5 (ua00) BAR1
  Offset(299),    SB16, 32, // Offset(299),   SerialIo controller 6 (ua01) BAR1
  Offset(303),    SB17, 32, // Offset(303),   SerialIo controller 7 (shdc) BAR1
  Offset(307),    SB18, 32, // Offset(307),   SerialIo controller 8 (shdc) BAR1
  Offset(311),    SB19, 32, // Offset(311),   SerialIo controller 9 (shdc) BAR1
  Offset(315),    SB1A, 32, // Offset(315),   SerialIo controller A (shdc) BAR1
  //end of SerialIo block
  Offset(319),    GPEN, 8,  // Offset(319),   GPIO enabled
  Offset(320),    SGIR, 8,  // Offset(320),   GPIO IRQ
  Offset(321),    NIT1, 8,  // Offset(321),   RST PCIe Storage Cycle Router#1 Interface Type
  Offset(322),    NIT2, 8,  // Offset(322),   RST PCIe Storage Cycle Router#2 Interface Type
  Offset(323),    NIT3, 8,  // Offset(323),   RST PCIe Storage Cycle Router#3 Interface Type
  Offset(324),    NPM1, 8,  // Offset(324),   RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
  Offset(325),    NPM2, 8,  // Offset(325),   RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
  Offset(326),    NPM3, 8,  // Offset(326),   RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  Offset(327),    NPC1, 8,  // Offset(327),   RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
  Offset(328),    NPC2, 8,  // Offset(328),   RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
  Offset(329),    NPC3, 8,  // Offset(329),   RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  Offset(330),    NL11, 16, // Offset(330),   RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
  Offset(332),    NL12, 16, // Offset(332),   RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
  Offset(334),    NL13, 16, // Offset(334),   RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  Offset(336),    ND21, 8,  // Offset(336),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
  Offset(337),    ND22, 8,  // Offset(337),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
  Offset(338),    ND23, 8,  // Offset(338),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  Offset(339),    ND11, 32, // Offset(339),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
  Offset(343),    ND12, 32, // Offset(343),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
  Offset(347),    ND13, 32, // Offset(347),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  Offset(351),    NLR1, 16, // Offset(351),   RST PCIe Storage Cycle Router#1 LTR Capability Pointer
  Offset(353),    NLR2, 16, // Offset(353),   RST PCIe Storage Cycle Router#2 LTR Capability Pointer
  Offset(355),    NLR3, 16, // Offset(355),   RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  Offset(357),    NLD1, 32, // Offset(357),   RST PCIe Storage Cycle Router#1 Endpoint LTR Data
  Offset(361),    NLD2, 32, // Offset(361),   RST PCIe Storage Cycle Router#2 Endpoint LTR Data
  Offset(365),    NLD3, 32, // Offset(365),   RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  Offset(369),    NEA1, 16, // Offset(369),   RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
  Offset(371),    NEA2, 16, // Offset(371),   RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
  Offset(373),    NEA3, 16, // Offset(373),   RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  Offset(375),    NEB1, 16, // Offset(375),   RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
  Offset(377),    NEB2, 16, // Offset(377),   RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
  Offset(379),    NEB3, 16, // Offset(379),   RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  Offset(381),    NEC1, 16, // Offset(381),   RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
  Offset(383),    NEC2, 16, // Offset(383),   RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
  Offset(385),    NEC3, 16, // Offset(385),   RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  Offset(387),    NRA1, 16, // Offset(387),   RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
  Offset(389),    NRA2, 16, // Offset(389),   RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
  Offset(391),    NRA3, 16, // Offset(391),   RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  Offset(393),    NMB1, 32, // Offset(393),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
  Offset(397),    NMB2, 32, // Offset(397),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
  Offset(401),    NMB3, 32, // Offset(401),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  Offset(405),    NMV1, 32, // Offset(405),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
  Offset(409),    NMV2, 32, // Offset(409),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
  Offset(413),    NMV3, 32, // Offset(413),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  Offset(417),    NPB1, 32, // Offset(417),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
  Offset(421),    NPB2, 32, // Offset(421),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
  Offset(425),    NPB3, 32, // Offset(425),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  Offset(429),    NPV1, 32, // Offset(429),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
  Offset(433),    NPV2, 32, // Offset(433),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
  Offset(437),    NPV3, 32, // Offset(437),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  Offset(441),    SDME, 8,  // Offset(441),   SCS SDIO Controller Mode (0: disabled, 1: pci, 2: acpi)
  Offset(442),    SDIR, 8,  // Offset(442),   SCS SDIO Controller interrupt number
  Offset(443),    SDB0, 32, // Offset(443),   SCS SDIO controller BAR0
  Offset(447),    SDB1, 32, // Offset(447),   SCS SDIO controller BAR1
  Offset(451),        , 8,  // Offset(451),   Flag indicating Exit Boot Service, to inform SMM
  Offset(452),    SXRB, 32, // Offset(452),   Sx handler reserved MMIO base
  Offset(456),    SXRS, 32, // Offset(456),   Sx handler reserved MMIO size
  Offset(460),    CIOE, 8,  // Offset(460),   Cio2 Device Enabled as ACPI device
  Offset(461),    CIOI, 8,  // Offset(461),   Cio2 Interrupt Number
  Offset(462),    TAEN, 8,  // Offset(462),   Thermal Device Acpi mode enabled
  Offset(463),    TIRQ, 8,  // Offset(463),   Thermal Device IRQ number
  Offset(464),    XWMB, 64, // Offset(464),   XHCI reserved base addresss
       
  }
