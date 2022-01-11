/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _SA_PLATFORM_CONFIG_H_
#define _SA_PLATFORM_CONFIG_H_

#define SA_POLICY_PPI_REVISION_1   1

#ifndef SA_MC_MAX_SOCKETS
#define SA_MC_MAX_SOCKETS 4
#endif

#define S3_TIMING_DATA_LEN          9
#define S3_READ_TRAINING_DATA_LEN   16
#define S3_WRITE_TRAINING_DATA_LEN  12

//!!!This macro has the copy with same purpose @/HarrisonvillePkg/Include/SetupVariable.h and @/DenvertonPkg/SystemAgent/Include/SaDataHob.h
#define COS_CAT_AGENT_COUNT	16


#ifndef S3_RESTORE_DATA_LEN
#define S3_RESTORE_DATA_LEN (S3_TIMING_DATA_LEN + S3_READ_TRAINING_DATA_LEN + S3_WRITE_TRAINING_DATA_LEN)
#endif // S3_RESTORE_DATA_LEN

#define SA_PLATFORM_CONFIG_REVISION 1

///
/// Subsystem Vendor ID / Subsystem ID
///
typedef struct _SA_DEFAULT_SVID_SID{
  UINT16         SubSystemVendorId;
  UINT16         SubSystemId;
} SA_DEFAULT_SVID_SID;
///
/// SA Platform Data Structure
///
typedef struct _SA_PLATFORM_CONFIG{
  UINT16  BlockId;                           // GUID should be used
  UINT8   BlockRevision;                     // revision
  UINT16  BlockSize;                         // do we need this for 4bytes or 2 B should be enuf?
  UINT8   Revision;
  UINT8   SpdAddressTable[SA_MC_MAX_SOCKETS];///< Memory DIMMs' SPD address for reading SPD data
  UINT32  MchBar;                            ///< Address of System Agent MCHBAR
  UINT32  SmbusBar;                          ///< Address of System Agent SMBUS BAR
  UINT32  TsegSize;                          ///< Size of TSEG in bytes
  UINT16  BoardId;                           ///< Platform Board ID
  BOOLEAN BdatEnable;                        ///< This field enables the generation of the BIOS DATA ACPI Tables. Please refer to the MRC documentation for more details
  SA_DEFAULT_SVID_SID         DefaultSvidSid;
  UINT16  MmioSize;                          ///< Reserved MMIO space for Graphics
  UINT8   EnableSsaClockGating;              ///< Used to get clock gating policy value from set up option
  UINT8   MsiRedirAlgorithm;                 ///< MSI Algorithm
  UINT8   SchedulerLatency;					 ///< Scheduler Latency value
  UINT8   CosCatAgent[COS_CAT_AGENT_COUNT];
  UINT8   BestEffortMaxLat;
} SA_PLATFORM_CONFIG;


#endif // _SA_PLATFORM_CONFIG_H_
