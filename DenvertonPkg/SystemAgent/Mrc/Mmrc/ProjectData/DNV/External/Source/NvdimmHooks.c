/**  @file
  NvdimmHooks.c
  This file include all the OEM hooks for MRC.

  Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "NvdimmHooks.h"
#include "MmrcHooks.h"
#include "MmrcLibraries.h"
#include "MmrcData.h"

//helper functions not in DNV MRC
typedef MMRC_DATA* PSYSHOST;

typedef struct smbDevice{
  UINT8 deviceAddr;
} smbDevice;

#define NVDIMM_FLASH_CONTROLLER(SpdAddress, NVMctrlr) (SpdAddress&0x0f | (0xf0 & (NVMctrlr<<4)))
#define SMB_ID(smb) (smb.deviceAddr)

UINT16
SearchSubtaskName (
  IN            UINT16    CurrentTaskOffset
);

UINT32
ReadSmbNVDIMM(
	PSYSHOST    host,
	UINT8       socket,
	struct smbDevice  dev,
	UINT8       byteOffset,
	UINT8       *data
  );
UINT32
WriteSmbNVDIMM(
	PSYSHOST    host,                             // Pointer to the system host (root) structure
	UINT8       socket,                           // Processor socket ID
	struct smbDevice  dev,                        // Device address on SMBUS
	UINT8       byteOffset,                       // Offset in Device
	UINT8       *data                             // Pointer to store value
  );
VOID 
NvDimmMrsRestore(
  MMRC_DATA   *MrcData,
  UINT8       ch,
  UINT8       dimm
);
VOID PrechargeAllRanks(
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       ch
);

static UINT32 WriteSmb (
    IN  OUT  MMRC_DATA   *host,
    IN       UINT8       socket,
    IN       smbDevice   spd,
    IN       UINT16      SPD_FIELD,
    IN       UINT8*      SPDReg
    );
static UINT32 ReadSmb  (
    IN  OUT  MMRC_DATA   *host,
    IN       UINT8       socket,
    IN       smbDevice   spd,
    IN       UINT16      SPD_FIELD,
    IN       UINT8*      SPDReg
    );
static VOID FixedDelay (
    IN  OUT  MMRC_DATA   *host,
    IN       UINT32      duration
    );
UINT32 ReadSpd(
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch,
  UINT8       dimm,
  UINT16      byteOffset,
  UINT8       *data
  );
UINT32 CoreDetectNVDIMM(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  struct smbDevice  spd
  );
UINT32 CoreArmNVDIMMs(
    PSYSHOST host
  );
UINT32 CoreNVDIMMStatus(
  PSYSHOST host
  );

UINT32 JEDECSwitchPage(
  PSYSHOST          host,
  UINT8             socket,
  struct smbDevice  smbusdev,
  UINT8            page
  );

#define SPD_MODULE_PN_SIZE  18
#define NV_NAME_LEN         18
#define NV_REPORT           1
#define NV_REPORT_NOT       0

#define MAX_CH              MAX_CHANNELS/*DNV's*/
#define MAX_DIMMS           2/*DNV's*/
#define FAILURE             MMRC_FAILURE
#define SUCCESS             MMRC_SUCCESS
#define CONTINUOUS_REFRESH   0

typedef UINT32(*NVDIMM_FUNC)(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 vendorIndex, struct smbDevice dev);



typedef struct _nvDev {
  UINT16         nvVid;                              /* SPD Vendor ID                                                       */
  UINT8          nvDid[SPD_MODULE_PN_SIZE+1];        /* SPD Device ID                                                       */
  UINT32         nvDidCnt;                           /* Str Len for nvDid match: Multiple matched devices use this control table  */
  UINT8          nvName[NV_NAME_LEN];                /* sZ Vendor name                                                      */
  UINT8          smbusAdr;                           /* SMBus address used for NVDIMM operations                            */
  UINT8          vendorIndex;  // Unique number per vendor to allow handling multiple NVDIMMs from one function
  NVDIMM_FUNC    getStatus;
  NVDIMM_FUNC    startRestore;
  NVDIMM_FUNC    restoreDone;
  NVDIMM_FUNC    arm;
} nvDev;



/**
  This routine reads an SMBus address for a given number of seconds or
  until the expected value is found

  @param[in]       host          MRC paramater frame
  @param[in]       socket        NOT used - kept to maintaine cominality
  @param[in]       smbusdev      Describes the SMBus and device to query
  @param[in out]   data          result of the last read
  @param[in]       mask          mask of bits being watched
  @param[in]       expectedvalue value expected after mask 'or'ed to register contents
  @param[in]       timeout       number of seconds to wait
**/
UINT32
ReadSmbNVDIMM(
  PSYSHOST    host,
  UINT8       socket,
  struct smbDevice  dev,
  UINT8      byteOffset,
  UINT8       *data
  )
{
  UINT32 status;
  status = ReadSmb(host, socket, dev, (UINT16)byteOffset, data);
  if (status == EFI_SUCCESS)
    ;//MspDebugPrint((MSP_DBG_TRACE, "  ReadSmb[%x]:Offset=0x%x, Value Read=0x%x,status=0x%x\n", SMB_ID(dev), byteOffset, *data,status));
  else
    MspDebugPrint((MSP_DBG_MED, "  ReadSmb[%x]:Offset=0x%x, Read failed, ret value=0%x\n", SMB_ID(dev), byteOffset, status));

  return status;
}


/**
  This routine Writes an SMBus address for a given number of seconds or
  until the expected value is found

  @param[in]       host          MRC paramater frame
  @param[in]       socket        NOT used - kept to maintaine cominality
  @param[in]       smbusdev      Describes the SMBus and device to query
  @param[in out]   data          result of the last read
  @param[in]       mask          mask of bits being watched
  @param[in]       expectedvalue value expected after mask 'or'ed to register contents
  @param[in]       timeout       number of seconds to wait
**/
UINT32
WriteSmbNVDIMM(
  PSYSHOST    host,                             // Pointer to the system host (root) structure
  UINT8       socket,                           // Processor socket ID
  struct smbDevice  dev,                        // Device address on SMBUS
  UINT8       byteOffset,                       // Offset in Device
  UINT8       *data                             // Pointer to store value
  //   NOTE: For a PCH block read, the array must be 33bytes in length,
  //   and the first data byte pointed to is the number of bytes read.
  //   This implementation only allows a maximum of 32bytes to be read.
  )
{
  UINT32 status;
  status = WriteSmb(host, socket, dev, byteOffset, data);
  MspDebugPrint((MSP_DBG_MED, "  WriteSmb[%x]:Offset=0x%x, Value Written=0x%x, status=0x%x\n", SMB_ID(dev), byteOffset, *data,status));

  if (status != EFI_SUCCESS)
    MspDebugPrint((MSP_DBG_MED, "  WRITE FAILED\n"));
  
  return status;
}



UINT32
ReadSmbTimeout (
  PSYSHOST         host,
  UINT8            socket,
  struct smbDevice smbusdev,
  UINT8            byteOffset,
  UINT8            *data,
  UINT8            mask,
  UINT8            expectedvalue,
  UINT16           timeout
  )
{
  UINT32        passnum = 0;
  UINT32        status;

  if (!timeout) {
    //
	// minimum of two polls of the register
	//
    timeout = 1;
  }
  MspDebugPrint (
      (MSP_DBG_MED,
       "    Starting a Poll Reg=0x%x, Mask=0x%x, Expected Value=0x%x\n",byteOffset,mask,expectedvalue));

  //ReadTsc64(&startTsc);
  do {
    status = ReadSmb(host, socket, smbusdev, (UINT16)byteOffset, data);
    if ((passnum & 0xFF) == 1) {
      if (status == SUCCESS)
        MspDebugPrint ((MSP_DBG_MED, "        Data=0x%x  ", *data));
      else
        MspDebugPrint ((MSP_DBG_MED, "      SMBUS Read FAILED!!!! Status=0x%x\n", status));

    }

    if(passnum != 0){
      CONST UINT32  OneSecInUsecs = 1000000; //should be 1,000,000 but not correct timing

      FixedDelay( host, OneSecInUsecs );
      MspDebugPrint ((MSP_DBG_MED, ".", status));
      timeout--;
    }
    passnum++;

    if (0 == timeout) {
      MspDebugPrint ((MSP_DBG_MED, "\n      ReadSmbTimeoutTimeout after %d seconds, Reg=0x%x, Mask=0x%x, Expected Value=0x%x, last read=0x%x, last status=0x%x\n", timeout, byteOffset, mask, expectedvalue, *data, status));
      return FAILURE;
    }
  } while (((*data & mask) != expectedvalue) || (status != SUCCESS));
  MspDebugPrint ((MSP_DBG_MED, "      Final Data=0x%x\n", *data));
  return SUCCESS;
} // ReadSmbTimeout

//
//
// Netlist NVDIMM helper functions: Status, RestoreStrat, RestoreDone, Arm
//
//
UINT32
NetlistStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32  status;
  UINT8   data;

  // Wait for NVDIMM to init
  status = ReadSmbTimeout(host, socket, smbusdev, 0x9, &data, (1 << 3), (1 << 3), 6);
  if (status == FAILURE) return NVDIMM_ERROR;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "  Making sure there aren't any operations in progress\n"));
#endif // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, smbusdev, 0x8, &data, 0x7, 0, 5*60);

  FixedDelay(host, 1000000);

  status = ReadSmbNVDIMM(host, socket, smbusdev, 0x20, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data & (1 << 7)) return NVDIMM_ERROR;

  // Read Nonvolatile Status Flag
  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xC, &data);
  if (status == FAILURE)
    return NVDIMM_ERROR;

  switch (data) {
  case 0:
  case 1:
    return NVDIMM_NODATA;
    break;

  case 3:
    return NVDIMM_RESTORE;
    break;

  default:
  case 2:
    return NVDIMM_ERROR;
    break;
  }

  //Should never make it here
  //return NVDIMM_ERROR;
} // NetlistStatus


UINT32
NetlistRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  data = 1;
  status = WriteSmbNVDIMM(host, socket, smbusdev, 0x4, &data);

  return SUCCESS;
} // NetlistRestoreStart

UINT32
NetlistRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT8   i;
  UINT32  status;

  //Wait for State to change to 0x8
  status = ReadSmbTimeout(host, socket, smbusdev, 0x10, &data, 0xFF, 0x8, 5 * 60);
  if (status == FAILURE) return FAILURE;

  // Paranoia: Also check for RIP bit to go low
  status = ReadSmbTimeout(host, socket, smbusdev, 0x8, &data, (1 << 1), 0, 60);
  if (status == FAILURE) return FAILURE;

  if (data & (1 << 7)) {
#ifdef SERIAL_DBG_MSG
      MspDebugPrint ((MSP_DBG_MED, "Error during restore\n"));
#endif // SERIAL_DBG_MSG

    for (i = 0x20; i < 0x28; i++) {
      status = ReadSmbNVDIMM(host, socket, smbusdev, i, &data);
#ifdef SERIAL_DBG_MSG
      MspDebugPrint ((MSP_DBG_MED, "Error Reg 0x%x=0x%x\n",i, data));
#endif // SERIAL_DBG_MSG
      status = WriteSmbNVDIMM(host, socket, smbusdev, i, &data);
    } // i loop
  }

  return SUCCESS;
} // NetlistRestoreDone

