/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmErrorHandler.h

Abstract:


------------------------------------------------------------------*/

#ifndef _SMM_ERROR_HANDLER_H
#define _SMM_ERROR_HANDLER_H

#ifndef EFI_RUNTIMESERVICE
#define EFI_RUNTIMESERVICE
#endif
#ifndef __FFS__H__
#define __FFS__H__
#endif
#ifndef __PEI__H__
#define __PEI__H__
#endif
#ifndef __EFI__H__
#define __EFI__H__
#endif

//
// catagorized error Types for logging and viewing.
//
#define INVALID_ERROR_TYPE        0x00
#define NONFATAL_ERROR_TYPE       0x01
#define FATAL_ERROR_TYPE          0x02
#define COR_ERROR_TYPE            0x03
#define SERROR_TYPE               0x04
#define PERROR_TYPE               0x05
#define DPERROR_TYPE              0x06
#define REC_MASTER_ABORT_TYPE     0x07
#define REC_TARGET_ABORT_TYPE     0x08
#define REC_SERROR_TYPE           0x09
#define REDUNDANCY_ERROR_TYPE     0x0A
#define LEGACY_ERROR_TYPE         0x0B
#define EXTENDED_ERROR_TYPE       0x0C
#define RECOVERABLE_ERROR_TYPE    0x0D
#define MISCERROR_TYPE            0xEE
#define ERROR_CLOAKING_DISABLED   0x00
#define ERROR_CLOAKING_ENABLED    0x01
#define MSR_B_SMM_MCA_CTL         0x52
#define  B_ERROR_CLOAKING         BIT0

typedef struct {
  UINT8       ErrorSource;        // PCI or CSI or Mem or Processor etc..
  UINT8       ErrorType;          // PCI
  UINT8       Segment;            // Segment
  UINT8       Bus;                // Bus
  UINT8       Device;             // Device
  UINT8       Function;           // Func
  UINT16      Reg;                // Register Number
  UINT64      Value;              // Value
  UINT16      OEMEvData1;         // More error info
} PCI_PCIE_ERROR_LOCAL_STRUC;

typedef struct _ERROR_LOG_SETUP_DATA {
  UINT8  SystemErrorEn;
  UINT8  ErrorCloakingEnable;
  UINT8  MemoryErrorLog;
  UINT8  MemoryCorrErrorLog;
  UINT8  MemoryUnCorrErrorLog;
  UINT8  PCIeErrorLog;
  UINT8  PCIeCorrErrorLog;
  UINT8  PCIeNFErrorLog;
  UINT8  PCIeFatalErrorLog;
  UINT8  PCIeSystemErrEnable;
  UINT8  PCIeParityErrEnable;
  UINT8  RCECErrorLog;
  UINT8  BUnitErrorLog;
  UINT8  WheaSupport;
  UINT8  WheaEinj_05Extn;
  UINT8  WheaLogging;
}ERROR_LOG_SETUP_DATA;

typedef enum {
  Unknown       = 0,
  PciSource ,
  CsiSource,
  MemSource,
  ProcSource,
  UncoreSource
} ERROR_SOURCE;
// Denverton Aptiov override Start - EIP#249150
// CSI Error types
#define MC_ERROR_TYPE              0x00
#define IERR_ERROR_TYPE            0x01

#define SMM_MCA_CONTROL 0x52 // Denverton AptioV Override - EIP#275412
typedef struct {
     UINT8                                   Source;
     UINT8                                   Type;
     MEMORY_DEV_INFO                         *MemInfo;
} MEM_ERROR_LOG_INFO;

typedef union {
   struct{
      UINT64 mca_code:16;       /* MCA architected error code */
      UINT64 ms_code:16;        /* Model specific error code */
      UINT64 other_info:6;      /* Intel Internal information */
      UINT64 other_info_1:15;   /* Intel Internal information */
      UINT64 rsvd:4;            /* Reserved */
      UINT64 pcc:1;             /* processor context corrupted */
      UINT64 addrv:1;           /* MCi_ADDR valid */
      UINT64 miscv:1;           /* MCi_MISC valid */
      UINT64 en:1;              /* error reporting enabled */
      UINT64 uc:1;              /* uncorrected error */
      UINT64 over:1;            /* error overflow */
      UINT64 val:1;             /* MCi_STATUS valid */
   } Bits;
   UINT64 Data;
} IA32_MCI_STATUS_MSR_STRUCT;

typedef union {
   struct{
      UINT64 bank_count:8;   /* number of MCA banks */
      UINT64 mcg_ctl_p:1;    /* control MSR present */
      UINT64 mcg_ext_p:1;    /* extended MSRs present */
      UINT64 mcg_cmci_p:1;   /* corrected MC error counting/signaling present */
      UINT64 mcg_tes_p:1;    /* threshold based error status present */
      UINT64 rsvd1:4;        /* Reserved */
      UINT64 mcg_ext_cnt:8;  /* Number of Extended Machine Check State registers */
      UINT64 mcg_ser_p:1;    /* software error recovery support present */
      UINT64 rsvd2:39;       /* Reserved */
   } Bits;
   UINT64 Data;
} IA32_MCG_CAP_MSR_STRUCT;

