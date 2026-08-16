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

#include <vector>
#include "GenApi/GenApi.h"

#include "DataStream_PreCondition.h"
#include "GenTLTestParameter.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// definition section
////////////////////////////////////////////////////////////////////////////////////////////

extern uint8_t g_bReusePayloadsize;

DataStream_PreCondition::eLastPayloadSizeSource DataStream_PreCondition::m_eLastPayloadSizeSource=LAST_PAYLOADSIZE_SOURCE_NA;
size_t DataStream_PreCondition::m_uiFoundPayloadSize=0;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DataStream_PreCondition::DataStream_PreCondition(GenICam::Client::DEV_HANDLE hDev, 
                                         std::string &sDataStreamID, 
                                         GenICam::Client::DS_HANDLE *hDs)
: m_hDs(GENTL_INVALID_HANDLE)
, m_LastResult(GC_ERR_SUCCESS)
{
    std::string sMsg;
        
    m_LastResult = m_ModDev.eDevOpenDataStream(hDev, sDataStreamID.c_str(), &m_hDs);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream returned result=" << sConvertGCError2String(m_LastResult).c_str() <<
        " expected result >= GC_ERR_SUCCESS || result == GC_ERR_RESOURCE_IN_USE, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_RESOURCE_IN_USE);

    *hDs = m_hDs;
}

DataStream_PreCondition::~DataStream_PreCondition(void)
{
    vClose();

    for (size_t i=0; i<m_vecPayloadList.size(); i++)
    {
        free(m_vecPayloadList[i]);
    }
}

GC_ERROR DataStream_PreCondition::eGetLastResult()
{
	return m_LastResult;
}

