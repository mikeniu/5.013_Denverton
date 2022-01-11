/*++
 This file contains an 'Intel Peripheral Driver' and uniquely
 identified as "Intel Reference Module" and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
 --*/

/*++

 Copyright (c)  2015 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.


 Module Name:

 PeiPcieIpPolicy.h

 Abstract:

 Driver for Pcie Policy structure.

 Revision History

 --*/

#ifndef _PCIE_IP_POLICY_H_
#define _PCIE_IP_POLICY_H_

#include <PcieIpPlatform.h>

#define MAX_PCIE_ROOT_PORTS  (PCIE_IP_STANDARD_ROOT_PORT + PCIE_IP_VIRTUAL_ROOT_PORT) 	//< Standard root ports + Virtual root ports
#define MAX_PCIE_CLUSTERS    2

//type of root port
typedef enum {
	StandardRootPort = 0,
	VirtualRootPort
} PCIE_TYPE;

// type of bifurcation
typedef enum {
	X2X2X2X2 = 0,
	X2X2X4,
	X4X2X2,
	X4X4,
	X8
} PCIE_BIFURCATION;

typedef enum {
  PcieCompletionTO_Default = 0,
  PcieCompletionTO_50us_50ms,
  PcieCompletionTO_50us_100us,
  PcieCompletionTO_1ms_10ms,
  PcieCompletionTO_16ms_55ms,
  PcieCompletionTO_65ms_210ms,
  PcieCompletionTO_260ms_900ms,
  PcieCompletionTO_1s_3_5s,
  PcieCompletionTO_4s_13s,
  PcieCompletionTO_17s_64s
} PCIE_COMPLETION_TIMEOUT;

///
/// Determines each PCIE Port speed capability.
/// 0: Auto; 1: Gen1; 2: Gen2; 3: Gen3 (see: PCH_PCIE_SPEED)
///
typedef enum {
	PcieAuto,
	PcieGen1,
	PcieGen2,
	PcieGen3
} PCIE_SPEED;

typedef struct {
	UINT32 Segment;
	UINT32 InterruptSwizzleConfig;
} PCIE_INT_SWIZZLE;

// structure for mapping bus/device/function for each root port ( standard y virtual)
typedef struct {
	UINT32 PortNumber;		//< Number port
	PCIE_TYPE PortType;		//< Type of port
	BOOLEAN Enabled;		//< True= actived or False = no active
	UINT32 BusNum;			//< bus number
	UINT32 DevNum;			//< Device
	UINT32 FuncNum;		//< Function
	UINT32 RPBase;			//< Address direction
	UINT8 SlotImplemented;			//NEW ADDED
} PCIE_IP_BDF_MAP_ENTRY;

///
/// PCIE Device Control configuration
///
typedef struct {
	UINT8 MaxPayload;
	UINT8 MaxReadRequest;
	UINT8 ExtTagField;
	UINT8 RelaxedOrdering;
	UINT8 EnableSLD;
	UINT8 LtrEnable;                              ///< Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
} PCIE_DEV_CTL;

//cluster structure
typedef struct {
	PCIE_BIFURCATION Bifurcation;
	UINT32 PcrAddress;
	UINT32 Straps;
    UINT8 ReplyTimerTimeout;
#ifdef FSP_FLAG
    UINT8 PcieEnable;
#endif
} PCIE_IP_CLUSTER_POLICY;

//single root port structure
typedef struct {
	PCIE_INT_SWIZZLE InterruptSwizzle;
	UINT32 RPBase;						//< Address direction
	BOOLEAN VirtualRootPort;
	UINT8 ClockGatingEn;
	PCIE_SPEED PcieSpeed;					//< Speed in PEI
	PCIE_DEV_CTL DeviceCtlConfig; 		//< PCIE Device Control configuration
	UINT8 ExtSync :1;					//< Configure Extended Synch
	UINT8 CtlLaneReversal :2;			//< CtlLaneReversal
	UINT8 DeEmphasis :1;				//< De-emphasis Level for root ports
	UINT8 Padding :4;					//< Extended Synch
    UINT8 ExtTagField; 					//[Disable]\ Enable		NEW ADDED
    UINT8 WheaPcieErrInj;				//< WHEA	NEW ADDED
    UINT8 PmSci;					// NEW ADDED
    UINT8 HotPlug;					// new added
    UINT8 UnsupportedRequestReport;			// new added
    UINT8 TransmitterHalfSwing;				// new added
    UINT8 CompletionTimeout;
    UINT8 Lock;
    UINT8 StopScream;
    UINT8 CompletionTimeoutRange;
    UINT8 Aspm;                     //< activate ASPM L1
} PCIE_IP_PORT_POLICY;

//Endpoint PCIE
typedef struct {
	PCIE_DEV_CTL DeviceCtlConfig; 		//< PCIE Device Control configuration
    UINT8 Aspm;                     //< activate ASPM L1
	UINT8 ExtSync;					//< Configure Extended Synch
} PCIE_IP_EP_POLICY;

//global root port structure
struct PCIE_IP_GLOBAL_POLICY {
	BOOLEAN HandoffEnable;
	UINT32 NumPcieClusters;								//< count of clusters
	UINT32 NumPcieRootPorts;								//< count of root ports
	UINT32 PciePllHandoffBaseAddress;
	UINT32 PciExpressBaseAddress;
	UINT8 EnableMphyMsg;
	PCIE_IP_BDF_MAP_ENTRY PcieIpPortBdfMap[MAX_PCIE_ROOT_PORTS];//< structure for mapping bus/device/function for each root port ( standard y virtual)
	PCIE_IP_CLUSTER_POLICY PcieIpClusterPolicy[MAX_PCIE_CLUSTERS];//< cluster structure
	PCIE_IP_PORT_POLICY PcieIpPortPolicy[MAX_PCIE_ROOT_PORTS];//< single root port structure
	PCIE_IP_EP_POLICY PcieIPEndPoints;
};

#endif
