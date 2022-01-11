/*++

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  UfsHcRegs.h

Abstract:

  Header file for the UFS HC Regiters.

--*/

#ifndef __UFS_HC_REGS_H__
#define __UFS_HC_REGS_H__

// MMIO mapped UFS HC Registers offset
#define UFS_HC_CAP      0x00    // UFS Host Controller Capabilities
    #define M_UICDMETMS    BIT26   // UIC DME_TEST_MODE command supported
    #define M_OODDS        BIT25   // Out of order data delivery supported
    #define M_64AS         BIT24   // Indicates whether the host controller can access 64-bit data structures
    #define M_NUTMRS       BIT18 | BIT17 | BIT16   // Number of UTP Task Management Request Slots
    #define S_NUTMRS       16
    #define M_NUTRS        BIT4 | BIT3 | BIT2 | BIT1 | BIT0 //Number of UTP Transfer Request Slots

#define UFS_HC_VER      0x08                     // UFS Version of the UFSHCI specification supported by controller
    #define M_UFS_HC_MAJOR_VERSION     0xFFFF0000   // Indicates Major version
    #define M_UFS_HC_MINOR_VERSION     0x0000FFFF   // Indicates Minor version

#define UFS_HCDDID      0x10                        // Host Controller Identification Descriptor - Device ID and Device Class
    #define S_UFS_HC_HCDID 24
    #define M_UFS_HC_HCDID (0xFF << S_UFS_HC_HCDID)    // UniPro Device ID assigned for host controller
    #define M_UFS_HC_DC    0x0000FFFF                  // Device class for host controller (MIPI assigned)

#define UFS_HC_HCPMID   0x14                        // Host Controller Identification Descriptor - Product ID and Manufacturer ID
    #define S_UFS_HC_PID   16
    #define M_UFS_HC_PID   (0xFFFF << S_UFS_HC_PID)    // Product ID for host controller maker
    #define M_UFS_HC_MID   0xFFFF                      // Manufacturer ID (MIPI assigned) of host controller maker

// UFS HC Operation and Runtime Registers
#define UFS_HC_IS       0x20                        // UFS Host Controller Interrupt Status
    #define M_UFS_HC_SBFES BIT17                       // System Bus Fatal Error Status 
    #define M_UFS_HC_HCFES BIT16                       // Host Controller Fatal Error Status
    #define M_UFS_HC_UTPES BIT12                       // UTP Error Status
    #define M_UFS_HC_DFES  BIT11                       // Device Fatal Error Status
    #define M_UFS_HC_UCCS  BIT10                       // UIC Command Completion Status
    #define M_UFS_HC_UTMRCS  BIT9                      // UTP Task Management Request Completion Status
    #define M_UFS_HC_ULSS  BIT8                        // UIC Link Startup Status
    #define M_UFS_HC_ULLS  BIT7                        // UIC Link Lost Status
    #define M_UFS_HC_UHES  BIT6                        // UIC Hibernate Enter Status
    #define M_UFS_HC_UHXS  BIT5                        // UIC Hibernate Exit Status
    #define M_UFS_HC_UPMS  BIT4                        // UIC Power Mode Status
    #define M_UFS_HC_UTMS  BIT3                        // UIC Test Mode Status
    #define M_UFS_HC_UE    BIT2                        // UIC Error
    #define M_UFS_HC_UDEPRI BIT1                        // UIC DME_ENDPOINTRESET Indication
    #define M_UFS_HC_UTRCS  BIT0                        // UTP Transfer Request Completion Status

#define UFS_HC_IE       0x24    // UFS Host Controller Interrupt Enable
    #define M_UFS_HC_SBFEE BIT17   // System Bus Fatal Error Enable
    #define M_UFS_HC_HCFEE BIT16   // Host Controller Fatal Error Enable                    
    #define M_UFS_HC_UTPEE BIT12   // UTP Error Enable 
    #define M_UFS_HC_DFEE  BIT11   // Device Fatal Error Enable
    #define M_UFS_HC_UCCE  BIT10   // UIC COMMAND Completion Enable
    #define M_UFS_HC_UTMRCE BIT9   // UTP Task Management Request Completion Enable
    #define M_UFS_HC_ULSSE  BIT8   // UIC Link Startup Status Enable
    #define M_UFS_HC_ULLSE  BIT7   // UIC Link Lost Status Enable 
    #define M_UFS_HC_UHESE  BIT6   // UIC Hibernate Enter Status Enable 
    #define M_UFS_HC_UHXSE  BIT5   // UIC Hibernate Exit Status Enable
    #define M_UFS_HC_UPMSE  BIT4   // UIC Power Mode Status Enable 
    #define M_UFS_HC_UTMSE  BIT3   // UIC Test Mode Status Enable
    #define M_UFS_HC_UEE    BIT2   // UIC Error Enable 
    #define M_UFS_HC_UDEPRIE BIT1  // UIC DME_ENDPOINTRESET
    #define M_UFS_HC_UTRCE  BIT0   // UTP Transfer Request Completion Enable

