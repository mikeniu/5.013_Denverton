/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  MeFiaMuxLib.h

Abstract:

  Definitions for FIA MUX configuration interface to ME.

--*/
#ifndef _ME_FIA_MUX_LIB_H_
#define _ME_FIA_MUX_LIB_H_

#pragma pack(1)
/*
 * The structure of FIA MUX configuration that can be set in ME.
 */
#define ME_FIA_MUX_LANE_NUM_MAX         20
#define ME_FIA_MUX_LANE_MUX_SEL_WIDTH   2
#define ME_FIA_MUX_LANE_MUX_SEL_MASK    0x3
#define ME_FIA_MUX_LANE_XHCI_ONLY       0xFF00000000

#define FIA_LANE00   0
#define FIA_LANE01   1
#define FIA_LANE02   2
#define FIA_LANE03   3
#define FIA_LANE04   4
#define FIA_LANE05   5
#define FIA_LANE06   6
#define FIA_LANE07   7
#define FIA_LANE08   8
#define FIA_LANE09   9
#define FIA_LANE10  10
#define FIA_LANE11  11
#define FIA_LANE12  12
#define FIA_LANE13  13
#define FIA_LANE14  14
#define FIA_LANE15  15
#define FIA_LANE16  16
#define FIA_LANE17  17
#define FIA_LANE18  18
#define FIA_LANE19  19

#define ME_FIA_MUX_LANE_SATA0_BEGING FIA_LANE04
#define ME_FIA_MUX_LANE_SATA1_BEGING FIA_LANE12

#define FIA_LANE_CONFIG(Config, Lane) ( (UINT64) ( (UINT64)(Config) << ( (UINT64)(Lane) * (ME_FIA_MUX_LANE_MUX_SEL_WIDTH))))

typedef union _ME_FIA_MUX_CONFIG {
  UINT64 MeFiaMuxLaneConfig;
  struct {
    UINT64 Lane00MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE
    UINT64 Lane01MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE
    UINT64 Lane02MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE
    UINT64 Lane03MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE
    UINT64 Lane04MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane05MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane06MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane07MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane08MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane09MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane10MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane11MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane12MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane13MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane14MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane15MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or PCIE or SATA
    UINT64 Lane16MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or XHCI or SATA
    UINT64 Lane17MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or XHCI or SATA
    UINT64 Lane18MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or XHCI or SATA
    UINT64 Lane19MuxSel : 2;  // ME_FIA_MUX_LANE_DISABLED or XHCI or SATA
    UINT64 reserved     : 24;
  } MeFiaMuxLaneMuxSel;
} ME_FIA_MUX_CONFIG;

typedef enum {
    ME_FIA_MUX_LANE_DISCONNECTED,
    ME_FIA_MUX_LANE_PCIE,
    ME_FIA_MUX_LANE_SATA,
    ME_FIA_MUX_LANE_XHCI
} ME_FIA_MUX_LANE_CONFIG;

#define ME_FIA_SATA_LANE_SEL_WIDTH   2
#define ME_FIA_SATA_LANE_XHCI_ONLY   0x55000000

#define FIA_SATA_LANE04   0
#define FIA_SATA_LANE05   1
#define FIA_SATA_LANE06   2
#define FIA_SATA_LANE07   3
#define FIA_SATA_LANE08   4
#define FIA_SATA_LANE09   5
#define FIA_SATA_LANE10   6
#define FIA_SATA_LANE11   7
#define FIA_SATA_LANE12   8
#define FIA_SATA_LANE13   9
#define FIA_SATA_LANE14  10
#define FIA_SATA_LANE15  11
#define FIA_SATA_LANE16  12
#define FIA_SATA_LANE17  13
#define FIA_SATA_LANE18  14
#define FIA_SATA_LANE19  15

#define FIA_SATA_LANE_CONFIG(Config, Lane) ( (UINT32) ( (UINT32)(Config) << ( (UINT32)(Lane) * (ME_FIA_SATA_LANE_SEL_WIDTH))))

typedef union _ME_FIA_SATA_CONFIG {
  UINT64 MeFiaSataLaneConfig;
  struct {
    UINT64 Lane04SataSel : 2;
    UINT64 Lane05SataSel : 2;
    UINT64 Lane06SataSel : 2;
    UINT64 Lane07SataSel : 2;
    UINT64 Lane08SataSel : 2;
    UINT64 Lane09SataSel : 2;
    UINT64 Lane10SataSel : 2;
    UINT64 Lane11SataSel : 2;
    UINT64 Lane12SataSel : 2;
    UINT64 Lane13SataSel : 2;
    UINT64 Lane14SataSel : 2;
    UINT64 Lane15SataSel : 2;
    UINT64 Lane16SataSel : 2;
    UINT64 Lane17SataSel : 2;
    UINT64 Lane18SataSel : 2;
    UINT64 Lane19SataSel : 2;
    UINT64 reserved      : 32;
  } MeFiaSataLaneSataSel;
} ME_FIA_SATA_CONFIG;

typedef enum
{
  ME_FIA_SATA_CONTROLLER_LANE_ASSIGNED = 0,
  ME_FIA_SATA_CONTROLLER_LANE_NOT_ASSIGNED = 1,
  ME_FIA_SATA_CONTROLLER_LANE_SS_AND_GPIO_ASSIGNED = 3
} ME_FIA_SATA_LANE_CONFIG;

