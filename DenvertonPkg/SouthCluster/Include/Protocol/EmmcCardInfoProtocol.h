//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

--*/


/*++
Module Name:

  EmmcCardInfoProtocol.h

Abstract:

  Interface definition for EFI_EMMC_CARD_INFO_PROTOCOL

--*/


#ifndef _EMMC_CARD_INFO_H_
#define _EMMC_CARD_INFO_H_

#define EFI_EMMC_CARD_INFO_PROTOCOL_GUID \
  { \
    0x1ebe5ab9, 0x2129, 0x49e7, {0x84, 0xd7, 0xee, 0xb9, 0xfc, 0xe5, 0xde, 0xdd } \
  }

typedef struct _EFI_EMMC_CARD_INFO_PROTOCOL  EFI_EMMC_CARD_INFO_PROTOCOL;


//
// EMMC Card info Structures
//
struct _EFI_EMMC_CARD_INFO_PROTOCOL{
  CARD_DATA *CardData;
};

extern EFI_GUID gEfiEmmcCardInfoProtocolGuid;
#endif
