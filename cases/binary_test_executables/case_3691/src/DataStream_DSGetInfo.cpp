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

#include "DataStream_DSGetInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DataStream_DSGetInfo::DataStream_DSGetInfo()
{
}

DataStream_DSGetInfo::~DataStream_DSGetInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSGetInfo::setUp(void)
{
}

void DataStream_DSGetInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DSGetInfo
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSGetInfo::TestDSGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSGetInfo");
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
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        int nCommandCounter=0;
                        
                        for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                            
                            Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                        
                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                nCommandCounter++;
                                std::vector<char> sBuffer(iSize);
                                Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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
                                        ") " << sConvertDataStreamCommand2String(eCommand).c_str(),
                                        iType, &sBuffer[0], iSize);

                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                    switch (eCommand)
                                    {
                                    case STREAM_INFO_TLTYPE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sTLType=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE("Wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                        }
                                        break;
                                    case STREAM_INFO_ID:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        break;
                                    case STREAM_INFO_NUM_DELIVERED:
                                    case STREAM_INFO_NUM_UNDERRUN:
                                    case STREAM_INFO_NUM_STARTED:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                        break;
                                    case STREAM_INFO_NUM_ANNOUNCED:
                                    case STREAM_INFO_NUM_QUEUED:
                                    case STREAM_INFO_NUM_AWAIT_DELIVERY:
                                    case STREAM_INFO_PAYLOAD_SIZE:
                                    case STREAM_INFO_NUM_CHUNKS_MAX:
                                    case STREAM_INFO_BUF_ANNOUNCE_MIN:
                                    case STREAM_INFO_BUF_ALIGNMENT:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_SIZET type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
                                        break;
                                    case STREAM_INFO_IS_GRABBING:
                                    case STREAM_INFO_DEFINES_PAYLOADSIZE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertDataStreamCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_BOOL8 type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
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
                                        " LastErrorMessage("<<sConvertDataStreamCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage("<<sConvertDataStreamCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                            }
                        }

                        if (nCommandCounter == 0)
                        {
                            GENTLTEST_CHECK_MESSAGE("DSGetInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " failed. No commands implemented.", false);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSGetInfo::TestDSGetInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with closed library before");
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
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDS.eDSGetInfo(hDs, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with closed system before");
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
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDS.eDSGetInfo(hDs, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with closed interface before");
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
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);

            oIFPreCondition.vClose();
        
            Result = m_ModDS.eDSGetInfo(hDs, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with closed device before");
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
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);

            oDevPreCondition.vClose();

            Result = m_ModDS.eDSGetInfo(hDs, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with closed datastream before");
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
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);

            oDSPreCondition.vClose();

            Result = m_ModDS.eDSGetInfo(hDs, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DSGetInfo size check command=" << sConvertDataStreamCommand2String(sDS.eCommand).c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with parameter piSize = NULL");
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
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                        for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, NULL, NULL);
                            GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                "DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with piType = NULL at second call with initialized buffer");
    
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
                    
                        for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                        {
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                            for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                            {
                                GC_ERROR Result=GC_ERR_SUCCESS;
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;

                                Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                        
                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    INFO_DATATYPE iTypeSave=iType;
                                    size_t iSizeSave=iSize;

                                    std::vector<char> sBuffer(iSize);
                                    Result = m_ModDS.eDSGetInfo(hDs, eCommand, NULL, &sBuffer[0], &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER,
                                        "DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    if (Result < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
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

void DataStream_DSGetInfo::TestDSGetInfoWithDSIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with datastream handle = GENTL_INVALID_HANDLE");
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
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                        for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetInfo(GENTL_INVALID_HANDLE, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_AVAILABLE,
                                "DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with iSize = 0 at second call with initialized buffer");
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
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                        for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;

                            Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                        
                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                INFO_DATATYPE iTypeSave=iType;
                                size_t iSizeSave=iSize;

                                std::vector<char> sBuffer(iSize);
                                iSize = 0;
                                Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_RESULT("Note: DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                    Result == GC_ERR_INVALID_PARAMETER,
                                    "DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with piSize = NULL at second call with initialized buffer");
    
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
                    
                        for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                        {
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                            for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                            {
                                GC_ERROR Result=GC_ERR_SUCCESS;
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;

                                Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_MESSAGE("DSGetInfo size check command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                        
                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    INFO_DATATYPE iTypeSave=iType;
                                    size_t iSizeSave=iSize;

                                    std::vector<char> sBuffer(iSize);
                                    Result = m_ModDS.eDSGetInfo(hDs, eCommand, &iType, &sBuffer[0], NULL);
                                    GENTLTEST_CHECK_RESULT("Note: DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER,
                                        "DSGetInfo with buffer command=" << sConvertDataStreamCommand2String(eCommand).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
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

void DataStream_DSGetInfo::TestDSGetInfoWithInvalidCommand( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSGetInfo with parameter iInfoCmd = STREAM_INFO_BUF_ALIGNMENT+1");
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
                        
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        size_t iSize=0;

                        Result = m_ModDS.eDSGetInfo(hDs, STREAM_INFO_BUF_ALIGNMENT+1, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: DSGetInfo size check command=STREAM_INFO_BUF_ALIGNMENT+1" << 
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                            "DSGetInfo size check command=STREAM_INFO_BUF_ALIGNMENT+1" << 
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
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

void DataStream_DSGetInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tDataStreamList &vecDataStreamList)
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
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                        for (STREAM_INFO_CMD_LIST eCommand=STREAM_INFO_ID; eCommand<=STREAM_INFO_BUF_ALIGNMENT; eCommand=(STREAM_INFO_CMD_LIST)(eCommand+1))
                        {
                            stDataStream sDS;
        
                            sDS.sInterfaceID = xInterfaceList[index1];
                            sDS.sDeviceID = sDeviceID;
                            sDS.eAccess = eAccess;
                            sDS.eCommand = eCommand;
                            sDS.sDataStreamID = sDataStreamID;
                            vecDataStreamList.push_back(sDS);
                        }
                    }       
                }
            }
        }
    }

    //GENTLTEST_PRINT("DataStream_DSGetInfo::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
