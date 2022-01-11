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

    SetupVariable.h

Abstract:

    Driver configuration include file

Revision History

--*/

#ifndef __SETUP_VARIABLE_H__
#define __SETUP_VARIABLE_H__

//DNV TODO: This file needs to be merged into:
//Denverton\HarrisonvillePkg\Include\SetupVariable.h

//
// ---------------------------------------------------------------------------
//
// Driver Configuration
//
// ---------------------------------------------------------------------------
//
#define EFI_EPG_GENERIC_VARIABLE_GUID \
  { \
    0x8302cc54, 0xbb1a, 0x4564, 0x92, 0xdc, 0xae, 0x1a, 0xbb, 0x15, 0x5f, 0x35 \
  }

#define PCH_SETUP_GUID \
 { 0x4570b7f1, 0xade8, 0x4943, 0x8d, 0xc3, 0x40, 0x64, 0x72, 0x84, 0x23, 0x84 }


//
// {EC87D643-EBA4-4bb5-A1E5-3F3E36B20DA9}
//
// Denverton AptioV Override Start - EIP#217439
#define SYSTEM_CONFIGURATION_GUID \
  { \
    0xc598db5c, 0x50e7, 0x46f6, 0xab, 0x16, 0x41, 0x6f, 0x47, 0x23, 0x9c, 0xa9 \
  }
// Denverton AptioV Override End - EIP#217439
#define SYSTEM_CONFIGURATION_CONTROL_GUID \
  { \
    0x8d247131, 0x385e, 0x491f, 0xba, 0x68, 0x8d, 0xe9, 0x55, 0x30, 0xb3, 0xa6 \
  }

#define SYSTEM_CONFIGURATION_ICHPCIE_GUID \
  { \
    0x10e023a7, 0x4ce5, 0x4a6a, 0xa1, 0xbb, 0xbd, 0xd0, 0x8d, 0x16, 0x37, 0x57 \
  }

#define SYSTEM_DEFAULT_CONFIGURATION_GUID \
  { \
    0x99a96812, 0x4730, 0x4290, 0x8b, 0xfe, 0x7b, 0x4e, 0x51, 0x4f, 0xf9, 0x3b \
  }


#endif // #ifndef _SETUP_VARIABLE
