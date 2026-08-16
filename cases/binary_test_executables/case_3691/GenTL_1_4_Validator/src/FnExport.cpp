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

#include "Base/GCTypes.h"
#include "GenTL_v1_4.h"

#include "FnExport.h"
#include "FnExportTestWrapper.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

void vFnExportTest()
{
    GENTLTEST_PRINT(std::endl << "**********************************************************************" << std::endl);
    GENTLTEST_PRINT("==> Running vTestFnExportTestWrapper:" << std::endl);

    uint32_t id = GENTLTEST_GET_TEST_UNIT_ID("vFnExportTest");

    GENTLTEST_DESCRIPTION(id, "Test if GenTL functions version " <<  GenTLMajorVersion << "." << GenTLMinorVersion << " exported.");

    FnExportTestWrapper oWrapper;

    vFnExportTestGCGetInfo( oWrapper, id );
    vFnExportTestGCGetLastError( oWrapper, id );
    vFnExportTestGCInitLib( oWrapper, id );
    vFnExportTestGCCloseLib( oWrapper, id );
    vFnExportTestGCReadPort( oWrapper, id );
    vFnExportTestGCWritePort( oWrapper, id );
    vFnExportTestGCGetPortURL( oWrapper, id );
    vFnExportTestGCGetNumPortURLs( oWrapper, id );
    vFnExportTestGCGetPortInfo( oWrapper, id );
    vFnExportTestGCGetPortURLInfo( oWrapper, id );
    vFnExportTestGCReadPortStacked( oWrapper, id );
    vFnExportTestGCWritePortStacked( oWrapper, id );
    vFnExportTestGCRegisterEvent( oWrapper, id );
    vFnExportTestGCUnregisterEvent( oWrapper, id );
    vFnExportTestEventGetData( oWrapper, id );
    vFnExportTestEventGetDataInfo( oWrapper, id );
    vFnExportTestEventGetInfo( oWrapper, id );
    vFnExportTestEventFlush( oWrapper, id );
    vFnExportTestEventKill( oWrapper, id );
    vFnExportTestTLOpen( oWrapper, id );
    vFnExportTestTLClose( oWrapper, id );
    vFnExportTestTLGetInfo( oWrapper, id );
    vFnExportTestTLGetNumInterfaces( oWrapper, id );
    vFnExportTestTLGetInterfaceID( oWrapper, id );
    vFnExportTestTLGetInterfaceInfo( oWrapper, id );
    vFnExportTestTLOpenInterface( oWrapper, id );
    vFnExportTestTLUpdateInterfaceList( oWrapper, id );
    vFnExportTestIFClose( oWrapper, id );
    vFnExportTestIFGetInfo( oWrapper, id );
    vFnExportTestIFGetNumDevices( oWrapper, id );
    vFnExportTestIFGetDeviceID( oWrapper, id );
    vFnExportTestIFUpdateDeviceList( oWrapper, id );
    vFnExportTestIFGetDeviceInfo( oWrapper, id );
    vFnExportTestIFOpenDevice( oWrapper, id );
    vFnExportTestIFGetParentTL( oWrapper, id );
    vFnExportTestDevGetPort( oWrapper, id );
    vFnExportTestDevGetNumDataStreams( oWrapper, id );
    vFnExportTestDevGetDataStreamID( oWrapper, id );
    vFnExportTestDevOpenDataStream( oWrapper, id );
    vFnExportTestDevGetInfo( oWrapper, id );
    vFnExportTestDevClose( oWrapper, id );
    vFnExportTestDevGetParentIF( oWrapper, id );
    vFnExportTestDSAnnounceBuffer( oWrapper, id );
    vFnExportTestDSAllocAndAnnounceBuffer( oWrapper, id );
    vFnExportTestDSFlushQueue( oWrapper, id );
    vFnExportTestDSStartAcquisition( oWrapper, id );
    vFnExportTestDSStopAcquisition( oWrapper, id );
    vFnExportTestDSGetInfo( oWrapper, id );
    vFnExportTestDSGetBufferID( oWrapper, id );
    vFnExportTestDSClose( oWrapper, id );
    vFnExportTestDSRevokeBuffer( oWrapper, id );
    vFnExportTestDSQueueBuffer( oWrapper, id );
    vFnExportTestDSGetBufferInfo( oWrapper, id );
    vFnExportTestDSGetBufferChunkData( oWrapper, id );
    vFnExportTestDSGetParentDev( oWrapper, id );

    GENTLTEST_PRINT_RESULT(id);
}

void vFnExportTestGCGetInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCGetInfo(id);
}

void vFnExportTestGCGetLastError(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCGetLastError(id);
}