typedef union {
   struct{
      UINT64 restart_ip_valid:1;   /* Restart IP Valid flag */
      UINT64 err_ip_valid:1;       /* Error IP Valid flag */
      UINT64 mc_progress:1;        /* Machine Check In Progress flag */
      UINT64 rsvd2:61;             /* Reserved */
   } Bits;
   UINT64 Data;
} IA32_MCG_STATUS_MSR_STRUCT;


typedef struct{
    UINT8                         CpuIndex;
    UINT8                         BankIndex;
    
    // Global Machinge Check Control MSRs
    IA32_MCG_CAP_MSR_STRUCT       McgCapMsr;
    IA32_MCG_STATUS_MSR_STRUCT    McgStatusMsr;
    
    // Machine check error reporting MSRs
    UINT64                        MciCtlMsr;
    IA32_MCI_STATUS_MSR_STRUCT    MciStatusMsr;
    UINT64                        MciAddrMsr;
} MC_DEV_INFO;

typedef struct {
     UINT8                                   Source;
     UINT8                                   Type;
} CSI_GENERIC_ERROR_LOG_HEADER;

typedef struct {
    UINT8                                   Source;
    UINT8                                   Type;
    MC_DEV_INFO                             *McDevInfo;
} MC_ERROR_LOG_INFO;

typedef union {
   struct{
      UINT32 cpu0_ierr:1;   /* IERR from the CPU module 0 */
      UINT32 cpu1_ierr:1;   /* IERR from the CPU module 1 */
      UINT32 cpu2_ierr:1;   /* IERR from the CPU module 2 */
      UINT32 cpu3_ierr:1;   /* IERR from the CPU module 3 */
      UINT32 p_ierr:1;      /* IERR from the P Unit */
      UINT32 rsvd:27;       /* Reserved */
   } Bits;
   UINT32 Data;
} IERR_STATUS_STRUCT;

typedef struct {
    UINT8                                   Source;
    UINT8                                   Type;
    IERR_STATUS_STRUCT                      *IerrStatusInfo;
} IERR_ERROR_LOG_INFO;

// end of Whea variables
// Denverton Aptiov override End - EIP#249150


extern ERROR_LOG_SETUP_DATA                    mErrLogSetupData;

//
// PCI/PCIe error settings and status
//
extern BOOLEAN  mNmiOnSerr;
extern BOOLEAN  mNmiOnPerr;
extern BOOLEAN  mPciSerrDetected;
extern BOOLEAN  mPciPerrDetected;
extern BOOLEAN  mPcieFatalErrDetected;
extern BOOLEAN  mNumCorrectablePcieErrs;
extern BOOLEAN  mUCEccDetected;
extern BOOLEAN  mUCNonStdErrDetected;
extern BOOLEAN  mAllowClearingOfPcieDeviceStatus;
extern BOOLEAN  mAllowClearingOfUEStatus;
extern BOOLEAN  mWheaPlatformSupportAvailable;
extern EFI_WHEA_PLATFORM_SUPPORT_PROTOCOL *mWheaPlatformSupport;

//
// Global export to chipset modules
//
extern EFI_SMM_BASE2_PROTOCOL *SmmBase;
extern EFI_SMM_SYSTEM_TABLE2* mSmst;
extern EFI_HANDLE			  mImageHandle;
//
// Error Handling Routine Prototypes
//
EFI_STATUS
ElogIohErrorHandler (
  VOID
  );

EFI_STATUS
ElogIchErrorHandler (
  VOID
  );

EFI_STATUS
ErrorHandlerMain 
(
  IN  EFI_HANDLE    DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommunicationBuffer,
  IN OUT UINTN      *SourceSize
);
// Denverton AptioV Override Start - EIP#275412
VOID
ClearMCErrors(
  );
/*++

Routine Description:

  Clear all Machine check errors

Arguments:
  
  none

Returns:

  none

--*/

VOID
EFIAPI
DetectLastBootMCError (
  IN  VOID *Buffer
  );
/*++
Routine Description:

  This function is used to Detects, if any machine check error has 
  occurred on the current processor core

Arguments:
  
  CpuIndex is passed as input vai Buffer

Returns:

  none
--*/

BOOLEAN
EFIAPI
ELogCSIErrorHandler (
  VOID
  );
/*++
Routine Description:

  This function is used to Check the occurance of any CSI errors

Arguments:
  
  none

Returns:

  ErrorDetected status
--*/

VOID
EFIAPI
HandleLastBootErrors (
  VOID
  );
/*++
Routine Description:

  This function is used to Collect information from the last boot 
  and record if any error has occured

Arguments:
  
  none

Returns:

  none
--*/
// Denverton AptioV Override End - EIP#275412

EFI_STATUS
UpdateErrorCloaking (
  VOID
  );

EFI_STATUS
RegisterErrorSMIHandler (
  VOID
  );
#endif

