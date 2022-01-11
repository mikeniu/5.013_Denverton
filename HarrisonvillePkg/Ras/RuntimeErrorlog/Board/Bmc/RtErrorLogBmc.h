//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogBmc.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _BMC_SEL_LOG_H
#define _BMC_SEL_LOG_H

#include <Ras\RuntimeErrorlog\RtErrorLogBoard.h>
#include <Protocol/IPMITransportProtocol.h>

#define ADD_SEL_ENTRY                   0x44
#define EFI_SM_STORAGE      			  0xA

//
// General Info related to SEL Record
//

#define EFI_EVM_REVISION    0x04
#define EFI_BIOS_ID         0x18
#define EFI_FORMAT_REV      0x00
#define EFI_FORMAT_REV1     0x01
#define EFI_SOFTWARE_ID     0x01
#define EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE     0x6F

#define EFI_MEMORY_SENSOR_TYPE              0x0C
#define EFI_PROCESSOR_SENSOR_TYPE           0x07
#define SEL_SYSTEM_RECORD                   0x2  

//
// SEL Sensor Types and Error Codes for PCI Errors
//
#define SEL_SENS_TYPE_CRIT_ERR      0x13
#define SEL_SENS_PCI_PERR           0x04      
#define SEL_SENS_PCI_SERR           0x05

#define MEMORY_SENSOR_CE_OFFSET              0x00
#define MEMORY_SENSOR_UCE_OFFSET              0x01
#define MEMORY_SENSOR_CEMAX_OFFSET              0x05

#define EFI_GENERATOR_ID(a)             ((EFI_BIOS_ID << 1) | (a << 1) | EFI_SOFTWARE_ID)

#define SOCKET_NUM_BITS              0xC0

#define OEMEvData3_SOCKET_NUM_BITS             0x6
#define OEMEvData3_CHANNEL_NUM_BITS             0x4
#define DIMM_NUM_BITS                0x0F

typedef enum {
  EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE=0,
  EFI_MEMORY_SENSOR_OFFSET_ECC_UNCORRECTABLE,
  EFI_MEMORY_SENSOR_OFFSET_PARITY,
  EFI_MEMORY_SENSOR_OFFSET_SCRUB_FAILED,
  EFI_MEMORY_SENSOR_OFFSET_MEM_DEVICE_DISABLED,
  EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE_MAX,
}EFI_SEL_SENSOR_MEMORY_OFFSET;

typedef enum {
  EFI_PROCESSOR_SENSOR_OFFSET_CONFIGURATION_ERROR=5,
}EFI_SEL_SENSOR_PROCESSOR_OFFSET;

typedef struct {          
  UINT32      Year    : 4;
  UINT32      Month   : 4;
  UINT32      Day     : 5;        
  UINT32      Hour    : 5;       
  UINT32      Minute  : 6;     
  UINT32      Second  : 6;     
} TIME_STAMP;

#pragma pack(1)
typedef struct {
  UINT16  RecordId;
  UINT8   RecordType;
  TIME_STAMP  TimeStamp;
  UINT16  GeneratorId;
  UINT8   EvMRevision;
  UINT8   SensorType;
  UINT8   SensorNumber;
  UINT8   EventDirType;
  UINT8   OEMEvData1;
  UINT8   OEMEvData2;
  UINT8   OEMEvData3;
}EFI_SEL_RECORD_DATA;
#pragma pack()


EFI_STATUS
LogMeToSel (
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
);

EFI_STATUS
LogPciErrorToBmc (
    IN UINT8    ErrorSource,
    IN UINT8    ErrType,
    IN UINT8    Segment,
    IN UINT8    Bus,
    IN UINT8    Dev,
    IN UINT8    Func
);

VOID
EFIAPI
LogMcErrorToBmc (
  IN  MC_ERROR_LOG_INFO *McErrLogInfo
  );

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
