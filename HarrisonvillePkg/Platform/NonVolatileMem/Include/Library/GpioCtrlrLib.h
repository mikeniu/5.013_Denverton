/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             

@copyright
  Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement


Module Name:
  
  GpioCtrlrLib.c

Abstract:

  Do platform specific stage2 initializations. 

--*/

#ifndef GPIOCTRLRLIB_H_
#define GPIOCTRLRLIB_H_

UINTN SBREGbase();
#define gpioSOUTH_SBPORT (0xC5) 
#define gpioNORTH_SBPORT (0xC2) 

#define gpioSouthGpioBase() (SBREGbase() | (gpioSOUTH_SBPORT<<16))
#define gpioNorthGpioBase() (SBREGbase() | (gpioNORTH_SBPORT<<16))

typedef enum {
	//North_All_0 
	gpio_GBE0_SDP0=0,     gpio_GBE1_SDP0,      gpio_GBE0_SDP1,      gpio_GBE1_SDP1,
	gpio_GBE0_SDP2,       gpio_GBE1_SDP2,      gpio_GBE0_SDP3,      gpio_GBE1_SDP3,
	gpio_GBE2_LED0,       gpio_GBE2_LED1,      gpio_GBE0_I2C_CLK,   gpio_GBE0_I2C_DATA,
	gpio_GBE1_I2C_CLK,    gpio_GBE1_I2C_DATA,  gpio_SMB5_GBE_DATA,  gpio_SMB5_GBE_CLK,
	gpio_SMB5_GBE_ALRT_N, gpio_NCSI_CRS_DV,    gpio_NCSI_ARB_IN,    gpio_NCSI_TX_EN,
	gpio_NCSI_TXD0,       gpio_NCSI_TXD1,      gpio_NCSI_ARB_OUT,   gpio_GBE0_LED0,
	gpio_GBE0_LED1,       gpio_GBE1_LED0,      gpio_GBE1_LED1,      gpio_GPIO_0,
	gpio_PCIE_CLKREQ0_N,  gpio_PCIE_CLKREQ1_N, gpio_PCIE_CLKREQ2_N, gpio_PCIE_CLKREQ3_N,
	//North_All_1
	gpio_PCIE_CLKREQ4_N,  gpio_GPIO_1,         gpio_GPIO_2,         gpio_SVID_ALERT_N, 
	gpio_SVID_DATA,       gpio_SVID_CLK,       gpio_THERMTRIP_N,    gpio_PROCHOT_N,
	gpio_MEMHOT_N,
	//South_DFX_0: {OWN} South_DFX_0, South_DFX_1, South_DFX_2
	gpio_DFX_PORT_CLK0,   gpio_DFX_PORT_CLK1,   gpio_DFX_PORT0,      gpio_DFX_PORT1,
	gpio_DFX_PORT2,       gpio_DFX_PORT3,       gpio_DFX_PORT4,      gpio_DFX_PORT5,
	gpio_DFX_PORT6,       gpio_DFX_PORT7,       gpio_DFX_PORT8,      gpio_DFX_PORT9,
	gpio_DFX_PORT10,      gpio_DFX_PORT11,      gpio_DFX_PORT12,     gpio_DFX_PORT13,
	gpio_DFX_PORT14,      gpio_DFX_PORT15,
	//South_Group0_0: {OWN}South_Group0_0, South_Group0_1, South_Group0_2, South_Group0_3
	gpio_SMB3_CLTT_DATA,  gpio_SMB3_CLTT_CLK,   gpio_PCIE_CLKREQ5_N, gpio_PCIE_CLKREQ6_N, 
	gpio_PCIE_CLKREQ7_N,  gpio_UART0_RXD,       gpio_UART0_TXD,      gpio_CPU_RESET_N,
	gpio_NMI,             gpio_ERROR2_N,        gpio_ERROR1_N,       gpio_ERROR0_N, 
	gpio_IERR_N,          gpio_MCERR_N,         gpio_SMB0_LEG_CLK,   gpio_SMB0_LEG_DATA,
	gpio_SMB0_LEG_ALRT_N, gpio_SMB1_HOST_DATA,  gpio_SMB1_HOST_CLK,  gpio_SMB2_PECI_DATA,
	gpio_SMB2_PECI_CLK,   gpio_SMB4_CSME0_DATA, gpio_SMB4_CSME0_CLK, gpio_SMB4_CSME0_ALRT_N,
	gpio_USB_OC0_N,       gpio_FLEX_CLK_SE0,    gpio_FLEX_CLK_SE1,   gpio_SATA0_LED_N,
	gpio_SATA1_LED_N,     gpio_SATA_PDETECT0,   gpio_SATA_PDETECT1,  gpio_SATA0_SDOUT,
	//South_Group0_1: {OWN}South_Group0_4, South_Group0_5
	gpio_SATA1_SDOUT,     gpio_UART1_RXD,       gpio_UART1_TXD,      gpio_TCK,
	gpio_TRST_N,          gpio_TMS,             gpio_TDI,            gpio_TDO, 
	gpio_CX_PRDY_N,       gpio_CX_PREQ_N,       gpio_CTBTRIGINOUT,   gpio_CTBTRIGOUT,
	gpio_DFX_SPARE2,      gpio_DFX_SPARE3,      gpio_DFX_SPARE4,
	//South_Group1_0: {OWN}South_Group1_0, South_Group1_1, South_Group1_2, South_Group1_3
	gpio_SUSPWRDNACK,     gpio_PMU_SUSCLK,      gpio_PMU_AC_PRESENT, gpio_PMU_SLP_S45_N,
	gpio_PMU_SLP_S3_N,    gpio_PMU_WAKE_N,      gpio_PMU_PWRBTN_N,   gpio_PMU_RESETBUTTON_N,
	gpio_PMU_PLTRST_N,    gpio_SUS_STAT_N,      gpio_SLP_S0IX_N,     gpio_SPI_CS0_N,
	gpio_SPI_CS1_N,       gpio_SPI_MOSI_IO0,    gpio_SPI_MISO_IO1,   gpio_SPI_IO2,
	gpio_SPI_IO3,         gpio_SPI_CLK,         gpio_SPI_CLK_LOOPBK, gpio_ESPI_IO0,
	gpio_ESPI_IO1,        gpio_ESPI_IO2,        gpio_ESPI_IO3,       gpio_ESPI_CS0_N,
	gpio_ESPI_CLK,        gpio_ESPI_RST_N,      gpio_ESPI_ALRT0_N,   gpio_ESPI_CS1_N,
	gpio_ESPI_ALRT1_N,    gpio_ESPI_CLK_LOOPBK, gpio_EMMC_CMD,       gpio_EMMC_STROBE,
	//South_Group1_1: {OWN}South_Group1_4, South_Group1_5
	gpio_EMMC_CLK,        gpio_EMMC_D0,         gpio_EMMC_D1,        gpio_EMMC_D2,
	gpio_EMMC_D3,         gpio_EMMC_D4,         gpio_EMMC_D5,        gpio_EMMC_D6,
	gpio_EMMC_D7,         gpio_GPIO_3
} gpio_name;

