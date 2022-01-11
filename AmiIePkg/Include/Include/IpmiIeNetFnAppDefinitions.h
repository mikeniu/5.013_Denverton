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

/** @file IpmiIeNetFnAppDefinitions.h
    Definitions required for Application commands.

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

/** @file IpmiIeNetFnOtherDefinitions.h
    Definitions required for OEM commands.

**/

#ifndef _IPMIIENETFNAPPDEFINITIONS_H_
#define _IPMIIENETFNAPPDEFINITIONS_H_

//
// LUN definition
//
#ifndef IE_LUN 
    #define IE_LUN     0x00
#endif 

//
// Net function definition for App command
//
#define EFI_SM_NETFN_APP  0x06

/*----------------------------------------------------------------------------------------
    Definitions for Get Device ID command
----------------------------------------------------------------------------------------*/
#define EFI_APP_GET_DEVICE_ID 0x1

//
//  Constants and Structure definitions for "Get Device ID" command to follow here
//
#pragma pack(1)
typedef struct {
  UINT8   DeviceId;
  UINT8   DeviceRevision : 4;
  UINT8   Reserved : 3;
  UINT8   DeviceSdr : 1;
  UINT8   MajorFirmwareRev : 7;
  UINT8   UpdateMode : 1;
  UINT8   MinorFirmwareRev;
  UINT8   SpecificationVersion;
  UINT8   SensorDeviceSupport : 1;
  UINT8   SdrRepositorySupport : 1;
  UINT8   SelDeviceSupport : 1;
  UINT8   FruInventorySupport : 1;
  UINT8   IPMBMessageReceiver : 1;
  UINT8   IPMBMessageGenerator : 1;
  UINT8   BridgeSupport : 1;
  UINT8   ChassisSupport : 1;
  UINT8   ManufacturerId[3];
  UINT16  ProductId;
  UINT32  AuxFirmwareRevInfo;
} EFI_SM_CTRL_INFO;
#pragma pack()

//
//  Constants and Structure definitions for "Get Self Results" command to follow here
//

/*----------------------------------------------------------------------------------------
    Definitions for Get Self Results command
----------------------------------------------------------------------------------------*/
#define EFI_APP_GET_SELFTEST_RESULTS  0x4

#define EFI_APP_SELFTEST_RESERVED             0xFF
#define EFI_APP_SELFTEST_NO_ERROR             0x55
#define EFI_APP_SELFTEST_NOT_IMPLEMENTED      0x56
#define EFI_APP_SELFTEST_ERROR                0x57
#define EFI_APP_SELFTEST_FATAL_HW_ERROR       0x58
#define EFI_APP_SELFTEST_INACCESSIBLE_SEL     0x80
#define EFI_APP_SELFTEST_INACCESSIBLE_SDR     0x40
#define EFI_APP_SELFTEST_INACCESSIBLE_FRU     0x20
#define EFI_APP_SELFTEST_IPMB_SIGNAL_FAIL     0x10
#define EFI_APP_SELFTEST_SDR_REPOSITORY_EMPTY 0x08
#define EFI_APP_SELFTEST_FRU_CORRUPT          0x04
#define EFI_APP_SELFTEST_FW_BOOTBLOCK_CORRUPT 0x02
#define EFI_APP_SELFTEST_FW_CORRUPT           0x01

typedef struct {
  UINT8   Status;
  UINT8   ErrorBitSet;
} EFI_GET_SELF_TEST_STATUS;

#pragma pack()
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