void DataStream_PreCondition::vClose()
{
    m_ModDS.eDSClose(m_hDs);
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSAnnounceBuffer(GenICam::Client::DS_HANDLE hDs, 
                                                                     size_t uiPayLoadSize, 
                                                                     void *pvPrivate, 
                                                                     GenICam::Client::BUFFER_HANDLE *hBuffer,
                                                                     void **ppvPayLoad)
{
    std::string sMsg;
    void *pvPayLoad = malloc(uiPayLoadSize);

    memset(pvPayLoad, 0xAA, uiPayLoadSize);

    m_LastResult = m_ModDS.eDSAnnounceBuffer(hDs, pvPayLoad, uiPayLoadSize, pvPrivate, hBuffer);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
        free(pvPayLoad);
    }
    else 
    {
        m_vecPayloadList.push_back(pvPayLoad);
        if (ppvPayLoad != NULL)
        {
            *ppvPayLoad = pvPayLoad;
        }
    }

    GENTLTEST_REQUIRE_MESSAGE("DSAnnounceBuffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSQueueBuffer(GenICam::Client::DS_HANDLE hDs, 
                                                                  GenICam::Client::BUFFER_HANDLE hBuffer)
{
    std::string sMsg;
    
    m_LastResult = m_ModDS.eDSQueueBuffer(hDs, hBuffer);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DSQueueBuffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSAnnounceBufferAndQueue(GenICam::Client::DS_HANDLE hDs, 
                                                                            size_t uiPayLoadSize, 
                                                                            void *pvPrivate, 
                                                                            GenICam::Client::BUFFER_HANDLE *hBuffer,
                                                                            void **ppvPayLoad)
{
    m_LastResult = eDSAnnounceBuffer(hDs, uiPayLoadSize, pvPrivate, hBuffer, ppvPayLoad);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        m_LastResult = eDSQueueBuffer(hDs, *hBuffer);
    }

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSAllocAndAnnounceBufferAndQueue(GenICam::Client::DS_HANDLE hDs, 
                                                                                    size_t uiPayLoadSize, 
                                                                                    void *pvPrivate, 
                                                                                    GenICam::Client::BUFFER_HANDLE *hBuffer)
{
    m_LastResult = eDSAllocAndAnnounceBuffer(hDs, uiPayLoadSize, pvPrivate, hBuffer);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        m_LastResult = eDSQueueBuffer(hDs, *hBuffer);
    }

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSStartAcquisition(GenICam::Client::DS_HANDLE hDs, 
                                                                       uint64_t uiNumToAcquire)
{
    std::string sMsg;

    m_LastResult = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, uiNumToAcquire);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DSStartAcquisition (ACQ_START_FLAGS_DEFAULT) failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSStopAcquisition(GenICam::Client::DS_HANDLE hDs, 
                                                                       GenICam::Client::ACQ_STOP_FLAGS_LIST eStopFlag)
{
    std::string sMsg;

    m_LastResult = m_ModDS.eDSStopAcquisition(hDs, eStopFlag);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DSStopAcquisition stopFlag=" << sConvertDataStreamStopFlag2String(eStopFlag) << 
        " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSFlushQueue(GenICam::Client::DS_HANDLE hDs, 
                                                                   GenICam::Client::ACQ_QUEUE_TYPE_LIST eOperation)
{
    std::string sMsg;

    m_LastResult = m_ModDS.eDSFlushQueue(hDs, eOperation);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DSFlushQueue operation=" << sConvertDataStreamOperation2String(eOperation).c_str() <<
        " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSRevokeBuffer(GenICam::Client::DS_HANDLE hDs, 
                                                                   GenICam::Client::BUFFER_HANDLE hBuffer,
                                                                   void **pvBuffer,
                                                                   void **pvPrivateData)
{
    std::string sMsg;

    m_LastResult = m_ModDS.eDSRevokeBuffer(hDs, hBuffer, pvBuffer, pvPrivateData);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_CHECK_MESSAGE("DSRevokeBuffer " <<
        " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

size_t DataStream_PreCondition::uiGetPayLoadListSize()
{
    return m_vecPayloadList.size();
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eDSAllocAndAnnounceBuffer(GenICam::Client::DS_HANDLE hDs, 
                                                                            size_t uiPayLoadSize, 
                                                                            void *pvPrivate, 
                                                                            GenICam::Client::BUFFER_HANDLE *hBuffer)
{
    std::string sMsg;

    m_LastResult = m_ModDS.eDSAllocAndAnnounceBuffer(hDs, uiPayLoadSize, pvPrivate, hBuffer);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DSAllocAndAnnounceBuffer size=" << uiPayLoadSize << 
        " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

size_t DataStream_PreCondition::uiGetPayLoadSize(LibrarySystemSetup &oLibSysSetup,
                                                 GenICam::Client::DS_HANDLE hDs,
                                                 GenICam::Client::PORT_HANDLE hPort)
{
    size_t uiResult=0;
    std::string sMsg;
    INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
    size_t iSize=1;
    bool bPayloadSizeAvailable=false;

    // for speedup development purpose it is possible to reuse a already found payloadsize
    if (g_bReusePayloadsize && m_uiFoundPayloadSize > 0)
        return m_uiFoundPayloadSize;

    // get info if payload size is available
    m_ModDS.eDSGetInfo(hDs, STREAM_INFO_DEFINES_PAYLOADSIZE, &iType, &bPayloadSizeAvailable, &iSize);

    if (bPayloadSizeAvailable)
    {
        // yes, it is available
        size_t iSize=0;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;

        // get payload size DSGetInfo(STREAM_INFO_PAYLOAD_SIZE) size
        m_LastResult = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_PAYLOAD_SIZE, &iType, NULL, &iSize);

        if (m_LastResult < GC_ERR_SUCCESS)
        {
            sMsg = sGetLastErrorMessage();
            GENTLTEST_PRINT("Error: DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_DEFINES_PAYLOADSIZE) returned payloadsize is available"<<std::endl);
            GENTLTEST_PRINT("       but DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_PAYLOAD_SIZE, &iType, NULL, &iSize) returned "<<sConvertGCError2String(m_LastResult).c_str()<<std::endl);
        }

        GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_PAYLOAD_SIZE) size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
            ", error message ='" << sMsg.c_str() << "'", 
            m_LastResult >= GC_ERR_SUCCESS);

        if (m_LastResult >= GC_ERR_SUCCESS)
        {
            // get payload size DSGetInfo(STREAM_INFO_PAYLOAD_SIZE) value
            m_LastResult = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_PAYLOAD_SIZE, &iType, &uiResult, &iSize);

            if (m_LastResult < GC_ERR_SUCCESS)
            {
                sMsg = sGetLastErrorMessage();
                GENTLTEST_PRINT("Error: DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_DEFINES_PAYLOADSIZE) returned payloadsize is available"<<std::endl);
                GENTLTEST_PRINT("       but DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_PAYLOAD_SIZE, &iType, &uiResult, &iSize) returned "<<sConvertGCError2String(m_LastResult).c_str()<<std::endl);
            }

            GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_PAYLOAD_SIZE) with buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
                ", error message ='" << sMsg.c_str() << "'", 
                m_LastResult >= GC_ERR_SUCCESS);

            // we only want to see the payloadsize source changes
            if (LAST_PAYLOADSIZE_SOURCE_DSGETINFO != m_eLastPayloadSizeSource)
            {
                GENTLTEST_PRINT("Info: PayloadSize returned by DSGetInfo, PayloadSize=0x" << std::hex << uiResult << "=" << std::dec << uiResult << std::endl);
                m_eLastPayloadSizeSource = LAST_PAYLOADSIZE_SOURCE_DSGETINFO;
            }

            // this is the end of the complete PayloadSize discovery
            GENTLTEST_REQUIRE_MESSAGE("Major problem after calling DSGetInfo: PayloadSize is 0 (last call was DSGetInfo(STREAM_INFO_PAYLOAD_SIZE) processing is not possible.", uiResult > 0);

            m_uiFoundPayloadSize = uiResult;
        }
    }
    else
    {
        // no, it is not available by DSGetInfo
        // try to get payload size by DataStream XML node PayloadSize
        ocGenTLTestParameter oParameterDataStream;

        oParameterDataStream.bPrepareDataStreamXML(oLibSysSetup, hDs);
        GenApi::CIntegerPtr nValue1=oParameterDataStream.xGetXMLNodeInteger("PayloadSize", false);
        if (!nValue1.IsValid())
        {
            // failed
            // try to get payload size by RemoteDevice XML PayloadSize
            ocGenTLTestParameter oParameterRemoteDevice;

            oParameterRemoteDevice.bPrepareRemoteDeviceXML(oLibSysSetup, hPort);
            GenApi::CIntegerPtr nValue2=oParameterRemoteDevice.xGetXMLNodeInteger("PayloadSize", false);
            if (!nValue2.IsValid())
            {
                // this is the end of the complete PayloadSize discovery
                GENTLTEST_REQUIRE_MESSAGE("Major problem: did not get PayloadSize by DSGetInfo nor by DataStream XML nor by RemoteDevice XML, processing is not possible.", false);
            }
            else
            {
                uiResult = (size_t)nValue2->GetValue();

                // we only want to see the source changes
                if (LAST_PAYLOADSIZE_SOURCE_XMLREMOTEDEVICE != m_eLastPayloadSizeSource)
                {
                    GENTLTEST_PRINT("Info: RemoteDevice XML node 'PayloadSize'=0x" << std::hex << uiResult << "=" << std::dec << uiResult << std::endl);
                    m_eLastPayloadSizeSource = LAST_PAYLOADSIZE_SOURCE_XMLREMOTEDEVICE;
                }

                // this is the end of the complete PayloadSize discovery
                GENTLTEST_REQUIRE_MESSAGE("Major problem: RemoteDevice XML returned PayloadSize=0x"<<std::hex<<uiResult<<std::dec<<"="<<uiResult<<", processing is not possible.", uiResult > 0);

                m_uiFoundPayloadSize = uiResult;
            }
        }
        else
        {
            // got payload size by DataStream XML PayloadSize
            uiResult = (size_t)nValue1->GetValue();

            // we only want to see the source changes
            if (LAST_PAYLOADSIZE_SOURCE_XMLDATASTREAM != m_eLastPayloadSizeSource)
            {
                GENTLTEST_PRINT("Info: DataStream XML node 'PayloadSize'=0x" << std::hex << uiResult << "=" << std::dec << uiResult << std::endl);
                m_eLastPayloadSizeSource = LAST_PAYLOADSIZE_SOURCE_XMLDATASTREAM;
            }

            // this is the end of the complete PayloadSize discovery
            GENTLTEST_REQUIRE_MESSAGE("Major problem: DataStream XML returned PayloadSize=0x"<<std::hex<<uiResult<<std::dec<<"="<<uiResult<<", processing is not possible.", uiResult > 0);

            m_uiFoundPayloadSize = uiResult;
        }
    }

    return uiResult;
}

