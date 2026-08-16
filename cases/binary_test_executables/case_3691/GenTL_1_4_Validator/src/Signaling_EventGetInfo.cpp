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

#include "Signaling_EventGetInfo.h"
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

Signaling_EventGetInfo::Signaling_EventGetInfo()
{
}

Signaling_EventGetInfo::~Signaling_EventGetInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_EventGetInfo::setUp(void)
{
}

void Signaling_EventGetInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test EventGetInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_EventGetInfo::TestEventGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard EventGetInfo");
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

                        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                        {
                            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);
                            //oSigPreCondition.eStartDevice(m_oParameter);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                {
                                    GC_ERROR Result=GC_ERR_SUCCESS;
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;
                            
                                    Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_MESSAGE("EventGetInfo size check command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String(eEventID).c_str() << 
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
                                        
                                        Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, &sBuffer[0], &iSize);
                                        GENTLTEST_CHECK_MESSAGE("EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                            " eventID=" << sConvertEventID2String(eEventID).c_str() << 
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
                                                ", eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                                ") " << sConvertEventInfoCommand2String(eCommand).c_str(),
                                                iType, &sBuffer[0], iSize);

                                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                            switch (eCommand)
                                            {
                                            case EVENT_EVENT_TYPE:
                                                GENTLTEST_CHECK_MESSAGE(sConvertEventInfoCommand2String(eCommand).c_str() << " wrong data: expected INFO_DATATYPE_INT32 type=" << 
                                                    sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
                                                break;
                                            case EVENT_NUM_FIRED:
                                                GENTLTEST_CHECK_MESSAGE(sConvertEventInfoCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_UINT64 type=" << 
                                                    sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                                break;
                                            case EVENT_NUM_IN_QUEUE:
                                            case EVENT_SIZE_MAX:
                                            case EVENT_INFO_DATA_SIZE_MAX:
                                                GENTLTEST_CHECK_MESSAGE(sConvertEventInfoCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_SIZET type=" << 
                                                    sConvertDataType2String(iType), iType == INFO_DATATYPE_SIZET); 
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
                                                " LastErrorMessage(" << sConvertEventInfoCommand2String(eCommand).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                                        }
                                        else
                                        {
                                            GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                                " LastErrorMessage(" << sConvertEventInfoCommand2String(eCommand).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (nNumberOfFunctionTests == 0)
        GENTLTEST_PRINT("Info: EventGetInfo not tested, no commands available." << std::endl);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetInfo::TestEventGetInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    
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
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            //oSigPreCondition.eStartDevice(m_oParameter);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModEvent.eEventGetInfo(hEvent, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_EventGetInfo::TestEventGetInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    
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
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModEvent.eEventGetInfo(hEvent, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_EventGetInfo::TestEventGetInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    
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
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            //oSigPreCondition.eStartDevice(m_oParameter);

            oIFPreCondition.vClose();
        
            Result = m_ModEvent.eEventGetInfo(hEvent, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_EventGetInfo::TestEventGetInfoWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    
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
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            //oSigPreCondition.eStartDevice(m_oParameter);

            oDevPreCondition.vClose();

            Result = m_ModEvent.eEventGetInfo(hEvent, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_EventGetInfo::TestEventGetInfoWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with closed datastream before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    
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
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            //oSigPreCondition.eStartDevice(m_oParameter);

            oDSPreCondition.vClose();

            Result = m_ModEvent.eEventGetInfo(hEvent, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_EventGetInfo::TestEventGetInfoWithoutRegisterEvent( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with unregistered event before");
    LibrarySystemSetup oLibSysSetup;
    tSignalingList vecDataStreamList;
    tSignalingList::iterator xIter;
    
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
            INFO_DATATYPE iType=0;
            size_t iSize=0;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            //oSigPreCondition.eStartDevice(m_oParameter);

            oSigPreCondition.eGCUnregisterEvent(hDs, sDS.eEventID);

            Result = m_ModEvent.eEventGetInfo(hEvent, sDS.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "EventGetInfo command=" << sConvertEventInfoCommand2String(sDS.eCommand).c_str() << 
                " eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_EventGetInfo::TestEventGetInfoWithEventHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with event handle = GENTL_INVALID_HANDLE");
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

                        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                        {
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                            //oSigPreCondition.eStartDevice(m_oParameter);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;
                    
                                    Result = m_ModEvent.eEventGetInfo(GENTL_INVALID_HANDLE, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetInfo command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String(eEventID).c_str() << 
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
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetInfo::TestEventGetInfoWithInvalidCommand( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with parameter iInfoCmd = EVENT_INFO_DATA_SIZE_MAX+1");
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
            
                        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                        {
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                            //oSigPreCondition.eStartDevice(m_oParameter);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                INFO_DATATYPE iType=0;
                                size_t iSize=0;
                
                                Result = m_ModEvent.eEventGetInfo(hEvent, EVENT_INFO_DATA_SIZE_MAX+1, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=EVENT_INFO_DATA_SIZE_MAX+1" << 
                                    " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                    "EventGetInfo command=EVENT_INFO_DATA_SIZE_MAX+1" << 
                                    " eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
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
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventGetInfo::TestEventGetInfoWithTypeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with parameter piType = NULL at second call with initialized buffer");
    
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
            
                            for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                            {
                                Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                                //oSigPreCondition.eStartDevice(m_oParameter);

                                if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                                {
                                    for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                    {
                                        INFO_DATATYPE iType=0;
                                        size_t iSize=0;
                    
                                        Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, NULL, &iSize);
                                        GENTLTEST_CHECK_MESSAGE("EventGetInfo size check command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                            " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " failed. Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                        if (Result >= GC_ERR_SUCCESS)
                                        {
                                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                            size_t iSizeSave=iSize;
                                            std::vector<char> sBuffer(iSize);

                                            Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, NULL, &sBuffer[0], &iSize);
                                            GENTLTEST_CHECK_RESULT("Note: EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                                " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED,
                                                "EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                                " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                                Result < GC_ERR_SUCCESS);

                                            if (Result < GC_ERR_SUCCESS)
                                            {
                                                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSize == iSizeSave);
                                            }
                                        }
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

void Signaling_EventGetInfo::TestEventGetInfoWithSizeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with parameter piSize = NULL");
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
            
                        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                        {
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                            //oSigPreCondition.eStartDevice(m_oParameter);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;
                    
                                    Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, NULL, NULL);
                                    GENTLTEST_CHECK_RESULT("Note: EventGetInfo command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                                        "EventGetInfo command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                        Result < GC_ERR_SUCCESS);

                                    if (Result < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                                    }
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

void Signaling_EventGetInfo::TestEventGetInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with parameter iSize = 0 at second call with initialized buffer");
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
            
                        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                        {
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                            //oSigPreCondition.eStartDevice(m_oParameter);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                {
                                    INFO_DATATYPE iType=0;
                                    size_t iSize=0;
                    
                                    Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, NULL, &iSize);
                                    GENTLTEST_CHECK_MESSAGE("EventGetInfo size check command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                        " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                        " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        " DataStreamID=" << sDataStreamID.c_str() << 
                                        " DeviceID=" << sDeviceID.c_str() << 
                                        " failed. Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                    if (Result >= GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                        INFO_DATATYPE iTypeSave=iType;
                                        std::vector<char> sBuffer(iSize);

                                        iSize = 0;
                                        Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, &sBuffer[0], &iSize);
                                        GENTLTEST_CHECK_RESULT("Note: EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                            " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED,
                                            "EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                            " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                            Result < GC_ERR_SUCCESS);

                                        if (Result < GC_ERR_SUCCESS)
                                        {
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iType == iTypeSave);
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                                        }
                                    }
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

void Signaling_EventGetInfo::TestEventGetInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventGetInfo with parameter piSize = NULL at second call with initialized buffer");
    
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
            
                            for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                            {
                                Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                                //oSigPreCondition.eStartDevice(m_oParameter);

                                if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                                {
                                    for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                    {
                                        INFO_DATATYPE iType=0;
                                        size_t iSize=0;
                    
                                        Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, NULL, &iSize);
                                        GENTLTEST_CHECK_MESSAGE("EventGetInfo size check command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                            " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            " DataStreamID=" << sDataStreamID.c_str() << 
                                            " DeviceID=" << sDeviceID.c_str() << 
                                            " failed. Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                                        if (Result >= GC_ERR_SUCCESS)
                                        {
                                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                            INFO_DATATYPE iTypeSave=iType;
                                            std::vector<char> sBuffer(iSize);

                                            Result = m_ModEvent.eEventGetInfo(hEvent, eCommand, &iType, &sBuffer[0], NULL);
                                            GENTLTEST_CHECK_RESULT("Note: EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                                " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED,
                                                "EventGetInfo with initialized buffer command=" << sConvertEventInfoCommand2String(eCommand).c_str() << 
                                                " eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                                " DataStreamID=" << sDataStreamID.c_str() << 
                                                " DeviceID=" << sDeviceID.c_str() << 
                                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                                Result < GC_ERR_SUCCESS);

                                            if (Result < GC_ERR_SUCCESS)
                                            {
                                                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iType == iTypeSave);
                                            }
                                        }
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

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Signaling_EventGetInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tSignalingList &vecDataStreamList)
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

                        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                        {
                            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);
                            
                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                for (EVENT_INFO_CMD eCommand=EVENT_EVENT_TYPE; eCommand<=EVENT_INFO_DATA_SIZE_MAX; eCommand=(EVENT_INFO_CMD)(eCommand+1))
                                {
                                    stSignaling sDS;
            
                                    sDS.sInterfaceID = xInterfaceList[index1];
                                    sDS.sDeviceID = sDeviceID;
                                    sDS.eAccess = eAccess;
                                    sDS.sDataStreamID = sDataStreamID;
                                    sDS.eEventID = eEventID;
                                    sDS.eCommand = eCommand;
                                    vecDataStreamList.push_back(sDS);
                                }
                            }
                        }
                    }       
                }
            }
        }
    }

    //GENTLTEST_PRINT("Signaling_EventGetInfo::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
