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

#include "GenTLTest_Win.h"

#include <string>

#include "FnExportTest.h"
#include "GenTLTestTools.h"

FnExportTest *FnExportTest::s_poInstance=NULL;

extern std::string g_strTLPath;

//////////////////////////////////////////////////////////////////////////////////////
// instantiate singleton
//////////////////////////////////////////////////////////////////////////////////////

FnExportTest *FnExportTest::poGetInstance()
{
    if (NULL == s_poInstance)
    {
        s_poInstance = new FnExportTest();
    }

    return s_poInstance;
}

void FnExportTest::vExitInstance()
{
    if (NULL != s_poInstance)
    {
        delete s_poInstance;
        s_poInstance = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////////////
// class FnExportTest
//////////////////////////////////////////////////////////////////////////////////////

FnExportTest::FnExportTest()
: m_hModule (NULL)
, m_pGCGetInfo(NULL)
, m_pGCGetLastError(NULL)
, m_pGCInitLib(NULL)
, m_pGCCloseLib(NULL)
, m_pGCReadPort(NULL)
, m_pGCWritePort(NULL)
, m_pGCGetPortURL(NULL)
, m_pGCGetNumPortURLs(NULL)
, m_pGCGetPortInfo(NULL)
, m_pGCGetPortURLInfo(NULL)
, m_pGCReadPortStacked(NULL)
, m_pGCWritePortStacked(NULL)
, m_pGCRegisterEvent(NULL)
, m_pGCUnregisterEvent(NULL)
, m_pEventGetData(NULL)
, m_pEventGetDataInfo(NULL)
, m_pEventGetInfo(NULL)
, m_pEventFlush(NULL)
, m_pEventKill(NULL)
, m_pTLOpen(NULL)
, m_pTLClose(NULL)
, m_pTLGetInfo(NULL)
, m_pTLGetNumInterfaces(NULL)
, m_pTLGetInterfaceID(NULL)
, m_pTLGetInterfaceInfo(NULL)
, m_pTLOpenInterface(NULL)
, m_pTLUpdateInterfaceList(NULL)
, m_pIFClose(NULL)
, m_pIFGetInfo(NULL)
, m_pIFGetNumDevices(NULL)
, m_pIFGetDeviceID(NULL)
, m_pIFUpdateDeviceList(NULL)
, m_pIFGetDeviceInfo(NULL)
, m_pIFOpenDevice(NULL)
, m_pIFGetParentTL(NULL)
, m_pDevGetPort(NULL)
, m_pDevGetNumDataStreams(NULL)
, m_pDevGetDataStreamID(NULL)
, m_pDevOpenDataStream(NULL)
, m_pDevGetInfo(NULL)
, m_pDevClose(NULL)
, m_pDevGetParentIF(NULL)
, m_pDSAnnounceBuffer(NULL)
, m_pDSAllocAndAnnounceBuffer(NULL)
, m_pDSFlushQueue(NULL)
, m_pDSStartAcquisition(NULL)
, m_pDSStopAcquisition(NULL)
, m_pDSGetInfo(NULL)
, m_pDSGetBufferID(NULL)
, m_pDSClose(NULL)
, m_pDSRevokeBuffer(NULL)
, m_pDSQueueBuffer(NULL)
, m_pDSGetBufferInfo(NULL)
, m_pDSGetBufferChunkData(NULL)
, m_pDSGetParentDev(NULL)
{
    m_hModule = DLLLoad(g_strTLPath.c_str());

    if (m_hModule == NULL)
    {
        vDisplayError("LoadLibrary");
    }

    GENTLTEST_REQUIRE_MESSAGE("Could not load '" << g_strTLPath.c_str() << "', please check installation !!!", m_hModule != NULL);
}

FnExportTest::~FnExportTest()
{
    FreeLibrary(m_hModule);
}

void FnExportTest :: setUp (void)
{
  
}

void FnExportTest :: tearDown (void)
{
  
}

HMODULE FnExportTest::hGetModuleHandle()
{
    return m_hModule;
}

//////////////////////////////////////////////////////////
// Library
//////////////////////////////////////////////////////////

void *FnExportTest::pGCGetInfo()
{
    if (m_hModule != NULL && NULL == m_pGCGetInfo)
        m_pGCGetInfo = GetProcAddress(m_hModule, "GCGetInfo");
    return m_pGCGetInfo;
}

void *FnExportTest::pGCGetLastError()
{
    if (m_hModule != NULL && NULL == m_pGCGetLastError)
        m_pGCGetLastError = GetProcAddress(m_hModule, "GCGetLastError");
    return m_pGCGetLastError;
}

void *FnExportTest::pGCInitLib()
{
    if (m_hModule != NULL && NULL == m_pGCInitLib)
        m_pGCInitLib = GetProcAddress(m_hModule, "GCInitLib");

    GENTLTEST_REQUIRE_MESSAGE("Could not get 'GCInitLib' function pointer, please check '" << g_strTLPath.c_str() << "' !!!", m_pGCInitLib != NULL);

    return m_pGCInitLib;
}

void *FnExportTest::pGCCloseLib()
{
    if (m_hModule != NULL && NULL == m_pGCCloseLib)
        m_pGCCloseLib = GetProcAddress(m_hModule, "GCCloseLib");

    GENTLTEST_REQUIRE_MESSAGE("Could not get 'GCCloseLib' function pointer, please check '" << g_strTLPath.c_str() << "' !!!", m_pGCCloseLib != NULL);

    return m_pGCCloseLib;
}

//////////////////////////////////////////////////////////
// Port
//////////////////////////////////////////////////////////

void *FnExportTest::pGCReadPort()
{
    if (m_hModule != NULL && NULL == m_pGCReadPort)
        m_pGCReadPort = GetProcAddress(m_hModule, "GCReadPort");
    return m_pGCReadPort;
}

void *FnExportTest::pGCWritePort()
{
    if (m_hModule != NULL && NULL == m_pGCWritePort)
        m_pGCWritePort = GetProcAddress(m_hModule, "GCWritePort");
    return m_pGCWritePort;
}

void *FnExportTest::pGCGetPortURL()
{
    if (m_hModule != NULL && NULL == m_pGCGetPortURL)
        m_pGCGetPortURL = GetProcAddress(m_hModule, "GCGetPortURL");
    return m_pGCGetPortURL;
}

void *FnExportTest::pGCGetNumPortURLs()
{
    if (m_hModule != NULL && NULL == m_pGCGetNumPortURLs)
        m_pGCGetNumPortURLs = GetProcAddress(m_hModule, "GCGetNumPortURLs");
    return m_pGCGetNumPortURLs;
}

void *FnExportTest::pGCGetPortInfo()
{
    if (m_hModule != NULL && NULL == m_pGCGetPortInfo)
        m_pGCGetPortInfo = GetProcAddress(m_hModule, "GCGetPortInfo");
    return m_pGCGetPortInfo;
}

void *FnExportTest::pGCGetPortURLInfo()
{
    if (m_hModule != NULL && NULL == m_pGCGetPortURLInfo)
        m_pGCGetPortURLInfo = GetProcAddress(m_hModule, "GCGetPortURLInfo");
    return m_pGCGetPortURLInfo;
}

void *FnExportTest::pGCReadPortStacked()
{
    if (m_hModule != NULL && NULL == m_pGCReadPortStacked)
        m_pGCReadPortStacked = GetProcAddress(m_hModule, "GCReadPortStacked");
    return m_pGCReadPortStacked;
}

void *FnExportTest::pGCWritePortStacked()
{
    if (m_hModule != NULL && NULL == m_pGCWritePortStacked)
        m_pGCWritePortStacked = GetProcAddress(m_hModule, "GCWritePortStacked");
    return m_pGCWritePortStacked;
}

//////////////////////////////////////////////////////////
// Signaling
//////////////////////////////////////////////////////////

void *FnExportTest::pGCRegisterEvent()
{
    if (m_hModule != NULL && NULL == m_pGCRegisterEvent)
        m_pGCRegisterEvent = GetProcAddress(m_hModule, "GCRegisterEvent");
    return m_pGCRegisterEvent;
}

void *FnExportTest::pGCUnregisterEvent()
{
    if (m_hModule != NULL && NULL == m_pGCUnregisterEvent)
        m_pGCUnregisterEvent = GetProcAddress(m_hModule, "GCUnregisterEvent");
    return m_pGCUnregisterEvent;
}

void *FnExportTest::pEventGetData()
{
    if (m_hModule != NULL && NULL == m_pEventGetData)
        m_pEventGetData = GetProcAddress(m_hModule, "EventGetData");
    return m_pEventGetData;
}

void *FnExportTest::pEventGetDataInfo()
{
    if (m_hModule != NULL && NULL == m_pEventGetDataInfo)
        m_pEventGetDataInfo = GetProcAddress(m_hModule, "EventGetDataInfo");
    return m_pEventGetDataInfo;
}

void *FnExportTest::pEventGetInfo()
{
    if (m_hModule != NULL && NULL == m_pEventGetInfo)
        m_pEventGetInfo = GetProcAddress(m_hModule, "EventGetInfo");
    return m_pEventGetInfo;
}

void *FnExportTest::pEventFlush()
{
    if (m_hModule != NULL && NULL == m_pEventFlush)
        m_pEventFlush = GetProcAddress(m_hModule, "EventFlush");
    return m_pEventFlush;
}

void *FnExportTest::pEventKill()
{
    if (m_hModule != NULL && NULL == m_pEventKill)
        m_pEventKill = GetProcAddress(m_hModule, "EventKill");
    return m_pEventKill;
}

//////////////////////////////////////////////////////////
// System
//////////////////////////////////////////////////////////

void *FnExportTest::pTLOpen()
{
    if (m_hModule != NULL && NULL == m_pTLOpen)
        m_pTLOpen = GetProcAddress(m_hModule, "TLOpen");
    return m_pTLOpen;
}

void *FnExportTest::pTLClose()
{
    if (m_hModule != NULL && NULL == m_pTLClose)
        m_pTLClose = GetProcAddress(m_hModule, "TLClose");
    return m_pTLClose;
}

void *FnExportTest::pTLGetInfo()
{
    if (m_hModule != NULL && NULL == m_pTLGetInfo)
        m_pTLGetInfo = GetProcAddress(m_hModule, "TLGetInfo");
    return m_pTLGetInfo;
}

void *FnExportTest::pTLGetNumInterfaces()
{
    if (m_hModule != NULL && NULL == m_pTLGetNumInterfaces)
        m_pTLGetNumInterfaces = GetProcAddress(m_hModule, "TLGetNumInterfaces");
    return m_pTLGetNumInterfaces;
}

void *FnExportTest::pTLGetInterfaceID()
{
    if (m_hModule != NULL && NULL == m_pTLGetInterfaceID)
        m_pTLGetInterfaceID = GetProcAddress(m_hModule, "TLGetInterfaceID");
    return m_pTLGetInterfaceID;
}

void *FnExportTest::pTLGetInterfaceInfo()
{
    if (m_hModule != NULL && NULL == m_pTLGetInterfaceInfo)
        m_pTLGetInterfaceInfo = GetProcAddress(m_hModule, "TLGetInterfaceInfo");
    return m_pTLGetInterfaceInfo;
}

void *FnExportTest::pTLOpenInterface()
{
    if (m_hModule != NULL && NULL == m_pTLOpenInterface)
        m_pTLOpenInterface = GetProcAddress(m_hModule, "TLOpenInterface");
    return m_pTLOpenInterface;
}

void *FnExportTest::pTLUpdateInterfaceList()
{
    if (m_hModule != NULL && NULL == m_pTLUpdateInterfaceList)
        m_pTLUpdateInterfaceList = GetProcAddress(m_hModule, "TLUpdateInterfaceList");
    return m_pTLUpdateInterfaceList;
}

//////////////////////////////////////////////////////////
// Interface
//////////////////////////////////////////////////////////

void *FnExportTest::pIFClose()
{
    if (m_hModule != NULL && NULL == m_pIFClose)
        m_pIFClose = GetProcAddress(m_hModule, "IFClose");
    return m_pIFClose;
}

void *FnExportTest::pIFGetInfo()
{
    if (m_hModule != NULL && NULL == m_pIFGetInfo)
        m_pIFGetInfo = GetProcAddress(m_hModule, "IFGetInfo");
    return m_pIFGetInfo;
}

void *FnExportTest::pIFGetNumDevices()
{
    if (m_hModule != NULL && NULL == m_pIFGetNumDevices)
        m_pIFGetNumDevices = GetProcAddress(m_hModule, "IFGetNumDevices");
    return m_pIFGetNumDevices;
}

void *FnExportTest::pIFGetDeviceID()
{
    if (m_hModule != NULL && NULL == m_pIFGetDeviceID)
        m_pIFGetDeviceID = GetProcAddress(m_hModule, "IFGetDeviceID");
    return m_pIFGetDeviceID;
}

void *FnExportTest::pIFUpdateDeviceList()
{
    if (m_hModule != NULL && NULL == m_pIFUpdateDeviceList)
        m_pIFUpdateDeviceList = GetProcAddress(m_hModule, "IFUpdateDeviceList");
    return m_pIFUpdateDeviceList;
}

void *FnExportTest::pIFGetDeviceInfo()
{
    if (m_hModule != NULL && NULL == m_pIFGetDeviceInfo)
        m_pIFGetDeviceInfo = GetProcAddress(m_hModule, "IFGetDeviceInfo");
    return m_pIFGetDeviceInfo;
}

void *FnExportTest::pIFOpenDevice()
{
    if (m_hModule != NULL && NULL == m_pIFOpenDevice)
        m_pIFOpenDevice = GetProcAddress(m_hModule, "IFOpenDevice");
    return m_pIFOpenDevice;
}

void *FnExportTest::pIFGetParentTL()
{
    if (m_hModule != NULL && NULL == m_pIFGetParentTL)
        m_pIFGetParentTL = GetProcAddress(m_hModule, "IFGetParentTL");
    return m_pIFGetParentTL;
}

//////////////////////////////////////////////////////////
// Device
//////////////////////////////////////////////////////////

void *FnExportTest::pDevGetPort()
{
    if (m_hModule != NULL && NULL == m_pDevGetPort)
        m_pDevGetPort = GetProcAddress(m_hModule, "DevGetPort");
    return m_pDevGetPort;
}

void *FnExportTest::pDevGetNumDataStreams()
{
    if (m_hModule != NULL && NULL == m_pDevGetNumDataStreams)
        m_pDevGetNumDataStreams = GetProcAddress(m_hModule, "DevGetNumDataStreams");
    return m_pDevGetNumDataStreams;
}

void *FnExportTest::pDevGetDataStreamID()
{
    if (m_hModule != NULL && NULL == m_pDevGetDataStreamID)
        m_pDevGetDataStreamID = GetProcAddress(m_hModule, "DevGetDataStreamID");
    return m_pDevGetDataStreamID;
}

void *FnExportTest::pDevOpenDataStream()
{
    if (m_hModule != NULL && NULL == m_pDevOpenDataStream)
        m_pDevOpenDataStream = GetProcAddress(m_hModule, "DevOpenDataStream");
    return m_pDevOpenDataStream;
}

void *FnExportTest::pDevGetInfo()
{
    if (m_hModule != NULL && NULL == m_pDevGetInfo)
        m_pDevGetInfo = GetProcAddress(m_hModule, "DevGetInfo");
    return m_pDevGetInfo;
}

void *FnExportTest::pDevClose()
{
    if (m_hModule != NULL && NULL == m_pDevClose)
        m_pDevClose = GetProcAddress(m_hModule, "DevClose");
    return m_pDevClose;
}

void *FnExportTest::pDevGetParentIF()
{
    if (m_hModule != NULL && NULL == m_pDevGetParentIF)
        m_pDevGetParentIF = GetProcAddress(m_hModule, "DevGetParentIF");
    return m_pDevGetParentIF;
}

//////////////////////////////////////////////////////////
// Datastream
//////////////////////////////////////////////////////////

void *FnExportTest::pDSAnnounceBuffer()
{
    if (m_hModule != NULL && NULL == m_pDSAnnounceBuffer)
        m_pDSAnnounceBuffer = GetProcAddress(m_hModule, "DSAnnounceBuffer");
    return m_pDSAnnounceBuffer;
}

void *FnExportTest::pDSAllocAndAnnounceBuffer()
{
    if (m_hModule != NULL && NULL == m_pDSAllocAndAnnounceBuffer)
        m_pDSAllocAndAnnounceBuffer = GetProcAddress(m_hModule, "DSAllocAndAnnounceBuffer");
    return m_pDSAllocAndAnnounceBuffer;
}

void *FnExportTest::pDSFlushQueue()
{
    if (m_hModule != NULL && NULL == m_pDSFlushQueue)
        m_pDSFlushQueue = GetProcAddress(m_hModule, "DSFlushQueue");
    return m_pDSFlushQueue;
}

void *FnExportTest::pDSStartAcquisition()
{
    if (m_hModule != NULL && NULL == m_pDSStartAcquisition)
        m_pDSStartAcquisition = GetProcAddress(m_hModule, "DSStartAcquisition");
    return m_pDSStartAcquisition;
}

void *FnExportTest::pDSStopAcquisition()
{
    if (m_hModule != NULL && NULL == m_pDSStopAcquisition)
        m_pDSStopAcquisition = GetProcAddress(m_hModule, "DSStopAcquisition");
    return m_pDSStopAcquisition;
}

void *FnExportTest::pDSGetInfo()
{
    if (m_hModule != NULL && NULL == m_pDSGetInfo)
        m_pDSGetInfo = GetProcAddress(m_hModule, "DSGetInfo");
    return m_pDSGetInfo;
}

void *FnExportTest::pDSGetBufferID()
{
    if (m_hModule != NULL && NULL == m_pDSGetBufferID)
        m_pDSGetBufferID = GetProcAddress(m_hModule, "DSGetBufferID");
    return m_pDSGetBufferID;
}

void *FnExportTest::pDSClose()
{
    if (m_hModule != NULL && NULL == m_pDSClose)
        m_pDSClose = GetProcAddress(m_hModule, "DSClose");
    return m_pDSClose;
}

void *FnExportTest::pDSRevokeBuffer()
{
    if (m_hModule != NULL && NULL == m_pDSRevokeBuffer)
        m_pDSRevokeBuffer = GetProcAddress(m_hModule, "DSRevokeBuffer");
    return m_pDSRevokeBuffer;
}

void *FnExportTest::pDSQueueBuffer()
{
    if (m_hModule != NULL && NULL == m_pDSQueueBuffer)
        m_pDSQueueBuffer = GetProcAddress(m_hModule, "DSQueueBuffer");
    return m_pDSQueueBuffer;
}

void *FnExportTest::pDSGetBufferInfo()
{
    if (m_hModule != NULL && NULL == m_pDSGetBufferInfo)
        m_pDSGetBufferInfo = GetProcAddress(m_hModule, "DSGetBufferInfo");
    return m_pDSGetBufferInfo;
}

void *FnExportTest::pDSGetBufferChunkData()
{
    if (m_hModule != NULL && NULL == m_pDSGetBufferChunkData)
        m_pDSGetBufferChunkData = GetProcAddress(m_hModule, "DSGetBufferChunkData");
    return m_pDSGetBufferChunkData;
}

void *FnExportTest::pDSGetParentDev()
{
    if (m_hModule != NULL && NULL == m_pDSGetParentDev)
        m_pDSGetParentDev = GetProcAddress(m_hModule, "DSGetParentDev");
    return m_pDSGetParentDev;
}