UINT32
NetlistArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;


#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Waiting for NVDIMM to charge\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, smbusdev, 8, &data, (1 << 4), (1 << 4), 10 * 60);
  if (status == FAILURE) return FAILURE;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Erasing\n"));
#endif // SERIAL_DBG_MSG

  // Always erase
  // Initiate Erase
  data = (1 << 2) | (1 << 0);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x4, &data);
  // Wait for Erase to Finish
  status = ReadSmbTimeout(host, socket, smbusdev, 8, &data, (1 << 2), 0, 60);
  if (status == FAILURE) return FAILURE;

  // Enable SAVE_N pin
  data = (1 << 6) | (1 << 1) | (1 << 0);
  status = WriteSmbNVDIMM(host, socket, smbusdev, 0x4, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Making sure NVDIMM is armed\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, smbusdev, 0x9, &data, (1 << 1) | (1 << 2), (1 << 1) | (1 << 2), 60);
  if (status == FAILURE) {
#ifdef SERIAL_DBG_MSG
    status = ReadSmbNVDIMM(host, socket, smbusdev, 0xC, &data);
    MspDebugPrint ((MSP_DBG_MED, "NV STATUS Register = 0x%x\n", data));
#endif // SERIAL_DBG_MSG
    return FAILURE;
  }

  return SUCCESS;
} // NetlistArm


//
//
// Agigatech NVDIMM helper functions: Status, RestoreStrat, RestoreDone, Arm
//
//
UINT32
AgigaStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Wait for NVDIMM Enabled\n"));
#endif // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, smbusdev, 9, &data, 0xff, 0xa5, 6);
  if (status == FAILURE) return FAILURE;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Making sure there isn't a Restore in Progress\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, smbusdev, 0xA, &data, 0xff, 0, (2 * 60));
  if (status == FAILURE) return FAILURE;

  status = ReadSmb(host, socket, smbusdev, 20, &data);

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Backup Status Register = 0x%x\n", data));
#endif // SERIAL_DBG_MSG

  if (data & (1 << 1))
    return NVDIMM_RESTORE;
  else
    return NVDIMM_NODATA;
} // AgigaStatus

UINT32
AgigaRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8 data;

  // Start the Restore
  data = 0x96;
  WriteSmbNVDIMM(host, socket, smbusdev, 11, &data);

  return SUCCESS;
} // AgigaRestoreStart

UINT32
AgigaRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Wait until Restore is complete
  status = ReadSmbTimeout(host, socket, smbusdev, 11, &data, 0xff, 0, (5 * 60));
  if (status == FAILURE) return FAILURE;

  ReadSmbNVDIMM(host, socket, smbusdev, 21, &data);
  if (!(data & 0x1)) {

#ifdef SERIAL_DBG_MSG
      MspDebugPrint ((MSP_DBG_MED, "Restore Status Register = 0x%x\n", data));
      MspDebugPrint ((MSP_DBG_MED, "Restore FAILED\n", data));
#endif // SERIAL_DBG_MSG
    return FAILURE;
  }

  return SUCCESS;
} // AgigaRestoreDone

UINT32
AgigaArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Make sure Cap is charged

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Waiting for Cap to Charge\n"));
#endif // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, smbusdev, 18, &data, 7, 7, (10 * 60));
  if (status == FAILURE) return FAILURE;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Erasing the NVDIMM\n"));
#endif // SERIAL_DBG_MSG
  data = 0x37;
  status = WriteSmbNVDIMM(host, socket, smbusdev, 0xC, &data);

  status = ReadSmbTimeout(host, socket, smbusdev, 0x12, &data, 0x7F, 0x7F, 60);
  if (status == FAILURE) return FAILURE;
  MspDebugPrint ((MSP_DBG_MED, "GTG1=0x%x,",data));

  ReadSmbNVDIMM(host, socket, smbusdev, 0x13, &data);
  MspDebugPrint ((MSP_DBG_MED, " GTG2=0x%x\n",data));

  // Arming the NVDIMM
  data = 0x18;
  status = WriteSmbNVDIMM(host, socket, smbusdev, 8, &data);

  return SUCCESS;
} // AgigaArm

//
//
// Viking NVDIMM helper functions: Status, RestoreStrat, RestoreDone, Arm
//
//
UINT32 VikingStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Disable All Save Triggers
  data = 0xC0;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x1, &data);


  // Make sure a Save isn't in progress
  status = ReadSmbTimeout(host, socket, smbusdev, vendorIndex, &data, 3, 0, 2 * 60);
  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
    MspDebugPrint ((MSP_DBG_MED, "FAILED to read Status register1\n"));
#endif // SERIAL_DBG_MSG

    return NVDIMM_ERROR;
  }


  // Issue a Reset Command
  data = 0x8F;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Starting 7s Delay\n"));
#endif
  FixedDelay(host,1000000);
  FixedDelay(host,1000000);
  FixedDelay(host,1000000);
  FixedDelay(host,1000000);
  FixedDelay(host,1000000);
  FixedDelay(host,1000000);
  FixedDelay(host,1000000);

  status = ReadSmbNVDIMM(host, socket, smbusdev, vendorIndex, &data);
  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
      MspDebugPrint ((MSP_DBG_MED, "FAILED to read Status register1\n"));
#endif // SERIAL_DBG_MSG

    return NVDIMM_ERROR;
  }

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Status Register = 0x%x\n",data));
#endif // SERIAL_DBG_MSG

  if (data & (1 << 2)) {
    // Check for Host control again
    status = ReadSmbTimeout(host, socket, smbusdev, 0xC, &data, 1, 1, 120);
    return NVDIMM_RESTORE;
  }
  else {
    // Switch Memory control to FPGA
#ifdef SERIAL_DBG_MSG
      MspDebugPrint ((MSP_DBG_MED, "Switching control to FPGA\n"));
#endif
    data = 0x80;
    WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

#ifdef SERIAL_DBG_MSG
    MspDebugPrint ((MSP_DBG_MED, "Waiting for DDR Init Bit to go high\n"));
#endif
    // Wait for DDR Init Bit to go high
    status = ReadSmbTimeout(host, socket, smbusdev, 0xC, &data, (1 << 1), (1 << 1), 30);
    if (status != SUCCESS) return NVDIMM_ERROR;

#ifdef SERIAL_DBG_MSG
    MspDebugPrint ((MSP_DBG_MED, "Switching Control back to host\n"));
#endif
    // Switch Memory control back to CPU
    data = 0x88;
    WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);
#ifdef SERIAL_DBG_MSG
    MspDebugPrint ((MSP_DBG_MED, "Checking for Host Control\n"));
#endif
    // Check for Host control again
    status = ReadSmbTimeout(host, socket, smbusdev, 0xC, &data, 1, 1, 120);
    // Figure Out how to Handle this better
    if (status != SUCCESS) return NVDIMM_ERROR;

    return NVDIMM_NODATA;
  }
} // VikingStatus

UINT32
  VikingRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Switch Memory control to FPGA
  data = 0x80;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

  FixedDelay(host, 5000);

  // Wait for DDR Init Bit to go high
  status = ReadSmbTimeout(host, socket, smbusdev, 0xC, &data, (1 << 1), (1 << 1), 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  // Make sure NVDIMM is idle
  status = ReadSmbTimeout(host, socket, smbusdev, vendorIndex, &data, 3, 0, 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  // Make sure SSD is Ready
  status = ReadSmbTimeout(host, socket, smbusdev, 6, &data, (1 << 4), (1 << 4), 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  // Start Restore
  data = 0x82;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

  FixedDelay(host, 5000);

  // Make sure Restore has started
  status = ReadSmbTimeout(host, socket, smbusdev, vendorIndex, &data, 3, 3, 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  return SUCCESS;
} // VikingRestoreStart


UINT32
VikingRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  restorestatus;
  UINT32  status;
  //volatile int a = 0;

  // Waiting for restore to complete
  restorestatus = ReadSmbTimeout(host, socket, smbusdev, vendorIndex, &data, (1 << 3), (1 << 3), 5 * 60);

  FixedDelay(host, 5000);

  // Switch Memory control back to CPU
  data = 0x88;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

  // Check for Host control again
  status = ReadSmbTimeout(host, socket, smbusdev, 0xC, &data, 1, 1, 30);

  // Figure Out how to Handle this better
  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
      MspDebugPrint ((MSP_DBG_MED, "Couldn't switch back control to host\n"));
#endif
    //while (a == 0);

    return FAILURE;
  }

  return restorestatus;
} // VikingRestoreDone


UINT32
VikingArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Waiting for Cap to charge\n"));
#endif
  status = ReadSmbTimeout(host, socket, smbusdev, vendorIndex, &data, (1 << 5), (1 << 5), 10 * 60);

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Clearing SDV\n"));
#endif
  data = 0x40;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Making sure SAVE pin is only save trigger\n"));
#endif
  data = 0;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x1, &data);

  status = ReadSmbNVDIMM(host, socket, smbusdev, vendorIndex, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Status Register0 = 0x%x\n",data));
#endif

  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xC, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "Status Register1 = 0x%x\n",data));
#endif

  return status;
} // VikingArm

//
//
// Smart NVDIMM helper functions: Unlock, Status, RestoreStrat, RestoreDone, Arm
//
//
UINT32
SmartUnlock(
  PSYSHOST          host,
  UINT8             socket,
  struct smbDevice  smbusdev
  )
{
  UINT32  status;
  UINT8   data;
  UINT8   unlockdata[]   = {'S','M','O','D'};

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Unlocking\n"));
#endif // SERIAL_DBG_MSG

  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[0]);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[1]);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[2]);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[3]);

  status = ReadSmbTimeout(host, socket, smbusdev, 0x3D, &data, 0x80, 0x80, 10);

  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
    MspDebugPrint ((MSP_DBG_MED, "    Smart Unlock:FAILED\n"));
#endif // SERIAL_DBG_MSG
    return FAILURE;
  }

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Unlock:Passed\n"));
#endif // SERIAL_DBG_MSG
  return SUCCESS;
} // SmartUnlock

UINT32 SmartStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;



  //Make Sure NVDIMM is alive
  status = ReadSmbTimeout(host, socket, smbusdev, 0xF, &data, 0x80, 0x80, 10);

  status = SmartUnlock(host, socket, smbusdev);
  if (status == FAILURE) return NVDIMM_ERROR;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Making sure there isn't a Restore in Progress\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, smbusdev, 0x8, &data, (1 << 1), 0, 4*60);
  if (status == FAILURE) return NVDIMM_ERROR;

  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xA, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Status/Event #1(0xA)=0x%x\n",data));
#endif // SERIAL_DBG_MSG
  // Clear the register
  WriteSmbNVDIMM(host, socket, smbusdev, 0xA, &data);

  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xB, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Status/Event #2(0xB)=0x%x\n",data));
