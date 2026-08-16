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

#include "GenTLTestCases.h"

#include "Library.h"
#include "System.h"
#include "Interface.h"
#include "Device.h"
#include "Port.h"
#include "DataStream.h"
#include "Signaling.h"
#include "Impl.h"
#include "FnExport.h"

#include "GenTLTestSuite.h"

uint32_t g_zCurrentTestCase=0;

//////////////////////////////////////////////////////
// tests
//////////////////////////////////////////////////////

void vTestFnExportTestWrapper(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    g_zCurrentTestCase++;
    if (zSpecialTestCaseToDoFrom == ALL_TEST_CASES ||   
        (g_zCurrentTestCase >= zSpecialTestCaseToDoFrom && g_zCurrentTestCase <= zSpecialTestCaseToDoTo))
    {
        GENTLUNITTEST_ADD_TEST_UNIT(g_zCurrentTestCase, vFnExportTest);
    }
}
        
void vTestLibrary_GetTLInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Library_GetTLInfo, vDisplayTLInfo);
}

void vTestLibraryTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCInitLibGCCloseLib);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestDoubleGCInitLibGCCloseLib);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCInitLibDoubleGCCloseLib);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCCloseLibWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastError);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorBeforeGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorERR_SUCCESS);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorErrorCodeNULL);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorErrorSizeNULL);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorWithBufferErrorCodeNULL);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorWithBufferErrorSizeNULL);
    GENTLTEST_ADD_TEST_UNIT(LibraryTest, TestGCGetLastErrorWithBufferErrorLessSize);
}
 
void vTestLibrary_GCGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfo);
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfo1Size0);
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfo2Size0);
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfoLowSize);
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfoTypeNULL);
    GENTLTEST_ADD_TEST_UNIT(Library_GCGetInfo, TestGCGetInfoWithInvalidCommand);
}

void vTestSystemTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLOpen);
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLOpenWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLOpenTwice);
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLOpenHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLCloseWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLCloseWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(SystemTest, TestTLCloseTwice);
}

void vTestSystem_TLUpdateInterface(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(System_TLUpdateInterface, TestTLUpdateInterfaceList);
    GENTLTEST_ADD_TEST_UNIT(System_TLUpdateInterface, TestTLUpdateInterfaceListWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(System_TLUpdateInterface, TestTLUpdateInterfaceListWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(System_TLUpdateInterface, TestTLUpdateInterfaceListWithHandleNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLUpdateInterface, TestTLUpdateInterfaceListWithChangedNull);
}

void vTestSystem_TLGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfo);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithTLHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithInvalidCommand);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithLowSize);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInfo, TestTLGetInfoWithBufferSizeNULL);
}

void vTestSystem_TLGetNumInterfaces(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(System_TLGetNumInterfaces, TestTLGetNumInterfaces);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithHandleNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithNumbersNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetNumInterfaces, TestTLGetNumInterfacesWithoutUpdate);
}

void vTestSystem_TLGetInterfaceID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceID);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDWithTLHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDWithSizeNULL1);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDWithSizeNULL2);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDWithInvalidIndex);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceID, TestTLGetInterfaceIDPersistence);
}

void vTestSystem_TLOpenInterface(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterface);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceWithInterfaceID);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceWithIFNULL);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceWithIFIDNULL);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceWithWrongIFID);
    GENTLTEST_ADD_TEST_UNIT(System_TLOpenInterface, TestTLOpenInterfaceDoubleOpen);
}

void vTestSystem_TLGetInterfaceInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfo);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoWithIFIDNull);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(System_TLGetInterfaceInfo, TestTLGetInterfaceInfoBufferWithSizeNull);
}


void vTestInterfaceTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFClose);
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFCloseWithLibraryClosed);
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFCloseWithSystemClosed);
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFCloseDouble);
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFCloseReopen);
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFCloseWithHandleIFNull);
    GENTLTEST_ADD_TEST_UNIT(InterfaceTest, TestIFCloseWithHandleIFWrong);
}

void vTestInterface_IFGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfo);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithInvalidCommand);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetInfo, TestIFGetInfoWithTypeNULL);
}

void vTestInterface_IFUpdateDeviceList(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFUpdateDeviceList, TestIFUpdateDeviceList);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithChangedNull);
}

void vTestInterface_IFGetNumDevices(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevices);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevicesWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevicesWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevicesWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevicesWithHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevicesWithNumbersNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetNumDevices, TestIFGetNumDevicesWithoutUpdate);
}

void vTestInterface_IFGetDeviceID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceID);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithIfHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithSizeNULL1);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithSizeNULL2);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithInvalidIndex);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDWithSizeLow);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceID, TestIFGetDeviceIDPersistence);
}

