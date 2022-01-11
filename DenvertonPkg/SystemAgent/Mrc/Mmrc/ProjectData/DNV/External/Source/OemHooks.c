/**  @file
  OemHooks.c
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
#include <Uefi.h>
#include "OemHooks.h"
#include "IchRegs.h"

#include "ConfigMem.h"
#include "Mailbox.h"

#ifdef FSP_FLAG
#include <FspmUpd.h>
#include <Library/FspCommonLib.h>
#endif

// 
// External - HW used for Write Vref Training, its center point starts at 0x40
// and increments/decrement in a non-linear 8mV per single step
//
CONST UINT8 LinearToPhysicalVrefCodesIsl90727[] = { 
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, // 00 - 15
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // 16 - 31
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // 32 - 47
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, // 48 - 63
};
// 
// For converting the Write Vref to a linear value it is needed to call this 
// array with the [physical value - 0x20] due that physical vref values range
// is from 0x20 to 0x5F and linear values are from 0x00 to 0x3F. This will help 
// to do not waste memory space filling the array with dummy data
//
CONST UINT8 PhysicalToLinearVrefCodesIsl90727[] = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 00 - 15
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 16 - 31
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, // 32 - 47
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, // 48 - 63
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, // 64 - 79
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // 80 - 95
};


//
// BBC MT40A256M16-093 deltas compared
// against CCMMT40A512M16_083
//
CONST OVERRIDE_SPD_DELTA BBCMT40A256M16_093 [] = {
  //  Byte  Override Value
  {   4,    0x84},
  {   5,    0x19},
  {   14,   0x00},
  {   18,   0x08},
  {   20,   0xFC},
  {   21,   0x1B},
  {   24,   0x78},
  {   25,   0x78},
  {   26,   0x78},
  {   28,   0x08},
  {   29,   0x80},
  {   30,   0x20},
  {   31,   0x08},
  {   32,   0x00},
  {   33,   0x05},
  {   34,   0x70},
  {   35,   0x03},
  {   38,   0x2B},
  {   39,   0x34},
  {  117,   0xED},
  {  125,   0xC2},
  {  128,   0x11},
  {  129,   0x01},
  {  130,   0x03},
};


//
// CCM MT40A512M16-083
//
CONST UINT8 CCMMT40A512M16_083[MAX_SPD_ADDR] = {
    0,0,                 // Byte 0 -> 1
    0x0C,                // Byte 2
    0x02,                // Byte 3
    0x85,                // Byte 4
    0x21,                // Byte 5
    0x00,                // Byte 6
    0x08,                // Byte 7
    0x00,                // Byte 8
    0x00,                // Byte 9
    0,                   // Byte 10 -> 10
    0x03,                // Byte 11
    0x02,                // Byte 12
    0x0B,                // Byte 13
    0x80,                // Byte 14
    0,0,                 // Byte 15 -> 16
    0x00,                // Byte 17
    0x07,                // Byte 18
    0x0C,                // Byte 19
    0xFC,                // Byte 20
    0x7F,                // Byte 21
    0x00,                // Byte 22
    0x00,                // Byte 23
    0x71,                // Byte 24
    0x71,                // Byte 25
    0x71,                // Byte 26
    0x11,                // Byte 27
    0x00,                // Byte 28
    0x72,                // Byte 29
    0xF0,                // Byte 30
    0x0A,                // Byte 31
    0,0,0,0,             // Byte 32 -> 35
    0x00,                // Byte 36
    0xF0,                // Byte 37
    0x2B,                // Byte 38
    0x32,                // Byte 39
    0x2B,                // Byte 40
    0,0,0,0,0,0,0,0,0,0, // Byte 41 -> 50
    0,0,0,0,0,0,0,0,0,0, // Byte 51 -> 60
    0,0,0,0,0,0,0,0,0,0, // Byte 61 -> 70
    0,0,0,0,0,0,0,0,0,0, // Byte 71 -> 80
    0,0,0,0,0,0,0,0,0,0, // Byte 81 -> 90
    0,0,0,0,0,0,0,0,0,0, // Byte 91 -> 100
    0,0,0,0,0,0,0,0,0,0, // Byte 101 -> 110
    0,0,0,0,0,0,         // Byte 111 -> 116
    0x00,                // Byte 117
    0,0,                 // Byte 118 -> 119
    0x00,                // Byte 120
    0x00,                // Byte 121
    0x00,                // Byte 122
    0x00,                // Byte 123
    0x00,                // Byte 124
    0xD6,                // Byte 125
    0,0,0,0,0,           // Byte 126 -> 130
    0x00,                // Byte 131
    0,0,0,0,0,0,0,0,0,0, // Byte 132 -> 141
    0,0,0,0,0,0,0,0,0,0, // Byte 142 -> 151
    0,0,0,0,0,0,0,0,0,0, // Byte 152 -> 161
    0,0,0,0,0,0,0,0,0,0, // Byte 162 -> 171
    0,0,0,0,0,0,0,0,0,0, // Byte 172 -> 181
    0,0,0,0,0,0,0,0,0,0, // Byte 182 -> 191
    0,0,0,0,0,0,0,0,0,0, // Byte 192 -> 201
    0,0,0,0,0,0,0,0,0,0, // Byte 202 -> 211
    0,0,0,0,0,0,0,0,0,0, // Byte 212 -> 221
    0,0,0,0,0,0,0,0,0,0, // Byte 222 -> 231
    0,0,0,0,0,0,0,0,0,0, // Byte 232 -> 241
    0,0,0,0,0,0,0,0,0,0, // Byte 242 -> 251
    0,0,0,0,0,0,0,0,0,0, // Byte 252 -> 261
    0,0,0,0,0,0,0,0,0,0, // Byte 262 -> 271
    0,0,0,0,0,0,0,0,0,0, // Byte 272 -> 281
    0,0,0,0,0,0,0,0,0,0, // Byte 282 -> 291
    0,0,0,0,0,0,0,0,0,0, // Byte 292 -> 301
    0,0,0,0,0,0,0,0,0,0, // Byte 302 -> 311
    0,0,0,0,0,0,0,0,     // Byte 312 -> 319
    0x80,                // Byte 320
    0x2C,                // Byte 321
    0x00,                // Byte 322
    0x00,                // Byte 323
    0,                   // Byte 324 -> 324
    0x00,                // Byte 325
    0,0,0,               // Byte 326 -> 328
    0x9A,                // Byte 329
    0,0,0,0,0,0,0,0,0,0, // Byte 330 -> 339
    0,0,0,0,0,0,0,0,0,   // Byte 340 -> 348
    0x31,                // Byte 349
    0x80,                // Byte 350
    0x2C,                // Byte 351
    0x41,                // Byte 352
    0,0,0,0,0,0,0,0,0,0, // Byte 353 -> 362
    0,0,0,0,0,0,0,0,0,0, // Byte 363 -> 372
    0,0,0,0,0,0,0,0,0,   // Byte 373 -> 381
    0x00,                // Byte 382
    0x00,                // Byte 383
  };


CONST UINT8 DASyB2_MT5KTF6472HZ_1G9J1[MAX_SPD_ADDR] = {
    0x92,   // Byte 0        DDR3 - CRC RANGE, EEPROM BYTES, BYTES USED
    0x13,   // Byte 1        DDR3 - SPD REVISON
    0x0B,   // Byte 2        DDR3 - DRAM DEVICE TYPE
    0x08,   // Byte 3        DDR3 - MODULE TYPE(FORM FACTOR) - UDIMM
    0x02,   // Byte 4        DDR3 - SDRAM DEVICE DENSITY & BANKS
    0x09,   // Byte 5        DDR3 - SDRAM DEVICE ROW & COLUMN COUNT
    0x02,   // Byte 6        DDR3 - MODULE NOMINAL VDD
    0x02,   // Byte 7        DDR3 - MODULE RANKS &  DEVICE DQ COUNT 64x16
    0x0A,   // Byte 8        DDR3 - ECC TAG & MODULE MEMORY BUS WIDTH - 32 w / ECC
    0x11,   // Byte 9        DDR3 - FINE TIMEBASE DIVIDEND / DIVISOR
    0x01,   // Byte 10       DDR3 - MEDIUM TIMEBASE DIVIDEND
    0x08,   // Byte 11       DDR3 - MEDIUM TIMEBASE DIVISOR
    0x09,   // Byte 12       DDR3 - MIN SDRAM CYCLE TIME(TCKMIN)
    0x00,   // Byte 13       DDR3 - BYTE 13 RESERVED
    0xFE,   // Byte 14       DDR3 - CAS LATENCIES SUPPORTED(CL4 = > CL11)
    0x02,   // Byte 15       DDR3 - CAS LATENCIES SUPPORTED(CL12 = > CL18)
    0x69,   // Byte 16       DDR3 - MIN CAS LATENCY TIME(TAAMIN) - 13.91ns close to 14ns
    0x78,   // Byte 17       DDR3 - MIN WRITE RECOVERY TIME(TWRMIN)
    0x69,   // Byte 18       DDR3 - MIN RAS# TO CAS# DELAY(TRCDMIN) - 13.91ns
    0x30,   // Byte 19       DDR3 - MIN ROW ACTIVE TO ROW ACTIVE DELAY(TRRDMIN
    0x69,   // Byte 20       DDR3 - MIN ROW PRECHARGE DELAY(TRPMIN) - 13.91ns
    0x11,   // Byte 21       DDR3 - UPPER NIBBLE FOR TRAS & TRC
    0x10,   // Byte 22       DDR3 - MIN ACTIVE TO PRECHARGE DELAY(TRASMIN) - 34ns
    0x79,   // Byte 23       DDR3 - MIN ACTIVE TO ACTIVE / REFRESH DELAY(TRCMI - 48ns
    0x70,   // Byte 24       DDR3 - MIN REFRESH RECOVERY DELAY(TRFCMIN) LSB
    0x03,   // Byte 25       DDR3 - MIN REFRESH RECOVERY DELAY(TRFCMIN) MSB
    0x3C,   // Byte 26       DDR3 - MIN INTERNAL WRITE TO READ CMD DELAY(TWTRM
    0x3C,   // Byte 27       DDR3 - MIN INTERNAL READ TO PRECHARGE CMD DELAY(T
    0x01,   // Byte 28       DDR3 - MIN FOUR ACTIVE WINDOW DELAY(TFAWMIN) MSB
    0x18,   // Byte 29       DDR3 - MIN FOUR ACTIVE WINDOW DELAY(TFAWMIN) LSB - 35.31ns
    0x83,   // Byte 30       DDR3 - SDRAM DEVICE OUTPUT DRIVERS SUPPORTED
    0x05,   // Byte 31       DDR3 - SDRAM DEVICE THERMAL & REFRESH OPTIONS - 2x refresh
    0x00,   // Byte 32       DDR3 - MODULE THERMAL SENSOR
    0x00,   // Byte 33       DDR3 - SDRAM DEVICE TYPE
    0xCA,   // Byte 34       DDR3 - FINE OFFSET FOR TCKMIN
    0x00,   // Byte 35       DDR3 - FINE OFFSET FOR TAAMIN
    0x00,   // Byte 36       DDR3 - FINE OFFSET FOR TRCDMIN
    0x00,   // Byte 37       DDR3 - FINE OFFSET FOR TRPMIN
    0x00,   // Byte 38       DDR3 - FINE OFFSET FOR TRCMIN
    0x00,   // Byte 39       DDR3 - BYTE 39 RESERVED
    0x00,   // Byte 40       DDR3 - BYTE 40 RESERVED
    0x88,   // Byte 41       DDR3 - PTRR TMAW & MAC
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Bytes 42-59   DDR3 - RESERVED BYTES 42 - 59
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    0x0F,   // Byte 60       DDR3 - RC REV & NOM MODULE HEIGHT
    0x01,   // Byte 61       DDR3 - MODULE THICKNESS(MAX)
    0x62,   // Byte 62       DDR3 - REFERENCE RAW CARD ID
    0x00,   // Byte 63       DDR3 - ADDRESS MAPPING / MODULE ATTRIBUTES
    0x00,   // Byte 64       DDR3 - HEATSPREADER SOLUTION
    0x00,   // Byte 65       DDR3 - REGISTER VENDOR ID(LSB)
    0x00,   // Byte 66       DDR3 - REGISTER VENDOR ID(MSB)
    0x00,   // Byte 67       DDR3 - REGISTER REVISON NUMBER
    0x00,   // Byte 68       DDR3 - REGISTER TYPE
    0x00,   // Byte 69       DDR3 - REG CTRL WORDS 1 AND ZERO
    0x00,   // Byte 70       DDR3 - REG CTRL WORDS 3 AND 2
    0x00,   // Byte 71       DDR3 - REG CTRL WORDS 5 AND 4
    0x00,   // Byte 72       DDR3 - REG CTRL WORDS 7 AND 6
    0x00,   // Byte 73       DDR3 - REG CTRL WORDS 9 AND 8
    0x00,   // Byte 74       DDR3 - REG CTRL WORDS 11 AND 10
    0x00,   // Byte 75       DDR3 - REG CTRL WORDS 13 AND 12
    0x00,   // Byte 76       DDR3 - REG CTRL WORDS 15 AND 14
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Bytes 77-116   DDR3 - RESERVED BYTES 77 - 116
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80,   // Byte 117      DDR3 - MODULE MFR ID(LSB)
    0x2C,   // Byte 118      DDR3 - MODULE MFR ID(MSB)
    0x00,   // Byte 119      DDR3 - MODULE MFR LOCATION ID
    0x00,   // Byte 120      DDR3 - MODULE MFR YEAR
    0x00,   // Byte 121      DDR3 - MODULE MFR WEEK
    0x00, 0x00, 0x00, 0x00, // Bytes 122-125  DDR3 - MODULE SERIAL NUMBER
    0xE5, 0x28, // Bytes 126-127  DDR3 - CRC
    '5', 'K', 'T', 'F', '6', '4', '7', '2', 'H', 'Z', ' ', '-', ' ', '1', 'G', '9', 'J', '1', // Bytes 128-145  DDR3 - MODULE PART NUMBER
    0x4A,   // Byte 146      DDR3 - MODULE DIE REV
    0x31,   // Byte 147      DDR3 - MODULE PCB REV
    0x80,   // Byte 148      DDR3 - DRAM DEVICE MFR ID(LSB)
    0x2C,   // Byte 149      DDR3 - DRAM DEVICE MFR(MSB)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Bytes 150-175  DDR3 - MFR RESERVED BYTES 150 - 175
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Bytes 176-255  DDR3 - CUSTOMER RESERVED BYTES 176 - 255
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

CONST UINT8 ALVMT5KTF12872HZ_1G9P1[MAX_SPD_ADDR] = {
	0x92,	// Byte 0        DDR3 - CRC RANGE, EEPROM BYTES, BYTES USED
	0x13,	// Byte 1        DDR3 - SPD REVISON
	0x0B,	// Byte 2        DDR3 - DRAM DEVICE TYPE
	0x02,	// Byte 3        DDR3 - MODULE TYPE(FORM FACTOR) - UDIMM
	0x03,	// Byte 4        DDR3 - SDRAM DEVICE DENSITY & BANKS
	0x11,	// Byte 5        DDR3 - SDRAM DEVICE ROW & COLUMN COUNT
	0x02,	// Byte 6        DDR3 - MODULE NOMINAL VDD
	0x02,	// Byte 7        DDR3 - MODULE RANKS &  DEVICE DQ COUNT
	0x0A,	// Byte 8        DDR3 - ECC TAG & MODULE MEMORY BUS WIDTH
	0x11,	// Byte 9        DDR3 - FINE TIMEBASE DIVIDEND / DIVISOR
	0x01,	// Byte 10       DDR3 - MEDIUM TIMEBASE DIVIDEND
	0x08,	// Byte 11       DDR3 - MEDIUM TIMEBASE DIVISOR
	0x0A,	// Byte 12       DDR3 - MIN SDRAM CYCLE TIME(TCKMIN)
	0x00,	// Byte 13       DDR3 - BYTE 13 RESERVED
	0xFE,	// Byte 14       DDR3 - CAS LATENCIES SUPPORTED(CL4 = > CL11)
	0x00,	// Byte 15       DDR3 - CAS LATENCIES SUPPORTED(CL12 = > CL18)
	0x6E,	// Byte 16       DDR3 - MIN CAS LATENCY TIME(TAAMIN)
	0x78,	// Byte 17       DDR3 - MIN WRITE RECOVERY TIME(TWRMIN)
	0x6E,	// Byte 18       DDR3 - MIN RAS# TO CAS# DELAY(TRCDMIN)
	0x30,	// Byte 19       DDR3 - MIN ROW ACTIVE TO ROW ACTIVE DELAY(TRRDMIN
	0x6E,	// Byte 20       DDR3 - MIN ROW PRECHARGE DELAY(TRPMIN)
	0x11,	// Byte 21       DDR3 - UPPER NIBBLE FOR TRAS & TRC
	0x18,	// Byte 22       DDR3 - MIN ACTIVE TO PRECHARGE DELAY(TRASMIN)
	0x86,	// Byte 23       DDR3 - MIN ACTIVE TO ACTIVE/REFRESH DELAY(TRCMIN)
	0x00,	// Byte 24       DDR3 - MIN REFRESH RECOVERY DELAY(TRFCMIN) LSB
	0x05,	// Byte 25       DDR3 - MIN REFRESH RECOVERY DELAY(TRFCMIN) MSB
	0x3C,	// Byte 26       DDR3 - MIN INTERNAL WRITE TO READ CMD DELAY(TWTRM
	0x3C,	// Byte	27       DDR3 - MIN INTERNAL READ TO PRECHARGE CMD DELAY(T
	0x01,	// Byte 28       DDR3 - MIN FOUR ACTIVE WINDOW DELAY(TFAWMIN) MSB
	0x40,	// Byte 29       DDR3 - MIN FOUR ACTIVE WINDOW DELAY(TFAWMIN) LSB
	0x83,	// Byte 30       DDR3 - SDRAM DEVICE OUTPUT DRIVERS SUPPORTED
	0x05,	// Byte 31       DDR3 - SDRAM DEVICE THERMAL & REFRESH OPTIONS
	0x00,	// Byte 32       DDR3 - MODULE THERMAL SENSOR
	0x00,	// Byte 33       DDR3 - SDRAM DEVICE TYPE
	0x00,	// Byte 34       DDR3 - FINE OFFSET FOR TCKMIN
	0x00,	// Byte 35       DDR3 - FINE OFFSET FOR TAAMIN
	0x00,	// Byte 36       DDR3 - FINE OFFSET FOR TRCDMIN
	0x00,	// Byte 37       DDR3 - FINE OFFSET FOR TRPMIN
	0x00,	// Byte 38       DDR3 - FINE OFFSET FOR TRCMIN
	0x00,	// Byte 39       DDR3 - BYTE 39 RESERVED
	0x00,	// Byte 40       DDR3 - BYTE 40 RESERVED
	0x88,	// Byte 41       DDR3 - PTRR TMAW & MAC
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Bytes 42-59   DDR3 -RESERVED BYTES 42 - 59
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	0x0F,	// Byte 60       DDR3 - RC REV & NOM MODULE HEIGHT
	0x01,	// Byte 61       DDR3 - MODULE THICKNESS(MAX)
	0x62,	// Byte 62       DDR3 - REFERENCE RAW CARD ID
	0x00,	// Byte 63       DDR3 - ADDRESS MAPPING / MODULE ATTRIBUTES
	0x00,	// Byte 64       DDR3 - HEATSPREADER SOLUTION
	0x00,	// Byte 65       DDR3 - REGISTER VENDOR ID(LSB)
	0x00,	// Byte 66       DDR3 - REGISTER VENDOR ID(MSB)
	0x00,	// Byte 67       DDR3 - REGISTER REVISON NUMBER
	0x00,	// Byte 68       DDR3 - REGISTER TYPE
	0x00,	// Byte 69       DDR3 - REG CTRL WORDS 1 AND ZERO
	0x00,	// Byte 70       DDR3 - REG CTRL WORDS 3 AND 2
	0x00,	// Byte 71       DDR3 - REG CTRL WORDS 5 AND 4
	0x00,	// Byte 72       DDR3 - REG CTRL WORDS 7 AND 6
	0x00,	// Byte 73       DDR3 - REG CTRL WORDS 9 AND 8
	0x00,	// Byte 74       DDR3 - REG CTRL WORDS 11 AND 10
	0x00,	// Byte 75       DDR3 - REG CTRL WORDS 13 AND 12
	0x00,	// Byte 76       DDR3 - REG CTRL WORDS 15 AND 14
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Bytes 77-116   DDR3 - RESERVED BYTES 77 - 116
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80,	// Byte 117      DDR3 - MODULE MFR ID(LSB)
	0x2C,	// Byte 118      DDR3 - MODULE MFR ID(MSB)
	0x00,	// Byte 119      DDR3 - MODULE MFR LOCATION ID
	0x00,	// Byte 120      DDR3 - MODULE MFR YEAR
	0x00,	// Byte 121      DDR3 - MODULE MFR WEEK
	0x00, 0x00, 0x00, 0x00, // Bytes 122-125  DDR3 - MODULE SERIAL NUMBER
	0x40, 0x8D, // Bytes 126-127  DDR3 - CRC
	'5', 'K', 'T', 'F', '1', '2', '8', '7', '2', 'H', 'Z', '-', '1', 'G', '9', 'P', '1', ' ', // Bytes 128-145  DDR3 - MODULE PART NUMBER
	0x50,	// Byte 146      DDR3 - MODULE DIE REV
	0x31,	// Byte 147      DDR3 - MODULE PCB REV
	0x80,	// Byte 148      DDR3 - DRAM DEVICE MFR ID(LSB)
	0x2C,	// Byte 149      DDR3 - DRAM DEVICE MFR(MSB)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Bytes 150-175  DDR3 - MFR RESERVED BYTES 150 - 175
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Bytes 176-255  DDR3 - CUSTOMER RESERVED BYTES 176 - 255
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


BOARD_PRODUCT_NAME BoardProductNames[NUM_PLAT+PLATFORM_VARIATIONS] = {
  {"OSTRICH BAY-S",       P_INT},
  {"Aspen Cove",          P_CRB},
  {"HARCUVAR",            P_CRB},
  {"MINNEHAHA",           P_MEM_DOWN},
  {"PINELAKE",            P_CRB},
  {"CAR CREEK MODULE",    P_CAR_LAKE},
  {"CAR CREEK MODULE 2",  P_CAR_LAKE_2},
  {"BIG BUG CREEK",       P_OEM_BRD0},
  {"BIG BUG CREEK 2",     P_OEM_BRD1},
  {"Autoliv",     		  P_OEM_BRD2},
  {"PINELAKE-B",          P_CRB},
};

/**
  This function detects the platform under test.

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Channel under test.
  @retval          MMRC_SUCCESS    The platform was detected.
**/
MMRC_STATUS
OemGetFruInformation (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Channel
  )
{
#ifdef FSP_FLAG
  return  MMRC_FAILURE;
#else
  BOOLEAN FoundPlatform;
  UINT8   ReadByte;
  UINT8   Length;
  UINT8   BoardName[MAX_BOARD_PRODUCT_NAME];
  UINT16  Offset;
  UINT8   i;

  ReadByte  = 0;
  Length    = 1;

  //
  // Read the Board Area Starting
  //
  Offset    = 3;
  MrcSmbusExec (
    MrcData->SmbusBar,
    FRU_DEVICE_ADDRESS,
    SmbusReadByte,
    Offset,
    &Length,
    &ReadByte
  );
  if (ReadByte == 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Board Area Not Present\n"));
    return  MMRC_FAILURE;
  }
  //
  // Offset is provided in multiples of 8 bytes
  //
  ReadByte *= 8;
  //
  // Point to Board Manufacturer Length
  // "Board Manufacturer Length" + 6
  //
  Offset    = ReadByte + 6;
  MrcSmbusExec (
    MrcData->SmbusBar,
    FRU_DEVICE_ADDRESS,
    SmbusReadByte,
    Offset,
    &Length,
    &ReadByte
  );
  //
  // Type/Length Byte Format
  // 7:6 -> Type Code
  // 5:0 -> Number Of Bytes
  //
  ReadByte = ReadByte & 0x3f;
  //
  // Point to Board Product Name Length
  // 1 (Skip "Board Manufacturer Length") + "Board Product Name Length"
  //
  Offset    += 1 + ReadByte;
  MrcSmbusExec (
    MrcData->SmbusBar,
    FRU_DEVICE_ADDRESS,
    SmbusReadByte,
    Offset,
    &Length,
    &ReadByte
  );
  if (ReadByte == 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Board Product Name Length Not Found\n"));
    return  MMRC_FAILURE;
  }
  //
  // Type/Length Byte Format
  // 7:6 -> Type Code
  // 5:0 -> Number Of Bytes
  //
  ReadByte = ReadByte & 0x3f;
  //
  // Point to Board Product Name Bytes
  //
  Offset += 1;
  //
  // Clear the Buffer
  //
  MmrcMemset (BoardName, 0, sizeof(UINT8) * MAX_BOARD_PRODUCT_NAME);
  if (ReadByte > MAX_BOARD_PRODUCT_NAME) {
    MspDebugPrint ((MSP_DBG_MIN, "Board Product Name Length Invalid\n"));
    return  MMRC_FAILURE;
  }
  for (i = 0; i < ReadByte; i++) {
    MrcSmbusExec (
      MrcData->SmbusBar,
      FRU_DEVICE_ADDRESS,
      SmbusReadByte,
      (Offset + i),
      &Length,
      &BoardName[i]
    );
  }

  MspDebugPrint ((MSP_DBG_MIN, "Board Name: %s\n", BoardName));

  for (i = 0; i < (NUM_PLAT + PLATFORM_VARIATIONS); i++) {
    FoundPlatform = MmrcStringCompare (BoardName, BoardProductNames[i].BoardProductName, ReadByte);
    if (FoundPlatform) {
      MspDebugPrint ((MSP_DBG_MIN, "Board Name Found[%d]: %s\n", i, BoardProductNames[i].BoardProductName));
      break;
    }
  }

  if (i < (NUM_PLAT + PLATFORM_VARIATIONS)) {
    MspDebugPrint ((MSP_DBG_MIN, "P_ID: 0x%02x\n", BoardProductNames[i].PlatformId));
    MrcData->MspData.CurrentPlatform[Channel] = BoardProductNames[i].PlatformId;
  } else {
    return MMRC_FAILURE;
  }

  return MMRC_SUCCESS;
#endif
}

