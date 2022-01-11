//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file SbChipsetPpi.h
    This header file contains the PPI definition for the SB.
*/

#ifndef __AMI_SB_CHIPSET_PPI_H__
#define __AMI_SB_CHIPSET_PPI_H__

/**** PORTING REQUIRED ****
  AMI_GPIO_INIT_TABLE_STRUCT is optional for SbChipsetPpi.h.
  Chipset porting owner can change it per your GPIO porting need.
****/
typedef struct _AMI_GPIO_INIT_TABLE_STRUCT  AMI_GPIO_INIT_TABLE_STRUCT;

struct _AMI_GPIO_INIT_TABLE_STRUCT {
    UINT16                      GpioNo;
    UINT16                      GpioCfg;
};


#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