void vTestInterface_IFOpenDevice(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDevice);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithPublicDeviceID);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithIFNULL);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithDevIDNULL);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithWrongDevID);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceDoubleOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceControlOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceExclusiveOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithDevHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFOpenDevice, TestIFOpenDeviceWithInvalidAccessMode);
}

void vTestInterface_IFGetDeviceInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfo);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithIFNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithDevIDNull);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithWrongDevID);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoBufferWithSizeNull);
}


void vTestInterface_IFGetParentTL(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetParentTL, TestIFGetParentTL);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetParentTL, TestIFGetParentTLWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetParentTL, TestIFGetParentTLWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetParentTL, TestIFGetParentTLWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetParentTL, TestIFGetParentTLWithIFNULL);
    GENTLTEST_ADD_TEST_UNIT(Interface_IFGetParentTL, TestIFGetParentTLWithSystemNULL);
}


void vTestDeviceTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevClose);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseWithLibraryClosed);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseWithoutInterfaceOpen);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseDouble);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseReopen);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseWithHandleDevNull);
    GENTLTEST_ADD_TEST_UNIT(DeviceTest, TestDevCloseWithHandleDevWrong);
}

void vTestDevice_DevGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfo);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoWithDevIDNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoBufferWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetInfo, TestDevGetInfoBufferWithSizeLow);
}

void vTestDevice_DevGetNumDataStreams(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreams);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithNumNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithDevHandleNull);
}

void vTestDevice_DevGetDataStreamID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamID);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithDevHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithDevGreaterIndex);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDBufferWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetDataStreamID, TestDevGetDataStreamIDPersistence);
}

void vTestDevice_DevGetPort(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPort);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPortWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPortWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPortWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPortWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPortWithDevIDNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetPort, TestDevGetPortWithPortHandleNull);
}

void vTestDevice_DevOpenDataStream(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStream);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithDevHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithDSHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamWithDataStreamIDNull);
    GENTLTEST_ADD_TEST_UNIT(Device_DevOpenDataStream, TestDevOpenDataStreamDoubleOpen);
}

void vTestDevice_DevGetParentIF(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIF);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIFWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIFWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIFWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIFWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIFWithDevNULL);
    GENTLTEST_ADD_TEST_UNIT(Device_DevGetParentIF, TestDevGetParentIFWithIFNULL);
}

void vTestPort_GCGetPortInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoSystem);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoInterface);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoDevice);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoRemoteDevice);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoDataStream);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoBuffer);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithPortHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithInvalidCommand);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithSizeLow);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortInfo, TestGCGetPortInfoWithBufferSizeNULL);
}

void vTestPort_GCGetPortURL(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURL);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithPortHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURL, TestGCGetPortURLWithSizeLow);
}

void vTestPort_GCGetNumPortURLs(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLs);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLsPortHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithNumURLsNull);
}

void vTestPort_GCGetPortURLInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfo);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoPortHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoBufferWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoBufferWithSizeLow);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithInvalidCommand);
    GENTLTEST_ADD_TEST_UNIT(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithInvalidIndex);
}

void vTestPort_GCReadPort(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortSystem);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortSystemEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortSystemMandatoryEntries);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortInterface);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortInterfaceEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortInterfaceMandatoryEntries);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortDevice);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortDeviceEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortDeviceMandatoryEntries);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortRemoteDevice);
    // SVW not sure to test in the moment
    // Tests accessability of all standard entries,
    // is it possible to get access of all standard entries even they are not mandatory ?
    //GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortRemoteDeviceEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortRemoteDeviceMandatoryEntries);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortDatastream);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortDatastreamEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortDatastreamMandatoryEntries);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortBufferMandatoryEntries);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithSizeLow);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPort, TestGCReadPortWithBufferNull);
}

void vTestPort_GCWritePort(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCWritePort, TestGCWritePort);
    GENTLTEST_ADD_TEST_UNIT(Port_GCWritePort, TestGCWritePortWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCWritePort, TestGCWritePortWithoutTLOpen);
}

void vTestPort_GCReadPortstacked(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedSystemEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedInterfaceEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedDeviceEntry);
    // SVW not sure to test in the moment
    // Tests accessability of all standard entries,
    // is it possible to get access of all standard entries even they are not mandatory ?
    //GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedRemoteDeviceEntry);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedWithNumentriesNull);
    GENTLTEST_ADD_TEST_UNIT(Port_GCReadPortStacked, TestGCReadPortStackedWithEntriesNull);
}

void vTestPort_GCWritePortstacked(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Port_GCWritePortStacked, TestGCWritePortStacked);
    GENTLTEST_ADD_TEST_UNIT(Port_GCWritePortStacked, TestGCWritePortStackedWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Port_GCWritePortStacked, TestGCWritePortStackedWithoutTLOpen);
}

void vTestDataStreamTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStream);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamWithHandleNull);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamDoubleClose);
    GENTLTEST_ADD_TEST_UNIT(DataStreamTest, TestDataStreamReopen);
}