#endif // SERIAL_DBG_MSG
  // Clear the register
  WriteSmbNVDIMM(host, socket, smbusdev, 0xB, &data);

  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xE, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    State Information #1(0xE)=0x%x\n",data));
#endif

  status = ReadSmb(host, socket, smbusdev, 0xF, &data);
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    State Information #2(0xF)=0x%x\n",data));
#endif
  if(status == FAILURE) {
#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    FAILED to read State Information Register"));
#endif // SERIAL_DBG_MSG
    return NVDIMM_ERROR;
  }

  if (data & (1 << 2))
    return NVDIMM_RESTORE;
  else
    return NVDIMM_NODATA;
} // SmartStatus


UINT32
SmartRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8 data;

  // Start the Restore
  data = 1 << 4;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x8, &data);

  return SUCCESS;
} // SmartRestoreStart

UINT32
SmartRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Waiting for restore to complete
  status = ReadSmbTimeout(host, socket, smbusdev, 0x8, &data, (1 << 4), 0, 2 * 60);
  if (status == FAILURE) return FAILURE;

  status = ReadSmbTimeout(host, socket, smbusdev, 0xB, &data, 1, 1, 10);
  if (status == FAILURE) return FAILURE;

  // Check for Restore Failure
  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xB, &data);
  if (status == FAILURE) return FAILURE;

  if (data & (1 << 3)) return FAILURE;

  // Clear Trigger Done bit
  WriteSmbNVDIMM(host, socket, smbusdev, 0xB, &data);

  return SUCCESS;
} // SmartRestoreDone


UINT32
SmartArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Waiting for the CAP to charge\n"));
#endif  // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, smbusdev, 0xF, &data, (1 << 3), (1 << 3), 3 * 60);

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Erasing Flash\n"));
#endif  // SERIAL_DBG_MSG
  data = 1 << 5;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x8, &data);

  status = ReadSmbTimeout(host, socket, smbusdev, 0x8, &data, (1 << 5), 0, 10);
  if (status == FAILURE) return FAILURE;

  status = ReadSmbTimeout(host, socket, smbusdev, 0xB, &data, 1, 1, 10);
  if (status == FAILURE) return FAILURE;

  // Clear Trigger Done bit
  WriteSmbNVDIMM(host, socket, smbusdev, 0xB, &data);

  // Check for Erase errors
  status = ReadSmbNVDIMM(host, socket, smbusdev, 0xA, &data);
  if (status == FAILURE)
    return FAILURE;
  if (data & (1 << 1))
    return FAILURE;

#ifdef SERIAL_DBG_MSG
  MspDebugPrint ((MSP_DBG_MED, "    Arming NVDIMM\n"));
#endif  // SERIAL_DBG_MSG
  data = 1;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x7, &data);

  // Verify NVDIMM Armed
  status = ReadSmbTimeout(host, socket, smbusdev, 0xE, &data, 0x80, 0x80, 10);
  if (status == FAILURE) return FAILURE;

  return SUCCESS;
} // SmartArm



#define NVDIMM_INIT_TIMEOUT 203

#define OPEN_PAGE         0x0

#define HOST_MAX_OPERATION_RETRY 0x15

#define CSAVE_TIMEOUT0     0x18
#define CSAVE_TIMEOUT1     0x19

#define PAGE_SWITCH_LATENCY0  0x1A
#define PAGE_SWITCH_LATENCY1  0x1B

#define RESTORE_TIMEOUT0   0x1C
#define RESTORE_TIMEOUT1   0x1D

#define ERASE_TIMEOUT0     0x1E
#define ERASE_TIMEOUT1     0x1F

#define ARM_TIMEOUT0       0x20
#define ARM_TIMEOUT1       0x21

#define FIRMWARE_OPS_TIMEOUT0 0x22
#define FIRMWARE_OPS_TIMEOUT1 0x23

#define ABORT_CMD_TIMEOUT0   0x24
#define ABORT_CMD_TIMEOUT1   0x25


#define NVDIMM_MGT_CMD0      0x40
#define CL_SAVE_STAT          (1<<2)
#define CL_SET_ES_STAT        (1<<7)
#define NVDIMM_MGT_CMD1      0x41
#define NVDIMM_FUNC_CMD0     0x43
#define START_SAVE            (1<<1)
#define START_RESTORE         (1<<2)
#define START_ERASE           (1<<3)
#define ABORT_CURRENT_OP      (1<<4)
#define ARM_CMD             0x45
#define SET_EVENT_NOTIFICATION_CMD 0x47
#define SET_ES_POLICY_CMD   0x49


#define NVDIMM_READY        0x60
#define NVDIMM_CMD_STATUS   0x61
#define SAVE_IN_PROGRESS     (1<<2)
#define RESTORE_IN_PROGRESS  (1<<3)
#define ERASE_IN_PROGRESS    (1<<4)
#define ABORT_IN_PROGRESS    (1<<5)
#define ARM_IN_PROGRESS      (1<<6)

#define ABORT_SUCCESS        (1<<4)
#define ABORT_ERROR          (1<<5)
#define SAVE_STATUS         0x64
#define RESTORE_STATUS0     0x66
#define ERASE_STATUS0       0x68
#define ARM_STATUS0         0x6A
#define FACTORY_DEFAULT_STATUS0        0x6C
#define SET_EVENT_NOTIFICATION_STATUS0 0x6E
#define SET_ES_POLICY_STATUS          0x70
#define SET_ES_POLICY_SUCCESS          (1<<0)
#define SET_ES_POLICY_ERROR            (1<<1)
#define FIRMWARE_OPS_STATUS0          0x71

#define CSAVE_INFO          0x80
#define CSAVE_FAIL_INFO0    0x84
#define CSAVE_FAIL_INFO1    0x85

#define MODULE_HEALTH          0xA0
#define MODULE_HEALTH_STATUS0  0xA1
#define MODULE_HEALTH_STATUS1  0xA2
#define MODULE_OPS_CONFIG      0xAA
#define CSAVE_FAIL_INFO0      0x84
#define CSAVE_FAIL_INFO1      0x85

#define ES_CHARGE_TIMEOUT0 0x10
#define ES_CHARGE_TIMEOUT1 0x11

UINT32 JEDECDumpStatusRegs(
    PSYSHOST          host,
    UINT8             socket,
    struct smbDevice  smbusdev
    )
{
  UINT32 status;
  UINT8  data;

  status = ReadSmbNVDIMM(host, socket, smbusdev, NVDIMM_READY, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (0xA5 != data ){
    MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_READY=0x%x != 0xA5\n",data));
  }
  status = ReadSmbNVDIMM(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[0] - Controller busy\n",data));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[1] - FACTORY_DEFAULT \n",data));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[2] - SAVE in Progress \n",data));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[3] - RESTORE in Progress \n",data));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[4] - ERASE in Progress \n",data));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[5] - ABORT in Progress \n",data));
    if (data & (1 << 6))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[6] - ARM in Progress \n",data));
    if (data & (1 << 7))
      MspDebugPrint ((MSP_DBG_MED, "  NVDIMM_CMD_STATUS[7] - FIRMWARE_OPS in Progress \n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, SAVE_STATUS, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  SAVE_STATUS[0] - SAVE SUCCESS\n",data));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  SAVE_STATUS[1] - SAVE ERROR\n",data));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  SAVE_STATUS[4] - ABORT SUCESS\n",data));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  SAVE_STATUS[5] - ABORT ERROR\n",data));
    if (data & ((1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)))
      MspDebugPrint ((MSP_DBG_MED, "  SAVE_STATUS=0x%x[3:2,7:6] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, CSAVE_INFO, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_INFO[0]-NVM_Data_Valid\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_INFO[1]-START_SAVE_CMD\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_INFO[2]-SAVE_N\n"));
    if (data & (0xf8))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_INFO=0x%x[7:3]-reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, CSAVE_FAIL_INFO0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[0]-VOLTAGE_REGULATOR_FAILED\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[1]-VDD_LOST\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[2]-VPP_LOST\n"));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[3]-VTT_LOST\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[4]-DRAM_NOT_SELF_REFRESH\n"));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[5]-CONTROLLER_HARDWARE_ERROR\n"));
    if (data & (1 << 6))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[6]-NVM_CONTROLLER_ERROR\n"));
    if (data & (1 << 7))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO0[7]-NVM_MEDIA_ERROR\n"));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, CSAVE_FAIL_INFO1, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO1[0]-NOT_ENOUGH_ENERGY_FOR_CSAVE\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_FAIL_INFO1[1]-PARTIAL_DATA_SAVED\n"));
    if (data & (0xfC))//bits 7:2
      MspDebugPrint ((MSP_DBG_MED, "  CSAVE_INFO=0x%x[7:2]-reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, RESTORE_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  RESTORE_STATUS0[0]-RESTORE_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  RESTORE_STATUS0[1]-RESTORE_ERROR\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  RESTORE_STATUS0=0x%x[4] - ABORT SUCESS\n",data));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  RESTORE_STATUS0=0x%x[5] - ABORT ERROR\n",data));
    if (data & ((1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)))
      MspDebugPrint ((MSP_DBG_MED, "  RESTORE_STATUS0=0x%x[3:2,7:6] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, ERASE_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  ERASE_STATUS0[0]-RESTORE_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  ERASE_STATUS0[1]-RESTORE_ERROR\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  ERASE_STATUS0[4] - ABORT SUCESS\n",data));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  ERASE_STATUS0[5] - ABORT ERROR\n",data));
    if (data & ((1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)))
      MspDebugPrint ((MSP_DBG_MED, "  ERASE_STATUS0=0x%x[3:2,7:6] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, ARM_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  ARM_STATUS0[0]-ARM_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  ARM_STATUS0[1]-ARM_ERROR\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  ARM_STATUS0[2]-SAVE_N_ARMED\n",data));
    if (data & (0xF8))
      MspDebugPrint ((MSP_DBG_MED, "  ARM_STATUS0=0x%x[7:3] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, FACTORY_DEFAULT_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  FACTORY_DEFAULT_STATUS0[0]-FACTORY_DEFAULT_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  FACTORY_DEFAULT_STATUS0[1]-FACTORY_DEFAULT_ERROR\n"));
    if (data & (0xFC))
      MspDebugPrint ((MSP_DBG_MED, "  ARM_STATUS0=0x%x[7:2] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, SET_EVENT_NOTIFICATION_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  SET_EVENT_NOTIFICATION_STATUS0[0]-SET_EVENT_NOTIFICATION_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  SET_EVENT_NOTIFICATION_STATUS0[1]-SET_EVENT_NOTIFICATION_ERROR\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  SET_EVENT_NOTIFICATION_STATUS0[2]-PERSISTENCY_ENABLED\n"));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  SET_EVENT_NOTIFICATION_STATUS0[3]-WARNING_THRESHOLD_ENABLED\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  SET_EVENT_NOTIFICATION_STATUS0[4]-VOLTAGE_REGULATOR_FAIL_ENABLED\n"));
    if (data & (0xE0))
      MspDebugPrint ((MSP_DBG_MED, "  SET_EVENT_NOTIFICATION_STATUS0=0x%x[7:5] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, SET_ES_POLICY_STATUS, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  SET_ES_POLICY_STATUS[0]-SET_ES_POLICY_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  SET_ES_POLICY_STATUS[1]-SET_ES_POLICY_ERROR\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  SET_ES_POLICY_STATUS[2]-DEVICE_MANAGED_POLICY_ENABLED\n"));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  SET_ES_POLICY_STATUS[3]-HOST_MANAGED_POLICY_ENABLED\n"));
    if (data & (0xF0))
      MspDebugPrint ((MSP_DBG_MED, "  SET_ES_POLICY_STATUS=0x%x[7:4] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, FIRMWARE_OPS_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  FIRMWARE_OPS_STATUS0[0]-FIRMWARE_OPS_SUCCESS\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  FIRMWARE_OPS_STATUS0[1]-FIRMWARE_OPS_ERROR\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  FIRMWARE_OPS_STATUS0[1]-FIRMWARE_UPDATE_MODE\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  FIRMWARE_OPS_STATUS0[4] - ABORT SUCESS\n",data));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  FIRMWARE_OPS_STATUS0[5] - ABORT ERROR\n",data));
    if (data & (0xC8))
      MspDebugPrint ((MSP_DBG_MED, "  FIRMWARE_OPS_STATUS0=0x%x[3, 7:6] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH[0]-LOST_PERSISTENCY_ERROR\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH[1]-WARNING_THRESHOLD_EXCEEDED\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH[2]-PERSISTENCY_RESTORED\n"));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH[3]-BELOW_WARNING_THRESHOLD\n"));
    if (data & (0xf0))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH=0x%x[7:4] - reserved\n",data));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[0]-VOLTAGE_REGULATOR_FAILED\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[1]-VDD_LOST\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[2]-VPP_LOST\n"));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[3]-VTT_LOST\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[4]-DRAM_NOT_SELF_REFRESH\n"));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[5]-CONTROLLER_HARDWARE_ERROR\n"));
    if (data & (1 << 6))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[6]-NVM_CONTROLLER_ERROR\n"));
    if (data & (1 << 7))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS0[7]-NVM_LIFETIME_ERROR\n"));
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH_STATUS1, &data);
  if (data){
    if (data & (1 << 0))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[0]-NOT_ENOUGH_ENERGY_FOR_CSAVE\n"));
    if (data & (1 << 1))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[1]-INVALID_FIRMWARE_ERROR\n"));
    if (data & (1 << 2))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[2]-CONFIG_DATA_ERROR\n"));
    if (data & (1 << 3))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[3]-NO_ES_PRESENT\n"));
    if (data & (1 << 4))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[4]-ES_POLICY_NOT_SET\n"));
    if (data & (1 << 5))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[5]-ES_HARDWARE_FAILURE\n"));
    if (data & (1 << 6))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[6]-ES_HEALTH_ASSESSMENT_ERROR\n"));
    if (data & (1 << 7))
      MspDebugPrint ((MSP_DBG_MED, "  MODULE_HEALTH_STATUS1[7]-Reserved\n"));
  }
  return status;
}


