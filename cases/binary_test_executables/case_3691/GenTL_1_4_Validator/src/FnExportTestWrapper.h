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

#ifndef FNEXPORTTESTWRAPPER_INCLUDE___
#define FNEXPORTTESTWRAPPER_INCLUDE___

#include "GenTL_v1_4.h"

class FnExportTest;

class FnExportTestWrapper
{
public:
    FnExportTestWrapper(void);
    virtual ~FnExportTestWrapper(void);

    void TestGCGetInfo( uint32_t test_id );
    void TestGCGetLastError( uint32_t test_id );
    void TestGCInitLib( uint32_t test_id );
    void TestGCCloseLib( uint32_t test_id );
    void TestGCReadPort( uint32_t test_id );
    void TestGCWritePort( uint32_t test_id );
    void TestGCGetPortURL( uint32_t test_id );
    void TestGCGetNumPortURLs( uint32_t test_id );
    void TestGCGetPortInfo( uint32_t test_id );
    void TestGCGetPortURLInfo( uint32_t test_id );
    void TestGCReadPortStacked( uint32_t test_id );
    void TestGCWritePortStacked( uint32_t test_id );
    void TestGCRegisterEvent( uint32_t test_id );
    void TestGCUnregisterEvent( uint32_t test_id );
    void TestEventGetData( uint32_t test_id );
    void TestEventGetDataInfo( uint32_t test_id );
    void TestEventGetInfo( uint32_t test_id );
    void TestEventFlush( uint32_t test_id );
    void TestEventKill( uint32_t test_id );
    void TestTLOpen( uint32_t test_id );
    void TestTLClose( uint32_t test_id );
    void TestTLGetInfo( uint32_t test_id );
    void TestTLGetNumInterfaces( uint32_t test_id );
    void TestTLGetInterfaceID( uint32_t test_id );
    void TestTLGetInterfaceInfo( uint32_t test_id );
    void TestTLOpenInterface( uint32_t test_id );
    void TestTLUpdateInterfaceList( uint32_t test_id );
    void TestIFClose( uint32_t test_id );
    void TestIFGetInfo( uint32_t test_id );
    void TestIFGetNumDevices( uint32_t test_id );
    void TestIFGetDeviceID( uint32_t test_id );
    void TestIFUpdateDeviceList( uint32_t test_id );
    void TestIFGetDeviceInfo( uint32_t test_id );
    void TestIFOpenDevice( uint32_t test_id );
    void TestIFGetParentTL( uint32_t test_id );
    void TestDevGetPort( uint32_t test_id );
    void TestDevGetNumDataStreams( uint32_t test_id );
    void TestDevGetDataStreamID( uint32_t test_id );
    void TestDevOpenDataStream( uint32_t test_id );
    void TestDevGetInfo( uint32_t test_id );
    void TestDevClose( uint32_t test_id );
    void TestDevGetParentIF( uint32_t test_id );
    void TestDSAnnounceBuffer( uint32_t test_id );
    void TestDSAllocAndAnnounceBuffer( uint32_t test_id );
    void TestDSFlushQueue( uint32_t test_id );
    void TestDSStartAcquisition( uint32_t test_id );
    void TestDSStopAcquisition( uint32_t test_id );
    void TestDSGetInfo( uint32_t test_id );
    void TestDSGetBufferID( uint32_t test_id );
    void TestDSClose( uint32_t test_id );
    void TestDSRevokeBuffer( uint32_t test_id );
    void TestDSQueueBuffer( uint32_t test_id );
    void TestDSGetBufferInfo( uint32_t test_id );
    void TestDSGetBufferChunkData( uint32_t test_id );
    void TestDSGetParentDev( uint32_t test_id );

protected:
    void setUp (void);
    void tearDown (void);

private:
    FnExportTest *m_poFnExportTest;
};

#endif  /* FNEXPORTTESTWRAPPER_INCLUDE___ */