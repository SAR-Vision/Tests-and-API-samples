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

#include "Signaling_GCUnregisterEvent.h"
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

Signaling_GCUnregisterEvent::Signaling_GCUnregisterEvent()
{
}

Signaling_GCUnregisterEvent::~Signaling_GCUnregisterEvent()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_GCUnregisterEvent::setUp(void)
{
}

void Signaling_GCUnregisterEvent::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCUnregisterEvent
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_GCUnregisterEvent::TestGCUnregisterEventSystem( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCUnregisterEvent (System)");
    LibrarySystemSetup oLibSysSetup;
    
    for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;

        Signaling_PreCondition oSigPrecondition;
        oSigPrecondition.eGCRegisterEvent(oLibSysSetup.hGetTLHandle(), eEventID, &hEvent);

        Result = m_ModEvent.eGCUnregisterEvent(oLibSysSetup.hGetTLHandle(), eEventID);
        GENTLTEST_CHECK_MESSAGE("GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

        if (Result < GC_ERR_SUCCESS)
        {
            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            if (Result == GC_ERR_NOT_IMPLEMENTED)
            {
                GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                    " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
            }
            else
            {
                GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                    " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
            }
        }
        else
        {
            GENTLTEST_PRINT("Info: GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                " successful executed." << std::endl);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventInterface( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCUnregisterEvent (Interface)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
        {
            GC_ERROR Result=GC_ERR_SUCCESS;
            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;

            Signaling_PreCondition oSigPrecondition;
            oSigPrecondition.eGCRegisterEvent(hIF, eEventID, &hEvent);

            Result = m_ModEvent.eGCUnregisterEvent(hIF, eEventID);
            GENTLTEST_CHECK_MESSAGE("GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

            if (Result < GC_ERR_SUCCESS)
            {
                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                if (Result == GC_ERR_NOT_IMPLEMENTED)
                {
                    GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                        " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                }
                else
                {
                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                        " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                }
            }
            else
            {
                GENTLTEST_PRINT("Info: GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                    " successful executed." << std::endl);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCUnregisterEvent (Device)");
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
                    for (EVENT_TYPE_LIST eEventID=EVENT_ERROR; eEventID<=EVENT_MODULE; eEventID=(EVENT_TYPE_LIST)(eEventID+1))
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;

                        Signaling_PreCondition oSigPrecondition;
                        oSigPrecondition.eGCRegisterEvent(hDev, eEventID, &hEvent);

                        Result = m_ModEvent.eGCUnregisterEvent(hDev, eEventID);
                        GENTLTEST_CHECK_MESSAGE("GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            if (Result == GC_ERR_NOT_IMPLEMENTED)
                            {
                                GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                                    " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                            }
                            else
                            {
                                GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                    " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                            }
                        }
                        else
                        {
                            GENTLTEST_PRINT("Info: GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << " successful executed." << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventDataStream( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCUnregisterEvent (DataStream)");
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
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        uint32_t nBufferCount=10;
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
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
                            Signaling_PreCondition oSigPrecondition;
                            oSigPrecondition.eGCRegisterEvent(hDs, eEventID, &hEvent);

                            Result = m_ModEvent.eGCUnregisterEvent(hDs, eEventID);
                            GENTLTEST_CHECK_MESSAGE("GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);

                            if (Result < GC_ERR_SUCCESS)
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                if (Result == GC_ERR_NOT_IMPLEMENTED)
                                {
                                    GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                        " LastErrorMessage(" << sConvertEventID2String(eEventID).c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                                }
                            }
                            else
                            {
                                GENTLTEST_PRINT("Info: GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << " successful executed." << std::endl);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with closed library before");
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

            Signaling_PreCondition oSigPrecondition;
            oSigPrecondition.eGCRegisterEvent(hDs, sDS.eEventID, &hEvent);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModEvent.eGCUnregisterEvent(hDs, sDS.eEventID);
            GENTLTEST_CHECK_MESSAGE("GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with closed system before");
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

            Signaling_PreCondition oSigPrecondition;
            oSigPrecondition.eGCRegisterEvent(hDs, sDS.eEventID, &hEvent);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModEvent.eGCUnregisterEvent(hDs, sDS.eEventID);
            GENTLTEST_CHECK_RESULT("Note: GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER,
                "GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
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

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with closed interface before");
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

            Signaling_PreCondition oSigPrecondition;
            oSigPrecondition.eGCRegisterEvent(hDs, sDS.eEventID, &hEvent);

            oIFPreCondition.vClose();
        
            Result = m_ModEvent.eGCUnregisterEvent(hDs, sDS.eEventID);
            GENTLTEST_CHECK_RESULT("Note: GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER,
                "GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with closed device before");
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

            Signaling_PreCondition oSigPrecondition;
            oSigPrecondition.eGCRegisterEvent(hDs, sDS.eEventID, &hEvent);

            oDevPreCondition.vClose();

            Result = m_ModEvent.eGCUnregisterEvent(hDs, sDS.eEventID);
            GENTLTEST_CHECK_RESULT("Note: GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER,
                "GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with closed datastream before");
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

            Signaling_PreCondition oSigPrecondition;
            oSigPrecondition.eGCRegisterEvent(hDs, sDS.eEventID, &hEvent);

            oDSPreCondition.vClose();

            Result = m_ModEvent.eGCUnregisterEvent(hDs, sDS.eEventID);
            GENTLTEST_CHECK_RESULT("Note: GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER,
                "GCUnregisterEvent eventID=" << sConvertEventID2String(sDS.eEventID).c_str() << 
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithDSIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with datastream handle = GENTL_INVALID_HANDLE");
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
                            EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                            Signaling_PreCondition oSigPrecondition;
                            oSigPrecondition.eGCRegisterEvent(hDs, eEventID, &hEvent);

                            Result = m_ModEvent.eGCUnregisterEvent(GENTL_INVALID_HANDLE, eEventID);
                            GENTLTEST_CHECK_RESULT("Note: GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE,
                                "GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void Signaling_GCUnregisterEvent::TestGCUnregisterEventWithWrongEventID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCUnregisterEvent with parameter iEventID = EVENT_MODULE+1");
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

                        Signaling_PreCondition oSigPrecondition;
                        oSigPrecondition.eGCRegisterEvent(hDs, EVENT_MODULE, &hEvent);

                        Result = m_ModEvent.eGCUnregisterEvent(hDs, EVENT_MODULE+1);
                        GENTLTEST_CHECK_RESULT("Note: GCUnregisterEvent eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED,
                            "GCUnregisterEvent eventID=" << sConvertEventID2String((EVENT_TYPE_LIST)(EVENT_MODULE+1)).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Signaling_GCUnregisterEvent::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tSignalingList &vecDataStreamList)
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

    //GENTLTEST_PRINT("Signaling_GCUnregisterEvent::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
