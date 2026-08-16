//-----------------------------------------------------------------------------
//  (c) 2012 by Allied Vision Technologies GmbH
//  Project: GenTLValidation
//  Author:  SVW
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group.
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
//
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef SYSTEM_H
#define SYSTEM_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "SystemTest.h"
#include "System_TLUpdateInterface.h"
#include "System_TLOpenInterface.h"
#include "System_TLGetNumInterfaces.h"
#include "System_TLGetInterfaceID.h"
#include "System_TLGetInfo.h"
#include "System_TLGetInterfaceInfo.h"

GENTLTEST_UNIT_DECL(SystemTest, TestTLOpen);
GENTLTEST_UNIT_DECL(SystemTest, TestTLOpenWithoutGCInitLib);
GENTLTEST_UNIT_DECL(SystemTest, TestTLOpenTwice);
GENTLTEST_UNIT_DECL(SystemTest, TestTLOpenHandleNULL);
GENTLTEST_UNIT_DECL(SystemTest, TestTLCloseWithoutGCInitLib);
GENTLTEST_UNIT_DECL(SystemTest, TestTLCloseWithoutTLOpen);
GENTLTEST_UNIT_DECL(SystemTest, TestTLCloseTwice);

GENTLTEST_UNIT_DECL(System_TLUpdateInterface, TestTLUpdateInterfaceList);
GENTLTEST_UNIT_DECL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithoutGCInitLib);
GENTLTEST_UNIT_DECL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithoutTLOpen);
GENTLTEST_UNIT_DECL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithHandleNull);
GENTLTEST_UNIT_DECL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithChangedNull);

GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfo);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithTLHandleNULL);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithInvalidCommand);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithTypeNull);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithSizeNull);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithLowSize);
GENTLTEST_UNIT_DECL(System_TLGetInfo, TestTLGetInfoWithBufferSizeNULL);

GENTLTEST_UNIT_DECL(System_TLGetNumInterfaces, TestTLGetNumInterfaces);
GENTLTEST_UNIT_DECL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutGCInitLib);
GENTLTEST_UNIT_DECL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutTLOpen);
GENTLTEST_UNIT_DECL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithHandleNull);
GENTLTEST_UNIT_DECL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithNumbersNull);
GENTLTEST_UNIT_DECL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutUpdate);

GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceID);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithoutGCInitLib);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithoutTLOpen);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithTLHandleNULL);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithSizeNULL1);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithSizeNULL2);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithInvalidIndex);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceID, TestTLGetInterfaceIDPersistence);

GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterface);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceWithInterfaceID);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceWithoutGCInitLib);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceWithoutTLOpen);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceWithIFNULL);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceWithIFIDNULL);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceWithWrongIFID);
GENTLTEST_UNIT_DECL(System_TLOpenInterface, TestTLOpenInterfaceDoubleOpen);

GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfo);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithSizeNull);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithTypeNull);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithIFIDNull);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoBufferWithSizeNull);



#endif  //SYSTEM_H
