//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file DigitalThermalSensor.h
    Defines and prototypes for the Digital Thermal Sensor SMM driver

*/

#ifndef _DIGITAL_THERMAL_SENSOR_H_
#define _DIGITAL_THERMAL_SENSOR_H_

//
// Include files
//
#include "token.h"
#include <Include/Efi.h>
#include <Include/AmiDxeLib.h>
#include <Library/BaseLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciLib.h>
#include <CpuPpmLib.h>
#include <CpuRegs.h>
#include <Protocol/GlobalNvsArea.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsLpc.h>

extern EFI_GUID SmmRsTableGuid;

//
// Defines
//

#define B_READING_VALID                   (1 << 31)
#define B_THERMAL_THRESHOLD_2_STATUS_LOG  (1 << 9)
#define B_THERMAL_THRESHOLD_2_STATUS      (1 << 8)
#define B_THERMAL_THRESHOLD_1_STATUS_LOG  (1 << 7)
#define B_THERMAL_THRESHOLD_1_STATUS      (1 << 6)
#define B_OUT_OF_SPEC_STATUS_LOG          (1 << 5)
#define B_OUT_OF_SPEC_STATUS              (1 << 4)


#define OFFSET_MASK                       (0x7F)
#define LOCAL_APIC_THERMAL_DEF            0xFEE00330                                            // Default Thermal Entry
#define EFI_MSR_EXT_XAPIC_LVT_THERM       0x833
#define B_INTERRUPT_MASK                  (1 << 16)
#define B_DELIVERY_MODE                   (0x07 << 8)
#define V_MODE_SMI                        (0x02 << 8)
#define B_VECTOR                          (0xFF << 0)

#define R_PCH_LPC_ACPI_BASE                       0x40 // ABASE, 16bit
#define B_PCH_LPC_ACPI_BASE_BAR                   0x0000FF80 // Base Address, 128 Bytes
//
// Enumerate a DTS event type
//
typedef enum {
  DtsEventNone,
  DtsEventThreshold2,
  DtsEventThreshold1,
  DtsEventOutOfSpec,
  DtsEventClose,
  DtsEventMax
} DTS_EVENT_TYPE;

//
// Function declarations
//

/**
  Issue a Shutdown command
**/
VOID
PerformShutdown (
  VOID
);

/**
  Read the temperature data per core/thread.
  This function must be AP safe.

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature
**/

VOID
EFIAPI
DigitalThermalSensorUpdateTemperature (
  VOID *Buffer
);

/**
  SMI handler to handle DigitalThermalSensor CPU Local APIC SMI 
  for thermal threshold interrupt 

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
EFIAPI
DtsSmiCallback (
  IN EFI_HANDLE             DispatchHandle,
  IN     CONST VOID         *Context,         OPTIONAL
  IN OUT VOID               *CommBuffer,      OPTIONAL
  IN OUT UINTN              *CommBufferSize   OPTIONAL
  );


/**
  Generates a _GPE._L02 SCI to an ACPI OS.
**/
VOID
DigitalThermalSensorSetSwGpeSts (
  VOID
  );

/**
  Checks for a Core Thermal Event by reading MSR.

  This function must be MP safe.

  @param[in] Buffer    Pointer to DTS_EVENT_TYPE
**/
VOID
EFIAPI
DigitalThermalSensorEventCheckMsr (
  IN  VOID        *Buffer
  );
      
/**
  Checks for a Core Thermal Event on any processor

  @retval TRUE means this is a DTS Thermal event
  @retval FALSE means this is not a DTS Thermal event.
**/
BOOLEAN
DigitalThermalSensorEventCheck (
  VOID
  );
   


/**
  Enables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Enable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorEnableSmi (
  IN VOID          *Buffer
  );

/**
  Disables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Disable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorDisableSmi (
  IN VOID          *Buffer
  );                                            

/**
  Perform first time initialization of the Digital Thermal Sensor

  @retval EFI_SUCCESS  Init Digital Thermal Sensor successfully
**/
EFI_STATUS
DigitalThermalSensorInit (
  VOID
  );

/**
  Digital Thermal Sensor (DTS) SMM driver entry point function.

  @param[in] ImageHandle   Image handle for this driver image
  @param[in] SystemTable   Pointer to the EFI System Table

  @retval EFI_SUCCESS   Driver initialization completed successfully
**/
EFI_STATUS
InstallDigitalThermalSensor (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
