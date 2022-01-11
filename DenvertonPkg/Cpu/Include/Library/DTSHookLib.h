/*++        

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  DTSHookLib.h

Abstract:

  Defines and prototypes for the DigitalThermalSensor SMM driver
  
--*/

#ifndef _DTS_LIB_H_
#define _DTS_LIB_H_
//
// Include files
//
#include <Library/KscLib.h>

EFI_STATUS
InitializeDtsHookLib (
  VOID
);

VOID
PlatformHookBeforeGenerateSCI (
  VOID
);

UINT8
ReadPlatformThermalDiode(
   VOID
);

VOID
PlatformEventOutOfSpec (
  VOID
);

#endif