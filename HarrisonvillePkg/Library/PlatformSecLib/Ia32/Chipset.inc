;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
; 
; Module Name:
;
;   Chipset.inc
;
; Abstract:
;
;   Chipset constants and macros
;
;------------------------------------------------------------------------------


;
; APIC register
;
APICID                        EQU 0FEE00020h

R_PCI_CMD                     EQU 04H
B_PCI_CMD_IOSE                EQU BIT0
B_PCI_CMD_MSE                 EQU BIT1
;
; PCH base addresses and register definitions.
;

ACPI_BASE_ADDRESS             EQU 01800h
  B_PCH_PMC_ACTL_ACPI_EN      EQU BIT7
  R_PCH_ACPI_PM1_CNT          EQU 004h
  V_PCH_ACPI_PM1_CNT_S5       EQU 01C00h
  R_PCH_TCO_STS               EQU 064h
  B_PCH_TCO_STS_SECOND_TO     EQU BIT17
  R_PCH_TCO_CNT               EQU 068h
  B_PCH_TCO_CNT_TMR_HLT       EQU BIT11
SMBUS_CFG_BASE                EQU ((1Fh * 8 + 04h) * 1000h + 0000h + CPU_HEC_BASE)
  R_PCH_SMBUS_HOSTC           EQU 040h
SMBUS_BASE_ADDRESS            EQU 0EFA0h
  B_PCH_SMBUS_PCICMD_IOSE     EQU BIT0
  R_PCH_SMBUS_AUXC            EQU 00Dh
SPI_CFG_BASE                  EQU ((1Fh * 8 + 05h) * 1000h + 0000h + CPU_HEC_BASE)
  R_PCH_SPI_BCR               EQU 0DCh
SPI_BASE_ADDRESS              EQU 0FED01000h
PMC_CFG_BASE                  EQU ((1Fh * 8 + 02h) * 1000h + 0000h + CPU_HEC_BASE)
  R_PCH_PMC_GEN_PMCON_A       EQU 0A0h
  R_PCH_PMC_GEN_PMCON_B       EQU 0A4h
  B_PCH_PMC_GEN_PMCON_B_PWR_FLR EQU BIT1
  R_PCH_PMC_ETR3              EQU 0ACh
  B_PCH_PMC_ETR3_CF9GR        EQU BIT20
PMC_BASE_ADDRESS              EQU 0FED03000h
  B_PCH_PMC_ACTL_BASE_EN      EQU BIT8
PUNIT_BASE_ADDRESS            EQU 0FED06000h
  B_PCH_LPC_PUNIT_BASE_EN     EQU BIT1
P2SB_CFG_BASE                 EQU ((1Fh * 8 + 01h) * 1000h + 0000h + CPU_HEC_BASE)
SBREG_BASE_ADDRESS            EQU 0FD000000h
MCH_BASE_ADDRESS              EQU 0FED10000h
  B_MCH_BASE_ADDRESS_EN       EQU BIT0
  R_MCH_BMCMODE               EQU 06E48h
  B_MCH_BMCMODE_MC_SIGNAL     EQU BIT0


;
;B_Unit Registers
;
BUNIT_BMISC	                  EQU 6800h
  B_BMISC_RESDRAM	            EQU 01h   	;Bit 0 - When this bit is set, reads targeting E-segment are routed to DRAM.
  B_BMISC_RFSDRAM	            EQU 02h   	;Bit 1 - When this bit is set, reads targeting F-segment are routed to DRAM.

;
; HPET compare register
;
HPET_COMP_1                   EQU 0FED00108h
HPET_COMP_2                   EQU 0FED0010Ch
HPET_COMP_3                   EQU 0FED00128h
HPET_COMP_4                   EQU 0FED00148h
HPTC_AE                       EQU BIT7
;
; MCH PCIe base address
;
CPU_HEC_BASE                  EQU 0E0000000h      ; Must be X0000000
CPU_HEC_SIZE                  EQU 000000000h      ; 256M
CPU_HEC_EN                    EQU 000000001h      ; Enable
CPU0_HEC_PCI_ADDR             EQU 080FF0150h
CPU1_HEC_PCI_ADDR             EQU 080FE0150h

;
;C_Unit Registers
;
R_CUNIT_MANID                   EQU ((0h * 8 + 00h) * 1000h + 00f4h + CPU_HEC_BASE)
  CUNIT_MANID_MSID_MASK         EQU 0ffh
  V_CUNIT_MANID_MSID_B_STEPPING EQU 01h           ; B stepping set to 1 for now, should be updated when known

;
; PCI registers
;
R_MCH_BASE                    EQU ((0h * 8 + 00h) * 1000h + 0048h + CPU_HEC_BASE)
R_PCH_LPC_ACPI_BASE           EQU ((1Fh * 8 + 02h) * 1000h + 0040h + CPU_HEC_BASE)
R_PCH_LPC_PMC_CFG             EQU ((1Fh * 8 + 02h) * 1000h + 0000h + CPU_HEC_BASE)
R_PCH_LPC_PMC_ACTL            EQU ((1Fh * 8 + 02h) * 1000h + 0044h + CPU_HEC_BASE)
R_PCH_LPC_PMC_BASE            EQU ((1Fh * 8 + 02h) * 1000h + 0048h + CPU_HEC_BASE)
R_PCH_LPC_SPI_BASE            EQU ((1Fh * 8 + 05h) * 1000h + 0010h + CPU_HEC_BASE)
R_PCH_SMBUS_CMD               EQU ((1Fh * 8 + 04h) * 1000h + 0004h + CPU_HEC_BASE)
R_PCH_SMBUS_BASE              EQU ((1Fh * 8 + 04h) * 1000h + 0020h + CPU_HEC_BASE)
R_P2SB_SBREG_BASE             EQU ((1Fh * 8 + 01h) * 1000h + 0010h + CPU_HEC_BASE)
R_PCH_P2SB_HPTC               EQU ((1Fh * 8 + 01h) * 1000h + 0060h + CPU_HEC_BASE)  ;zchai HPTC config register. B0:D31:F1 0x60x

SYRE_CPURST                   EQU 14

;
; PCIEXBAR constants for enable in bit [0]
;
ENABLE                        EQU 1

;
; PCIEXBAR constants for size in bit [2:1]
;
PCIEXBAR_64MB                 EQU 010b
PCIEXBAR_128MB                EQU 001b
PCIEXBAR_256MB                EQU 000b

MMCFG_BASE                    EQU CPU_HEC_BASE    ; 4GB-128MB