#define UFS_HC_HCS      0x30    // UFS Host Controller Status
    #define S_UFS_HC_TLUNUTPE 24
    #define M_UFS_HC_TLUNUTPE (0xFF << S_UFS_HC_TLUNUTPE)  // The LUN of the command that a UTP error occurs during execution of the command
    #define S_UFS_HC_TTAGUTPE  16
    #define M_UFS_HC_TTAGUTPE  (0xFF << S_UFS_HC_TTAGUTPE) // The Task Tag of the command that a UTP error occurs during execution of the command.
    #define S_UFS_HC_UTPEC  12
    //#define M_UFS_HC_TTAGUTPE  (0xF << S_UFS_HC_TTAGUTPE) // Indicate that the error code of a UTP layer error.
        #define UTPEC_INV_UPIU    0x1     // Invalid UPIU type 
    #define S_UFS_HC_UPMCRS    8 
    #define M_UFS_HC_UPMCRS    BIT10 | BIT9 | BIT8 // UIC Power Mode Change Request Status Indicate that the status of a UIC layer request for power mode change.
        #define UFS_UPMRCS_PWR_OK                 0   // The request was accepted
        #define UFS_UPMRCS_PWR_LOCAL              1   // The local request was successfully applied
        #define UFS_UPMRCS_PWR_REMOTE             2   // The remote request was successfully applied
        #define UFS_UPMRCS_PWR_BUSY               3   // The request was aborted due to concurrent requests
        #define UFS_UPMRCS_PWR_ERROR_CAP          4   // The request was rejected because the requested configuration exceeded the Link’s capabilities
        #define UFS_UPMRCS_PWR_PWR_FATAL_ERROR    5   // The request was aborted due to a communication problem. The Link may be inoperable
    #define M_UFS_HC_UCRDY     BIT3    // UIC COMMAND Ready
    #define M_UFS_HC_UTMRLRDY  BIT2    // UTP Task Management Request List Ready 
    #define M_UFS_HC_UTRLRDY   BIT1    // UTP Transfer Request List Ready
    #define M_UFS_HC_DP        BIT0    // Device Present (DP): This field is set to ''  when an UFS device is attached to the controller

#define UFS_HC_HCE      0x34    // UFS Host controller Enable
    #define M_UFS_HC_HCE   BIT0    // 0 - Disable, 1 - Enable: when set to 1 by SW, this bit cause host controller and local UIC layer to be reset via the UFS Host Controller internal HW reset

#define UFS_HC_UECPA    0x38    // Host UIC Error Code PHY Adapter Layer
    #define M_UFS_HC_UECPA_ERR     BIT31   // UIC PHY AdapterA Layer Error Indicates whether an error was generated by the PHY Adapter Layer
    #define M_UFS_HC_UECPA_EC      0x1F    // UIC PHY Adapter Layer Error Code
        #define M_UFS_HC_UECPA_EC_PHY_LANE0   0x00    // Error reported by the M-PHY layer: PHY error on Lane 0  
        #define M_UFS_HC_UECPA_EC_PHY_LANE1   0x01    // Error reported by the M-PHY layer: PHY error on Lane 1
        #define M_UFS_HC_UECPA_EC_PHY_LANE2   0x02    // Error reported by the M-PHY layer: PHY error on Lane 2
        #define M_UFS_HC_UECPA_EC_PHY_LANE3   0x03    // Error reported by the M-PHY layer: PHY error on Lane 3
        #define M_UFS_HC_UECPA_EC_GPERR       0x04    // Generic PHY Adapter Error. This should be the LINERESET indication. 
                                                    // Categorized as "ERROR" (Not FATAL). 
                                                    // SW is informed that M-PHY has been reset and 
                                                    // all M-PHY Attributes (that are not handled by UniPro) need to be restored in order to keep the link optimized.