size_t DataStream_PreCondition::uiGetPayLoadSizeByDSGetInfo(GenICam::Client::DS_HANDLE hDs)
{
    size_t uiResult=0;
    std::string sMsg;
    INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
    size_t iSize=1;
    bool bPayloadSizeAvailable=false;

    m_ModDS.eDSGetInfo(hDs, STREAM_INFO_DEFINES_PAYLOADSIZE, &iType, &bPayloadSizeAvailable, &iSize);

    if (bPayloadSizeAvailable)
    {
        size_t iSize=0;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;

        m_LastResult = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_PAYLOAD_SIZE, &iType, NULL, &iSize);

        if (m_LastResult < GC_ERR_SUCCESS)
        {
            sMsg = sGetLastErrorMessage();
            GENTLTEST_PRINT("Error: DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_DEFINES_PAYLOADSIZE) returned payloadsize is available"<<std::endl);
            GENTLTEST_PRINT("       but DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_PAYLOAD_SIZE, &iType, NULL, &iSize) returned "<<sConvertGCError2String(m_LastResult).c_str()<<std::endl);
        }

        GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_PAYLOAD_SIZE) size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
            ", error message ='" << sMsg.c_str() << "'", 
            m_LastResult >= GC_ERR_SUCCESS);

        if (m_LastResult >= GC_ERR_SUCCESS)
        {
            m_LastResult = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_PAYLOAD_SIZE, &iType, &uiResult, &iSize);

            if (m_LastResult < GC_ERR_SUCCESS)
            {
                sMsg = sGetLastErrorMessage();
                GENTLTEST_PRINT("Error: DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_DEFINES_PAYLOADSIZE) returned payloadsize is available"<<std::endl);
                GENTLTEST_PRINT("       but DSGetInfo(hDs=0x"<<std::hex<<hDs<<", STREAM_INFO_PAYLOAD_SIZE, &iType, &uiResult, &iSize) returned "<<sConvertGCError2String(m_LastResult).c_str()<<std::endl);
            }

            GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_PAYLOAD_SIZE) with buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
                ", error message ='" << sMsg.c_str() << "'", 
                m_LastResult >= GC_ERR_SUCCESS);

            //GENTLTEST_PRINT("Info: DSGetInfo PayloadSize = 0x" << std::hex << uiResult << std::dec << std::endl);

            GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_PAYLOAD_SIZE) returned PayloadSize = 0, processing is not possible.", uiResult > 0);
        }
    }
    
    return uiResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eLockParameter(ocGenTLTestParameter &oParameter, bool bLock)
{
    GenApi::CIntegerPtr oTLParamsLocked1 = oParameter.xGetXMLNodeInteger("TLParamsLocked");
    if (oTLParamsLocked1.IsValid())
    {
        try 
        {
            if (bLock)
                oTLParamsLocked1->SetValue(1);
            else 
                oTLParamsLocked1->SetValue(0);

            m_LastResult = GC_ERR_SUCCESS;
        }
        catch (GenICam::AccessException ex)
        {
            GENTLTEST_REQUIRE_MESSAGE("Error: access problem for IInteger node='TLParamsLocked', details=" << ex.GetDescription() << std::endl, false);
            m_LastResult = GC_ERR_ERROR;
        }
    }
    else
    {
        GenApi::CBooleanPtr oTLParamsLocked2 = oParameter.xGetXMLNodeBoolean("TLParamsLocked");
        if (oTLParamsLocked2.IsValid())
        {
            try 
            {
                if (bLock)
                {
                    oTLParamsLocked2->SetValue(1);
                    GENTLTEST_PRINT("Info: set old style IBoolean node TLParamsLocked to true OK." << std::endl);
                }
                else 
                {
                    oTLParamsLocked2->SetValue(0);
                    GENTLTEST_PRINT("Info: set old style IBoolean node TLParamsLocked to false OK." << std::endl);
                }
                m_LastResult = GC_ERR_SUCCESS;
            }
            catch (GenICam::AccessException ex)
            {
                GENTLTEST_REQUIRE_MESSAGE("Error: access problem IBoolean node='TLParamsLocked', details=" << ex.GetDescription() << std::endl, false);
                m_LastResult = GC_ERR_ERROR;
            }
        }
        else
        {
            GENTLTEST_REQUIRE_MESSAGE("Device TLParamsLocked not valid, node (neither Interger nor Boolean) not found", false);
            m_LastResult = GC_ERR_ERROR;
        }
    }
    
    return m_LastResult;
}

