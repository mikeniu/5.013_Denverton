//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/**

  @file IeHeciDxe.h

**/
#ifndef _IE_HECI_DXE_H_
#define _IE_HECI_DXE_H_

//
// DXE Timeout values
//
#define HECI_WAIT_DELAY    1000    // 1 ms
#define HECI_INIT_TIMEOUT  2000000 // 2 s
#define HECI_READ_TIMEOUT  500000  // 0.5 s
#define HECI_SEND_TIMEOUT  500000  // 0.5 s
#define HECI_RESET_TIMEOUT 2000000 // 2 sec


//
// Heci Protocol definitions
//
#define DXE_IE_HECI_PROTOCOL_GUID \
  { \
    0xace08a6e, 0x7c7b, 0x4afe, { 0xae, 0x9d, 0xbb, 0x1c, 0x1c, 0x7e, 0x5c, 0xf3 }\
  }

#define SMM_IE_HECI_PROTOCOL_GUID \
  { \
    0x4acd0113, 0xf661, 0x4259, { 0xa1, 0x49, 0x84, 0xe7, 0x78, 0x71, 0xa,  0xf2 }\
  }
typedef struct _IE_HECI_PROTOCOL IE_HECI_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *IE_HECI_SENDWACK) (
  IN      IE_HECI_PROTOCOL   *This,
  IN OUT  UINT32                 *Message,
  IN OUT  UINT32                 *Length,
  IN      UINT8                  HostAddress,
  IN      UINT8                  IeAddress
  );

typedef
EFI_STATUS
(EFIAPI *IE_HECI_READ_MESSAGE) (
  IN      IE_HECI_PROTOCOL   *This,
  IN      UINT32                 Blocking,
  IN      UINT32                 *MessageBody,
  IN OUT  UINT32                 *Length
  );

typedef
EFI_STATUS
(EFIAPI *IE_HECI_SEND_MESSAGE) (
  IN      IE_HECI_PROTOCOL   *This,
  IN      UINT32                 *Message,
  IN      UINT32                 Length,
  IN      UINT8                  HostAddress,
  IN      UINT8                  IeAddress
  );

typedef
EFI_STATUS
(EFIAPI *IE_HECI_INITIALIZE) (
  IN      IE_HECI_PROTOCOL   *This
  );


typedef struct _IE_HECI_PROTOCOL {

  IE_HECI_SENDWACK       SendwAck;
  IE_HECI_READ_MESSAGE   ReadMsg;
  IE_HECI_SEND_MESSAGE   SendMsg;
  IE_HECI_INITIALIZE     InitializeHeci;

} IE_HECI_PROTOCOL;

extern EFI_GUID gDxeIeHeciProtocolGuid;
extern EFI_GUID gSmmIeHeciProtocolGuid;

#endif
