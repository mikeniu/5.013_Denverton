//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Gpio.h

Abstract:

  Platform specific GPIO information 

--*/


#ifndef __GPIO_H__
#define __GPIO_H__

//Core well GPIO registers
#define GPIO_CGEN      0
#define GPIO_CGIO      4
#define GPIO_CGLV      8
#define GPIO_CGTPE   0xc
#define GPIO_CGTNE  0x10
#define GPIO_CGGPE  0x14
#define GPIO_CGSMI  0x18
#define GPIO_CGTS   0x1c
#define GPIO_CNMIEN 0x40

//Resume well GPIO registers
#define GPIO_RGEN   0x20
#define GPIO_RGIO   0x24
#define GPIO_RGLV   0x28
#define GPIO_RGTPE  0x2c
#define GPIO_RGTNE  0x30
#define GPIO_RGGPE  0x34
#define GPIO_RGSMI  0x38
#define GPIO_RGTS   0x3c
#define GPIO_RNMIEN 0x44

#define GPIO(bit) (1<<bit)    /*use in CGEN reg definition*/
#define GP_IN(bit) (1<<bit)   /*use in CGIO reg definition*/
#define GP_HIGH(bit) (1<<bit) /*use in LVL reg definition*/
#define GP_POS(bit) (1<<bit)  /*use in CGTPE reg definition*/
#define GP_NEG(bit) (1<<bit)  /*use in CGTNE reg definition*/
#define GP_TRG(bit) (1<<bit)  /*use in CGGPE reg definition*/
#define GP_SMI(bit) (1<<bit)  /*use in CGSMI reg definition*/
#define GP_NMI(bit) (1<<bit)  /*use in CNMIEN reg definition*/
#define GPIO_CORE_MASK (0x001ffffe) /*Core GPIOs [20:1]*/
#define GPIO_RESUME_MASK (0x000001ff) /*Core GPIOs [8:0]*/
#define GPIO_REG_SETTING(reg, mask, gpio) ( ((reg) & ~(mask)) | ((gpio) & (mask)) )

#define CGEN_VALUE(bitmap)  ((GPIO_CORE_MASK) & (bitmap))
#define RGEN_VALUE(bitmap) ((GPIO_RESUME_MASK) & (bitmap))
#define GPIO_GIO_VALUE(GEN_REG_VALUE, bitmap)   (GEN_REG_VALUE & (bitmap))
// This register settings only apply to input GPIOs for Core or Resume
#define GPIO_GLVL_VALUE(GIO_REG_VALUE, bitmap)  (~(GIO_REG_VALUE) & (bitmap))
// These register settings only apply to input GPIOs
#define GPIO_GTPE_VALUE(GIO_REG_VALUE, bitmap)  (GIO_REG_VALUE & (bitmap))
#define GPIO_GTNE_VALUE(GIO_REG_VALUE, bitmap)  (GIO_REG_VALUE & (bitmap))
#define GPIO_GGPE_VALUE(GIO_REG_VALUE, bitmap)  (GIO_REG_VALUE & (bitmap))
#define GPIO_GSMI_VALUE(GIO_REG_VALUE, bitmap)  (GIO_REG_VALUE & (bitmap))
#define GPIO_NMIEN_VALUE(GIO_REG_VALUE, bitmap) (GIO_REG_VALUE & (bitmap))


//evaluates true if bits outside the valid range are set
#define GPIO_GIO_INVALID(GEN_REG_VALUE, GIO_REG_VALUE)   ((~(CGEN_REG_VALUE) & CGIO_REG_VALUE)!=0)
//evaluates true if output bits are being set
#define PIO_INPUT_INVALID(GIO_REG_VALUE, REG_VALUE)   ((~(CGIO_REG_VALUE) & REG_VALUE)!=0)
//evaluates true if output bits are being set
#define GPIO_OUTPUT_INVALID(GIO_REG_VALUE, CGLVL_REG_VALUE)   ((CGIO_REG_VALUE & CGLVL_REG_VALUE)!=0)
typedef struct GPIO_Reg_Value{
 UINT16 Off;
 UINT32 Val;
 UINT32 Msk;
} GPIO_Reg_Value_Tuple;
#define MAX_GPIO_REG_SETUPS (16)
typedef GPIO_Reg_Value_Tuple GPIOSetupList[MAX_GPIO_REG_SETUPS]; /*8 core and 8 Resume*/
#define GPIO_LIST_END 0xffff,0xffffffff,0xffffffff

