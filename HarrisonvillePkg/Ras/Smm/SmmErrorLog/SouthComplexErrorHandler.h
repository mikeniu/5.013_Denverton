/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SouthFabricErrorHandler.h

Abstract:


------------------------------------------------------------------*/

#ifndef _SOUTH_FABRIC_ERROR_HANDLER_H
#define _SOUTH_FABRIC_ERROR_HANDLER_H

#define  GLREG_BUS                           0x00     //Global Error register Bus number
#define  GLREG_DEVICE                        0x04     //Global Error register Device number
#define  GLREG_FUNC                          0x00     //Global Error register function number

//Global Correctable Error Status Register
#define  R_GLREG_GCOERRSTS                   0x200
#define    B_RCEC_CO_STS                     BIT0
#define    B_AUNIT_CO_STS                    BIT1
#define    B_BUNIT_CO_STS                    BIT2
#define    B_DUNIT0_CO_STS                   BIT3
#define    B_DUNIT1_CO_STS                   BIT4
#define    B_PCIERP0_CO_STS                  BIT5
#define    B_PCIERP1_CO_STS                  BIT6
#define    B_PCIERP2_CO_STS                  BIT7
#define    B_PCIERP3_CO_STS                  BIT8
#define    B_PCIERP4_CO_STS                  BIT9
#define    B_PCIERP5_CO_STS                  BIT10
#define    B_PCIERP6_CO_STS                  BIT11
#define    B_PCIERP7_CO_STS                  BIT12
#define    B_VRP0_CO_STS                     BIT13
#define    B_VRP1_CO_STS                     BIT14
#define    B_VRP2_CO_STS                     BIT15

//Global Non-Fatal Error Status Register
#define  R_GLREG_GNFERRSTS                   0x204
#define    B_RCEC_NF_STS                     BIT0
#define    B_AUNIT_NF_STS                    BIT1
#define    B_BUNIT_NF_STS                    BIT2
#define    B_DUNIT0_NF_STS                   BIT3
#define    B_DUNIT1_NF_STS                   BIT4
#define    B_PCIERP0_NF_STS                  BIT5
#define    B_PCIERP1_NF_STS                  BIT6
#define    B_PCIERP2_NF_STS                  BIT7
#define    B_PCIERP3_NF_STS                  BIT8
#define    B_PCIERP4_NF_STS                  BIT9
#define    B_PCIERP5_NF_STS                  BIT10
#define    B_PCIERP6_NF_STS                  BIT11
#define    B_PCIERP7_NF_STS                  BIT12
#define    B_VRP0_NF_STS                     BIT13
#define    B_VRP1_NF_STS                     BIT14
#define    B_VRP2_NF_STS                     BIT15

//Global Fatal Error Status Register
#define  R_GLREG_GFAERRSTS                   0x208
#define    B_RCEC_FA_STS                     BIT0
#define    B_AUNIT_FA_STS                    BIT1
#define    B_BUNIT_FA_STS                    BIT2
#define    B_DUNIT0_FA_STS                   BIT3
#define    B_DUNIT1_FA_STS                   BIT4
#define    B_PCIERP0_FA_STS                  BIT5
#define    B_PCIERP1_FA_STS                  BIT6
#define    B_PCIERP2_FA_STS                  BIT7
#define    B_PCIERP3_FA_STS                  BIT8
#define    B_PCIERP4_FA_STS                  BIT9
#define    B_PCIERP5_FA_STS                  BIT10
#define    B_PCIERP6_FA_STS                  BIT11
#define    B_PCIERP7_FA_STS                  BIT12
#define    B_VRP0_FA_STS                     BIT13
#define    B_VRP1_FA_STS                     BIT14
#define    B_VRP2_FA_STS                     BIT15
#define    B_SATA0_FA_STS                    BIT16
#define    B_SATA1_FA_STS                    BIT17
#define    B_USB_FA_STS                      BIT18
#define    B_LPC_FA_STS                      BIT19
#define    B_UART_FA_STS                     BIT21