VOID PrintErrorRegs(
  PSYSHOST          host,
  UINT8             socket,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8  data;

  MspDebugPrint ((MSP_DBG_MED, "Error Registers\n"));
  status = ReadSmbNVDIMM(host, socket, smbusdev, CSAVE_FAIL_INFO0, &data);
  if (status == FAILURE) return ;
  MspDebugPrint ((MSP_DBG_MED, "CSAVE_FAIL_INFO0 = 0x%x\n", data));
  status = ReadSmbNVDIMM(host, socket, smbusdev, CSAVE_FAIL_INFO1, &data);
  if (status == FAILURE) return;
  MspDebugPrint ((MSP_DBG_MED, "CSAVE_FAIL_INFO1 = 0x%x\n", data));
  status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH, &data);
  if (status == FAILURE) return;
  MspDebugPrint ((MSP_DBG_MED, "MODULE_HEALTH = 0x%x\n", data));
  status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH_STATUS0, &data);
  if (status == FAILURE) return;
  MspDebugPrint ((MSP_DBG_MED, "MODULE_HEALTH_STATUS0 = 0x%x\n", data));
  status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH_STATUS1, &data);
  if (status == FAILURE) return;
  MspDebugPrint ((MSP_DBG_MED, "MODULE_HEALTH_STATUS1 = 0x%x\n", data));


}

UINT32 JEDECSwitchPage(
  PSYSHOST          host,
  UINT8             socket,
struct smbDevice  smbusdev,
  UINT8            page
  )
{
  UINT32 status;
  UINT8 currentpage;

  status = ReadSmbNVDIMM(host, socket, smbusdev, OPEN_PAGE, &currentpage);
  MspDebugPrint ((MSP_DBG_MED, "  Current Page=0x%x -> %x\n", currentpage, page));

  // Avoid the Page Switch if we are already on the new Page
  if (currentpage == page)
    return SUCCESS;

  WriteSmbNVDIMM(host, socket, smbusdev, OPEN_PAGE, &page);

  status = ReadSmbTimeout(host, socket, smbusdev, OPEN_PAGE, &currentpage, page, page, 5);
  MspDebugPrint ((MSP_DBG_MED, "New Page=0x%x\n", currentpage));

  if (currentpage == page)
    return SUCCESS;
  else
    return FAILURE;
}




UINT16 JEDECTimeoutInSeconds(
  PSYSHOST          host,
  UINT8             socket,
  struct smbDevice  smbusdev,
  UINT8            offset
  )
{
  UINT32 status;
  UINT16 timeout;
  UINT8 low, high;

  status = ReadSmbNVDIMM(host, socket, smbusdev, offset, &low);
  if (status == FAILURE) {
    MspDebugPrint ((MSP_DBG_MED, "JEDECTimeoutInSeconds: Low Read Failed\n"));
    return 0;
  }

  status = ReadSmbNVDIMM(host, socket, smbusdev, offset+1, &high);
  if (status == FAILURE) {
    MspDebugPrint ((MSP_DBG_MED, "JEDECTimeoutInSeconds: High Read Failed\n"));
    return 0;
  }

  //MspDebugPrint ((MSP_DBG_MED, "JEDECTimeoutInSeconds: high=0x%x, low=0x%x\n", high, low));

  timeout = ((high & 0x7f) << 8) | low;

  // If timescale is milliseconds divide by 1000 to convert to seconds
  if (!(high & (1 << 7)))
    timeout = timeout / 1000;

  //  If the time ends up being less than 1 second, return 1 second
  if (timeout == 0)
    timeout = 1;

  return timeout;
}

  UINT32
    JEDECUnarmNVdimm
    (
    PSYSHOST          host,
    UINT8             socket,
    UINT8             ch,
    UINT8             dimm,
    UINT8             vendorIndex,
  struct smbDevice  smbusdev
    )
  {
      UINT32  status;
      UINT8   data = 0;

      JEDECSwitchPage(host, socket, smbusdev, 0);
      MspDebugPrint ((MSP_DBG_MED, "%a() N%d.C%d.D%d: ", __FUNCTION__, socket, ch, dimm));
      status = ReadSmbNVDIMM(host, socket, smbusdev, ARM_CMD, &data);
      data = 0;
      MspDebugPrint ((MSP_DBG_MED, "ARM_CMD[%x] <- %x", ARM_CMD, data));
      WriteSmbNVDIMM(host, socket, smbusdev, ARM_CMD, &data);
      MspDebugPrint ((MSP_DBG_MED, " ...  NVDIMM UNARMED\n"));

      return 0;

    }

  UINT32 JEDECStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT16 timeout;
  UINT8  data;

  JEDECSwitchPage(host, socket, smbusdev, 0);
  JEDECDumpStatusRegs(host, socket, smbusdev);

  //ReadSpd(host, socket, ch, dimm, NVDIMM_INIT_TIMEOUT, &data);
  //timeout = (UINT16)data;
  timeout = 10;
  MspDebugPrint ((MSP_DBG_MED, "Waiting for NVDIMM to be ready\n"));
  // Wait for the NVDIMM to be Ready
  status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_READY, &data, 0xA5, 0xA5, timeout);
  if (status == FAILURE) {
    JEDECDumpStatusRegs(host, socket, smbusdev);
    return NVDIMM_ERROR;
  }

  timeout = JEDECTimeoutInSeconds(host, socket, smbusdev, CSAVE_TIMEOUT0);
  if (timeout == 0) return NVDIMM_ERROR;
  MspDebugPrint ((MSP_DBG_MED, "Save timeout=%d seconds\n", timeout));

  // Make sure there isn't a Backup in Progress
  MspDebugPrint ((MSP_DBG_MED, "Making sure there isn't a backup in progress\n"));
  status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) {
    JEDECDumpStatusRegs(host, socket, smbusdev);
    return NVDIMM_ERROR;
  }

  JEDECUnarmNVdimm(host, socket, ch, dimm, vendorIndex, smbusdev);

  // Check to see if there is a backup to restore from
  MspDebugPrint ((MSP_DBG_MED, "Checking if there is a backup to restore from\n"));
  status = ReadSmbNVDIMM(host, socket, smbusdev, SAVE_STATUS, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
   
  // If there was a SAVE ERROR return NVDIMM_ERROR
  if (data & (1 << 1)){
    JEDECDumpStatusRegs(host, socket, smbusdev);
    return NVDIMM_ERROR | ERROR_SAVE;
  }
  //  Check if there was a SAVE operation
  if (!(data & (1 << 0)))
    return NVDIMM_NODATA;

  // Verify there is a valid save image
  status = ReadSmbNVDIMM(host, socket, smbusdev, CSAVE_INFO, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  MspDebugPrint ((MSP_DBG_MED, "CSAVE_INFO=0x%x\n", data));
  JEDECDumpStatusRegs(host, socket, smbusdev);

  if (data & (1 << 0))
    return NVDIMM_RESTORE;
  else {
    data = CL_SAVE_STAT;
    status = WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);
    return NVDIMM_ERROR | ERROR_RESTORE;
  }
}


UINT32 JEDECRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8 data;
  UINT16 timeout;


  timeout = 10;
  //1.	NVDIMM_CMD_STATUS0 : wait for Controller NOT busy.
  // Wait for the NVDIMM to be Ready
  MspDebugPrint ((MSP_DBG_MED, "Making sure NVDIMM is idle\n"));
  status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) {
    JEDECDumpStatusRegs(host, socket, smbusdev);
    return NVDIMM_ERROR;
  }

  //2.	NVDIMM_MGT_CMD0 : set CL_RESTORE_STAT to clear restore status register.
  MspDebugPrint ((MSP_DBG_MED, "Clearing Restore Status\n"));
  data = (1 << 3);
  WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

  // 3.	NVDIMM_FUNC_CMD0 : START_RESTORE
  MspDebugPrint ((MSP_DBG_MED, "Starting Restore\n"));
  data = (1 << 2);
  WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_FUNC_CMD0, &data);

  //4.	NVDIMM_CMD_STATUS0 : check for RESTORE in progress bit to be set.If not set, check for RESTORE to be done in RESTORE_STATUS0.If restore not done, retry step 3.

  return SUCCESS;

}

