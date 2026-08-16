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

#include <string>

#include "GenApi/GenApi.h"

#include "System.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

GENTLTEST_UNIT_IMPL_MSG(SystemTest, TestTLOpen, "SystemTest");
GENTLTEST_UNIT_IMPL(SystemTest, TestTLOpenWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(SystemTest, TestTLOpenTwice);
GENTLTEST_UNIT_IMPL(SystemTest, TestTLOpenHandleNULL);
GENTLTEST_UNIT_IMPL(SystemTest, TestTLCloseWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(SystemTest, TestTLCloseWithoutTLOpen);
GENTLTEST_UNIT_IMPL(SystemTest, TestTLCloseTwice);

GENTLTEST_UNIT_IMPL_MSG(System_TLUpdateInterface, TestTLUpdateInterfaceList, "System_TLUpdateInterface");
GENTLTEST_UNIT_IMPL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithoutTLOpen);
GENTLTEST_UNIT_IMPL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithHandleNull);
GENTLTEST_UNIT_IMPL(System_TLUpdateInterface, TestTLUpdateInterfaceListWithChangedNull);

GENTLTEST_UNIT_IMPL_MSG(System_TLGetInfo, TestTLGetInfo, "System_TLGetInfo");
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithTLHandleNULL);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithInvalidCommand);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithTypeNull);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithSizeNull);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithLowSize);
GENTLTEST_UNIT_IMPL(System_TLGetInfo, TestTLGetInfoWithBufferSizeNULL);

GENTLTEST_UNIT_IMPL_MSG(System_TLGetNumInterfaces, TestTLGetNumInterfaces, "System_TLGetNumInterfaces");
GENTLTEST_UNIT_IMPL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutTLOpen);
GENTLTEST_UNIT_IMPL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithHandleNull);
GENTLTEST_UNIT_IMPL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithNumbersNull);
GENTLTEST_UNIT_IMPL(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutUpdate);

GENTLTEST_UNIT_IMPL_MSG(System_TLGetInterfaceID, TestTLGetInterfaceID, "System_TLGetInterfaceID");
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithoutTLOpen);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithTLHandleNULL);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithSizeNULL1);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithSizeNULL2);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDWithInvalidIndex);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceID, TestTLGetInterfaceIDPersistence);

GENTLTEST_UNIT_IMPL_MSG(System_TLOpenInterface, TestTLOpenInterface, "System_TLOpenInterface");
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceWithInterfaceID);
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceWithoutTLOpen);
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceWithIFNULL);
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceWithIFIDNULL);
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceWithWrongIFID);
GENTLTEST_UNIT_IMPL(System_TLOpenInterface, TestTLOpenInterfaceDoubleOpen);

GENTLTEST_UNIT_IMPL_MSG(System_TLGetInterfaceInfo, TestTLGetInterfaceInfo, "System_TLGetInterfaceInfo");
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithSizeNull);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithTypeNull);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithIFIDNull);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoBufferWithoutSize);
GENTLTEST_UNIT_IMPL(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoBufferWithSizeNull);
