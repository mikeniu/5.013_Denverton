@REM
@REM This file contains an 'Intel Peripheral Driver' and is      
@REM licensed for Intel CPUs and chipsets under the terms of your
@REM license agreement with Intel or your vendor.  This file may 
@REM be modified by the user, subject to additional terms of the 
@REM license agreement                                           
@REM
@REM #/*++
@REM #  
@REM #  Copyright (c) 2010 - 2012, Intel Corporation. All rights reserved.<BR>
@REM #  This software and associated documentation (if any) is furnished
@REM #  under a license and may only be used or copied in accordance
@REM #  with the terms of the license. Except as permitted by such
@REM #  license, no part of this software or documentation may be
@REM #  reproduced, stored in a retrieval system, or transmitted in any
@REM #  form or by any means without the express written consent of
@REM #  Intel Corporation.
@REM #
@REM #  Module Name:
@REM #  
@REM #    BuildImage.bat
@REM #  
@REM #  Abstract:
@REM #    
@REM #    Platform full 8M flash image build script. 
@REM #
@REM #--*/
if not exist %OUTPUT_DIR%\ROM mkdir %OUTPUT_DIR%\ROM

set PLATFORM_PKG=HarrisonvillePkg
set FIT_DIR=%PLATFORM_PKG%\Tools\FTool
set PATH=%PATH%;C:\python27;C:\python26;C:\python34

@REM ---run FIT and create ROM images---
@cd %WORKSPACE%\%PLATFORM_PKG%\Tools

if %CRB_PLATFORM_SELECT%==1 (
%WORKSPACE%\%FIT_DIR%\FlashImageTool\spsFITc.exe -w . -b -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -f %WORKSPACE%\%FIT_DIR%\Config\DNVHV_SiEn.xml -o %WORKSPACE%\%OUTPUT_DIR%\ROM\spi_image_HV.bin -flashcount 1 -flashsize1 5 -gbea %WORKSPACE%\%FIT_DIR%\DNV_LAN0_KR_Inphi_noMNG_1.16_0.05_8000084F.bin -gbeb %WORKSPACE%\%FIT_DIR%\DNV_LAN1_KR_Inphi_noMNG_1.16_0.05_8000086B.bin -opr %WORKSPACE%\%FIT_DIR%\spsOperational.bin -rcv %WORKSPACE%\%FIT_DIR%\spsRecovery.bin -unlockToken %WORKSPACE%\%FIT_DIR%\Tokens\orange_global_token.bin
)
if %CRB_PLATFORM_SELECT%==2 (
if %CML_GBE_NCSI_SUPPORT%==0 (
%WORKSPACE%\%FIT_DIR%\FlashImageTool\spsFITc.exe -w . -b -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -f %WORKSPACE%\%FIT_DIR%\Config\DNVCL_SiEn.xml -o %WORKSPACE%\%OUTPUT_DIR%\ROM\spi_image_CL.bin -flashcount 1 -flashsize1 5 -gbea %WORKSPACE%\%FIT_DIR%\DNV_LAN0_KR_Inphi_noMNG_1.16_0.05_8000084F.bin -gbeb %WORKSPACE%\%FIT_DIR%\DNV_LAN1_KR_Inphi_noMNG_1.16_0.05_8000086B.bin -opr %WORKSPACE%\%FIT_DIR%\spsOperational.bin -rcv %WORKSPACE%\%FIT_DIR%\spsRecovery.bin -unlockToken %WORKSPACE%\%FIT_DIR%\Tokens\orange_global_token.bin
)else (
%WORKSPACE%\%FIT_DIR%\FlashImageTool\spsFITc.exe -w . -b -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -f %WORKSPACE%\%FIT_DIR%\Config\DNVCL_SiEn.xml -o %WORKSPACE%\%OUTPUT_DIR%\ROM\spi_image_CL.bin -flashcount 1 -flashsize1 5 -gbea %WORKSPACE%\%FIT_DIR%\DNV_LAN0_KR_Inphi_NCSI_1.16_0.05_8000084E.bin -gbeb %WORKSPACE%\%FIT_DIR%\DNV_LAN1_KR_Inphi_NCSI_1.16_0.05_8000086A.bin -opr %WORKSPACE%\%FIT_DIR%\spsOperational.bin -rcv %WORKSPACE%\%FIT_DIR%\spsRecovery.bin -unlockToken %WORKSPACE%\%FIT_DIR%\Tokens\orange_global_token.bin
)
)
if %CRB_PLATFORM_SELECT%==3 (
%WORKSPACE%\%FIT_DIR%\FlashImageTool\spsFITc.exe -w . -b -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -f %WORKSPACE%\%FIT_DIR%\Config\DNVAC_SiEn.xml -o %WORKSPACE%\%OUTPUT_DIR%\ROM\spi_image_AC.bin -flashcount 1 -flashsize1 5 -gbea %WORKSPACE%\%FIT_DIR%\DNV_LAN0_KR_Inphi_noMNG_1.16_0.05_8000084F.bin -gbeb %WORKSPACE%\%FIT_DIR%\DNV_LAN1_KR_Inphi_noMNG_1.16_0.05_8000086B.bin -opr %WORKSPACE%\%FIT_DIR%\spsOperational.bin -rcv %WORKSPACE%\%FIT_DIR%\spsRecovery.bin -unlockToken %WORKSPACE%\%FIT_DIR%\Tokens\orange_global_token.bin
)
@cd %WORKSPACE%