GenICam::Client::GC_ERROR DataStream_PreCondition::eSetChunkModeActive(ocGenTLTestParameter &oParameter)
{
    GenApi::CBooleanPtr oChunkMode = oParameter.xGetXMLNodeBoolean("ChunkModeActive");
    if (oChunkMode.IsValid())
    {
        try 
        {
            oChunkMode->SetValue(1);
            m_LastResult = GC_ERR_SUCCESS;
        }
        catch (GenICam::AccessException ex)
        {
            GENTLTEST_REQUIRE_MESSAGE("Error: access problem node='ChunkModeActive', details=" << ex.GetDescription() << std::endl, false);
            m_LastResult = GC_ERR_ERROR;
        }
    }

    return m_LastResult;
}

size_t DataStream_PreCondition::uiDSGetInfoMinBuffers(GenICam::Client::DS_HANDLE hDs)
{
    size_t uiResult=0;
    std::string sMsg;
    INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
    size_t iSize=0;

    m_LastResult = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_BUF_ANNOUNCE_MIN, &iType, NULL, &iSize);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_BUF_ANNOUNCE_MIN) size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        m_LastResult = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_BUF_ANNOUNCE_MIN, &iType, &uiResult, &iSize);

        GENTLTEST_REQUIRE_MESSAGE("DSGetInfo (STREAM_INFO_BUF_ANNOUNCE_MIN) with buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str() <<
            ", error message ='" << sMsg.c_str() << "'", 
            m_LastResult >= GC_ERR_SUCCESS);

        GENTLTEST_REQUIRE_MESSAGE("Minnimum buffers == 0", uiResult > 0);
    }

    return uiResult;
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

std::string DataStream_PreCondition::sGetLastErrorMessage()
{
    std::string sMsg;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t zSize=0;
    
    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
        eResult >= GC_ERR_SUCCESS);

    if (eResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after GCGetLastError call iSize == 0", zSize > 0);

        if (zSize > 0)
        {
            std::vector<char> sBuffer(zSize);
            eResult = m_ModGC.eGCGetLastError(&eErrorCode, &sBuffer[0], &zSize);
            GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
                eResult >= GC_ERR_SUCCESS);

            if (eResult >= GC_ERR_SUCCESS) 
            {
                sMsg = &sBuffer[0];
            }
        }
    }

    return sMsg;
}
