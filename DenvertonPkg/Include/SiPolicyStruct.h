/** @file
  Intel reference code configuration policies.

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _SI_POLICY_STRUCT_H_
#define _SI_POLICY_STRUCT_H_

/**
  Silicon Policy revision number
  Any backwards compatible changes to this structure will result in an update in the revision number

  Revision 1:   Initial version
**/
#define SI_POLICY_REVISION  1

/**
  The Silicon Policy allows the platform code to publish a set of configuration
  information that the RC drivers will use to configure the silicon hardware.
**/
struct _SI_POLICY_STRUCT {
  /**
    This member specifies the revision of the Silicon Policy. This field is used to indicate backward
    compatible changes to the policy structure. Any such changes to this policy structure will result in an update
    in the revision number.

    <b>Revision 1</b>:
     - Initial version.
  **/
  UINT8  Revision;
  //
  // Platform specific common policies that used by several silicon components.
  //
  ///
  /// The minimum temporary bus number available to be assigned to each root port and its downstream
  /// devices for initialization of these devices before PCI Bus enumeration.
  ///
  UINT8  TempPciBusMin;
  ///
  /// The maximum temporary bus number available to be assigned to each root port and its downstream
  /// devices for initialization of these devices before PCI Bus enumeration.
  ///
  UINT8  TempPciBusMax;
  UINT8  Reserved;            ///< Reserved bytes, align to multiple 4.
  ///
  /// Temporary Memory Base Address for PCI devices to be used to initialize MMIO registers.
  /// Minimum size is 2MB bytes
  ///
  UINT32 TempMemBaseAddr;
  ///
  /// Size of allocated temp MMIO space
  ///
  UINT32 TempMemSize;
  ///
  /// Temporary IO Base Address for PCI devices to be used to initialize IO registers.
  ///
  UINT16 TempIoBaseAddr;
  UINT16 TempIoSize;          ///< Size of temporary IO space
  UINT32 EcPresent :  1;      ///< Indicate Embeded Controller is present in platform.
  UINT32 Rsvdbits  : 31;
};

#endif
