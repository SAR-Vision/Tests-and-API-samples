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

#include "DataStream_DSStopAcquisition.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DataStream_DSStopAcquisition::DataStream_DSStopAcquisition()
{
}

DataStream_DSStopAcquisition::~DataStream_DSStopAcquisition()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSStopAcquisition::setUp(void)
{
}

void DataStream_DSStopAcquisition::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DSStopAcquisition
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSStopAcquisition::TestDSStopAcquisition( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSStopAcquisition");
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
                        uint32_t nBufferSize=10;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

                        for (uint32_t i=0; i<nBufferSize; i++)
                        {
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        }

                        for (ACQ_STOP_FLAGS_LIST eStopFlag=ACQ_STOP_FLAGS_DEFAULT; eStopFlag<=ACQ_STOP_FLAGS_KILL; eStopFlag=(ACQ_STOP_FLAGS_LIST)(eStopFlag+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                        
                            
                            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

                            Result = m_ModDS.eDSStopAcquisition(hDs, eStopFlag);
                            GENTLTEST_CHECK_MESSAGE("DSStopAcquisition stopFlag=" << sConvertDataStreamStopFlag2String(eStopFlag) << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS);

                            if (Result < GC_ERR_SUCCESS)
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                    " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with closed library before");
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
            
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDS.eDSStopAcquisition(hDs, sDS.eStopFlag);
            GENTLTEST_CHECK_MESSAGE("DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
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

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with closed system before");
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
            
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDS.eDSStopAcquisition(hDs, sDS.eStopFlag);
            GENTLTEST_CHECK_RESULT("Note: DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_NOT_INITIALIZED == == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
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

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with closed interface before");
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
            
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            oIFPreCondition.vClose();
        
            Result = m_ModDS.eDSStopAcquisition(hDs, sDS.eStopFlag);
            GENTLTEST_CHECK_RESULT("Note: DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with closed device before");
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
            
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            oDevPreCondition.vClose();

            Result = m_ModDS.eDSStopAcquisition(hDs, sDS.eStopFlag);
            GENTLTEST_CHECK_RESULT("Note: DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with closed datastream before");
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
            
            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
            DataStream_PreCondition oDSPreCondition(hDev, sDS.sDataStreamID, &hDs);
            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());

            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

            oDSPreCondition.vClose();

            Result = m_ModDS.eDSStopAcquisition(hDs, sDS.eStopFlag);
            GENTLTEST_CHECK_RESULT("Note: DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStopAcquisition stopflag=" << sConvertDataStreamStopFlag2String(sDS.eStopFlag) <<
                " device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithDSIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with datastream handle = GENTL_INVALID_HANDLE");
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
                        for (ACQ_STOP_FLAGS_LIST eStopFlag=ACQ_STOP_FLAGS_DEFAULT; eStopFlag<=ACQ_STOP_FLAGS_KILL; eStopFlag=(ACQ_STOP_FLAGS_LIST)(eStopFlag+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                            BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                            std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                            DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                            size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                            
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                            oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

                            Result = m_ModDS.eDSStopAcquisition(GENTL_INVALID_HANDLE, eStopFlag);
                            GENTLTEST_CHECK_RESULT("Note: DSStopAcquisition startflag=" << sConvertDataStreamStopFlag2String(eStopFlag) << 
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE,
                                "DSStopAcquisition startflag=" << sConvertDataStreamStopFlag2String(eStopFlag) << 
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

void DataStream_DSStopAcquisition::TestDSStopAcquisitionWithWrongStopFlag( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStopAcquisition with parameter iStopFlags = ACQ_STOP_FLAGS_KILL+1");
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
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);

                        Result = m_ModDS.eDSStopAcquisition(hDs, ACQ_STOP_FLAGS_KILL+1);
                        GENTLTEST_CHECK_RESULT("Note: DSStopAcquisition startflag=ACQ_STOP_FLAGS_KILL+1" << 
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED,
                            "DSStopAcquisition startflag=ACQ_STOP_FLAGS_KILL+1" << 
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

void DataStream_DSStopAcquisition::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tDataStreamList &vecDataStreamList)
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

                        for (ACQ_STOP_FLAGS_LIST eStopFlag=ACQ_STOP_FLAGS_DEFAULT; eStopFlag<=ACQ_STOP_FLAGS_KILL; eStopFlag=(ACQ_STOP_FLAGS_LIST)(eStopFlag+1))
                        {
                            stDataStream sDS;
            
                            sDS.sInterfaceID = xInterfaceList[index1];
                            sDS.sDeviceID = sDeviceID;
                            sDS.eAccess = eAccess;
                            sDS.sDataStreamID = sDataStreamID;
                            sDS.eStopFlag = eStopFlag;
                            vecDataStreamList.push_back(sDS);
                        }
                    }       
                }
            }
        }
    }

    //GENTLTEST_PRINT("DataStream_DSStopAcquisition::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