//Global Error Mask Register
#define  R_GLREG_GERRMSK                     0x20C
#define    B_RCEC_MSK                        BIT0
#define    B_AUNIT_MSK                       BIT1
#define    B_BUNIT_MSK                       BIT2
#define    B_DUNIT0_MSK                      BIT3
#define    B_DUNIT1_MSK                      BIT4
#define    B_PCIERP0_MSK                     BIT5
#define    B_PCIERP1_MSK                     BIT6
#define    B_PCIERP2_MSK                     BIT7
#define    B_PCIERP3_MSK                     BIT8
#define    B_PCIERP4_MSK                     BIT9
#define    B_PCIERP5_MSK                     BIT10
#define    B_PCIERP6_MSK                     BIT11
#define    B_PCIERP7_MSK                     BIT12
#define    B_VRP0_MSK                        BIT13
#define    B_VRP1_MSK                        BIT14
#define    B_VRP2_MSK                        BIT15
#define    B_SATA0_MSK                       BIT16
#define    B_SATA1_MSK                       BIT17
#define    B_USB_MSK                         BIT18
#define    B_LPC_MSK                         BIT19
#define    B_SMBUS_MSK                       BIT20
#define    B_HSUART_MSK                      BIT21
#define    B_SPIESPI_MSK                     BIT22
#define    B_MSK_ALL                         0x7FFFFF

//Global System Event Status Register
#define  R_GLREG_GSYSEVTSTS                  0x248
#define    B_CO_SYSEV_STS                    BIT0
#define    B_NF_SYSEV_STS                    BIT1
#define    B_FA_SYSEV_STS                    BIT2

//Global System Event Mask Register
#define  R_GLREG_GSYSEVTMSK                  0x24C
#define    B_CO_SYSEV_M                      BIT0
#define    B_NF_SYSEV_M                      BIT1
#define    B_FA_SYSEV_M                      BIT2

//Global System Event Map Register
#define  R_GLREG_GSYSEVTMAP                  0x250
#define    B_CO_SYSEV_MAP_NMI                BIT1       //Generate NMI
#define    B_NF_SYSEV_MAP_NMI                BIT3       //Generate NMI
#define    B_FA_SYSEV_MAP_NMI                BIT5       //Generate NMI
#define    B_CO_SYSEV_MAP_SMI                BIT0       //Generate SMI
#define    B_NF_SYSEV_MAP_SMI                BIT2       //Generate SMI
#define    B_FA_SYSEV_MAP_SMI                BIT4       //Generate SMI
#define    B_CO_SYSEV_MAP_NE                 0x00       //No event generation
#define    B_NF_SYSEV_MAP_NE                 0x00       //No event generation
#define    B_FA_SYSEV_MAP_NE                 0x00       //No event generation

//Error Pin Status Register
#define  R_GLREG_ERRPINSTS                   0x258

#define  R_GLREG_ERRUNCSTS                   0x280
#define    B_IEH_CFGREGP                     BIT0
#define    B_P2SB_CMDP                       BIT1
#define    B_P2SB_DATAP                      BIT2
#define    B_P2SB_NDT                        BIT3
#define    B_PSF0_CMDP                       BIT4
#define    B_PSF0_UC                         BIT5
#define    B_PSF0_UR                         BIT6
#define    B_PSF1_CMDP                       BIT7
#define    B_PSF1_UC                         BIT8
#define    B_PSF1_UR                         BIT9
#define    B_PSF2_CMDP                       BIT10
#define    B_PSF2_UC                         BIT11
#define    B_PSF2_UR                         BIT12
#define    B_PSF3_CMDP                       BIT13
#define    B_PSF3_UC                         BIT14
#define    B_PSF3_UR                         BIT15
#define    B_PSFS0_CMDP                      BIT16
#define    B_PSFS0_UC                        BIT17
#define    B_PSFS0_UR                        BIT18
#define    B_PSFS1_CMDP                      BIT19
#define    B_PSFS1_UC                        BIT20
#define    B_PSFS1_UR                        BIT21
#define    B_PSFCSME_CMDP                    BIT22
#define    B_PSFCSME_UC                      BIT23
#define    B_PSFCSME_UR                      BIT24
#define    B_PSFIE_CMDP                      BIT25
#define    B_PSFIE_UC                        BIT26
#define    B_PSFIE_UR                        BIT27