/**
  This function detects the platform under test.

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    The platform was detected.
**/
MMRC_STATUS
OemGetPlatformType (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8       Channel;
  UINT8       Simics;
  MMRC_STATUS Status;
  Simics = 0;
  Status = MMRC_SUCCESS;
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->MrcParameters.ChEnabled[Channel]) {
#if defined SIM
      MrcData->MspData.CurrentPlatform[Channel] = P_SIM;
      MspDebugPrint ((MSP_DBG_MIN, "Simulation Platform\n"));
#elif defined JTAG
      MrcData->MspData.CurrentPlatform[Channel] = P_JTAG;
      MspDebugPrint ((MSP_DBG_MIN, "Jtag Platform\n"));
#elif defined DELPHI_SUPPORT	  
	  MrcData->MspData.CurrentPlatform[Channel] = P_CAR_LAKE_2;
      MspDebugPrint ((MSP_DBG_MIN, "Delphi Platform\n"));	
#else
      Simics = (UINT8) PCI_CFG_32B_READ (MrcData->EcBase, MC_BUS, 0x0, 0x0, 0xDC);
      if (Simics >= 1 && Simics <= 2) {
        MrcData->SkipTrainingAlgos[LRxVoc]                = TRUE;
        MrcData->SkipTrainingAlgos[LReceiveEnable]        = TRUE;
        MrcData->SkipTrainingAlgos[LEarlyMpr]             = TRUE;
        MrcData->SkipTrainingAlgos[LFineWriteLeveling]    = TRUE;
        MrcData->SkipTrainingAlgos[LCoarseWriteLeveling]  = TRUE;
        MrcData->SkipTrainingAlgos[LReadTraining]         = TRUE;
        MrcData->SkipTrainingAlgos[LWriteTraining]        = TRUE;
        MrcData->SkipTrainingAlgos[LCommandTraining]      = TRUE;
        MrcData->SkipTrainingAlgos[LMemoryTest]           = TRUE;
      }
      if (Simics == 1) {
        OemGetFruInformation (MrcData, Channel);
        MrcData->MspData.CurrentPlatform[Channel] = P_SIMICS;
        MspDebugPrint ((MSP_DBG_MIN, "Simics Platform\n"));
      } else if (Simics == 2) {
        MspDebugPrint ((MSP_DBG_MIN, "Emulation Platform"));
        Status = MmrcExecuteTask (MrcData, IsDunitEmuModel, NO_PRINT, Channel);
        if (MrcData->MspData.DynamicVars[Channel][SKPD5] == 0xDEADBEEF) {
          MspDebugPrint ((MSP_DBG_MIN, "with Dunit\n"));
          MrcData->MspData.CurrentPlatform[Channel] = P_EMU_DUNIT;
        } else {
          MspDebugPrint ((MSP_DBG_MIN, "without Dunit\n"));
          MrcData->MspData.CurrentPlatform[Channel] = P_EMU;
        }
      } else {
        Status = OemGetFruInformation (MrcData, Channel);
        if (Status == MMRC_FAILURE) {
          //
          // Set the HCB board by default
          //
          MrcData->MspData.CurrentPlatform[Channel] = P_CRB;
        }
      }
#endif
    }
  }
  return Status;
}