#define ME_FIA_PCIE_ROOT_PORT_LINK_WIDTH_SEL_WIDTH   4
#define ME_FIA_PCIE_ROOT_PORTS_STATE_WIDTH           8
#define ME_FIA_PCIE_ROOT_CONFIG_XHCI_ONLY            0x0

#define FIA_PCIE_ROOT_PORT_0   0
#define FIA_PCIE_ROOT_PORT_1   1
#define FIA_PCIE_ROOT_PORT_2   2
#define FIA_PCIE_ROOT_PORT_3   3
#define FIA_PCIE_ROOT_PORT_4   4
#define FIA_PCIE_ROOT_PORT_5   5
#define FIA_PCIE_ROOT_PORT_6   6
#define FIA_PCIE_ROOT_PORT_7   7

#define FIA_PCIE_ROOT_PORT_CONFIG(Type, Config, PcieRootPort) \
  (((Type) == ME_FIA_PCIE_ROOT_PORT_STATE) ? \
      ((UINT64)((UINT64)(Config) << (UINT64)(PcieRootPort))) : \
      ((UINT64)((UINT64)(Config) << (UINT64)(((UINT64)(PcieRootPort) * (ME_FIA_PCIE_ROOT_PORT_LINK_WIDTH_SEL_WIDTH)) + ME_FIA_PCIE_ROOT_PORTS_STATE_WIDTH))))

typedef union _ME_FIA_PCIE_ROOT_PORTS_CONFIG {
  UINT64 MeFiaPcieRootPortsConfig;
  struct {
    UINT64 PcieRp0En        : 1;
    UINT64 PcieRp1En        : 1;
    UINT64 PcieRp2En        : 1;
    UINT64 PcieRp3En        : 1;
    UINT64 PcieRp4En        : 1;
    UINT64 PcieRp5En        : 1;
    UINT64 PcieRp6En        : 1;
    UINT64 PcieRp7En        : 1;
    UINT64 PcieRp0LinkWidth : 4;
    UINT64 PcieRp1LinkWidth : 4;
    UINT64 PcieRp2LinkWidth : 4;
    UINT64 PcieRp3LinkWidth : 4;
    UINT64 PcieRp4LinkWidth : 4;
    UINT64 PcieRp5LinkWidth : 4;
    UINT64 PcieRp6LinkWidth : 4;
    UINT64 PcieRp7LinkWidth : 4;
    UINT64 reserved         : 24;
  } MeFiaPcieRpConfig;
} ME_FIA_PCIE_ROOT_PORTS_CONFIG;

typedef enum
{
  ME_FIA_PCIE_ROOT_PORT_STATE,
  ME_FIA_PCIE_ROOT_PORT_LINK_WIDTH
}ME_FIA_PCIE_ROOT_PORT_CONFIG_TYPE;

typedef enum
{
  ME_FIA_PCIE_ROOT_PORT_DISABLED,
  ME_FIA_PCIE_ROOT_PORT_ENABLED
} ME_FIA_PCIE_ROOT_PORT_STATE_CONFIG;

typedef enum
{
  ME_FIA_PCIE_ROOT_PORT_LINK_WIDTH_BICTRL = 0,
  ME_FIA_PCIE_ROOT_PORT_LINK_X1 = 0xF
} ME_FIA_PCIE_ROOT_PORT_LINK_CONFIG;

typedef struct _ME_FIA_CONFIG
{
  ME_FIA_MUX_CONFIG               MuxConfiguration;
  ME_FIA_SATA_CONFIG              SataLaneConfiguration;
  ME_FIA_PCIE_ROOT_PORTS_CONFIG   PcieRootPortsConfiguration;
} ME_FIA_CONFIG;

#pragma pack()

/**
 * @breaf Read FIA MUX configuration from ME.
 *
 * @param[out] pConfig       Pointer to a buffer for ME FIA MUX configuration
 * @param[out] pLanesAllowed Pointer to a buffer for the number of lanes allowed
 *
 * @retval EFI_NOT_READY Returned if this API is called before ME signals init
 *                       complete, or after END_OF_POST was sent.
 * @retval EFI_INVALID_PARAMETER Returned if any pointers passed is NULL
 * @retval EFI_DEVICE_ERROR Returned if HECI transport is not available.
 * @retval EFI_PROTOCOL_ERROR Unexpected protocol error returned by ME.
 */
EFI_STATUS
EFIAPI
MeFiaMuxConfigGet(OUT ME_FIA_CONFIG *pConfig, OUT UINT32 *pLanesAllowed);

/**
 * Sends FIA MUX configuration to ME.
 *
 * param[in]  pConfig       Pointer to FIA MUX configuration to set in ME,
 *                          when NULL, Invalidate field is set to 1 (causing
 *                          invalidate of configuration stored in flash -
 *                          configuration from Soft Straps will be used)
 *
 * @retval EFI_NOT_READY Returned if this API is called before ME signals init
 *                       complete, or after END_OF_POST was sent.
 * @retval EFI_DEVICE_ERROR Returned if HECI transport is not available.
 * @retval EFI_PROTOCOL_ERROR Unexpected protocol error returned by ME.
 * @retval EFI_OUT_OF_RESOURCES Requested configuration refused by ME, because
 *                              it does not match the SoC SKU.
 * @retval EFI_ABORTED Requested configuration refused by ME, because it is
 *                     invalid
 */
EFI_STATUS
EFIAPI
MeFiaMuxConfigSet(IN ME_FIA_CONFIG *pConfig);

#endif // _ME_FIA_MUX_LIB_H_
