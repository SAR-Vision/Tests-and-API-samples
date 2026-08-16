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
#include <map>

#include "GenApi/GenApi.h"

#include "Impl_Test.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "Signaling_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Impl_Test::Impl_Test()
{
}

Impl_Test::~Impl_Test()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Impl_Test::setUp(void)
{
}

void Impl_Test::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test normal acquisition
////////////////////////////////////////////////////////////////////////////////////////////

void Impl_Test::TestNormalAcquisitionConsumerBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard acquisition with registered event EVENT_NEW_BUFFER (using DSAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        if (uiNumDevices > 0)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition;

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            // try to open top down
            Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_EXCLUSIVE, &hDev);
            if (Result < GC_ERR_SUCCESS)
            {
                Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_CONTROL, &hDev);

                if (Result < GC_ERR_SUCCESS)
                {
                    std::string sMsg;

                    Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_READONLY, &hDev);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        sMsg = oLibSysSetup.sGetLastErrorMessage();
                    }

                    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice(DEVICE_ACCESS_READONLY) failed with error " << sConvertGCError2String(Result) << 
                        ", error message = " << sMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS && Result >= GC_ERR_SUCCESS)
            {
                m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
                uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                if (uiNumDataStreams > 0)
                {
                    DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    std::vector<BUFFER_HANDLE> vecBuffer;
                    std::vector<void *> vecPayLoad;
                    EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                    std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, 0);
                    DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                    size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                    int nEventGetDataTimeoutCounter=0;
                    int nEventGetDataSuccessCounter=0;
                        
                    for (uint32_t i=0; i<NUMBER_BUFFERS; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        void *pvPayLoad=NULL;

                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer, &pvPayLoad);
                        vecBuffer.push_back(hBuffer);
                        vecPayLoad.push_back(pvPayLoad);
                    }

                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                    oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                    
                    GENTLTEST_PRINT("Info: starting image acquisition and do " << NUMBER_EVENTGETDATA_CYCLES << " EventGetData cycles" << std::endl);
                    GENTLTEST_PRINT("      the first " << NUMBER_SHOW_INFOS << " data buffer infos will be shown." << std::endl);

                    oSigPreCondition.eStartDevice(m_oParameter);

                    nTestRunCounter++; 

                    for (int i=0; i<NUMBER_EVENTGETDATA_CYCLES; i++)
                    {
                        GC_ERROR Result=oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, EVENTGETDATA_TIMEOUT);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData Frame#" << i << " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS) 
                        {
                            if (i < NUMBER_SHOW_INFOS)
                            {
                                GENTLTEST_PRINT("Info: data received: Frame#" << i << " BufferHandle=0x" << std::hex << xData.BufferHandle << 
                                    ", UserPointer=0x" << xData.pUserPointer << 
                                    ", DataSize=" << std::dec << iDataSize << std::endl);
                            }

                            nEventGetDataSuccessCounter++;

                            vDumpBufferInfosConsumer(oLibSysSetup, hDs, xData.BufferHandle, vecPayLoad, uiPayLoadSize, i);

                            Result = oDSPreCondition.eDSQueueBuffer(hDs, xData.BufferHandle);
                        }

                        if (Result == GC_ERR_TIMEOUT)
                        {
                            GENTLTEST_PRINT("Hint: EventGetData Frame#" << i << " failed with result GC_ERR_TIMEOUT" << std::endl);
                            nEventGetDataTimeoutCounter++;
                        }
                    }

                    GENTLTEST_PRINT("Info: EventGetData statistics: " << nEventGetDataSuccessCounter << 
                        " succeeded, " << nEventGetDataTimeoutCounter << " timeouts." << std::endl);

                    oSigPreCondition.eStopDevice(m_oParameter);

                    oDSPreCondition.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL);
                    oDSPreCondition.eLockParameter(m_oParameter, false);
                    oDSPreCondition.eDSFlushQueue(hDs, ACQ_QUEUE_ALL_DISCARD);

                    std::vector<BUFFER_HANDLE>::iterator xIter;
                    for (xIter=vecBuffer.begin(); xIter!=vecBuffer.end(); xIter++)
                    {
                        oDSPreCondition.eDSRevokeBuffer(hDs, *xIter, NULL, NULL);
                    }
                    
                }
            }
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Impl_Test::TestNormalAcquisitionConsumerBufferForAquiredFrames( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard acquisition with registered event EVENT_NEW_BUFFER for correct event amount of 10 (using DSAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        if (uiNumDevices > 0)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition;

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            // try to open top down
            Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_EXCLUSIVE, &hDev);
            if (Result < GC_ERR_SUCCESS)
            {
                Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_CONTROL, &hDev);

                if (Result < GC_ERR_SUCCESS)
                {
                    std::string sMsg;

                    Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_READONLY, &hDev);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        sMsg = oLibSysSetup.sGetLastErrorMessage();
                    }

                    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice(DEVICE_ACCESS_READONLY) failed with error " << sConvertGCError2String(Result) << 
                        ", error message = " << sMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS && Result >= GC_ERR_SUCCESS)
            {
                m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
                uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                if (uiNumDataStreams > 0)
                {
                    DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    std::vector<BUFFER_HANDLE> vecBuffer;
                    std::vector<void *> vecPayLoad;
                    EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                    std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, 0);
                    DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                    size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                        
                    for (uint32_t i=0; i<NUMBER_BUFFERS; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        void *pvPayLoad=NULL;

                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer, &pvPayLoad);
                        vecBuffer.push_back(hBuffer);
                        vecPayLoad.push_back(pvPayLoad);
                    }

                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, 10);
                    oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                    
                    oSigPreCondition.eStartDevice(m_oParameter);

                    nTestRunCounter++;
    
                    GC_ERROR Result=GC_ERR_SUCCESS;
                    int nCycles=0;
                    while (Result == GC_ERR_SUCCESS || nCycles > 10)
                    {
                        Result = oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, 10000);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS) 
                        {
                            nCycles++;
                            Result = oDSPreCondition.eDSQueueBuffer(hDs, xData.BufferHandle);
                        }
                    }

                    GENTLTEST_CHECK_MESSAGE("Error: last expected result == GC_ERR_TIMEOUT, received = " << sConvertGCError2String(Result).c_str(), Result == GC_ERR_TIMEOUT);
                    GENTLTEST_CHECK_MESSAGE("Error: collected data event amount = " << nCycles, nCycles == 10);

                    oSigPreCondition.eStopDevice(m_oParameter);

                    oDSPreCondition.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL);
                    oDSPreCondition.eLockParameter(m_oParameter, false);
                    oDSPreCondition.eDSFlushQueue(hDs, ACQ_QUEUE_ALL_DISCARD);

                    std::vector<BUFFER_HANDLE>::iterator xIter;
                    for (xIter=vecBuffer.begin(); xIter!=vecBuffer.end(); xIter++)
                    {
                        oDSPreCondition.eDSRevokeBuffer(hDs, *xIter, NULL, NULL);
                    }
                    
                }
            }
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Impl_Test::TestNormalAcquisitionConsumerBufferDSGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard acquisition with registered event EVENT_NEW_BUFFER (using DSAnnounceBuffer and DSGetInfo)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        if (uiNumDevices > 0)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition;

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            // try to open top down
            Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_EXCLUSIVE, &hDev);
            if (Result < GC_ERR_SUCCESS)
            {
                Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_CONTROL, &hDev);

                if (Result < GC_ERR_SUCCESS)
                {
                    std::string sMsg;

                    Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_READONLY, &hDev);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        sMsg = oLibSysSetup.sGetLastErrorMessage();
                    }

                    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice(DEVICE_ACCESS_READONLY) failed with error " << sConvertGCError2String(Result) << 
                        ", error message = " << sMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS && Result >= GC_ERR_SUCCESS)
            {
                uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();

                m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                if (uiNumDataStreams > 0)
                {
                    DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    std::vector<BUFFER_HANDLE> vecBuffer;
                    std::vector<void *> vecPayLoad;
                    EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                    std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, 0);
                    DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                    size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    size_t uiNumberOfBuffers=oDSPreCondition.uiDSGetInfoMinBuffers(hDs);
                        
                    for (uint32_t i=0; i<uiNumberOfBuffers; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        void *pvPayLoad=NULL;

                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer, &pvPayLoad);
                        vecBuffer.push_back(hBuffer);
                        vecPayLoad.push_back(pvPayLoad);
                    }

                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                    oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                    
                    GENTLTEST_PRINT("Info: starting image acquisition and do " << NUMBER_EVENTGETDATA_CYCLES << " EventGetData cycles" << std::endl);

                    oSigPreCondition.eStartDevice(m_oParameter);

					nTestRunCounter++;

                    for (int i=0; i<NUMBER_EVENTGETDATA_CYCLES; i++)
                    {
                        GC_ERROR Result=oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, EVENTGETDATA_TIMEOUT);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS) 
                        {
                            if (i < NUMBER_SHOW_INFOS)
                            {
                                GENTLTEST_PRINT("Info: data received: buffer BufferHandle=0x" << std::hex << xData.BufferHandle << 
                                    ", UserPointer=0x" << xData.pUserPointer << 
                                    ", DataSize=" << std::dec << iDataSize << std::endl);
                            }

                            vDumpBufferInfosConsumer(oLibSysSetup, hDs, xData.BufferHandle, vecPayLoad, uiPayLoadSize, i);

                            Result = oDSPreCondition.eDSQueueBuffer(hDs, xData.BufferHandle);
                        }
                    }

                    oSigPreCondition.eStopDevice(m_oParameter);

                    oDSPreCondition.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL);
                    oDSPreCondition.eLockParameter(m_oParameter, false);
                    oDSPreCondition.eDSFlushQueue(hDs, ACQ_QUEUE_ALL_DISCARD);

                    std::vector<BUFFER_HANDLE>::iterator xIter;
                    for (xIter=vecBuffer.begin(); xIter!=vecBuffer.end(); xIter++)
                    {
                        oDSPreCondition.eDSRevokeBuffer(hDs, *xIter, NULL, NULL);
                    }
                    
                }
            }
        }
    }

	GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Impl_Test::TestNormalAcquisitionProducerBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard acquisition with registered event EVENT_NEW_BUFFER (using DSAllocAndAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        if (uiNumDevices > 0)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition;

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            // try to open top down
            Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_EXCLUSIVE, &hDev);
            if (Result < GC_ERR_SUCCESS)
            {
                Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_CONTROL, &hDev);

                if (Result < GC_ERR_SUCCESS)
                {
                    std::string sMsg;

                    Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_READONLY, &hDev);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        sMsg = oLibSysSetup.sGetLastErrorMessage();
                    }

                    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice(DEVICE_ACCESS_READONLY) failed with error " << sConvertGCError2String(Result) << 
                        ", error message = " << sMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS && Result >= GC_ERR_SUCCESS)
            {
                m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                if (uiNumDataStreams > 0)
                {
                    DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    std::vector<BUFFER_HANDLE> vecBuffer;
                    EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                    std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, 0);
                    DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                    size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                    int nEventGetDataTimeoutCounter=0;
                    int nEventGetDataSuccessCounter=0;
                        
                    for (uint32_t i=0; i<NUMBER_BUFFERS; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        
                        oDSPreCondition.eDSAllocAndAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        vecBuffer.push_back(hBuffer);
                    }

                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                    oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                    
                    GENTLTEST_PRINT("Info: starting image acquisition and do " << NUMBER_EVENTGETDATA_CYCLES << " EventGetData" << std::endl);
                    GENTLTEST_PRINT("      the first " << NUMBER_SHOW_INFOS << " data buffer infos will be shown." << std::endl);

                    oSigPreCondition.eStartDevice(m_oParameter);

                    nTestRunCounter++;
    
                    for (int i=0; i<NUMBER_EVENTGETDATA_CYCLES; i++)
                    {
                        GC_ERROR Result=oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, EVENTGETDATA_TIMEOUT);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData Frame#" << i << " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS) 
                        {
                            if (i < NUMBER_SHOW_INFOS)
                            {
                                GENTLTEST_PRINT("Info: data received: Frame#" << i << " BufferHandle=0x" << std::hex << xData.BufferHandle << 
                                    ", UserPointer=0x" << xData.pUserPointer << 
                                    ", DataSize=" << std::dec << iDataSize << std::endl);
                            }

                            nEventGetDataSuccessCounter++;

                            vDumpBufferInfosProducer(oLibSysSetup, hDs, xData.BufferHandle, i);

                            Result = oDSPreCondition.eDSQueueBuffer(hDs, xData.BufferHandle);
                        }

                        if (Result == GC_ERR_TIMEOUT)
                        {
                            GENTLTEST_PRINT("Hint: EventGetData Frame#" << i << " failed with result GC_ERR_TIMEOUT" << std::endl);
                            nEventGetDataTimeoutCounter++;
                        }
                    }

                    GENTLTEST_PRINT("Info: EventGetData statistics: " << nEventGetDataSuccessCounter << 
                        " succeeded, " << nEventGetDataTimeoutCounter << " timeouts." << std::endl);

                    oSigPreCondition.eStopDevice(m_oParameter);

                    oDSPreCondition.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL);
                    oDSPreCondition.eLockParameter(m_oParameter, false);
                    oDSPreCondition.eDSFlushQueue(hDs, ACQ_QUEUE_ALL_DISCARD);

                    std::vector<BUFFER_HANDLE>::iterator xIter;
                    for (xIter=vecBuffer.begin(); xIter!=vecBuffer.end(); xIter++)
                    {
                        oDSPreCondition.eDSRevokeBuffer(hDs, *xIter, NULL, NULL);
                    }
                    
                }
            }
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Impl_Test::TestCommandNewDataProducerBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test while receiving event EVENT_NEW_BUFFER if DSGetBufferInfo BUFFER_INFO_NEW_DATA returns true (using DSAllocAndAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        if (uiNumDevices > 0)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition;

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            // try to open top down
            Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_EXCLUSIVE, &hDev);
            if (Result < GC_ERR_SUCCESS)
            {
                Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_CONTROL, &hDev);

                if (Result < GC_ERR_SUCCESS)
                {
                    std::string sMsg;

                    Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_READONLY, &hDev);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        sMsg = oLibSysSetup.sGetLastErrorMessage();
                    }

                    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice(DEVICE_ACCESS_READONLY) failed with error " << sConvertGCError2String(Result) << 
                        ", error message = " << sMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS && Result >= GC_ERR_SUCCESS)
            {
                m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                if (uiNumDataStreams > 0)
                {
                    DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    std::vector<BUFFER_HANDLE> vecBuffer;
                    EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                    std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, 0);
                    DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                    size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                        
                    for (uint32_t i=0; i<NUMBER_BUFFERS; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        
                        oDSPreCondition.eDSAllocAndAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        vecBuffer.push_back(hBuffer);
                    }

                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                    oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                    
                    oSigPreCondition.eStartDevice(m_oParameter);

                    nTestRunCounter++;
    
                    for (int i=0; i<NUMBER_EVENTGETDATA_CYCLES; i++)
                    {
                        GC_ERROR Result=oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, EVENTGETDATA_TIMEOUT);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS && !bCheckForInvalidFrame(hDs, xData.BufferHandle, i))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=1;
                            bool bNewData=false;

                            Result = m_ModDS.eDSGetBufferInfo(hDs, xData.BufferHandle, BUFFER_INFO_NEW_DATA, &iType, &bNewData, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo(BUFFER_INFO_NEW_DATA) failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo(BUFFER_INFO_NEW_DATA) call, NewData = false, should be true", bNewData == true);
                            }

                            Result = oDSPreCondition.eDSQueueBuffer(hDs, xData.BufferHandle);
                        }
                    }

                    oSigPreCondition.eStopDevice(m_oParameter);

                    oDSPreCondition.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL);
                    oDSPreCondition.eLockParameter(m_oParameter, false);
                    oDSPreCondition.eDSFlushQueue(hDs, ACQ_QUEUE_ALL_DISCARD);

                    std::vector<BUFFER_HANDLE>::iterator xIter;
                    for (xIter=vecBuffer.begin(); xIter!=vecBuffer.end(); xIter++)
                    {
                        oDSPreCondition.eDSRevokeBuffer(hDs, *xIter, NULL, NULL);
                    }
                    
                }
            }
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Impl_Test::TestGCGetPortInfoPortName( uint32_t test_id )
{
	GC_ERROR eResult=GC_ERR_SUCCESS;
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo check Portnames (see section 4.1.1)");

    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);

    std::string sResult;
	eResult = oLibSysSetup.eGCGetPortInfoPortName(oLibSysSetup.hGetTLHandle(), sResult);

    GENTLTEST_CHECK_MESSAGE("Return value '" << sResult << "' not equal 'TLPort'", sResult == "TLPort" && eResult >= GC_ERR_SUCCESS);

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        std::string sResult;
		eResult = oLibSysSetup.eGCGetPortInfoPortName(hIF, sResult);

        GENTLTEST_CHECK_MESSAGE("Return value '" << sResult << "' not equal 'InterfacePort'", sResult == "InterfacePort" && eResult >= GC_ERR_SUCCESS);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                    uint32_t uiNumdataStream=0;
                
                    std::string sDevicePortName;
					eResult = oLibSysSetup.eGCGetPortInfoPortName(hDev, sDevicePortName);
                    GENTLTEST_CHECK_MESSAGE("Return value '" << sDevicePortName << "' not equal 'DevicePort'", sDevicePortName == "DevicePort" && eResult >= GC_ERR_SUCCESS);

                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    hPort = oDevPreCondition.hDevGetPort();
                    std::string sResult;
					eResult = oLibSysSetup.eGCGetPortInfoPortName(hPort, sResult);

                    GENTLTEST_CHECK_MESSAGE("Return value '" << sResult << "' not equal 'Device'", sResult == "Device" && eResult >= GC_ERR_SUCCESS);

                    uiNumdataStream = oDevPreCondition.uiGetNumDataStreams();

                    for (uint32_t index3=0; index3<uiNumdataStream; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDatastreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDatastreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        std::string sResult;
						eResult = oLibSysSetup.eGCGetPortInfoPortName(hDs, sResult);

                        GENTLTEST_CHECK_MESSAGE("Return value '" << sResult << "' not equal 'StreamPort'", sResult == "StreamPort" && eResult >= GC_ERR_SUCCESS);

                        nTestRunCounter++;
    
                        oDSPreCondition.eDSAllocAndAnnounceBuffer(hDs, uiPayLoadSize, (void*)this, &hBuffer);

                        eResult = oLibSysSetup.eGCGetPortInfoPortName(hBuffer, sResult); // 4.1.1

                        if (eResult >= GC_ERR_SUCCESS) // if implemented
                        {
                            GENTLTEST_CHECK_MESSAGE("Return value '" << sResult << "' not equal 'BufferPort'", sResult == "BufferPort");
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Impl_Test::TestBufferPoolLockDuringAcquisition( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test buffer pool lock during running aquisition (see section 5.1.1 and 5.2.2)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    GENTLTEST_REQUIRE_MESSAGE("No interfaces found, test aborted.", xInterfaceList.size() > 0);

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        if (uiNumDevices > 0)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition;

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            // try to open top down
            Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_EXCLUSIVE, &hDev);
            if (Result < GC_ERR_SUCCESS)
            {
                Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_CONTROL, &hDev);

                if (Result < GC_ERR_SUCCESS)
                {
                    std::string sMsg;

                    Result = oDevPreCondition.eIFOpenDevice(hIF, sDeviceID, DEVICE_ACCESS_READONLY, &hDev);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        sMsg = oLibSysSetup.sGetLastErrorMessage();
                    }

                    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice(DEVICE_ACCESS_READONLY) failed with error " << sConvertGCError2String(Result) << 
                        ", error message = " << sMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS && Result >= GC_ERR_SUCCESS)
            {
                m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
                uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                if (uiNumDataStreams > 0)
                {
                    DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    std::vector<BUFFER_HANDLE> vecBuffer;
                    std::vector<void *> vecPayLoad;
                    EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                    std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, 0);
                    DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                    size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    std::vector<char> vecPayLoadBuffer(uiPayLoadSize);
                        
                    for (uint32_t i=0; i<NUMBER_BUFFERS; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        void *pvPayLoad=NULL;

                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer, &pvPayLoad);
                        vecBuffer.push_back(hBuffer);
                        vecPayLoad.push_back(pvPayLoad);
                    }

                    nTestRunCounter++;
    
                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                    
                    BUFFER_HANDLE hNewBuffer;
                    GC_ERROR eResult = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoadBuffer[0], uiPayLoadSize, (void*)this, &hNewBuffer);
                    GENTLTEST_CHECK_MESSAGE("Buffer pool not locked for announcement.", eResult < GC_ERR_SUCCESS);

                    oDSPreCondition.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL);
                    oDSPreCondition.eLockParameter(m_oParameter, false);
                    oDSPreCondition.eDSFlushQueue(hDs, ACQ_QUEUE_ALL_DISCARD);

                    std::vector<BUFFER_HANDLE>::iterator xIter;
                    for (xIter=vecBuffer.begin(); xIter!=vecBuffer.end(); xIter++)
                    {
                        oDSPreCondition.eDSRevokeBuffer(hDs, *xIter, NULL, NULL);
                    }
                    
                }
            }
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("Test never run, may be no device plugged in.", nTestRunCounter > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

bool Impl_Test::bCheckForInvalidFrame(GenICam::Client::DS_HANDLE hDs, 
                                    GenICam::Client::BUFFER_HANDLE hBuffer, 
                                    int index)
{
    // special: checking returned buffer
    bool bResult=true;
    GC_ERROR Result=GC_ERR_SUCCESS;
    INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
    size_t iSize=1;

    Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, BUFFER_INFO_IS_INCOMPLETE, &iType, &bResult, &iSize);
    
    if (Result < GC_ERR_SUCCESS)
    {
        bResult = true;
    }

    return bResult;
}