#define  RCEC_BUS                            0x00     //Root complex event collector Bus number
#define  RCEC_DEVICE                         0x05     //Root complex event collector Device number
#define  RCEC_FUNC                           0x00     //Root complex event collector function number

#define  R_RCEC_PCISTS                       0x06
#define  R_RCEC_DEVCTL                       0x48
#define    B_RCEC_DEVCTL_CERE                BIT0
#define    B_RCEC_DEVCTL_NFERE               BIT1
#define    B_RCEC_DEVCTL_FERE                BIT2
#define    B_RCEC_DEVCTL_UREE                BIT3
#define  R_RCEC_DEVSTS                       0x4A
#define  R_RCEC_ROOTCTL                      0x5C
#define    B_RCEC_ROOTCTL_SECEE              BIT0
#define    B_RCEC_ROOTCTL_SENFEE             BIT1
#define    B_RCEC_ROOTCTL_SEFEE              BIT2
#define  R_RCEC_ERRUNCSTS                    0x104
#define    B_RCEC_ERRUNCSTS_UIE              BIT22
#define  R_RCEC_ERRUNCMSK                    0x108
#define    B_RCEC_ERRUNCMSK_UCEM             BIT16
#define    B_RCEC_ERRUNCMSK_MTLPEM           BIT18
#define    B_RCEC_ERRUNCMSK_UREM             BIT20
#define    B_RCEC_ERRUNCMSK_UIEM             BIT22
#define  R_RCEC_ERRUNCSEV                    0x10C
#define    B_RCEC_ERRUNCSEV_UIES             BIT22
#define  R_RCEC_ERRCORSTS                    0x110
#define  R_RCEC_ERRCORMSK                    0x114
#define    B_RCEC_ERRCORMSK_ANFEM            BIT13
#define  R_RCEC_ROOTERRSTS                   0x130

#define  PCH_SMBUS_HOST_DEVICE_NUMBER        18
#define  PCH_SMBUS_HOST_FUNCTION_NUMBER      0
#define  R_PCH_SMBUS_HOST_PCISTS             0x06
#define  R_PCH_SMBUS_HOST_DEVCTL             0x48
#define  R_PCH_SMBUS_HOST_DEVSTS             0x4A
#define  R_PCH_SMBUS_HOST_AER_ERRUNCSTS      0x104
#define  R_PCH_SMBUS_HOST_AER_ERRCORSTS      0x110
#define  R_PCH_SMBUS_HOST_AER_DEFEATURE      0x12C
#define    B_PCH_SMBUS_HOST_AER_CPERR        BIT0
#define    B_PCH_SMBUS_HOST_AER_DPERR        BIT1

#define  RPX16_PCIE_BUS                      00
#define  RPX16_PCIE_DEVICE                   9
#define  RPX16_PCIE_FUNC                     00
#define  RPX16_PCIE_MAX_DEVICE               4

#define  RPX4_PCIE_BUS                       00
#define  RPX4_PCIE_DEVICE                    14
#define  RPX4_PCIE_FUNC                      00
#define  RPX4_PCIE_MAX_DEVICE                4

#define  vRP_PCIE_BUS                        00
#define  vRP_PCIE_FUNC                       00

#define  vRP0_PCIE_DEVICE                    06
#define  vRP1_PCIE_DEVICE                    22
#define  vRP2_PCIE_DEVICE                    23
#define  vRP_PCIE_MAX_DEVICE                 3

#define DEFAULT_PCI_BUS_NUMBER_PCH           0
#define PCI_DEVICE_NUMBER_PCH_PMC            31
#define PCI_FUNCTION_NUMBER_PCH_PMC          2
#define R_PCH_PMC_ACPI_BASE                  0x40
#define B_PCH_PMC_ACPI_BASE_BAR              0xFFFC
#define PCI_DEVICE_NUMBER_PCH_SMBUS          31
#define PCI_FUNCTION_NUMBER_PCH_SMBUS        4
#define R_PCH_SMBUS_TCOBASE                  0x50
#define B_PCH_SMBUS_TCOBASE_BAR              0x0000FFE0

