/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c) 2012 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  GlobalNvsArea.h

Abstract:

  Definition of the global NVS area protocol.  This protocol
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM code and ASL code.
  The format is derived from the ACPI reference code, version 0.95.

  Note:  Data structures defined in this protocol are not naturally aligned.

--*/


#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

//
// Forward reference for pure ANSI compatability
//

typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL EFI_GLOBAL_NVS_AREA_PROTOCOL;

//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, {0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc} }


//
// Global NVS Area definition
//
#pragma pack (1)
  typedef struct EFI_GLOBAL_NVS_AREA_s {
    // Miscellaneous Dynamic Values 
    UINT32  OperatingSystemType;                // 00 OS Type 
    UINT32  HpetEnable;                         // 04 HPET 
    UINT8   SmiFunction;                        // 08 SMI function call via IO Trap 
    UINT8   GlobalLockFunction;                 // 09 Global Lock Function Call 

    // Thermal Policy Registers   
    UINT8   ActiveThermalTripPoint;             // 10 ActiveThermalTripPoint 
    UINT8   PassiveThermalTripPoint;            // 11 PassiveThermalTripPoint 
    UINT8   PassiveTc1Value;                    // 12 PassiveTc1Value 
    UINT8   PassiveTc2Value;                    // 13 PassiveTc2Value 
    UINT8   PassiveTspValue;                    // 14 PassiveTspValue 
    UINT8   CriticalThermalTripPoint;           // 15 CriticalThermalTripPoint 
    UINT8   EnableDigitalThermalSensor;         // 16 DTS Function enable 
    UINT8   BspDigitalThermalSensorTemperature; // 17 Temperature of BSP 
    UINT8   ApDigitalThermalSensorTemperature;  // 18 Temperature of AP 
    UINT8   DigitalThermalSensorSmiFunction;    // 19 SMI function call via DTS IO Trap 
  
    // Processor Configuration Values 
    UINT32  PpmFlags;                           // 20 PPM configuration flags, same as CFGD 
    UINT8   ApicEnable;                         // 24 APIC Enabled by SBIOS (APIC Enabled = 1) 
    UINT8   HTD;                                // 25 MP Capable/SBIOS Enabled (MP Enabled != 0) 

    UINT8   AcSetup;                            // 26 For Ac Powered Config option - IST applet 
    UINT8   BatterySetup;                       // 27 For Battery Powered Config option - IST applet 
    UINT8   PowerState;                         // 28 AC = 1 

    // TPM Registers 
    UINT8   TpmPresent;                         // 29 TPM Preset 
    UINT8   TpmEnabled;                         // 30 TPM Enabled 
    UINT8   Rsvd;                               // 31 Reserved 

    // Platform Data 
    UINT32 MMIOB;                              // 32 Low MMIO Base
    UINT32 MMIOL;                              // 36 Low MMIO Lt
    UINT64 MMIHB;                              // 40 High MMIO Base
    UINT64 MMIHL;                              // 48 High MMIO Lt
    UINT32 TsegB;                              // 56 TSEG Base
    UINT32 TsegL;                              // 60 TSEG Length
    UINT8  CStateEnable;                       // 61 CState
    UINT8  C6NS;                               // 62 Hints
    UINT8  C6FS;                               // 63 Hints
    UINT8  IpmiSupport;                        // 64 IPMI Support
    UINT32 eMMCAddr;                           // 65 eMMC BAR0
    UINT32 eMMCLen;                            // 69 eMMC BAR0 Length
    UINT32 eMMC1Addr;                          // 73 eMMC BAR1
    UINT32 eMMC1Len;                           // 77 eMMC BAR1 Length
    UINT8  NCpu;                               // 78 No of CPUs
    UINT8  Legacy;                             // 79 Legacy/Legacy VGA/UEFI
    UINT8  Usb3Port0Connectable;               // USB 3.0 Port 0 Connectivity flag
    UINT8  Usb3Port0Type;                      // USB 3.0 Port 0 Connector type
    UINT8  Usb3Port0Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb3Port0Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb3Port1Connectable;               // USB 3.0 Port 1 Connectivity flag
    UINT8  Usb3Port1Type;                      // USB 3.0 Port 1 Connector type
    UINT8  Usb3Port1Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb3Port1Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb3Port2Connectable;               // USB 3.0 Port 2 Connectivity flag
    UINT8  Usb3Port2Type;                      // USB 3.0 Port 2 Connector type
    UINT8  Usb3Port2Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb3Port2Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb3Port3Connectable;               // USB 3.0 Port 3 Connectivity flag
    UINT8  Usb3Port3Type;                      // USB 3.0 Port 3 Connector type
    UINT8  Usb3Port3Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb3Port3Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb2Port0Connectable;               // USB 2.0 Port 0 Connectivity flag
    UINT8  Usb2Port0Type;                      // USB 2.0 Port 0 Connector type
    UINT8  Usb2Port0Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb2Port0Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb2Port1Connectable;               // USB 2.0 Port 1 Connectivity flag
    UINT8  Usb2Port1Type;                      // USB 2.0 Port 1 Connector type
    UINT8  Usb2Port1Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb2Port1Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb2Port2Connectable;               // USB 2.0 Port 2 Connectivity flag
    UINT8  Usb2Port2Type;                      // USB 2.0 Port 2 Connector type
    UINT8  Usb2Port2Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb2Port2Group;                     // USB 3.0 Port 0 Connector group
    UINT8  Usb2Port3Connectable;               // USB 2.0 Port 3 Connectivity flag
    UINT8  Usb2Port3Type;                      // USB 2.0 Port 3 Connector type
    UINT8  Usb2Port3Visible;                   // USB 3.0 Port 0 Connector visibility
    UINT8  Usb2Port3Group;                     // USB 3.0 Port 0 Connector group
    UINT8  HpetBaseAddress;                    // HpetBaseAddress
    UINT8  GpioStatus;              // GpioStatus
    UINT32 HsUartStatus1;                      // HSUart status 1
    UINT32 HsUartStatus2;                      // HSUart status 2
    UINT32 HsUartStatus3;                      // HSUart status 3
    UINT32 HsUartStatus4;                      // HSUart status 4
    UINT32 CpldStatus;                         // CPLD status
  } EFI_GLOBAL_NVS_AREA;

typedef union {
  EFI_GLOBAL_NVS_AREA area;
  UINT8 Reserved[512];
} EFI_GLOBAL_NVS_AREA_u;
#pragma pack ()

//
// Global NVS Area Protocol
//
struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  EFI_GLOBAL_NVS_AREA     *Area;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

#endif