#define gpioFirstSouthGpio   (gpio_DFX_PORT_CLK0)
#define gpioFirstNorthGpio   (gpio_GBE0_SDP0)
#define gpioInNorth(GPIO)    (GPIO<gpioFirstSouthGpio)
#define gpioNorthIndex(GPIO) (GPIO-gpioFirstNorthGpio)
#define gpioSouthIndex(GPIO) (GPIO-gpioFirstSouthGpio)
 
typedef enum {gpioNorth=0, gpioSouth} gpio_locations;
typedef enum {gpioGPIO=0, gpioNative1, gpioNative2, gpioNative3} gpio_modes;

//define goGpioLocation(gpio) ((gpio)? gpioNorth; gpioSouth)
//define goLocBase(Loc) ((Loc=gpioNorth)? gpioNorthGpioBase() : gpioSouthGpioBase())
#define gpioCtrlBase(GPIO) ( gpioInNorth(GPIO) ? gpioNorthGpioBase() : gpioSouthGpioBase())
#define goMaxNorthGRPs (2)
#define goMaxSouthGRPs (5)

#define PAD_OWN_OFF (0x20)
#define gpio_Own_grp_off(base, GRP) *((UINT32 *)(base+PAD_OWN_OFF+4*GRP))
//define gpio_Own_grp(Loc, GRP)           gpioOwnedGrp_off(goLocBase(Loc), GRP)

