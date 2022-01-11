//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmBiosV234.h

Abstract:

  Data and structures defined in System Management BIOS Specification Version 2.3.4 

--*/


#ifndef _EFI_SMBIOS_V234_H_
#define _EFI_SMBIOS_V234_H_

//
// Wake Event Types
//
#define SMBIOS_WAKEUP_TYPE_RESERVED             0x00    
#define SMBIOS_WAKEUP_TYPE_OTHERS               0x01
#define SMBIOS_WAKEUP_TYPE_UNKNOWN              0x02
#define SMBIOS_WAKEUP_TYPE_APM_TIMER            0x03
#define SMBIOS_WAKEUP_TYPE_MODEM_RING           0x04
#define SMBIOS_WAKEUP_TYPE_LAN_REMOTE           0x05
#define SMBIOS_WAKEUP_TYPE_POWER_SWITCH         0x06
#define SMBIOS_WAKEUP_TYPE_PCI_PME              0x07
#define SMBIOS_WAKEUP_TYPE_AC_POWER_RESTORED    0x08
        
#endif  // _EFI_SMBIOS_V234_H_
        
        
        
        
        
        