void Impl_Test::vDumpBufferInfosConsumer(LibrarySystemSetup &oLibSysSetup, 
                                         GenICam::Client::DS_HANDLE hDs, 
                                         GenICam::Client::BUFFER_HANDLE hBuffer, 
                                         std::vector<void *> &vecPayLoad, 
                                         size_t uiPayLoadSize,
                                         int index)
{
    // special: checking returned buffer

    if (!bCheckForInvalidFrame(hDs, hBuffer, index))
    {
        void *pvComparePattern=malloc(uiPayLoadSize);
        memset(pvComparePattern, 0xAA, uiPayLoadSize);
    
        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
        {
            GC_ERROR Result=GC_ERR_SUCCESS;
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;

            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": DSGetBufferInfo (" << sConvertDataStreamBufferCommand2String(eCommand).c_str() << ") 1st failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Frame#" << index << ": Parameter after call iSize == 0", iSize > 0);

                std::vector<char> sBuffer(iSize);
                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, &sBuffer[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": DSGetBufferInfo (" << sConvertDataStreamBufferCommand2String(eCommand).c_str() << ") 2nd failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

                if (Result >= GC_ERR_SUCCESS)
                {
                    std::vector<void *>::iterator xIter;
                
                    if (index < NUMBER_SHOW_INFOS)
                        GENTLTEST_DUMP_ITYPE("    Info: Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " = ", iType, &sBuffer[0], iSize);

                    switch(eCommand)
                    {
                    case BUFFER_INFO_SIZE_FILLED:
                    case BUFFER_INFO_SIZE: 
                    case BUFFER_INFO_WIDTH:
                    case BUFFER_INFO_HEIGHT:
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", xConvertBuffer2SIZET(&sBuffer[0], iSize) != 0);
                        break;
                    case BUFFER_INFO_BASE: {
                        void *pvPayLoad=(void*)xConvertBuffer2PTR(&sBuffer[0], iSize);
                    
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", xConvertBuffer2PTR(&sBuffer[0], iSize) != 0);
                    
                        for (xIter=vecPayLoad.begin(); xIter!=vecPayLoad.end(); xIter++)
                        {
                            if (*xIter == pvPayLoad)
                            {
                                break;
                            }
                        }
                        GENTLTEST_CHECK_MESSAGE("    Error Frame#" << index << ": payload pointer not found in poiter list.", *xIter == pvPayLoad);
                        if (*xIter == pvPayLoad)
                        {
                            GENTLTEST_CHECK_MESSAGE("    Error Frame#" << index << ": payload buffer not filled with data.", memcmp(pvPayLoad, pvComparePattern, uiPayLoadSize) != 0);
                        }
                        }
                        break;
                    case BUFFER_INFO_USER_PTR: 
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", (void*)xConvertBuffer2PTR(&sBuffer[0], iSize) == (void *)this);
                        break;
                    case BUFFER_INFO_TIMESTAMP: 
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", xConvertBuffer2UINT64(&sBuffer[0]) != 0);
                        break;
                    case BUFFER_INFO_NEW_DATA: 
                        if (index == 0)
                            GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", sBuffer[0] != 0);
                        break;
                    case BUFFER_INFO_TLTYPE:
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            std::string sTLType=&sBuffer[0];
                            GENTLTEST_CHECK_MESSAGE("BUFFER_INFO_TLTYPE tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                        }
                        break;
                    case BUFFER_INFO_PAYLOADTYPE:
                        //GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", (PAYLOADTYPE_INFO_IDS)xConvertBuffer2SIZET(&sBuffer[0], iSize) != 0);
                        break;
                    }
                }
            }
            else
            {
                if (index < NUMBER_SHOW_INFOS)
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    if (Result == GC_ERR_NOT_IMPLEMENTED)
                    {
                        GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                    }
                    else
                    {
                        GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                    }
                }
            }
        }
        free(pvComparePattern);
    }
    else
    {
        GENTLTEST_PRINT("Error: Frame#" << index << ": is incomplete." << std::endl);
    }
}

