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

#include "DataStream_DSGetBufferInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "GenTLTestTools.h"
#include "LocalParser.h"

#include "SFNCPort.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DataStream_DSGetBufferInfo::DataStream_DSGetBufferInfo()
{
}

DataStream_DSGetBufferInfo::~DataStream_DSGetBufferInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSGetBufferInfo::setUp(void)
{
}

void DataStream_DSGetBufferInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DSGetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoConsumerBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSGetBufferInfo (using DSAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        void *pvPayLoad=NULL;
                        
                        oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer, &pvPayLoad);

                        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                std::vector<char> sBuffer(iSize);
                                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS);

                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                        ", DeviceID=" << sDeviceID.c_str() << 
                                        ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        ", DatastreamID=" << sDataStreamID.c_str() <<
                                        ") " << sConvertDataStreamBufferCommand2String(eCommand).c_str(),
                                        iType, &sBuffer[0], iSize);

                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                    switch (eCommand)
                                    {
                                    case BUFFER_INFO_BASE: {
                                        void *pBuffer=(void*)xConvertBuffer2PTR(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_BASE data, pBuffer != buffer pointer", pBuffer == pvPayLoad); 
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_BASE data, expected INFO_DATATYPE_PTR type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_PTR); 
                                        }
                                        break;
                                    case BUFFER_INFO_USER_PTR: {
                                        void *pUserPointer=(void*)xConvertBuffer2PTR(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_USER_PTR data, user pointer != input user pointer", pUserPointer == (void*)this); 
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_USER_PTR data, expected INFO_DATATYPE_PTR type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_PTR); 
                                        }
                                        break;
                                    case BUFFER_INFO_NEW_DATA:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_NEW_DATA data, expected INFO_DATATYPE_BOOL8 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    case BUFFER_INFO_IS_QUEUED:
                                    case BUFFER_INFO_IS_ACQUIRING:
                                    case BUFFER_INFO_IS_INCOMPLETE:
                                    case BUFFER_INFO_IMAGEPRESENT:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong data, expected INFO_DATATYPE_BOOL8 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    case BUFFER_INFO_SIZE: {
                                        size_t xBufferSize=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_SIZE data, buffer size != payload size", xBufferSize == uiPayLoadSize); 
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_SIZE data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_WIDTH: {
                                        size_t xWidth=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_WIDTH data, width = 0", xWidth != 0); 
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_WIDTH data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_HEIGHT: {
                                        size_t xHeight=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_HEIGHT data, height = 0", xHeight != 0); 
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_HEIGHT data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_SIZE_FILLED:
                                    case BUFFER_INFO_XOFFSET:
                                    case BUFFER_INFO_YOFFSET:
                                    case BUFFER_INFO_XPADDING:
                                    case BUFFER_INFO_YPADDING:
                                    case BUFFER_INFO_IMAGEOFFSET:
                                    case BUFFER_INFO_PAYLOADTYPE:
                                    case BUFFER_INFO_DELIVERED_IMAGEHEIGHT:
                                    case BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        break;
                                    case BUFFER_INFO_TIMESTAMP:
                                    case BUFFER_INFO_FRAMEID:
                                    case BUFFER_INFO_PIXELFORMAT:
                                    case BUFFER_INFO_CHUNKLAYOUTID:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                        break;
                                    case BUFFER_INFO_PIXELFORMAT_NAMESPACE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 

                                        if (iType == INFO_DATATYPE_UINT64)
                                        {
                                            uint64_t nameSpace = xConvertBuffer2UINT64(&sBuffer[0]);
                                            GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                                " wrong BUFFER_INFO_PIXELFORMAT_NAMESPACE, expected PIXELFORMAT_NAMESPACE_UNKNOWN, PIXELFORMAT_NAMESPACE_GEV, PIXELFORMAT_NAMESPACE_IIDC" <<
                                                ", PIXELFORMAT_NAMESPACE_PFNC_16BIT, PIXELFORMAT_NAMESPACE_PFNC_32BIT or PIXELFORMAT_NAMESPACE_CUSTOM_ID and higher, namespace ID=" << nameSpace,
                                                nameSpace == PIXELFORMAT_NAMESPACE_UNKNOWN || nameSpace == PIXELFORMAT_NAMESPACE_GEV || nameSpace == PIXELFORMAT_NAMESPACE_IIDC ||
                                                nameSpace == PIXELFORMAT_NAMESPACE_PFNC_16BIT || nameSpace == PIXELFORMAT_NAMESPACE_PFNC_32BIT || nameSpace >= PIXELFORMAT_NAMESPACE_CUSTOM_ID)
                                        }
                                        break;
                                    case BUFFER_INFO_TLTYPE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_TLTYPE datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sTLType=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                                " wrong BUFFER_INFO_TLTYPE tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                        }
                                        break;
                                    case BUFFER_INFO_FILENAME:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_FILENAME datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        break;
                                    case BUFFER_INFO_PIXEL_ENDIANNESS:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong data expected INFO_DATATYPE_INT32 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
                                            
                                        if (iType == INFO_DATATYPE_INT32)
                                        {
                                            int32_t endianness = xConvertBuffer2INT32(&sBuffer[0]);
                                            GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                                " wrong endianness value, expected PIXELENDIANNESS_UNKNOWN, PIXELENDIANNESS_LITTLE, PIXELENDIANNESS_BIG, value=" << sConvertEndianness2String(endianness), 
                                                endianness == PIXELENDIANNESS_UNKNOWN || endianness == PIXELENDIANNESS_LITTLE || endianness == PIXELENDIANNESS_BIG);
                                        }
                                        break;
                                    case BUFFER_INFO_DATA_SIZE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_DATA_SIZE datatype: expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        break;
                                    case BUFFER_INFO_TIMESTAMP_NS:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                        break;
                                    case BUFFER_INFO_DATA_LARGER_THAN_BUFFER:
                                    case BUFFER_INFO_CONTAINS_CHUNKDATA:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_BOOL8 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                if (Result == GC_ERR_NOT_IMPLEMENTED)
                                {
                                    GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Hint: allowed error: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoProducerBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSGetBufferInfo (using DSAllocAndAnnounceBuffer)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAllocAndAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                std::vector<char> sBuffer(iSize);
                                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS);

                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                        ", DeviceID=" << sDeviceID.c_str() << 
                                        ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        ", DatastreamID=" << sDataStreamID.c_str() <<
                                        ") " << sConvertDataStreamBufferCommand2String(eCommand).c_str(),
                                        iType, &sBuffer[0], iSize);

                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                    switch (eCommand)
                                    {
                                    case BUFFER_INFO_BASE:
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_BASE data, expected INFO_DATATYPE_PTR type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_PTR); 
                                        break;
                                    case BUFFER_INFO_USER_PTR: {
                                        void *pUserPointer=(void*)xConvertBuffer2PTR(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_USER_PTR data, user pointer != input user pointer", pUserPointer == (void*)this); 
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_USER_PTR data, expected INFO_DATATYPE_PTR type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_PTR); 
                                        }
                                        break;
                                    case BUFFER_INFO_NEW_DATA:
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_NEW_DATA data, expected INFO_DATATYPE_BOOL8 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    case BUFFER_INFO_IS_QUEUED:
                                    case BUFFER_INFO_IS_ACQUIRING:
                                    case BUFFER_INFO_IS_INCOMPLETE:
                                    case BUFFER_INFO_IMAGEPRESENT:
                                        GENTLTEST_CHECK_MESSAGE("Wrong data, expected INFO_DATATYPE_BOOL8 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    case BUFFER_INFO_SIZE: {
                                        size_t xBufferSize=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_SIZE data, buffer size != payload size", xBufferSize == uiPayLoadSize); 
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_SIZE data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_WIDTH: {
                                        size_t xWidth=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_WIDTH data, width = 0", xWidth != 0); 
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_WIDTH data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_HEIGHT: {
                                        size_t xHeight=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_HEIGHT data, height = 0", xHeight != 0); 
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_HEIGHT data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_SIZE_FILLED: {
                                        size_t xSizeFilled=(size_t)xConvertBuffer2SIZET(&sBuffer[0], iSize);
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_SIZE_FILLED data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        }
                                        break;
                                    case BUFFER_INFO_XOFFSET:
                                    case BUFFER_INFO_YOFFSET:
                                    case BUFFER_INFO_XPADDING:
                                    case BUFFER_INFO_YPADDING:
                                    case BUFFER_INFO_IMAGEOFFSET:
                                    case BUFFER_INFO_PAYLOADTYPE:
                                    case BUFFER_INFO_DELIVERED_IMAGEHEIGHT:
                                    case BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE:
                                        GENTLTEST_CHECK_MESSAGE("Wrong data expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        break;
                                    case BUFFER_INFO_TIMESTAMP:
                                    case BUFFER_INFO_FRAMEID:
                                    case BUFFER_INFO_PIXELFORMAT:
                                    case BUFFER_INFO_CHUNKLAYOUTID:
                                        GENTLTEST_CHECK_MESSAGE("Wrong datatype: expected INFO_DATATYPE_UINT64 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                        break;
                                    case BUFFER_INFO_PIXELFORMAT_NAMESPACE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 

                                        if (iType == INFO_DATATYPE_UINT64)
                                        {
                                            uint64_t nameSpace = xConvertBuffer2UINT64(&sBuffer[0]);
                                            GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                                " wrong BUFFER_INFO_PIXELFORMAT_NAMESPACE, expected PIXELFORMAT_NAMESPACE_UNKNOWN, PIXELFORMAT_NAMESPACE_GEV, PIXELFORMAT_NAMESPACE_IIDC" <<
                                                ", PIXELFORMAT_NAMESPACE_PFNC_16BIT, PIXELFORMAT_NAMESPACE_PFNC_32BIT or PIXELFORMAT_NAMESPACE_CUSTOM_ID and higher, namespace ID=" << nameSpace,
                                                nameSpace == PIXELFORMAT_NAMESPACE_UNKNOWN || nameSpace == PIXELFORMAT_NAMESPACE_GEV || nameSpace == PIXELFORMAT_NAMESPACE_IIDC ||
                                                nameSpace == PIXELFORMAT_NAMESPACE_PFNC_16BIT || nameSpace == PIXELFORMAT_NAMESPACE_PFNC_32BIT || nameSpace >= PIXELFORMAT_NAMESPACE_CUSTOM_ID)
                                        }
                                        break;
                                    case BUFFER_INFO_TLTYPE:
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_TLTYPE datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sTLType=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_TLTYPE tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                        }
                                        break;
                                    case BUFFER_INFO_FILENAME:
                                        GENTLTEST_CHECK_MESSAGE("Wrong BUFFER_INFO_FILENAME datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        break;
                                    case BUFFER_INFO_PIXEL_ENDIANNESS:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong data expected INFO_DATATYPE_INT32 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
                                            
                                        if (iType == INFO_DATATYPE_INT32)
                                        {
                                            int32_t endianness = xConvertBuffer2INT32(&sBuffer[0]);
                                            GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                                " wrong endianness value, expected PIXELENDIANNESS_UNKNOWN, PIXELENDIANNESS_LITTLE, PIXELENDIANNESS_BIG, value=" << sConvertEndianness2String(endianness), 
                                                endianness == PIXELENDIANNESS_UNKNOWN || endianness == PIXELENDIANNESS_LITTLE || endianness == PIXELENDIANNESS_BIG);
                                        }
                                        break;
                                    case BUFFER_INFO_DATA_SIZE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong BUFFER_INFO_DATA_SIZE datatype: expected INFO_DATATYPE_SIZET type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        break;
                                    case BUFFER_INFO_TIMESTAMP_NS:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                        break;
                                    case BUFFER_INFO_DATA_LARGER_THAN_BUFFER:
                                    case BUFFER_INFO_CONTAINS_CHUNKDATA:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamBufferCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_BOOL8 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                if (Result == GC_ERR_NOT_IMPLEMENTED)
                                {
                                    GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Hint: allowed error: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage("<<sConvertDataStreamBufferCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tDataStreamList vecDataStreamList;
    tDataStreamList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        stDataStream sDS=(stDataStream)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
        
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tDataStreamList vecDataStreamList;
    tDataStreamList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stDataStream sDS=(stDataStream)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tDataStreamList vecDataStreamList;
    tDataStreamList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stDataStream sDS=(stDataStream)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

            oIFPreCondition.vClose();
        
            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tDataStreamList vecDataStreamList;
    tDataStreamList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stDataStream sDS=(stDataStream)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

            oDevPreCondition.vClose();

            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with closed datastream before");
    LibrarySystemSetup oLibSysSetup;
    tDataStreamList vecDataStreamList;
    tDataStreamList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stDataStream sDS=(stDataStream)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
        
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

            oDSPreCondition.vClose();

            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED || == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(sDS.eCommand).c_str() <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with parameter piSize = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, NULL);
                            GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result < GC_ERR_SUCCESS);

                            if (Result < GC_ERR_SUCCESS)
                            {
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with piType = NULL at second call with initialized buffer");

    if (!g_SkipTypeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
        for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
        {
            uint32_t uiNumDevices;
            IF_HANDLE hIF = GENTL_INVALID_HANDLE;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

            uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

            for (uint32_t index2=0; index2<uiNumDevices; index2++)
            {
                std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

                for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
                {
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                        m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                        for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                        {
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                            for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                            {
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;

                                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    size_t iSizeSave=iSize;

                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    std::vector<char> sBuffer(iSize);
                                    Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, NULL, &sBuffer[0], &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER,
                                        "DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    if (Result < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithDSIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with datastream handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetBufferInfo(GENTL_INVALID_HANDLE, hBuffer, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result < GC_ERR_SUCCESS);

                            if (Result < GC_ERR_SUCCESS)
                            {
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with iSize = 0 at second call with initialized buffer");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                INFO_DATATYPE iTypeSave=iType;
                                size_t iSizeSave=iSize;

                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                std::vector<char> sBuffer(iSize);
                        
                                iSize = 0;
                                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                    Result == GC_ERR_INVALID_PARAMETER,
                                    "DSGetBufferInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result < GC_ERR_SUCCESS);

                                if (Result < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", 0 == iSize);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with piSize = NULL at second call with initialized buffer");
    
    if (!g_SkipSizeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
        for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
        {
            uint32_t uiNumDevices;
            IF_HANDLE hIF = GENTL_INVALID_HANDLE;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

            uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

            for (uint32_t index2=0; index2<uiNumDevices; index2++)
            {
                std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

                for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
                {
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                        m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                        for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                        {
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                            oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                            for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                            {
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;

                                Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    INFO_DATATYPE iTypeSave=iType;

                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    std::vector<char> sBuffer(iSize);
                        
                                    Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, eCommand, &iType, &sBuffer[0], NULL);
                                    GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER,
                                        "DSGetBufferInfo iInfoCmd=" << sConvertDataStreamBufferCommand2String(eCommand).c_str() <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    if (Result < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetBufferInfo::TestDSGetBufferInfoWithInvalidCommand( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetBufferInfo with parameter iInfoCmd = BUFFER_INFO_CONTAINS_CHUNKDATA+1");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBuffer(hDs, uiPayLoadSize, (void *)this, &hBuffer);

                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        
                        Result = m_ModDS.eDSGetBufferInfo(hDs, hBuffer, BUFFER_INFO_CONTAINS_CHUNKDATA+1, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: DSGetBufferInfo BUFFER_INFO_CONTAINS_CHUNKDATA+1 device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                            "DSGetBufferInfo BUFFER_INFO_CONTAINS_CHUNKDATA+1 device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void DataStream_DSGetBufferInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tDataStreamList &vecDataStreamList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        for (BUFFER_INFO_CMD eCommand=BUFFER_INFO_BASE; eCommand<=BUFFER_INFO_CONTAINS_CHUNKDATA; eCommand=(BUFFER_INFO_CMD)(eCommand+1))
                        {
                            stDataStream sDS;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
        
                            sDS.sInterfaceID = xInterfaceList[index1];
                            sDS.sDeviceID = sDeviceID;
                            sDS.eAccess = eAccess;
                            sDS.sDataStreamID = sDataStreamID;
                            sDS.eCommand = eCommand;
                            vecDataStreamList.push_back(sDS);
                        }
                    }
                }
            }
        }
    }

    //GENTLTEST_PRINT("DataStream_DSGetBufferInfo::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}

