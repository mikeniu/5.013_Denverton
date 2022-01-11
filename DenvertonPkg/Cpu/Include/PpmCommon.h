/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PpmCommon.h

Abstract:

  IST/PPM reference code definitions.

  Acronyms:
    PPM   Platform Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology

--*/
#ifndef _PPM_COMMON_H_
#define _PPM_COMMON_H_

#include <PiDxe.h>

//
// Forward Declarations
//
typedef union _FVID_TABLE FVID_TABLE;

//
// Limit the number of P-states to 16. Up to Windows 7, the OS allocates 1KB buffer for the PSS package.
// So the maximum number of P-state OS can handle is 19. This is not an OS issue. Having too many P-states 
// is not good for the system performance.
//
#define FVID_MAX_STATES     20 //  ACPI Max P-States = 32

#define FVID_MIN_STEP_SIZE  1

//
// Structure declarations
//

typedef struct _FVID_HEADER {
  UINT32  Stepping;     // Matches value returned by CPUID function 1
  UINT16  MaxVid;       // Matches VID_MAX field in PERF_STS_MSR
  UINT16  MaxBusRatio;  // Matches BUS_RATIO_MAX field in PERF_STS_MSR
  UINT16  EistStates;    // Number of states of FVID (N)
} FVID_HEADER;

typedef struct _FVID_STATE {
  UINT32  State;        // State Number (0 - N-1)
  UINT16  Vid;          // VID_SEL value to be written to PERF_CTL
  UINT16  BusRatio;     // BUS_RATIO_SEL value to be written to PERF_CTL
  UINT32  Power;        // Typical power consumed by CPU in this state
} FVID_STATE;

typedef union _FVID_TABLE {
  FVID_HEADER FvidHeader;
  FVID_STATE  FvidState;
  UINT64      FvidData;
} FVID_TABLE;

//
// Processor Power Management Flags Bit Definitions:
//
#define PPM_GV3                           (1 << 0)  // (EIST) - Enhanced Intel SpeedStep Technology.a.k.a Geyserville 3.
#define PPM_C1                            (1 << 1)  // C1 enabled, supported.
#define PPM_C1E                           (1 << 2)  // C1E enabled.
#define PPM_C3                            (1 << 3)  // C3 enabled, supported.
#define PPM_C6                            (1 << 4)  // C6 enabled.
#define PPM_C7                            (1 << 5)  // C7 enabled, supported.

#define PPM_C8                            (1 << 7)  // C8 enabled, supported.
#define PPM_C9                            (1 << 8)  // C9 enabled, supported.
#define PPM_C10                           (1 << 9)  // C10 enabled, supported.

#define PPM_TIMED_MWAIT                   (1 << 15) // Timed Mwait support

#define PPM_TM                            (1 << 16) // Thermal Monitor 1 & Thermal Monitor 2.
#define PPM_TM1                           (1 << 16) // Thermal Monitor 1.
#define PPM_TM2                           (1 << 17) // Thermal Monitor 2.

#define PPM_PROC_HOT                      (1 << 19) // Bi-directional ProcHot.
#define PPM_TSC                           (1 << 20) // TSC 64-bit writeable.
#define PPM_MWAIT_EXT                     (1 << 21) // MONITIOR/MWAIT Extensions supported.
#define PPM_QUAD                          (1 << 23) // Quad-core processor.

#define PPM_CMP                           (1 << 24) // CMP.
#define PPM_TURBO                         (1 << 25) // Turbo Mode
#define PPM_EMTTM                         (1 << 26) // Enhanced Multi Threaded Thermal Monitoring
#define PPM_DYNAMIC_FSB                   (1 << 27) // Dynamic FSB Switching

#define PPM_TSTATES                       (1 << 28) // CPU throttling states
#define PPM_BOOT_P_ST_HFM                 (1 << 29) // Set boot P-state to HFM when EIST is enabled
#define PPM_S0ix                          (1 << 30) // S0ix is enabled

#define PPM_FAST_BRK_SNOOP                0x200000 // BTI 21: Fast break Snoop Enable
#define PPM_FAST_BRK_INTERRUPT            0x400000 // BIT 22: Fast break Interrupt Enable

#define PPM_C_STATES                      (PPM_C1 + PPM_C1E + PPM_C3 + PPM_C6 + PPM_C7 + PPM_C8 + PPM_C9 + PPM_C10)
#define PPM_ENHANCED_C_STATES             (PPM_C1E)

#define CST_UPDT_TSC                      (1 << 1)
#define CST_DATA_TBASE                    3579545

#define PPM_ENABLE                        1
#define PPM_DISABLE                       0

#define MCHBAR_BUS                              0
#define MCHBAR_DEV                              0
#define MCHBAR_FUN                              0

#define MCHBAR64_SHIFT_HI                       32

#define REGISTER_MCHBAR_LO                      0x0048
#define REGISTER_MCHBAR_HI                      0x004C
#define BIOS_MAILBOX_DATA_MCHBAR                0x7080
#define BIOS_MAILBOX_INTERFACE_MCHBAR           0x7084
//
// DNV Punit reserves 2K of its Persistent RAM for storing the crash log
//
#define B2P_PUNIT_RAM_SIZE                      0x800
#define B2P_4B_FRAMES_PER_LINE                  0x8
//
// DNV B2P Mailbox Commands
//
#define MBX_CMD_READ_CRASHDUMP                  0x37
//
// DNV B2P Mailbox Completion Code Encodings
//
typedef enum {
  B2P_SUCCESS = 0,
  B2P_ILLEGAL_CMD,
  B2P_ILLEGAL_DATA,
  B2P_ILLEGAL_TIMEOUT,
  B2P_ILLEGAL_CMD_LOCKED,
  B2P_ILLEGAL_ADDR,
  B2P_ILLEGAL_EVENT_BUSY
} MBX_CMD_ERRORS_ENUM;

typedef union _B2P_MAILBOX_INTERFACE {
  UINT32 Data;
  struct {
    // This field contains the SW request command or the PCODE response code depending on the setting of RUN_BUSY.
    // Bits[7:0], RW/V, default = 0x0
    UINT32 Command : 8;
    // This field is used to specify an additional parameter to extend the command when needed.
    // Bits[28:8], RW/V, default = 0x0
    UINT32 Address : 21;
    // RSVD
    // Bits[30:29], RO, Default = 0x0
    UINT32 Reserved : 2;
    // SW may write to the two mailbox registers only when RUN_BUSY is clear(0).
    // Setting RUN_BUSY to 1 will pend a Fast Path event to Pcode. After setting this bit SW will poll this bit until it is cleared.
    // PCODE will clear RUN_BUSY after updating the mailbox registers with the result and error code
    // Bits[31:31], RW/1S/V, Default = 0x0
    UINT32 RunBusy : 1;
  } Bits;
} B2P_MAILBOX_INTERFACE;

#define B2P_HEADER_WARM_RESET                   BIT0
#define B2P_HEADER_MCA_ERROR                    BIT2
#define B2P_HEADER_MANUAL_INITIATED             BIT3
#define B2P_HEADER_ERROR_BITS_MASK              ( BIT0 | BIT2 | BIT3 )
#endif
