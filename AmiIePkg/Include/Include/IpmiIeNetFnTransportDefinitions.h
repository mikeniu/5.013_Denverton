//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiIeNetFnTransportDefinitions.h
    Definitions required for transport commands.

**/

//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  IpmiIeNetFnTransportDefinitions.h
  
Abstract:
  The file has all the definitions required for Transport commands
--*/

#ifndef _IPMIIENETFNTRANSPORTDEFINITIONS_H_
#define _IPMIIENETFNTRANSPORTDEFINITIONS_H_

//
// LUN definition
//
#ifndef IE_LUN
    #define IE_LUN     0x00
#endif


typedef enum {
  EfiSmReserved,
  EfiSmIpmb,
  EfiSmIcmb1_0,
  EfiSmIcmb0_9,
  EfiSm802_3_Lan,
  EfiSmRs_232,
  EfiSmOtherLan,
  EfiSmPciSmBus,
  EfiSmSmBus1_0,
  EfiSmSmBus2_0,
  EfiSmUsb1_x,
  EfiSmUsb2_x,
  EfiSmIe
} EFI_SM_CHANNEL_MEDIA_TYPE;

typedef struct {
  BOOLEAN IpAddressType;
  UINT16  IpPort;
  UINT8   IpAddress[16];
} EFI_SM_IP_ADDRESS;

typedef struct {
  UINT8 SlaveAddress;
  UINT8 LunAddress;
  UINT8 NetFunction;
  UINT8 ChannelAddress;
} EFI_SM_IPMI_ADDRESS;

typedef struct {
  UINT8 SerialPortNumber;
} EFI_SM_SERIAL_ADDRESS;

typedef union {
  EFI_SM_IP_ADDRESS     IpAddress;
  EFI_SM_IPMI_ADDRESS   IeAddress;
  EFI_SM_SERIAL_ADDRESS SerialAddress;
} EFI_SM_COM_ADDRESS_TYPE;

typedef struct {
  EFI_SM_CHANNEL_MEDIA_TYPE ChannelType;
  EFI_SM_COM_ADDRESS_TYPE   Address;
} EFI_SM_COM_ADDRESS;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