BOOLEAN gpioOwned(gpio_name GPIO);
BOOLEAN gpioOwned_off(UINTN LocationBase, gpio_name GPIO);

#define PAD_GPI_VWE_OFF (0x60)
#define gpioGPI_VWEgrp_off(base, GRP)  *((UINT32 *)(base+PAD_VWE_OFF+4*GRP))
//define gpioGPI_VWEgrp(Loc, GRP)           gpioGPI_VWEgrp_off(goLocBase(Loc), GRP)

BOOLEAN gpioGPI_VWE(gpio_name GPIO);
BOOLEAN gpioGPI_VWE_off(UINTN LocationBase, gpio_name GPIO);
void set_gpioGPI_VWE(UINTN LocationBase, gpio_name GPIO);
void clear_gpioGPI_VWE(UINTN LocationBase, gpio_name GPIO);


// CFGLOCK

//PADCFGLOCKTX

//HOSTSW_OWN

//GPI_IS

//GPI_IE

//GPI_GPE_STS

//GPI_GPE_EN

//GPI_SMI_STS
#define PAD_GPI_SMI_STS_OFF (0x180)
#define gpioGPI_SMI_STS_off(base, GRP)  *((UINT32 *)(base+PAD_GPI_SMI_STS_OFF+4*GRP))

BOOLEAN gpioSMI_STS(gpio_name GPIO);
void clear_gpioSMI_STS(gpio_name GPIO);

//GPI_SMI_EN
#define PAD_GPI_SMI_EN_OFF (0x1A0)
#define gpioGPI_SMI_EN_off(base, GRP)  *((UINT32 *)(base+PAD_GPI_SMI_EN_OFF+4*GRP))

BOOLEAN gpioSMI_EN(gpio_name GPIO);
void set_gpioSMI_EN(gpio_name GPIO);
void clear_gpioSMI_EN(gpio_name GPIO);

//GPI_NMI_STS

//GPI_NMI_EN


//PAD_CFG
#define gpioPadCfgOff   (0x400)
#define gpioPadCfgSize  (8)
#define PadMode(mode)   (((mode)&0x7)<<10)
#define PadModeClear    ~(PadMode(7))
typedef enum {gpioIn=01, gpioOut=10} gpio_direction;
#define PadDir(x)       (((x)&3)<<8)
#define PadDirClear     (~PadDir(3))
#define PadOut(x)       (((x)&1)<<0)
#define PadOutClear     (~PadOut(1))
#define PadInput        BIT1

UINTN gpioPadCfgBaseOf(gpio_name GPIO);
#define gpioClearSet(base,mask,value)     base = ((base) & (mask)) | (value)
#define gpioSetMode(GPIObase, mode)       gpioClearSet(*(volatile UINT32*)GPIObase, PadModeClear, PadMode(mode))
#define gpioSetDirection(GPIObase, dir)   gpioClearSet(*(volatile UINT32*)GPIObase, PadDirClear, PadDir(dir))
#define gpioSetOutput(GPIObase, level)    gpioClearSet(*(volatile UINT32*)GPIObase, PadOutClear, PadOut(level))
#define gpioGetInput(GPIObase)            (PadInput == ((*(volatile UINT32*)GPIObase) & PadInput))


#endif /* GPIOCTRLRLIB_H_ */