UINT32 JEDECAbortCmd(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev,
  UINT8             CmdStatus,
  UINT16            timeout
  )
{
  UINT8 const StartTries = 3;
  UINT8       tries = 0;
  UINT8       Started = 0;
  UINT8       data;
  UINT32      status;

  MspDebugPrint ((MSP_DBG_MED, "%a()\n", __FUNCTION__));
  do {
    // 1. NVDIMM_FUNC_CMD0: ABORT_CURRENT_OP
    MspDebugPrint ((MSP_DBG_MED, "Starting Abort\n"));
    data = ABORT_CURRENT_OP;
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_FUNC_CMD0, &data);
    //2.  NVDIMM_CMD_STATUS0: check for ABORT in progress bit to be set. If not set, check for ABORT to be done. If abort not done, retry step 1.
    // check for abort start - 2. NVDIMM_CMD_STATUS0
    status = ReadSmbNVDIMM(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data);
    if (status == FAILURE) return FAILURE;
    Started = (0 != (data | ABORT_IN_PROGRESS));
    if (!Started){
      MspDebugPrint ((MSP_DBG_MED, "Abort not started\n"));
      //check for ABORT to be done (int the commands status register)
      status = ReadSmbNVDIMM(host, socket, smbusdev, CmdStatus , &data);
      if (status == FAILURE) return FAILURE;
      if (data | (ABORT_SUCCESS | ABORT_ERROR)) break;
    }

    if (StartTries < (tries++)) return FAILURE;
  } while (!Started);

  // wait for abort to complete - 2.  NVDIMM_CMD_STATUS0
  MspDebugPrint ((MSP_DBG_MED, "Waiting for command to Complete\n"));
  status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) {
    JEDECDumpStatusRegs(host, socket, smbusdev);
  }
  // return if abort was successful or failed
  return status;
}

UINT32 JEDECRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8 data;
  UINT16 timeout;
  UINT8 count;
  UINT8 retries;


  status = ReadSmbNVDIMM(host, socket, smbusdev, HOST_MAX_OPERATION_RETRY, &data);
  if (status == FAILURE) return FAILURE;

  retries = data >> 2;
  retries &= 0x3;
  
  MspDebugPrint ((MSP_DBG_MED, "NVDIMM allows for %d retries\n", retries));

  for (count = 0; count < retries; count++) {

    if (count != 0)
      JEDECRestoreStart(host, socket, ch, dimm, vendorIndex, smbusdev);

    timeout = JEDECTimeoutInSeconds(host, socket, smbusdev, RESTORE_TIMEOUT0);
    if (timeout == 0) continue;
    MspDebugPrint ((MSP_DBG_MED, "Restore timeout=%d\n", timeout));

    MspDebugPrint ((MSP_DBG_MED, "Waiting for Restore to Complete\n"));
    // 5.	NVDIMM_CMD_STATUS0 : wait for RESTORE in progress bit to be clear.If RESTORE in progress bit is not clear after RESTORE_TIMEOUT, abort the RESTORE(see 9.6).
    status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
    if (status == FAILURE) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
     //Abort Restore command
      status = JEDECAbortCmd(host, socket, ch, dimm, vendorIndex, smbusdev, RESTORE_STATUS0, timeout);
      continue;
    }

    //  6.	RESTORE_STATUS0 : check for either RESTORE successful or error.If error or RESTORE aborted, Host may retry up to HOST_MAX_OPERATION_RETRY.
    status = ReadSmbNVDIMM(host, socket, smbusdev, RESTORE_STATUS0, &data);
    if (status == FAILURE) continue;

    if (data & (1 << 0)) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
      return SUCCESS;
    }
    MspDebugPrint ((MSP_DBG_MED, "Restore Status=0x%x\n", data));
  }

  MspDebugPrint ((MSP_DBG_MED, "Restore Failed after %d attempts\n", (count - 1)));
  JEDECDumpStatusRegs(host, socket, smbusdev);
  return FAILURE;

}

UINT32 JEDECEnergySelect(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8  data;
  UINT16 timeout = 10;
  UINT16 tries = 0;
  UINT8  PolicySetSuccess;

  MspDebugPrint ((MSP_DBG_MED, "%a()\n",__FUNCTION__));
  do {
    MspDebugPrint ((MSP_DBG_MED, "Setting Engery Policy to Device Managed\n"));
#ifndef HOST_MANAGED_POLICY
    data = BIT0;		// Device Manage Policy
#else
    data = BIT1;		// Host Manage Policy
#endif
    WriteSmbNVDIMM(host, socket, smbusdev, SET_ES_POLICY_CMD, &data);

    status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);

#ifndef HOST_MANAGED_POLICY
    PolicySetSuccess = BIT2;  // TLN Check [2] : DEVICE_MANAGED_POLICY_ENABLED 
#else
    PolicySetSuccess = BIT3;  // TLN Check [3] : HOST_MANAGED_POLICY_ENABLED 
#endif
    //
    // ignoring bit 0 - SET_ES_POLICY_SUCCESS
    //
    status = ReadSmbTimeout(host, socket, smbusdev, SET_ES_POLICY_STATUS, &data, 0xFE, PolicySetSuccess, timeout);
    MspDebugPrint ((MSP_DBG_MED, "SET_ES_POLICY_STATUS=0x%x\n", data));
    if (status == FAILURE) {
      //failed if error set or success not set
      MspDebugPrint ((MSP_DBG_MED, "SET_ES_POLICY not successful {SET_ES_POLICY_ERROR:%x, SET_ES_POLICY_ERROR:%x} \n", data, (data|SET_ES_POLICY_ERROR), (data|SET_ES_POLICY_ERROR)));
      JEDECDumpStatusRegs(host, socket, smbusdev);
      MspDebugPrint ((MSP_DBG_MED, "Clearing ES POLICY Status\n"));
      data = CL_SET_ES_STAT;
      status = WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);
    }
  } while ((tries--) && (status == FAILURE));
  return status;
}

UINT32 JEDECesCharge(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8  data;
  UINT16 timeout = 10;

  status = JEDECSwitchPage(host, socket, smbusdev, 1);

  timeout = JEDECTimeoutInSeconds(host, socket, smbusdev, ES_CHARGE_TIMEOUT0);
  timeout += JEDECTimeoutInSeconds(host, socket, smbusdev, ES_CHARGE_TIMEOUT1) << 8;
  MspDebugPrint ((MSP_DBG_MED, "Charge timeout=0x%x\n", timeout));

  status = JEDECSwitchPage(host, socket, smbusdev, 0);

  MspDebugPrint ((MSP_DBG_MED, "Waiting for the ES to charge\n"));
  status = ReadSmbTimeout(host, socket, smbusdev, MODULE_HEALTH_STATUS1, &data, 1, 0, timeout);
  if (status == FAILURE) {
    MspDebugPrint ((MSP_DBG_MED, "ES failed to charge\n"));
    //return NVDIMM_ERROR;
    JEDECDumpStatusRegs(host, socket, smbusdev);
  }
  return status;
}

UINT32 JEDECArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8 data;
  UINT16 timeout;
  //int i;


  
  //9.5 ERASE WORKFLOW

    //To initiate an erase, the Host shall do the following :
    //1.	NVDIMM_CMD_STATUS0 : wait for Controller NOT busy.
    // Wait for the NVDIMM to be Ready
  timeout = 10;
  MspDebugPrint ((MSP_DBG_MED, "Making sure NVDIMM is idle\n"));
  status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) {
    JEDECDumpStatusRegs(host, socket, smbusdev);
    return NVDIMM_ERROR;
  }

  status = JEDECEnergySelect(host, socket, ch, dimm, vendorIndex, smbusdev);
  if (status == FAILURE) return NVDIMM_ERROR;
  status = JEDECesCharge(host, socket, ch, dimm, vendorIndex, smbusdev);
  if (status == FAILURE) return NVDIMM_ERROR;

  timeout = JEDECTimeoutInSeconds(host, socket, smbusdev, ERASE_TIMEOUT0);
  if (timeout == 0) return NVDIMM_ERROR;

  MspDebugPrint ((MSP_DBG_MED, "Erase timeout=0x%x\n", timeout));

    //2.	NVDIMM_MGT_CMD0 : set CL_ERASE_STAT to clear restore status register.
  MspDebugPrint ((MSP_DBG_MED, "Clearing Erase Status\n"));
    data = (1 << 4);
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

    //3.	NVDIMM_FUNC_CMD0 : START_ERASE
    MspDebugPrint ((MSP_DBG_MED, "Starting Erase Command\n"));
    data = (1 << 3);
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_FUNC_CMD0, &data);

    //4.	NVDIMM_CMD_STATUS0 : check for ERASE in progress bit to be set.
    MspDebugPrint ((MSP_DBG_MED, "Waiting for Erase to Start\n"));
    status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, (1<<4), (1<<4), 1);
    if (status == FAILURE) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
    }

    //5.	NVDIMM_CMD_STATUS0 : wait for ERASE in progress bit to be clear.If the ERASE in progress bit is not clear after ERASE_TIMEOUT, abort the ERASE(see 9.6).
    MspDebugPrint ((MSP_DBG_MED, "Waiting for Erase to Finish\n"));
    status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
    if (status == FAILURE) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
      return NVDIMM_ERROR;
    }

    //6.	ERASE_STATUS0 : check for either ERASE successful or error.If error or ERASE aborted, Host may retry up to HOST_MAX_OPERATION_RETRY.

    MspDebugPrint ((MSP_DBG_MED, "Checking Erase status\n"));
    status = ReadSmbNVDIMM(host, socket, smbusdev, ERASE_STATUS0, &data);
    
    MspDebugPrint ((MSP_DBG_MED, "ERASE_STATUS0=0x%x\n", data));
    
    //AJB HACK to take out erase status check
    if (!(data & (1 << 0))) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
      return FAILURE;
    }
    
    
  
  //for (i = 0; i < 30; i++) FixedDelay(host, 1000000);

  

        //9.4 ARM WORKFLOW
    timeout = JEDECTimeoutInSeconds(host, socket, smbusdev, ARM_TIMEOUT0);
    if (timeout == 0) return NVDIMM_ERROR;
  //timeout = 120;
    MspDebugPrint ((MSP_DBG_MED, "Arm timeout=0x%x\n", timeout));

  //  To initiate an arm operation, the Host shall do the following :
  //1.	NVDIMM_CMD_STATUS0 : wait for Controller NOT busy.
    
    
    //2.	NVDIMM_MGT_CMD0 : set CL_ARM_STAT to clear arm status register.
    MspDebugPrint ((MSP_DBG_MED, "Clearing Arm status\n"));
    data = (1 << 5);
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

    //3.	ARM_CMD : Set the appropriate bits to reflect the save triggers to be enabled.
    MspDebugPrint ((MSP_DBG_MED, "Starting Arm operation\n"));
    data = (1 << 0);
    WriteSmbNVDIMM(host, socket, smbusdev, ARM_CMD, &data);

    //4.	NVDIMM_CMD_STATUS0 : check for ARM in progress bit to be set.
    //6.	NVDIMM_CMD_STATUS0 : wait for ARM in progress bit to be clear.If ARM in progress bit is not clear after ARM_TIMEOUT, abort the ARM(see 9.6).
    //5.
    //6.	ARM_STATUS0 : check for either ARM successful or error.If successful, the triggers that are enabled for catastrophic save shall be set.
    
    MspDebugPrint ((MSP_DBG_MED, "Waiting for Arm to complete\n"));
    status = ReadSmbTimeout(host, socket, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
    if (status == FAILURE) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
      return FAILURE;
    }
    

    status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH, &data);
    MspDebugPrint ((MSP_DBG_MED, "MODULE_HEALTH=0x%x\n", data));

    status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH_STATUS0, &data);
    MspDebugPrint ((MSP_DBG_MED, "MODULE_HEALTH_STATUS0=0x%x\n", data));

    status = ReadSmbNVDIMM(host, socket, smbusdev, MODULE_HEALTH_STATUS1, &data);
    MspDebugPrint ((MSP_DBG_MED, "MODULE_vHEALTH_STATUS1=0x%x\n", data));

        
    MspDebugPrint ((MSP_DBG_MED, "Checking Arm status\n"));
    status = ReadSmbTimeout(host, socket, smbusdev, ARM_STATUS0, &data, 0x5, 0x5, 30);
    //status = ReadSmbNVDIMM(host, socket, smbusdev, ARM_STATUS0, &data);
    MspDebugPrint ((MSP_DBG_MED, "ARM_STATUS0=0x%x\n", data));
    if (status == FAILURE) {
      JEDECDumpStatusRegs(host, socket, smbusdev);
    }
    
    if (data & (1 << 0)) {
      MspDebugPrint ((MSP_DBG_MED, "Clearing SAVE Status\n"));
      data = CL_SAVE_STAT;
      status = WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

      status = SUCCESS;
    } else {
      status = FAILURE;
    }
    JEDECDumpStatusRegs(host, socket, smbusdev);
    return status;
}