#ifdef FSP_FLAG
/**
  This function sets the OEM specific information

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
VOID
OemSetMrcDataFspOverrides (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8   MemoryDownEnable;
  UINT8   DmapX16Value;
  
  MemoryDownEnable = ((FSPM_UPD *)GetFspMemoryInitUpdDataPointer ())->FspmConfig.PcdMemoryDown;
  if (MemoryDownEnable) {
    MrcData->MspData.CurrentPlatform[0] = P_MEM_DOWN;
    MrcData->MspData.CurrentPlatform[1] = P_MEM_DOWN;
    DmapX16Value = ((FSPM_UPD *)GetFspMemoryInitUpdDataPointer ())->FspmConfig.PcdDmapX16;
    MspDebugPrint ((MSP_DBG_MIN, "FSP DmapX16Value = 0x%x\n", DmapX16Value));
    if (DmapX16Value) {
      MrcData->MspData.DynamicVars[0][DMAP_X16]     = DmapX16Value;
      MrcData->MspData.DynamicVars[1][DMAP_X16]     = DmapX16Value;
    }
  }
}
#endif

/**
  This function sets the OEM specific information

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
OemSetMrcData (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  
  MRC_OEM_FRAME *OemMrcData;
  UINT8         Channel;
  UINT8         Dimm;
  UINT16        SpdOffset;
  UINT8         SizeOfOverride;
  UINT8         Index;
  UINT16        ByteToOverride;


  OemMrcData  = &MrcData->OemMrcData;

#if SMM_SUPPORT
  OemMrcData->TsegSize = 0x2;
#endif
  OemMrcData->MmioSize = 0x800;

  switch (MrcData->MspData.CurrentPlatform[0]) {
  case P_OEM_BRD0:
  case P_OEM_BRD1:
  case P_OEM_BRD2:	
  case P_CAR_LAKE:
  case P_CAR_LAKE_2:
  case P_MEM_DOWN:
    //
    // Still dummy SPD addresses are required
    //
    OemMrcData->SpdAddressTable[0][0]               = 0xA0;
    OemMrcData->SpdAddressTable[0][1]               = 0xA2;
    OemMrcData->SpdAddressTable[1][0]               = 0xA4;
    OemMrcData->SpdAddressTable[1][1]               = 0xA6;
    //
    // MemoryDown Support
    //
    if (MrcData->MspData.CurrentPlatform[0] == P_CAR_LAKE || MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD0 || MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1 || MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD2) {
      OemMrcData->EnableMemoryDown[0][0] = TRUE;
      OemMrcData->EnableMemoryDown[0][1] = FALSE;
      OemMrcData->EnableMemoryDown[1][0] = FALSE;
      OemMrcData->EnableMemoryDown[1][1] = FALSE;
    } else if (MrcData->MspData.CurrentPlatform[0] == P_CAR_LAKE_2) {
      OemMrcData->EnableMemoryDown[0][0] = TRUE;
      OemMrcData->EnableMemoryDown[0][1] = FALSE;
      OemMrcData->EnableMemoryDown[1][0] = TRUE;
      OemMrcData->EnableMemoryDown[1][1] = FALSE;
	  #if defined DELPHI_SUPPORT
	  // Delphi board supports only single channel
	  OemMrcData->EnableMemoryDown[1][0] = FALSE;
	  #endif 
    } else {
      OemMrcData->EnableMemoryDown[0][0] = TRUE;
      OemMrcData->EnableMemoryDown[0][1] = FALSE;
      OemMrcData->EnableMemoryDown[1][0] = TRUE;
      OemMrcData->EnableMemoryDown[1][1] = FALSE;
    }
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
        if (OemMrcData->EnableMemoryDown[Channel][Dimm] == TRUE) {
          for (SpdOffset = 0; SpdOffset < MAX_SPD_ADDR; SpdOffset++) {
            if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1) {
                MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SpdOffset] = DASyB2_MT5KTF6472HZ_1G9J1[SpdOffset];
            } else if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD2) {
              MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SpdOffset] = ALVMT5KTF12872HZ_1G9P1[SpdOffset];
            } else {
              MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SpdOffset] = CCMMT40A512M16_083[SpdOffset];
            }
          }
        }
      }
    }


    if (MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD0) {
       // Do an override for specific bytes in the SPD
       SizeOfOverride = sizeof(BBCMT40A256M16_093)/sizeof(OVERRIDE_SPD_DELTA);
       for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
           for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
              if (OemMrcData->EnableMemoryDown[Channel][Dimm] == TRUE) {
                for (Index = 0; Index < SizeOfOverride; Index++) {
                  ByteToOverride = BBCMT40A256M16_093[Index].ByteOffset;
                  MspDebugPrint ((
                    MSP_DBG_MAX,
                    "C%dD%d Override Byte %02d 0x%02x --> 0x%02x\n",
                    Channel,
                    Dimm,
                    ByteToOverride,
                    MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[ByteToOverride],
                    BBCMT40A256M16_093[Index].OverrideValue
                  ));
                  MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[ByteToOverride] = BBCMT40A256M16_093[Index].OverrideValue;
                }
              }
           }
       }
    }


    if (MrcData->MspData.CurrentPlatform[0] == P_CAR_LAKE || MrcData->MspData.CurrentPlatform[0] == P_CAR_LAKE_2 || MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD0) {
      //
      // We override the P_CAR_LAKE platform variation to P_MEM_DOWN
      // on both channels, because DDRIO settings from this point on
      // will look exactly the same as P_MEM_DOWN
      //
      MrcData->MspData.CurrentPlatform[0] = P_MEM_DOWN;
      MrcData->MspData.CurrentPlatform[1] = P_MEM_DOWN;
      //
      // As CAR_LAKE is x16 device width we need to change the DMAP during
      // training algorithms and also the final DMAP is fixed for Single Rank.
      //
//      MrcData->MspData.DynamicVars[0][WA_CAR_LAKE]  = 1;
//      MrcData->MspData.DynamicVars[1][WA_CAR_LAKE]  = 1;
      MrcData->MspData.DynamicVars[0][DMAP_X16]     = 1;
      MrcData->MspData.DynamicVars[1][DMAP_X16]     = 1;
//      MspDebugPrint ((MSP_DBG_MIN, "WA_CAR_LAKE\n"));
      MspDebugPrint ((MSP_DBG_MIN, "DMAP_X16\n"));
    } else if(MrcData->MspData.CurrentPlatform[0] == P_OEM_BRD1){
      MrcData->MspData.CurrentPlatform[0] = P_MEM_DOWN;
      MrcData->MspData.CurrentPlatform[1] = P_MEM_DOWN;
    }

    break;
  default:
    OemMrcData->SpdAddressTable[0][0]               = 0xA0;
    OemMrcData->SpdAddressTable[0][1]               = 0xA2;
    OemMrcData->SpdAddressTable[1][0]               = 0xA4;
    OemMrcData->SpdAddressTable[1][1]               = 0xA6;
    break;
  } // CurrentPlatform switch

  switch (MrcData->MspData.CurrentPlatform[0]) {
  case P_INT:
    MrcData->OemMrcData.VrefSmbAddress[0]           = 0x5C;
    MrcData->OemMrcData.VrefSmbAddress[1]           = 0x7C;
    MrcData->OemMrcData.CmdVrefSmbAddress[0]        = 0xFF;
    MrcData->OemMrcData.CmdVrefSmbAddress[1]        = 0xFF;
    //
    // Tx Vref Table
    //
    OemMrcData->TxVrefTable.LinearToPhysicalTable   = (UINT8 *) LinearToPhysicalVrefCodesIsl90727;
    OemMrcData->TxVrefTable.SizeOfLtoPTable         = sizeof (LinearToPhysicalVrefCodesIsl90727) / sizeof (UINT8);
    OemMrcData->TxVrefTable.PhysicalToLinearTable   = (UINT8 *) PhysicalToLinearVrefCodesIsl90727;
    OemMrcData->TxVrefTable.SizeOfPtoLTable         = sizeof (PhysicalToLinearVrefCodesIsl90727) / sizeof (UINT8);
    //
    // Cmd Vref Table
    //
    OemMrcData->CmdVrefTable.LinearToPhysicalTable  = NULL;
    OemMrcData->CmdVrefTable.SizeOfLtoPTable        = 0;
    OemMrcData->CmdVrefTable.PhysicalToLinearTable  = NULL;
    OemMrcData->CmdVrefTable.SizeOfPtoLTable        = 0;
    break;
  default:
    MrcData->OemMrcData.VrefSmbAddress[0]           = 0xFF;
    MrcData->OemMrcData.VrefSmbAddress[1]           = 0xFF;
    MrcData->OemMrcData.CmdVrefSmbAddress[0]        = 0xFF;
    MrcData->OemMrcData.CmdVrefSmbAddress[1]        = 0xFF;

    //
    // Tx Vref Table
    //
    OemMrcData->TxVrefTable.LinearToPhysicalTable   = NULL;
    OemMrcData->TxVrefTable.SizeOfLtoPTable         = 0;
    OemMrcData->TxVrefTable.PhysicalToLinearTable   = NULL;
    OemMrcData->TxVrefTable.SizeOfPtoLTable         = 0;
    //
    // Cmd Vref Table
    //
    OemMrcData->CmdVrefTable.LinearToPhysicalTable  = NULL;
    OemMrcData->CmdVrefTable.SizeOfLtoPTable        = 0;
    OemMrcData->CmdVrefTable.PhysicalToLinearTable  = NULL;
    OemMrcData->CmdVrefTable.SizeOfPtoLTable        = 0;

    break;
  } // CurrentPlatform switch


  MspDebugPrint ((MSP_DBG_MIN, "InvertChannelSpdAddresses = %d\n", MrcData->InvertChannelSpdAddresses));
  MspDebugPrint ((MSP_DBG_MIN, "SizeOfLtoPTable = %d\n", OemMrcData->TxVrefTable.SizeOfLtoPTable));
  MspDebugPrint ((MSP_DBG_MIN, "SizeOfPtoLTable = %d\n", OemMrcData->TxVrefTable.SizeOfPtoLTable));
  MspDebugPrint ((MSP_DBG_MIN, "CmdSizeOfLtoPTable = %d\n", OemMrcData->CmdVrefTable.SizeOfLtoPTable));
  MspDebugPrint ((MSP_DBG_MIN, "CmdSizeOfPtoLTable = %d\n", OemMrcData->CmdVrefTable.SizeOfPtoLTable));

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    MspDebugPrint ((
      MSP_DBG_MIN,
      "OemMrcData.VrefSmbAddress[%d] = 0x%02x OemMrcData.CmdVrefSmbAddress[%d] = 0x%02x\n",
      Channel,
      MrcData->OemMrcData.VrefSmbAddress[Channel],
      Channel,
      MrcData->OemMrcData.CmdVrefSmbAddress[Channel]
    ));
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      MspDebugPrint ((MSP_DBG_MIN, "C%dD%d\n", Channel, Dimm));
      MspDebugPrint ((MSP_DBG_MIN, "  Spd Address = 0x%02x\n", OemMrcData->SpdAddressTable[Channel][Dimm]));
    }
  }
#if SIM
  OemMrcData->SpdAddressTable[0][0] = 0;
  OemMrcData->SpdAddressTable[0][1] = 1;
  OemMrcData->SpdAddressTable[1][0] = 2;
  OemMrcData->SpdAddressTable[1][1] = 3;
#endif

#ifdef FSP_FLAG
  OemSetMrcDataFspOverrides(MrcData);
#endif

  return MMRC_SUCCESS;
}

/**
  This function sets the OEM specific information

  @param[in]  IndexError         Refer to MrcDeadLoop in MmrcLibraries.c 
**/
VOID
OemMemoryErrorCode (
  IN        UINT8                 IndexError
  )
{

}

