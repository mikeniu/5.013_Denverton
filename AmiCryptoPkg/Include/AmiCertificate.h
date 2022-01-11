//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014 American Megatrends, Inc.          **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  AmiCertificate.h
  AMI FW Certificate defines and data structures
**/

#ifndef __AMI_CERTIFICATE_H__
#define __AMI_CERTIFICATE_H__

#include <Efi.h>
// All are EDKII defined headers
//#include "WinCertificate.h"
#include <Guid/WinCertificate.h>
//#include "ImageAuthentication.h"
#include <Guid/ImageAuthentication.h>
#ifndef ROM_AREA
#include <RomLayout.h>
#endif

#pragma pack(1)

#ifndef offsetof
#define offsetof(st, m) \
     ((UINTN) ( (char *)&((st *)0)->m - (char *)0 ))
#endif

//**********************************************************************
// AMI FW Certificate defines
//**********************************************************************

// Aptio FW Capsule
// Update CapsuleMain.c and CapsuleRt.c to process APTIO_FW_CAPSULE_GUID capsules as normal CAPSULE
//#define APTIO_FW_CAPSULE_GUID \
//    { 0xc50e4b4a, 0x302, 0x41fe, 0x9b, 0x99, 0x43, 0xde, 0x77, 0x7, 0x6, 0x3f }

///
/// Aptio FW Capsule with extended header fields.
///4A3CA68B-7723-48FB-803D-578CC1FEC44D
///
#define APTIO_FW_CAPSULE_GUID \
    { 0x4A3CA68B, 0x7723, 0x48FB, 0x80, 0x3d, 0x57, 0x8c, 0xc1, 0xfe, 0xc4, 0x4d }

///
///3FEEC852-F14C-4E7F-97FD-4C3A8C5BBECC
/// Key format: Raw 256 byte N-Modulus of RSA2048 Key
///
#define PR_KEY_FFS_FILE_RAW_GUID \
    { 0x3FEEC852, 0xF14C, 0x4E7F, 0x97, 0xFD, 0x4C, 0x3A, 0x8C, 0x5B, 0xBE, 0xCC }

///
/// 5B85965C-455D-4CC6-9C4C-7F086967D2B0
/// Key format: SHA256 hash of 256 byte N-Modulus of RSA2048 Key
///
#define PR_KEY_FFS_FILE_SHA256_GUID \
    { 0x05B85965C, 0x455D, 0x4CC6, 0x9C, 0x4C, 0x7F, 0x08, 0x69, 0x67, 0xD2, 0xB0 }

///
/// 3A666558-43E3-4D25-9169-DB81F5DB42E1
/// Key format: x509 ASN.1 DER containing RSA2048 Public Key
///
#define PR_KEY_FFS_FILE_X509_GUID \
    { 0x3A666558, 0x43E3, 0x4D25, 0x91, 0x69, 0xdb, 0x81, 0xf5, 0xdb, 0x42, 0xe1 }

///
/// APTIO Platform Fw Root Signing key identifier
/// {4E0F9BD4-E338-4B26-843E-BD3AD9B2837B}
///
#define PR_KEY_GUID \
    { 0x4E0F9BD4, 0xE338, 0x4B26, 0x84, 0x3E, 0xBD, 0x3A, 0xD9, 0xB2, 0x83, 0x7B }

//9E625A27-4840-47CC-A6B5-1E9311CFC60E
//#define PK_PUB_FFS_FILE_EFI_AUTH_VAR_GUID \
//    { 0x9E625A27, 0x4840, 0x47CC, 0xA6, 0xB5, 0x1E, 0x93, 0x11, 0xCF, 0xC6, 0x0E }

// APTIO PK Key identifier
// {A6C0E11E-929E-42b3-90CC-4F778E03FF57}
//#define PKPUB_KEY_GUID \
//    { 0xa6c0e11e, 0x929e, 0x42b3, 0x90, 0xcc, 0x4f, 0x77, 0x8e, 0x3, 0xff, 0x57 }

// The 256 byte buffer containing the public modulus of RSA2048 Public Key 
// Second part of the Public key, public exponent (e), must be hardwired elsewhere in the FW (recommended value of e=65537).
// use EFI_CERT_RSA2048_GUID instead
//#define AMI_PB_KEY_RSA2048_GUID \
//    { 0x04627B9B, 0x385E, 0x4744, 0x90, 0x21, 0xE6, 0x63, 0x19, 0xF2, 0x03, 0x94 }

///
/// AMI APTIO 4 Signature Owner id.
/// An identifier which identifies the agent which added the signature to the list.
///
#define AMI_APTIO_SIG_OWNER_GUID \
    { 0x26DC4851, 0x195F, 0x4AE1, 0x9A, 0x19, 0xFB, 0xF8, 0x83, 0xBB, 0xB3, 0x5E }

///
///Platform FwKey GUIDed HOB
///Header HobGUID = PR_KEY_GUID
///
typedef struct {
    EFI_HOB_GUID_TYPE   Header;
    EFI_GUID            KeyGuid;
    EFI_PHYSICAL_ADDRESS KeyAddress;
    UINT32              KeySize;
} FW_KEY_HOB;

