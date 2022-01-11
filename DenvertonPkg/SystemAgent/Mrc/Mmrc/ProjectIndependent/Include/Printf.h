/** @file
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
**/

#ifndef _printf_h
#define _printf_h
#include "MmrcProjectDefinitions.h"
#if defined SIM || defined JTAG
#include <stdarg.h>
#endif
#if !defined SIM && !defined JTAG
#if defined(EFI64) || defined(EFIx64)
#include <Library\DebugLib.h>
#include <Library\PeiServicesLib.h>
#include <Library\SerialPortLib.h>
#include <Library\PrintLib.h>
#endif
#endif 

#if DEBUG_MSG && EFI_DEBUG // Denverton Aptiov override - Release mode build
#define MspDebugPrint(dbgInfo) rcPrintf dbgInfo
#else
#define MspDebugPrint(dbgInfo)
#endif

#if ERROR_MESSAGES && EFI_DEBUG // Denverton Aptiov override - Release mode build
#define MmrcErrorPrint(dbgInfo) rcPrintf dbgInfo
#else
#define MmrcErrorPrint(dbgInfo)
#endif

#define MSP_DBG_NONE           0
#define MSP_DBG_MIN            BIT0                                  // Show only basic flow through MRC and training results.
#define MSP_DBG_MED            BIT1                                  // Show eye diagrams and other training info useful for EV.
#define MSP_DBG_MAX            BIT2                                  // Show intermediate steps in training. Useful for training algo debug.
#define MSP_DBG_TRACE          BIT3                                  // Show all register accesses.
#define MSP_DBG_TST            BIT4                                  // Show any test messages - these are ones MRC developers can use for
                                                                     // debug but don't typically want displayed in the release BIOS.
#define MSP_DBG_DEBUG          BIT5                                  // Use for temporary debug of issues. Do not release any MRC code with this
                                                                      // print level in use.
#define MSP_DBG_REG_ACCESS     BIT6                                  // Displays all register accesses.
#define MSP_DBG_TYPICAL        MSP_DBG_MIN + MSP_DBG_MED           // Normal messages are MRC flow plus training results and eye diagrams.
#define MSP_DBG_VERBOSE        MSP_DBG_MIN + MSP_DBG_MAX + MSP_DBG_MED + MSP_DBG_TRACE // This should be the log level for most debug. Ask customers to turn
                                                                      // this level on before submitting serial debug logs to sightings.

#ifndef ASM_INC
#define TAB_STOP            4
#define LEFT_JUSTIFY        0x01
#define PREFIX_SIGN         0x02
#define PREFIX_BLANK        0x04
#define COMMON_PREFIX_ZERO  0x08
#define LONG_TYPE           0x10

#define INT_SIGNED          0x20
#define COMA_TYPE           0x40
#define LONG_LONG_TYPE      0x80

#define CHAR_CR             0x0d
#define CHAR_LF             0x0a

//
// ANSI Escape sequences for color
//
#ifndef _WINCON_
#define FOREGROUND_BLACK    30
#define FOREGROUND_RED      31
#define FOREGROUND_GREEN    32
#define FOREGROUND_YELLOW   33
#define FOREGROUND_BLUE     34
#define FOREGROUND_MAGENTA  35
#define FOREGROUND_CYAN     36
#define FOREGROUND_WHITE    37

#define BACKGROUND_BLACK    40
#define BACKGROUND_RED      41
#define BACKGROUND_GREEN    42
#define BACKGROUND_YELLOW   43
#define BACKGROUND_BLUE     44
#define BACKGROUND_MAGENTA  45
#define BACKGROUND_CYAN     46
#define BACKGROUND_WHITE    47

#define ATTRIBUTE_OFF       0
#define ATTRIBUTE_BOLD      1
#define ATTRIBUTE_UNDERSCORE  4
#define ATTRIBUTE_BLINK     5
#define ATTRIBUTE_REVERSE   7
#define ATTRIBUTE_CONCEAL   8
#endif
#ifndef INT32_MAX
#define INT32_MAX           0x7fffffffU
#endif

#ifndef va_start
typedef INT8   *va_list;
#define _INTSIZEOF(n)       ((sizeof (n) + sizeof (UINT32) - 1) &~(sizeof (UINT32) - 1))
#define va_start(ap, v)     (ap = (va_list) & v + _INTSIZEOF (v))
#define va_arg(ap, t)       (*(t *) ((ap += _INTSIZEOF (t)) - _INTSIZEOF (t)))
#define va_end(ap)          (ap = (va_list) 0)
#endif

#define isdigit(_c)         (((_c) >= '0') && ((_c) <= '9'))
#define ishexdigit(_c)      (((_c) >= 'a') && ((_c) <= 'f'))
#endif // ASM_INC

typedef enum {
  MarkerInit = 0,
  MarkerFound25h,
  MarkerFoundMinus,
} MARKER_STATES;

VOID
rcPrintf (
  IN        UINT8     MsgLevel,
  IN        INT8      *Format,
  ...
)
/*++

Routine Description:

  Prints string to serial output

Arguments:

  MsgLevel          - Defines the level at which the message should be displayed
  Format            - Format string for output

Returns:

    0 - Success
    1 - Failure

--*/
;

#endif // _printf_h