nvDev nv[] =
{
  { // Dummy Entry, so none of the devices have an index of 0
    0xFFFF,
  },

  { // JEDEC
    // Do Not remove this from the first entry of the table.
    // The nvVid/nvDid are designed to never match anything
    0xFFFE,              // nvVid 
    "NOMATCH",        // nvDid  
    6,                  // nvDidCnt
    "JEDEC",           // nvName
    0x08,                // smbusAdr
    0,
    JEDECStatus,
    JEDECRestoreStart,
    JEDECRestoreDone,
    JEDECArm
  },
  
  { /*----         Viking         ----*/
    0x4001,              /* nvVid    */
    "VRA7",            /* nvDid    */
    4,                   /* nvDidCnt */
    "Viking",            /* nvName   */
    0xB,                /* smbusAdr */
    0,         /* Vendor index */
    VikingStatus,
    VikingRestoreStart,
    VikingRestoreDone,
    VikingArm
  },
  { /*----         Viking         ----*/
    0x4001,              /* nvVid    */
    "VRA9",            /* nvDid    */
    4,                   /* nvDidCnt */
    "Viking",            /* nvName   */
    0x8,                /* smbusAdr */
    2,         /* Vendor index */
    VikingStatus,
    VikingRestoreStart,
    VikingRestoreDone,
    VikingArm
  },

  { /*----          Smart        ----*/
    0x9401,              /* nvVid    */
    "SGN",   /* nvDid    */
    3,                  /* nvDidCnt */
    "Smart",             /* nvName   */
    0x0B,                /* smbusAdr */
    0,
    SmartStatus,
    SmartRestoreStart,
    SmartRestoreDone,
    SmartArm
  },
  { /*----          Smart        ----*/
    0x9401,              /* nvVid    */
    "S",   /* nvDid    */
    1,                  /* nvDidCnt */
    "Smart",             /* nvName   */
    0x08,                /* smbusAdr */
    0,
    SmartStatus,
    SmartRestoreStart,
    SmartRestoreDone,
    SmartArm
  },
  { /**----      Agigatech      -----*/
    0x3400,               /* nvVid   3480 BIT7 is cleard on SPD read??? */
    "AGIGA8",             /* nvDid    */
    5,                   /* nvDidCnt max len 18 ch*/
    "Agigatech",          /* nvName   */
    0x02,                 /* smbusAdr    */
    0,
    AgigaStatus,
    AgigaRestoreStart,
    AgigaRestoreDone,
    AgigaArm
  },
  { /**----      Micron      -----*/
    0x2C00,               /* nvVid   3480 BIT7 is cleard on SPD read??? */
    "",             /* nvDid    */
    0,                   /* nvDidCnt max len 18 ch*/
    "Micron",          /* nvName   */
    0x02,                 /* smbusAdr    */
    0,
    AgigaStatus,
    AgigaRestoreStart,
    AgigaRestoreDone,
    AgigaArm
  },
  { /**----      SK Hynix      -----*/
    0xAD00,               /* nvVid   3480 BIT7 is cleard on SPD read??? */
    "",             /* nvDid    */
    0,                   /* nvDidCnt max len 18 ch*/
    "SK Hynix",          /* nvName   */
    0x02,                 /* smbusAdr    */
    0,
    AgigaStatus,
    AgigaRestoreStart,
    AgigaRestoreDone,
    AgigaArm
  },

  { /*----       Netlist         ----*/
    0x1603,              /* nvVid    */
    "NV3",        /* nvDid    */
    3,                  /* nvDidCnt */
    "Netlist",           /* nvName   */
    0x0B,                /* smbusAdr    */
    0,
    NetlistStatus,
    NetlistRestoreStart,
    NetlistRestoreDone,
    NetlistArm
  },
  {
    0x00000000    /* Table End */
  }
};

//SPD defines not in DNV
#define SPD_OPEN_PAGE         0     // SPD data page open  //BYTE 0 keyByte2
#define SPD_REVISION          1     // SPD revision NVDIMMs > 5 //BYTE 1 keyByte2
#define SPD_KEY_BYTE2         3     // Module Type and Thickness (RDIMM or UDIMM)  //BYTE 3 keyByte2
#define SPD_MMID_LSB        117   // Module Manufacturer ID Code, Least Significant Byte
#define SPD_MMID_MSB        118   // Module Manufacturer ID Code, Mostst Significant Byte
#define SPD_MODULE_PN       128   // Module Part Number 128-145
// NVDIMM related SPD fields
#define SPD_NVDIMM_ID_N     174   // If NVDIMM value will be 'N'
#define SPD_NVDIMM_ID_V     175   // If NVDIMM value will be 'V'
#define NV_OFFSET 206
#define MFGID_OFFSET 203
#define PN_OFFSET 201
#define SPD_TYPE_DDR4 TypeDdr4


UINT32
CoreDetectNVDIMM(
PSYSHOST          host,
UINT8             socket,
UINT8             ch,
UINT8             dimm,
struct smbDevice  spd
  )
{
  UINT32  nvIdx = 1;
  UINT8   i;
  UINT8   SPDReg;
  UINT16  SPDMMfgId;
  UINT8 offset;
  UINT16 funcdesc;

  MspDebugPrint ((MSP_DBG_MED, "%a(ch:%d, dimm:%d, spd:%x)\n",__FUNCTION__, ch, dimm, spd.deviceAddr));
  // First look a JEDEC NVDIMM.
  ReadSpd(host, socket, ch, dimm, OPEN_PAGE, &SPDReg);
  MspDebugPrint((MSP_DBG_MED, "OPEN_PAGE=0x%x\n", SPDReg));
  ReadSpd(host, socket, ch, dimm, SPD_REVISION, &SPDReg);
  MspDebugPrint((MSP_DBG_MED, "SPD_REVISION=0x%x\n", SPDReg));
  ReadSpd(host, socket, ch, dimm, SPD_KEY_BYTE2, &SPDReg);
  MspDebugPrint((MSP_DBG_MED, "Keybyte2=0x%x\n", SPDReg));
  if ((SPDReg & 0xF0) == 0x90){
    MspDebugPrint((MSP_DBG_MED, "Found a Hybrid DIMM\n"));
    for (offset = 204; offset < 220; offset++) {
      ReadSpd(host, socket, ch, dimm, (offset + 1), &SPDReg);
      MspDebugPrint((MSP_DBG_MED,  "0x%x=0x%x\n",(offset + 1), SPDReg));
      funcdesc = (UINT16)(SPDReg << 8);
      ReadSpd(host, socket, ch, dimm, offset, &SPDReg);
      MspDebugPrint((MSP_DBG_MED,  "0x%x=0x%x\n", (offset), SPDReg));
      funcdesc |= (UINT16)(SPDReg);
      MspDebugPrint((MSP_DBG_MED,  "funcdesc=0x%x\n", funcdesc));

      if ((funcdesc & 0x3F) == 0x21){
        MspDebugPrint((MSP_DBG_MED,  "Found a JEDEC supported NVDIMM\n"));
        return 1;
      }
        
    }
  }

  spd.deviceAddr = host->OemMrcData.SpdAddressTable[ch][dimm];

  // If it isn't a JEDEC compliant NVDIMM, search for the NV tagging, and then through the legacy Table
  offset = 0;
  if (host->MrcParameters.DramType[ch] == SPD_TYPE_DDR4) {
    MspDebugPrint((MSP_DBG_MED,  "Found a DDR4 DIMM\n"));
    offset = NV_OFFSET;
  } else {
    MspDebugPrint((MSP_DBG_MED,  "Found a DDR3 DIMM\n"));
    offset = 0;
  }

  ReadSpd(host, socket, ch, dimm, (SPD_NVDIMM_ID_N + offset), &SPDReg);
  MspDebugPrint((MSP_DBG_MED,  "NVDIMM:N(%d)=0x%x\n", SPD_NVDIMM_ID_N + offset, SPDReg));
  if (SPDReg != 'N') return NOT_NVDIMM;
  
  /*
  if (SPDReg != 'N') {
    ReadSpd(host, socket, ch, dimm, SPD_NVDIMM_ID_N, &SPDReg);
    offset = 0;
  }
  if (SPDReg != 'N') {
    return NOT_NVDIMM;
  }
  */
  ReadSpd(host, socket, ch, dimm, (SPD_NVDIMM_ID_V + offset), &SPDReg);
  MspDebugPrint((MSP_DBG_MED,  "NVDIMM:V(%d)=0x%x\n", SPD_NVDIMM_ID_V + offset, SPDReg));
  if (SPDReg != 'V') return NOT_NVDIMM;


  if (host->MrcParameters.DramType[ch] == SPD_TYPE_DDR4) offset = MFGID_OFFSET;
  ReadSpd(host, socket, ch, dimm, (SPD_MMID_MSB + offset), &SPDReg);
  SPDMMfgId = (UINT16)(SPDReg << 8);
  ReadSpd(host, socket, ch, dimm, (SPD_MMID_LSB + offset), &SPDReg);
  SPDMMfgId |= (UINT16)(SPDReg &~BIT7);

  MspDebugPrint((MSP_DBG_MED,  "Found NVDIMM[%d][%d][%d] MfgId=0x%x, searching table\n", host, ch, dimm, SPDMMfgId));

  if (host->MrcParameters.DramType[ch] == SPD_TYPE_DDR4) offset = PN_OFFSET;
  while (nv[nvIdx].nvVid != 0) {
    if (nv[nvIdx].nvVid == SPDMMfgId) {
      for (i = 0; i < nv[nvIdx].nvDidCnt; i++) {
        ReadSpd(host, socket, ch, dimm, (SPD_MODULE_PN + i + offset), &SPDReg);
        MspDebugPrint((MSP_DBG_MED,  "Offset %d = 0x%x(%c)\n", (SPD_MODULE_PN + i + offset), SPDReg,SPDReg));
        if (nv[nvIdx].nvDid[i] != SPDReg) break;
      } // i loop

      if (i == nv[nvIdx].nvDidCnt) {
          MspDebugPrint ((MSP_DBG_MED, "NVDIMM(MemSPD): Found a %s NVDIMM vendorIndex=%d\n", nv[nvIdx].nvName, nv[nvIdx].vendorIndex));
        return nvIdx;
      }
    }

    nvIdx++;
  } // while loop

  MspDebugPrint ((MSP_DBG_MIN, "Could not find NVDIMM:%x in NVDIMM List!!!!!\n", SPDMMfgId));
  return NOT_NVDIMM;
}

