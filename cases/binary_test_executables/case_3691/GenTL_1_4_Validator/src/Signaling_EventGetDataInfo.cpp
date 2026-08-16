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

#include "Signaling_EventGetDataInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "Signaling_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Signaling_EventGetDataInfo::Signaling_EventGetDataInfo()
{
}

Signaling_EventGetDataInfo::~Signaling_EventGetDataInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_EventGetDataInfo::setUp(void)
{
}

void Signaling_EventGetDataInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test EventGetDataInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_EventGetDataInfo::TestEventGetDataInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard EventGetDataInfo");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        uint32_t nBufferCount=10;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

                        for (uint32_t i=0; i<nBufferCount; i++)
                        {
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        }

                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
                            
                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            size_t iDataSize=0;
                            uint64_t uiTimeOut=1000;
                            
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);
                                
                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    GC_ERROR Result=GC_ERR_SUCCESS;
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_MESSAGE("EventGetDataInfo size check command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=EVENT_NEW_BUFFER" <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                    if (Result >= GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                        std::vector<char> sBuffer(iSize);

                                        nNumberOfFunctionTests++;

                                        Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, &sBuffer[0], &iSize);
                                        GENTLTEST_CHECK_MESSAGE("EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                            " eventID=EVENT_NEW_BUFFER" <<
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
                                                ", datastreamID=" << sDataStreamID.c_str() <<
                                                ", eventID=EVENT_NEW_BUFFER" << 
                                                ") " << sConvertEventDataInfoCommand2String(eCommand).c_str(),
                                                iType, &sBuffer[0], iSize);

                                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);
                                        }
                                    }
                                    else
                                    {
                                        std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                        if (Result == GC_ERR_NOT_IMPLEMENTED)
                                        {
                                            GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                                                " LastErrorMessage(" << sConvertEventDataInfoCommand2String(eCommand).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                                        }
                                        else
                                        {
                                            GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                                " LastErrorMessage(" << sConvertEventDataInfoCommand2String(eCommand).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                                        }
                                    }
                                }
                            }
                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested, no commands implemented." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    int nNumberOfFunctionTests=0;

    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        stSignaling sDS=(stSignaling)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            size_t iDataSize=0;
            uint64_t uiTimeOut=1000;
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

            void *pvData=malloc(iDataSize);

            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
            {
                oLibSysSetup.tearDownLibrary();
        
                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, sDS.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
        
                nNumberOfFunctionTests++;

                oLibSysSetup.tearDownSystem();
                oLibSysSetup.setUp();
            }

            free(pvData);
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    int nNumberOfFunctionTests=0;

    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stSignaling sDS=(stSignaling)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            size_t iDataSize=0;
            uint64_t uiTimeOut=1000;
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

            void *pvData=malloc(iDataSize);

            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
            {
                oLibSysSetup.tearDownSystem();

                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, sDS.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                nNumberOfFunctionTests++;

                oLibSysSetup.setUpSystem();
            }

            free(pvData);
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    int nNumberOfFunctionTests=0;

    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stSignaling sDS=(stSignaling)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            size_t iDataSize=0;
            uint64_t uiTimeOut=1000;
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

            void *pvData=malloc(iDataSize);

            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
            {
                oIFPreCondition.vClose();

                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, sDS.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                nNumberOfFunctionTests++;
            }

            free(pvData);
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    int nNumberOfFunctionTests=0;

    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stSignaling sDS=(stSignaling)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            size_t iDataSize=0;
            uint64_t uiTimeOut=1000;
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

            void *pvData=malloc(iDataSize);

            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
            {
                oDevPreCondition.vClose();

                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, sDS.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                nNumberOfFunctionTests++;
            }

            free(pvData);
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with closed datastream before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    int nNumberOfFunctionTests=0;

    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stSignaling sDS=(stSignaling)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            size_t iDataSize=0;
            uint64_t uiTimeOut=1000;
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

            void *pvData=malloc(iDataSize);

            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
            {
                oDSPreCondition.vClose();

                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, sDS.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                nNumberOfFunctionTests++;
            }

            free(pvData);
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithoutRegisterEvent( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with unregistered event before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    int nNumberOfFunctionTests=0;

    vCreateTestCaseList(oLibSysSetup, vecDataStreamList);

    for (xIter=vecDataStreamList.begin(); xIter!=vecDataStreamList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stSignaling sDS=(stSignaling)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), sDS.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, sDS.sDeviceID, sDS.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            size_t iDataSize=0;
            uint64_t uiTimeOut=1000;
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

            void *pvData=malloc(iDataSize);

            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
            {
                oSigPreCondition.eGCUnregisterEvent(hDs, EVENT_NEW_BUFFER);

                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, sDS.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(sDS.eCommand).c_str() << 
                    " eventID=EVENT_NEW_BUFFER" << 
                    " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                    " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                    " DeviceID=" << sDS.sDeviceID.c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                nNumberOfFunctionTests++;
            }

            free(pvData);
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithEventHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with event handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(GENTL_INVALID_HANDLE, pvData, iDataSize, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=EVENT_NEW_BUFFER" <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=EVENT_NEW_BUFFER" <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    nNumberOfFunctionTests++;
                                }
                            }

                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithInvalidCommand( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter iInfoCmd = EVENT_DATA_NUMID+1");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                    
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                        
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                            
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                
                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                INFO_DATATYPE iType=0;
                                size_t iSize=0;

                                Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, EVENT_DATA_NUMID+1, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=EVENT_DATA_NUMID+1" << 
                                    " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                    "EventGetDataInfo command=EVENT_DATA_NUMID+1" << 
                                    " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result < GC_ERR_SUCCESS);

                                nNumberOfFunctionTests++;
                            }
                                    
                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithTypeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter piType = NULL at second call with initialized buffer");
    
    if (!g_SkipTypeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
        int nNumberOfFunctionTests=0;

        for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
        {
            uint32_t uiNumDevices;
            IF_HANDLE hIF = GENTL_INVALID_HANDLE;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

            uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

            for (uint32_t index2=0; index2<uiNumDevices; index2++)
            {
                std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

                for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
                {
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                        m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                        for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                        {
                            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                            oDSPreCondition.eLockParameter(m_oParameter, true);
                            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                            size_t iDataSize=0;
                            uint64_t uiTimeOut=1000;
                            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                oSigPreCondition.eStartDevice(m_oParameter);
                                oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                                void *pvData=malloc(iDataSize);

                                if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                                {
                                    for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                    {
                                        INFO_DATATYPE iType=0;
                                        size_t iSize=0;

                                        Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, NULL, &iSize);
                                        GENTLTEST_CHECK_MESSAGE("EventGetDataInfo size check command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                            " eventID=EVENT_NEW_BUFFER" <<
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                        if (Result >= GC_ERR_SUCCESS)
                                        {
                                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                            std::vector<char> sBuffer(iSize);

                                            Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, NULL, &sBuffer[0], &iSize);
                                            GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                                " eventID=EVENT_NEW_BUFFER" <<
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED,
                                                "EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                                " eventID=EVENT_NEW_BUFFER" <<
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                                Result < GC_ERR_SUCCESS);
                                        }

                                        nNumberOfFunctionTests++;
                                    }
                                }

                                free(pvData);
                            }
                        }
                    }
                }
            }
        }

        if (nNumberOfFunctionTests == 0)
            GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithSizeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter piSize = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        
        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, NULL, NULL);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=EVENT_NEW_BUFFER" <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=EVENT_NEW_BUFFER" <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    nNumberOfFunctionTests++;
                                }
                            }
                                
                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter iSize = 0 at second call with initialized buffer");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_MESSAGE("EventGetDataInfo size check command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=EVENT_NEW_BUFFER" <<
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

                                    if (Result >= GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                        std::vector<char> sBuffer(iSize);

                                        iSize = 0;
                                        Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, &sBuffer[0], &iSize);
                                        GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                            " eventID=EVENT_NEW_BUFFER" <<
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                            "EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                            " eventID=EVENT_NEW_BUFFER" <<
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                            Result < GC_ERR_SUCCESS);
                                    }

                                    nNumberOfFunctionTests++;
                                }
                            }

                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter piSize = NULL at second call with initialized buffer");
    
    if (!g_SkipSizeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
        int nNumberOfFunctionTests=0;

        for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
        {
            uint32_t uiNumDevices;
            IF_HANDLE hIF = GENTL_INVALID_HANDLE;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

            uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

            for (uint32_t index2=0; index2<uiNumDevices; index2++)
            {
                std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

                for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
                {
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                        m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                        for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                        {
                            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                            oDSPreCondition.eLockParameter(m_oParameter, true);
                            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                            size_t iDataSize=0;
                            uint64_t uiTimeOut=1000;
                            Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                oSigPreCondition.eStartDevice(m_oParameter);
                                oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                                void *pvData=malloc(iDataSize);

                                if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                                {
                                    for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                    {
                                        INFO_DATATYPE iType=0;
                                        size_t iSize=0;

                                        Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, NULL, &iSize);
                                        GENTLTEST_CHECK_MESSAGE("EventGetDataInfo size check command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                            " eventID=EVENT_NEW_BUFFER" <<
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                        if (Result >= GC_ERR_SUCCESS)
                                        {
                                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                            std::vector<char> sBuffer(iSize);

                                            Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize, eCommand, &iType, &sBuffer[0], NULL);
                                            GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                                " eventID=EVENT_NEW_BUFFER" <<
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED,
                                                "EventGetDataInfo with initialized buffer command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                                " eventID=EVENT_NEW_BUFFER" <<
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                                Result < GC_ERR_SUCCESS);
                                        }

                                        nNumberOfFunctionTests++;
                                    }
                                }

                                free(pvData);
                            }
                        }
                    }
                }
            }
        }

        if (nNumberOfFunctionTests == 0)
            GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }
    
    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithDataBufferNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter pInBuffer = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(hEvent, NULL, iDataSize, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    nNumberOfFunctionTests++;
                                }
                            }

                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithDataBufferSize0( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter iInSize = 0");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, 0, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    nNumberOfFunctionTests++;
                                }
                            }

                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetDataInfo::TestEventGetDataInfoWithDataBufferSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetDataInfo with parameter iInSize = iInSize-1");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nNumberOfFunctionTests=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
                        size_t iDataSize=0;
                        uint64_t uiTimeOut=1000;
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);

                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            oSigPreCondition.eStartDevice(m_oParameter);
                            oSigPreCondition.eEventGetInfoSizeMax(hEvent, &iDataSize);

                            void *pvData=malloc(iDataSize);

                            if ( GC_ERR_SUCCESS == oSigPreCondition.eEventGetData(hEvent, pvData, &iDataSize, uiTimeOut) )
                            {
                                for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;

                                    Result = m_ModEvent.eEventGetDataInfo(hEvent, pvData, iDataSize-1, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetDataInfo command=" << sConvertEventDataInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    nNumberOfFunctionTests++;
                                }
                            }

                            free(pvData);
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetDataInfo not tested due to invalid preconditions." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Signaling_EventGetDataInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tSignalingList &vecDataStreamList)
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

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
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
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        // we only want to test EVENT_NEW_BUFFER because thats the only event we can trigger for sure
                        Signaling_PreCondition oSigPreCondition(hDs, EVENT_NEW_BUFFER, &hEvent);
                            
                        if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                        {
                            for (EVENT_DATA_INFO_CMD eCommand=EVENT_DATA_ID; eCommand<=EVENT_DATA_NUMID; eCommand=(EVENT_DATA_INFO_CMD)(eCommand+1))
                            {
                                stSignaling sDS;
            
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
    }

    //GENTLTEST_PRINT("Signaling_EventGetDataInfo::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
