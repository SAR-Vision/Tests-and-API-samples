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

#include "Signaling_EventFlush.h"
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

Signaling_EventFlush::Signaling_EventFlush()
{
}

Signaling_EventFlush::~Signaling_EventFlush()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_EventFlush::setUp(void)
{
}

void Signaling_EventFlush::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test EventFlush
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_EventFlush::TestEventFlush( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard EventFlush");
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
                            
                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                GC_ERROR Result=GC_ERR_SUCCESS;
                                
                                oSigPreCondition.eStartDevice(m_oParameter);
                                
                                GENTLTEST_SLEEP(100);

                                oSigPreCondition.eStopDevice(m_oParameter);

                                Result = m_ModEvent.eEventFlush(hEvent);
                                GENTLTEST_CHECK_MESSAGE("EventFlush eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with closed library before");
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
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            GENTLTEST_SLEEP(100);
            oSigPreCondition.eStopDevice(m_oParameter);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModEvent.eEventFlush(hEvent);
            GENTLTEST_CHECK_MESSAGE("EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED);
        
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with closed system before");
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
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
            
            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            GENTLTEST_SLEEP(100);
            oSigPreCondition.eStopDevice(m_oParameter);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModEvent.eEventFlush(hEvent);
            GENTLTEST_CHECK_RESULT("Note: EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with closed interface before");
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
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            GENTLTEST_SLEEP(100);
            oSigPreCondition.eStopDevice(m_oParameter);

            oIFPreCondition.vClose();
        
            Result = m_ModEvent.eEventFlush(hEvent);
            GENTLTEST_CHECK_RESULT("Note: EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with closed device before");
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
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            GENTLTEST_SLEEP(100);
            oSigPreCondition.eStopDevice(m_oParameter);

            oDevPreCondition.vClose();

            Result = m_ModEvent.eEventFlush(hEvent);
            GENTLTEST_CHECK_RESULT("Note: EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with closed datastream before");
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
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            GENTLTEST_SLEEP(100);
            oSigPreCondition.eStopDevice(m_oParameter);

            oDSPreCondition.vClose();

            Result = m_ModEvent.eEventFlush(hEvent);
            GENTLTEST_CHECK_RESULT("Note: EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithoutRegisterEvent( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with unregistered event before");
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
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
            
            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eLockParameter(m_oParameter, true);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            Signaling_PreCondition oSigPreCondition(hDs, sDS.eEventID, &hEvent);
            oSigPreCondition.eStartDevice(m_oParameter);
            GENTLTEST_SLEEP(100);
            oSigPreCondition.eStopDevice(m_oParameter);

            oSigPreCondition.eGCUnregisterEvent(hDs, sDS.eEventID);

            Result = m_ModEvent.eEventFlush(hEvent);
            GENTLTEST_CHECK_RESULT("Note: EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "EventFlush eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_EventFlush::TestEventFlushWithEventHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test EventFlush with event handle = GENTL_INVALID_HANDLE");
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
                            size_t iDataSize=0;
                            uint64_t uiTimeOut=1000;
                            Signaling_PreCondition oSigPreCondition(hDs, eEventID, &hEvent);

                            if (oSigPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                            {
                                oSigPreCondition.eStartDevice(m_oParameter);
                                GENTLTEST_SLEEP(100);
                                oSigPreCondition.eStopDevice(m_oParameter);

                                Result = m_ModEvent.eEventFlush(GENTL_INVALID_HANDLE);
                                GENTLTEST_CHECK_RESULT("Note: EventFlush eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE,
                                    "EventFlush eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DataStreamID=" << sDataStreamID.c_str() << 
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result < GC_ERR_SUCCESS);
                            }
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

void Signaling_EventFlush::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tSignalingList &vecDataStreamList)
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
                                stSignaling sDS;
        
                                sDS.sInterfaceID = xInterfaceList[index1];
                                sDS.sDeviceID = sDeviceID;
                                sDS.eAccess = eAccess;
                                sDS.sDataStreamID = sDataStreamID;
                                sDS.eEventID = eEventID;
                                vecDataStreamList.push_back(sDS);
                            }
                        }
                    }       
                }
            }
        }
    }

    //GENTLTEST_PRINT("Signaling_EventFlush::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