///
/// Modified version of Win Uefi Cert. CertData is not defined in here
///
typedef struct {
  WIN_CERTIFICATE   Hdr;
  EFI_GUID          CertType;
//  UINT8            CertData[1];
//    EFI_CERT_BLOCK_RSA_2048_SHA256  CertData;
} WIN_CERTIFICATE_UEFI_GUID_1;

///
/// Certificate which encapsulates the UEFI RSA2048_SHA256 digital signature.
///
/// The WIN_CERTIFICATE_UEFI_GUID structure is derived from
/// WIN_CERTIFICATE and encapsulate the information needed to  
/// implement the digital signature algorithm
/// Hdr.Hdr.wRevision = 0x0200
/// Hdr.Hdr.wCertificateType = 0x0EF1; WIN_CERT_TYPE_EFI_GUID
/// Hdr.CertType = EFI_CERT_TYPE_RSA2048_SHA256_GUID;
///
/// Fields: Name     Type        Description
///----------------------------------------------------------------------------
/// Hdr      WIN_CERTIFICATE_UEFI_GUID   encapsulate the information needed to implement the digital signature algorithm 
/// CertData  EFI_CERT_BLOCK_RSA_2048_SHA256    Signature and Public key
///----------------------------------------------------------------------------
///
typedef struct {
    WIN_CERTIFICATE_UEFI_GUID_1      Hdr;        // 24
    EFI_CERT_BLOCK_RSA_2048_SHA256   CertData;   // 16+256+256
} AMI_CERTIFICATE_RSA2048_SHA256;
//(sizeof(AMI_CERTIFICATE_RSA2048_SHA256) == 552);

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        FW_CERTIFICATE
//
// Description:    Aptio FW Capsule certificate block
//
// Fields: Name     Type        Description
//----------------------------------------------------------------------------
// SignCert      AMI_CERTIFICATE_RSA2048_SHA256    Certificate contains a signature of a Capsule's payload and Signer's public key
// RootCert      EFI_CERT_BLOCK_RSA_2048_SHA256    Certificate contains a signature of a Signer's public key signed by the 
//                                                   Root Key, whose public portion is also included into this certificate. 
//    For Aptio FW update process to succeed, either of these keys must match the instance of a Platform FW Key in the Flash.
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct {
    AMI_CERTIFICATE_RSA2048_SHA256   SignCert;   // 24+16+256+256
    EFI_CERT_BLOCK_RSA_2048_SHA256   RootCert;   // 16+256+256
} FW_CERTIFICATE;
//(sizeof(FW_CERTIFICATE) == 1112);

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        APTIO_FW_CAPSULE_HEADER
//
// Description: Enhanced APTIO FW CAPSULE with certificate and the map of the modules in the 
//              capsule's volume to be included into the signature calculation
// Fields: 
//  Name            Type                Description
//----------------------------------------------------------------------------
//  CapHdr           EFI_CAPSULE_HEADER  UEFI 2.0 Capsule Hdr. Use APTIO_FW_CAPSULE_2_GUID
//  RomImageOffset   UINT16              offset within the Fw Capsule header to the FW image
//  RomLayoutOffset  UINT16              offset to the FW descriptor table used for signature calculation
//  FWCert           FW_CERTIFICATE      Certificate block. Format may change based on WIN_CERTIFICATE header Type.
//                                       Hdr.CertType = EFI_CERT_TYPE_RSA2048_SHA256_GUID;
//                                       Hdr.CertType = EFI_CERT_TYPE_PKCS7_GUID; 
//  ....
//  RomAreaMap       ROM_AREA
//  ....
//  Payload    
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct {
    EFI_CAPSULE_HEADER    CapHdr;
    UINT16                RomImageOffset;
    UINT16                RomLayoutOffset;
    FW_CERTIFICATE        FWCert;
    ROM_AREA              RomAreaMap[1];
} APTIO_FW_CAPSULE_HEADER;

// CapHdr.Flags Extended FwCapsule GUID flags
// RSA Signature padding scheme:
// 0-PKCS1v5
// 1-PSS
#define CAPSULE_FLAGS_RSA_PSS_PADDING_SCHEME    0x00000001 
// Blocks used for signing by Signing Certificate:
// 0-RomMap + Payload
// 1-FwCapHdr + RomMap + Payload
#define CAPSULE_FLAGS_CAPHDR_IN_SIGNCERT      0x00000002 
// Blocks used for signing by Root Certificate:
// 0-FwCert.SignCert structure
// 1-FwCert.SignCert.PublicKey field
#define CAPSULE_FLAGS_SIGNKEY_IN_ROOTCERT      0x00000004 
// Validity of the Signatures within FwCert block
// 0-Capsule Payload signatures are current in FwCert block
// 1-Capsule Payload Signatures are invalid
#define CAPSULE_FLAGS_FWCERTBLOCK_VALID          0x00000008 

#pragma pack()
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014 American Megatrends, Inc.          **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
