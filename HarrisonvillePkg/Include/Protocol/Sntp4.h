/*
 * Sntp4.h
 *
 *  Created on: Jun 2, 2015
 *      Author: magilhig
 */

#ifndef SNTP4_H_
#define SNTP4_H_

typedef struct _EFI_SNTP4_PROTOCOL EFI_SNTP4_PROTOCOL;

typedef struct {
  BOOLEAN                 UseDefaultSetting;
  EFI_IPv4_ADDRESS        StationIp;
  EFI_IPv4_ADDRESS        SubnetMask;
  UINT16                  LocalPort;
  EFI_IPv4_ADDRESS        GatewayIp;
  EFI_IPv4_ADDRESS        ServerIp;
  UINT16                  InitialServerPort;
  UINT16                  TryCount;
  UINT16                  TimeoutValue;
} EFI_SNTP4_CONFIG_DATA;


typedef struct {
  EFI_SNTP4_CONFIG_DATA   ConfigData;
  UINT8                   SupportedOptionCount;
  UINT8                   **SupportedOptions;
  UINT8                   UnsupportedOptionCount;
  UINT8                   **UnsupportedOptions;
} EFI_SNTP4_MODE_DATA;



typedef
EFI_STATUS
(EFIAPI *EFI_SNTP4_GET_MODE_DATA)(
    IN     EFI_SNTP4_PROTOCOL *This,
    OUT EFI_SNTP4_MODE_DATA *ModeData
    );


typedef
EFI_STATUS
(EFIAPI *EFI_SNTP4_QUERY)(
    IN      EFI_SNTP4_PROTOCOL *This,
    OUT     EFI_TIME           *SntpTime
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SNTP4_CONFIGURE)(
    IN EFI_SNTP4_PROTOCOL        *This,
    IN EFI_SNTP4_CONFIG_DATA     *ConfigData
    );
struct _EFI_SNTP4_PROTOCOL {
  EFI_SNTP4_GET_MODE_DATA     GetModeData;
  EFI_SNTP4_QUERY             Query;
  EFI_SNTP4_CONFIGURE    	  Configure;
};


extern EFI_GUID gEfiSntp4ServiceBindingProtocolGuid;
extern EFI_GUID gEfiSntp4ProtocolGuid;
#endif /* SNTP4_H_ */
