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

#include "Impl_Test2.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "Signaling_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_bTestChunkData;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Impl_Test2::Impl_Test2()
{
}

Impl_Test2::~Impl_Test2()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Impl_Test2::setUp(void)
{
}

void Impl_Test2::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test normal acquisition
////////////////////////////////////////////////////////////////////////////////////////////

void Impl_Test2::TestAcquisitionConsumerBufferDSGetBufferChunkData( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test acquisition with registered event EVENT_NEW_BUFFER and get buffer chunk data (using DSAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;

    if (!g_bTestChunkData)
    {
        GENTLTEST_SKIP("No chunkdata tests selected.");
        GENTLTEST_PRINT_RESULT(test_id);
        return;
    }

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
                    size_t uiPayLoadSize = 0;
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                    uint32_t uiNumberOfChunks=0;
                    uint32_t uiNumPayloadUnknown=0;
                    uint32_t uiNumPayloadImage=0;
                    uint32_t uiNumPayloadRawdata=0;
                    uint32_t uiNumPayloadFile=0;
                    uint32_t uiNumPayloadChunkdata=0;
                    uint32_t uiNumPayloadJPEG=0;
                    uint32_t uiNumPayloadJPEG2000=0;
                    uint32_t uiNumPayloadH264=0;
                    uint32_t uiNumPayloadChunkOnly=0;
                    uint32_t uiNumPayloadDeviceSpecific=0;
                    int nEventGetDataTimeoutCounter=0;
                    int nEventGetDataSuccessCounter=0;
                    
                    oDSPreCondition.eSetChunkModeActive(m_oParameter);
                    uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    
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
                    
                    GENTLTEST_PRINT("Info: starting image acquisition and do " << NUMBER_EVENTCHUNKDATA_CYCLES << " EventGetData cycles" << std::endl);
                    
                    oSigPreCondition.eStartDevice(m_oParameter);

                    nTestRunCounter++;
    
                    for (int i=0; i<NUMBER_EVENTCHUNKDATA_CYCLES; i++)
                    {
                        GC_ERROR Result=oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, EVENTGETDATA_TIMEOUT);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS) 
                        {
                            uint32_t uiPayloadUnknown=0;
                            uint32_t uiPayloadImage=0;
                            uint32_t uiPayloadRawdata=0;
                            uint32_t uiPayloadFile=0;
                            uint32_t uiPayloadChunkdata=0;
                            uint32_t uiPayloadJPEG=0;
                            uint32_t uiPayloadJPEG2000=0;
                            uint32_t uiPayloadH264=0;
                            uint32_t uiPayloadChunkOnly=0;
                            uint32_t uiPayloadDeviceSpecific=0;
                    
                            nEventGetDataSuccessCounter++;

                            uiNumberOfChunks += uiTryToGetChunkData(oLibSysSetup, 
                                                                    hDs, 
                                                                    xData.BufferHandle, 
                                                                    i,
                                                                    uiPayloadUnknown,
                                                                    uiPayloadImage,
                                                                    uiPayloadRawdata,
                                                                    uiPayloadFile,
                                                                    uiPayloadChunkdata,
                                                                    uiPayloadJPEG,
                                                                    uiPayloadJPEG2000,
                                                                    uiPayloadH264,
                                                                    uiPayloadChunkOnly,
                                                                    uiPayloadDeviceSpecific);

                            uiNumPayloadUnknown += uiPayloadUnknown;
                            uiNumPayloadImage += uiPayloadImage;
                            uiNumPayloadRawdata += uiPayloadRawdata;
                            uiNumPayloadFile += uiPayloadFile;
                            uiNumPayloadChunkdata += uiPayloadChunkdata;
                            uiNumPayloadJPEG += uiPayloadJPEG;
                            uiNumPayloadJPEG2000 += uiPayloadJPEG2000;
                            uiNumPayloadH264 += uiPayloadH264;
                            uiNumPayloadChunkOnly += uiPayloadChunkOnly;
                            uiNumPayloadDeviceSpecific += uiPayloadDeviceSpecific;
                    
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

                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_UNKNOWN          = " << uiNumPayloadUnknown << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_IMAGE            = " << uiNumPayloadImage << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_RAW_DATA         = " << uiNumPayloadRawdata << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_FILE             = " << uiNumPayloadFile << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_CHUNK_DATA       = " << uiNumPayloadChunkdata << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_JPEG             = " << uiNumPayloadJPEG << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_JPEG2000         = " << uiNumPayloadJPEG2000 << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_H264             = " << uiNumPayloadH264 << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_CHUNK_ONLY       = " << uiNumPayloadChunkOnly << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_DEVICE_SPECIFIC  = " << uiNumPayloadDeviceSpecific << std::endl);

                    GENTLTEST_CHECK_MESSAGE("Error: No chunk data received.", uiNumberOfChunks > 0);
                    
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

void Impl_Test2::TestAcquisitionProducerBufferDSGetBufferChunkData( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test acquisition with registered event EVENT_NEW_BUFFER and get buffer chunk data (using DSAllocAndAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nTestRunCounter=0;
    
    if (!g_bTestChunkData)
    {
        GENTLTEST_SKIP("No chunkdata tests selected.");
        GENTLTEST_PRINT_RESULT(test_id);
        return;
    }

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
                    size_t uiPayLoadSize = 0;
                    Signaling_PreCondition oSigPreCondition;
                    EVENT_NEW_BUFFER_DATA xData;
                    size_t iDataSize = sizeof(xData);
                    size_t iDefaultBufferSize=0;
                    uint32_t uiNumberOfChunks=0;
                    uint32_t uiNumPayloadUnknown=0;
                    uint32_t uiNumPayloadImage=0;
                    uint32_t uiNumPayloadRawdata=0;
                    uint32_t uiNumPayloadFile=0;
                    uint32_t uiNumPayloadChunkdata=0;
                    uint32_t uiNumPayloadJPEG=0;
                    uint32_t uiNumPayloadJPEG2000=0;
                    uint32_t uiNumPayloadH264=0;
                    uint32_t uiNumPayloadChunkOnly=0;
                    uint32_t uiNumPayloadDeviceSpecific=0;
                    int nEventGetDataTimeoutCounter=0;
                    int nEventGetDataSuccessCounter=0;
                        
                    oDSPreCondition.eSetChunkModeActive(m_oParameter);
                    uiPayLoadSize = oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t i=0; i<NUMBER_BUFFERS; i++)
                    {
                        BUFFER_HANDLE hBuffer;
                        
                        oDSPreCondition.eDSAllocAndAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        vecBuffer.push_back(hBuffer);
                    }

                    oDSPreCondition.eLockParameter(m_oParameter, true);
                    oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                    oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                    
                    GENTLTEST_PRINT("Info: starting image acquisition and do " << NUMBER_EVENTCHUNKDATA_CYCLES << " EventGetData" << std::endl);
                    
                    oSigPreCondition.eStartDevice(m_oParameter);

                    nTestRunCounter++;
    
                    for (int i=0; i<NUMBER_EVENTCHUNKDATA_CYCLES; i++)
                    {
                        GC_ERROR Result=oSigPreCondition.eEventGetData(hEvent, &xData, &iDataSize, EVENTGETDATA_TIMEOUT);

                        GENTLTEST_CHECK_MESSAGE("Info: EventGetData failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT returned " << sConvertGCError2String(Result),
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
                        
                        if (Result >= GC_ERR_SUCCESS) 
                        {
                            uint32_t uiPayloadUnknown=0;
                            uint32_t uiPayloadImage=0;
                            uint32_t uiPayloadRawdata=0;
                            uint32_t uiPayloadFile=0;
                            uint32_t uiPayloadChunkdata=0;
                            uint32_t uiPayloadJPEG=0;
                            uint32_t uiPayloadJPEG2000=0;
                            uint32_t uiPayloadH264=0;
                            uint32_t uiPayloadChunkOnly=0;
                            uint32_t uiPayloadDeviceSpecific=0;
                    
                            nEventGetDataSuccessCounter++;

                            uiNumberOfChunks += uiTryToGetChunkData(oLibSysSetup, 
                                                                    hDs, 
                                                                    xData.BufferHandle, 
                                                                    i,
                                                                    uiPayloadUnknown,
                                                                    uiPayloadImage,
                                                                    uiPayloadRawdata,
                                                                    uiPayloadFile,
                                                                    uiPayloadChunkdata,
                                                                    uiPayloadJPEG,
                                                                    uiPayloadJPEG2000,
                                                                    uiPayloadH264,
                                                                    uiPayloadChunkOnly,
                                                                    uiPayloadDeviceSpecific);

                            uiNumPayloadUnknown += uiPayloadUnknown;
                            uiNumPayloadImage += uiPayloadImage;
                            uiNumPayloadRawdata += uiPayloadRawdata;
                            uiNumPayloadFile += uiPayloadFile;
                            uiNumPayloadChunkdata += uiPayloadChunkdata;
                            uiNumPayloadJPEG += uiPayloadJPEG;
                            uiNumPayloadJPEG2000 += uiPayloadJPEG2000;
                            uiNumPayloadH264 += uiPayloadH264;
                            uiNumPayloadChunkOnly += uiPayloadChunkOnly;
                            uiNumPayloadDeviceSpecific += uiPayloadDeviceSpecific;
                    
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

                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_UNKNOWN          = " << uiNumPayloadUnknown << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_IMAGE            = " << uiNumPayloadImage << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_RAW_DATA         = " << uiNumPayloadRawdata << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_FILE             = " << uiNumPayloadFile << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_CHUNK_DATA       = " << uiNumPayloadChunkdata << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_JPEG             = " << uiNumPayloadJPEG << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_JPEG2000         = " << uiNumPayloadJPEG2000 << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_H264             = " << uiNumPayloadH264 << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_CHUNK_ONLY       = " << uiNumPayloadChunkOnly << std::endl);
                    GENTLTEST_PRINT("Info: number of payloads PAYLOAD_TYPE_DEVICE_SPECIFIC  = " << uiNumPayloadDeviceSpecific << std::endl);

                    GENTLTEST_CHECK_MESSAGE("Error: No chunk data received.", uiNumberOfChunks > 0);

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

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

bool Impl_Test2::bCheckForInvalidFrame(GenICam::Client::DS_HANDLE hDs, 
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


uint32_t Impl_Test2::uiTryToGetChunkData(LibrarySystemSetup &oLibSysSetup, 
                                     GenICam::Client::DS_HANDLE hDs, 
                                     GenICam::Client::BUFFER_HANDLE hBuffer, 
                                     int index,
                                     uint32_t &uiNumPayloadUnknown,
                                     uint32_t &uiNumPayloadImage,
                                     uint32_t &uiNumPayloadRawdata,
                                     uint32_t &uiNumPayloadFile,
                                     uint32_t &uiNumPayloadChunkdata,
                                     uint32_t &uiNumPayloadJPEG,
                                     uint32_t &uiNumPayloadJPEG2000,
                                     uint32_t &uiNumPayloadH264,
                                     uint32_t &uiNumPayloadChunkOnly,
                                     uint32_t &uiNumPayloadDeviceSpecific)
{
    uint32_t uiNumberOfChunksReceived=0;

    if (!bCheckForInvalidFrame(hDs, hBuffer, index))
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        size_t iNumChunks=0;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        std::string oErrorMsg;
        
        Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, BUFFER_INFO_PAYLOADTYPE, &iType, NULL, &iSize);
        if (Result < GC_ERR_SUCCESS)
            oErrorMsg = oLibSysSetup.sGetLastErrorMessage();
        GENTLTEST_CHECK_MESSAGE("Data event #" << index << ": DSGetBufferInfo for BUFFER_INFO_PAYLOADTYPE size check failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str() <<
                ", error message = " << oErrorMsg.c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("parameter after call DSGetBufferInfo: iSize != " << sizeof(size_t), iSize == sizeof(size_t));
            GENTLTEST_REQUIRE_MESSAGE("parameter after call DSGetBufferInfo: iType != INFO_DATATYPE_SIZET", iType == INFO_DATATYPE_SIZET);

            size_t xPayloadType=0;
            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, BUFFER_INFO_PAYLOADTYPE, &iType, &xPayloadType, &iSize);
            if (Result < GC_ERR_SUCCESS)
                oErrorMsg = oLibSysSetup.sGetLastErrorMessage();
            GENTLTEST_CHECK_MESSAGE("Data event #" << index << ": DSGetBufferInfo for BUFFER_INFO_PAYLOADTYPE failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str() <<
                ", error message = " << oErrorMsg.c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                switch(xPayloadType)
                {
                    case PAYLOAD_TYPE_UNKNOWN: uiNumPayloadUnknown++; break;
                    case PAYLOAD_TYPE_IMAGE: uiNumPayloadImage++; break;
                    case PAYLOAD_TYPE_RAW_DATA: uiNumPayloadRawdata++; break;
                    case PAYLOAD_TYPE_FILE: uiNumPayloadFile++; break;
                    case PAYLOAD_TYPE_CHUNK_DATA: uiNumPayloadChunkdata++; break;
                    case PAYLOAD_TYPE_JPEG: uiNumPayloadJPEG++; break;
                    case PAYLOAD_TYPE_JPEG2000: uiNumPayloadJPEG2000++; break;
                    case PAYLOAD_TYPE_H264: uiNumPayloadH264++; break;
                    case PAYLOAD_TYPE_CHUNK_ONLY: uiNumPayloadChunkOnly++; break;
                    case PAYLOAD_TYPE_DEVICE_SPECIFIC: uiNumPayloadDeviceSpecific++; break;
                }

                if (xPayloadType == PAYLOAD_TYPE_CHUNK_DATA)
                {
                    size_t iNumChunks=0;
            
                    Result = m_ModDS.eDSGetBufferChunkData(hDs, hBuffer, NULL, &iNumChunks);
                    if (Result < GC_ERR_SUCCESS)
                        oErrorMsg = oLibSysSetup.sGetLastErrorMessage();
                    GENTLTEST_CHECK_MESSAGE("Data event #" << index << ": DSGetBufferChunkData failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str() <<
                        ", error message = " << oErrorMsg.c_str(), 
                        Result >= GC_ERR_SUCCESS);

                    if (Result >= GC_ERR_SUCCESS)
                    {
                        if (iNumChunks > 0)
                        {
                            std::vector<SINGLE_CHUNK_DATA> xChunkData(iNumChunks);

                            Result = m_ModDS.eDSGetBufferChunkData(hDs, hBuffer, &xChunkData[0], &iNumChunks);
                            if (Result < GC_ERR_SUCCESS)
                                oErrorMsg = oLibSysSetup.sGetLastErrorMessage();
                            GENTLTEST_CHECK_MESSAGE("Data event #" << index << ": DSGetBufferChunkData failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str() <<
                                ", error message = " << oErrorMsg.c_str(), 
                                Result >= GC_ERR_SUCCESS);

                            if (iNumChunks > 0)
                            {
                                uiNumberOfChunksReceived += (uint32_t)iNumChunks;
                                for (size_t i=0; i<iNumChunks; i++)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Chunk #" << i << " ChunkID == 0", xChunkData[i].ChunkID != 0);
                                    GENTLTEST_CHECK_MESSAGE("Chunk #" << i << " ChunkLength == 0", xChunkData[i].ChunkLength != 0);
                                    // SVW xChunkData[i].ChunkOffset could be 0 ???
                                }
                            }
                        }
                        else
                        {
                            GENTLTEST_PRINT("Info: data event #" << index << ": eDSGetBufferChunkData number of chunks = " << iNumChunks << std::endl);
                        }
                    }
                }
            }
        }
        else
        { 
            std::string oErrorMsg=oLibSysSetup.sGetLastErrorMessage();
            GENTLTEST_PRINT("Info: data event #" << index << ": eDSGetBufferInfo BUFFER_INFO_PAYLOADTYPE returned GC_ERR_NOT_IMPLEMENTED, error message:" << oErrorMsg.c_str() << std::endl);
        }
    }
    else
    {
        GENTLTEST_PRINT("Error: Frame#" << index << ": is incomplete." << std::endl);
    }

    return uiNumberOfChunksReceived;
}