void vFnExportTestGCInitLib(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCInitLib(id);
}

void vFnExportTestGCCloseLib(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCCloseLib(id);
}

void vFnExportTestGCReadPort(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCReadPort(id);
}

void vFnExportTestGCWritePort(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCWritePort(id);
}

void vFnExportTestGCGetPortURL(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCGetPortURL(id);
}

void vFnExportTestGCGetNumPortURLs(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCGetNumPortURLs(id);
}

void vFnExportTestGCGetPortInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCGetPortInfo(id);
}

void vFnExportTestGCGetPortURLInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCGetPortURLInfo(id);

    GENTLTEST_PRINT(std::endl);
}

void vFnExportTestGCReadPortStacked(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCReadPortStacked(id);
}

void vFnExportTestGCWritePortStacked(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCWritePortStacked(id);
}

void vFnExportTestGCRegisterEvent(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCRegisterEvent(id);
}

void vFnExportTestGCUnregisterEvent(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestGCUnregisterEvent(id);
}

void vFnExportTestEventGetData(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestEventGetData(id);
}

void vFnExportTestEventGetDataInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestEventGetDataInfo(id);
}

void vFnExportTestEventGetInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestEventGetInfo(id);
}

void vFnExportTestEventFlush(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestEventFlush(id);
}

void vFnExportTestEventKill(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestEventKill(id);

    GENTLTEST_PRINT(std::endl);
}

void vFnExportTestTLOpen(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLOpen(id);
}

void vFnExportTestTLClose(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLClose(id);
}

void vFnExportTestTLGetInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLGetInfo(id);
}

void vFnExportTestTLGetNumInterfaces(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLGetNumInterfaces(id);
}

void vFnExportTestTLGetInterfaceID(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLGetInterfaceID(id);
}

void vFnExportTestTLGetInterfaceInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLGetInterfaceInfo(id);
}

void vFnExportTestTLOpenInterface(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLOpenInterface(id);
}

void vFnExportTestTLUpdateInterfaceList(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestTLUpdateInterfaceList(id);
}

void vFnExportTestIFClose(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFClose(id);

    GENTLTEST_PRINT(std::endl);
}

void vFnExportTestIFGetInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFGetInfo(id);
}

void vFnExportTestIFGetNumDevices(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFGetNumDevices(id);
}

void vFnExportTestIFGetDeviceID(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFGetDeviceID(id);
}

void vFnExportTestIFUpdateDeviceList(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFUpdateDeviceList(id);
}

void vFnExportTestIFGetDeviceInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFGetDeviceInfo(id);
}

void vFnExportTestIFOpenDevice(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFOpenDevice(id);
}

void vFnExportTestIFGetParentTL(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestIFGetParentTL(id);
}

void vFnExportTestDevGetPort(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevGetPort(id);
}

void vFnExportTestDevGetNumDataStreams(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevGetNumDataStreams(id);

    GENTLTEST_PRINT(std::endl);
}

void vFnExportTestDevGetDataStreamID(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevGetDataStreamID(id);
}

void vFnExportTestDevOpenDataStream(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevOpenDataStream(id);
}

void vFnExportTestDevGetInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevGetInfo(id);
}

void vFnExportTestDevClose(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevClose(id);
}

void vFnExportTestDevGetParentIF(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDevGetParentIF(id);
}

void vFnExportTestDSAnnounceBuffer(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSAnnounceBuffer(id);
}

void vFnExportTestDSAllocAndAnnounceBuffer(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSAllocAndAnnounceBuffer(id);
}

void vFnExportTestDSFlushQueue(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSFlushQueue(id);

    GENTLTEST_PRINT(std::endl);
}

void vFnExportTestDSStartAcquisition(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSStartAcquisition(id);
}

void vFnExportTestDSStopAcquisition(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSStopAcquisition(id);
}

void vFnExportTestDSGetInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSGetInfo(id);
}

void vFnExportTestDSGetBufferID(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSGetBufferID(id);
}

void vFnExportTestDSClose(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSClose(id);
}

void vFnExportTestDSRevokeBuffer(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSRevokeBuffer(id);
}

void vFnExportTestDSQueueBuffer(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSQueueBuffer(id);
}

void vFnExportTestDSGetBufferInfo(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSGetBufferInfo(id);
}

void vFnExportTestDSGetBufferChunkData(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSGetBufferChunkData(id);

    GENTLTEST_PRINT(std::endl);
}

void vFnExportTestDSGetParentDev(FnExportTestWrapper &oWrapper, uint32_t id)
{
    oWrapper.TestDSGetParentDev(id);

    GENTLTEST_PRINT(std::endl);
}