UINT32
PlatformNvDimmStatus (
    IN  OUT  MMRC_DATA   *MrcData
  )
{
  return CoreNVDIMMStatus(MrcData);
}

UINT32
CoreNVDIMMStatus(
  PSYSHOST host
)
{
  UINT8               socket = 0; //created to limit code changes below
  UINT8               ch;
  UINT8               dimm;
  UINT32              nvVenIdx;
  UINT32              status;
  //struct dimmNvram(*dimmNvList)[MAX_DIMM];
  //struct channelNvram(*channelNvList)[MAX_CH];
  smbDevice           smbusdev;

  MspDebugPrint ((MSP_DBG_MED, "%a()\n",__FUNCTION__));

  // First Loop is to detect status for all NVDIMMs, and start the restore process
  //channelNvList = &host->nvram.mem.socket[socket].channelList;
  for (ch = 0; ch < MAX_CH; ch++) {
    if (host->MrcParameters.ChEnabled[ch] == 0) {
        continue;
    }

    for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
      if (host->MrcParameters.DimmPresent[ch][dimm] == 0 ) {
          continue;
      }

      nvVenIdx = host->NvDimmType[ch][dimm];
      MspDebugPrint ((MSP_DBG_MED, "[%d][%d] nvVenIdx = %d\n",ch,dimm,nvVenIdx));
      if (nvVenIdx) {

        host->NvDimmStatus[ch][dimm] = 0;
        smbusdev.deviceAddr = NVDIMM_FLASH_CONTROLLER(host->OemMrcData.SpdAddressTable[ch][dimm], nv[nvVenIdx].smbusAdr);

        // Check if the NVDIMM has data to restore
        MspDebugPrint ((MSP_DBG_MED, "Calling %s getStatus with vendorIndex %d\n",nv[nvVenIdx].nvName,nv[nvVenIdx].vendorIndex));
        //MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "  busSegment=0x%x, strapAddress0x%x, deviceAddr=0x%x\n", smbusdev.address.busSegment, smbusdev.address.strapAddress, smbusdev.address.deviceAddr));
        status = nv[nvVenIdx].getStatus(host, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);

        switch (status) {
        case NVDIMM_RESTORE:
          MspDebugPrint ((MSP_DBG_MED, "    NVDIMM HAS data to restore\n"));
          host->NvDimmStatus[ch][dimm] |= STATUS_RESTORE_NEEDED;
          break;

        case NVDIMM_NODATA:
          MspDebugPrint ((MSP_DBG_MED, "    NVDIMM does NOT have data to restore\n"));
          break;

        case NVDIMM_ERROR:
        default:
          MspDebugPrint ((MSP_DBG_MIN, "    NVDIMM in ERROR condition\n"));
          host->NvDimmStatus[ch][dimm] |= ERROR_DETECT;
          break;
        }
      }
    } // dimm loop
  } // ch loop
  return 0;
} // PlatformNVDIMMStatus

UINT32
PlatformRestoreNvDimms (
    IN  OUT  MMRC_DATA   *MrcData
  )
{
  UINT8               socket = 0; //created to limit code changes below
  UINT8               ch;
  UINT8               dimm;
  UINT32              status;
  UINT32              nvVenIdx;
  smbDevice           smbusdev;
  UINT8               ChannelSelfRefreshSetup;
  UINT8               RestoreAttempted = 0;
  UINT8               RestoreCodeCached = 0;

  MspDebugPrint ((MSP_DBG_MED, "%a()..\n",__FUNCTION__));
  // Leave if a restore of NVDIMM data is not wanted because of a failure outside the MRC
  if ( 0 == MrcData->restoreNVDIMMS ){
      MspDebugPrint ((MSP_DBG_MED, "  skipped\n"));
      return MMRC_SUCCESS;
  }

  // First Loop to start the restore on all NVDIMMs
  for (ch = 0; ch < MAX_CH; ch++) {
    if (MrcData->MrcParameters.ChEnabled[ch] == 0 ) {
        continue;
    }
    ChannelSelfRefreshSetup = 0;
    for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
      if ( MrcData->MrcParameters.DimmPresent[ch][dimm] == 0 ) {
          continue;
      }

      nvVenIdx = MrcData->NvDimmType[ch][dimm];
      MspDebugPrint ((MSP_DBG_MED, "%a() [%d][%d] VenIdx:0x%x status:0x%x\n",__FUNCTION__, ch, dimm, nvVenIdx, MrcData->NvDimmStatus[ch][dimm]));

      if ((nvVenIdx) && ( MrcData->NvDimmStatus[ch][dimm] & STATUS_RESTORE_NEEDED)) {
        if (0 == RestoreCodeCached) {
          //
          //cache time sensitive DIMM hand off function by executing a restore
          //one time before starting the NVDIMM recovery. This will load all 
          //needed code from the SPI
          //
          NvDimmMrsRestore(MrcData, ch, dimm);
          RestoreCodeCached = 1;
        }
        if (!ChannelSelfRefreshSetup) {
          ChannelSelfRefreshSetup = 1;
          DisablePatrolDemandScrubbing(MrcData, ch);
          PrechargeAllRanks(MrcData, ch);
          //
          // Force ODT off
          //
          MmrcExecuteTask(MrcData, FORCEODT_OFF, SearchSubtaskName(FORCEODT_OFF), ch);
          //
          //Lower CKE# before starting data recovery
          //
          MmrcExecuteTask (MrcData, DISABLE_CKE, SearchSubtaskName (DISABLE_CKE), ch);
        }
        smbusdev.deviceAddr = NVDIMM_FLASH_CONTROLLER(MrcData->OemMrcData.SpdAddressTable[ch][dimm], nv[nvVenIdx].smbusAdr);
        MrcData->NvDimmStatus[ch][dimm] &= ~(STATUS_RESTORE_NEEDED);
        MspDebugPrint ((MSP_DBG_MED, "Calling %s startRestore with vendorIndex %d\n",nv[nvVenIdx].nvName,nv[nvVenIdx].vendorIndex));
        status = nv[nvVenIdx].startRestore(MrcData, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);

        if (status == FAILURE) {
          MspDebugPrint ((MSP_DBG_MIN, "    ERROR starting NVDIMM Recovery for %s vendorIndex %d\n",nv[nvVenIdx].nvName,nv[nvVenIdx].vendorIndex));
          MrcData->NvDimmStatus[ch][dimm] |= ERROR_RESTORE;
        } else {
           MspDebugPrint ((MSP_DBG_MED, "    Started NVDIMM Recovery\n"));
           MrcData->NvDimmStatus[ch][dimm] |= STATUS_RESTORE_INPROGRESS;
        }
      }
    } // dimm loop
  } // ch loop

  //
  // Middle loop to disable self-refresh
  // Standard DIMMS are losing data while in self-refresh waiting for the 
  // NVDIMM recovery to complete. Enabling the memory controller to refresh
  // the DIMMs maintains their data
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if (MrcData->MrcParameters.ChEnabled[ch] == 0) {
      continue;
    }
    MmrcExecuteTask(MrcData, RELEASE_CKE, SearchSubtaskName(RELEASE_CKE), ch);
    PrechargeAllRanks(MrcData, ch);

  }

  // Last Loop is to verify that all NVDIMMs have finished their Restore
  for (ch = 0; ch < MAX_CH; ch++) {
    if ( MrcData->MrcParameters.ChEnabled[ch] == 0 ) {
        continue;
    }

    for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
      if (MrcData->MrcParameters.DimmPresent[ch][dimm] == 0 ) {
          continue;
      }

      nvVenIdx = MrcData->NvDimmType[ch][dimm];
      if ((nvVenIdx) && (MrcData->NvDimmStatus[ch][dimm] & STATUS_RESTORE_INPROGRESS)) {
        RestoreAttempted++;
        smbusdev.deviceAddr = NVDIMM_FLASH_CONTROLLER(MrcData->OemMrcData.SpdAddressTable[ch][dimm], nv[nvVenIdx].smbusAdr);
        MspDebugPrint((MSP_DBG_MED, "Calling %s restoreDone with vendorIndex %d\n", nv[nvVenIdx].nvName, nv[nvVenIdx].vendorIndex));
        MrcData->NvDimmStatus[ch][dimm] &= ~(STATUS_RESTORE_INPROGRESS);
        status = nv[nvVenIdx].restoreDone(MrcData, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);
        if (status == FAILURE) {
          MspDebugPrint ((MSP_DBG_MIN, "    ERROR during NVDIMM Recovery\n"));
          MrcData->NvDimmStatus[ch][dimm] |= ERROR_RESTORE;
        } else {
          MrcData->NvDimmStatus[ch][dimm] |= STATUS_RESTORE_SUCCESSFUL;
        }
      }
    } // dimm loop
  } // ch loop

  if (RestoreAttempted) {
    //cache time sensitive DIMM hand off function
    PlatformCacheMrsRestoreNvDimms(MrcData);
    PlatformMrsRestoreNvDimms(MrcData);
  }

  return MMRC_SUCCESS;
} // PlatformRestoreNVDIMMs

UINT32
PlatformArmNvDimms (
    IN  OUT  MMRC_DATA   *MrcData
  )
{
	return CoreArmNVDIMMs(MrcData);
}