#define UFS_HC_UECDL    0x3C    // Host UIC Error Code Data Link Layer
    #define M_UFS_HC_UECDL_ERR     BIT31   // UIC Data Link Layer Error Indicates whether an error was generated by the Data Link Layer
    #define M_UFS_HC_UECDL_EC      0x7FFF  // UIC Data Link Layer Error Code (EC): error code generated when IS.UE and UECDL.ERR are set to '1'
        #define UFS_HC_UECDL_EC_NAC_RECEIVED                    0
        #define UFS_HC_UECDL_EC_TCx_REPLAY_TIMER_EXPIRED        1
        #define UFS_HC_UECDL_EC_AFCx_REQUEST_TIMER_EXPIRED      2
        #define UFS_HC_UECDL_EC_FCx_PROTECTION_TIMER_EXPIRED    3
        #define UFS_HC_UECDL_EC_CRC_ERROR                       4
        #define UFS_HC_UECDL_EC_RX_BUFFER_OVERFLOW              5
        #define UFS_HC_UECDL_EC_MAX_FRAME_LENGTH_EXCEEDED       6
        #define UFS_HC_UECDL_EC_WRONG_SEQUENCE_NUMBER           7
        #define UFS_HC_UECDL_EC_AFC_FRAME_SYNTAX_ERROR          8
        #define UFS_HC_UECDL_EC_NAC_FRAME_SYNTAX_ERROR          9
        #define UFS_HC_UECDL_EC_EOF_SYNTAX_ERROR                10
        #define UFS_HC_UECDL_EC_FRAME_SYNTAX_ERROR              11
        #define UFS_HC_UECDL_EC_BAD_CTRL_SYMBOL_TYPE            12
        #define UFS_HC_UECDL_EC_PA_INIT_ERROR                   13  
        #define UFS_HC_UECDL_EC_PA_ERROR_IND_RECEIVED           14

#define UFS_HC_UECN     0x40    // Host UIC Error Code Network Layer
    #define M_UFS_HC_UECN_ERR       BIT31   // UIC Network Layer Error (ERR): Indicates whether an error was generated by the Network Layer
    #define M_UFS_HC_UECN_EC        0x07    // UIC Network Layer Error Code (EC): error code generated when IS.UE and UECN.ERR are set to '1'
        #define UFS_HC_UECN_EC_USUP                         0   // UNSUPPORTED_HEADER_TYPE
        #define UFS_HC_UECN_EC_BAD_DID_ENC                  1   // BAD_DEVICEID_ENC
        #define UFS_HC_UECN_EC_LHDR_TRAP_PACKET_DROPPING    2   // LHDR_TRAP_PACKET_DROPPING

#define UFS_HC_UECT     0x44    // Host UIC Error Code Transport Layer
    #define M_UFS_HC_UECT_ERR   BIT31   // UIC Transport Layer Error (ERR): Indicates whether an error was generated by the Transport Layer
    #define M_UFS_HC_UECT_EC    0x7f    // UIC Transport Layer Error Code 
        #define UFS_HC_UECT_EC_UNSUPPORTED_HEADER_TYPE      0
        #define UFS_HC_UECT_EC_UNKNOWN_CPORTID              1    
        #define UFS_HC_UECT_EC_NO_CONNECTION_RX             2
        #define UFS_HC_UECT_EC_CONTROLLED_SEGMENT_DROPPING  3
        #define UFS_HC_UECT_EC_BAD_TC                       4
        #define UFS_HC_UECT_EC_E2E_CREDIT_OVERFLOW          5
        #define UFS_HC_UECT_EC_SAFETY_VALVE_DROPPING        6


#define UFS_HC_UECDME   0x48    // Host UIC Error Code
    #define M_UFS_HC_UECDME_ERR     BIT31   // UIC DME Error (ERR): Indicates whether an error was generated by the DME
    #define M_UFS_HC_UECDME_EC      0x1     // Generic DME Error

#define UFS_HC_UTRIACR  0x4c    // UTP Transfer Request Interrupt Aggregation Control Register
    // Bit masks TBD

// UFS Transfer Request Registers

#define UFS_HC_UTRLBA   0x50    // UTP Transfer Request List Base Address
    #define M_UFS_HC_UTRLBA     0xFFFFFC00      // 1 KB Aligned Base address  
 
#define UFS_HC_UTRLBAU  0x54    // UTP Transfer Request List Base Address Upper 32-bits
 
#define UFS_HC_UTRLDBR  0x58    // UTP Transfer Request List Door Bell Register
 
#define UFS_HC_UTRLCLR  0x5c    // UTP Transfer Request List CLear Register
                                // The host software shall use this field only when a UTP Transfer Request 
                                // is expected to not be completed, e.g., when a Transfer Request was aborted, 
                                // or in case of a system b

#define UFS_HC_VID          0x8086
#if  _SLE_HYB_
#define UFS_HC_DEVID        0x22E0
#define UFS_HC_BUS_NO       0x0
#define UFS_HC_DEVICE_NO    0xD
#else
#define UFS_HC_DEVID        0x0ACE
#define UFS_HC_BUS_NO       0x0
#define UFS_HC_DEVICE_NO    0x1D
#define UFS_HC_FUN_NO       0x0
#endif

#define UFS_HC_SSVID        0x0000         
 

#define DW_UFS_IS         0x20
#define DW_UFS_HCS        0x30

 #endif
