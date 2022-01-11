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

/** @file IpmiIeTransportPpi.h
    IPMI IE PPI definitions

**/

#ifndef _PEI_IPMI_IE_TRANSPORT_PPI_H_
#define _PEI_IPMI_IE_TRANSPORT_PPI_H_

#include <Token.h>
#include <Include/IpmiIeNetFnTransportDefinitions.h>

#define PEI_IPMI_IE_TRANSPORT_PPI_GUID \
  { \
    0xe50b6e5d, 0x359b, 0x4be0, 0xa0, 0xc0, 0x7, 0x15, 0xa4, 0x1, 0xc6, 0x8 \
  }

//
// Common Defines
//
typedef UINT32  EFI_PEI_IE_STATUS;

#define EFI_IE_OK                    0
#define EFI_IE_SOFTFAIL              1
#define EFI_IE_HARDFAIL              2
#define EFI_IE_UPDATE_IN_PROGRESS    3

#define MAX_SOFT_COUNT              10

typedef struct _PEI_IPMI_TRANSPORT_PPI PEI_IPMI_TRANSPORT_PPI;

//
//  IPMI Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_PEI_IPMI_SEND_COMMAND) (
  IN PEI_IPMI_TRANSPORT_PPI             * This,
  IN UINT8                              NetFunction,
  IN UINT8                              Lun,
  IN UINT8                              Command,
  IN UINT8                              *CommandData,
  IN UINT8                              CommandDataSize,
  OUT UINT8                             *ResponseData,
  OUT UINT8                             *ResponseDataSize
);

typedef
EFI_STATUS
(EFIAPI *EFI_PEI_IPMI_GET_CHANNEL_STATUS) (
  IN PEI_IPMI_TRANSPORT_PPI             * This,
  OUT EFI_PEI_IE_STATUS                 * IeStatus,
  OUT EFI_SM_COM_ADDRESS                * ComAddress
);

/**
 IPMI TRANSPORT PPI
*/
typedef struct _PEI_IPMI_TRANSPORT_PPI {
  UINT64                            Revision;
  UINT8                             CommandCompletionCode;
  EFI_PEI_IPMI_SEND_COMMAND         SendIpmiCommand;
  EFI_PEI_IPMI_GET_CHANNEL_STATUS   GetIeStatus;
} PEI_IPMI_TRANSPORT_PPI;

extern EFI_GUID gEfiPeiIpmiIeTransportPpiGuid;

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
