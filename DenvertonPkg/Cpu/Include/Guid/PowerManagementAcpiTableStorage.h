/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PowerManagementAcpiTableStorage.h
    
Abstract:

  GUID definition for the Power Management ACPI table storage file name

--*/

#ifndef _POWER_MANAGEMENT_ACPI_TABLE_STORAGE_H_
#define _POWER_MANAGEMENT_ACPI_TABLE_STORAGE_H_

#define POWER_MANAGEMENT_ACPI_TABLE_STORAGE_GUID \
  { 0x161be597, 0xe9c5, 0x49db, 0xae, 0x50, 0xc4, 0x62, 0xab, 0x54, 0xee, 0xda }

extern EFI_GUID gPowerManagementAcpiTableStorageGuid;

#endif