void Impl_Test::vDumpBufferInfosProducer(LibrarySystemSetup &oLibSysSetup, 
                                         GenICam::Client::DS_HANDLE hDs, 
                                         GenICam::Client::BUFFER_HANDLE hBuffer, 
                                         int index)
{
    // special: not checking returned buffer

    if (!bCheckForInvalidFrame(hDs, hBuffer, index))
    {
        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
        {
            GC_ERROR Result=GC_ERR_SUCCESS;
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;

            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": DSGetBufferInfo (" << sConvertDataStreamBufferCommand2String(eCommand).c_str() << ") 1st failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Frame#" << index << ": Parameter after call iSize == 0", iSize > 0);

                std::vector<char> sBuffer(iSize);
                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, &sBuffer[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": DSGetBufferInfo (" << sConvertDataStreamBufferCommand2String(eCommand).c_str() << ") 2nd failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

                if (Result >= GC_ERR_SUCCESS)
                {
                    if (index < NUMBER_SHOW_INFOS)
                        GENTLTEST_DUMP_ITYPE("    Info: Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " = ", iType, &sBuffer[0], iSize);

                    switch(eCommand)
                    {
                    case BUFFER_INFO_SIZE_FILLED:
                    case BUFFER_INFO_SIZE: 
                    case BUFFER_INFO_WIDTH:
                    case BUFFER_INFO_HEIGHT:
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", xConvertBuffer2SIZET(&sBuffer[0], iSize) != 0);
                        break;
                    case BUFFER_INFO_BASE:
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", xConvertBuffer2PTR(&sBuffer[0], iSize) != 0);
                        break;
                    case BUFFER_INFO_USER_PTR: 
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", (void*)xConvertBuffer2PTR(&sBuffer[0], iSize) == (void *)this);
                        break;
                    case BUFFER_INFO_TIMESTAMP: 
                        GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", xConvertBuffer2UINT64(&sBuffer[0]) != 0);
                        break;
                    case BUFFER_INFO_NEW_DATA: 
                        if (index == 0)
                            GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", sBuffer[0] != 0);
                        break;
                    case BUFFER_INFO_TLTYPE:
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            std::string sTLType=&sBuffer[0];
                            GENTLTEST_CHECK_MESSAGE("BUFFER_INFO_TLTYPE tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                        }
                        break;
                    case BUFFER_INFO_PAYLOADTYPE:
                        //GENTLTEST_CHECK_MESSAGE("Frame#" << index << ": " << sConvertDataStreamBufferCommand2String(eCommand).c_str() << " value = 0", (PAYLOADTYPE_INFO_IDS)xConvertBuffer2SIZET(&sBuffer[0], iSize) != 0);
                        break;
                    }
                }
            }
            else
            {
                if (index < NUMBER_SHOW_INFOS)
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    if (Result == GC_ERR_NOT_IMPLEMENTED)
                    {
                        GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                    }
                    else
                    {
                        GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_PRINT("Error: Frame#" << index << ": is incomplete." << std::endl);
    }
}

