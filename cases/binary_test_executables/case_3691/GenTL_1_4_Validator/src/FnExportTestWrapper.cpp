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

#include "FnExportTestWrapper.h"
#include "FnExportTest.h"
#include "GenTLTestTools.h"

FnExportTestWrapper::FnExportTestWrapper(void)
: m_poFnExportTest(NULL)
{
    m_poFnExportTest = FnExportTest::poGetInstance();
    setUp();
}

FnExportTestWrapper::~FnExportTestWrapper(void)
{
    tearDown();

    m_poFnExportTest = NULL;
    FnExportTest::vExitInstance();
}

void FnExportTestWrapper :: setUp (void)
{
    m_poFnExportTest->setUp();
}

void FnExportTestWrapper :: tearDown (void)
{
    m_poFnExportTest->tearDown();
}

void FnExportTestWrapper::TestGCGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCGetInfo'");
    void *pFunc = m_poFnExportTest->pGCGetInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCGetInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCGetLastError( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCGetLastError'");
    void *pFunc = m_poFnExportTest->pGCGetLastError();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCGetLastError\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCInitLib'");
    void *pFunc = m_poFnExportTest->pGCInitLib();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCInitLib\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCCloseLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCCloseLib'");
    void *pFunc = m_poFnExportTest->pGCCloseLib();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCCloseLib\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCReadPort( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCReadPort'");
    void *pFunc = m_poFnExportTest->pGCReadPort();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCReadPort\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCWritePort( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCWritePort'");
    void *pFunc = m_poFnExportTest->pGCWritePort();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCWritePort\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCGetPortURL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCGetPortURL'");
    void *pFunc = m_poFnExportTest->pGCGetPortURL();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCGetPortURL\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCGetNumPortURLs( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCGetNumPortURLs'");
    void *pFunc = m_poFnExportTest->pGCGetNumPortURLs();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCGetNumPortURLs\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCGetPortInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCGetPortInfo'");
    void *pFunc = m_poFnExportTest->pGCGetPortInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCGetPortInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCGetPortURLInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCGetPortURLInfo'");
    void *pFunc = m_poFnExportTest->pGCGetPortURLInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCGetPortURLInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCReadPortStacked( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCReadPortStacked'");
    void *pFunc = m_poFnExportTest->pGCReadPortStacked();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCReadPortStacked\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCWritePortStacked( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCWritePortStacked'");
    void *pFunc = m_poFnExportTest->pGCWritePortStacked();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCWritePortStacked\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCRegisterEvent( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCRegisterEvent'");
    void *pFunc = m_poFnExportTest->pGCRegisterEvent();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCRegisterEvent\"); failed", 
        pFunc != NULL);

    {                                                                                       
        unsigned long oAssertFailed=0;
        bool oAborted=false;

        oAssertFailed = GENTLUNITTEST_GET_ASSERT_FAILED(test_id);
        oAborted = GENTLUNITTEST_GET_ABORTED(test_id);
        if (test_id == -1)                                                                      
        {                                                                                   
            GENTLTEST_PRINT("Test Unit ID not found");                                      
        }                                                                                   
        if (oAborted == 0 && oAssertFailed == 0)                                            
        {                                                                                   
            GENTLTEST_PRINT("OK");                                                          
        }                                                                                   
    }
    //GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestGCUnregisterEvent( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'GCUnregisterEvent'");
    void *pFunc = m_poFnExportTest->pGCUnregisterEvent();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"GCUnregisterEvent\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestEventGetData( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'EventGetData'");
    void *pFunc = m_poFnExportTest->pEventGetData();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"EventGetData\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestEventGetDataInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'EventGetDataInfo'");
    void *pFunc = m_poFnExportTest->pEventGetDataInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"EventGetDataInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestEventGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'EventGetInfo'");
    void *pFunc = m_poFnExportTest->pEventGetInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"EventGetInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestEventFlush( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'EventFlush'");
    void *pFunc = m_poFnExportTest->pEventFlush();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"EventFlush\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestEventKill( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'EventKill'");
    void *pFunc = m_poFnExportTest->pEventKill();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"EventKill\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLOpen'");
    void *pFunc = m_poFnExportTest->pTLOpen();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLOpen\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLClose( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLClose'");
    void *pFunc = m_poFnExportTest->pTLClose();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLClose\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLGetInfo'");
    void *pFunc = m_poFnExportTest->pTLGetInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLGetInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLGetNumInterfaces( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLGetNumInterfaces'");
    void *pFunc = m_poFnExportTest->pTLGetNumInterfaces();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLGetNumInterfaces\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLGetInterfaceID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLGetInterfaceID'");
    void *pFunc = m_poFnExportTest->pTLGetInterfaceID();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLGetInterfaceID\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLGetInterfaceInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLGetInterfaceInfo'");
    void *pFunc = m_poFnExportTest->pTLGetInterfaceInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLGetInterfaceInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLOpenInterface( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLOpenInterface'");
    void *pFunc = m_poFnExportTest->pTLOpenInterface();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLOpenInterface\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestTLUpdateInterfaceList( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'TLUpdateInterfaceList'");
    void *pFunc = m_poFnExportTest->pTLUpdateInterfaceList();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"TLUpdateInterfaceList\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFClose( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFClose'");
    void *pFunc = m_poFnExportTest->pIFClose();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFClose\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFGetInfo'");
    void *pFunc = m_poFnExportTest->pIFGetInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFGetInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFGetNumDevices( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFGetNumDevices'");
    void *pFunc = m_poFnExportTest->pIFGetNumDevices();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFGetNumDevices\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFGetDeviceID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFGetDeviceID'");
    void *pFunc = m_poFnExportTest->pIFGetDeviceID();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFGetDeviceID\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFUpdateDeviceList( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFUpdateDeviceList'");
    void *pFunc = m_poFnExportTest->pIFUpdateDeviceList();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFUpdateDeviceList\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFGetDeviceInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFGetDeviceInfo'");
    void *pFunc = m_poFnExportTest->pIFGetDeviceInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFGetDeviceInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFOpenDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFOpenDevice'");
    void *pFunc = m_poFnExportTest->pIFOpenDevice();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFOpenDevice\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestIFGetParentTL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'IFGetParentTL'");
    void *pFunc = m_poFnExportTest->pIFGetParentTL();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"IFGetParentTL\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevGetPort( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevGetPort'");
    void *pFunc = m_poFnExportTest->pDevGetPort();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevGetPort\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevGetNumDataStreams( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevGetNumDataStreams'");
    void *pFunc = m_poFnExportTest->pDevGetNumDataStreams();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevGetNumDataStreams\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevGetDataStreamID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevGetDataStreamID'");
    void *pFunc = m_poFnExportTest->pDevGetDataStreamID();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevGetDataStreamID\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevOpenDataStream( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevOpenDataStream'");
    void *pFunc = m_poFnExportTest->pDevOpenDataStream();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevOpenDataStream\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevGetInfo'");
    void *pFunc = m_poFnExportTest->pDevGetInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevGetInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevClose( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevClose'");
    void *pFunc = m_poFnExportTest->pDevClose();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevClose\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDevGetParentIF( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DevGetParentIF'");
    void *pFunc = m_poFnExportTest->pDevGetParentIF();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DevGetParentIF\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSAnnounceBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSAnnounceBuffer'");
    void *pFunc = m_poFnExportTest->pDSAnnounceBuffer();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSAnnounceBuffer\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSAllocAndAnnounceBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSAllocAndAnnounceBuffer'");
    void *pFunc = m_poFnExportTest->pDSAllocAndAnnounceBuffer();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSAllocAndAnnounceBuffer\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSFlushQueue( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSFlushQueue'");
    void *pFunc = m_poFnExportTest->pDSFlushQueue();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSFlushQueue\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSStartAcquisition( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSStartAcquisition'");
    void *pFunc = m_poFnExportTest->pDSStartAcquisition();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSStartAcquisition\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSStopAcquisition( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSStopAcquisition'");
    void *pFunc = m_poFnExportTest->pDSStopAcquisition();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSStopAcquisition\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSGetInfo'");
    void *pFunc = m_poFnExportTest->pDSGetInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSGetInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSGetBufferID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSGetBufferID'");
    void *pFunc = m_poFnExportTest->pDSGetBufferID();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSGetBufferID\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSClose( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSClose'");
    void *pFunc = m_poFnExportTest->pDSClose();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSClose\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSRevokeBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSRevokeBuffer'");
    void *pFunc = m_poFnExportTest->pDSRevokeBuffer();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSRevokeBuffer\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSQueueBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSQueueBuffer'");
    void *pFunc = m_poFnExportTest->pDSQueueBuffer();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSQueueBuffer\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSGetBufferInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSGetBufferInfo'");
    void *pFunc = m_poFnExportTest->pDSGetBufferInfo();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSGetBufferInfo\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSGetBufferChunkData( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSGetBufferChunkData'");
    void *pFunc = m_poFnExportTest->pDSGetBufferChunkData();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSGetBufferChunkData\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}

void FnExportTestWrapper::TestDSGetParentDev( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION_NOTE(" 'DSGetParentDev'");
    void *pFunc = m_poFnExportTest->pDSGetParentDev();
    
    GENTLTEST_CHECK_MESSAGE("GetProcAddress(m_hModule=0x" << std::hex << m_poFnExportTest->hGetModuleHandle() << std::dec << ", \"DSGetParentDev\"); failed", 
        pFunc != NULL);

    GENTLTEST_PRINT_RESULT_NOTE(test_id);
}