/**
  This function configures the OEM board mux(es) appropriately.

  @param[in]  Channel         Channel being examined. 
  @param[in]  Dimm            Dimm being examined. 
  @retval     MMRC_SUCCESS    OEM muxes are set correctly. 
**/
MMRC_STATUS
OemI2cMux (
  IN        UINT8                 Channel,
  IN        UINT8                 Dimm
  )
{
  return MMRC_SUCCESS;
}

/**
  This function releases the board's activation of self refresh to the DIMMs

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex
  @param[in]       StringIndex
  @param[in]       Channel
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
OemClearSelfRefreshClamp (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
{
  UINT32 VOLATILE * CONST GpioCtrl = (UINT32 VOLATILE *)MrcData->CkeLatchGpioCtrl;
  UINT32 OrigGpioCtrl = 0;
  ASSERT(NULL != GpioCtrl);
  if (GpioCtrl == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
    return MMRC_FAILURE;
  }

  OrigGpioCtrl = *GpioCtrl;
  //
  // force a low to high to low pulse then return GPIO to starting state
  //
  *GpioCtrl = *GpioCtrl & ~(UINT32)BIT0;
  *GpioCtrl = *GpioCtrl | (UINT32)BIT0;
  *GpioCtrl = *GpioCtrl & ~(UINT32)BIT0;
  *GpioCtrl = OrigGpioCtrl;

  return MMRC_SUCCESS;
}