void vTestDataStream_DSGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfo);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoBufferWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetInfo, TestDSGetInfoWithInvalidCommand);
}

void vTestDataStream_DSAnnounceBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBuffer2);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithPayloadSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithPayloadNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferPrivatedataNULL);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithBufferHandleNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferTwice);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferTwiceToDifferentStreams);
}

void vTestDataStream_DSQueueBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSQueueBuffer, TestDSQueueBufferBufferHandleNULL);
}

void vTestDataStream_DSGetBufferInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoConsumerBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoProducerBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithTypeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoBufferWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithInvalidCommand);
}

void vTestDataStream_DSGetBufferID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferID);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithBufferHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferID, TestDSGetBufferIDWithInvalidIndex);
}

void vTestDataStream_DSStartAcquisition(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisition);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithNumToAcquireNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithWrongStartFlag);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWith10Frames);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithSmallBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithNoBuffer);
}

void vTestDataStream_DSStopAcquisition(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisition);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithWrongStopFlag);
}

void vTestDataStream_DSAllocAndAnnounceBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBuffer2);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithPayloadSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferPrivatedataNULL);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithBufferHandleNull);
    //GENTLTEST_ADD_TEST_UNIT(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferMixedUsage);
}

void vTestDataStream_DSFlushQueue(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueue);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSFlushQueue, TestDSFlushQueueWithWrongOperationFlag);
}

void vTestDataStream_DSRevokeBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBuffer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBuffer2);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferQueued);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithPrivatedata);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithBufferPointer);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithBufferHandleNull);
}

void vTestDataStream_DSGetBufferChunkData(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithDSIDNull);
}

void vTestDataStream_DSGetParentDev(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDev);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithOldHandle);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithDSNULL);
    GENTLTEST_ADD_TEST_UNIT(DataStream_DSGetParentDev, TestDSGetParentDevWithDevNULL);
}

void vTestSignaling_GCRegisterEvent(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventSystem);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventInterface);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventDevice);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventDataStream);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithWrongEventID);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventWithEventHandleNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCRegisterEvent, TestGCRegisterEventTwice);
}

void vTestSignaling_GCUnregisterEvent(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventSystem);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventInterface);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventDevice);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventDataStream);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithDSIDNull);
    GENTLTEST_ADD_TEST_UNIT(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithWrongEventID);
}

void vTestSignaling_TLParamsLocked(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_TLParamsLocked, TestTLParamsLocked);
}

void vTestSignaling_StartDevice(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_StartDevice, TestStartDevice);
    //GENTLTEST_ADD_TEST_UNIT(Signaling_StartDevice, TestStartDeviceReadOnly);
}

void vTestSignaling_StopDevice(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_StopDevice, TestStopDevice);
}

void vTestSignaling_EventGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfo);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithoutRegisterEvent);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithEventHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithInvalidCommand);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithTypeNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoWithSizeNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetInfo, TestEventGetInfoBufferWithSizeNull);
}

void vTestSignaling_EventGetData(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataConsumerAlloc);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataProducerAlloc);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithoutRegisterEvent);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithEventHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithBufferNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithBufferSizeNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithBufferSizeLow);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetData, TestEventGetDataWithTimeout0);
}

void vTestSignaling_EventGetDataInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfo);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutRegisterEvent);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithEventHandleNull);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithInvalidCommand);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithTypeNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithSizeNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoBufferWithoutSize);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoBufferWithSizeNull);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferNULL);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferSize0);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferSizeLow);
}

void vTestSignaling_EventKill(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    //GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKill);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithoutRegisterEvent);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventKill, TestEventKillWithEventHandleNull);
}

void vTestSignaling_EventFlush(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlush);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithoutGCInitLib);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithoutTLOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithoutIFOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithoutDevOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithoutDSOpen);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithoutRegisterEvent);
    GENTLTEST_ADD_TEST_UNIT(Signaling_EventFlush, TestEventFlushWithEventHandleNull);
}

void vTestImpl_Complete(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestNormalAcquisitionConsumerBuffer);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestNormalAcquisitionConsumerBufferForAquiredFrames);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestNormalAcquisitionConsumerBufferDSGetInfo);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestNormalAcquisitionProducerBuffer);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestCommandNewDataProducerBuffer);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestGCGetPortInfoPortName);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test, TestBufferPoolLockDuringAcquisition);

    GENTLTEST_ADD_TEST_UNIT(Impl_Test2, TestAcquisitionConsumerBufferDSGetBufferChunkData);
    GENTLTEST_ADD_TEST_UNIT(Impl_Test2, TestAcquisitionProducerBufferDSGetBufferChunkData);
}