#define PCH_TCO_BASE                         0x60
#define R_PCH_TCO1_STS                       0x04
#define R_PCH_TCO1_CNT                       0x08

#define R_PCH_NMI_SC                         0x61
#define   B_PCH_NMI_SC_SERR_NMI_STS          BIT7
#define   B_PCH_NMI_SC_IOCHK_NMI_STS         BIT6
#define   B_PCH_NMI_SC_TMR2_OUT_STS          BIT5
#define   B_PCH_NMI_SC_REF_TOGGLE            BIT4
#define   B_PCH_NMI_SC_IOCHK_NMI_EN          BIT3
#define   B_PCH_NMI_SC_PCI_SERR_EN           BIT2
#define   B_PCH_NMI_SC_SPKR_DAT_EN           BIT1
#define   B_PCH_NMI_SC_TIM_CNT2_EN           BIT0
#define R_PCH_NMI_EN                         0x70
#define R_PCH_NMI_EN_ALIAS                   0x74

#define R_PCH_SMI_EN                         0x30
#define   B_PCH_SMI_EN_GBL_SMI               BIT0
#define   B_PCH_SMI_EN_TCO                   BIT13
#define R_PCH_SMI_STS                        0x34

//PSF Register
#define R_PSF_GLOBAL_CONFIG                 0x4000
#define   B_ERR_MSI_EN                      BIT5
#define   B_EN_CMD_PARITY_DETECTION         BIT10 | BIT11

#define DNV_PCIE_RP_COUNT                   RPX16_PCIE_MAX_DEVICE + RPX4_PCIE_MAX_DEVICE + vRP_PCIE_MAX_DEVICE

#define PCH_SATA0_DEVICE_NUMBER             19
#define PCH_SATA1_DEVICE_NUMBER             20
#define PCH_SATA_FUNCTION_NUMBER            0
#define R_PCH_SATAGC                        0x9C
#define   B_PCH_SATAGC_CPEE                 BIT11
#define   B_PCH_SATAGC_DPPEE                BIT15
#define PCH_USB_DEVICE_NUMBER               21
#define PCH_USB_FUNCTION_NUMBER             0
#define R_PCH_USB_XHCC3                     0xFC
#define   B_PCH_USB_XHCC3_DDPC              BIT2
#define   B_PCH_USB_XHCC3_DCPC              BIT3
#define PCH_HSUART_DEVICE_NUMBER            26
#define PCH_HSUART_FUNCTION_NUMBER          0
#define R_PCH_HSUART_DEFEATURE              0x80
#define   B_PCH_HSUART_DEFEATURE_CPERR_MSK  BIT0
#define   B_PCH_HSUART_DEFEATURE_DPERR_MSK  BIT1
#define PCH_NCPM_DEVICE_NUMBER              0
#define PCH_NCPM_FUNCTION_NUMBER            0
#define R_PCH_NCPM_PFCGCIOSFPRIR            0x2C0
#define   B_PCH_NCPM_PFCGCIOSFPRIR_PARCHK   BIT16

//Unsupported Request
#define PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UC      0x07
//Unexpected Completion
#define PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UR      0x08
#define PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UC      0x09
#define PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UR      0x09
#define PLATFORM_NON_STANDARD_DEV_ERROR_P2SB_NDT    0x0A

typedef enum {
  PCIE_RP = 0,
  PCIE_vRP
}PCIE_RP_TYPE;

typedef struct {
  UINT8         Bus;
  UINT8         Dev;
  UINT8         Func;
  PCIE_RP_TYPE  PortType;
} DNV_PCIE_RP;

BOOLEAN
CheckForSystemError(
VOID
);

BOOLEAN
ElogSouthComplexErrorHandler (
VOID
);

EFI_STATUS
EnableAsyncSmiLogic (
VOID
);

VOID
ClearSouthComplexErrors(
VOID
);

VOID
ElogGenerateNmiNow (
VOID
);

VOID
ProgramSouthComplexErrorSignals(
VOID
);

#endif