#define INDY_CARSON_CORE_GIOS \
  CGEN_VALUE(  GPIO(1)| GPIO(2)| GPIO(3)| GPIO(4)| GPIO(5)\
             | GPIO(6)| GPIO(7)| GPIO(8)| GPIO(9)|GPIO(10)\
             |GPIO(11)|GPIO(12)|GPIO(13)|GPIO(14)|GPIO(15)\
             |GPIO(16)|GPIO(17)|GPIO(18)|GPIO(19)|GPIO(20))
#define INDY_CARSON_CORE_GIO_DIRECTION \
  GPIO_GIO_VALUE( INDY_CARSON_CORE_GIOS, \
                 (GP_IN(1)| GP_IN(2)| GP_IN(3) | GP_IN(7)| GP_IN(8)| GP_IN(9)) )
#define INDY_CARSON_CORE_GIO_LEVEL \
  GPIO_GLVL_VALUE(INDY_CARSON_CORE_GIO_DIRECTION, (0))
#define INDY_CARSON_CORE_GIO_PEDGE \
  GPIO_GTPE_VALUE(INDY_CARSON_CORE_GIO_DIRECTION, (0))
#define INDY_CARSON_CORE_GIO_NEDGE \
  GPIO_GTNE_VALUE(INDY_CARSON_CORE_GIO_DIRECTION, (0))
#define INDY_CARSON_CORE_GIO_GPE \
  GPIO_GGPE_VALUE(INDY_CARSON_CORE_GIO_DIRECTION, (0))
#define INDY_CARSON_CORE_GIO_SMI \
  GPIO_GSMI_VALUE(INDY_CARSON_CORE_GIO_DIRECTION, (0))
#define INDY_CARSON_CORE_GIO_NMI \
  GPIO_NMIEN_VALUE(INDY_CARSON_CORE_GIO_DIRECTION, (0))

#define INDY_CARSON_RESUME_GIOS \
  RGEN_VALUE(  GPIO(1)| GPIO(2)| GPIO(3)| GPIO(4)| GPIO(5)\
             | GPIO(6)| GPIO(7)| GPIO(8) )
#define INDY_CARSON_RESUME_GIO_DIRECTION \
  GPIO_GIO_VALUE(INDY_CARSON_RESUME_GIOS, \
                 ( GP_IN(0)| GP_IN(2)| GP_IN(4) | GP_IN(5)| GP_IN(6)| GP_IN(8)) )

#define DBC_USB_SMI (12)
#define DOUBLECOVE_CORE_GIOS \
  CGEN_VALUE( GPIO(7)| GPIO(8)| GPIO(9)| GPIO(11)| GPIO(DBC_USB_SMI)| GPIO(13) )
#define DOUBLECOVE_CORE_GIO_DIRECTION  GPIO_GIO_VALUE( DOUBLECOVE_CORE_GIOS, \
         ( GP_IN(7)| GP_IN(8)| GP_IN(9) | GP_IN(11)| GP_IN(DBC_USB_SMI)| GP_IN(13)) )
#define DOUBLECOVE_CORE_GIO_PEDGE GPIO_GTPE_VALUE(DOUBLECOVE_CORE_GIO_DIRECTION, \
         (GP_POS(DBC_USB_SMI)))
#define DOUBLECOVE_CORE_GIO_NEDGE GPIO_GTNE_VALUE(DOUBLECOVE_CORE_GIO_DIRECTION, \
         (GP_NEG(DBC_USB_SMI)))
#define DOUBLECOVE_CORE_GIO_GPE   GPIO_GGPE_VALUE(DOUBLECOVE_CORE_GIO_DIRECTION, \
         (GP_TRG(DBC_USB_SMI)))
#define DOUBLECOVE_CORE_GIO_SMI   GPIO_GSMI_VALUE(DOUBLECOVE_CORE_GIO_DIRECTION, \
         (GP_SMI(DBC_USB_SMI)))
#define DOUBLECOVE_RESUME_GIOS \
  RGEN_VALUE( GPIO(1)| GPIO(2)| GPIO(7) )
#define DOUBLECOVE_RESUME_GIO_DIRECTION \
  GPIO_GIO_VALUE( DOUBLECOVE_RESUME_GIOS, ( GP_IN(2) ) )


#endif