UINT32
CoreArmNVDIMMs(
PSYSHOST host
)
{
  UINT8               socket = 0; //created to limit code changes below
  UINT8               ch;
  UINT8               dimm;
  UINT32              nvVenIdx; // index into nv[] for vender data
  UINT32              status;

  smbDevice           smbusdev;

  MspDebugPrint ((MSP_DBG_MED, "%a()\n",__FUNCTION__));
  if (0 == host->eraseArmNVDIMMS){
      MspDebugPrint ((MSP_DBG_MED, "  skipped\n"));
     return 0;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ( host->MrcParameters.ChEnabled[ch] == 0 ) {
        continue;
    }

    for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
      if ( host->MrcParameters.DimmPresent[ch][dimm] == 0 ) {
          continue;
      }

      nvVenIdx = host->NvDimmType[ch][dimm];
      if (nvVenIdx) {
        // We have an NVDIMM, setup the structures and then call routines
	    smbusdev.deviceAddr = NVDIMM_FLASH_CONTROLLER(host->OemMrcData.SpdAddressTable[ch][dimm], nv[nvVenIdx].smbusAdr);
        //
        // Check if the NVDIMM has data to restore
        //
        MspDebugPrint ((MSP_DBG_MED, "Calling %s arm with vendorIndex %d CH%d DM:%d VenId:%x\n",nv[nvVenIdx].nvName,nv[nvVenIdx].vendorIndex, ch, dimm, host->NvDimmType[ch][dimm]));
        status = nv[nvVenIdx].arm(host, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);
        if (status == FAILURE) {
           MspDebugPrint ((MSP_DBG_MIN, "    ERROR Arming NVDIMM\n"));
           host->NvDimmStatus[ch][dimm] |= ERROR_ARM;
        } else {
           MspDebugPrint ((MSP_DBG_MED, "    NVDIMM armed\n"));
           host->NvDimmStatus[ch][dimm] |= STATUS_ARMED;
        }
      }
    } // dimm loop
  } // ch loop
  MspDebugPrint ((MSP_DBG_MED, "  %a<>\n",__FUNCTION__));
  return 0;
} // PlatformArmNvDimms

VOID PrechargeAllRanks(
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       ch
  )
{
  UINT8 Rank;
  //
  // PRECHARGEALL the existing ranks
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    if (!MrcData->RankEnabled[ch][Rank]) {
      continue;
    }
    MspDebugPrint((MSP_DBG_MED, "  C%dR%d PRE_A\n", ch, Rank));
    PrechargeAll(MrcData, ch, Rank);
  }
}

VOID NvDimmMrsRestore(
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       ch,
  IN        UINT8       dimm
)
{
  PrechargeAllRanks(MrcData, ch);
  JedecDllOffToOnFlow(MrcData, ch, dimm);
  //
  // Update all MRS register in the DIMM
  //
  JedecInitFunction (MrcData, 0, 0, ch);

  if (MrcData->PdaEnable) {
    UINT8   IndCh = ch;
    UINT8   IndRk;
    UINT8   IndSt;
    UINT32  Value;

    if (MrcData->ExecuteTxVrefSetup) {
      if (MrcData->MrcParameters.DramType[IndCh] == TypeDdr4) {
        for (IndRk = 0; IndRk < MAX_RANKS; IndRk++) {
          if (!MrcData->RankEnabled[IndCh][IndRk]) continue;
          for (IndSt = 0; IndSt < MAX_STROBES; IndSt++) {
            if (!IsStrobeValid (MrcData, IndCh, IndRk, IndSt, FALSE, NULL) ||
              ((MrcData->MaxDqCache[IndCh][IndRk] == MAX_STROBE_X8_DEVICES) && (IndSt > MAX_LOOP_STROBE_X8_DEVICES)) ||
              ((MrcData->PdaEnable == FALSE) && (IndSt > 0))) {
              continue;
            }
            //
            // Read from cache and write the MRS
            //
            Value = MrcData->CacheTxVref[IndCh][IndRk][IndSt];
            GetSetDdrioGroup2(MrcData, IndCh, IndRk, IndSt, TxVref, CMD_SET_VAL_FC_UC, &Value);
          }
        }
      } // RestoreTxVref
    }
  }
}

VOID PlatformMrsRestoreNvDimms(
  IN  OUT   MMRC_DATA   *MrcData)
{
  UINT8       ch;
  UINT8       dimm;
  UINT32      nvVenIdx; // index into nv[] for vender data

  for (ch = 0; ch < MAX_CH; ch++) {
    if ( MrcData->MrcParameters.ChEnabled[ch] == 0 ) {
        continue;
    }

    for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
      if ( MrcData->MrcParameters.DimmPresent[ch][dimm] == 0 ) {
          continue;
      }

      //
      // Leave if not a NVDIMM or restore was not attempted.
      // Restore MRS registers no matter if restore was successful or failed
      nvVenIdx = MrcData->NvDimmType[ch][dimm];
      if ((0 == nvVenIdx) ||
          (0 == (MrcData->NvDimmStatus[ch][dimm] & (STATUS_RESTORE_SUCCESSFUL | ERROR_RESTORE)))
         ){
          continue;
      }

      NvDimmMrsRestore(MrcData, ch, dimm);

    } //dimm

    RestorePatrolDemandScrubbing (MrcData, ch);
  } //ch
}

UINT8
PlatformMrsRestoreNvDimmsEndMarker ()
/*
  This function needs to immediately follow the
  ExitSelfRefresh() funtion.  This provides an end label that C
  can use to read the entire function (ExitSelfRefresh) into
  cache before executing it.
*/
{
  volatile UINT8 status;
  status = 0;
  return status;
}


VOID PlatformCacheMrsRestoreNvDimms(
    IN  OUT   MMRC_DATA   *MrcData)
{
  VOID     (*functionPtr)(PSYSHOST host);
  UINT8    (*endFunctionPtr)();
  UINT32   **dataPtr;
  UINT32   **endDataPtr;
  volatile UINT32 data;

  //
  // First read the entire function into cache so that there are
  // no flash reads during the S3 exit flow.  A flash read takes
  // too long and will violate timing parameters.
  //
  functionPtr    = PlatformMrsRestoreNvDimms;
  endFunctionPtr = PlatformMrsRestoreNvDimmsEndMarker;
  dataPtr        = (UINT32 **)(&functionPtr);
  endDataPtr     = (UINT32 **)(&endFunctionPtr);
  //
  // Check to make sure the compiler didn't reorganize the end
  // function marker to someplace before the critical function.
  // If it did, then set the endFunctionPtr to a place 1000 hex
  // bytes after the start of the function.  This is more than
  // enough bytes to read the entire function into processor
  // cache.
  if (*endDataPtr <= *dataPtr) {
    *endDataPtr = *dataPtr + 0x1000;
  }

  while (*dataPtr < *endDataPtr) {
    data = **dataPtr;
    (*dataPtr)++;
  }
}

VOID EccInitializeNeeded(
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Chan,
  IN        UINT8       Dimm,
  OUT   UINT8       *Skip)
{

  MspDebugPrint((MSP_DBG_MED, "%a()\n",__FUNCTION__));
  *Skip = 0;

  //
  // In NVDIMM_MODE, ECC init is always done for standard DIMMs and each NVDIMM is an independent region.
  // Only Addresses in NVDIMMS that successfully recovered data returns Skip set.
  //
  if ( 1 == MrcData->MrcParameters.ChEnabled[Chan] ) {
  if ( 1 == MrcData->MrcParameters.DimmPresent[Chan][Dimm] ) {
    if ( (NVDIMM_MODE == MrcData->NonVolMemMode) && (MrcData->NvDimmType[Chan][Dimm])) {
    if ( 0 == (MrcData->NvDimmStatus[Chan][Dimm] & ERROR_RESTORE) ) {
      //
      // Data has been restored so don't init ECC
      //
      *Skip = 1;
    }
    }
  }
  }
  MspDebugPrint((MSP_DBG_MED, "%a(Chan:%x, Dimm:%x) ECC init:%x\n",__FUNCTION__, Chan, Dimm, *Skip));

}

UINT32
PlatformUpdateBootMode (
    IN  OUT  MMRC_DATA   *MrcData
  )
{
  UINT8    ch;
  UINT8    dimm;

  MspDebugPrint((MSP_DBG_MIN, "%a()\n",__FUNCTION__));
  //
  //No change to the boot mode if none of the NonVolatile modes are enabled
  //
  if (!MrcData->NonVolMemMode){
    return NVDIMM_NO_CHANGE;
  }

  if (NVDIMM_MODE != MrcData->NonVolMemMode){
    return ( (MrcData->AdrRecoveryDetected) ? NVDIMM_ADR_RECOVERY : NVDIMM_NO_CHANGE );
  }

  PlatformNvDimmStatus(MrcData);

  for (ch = 0; ch < MAX_CH; ch++) {
    if (1 == MrcData->MrcParameters.ChEnabled[ch]) {
      for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
        if ( (1 == MrcData->MrcParameters.DimmPresent[ch][dimm]) && (MrcData->NvDimmType[ch][dimm]) ) {
          if (STATUS_RESTORE_NEEDED | MrcData->NvDimmStatus[ch][dimm]) {
            return NVDIMM_MODE_WANTED;
          }//STATUS_RESTORE_NEEDED
        }//Channel->DimmPresent
      }//dimm
    } //Channel->Enabled
  } //ch

  return NVDIMM_NO_CHANGE;
}

UINT32 PlatformDetectNvDimm (
    IN  OUT  MMRC_DATA   *MrcData,
    IN       UINT8 node,
    IN       UINT8 ch,
    IN       UINT8 dimm
  )
{
  const UINT8     socket = 0; //DNV only one socket
  smbDevice spd;  //created to limit code changes below
  UINT32 result = NOT_NVDIMM;

  if (NVDIMM_MODE == MrcData->NonVolMemMode) {
    spd.deviceAddr = MrcData->OemMrcData.SpdAddressTable[ch][dimm];
    result =  CoreDetectNVDIMM(MrcData, socket, ch, dimm, spd);
  }
  MspDebugPrint ((MSP_DBG_MED, "%a() CH%d DIMM:%d %x\n",__FUNCTION__, ch, dimm, result));
  return result;
}

UINT32 ReadSpd(
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch,
  UINT8       dimm,
  UINT16      byteOffset,
  UINT8       *data
  )
{
  smbDevice spd;  //created to limit code changes below

  spd.deviceAddr = host->OemMrcData.SpdAddressTable[ch][dimm];
  return ReadSmb(host, socket, spd, byteOffset, data);
}

UINT32 ReadSmb (
    IN  OUT  MMRC_DATA   *host,
    IN       UINT8       socket,
    IN       smbDevice   spd,
    IN       UINT16      SPD_FIELD,
    IN       UINT8*      SPDReg
    )
{
  UINT8 Length = 1;
  MMRC_STATUS Status;
  Status = MrcSmbusExec (
      host->SmbusBar,
      spd.deviceAddr,
      SmbusReadByte,
      SPD_FIELD,
      &Length,
      SPDReg
      );
  return Status;
}

UINT32 WriteSmb (
    IN  OUT  MMRC_DATA   *host,
    IN       UINT8       socket,
    IN       smbDevice   spd,
    IN       UINT16      SPD_FIELD,
    IN       UINT8*      SPDReg
    )
{
  UINT8 Length = 1;
  MMRC_STATUS Status;


  Status = MrcSmbusExec (
      host->SmbusBar,
      spd.deviceAddr,
      SmbusWriteByte,
      SPD_FIELD,
      &Length,
      SPDReg
      );
  return Status;
}

VOID FixedDelay (
    IN  OUT  MMRC_DATA   *host,
    IN       UINT32      duration
    )
{
  UINT8 i;
  const UINT8 adjustment = 16;
  
  //
  // stall appears to be returning in one tenth the time requested
  //
  for (i=0; i<adjustment; i++) {
      McStall(duration);
  }
}




