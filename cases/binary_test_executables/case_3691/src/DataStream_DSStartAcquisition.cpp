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

#include "DataStream_DSStartAcquisition.h"
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

DataStream_DSStartAcquisition::DataStream_DSStartAcquisition()
{
}

DataStream_DSStartAcquisition::~DataStream_DSStartAcquisition()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSStartAcquisition::setUp(void)
{
}

void DataStream_DSStartAcquisition::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DSStartAcquisition
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSStartAcquisition::TestDSStartAcquisition( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSStartAcquisition");
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
                        uint32_t nBufferSize=10;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        for (uint32_t i=0; i<nBufferSize; i++)
                        {
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        }

                        Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
                        GENTLTEST_CHECK_MESSAGE("DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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
                        else
                        {
                            GENTLTEST_PRINT("Info: device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << " " << nBufferSize << 
                                " buffers with payloadsize=0x" << std::hex << uiPayLoadSize << std::dec << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with closed library before");
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

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
            GENTLTEST_CHECK_MESSAGE("DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
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

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with closed system before");
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

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
            GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with closed interface before");
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

            oIFPreCondition.vClose();
        
            Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
            GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with closed device before");
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

            oDevPreCondition.vClose();

            Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
            GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with closed datastream before");
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

            oDSPreCondition.vClose();

            Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
            GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithNumToAcquireNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with parameter NumToAcquire = 0");
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

                        Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 0);
                        GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED,
                            "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithDSIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with datastream handle = GENTL_INVALID_HANDLE");
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

                        Result = m_ModDS.eDSStartAcquisition(GENTL_INVALID_HANDLE, ACQ_START_FLAGS_DEFAULT, 1);
                        GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_HANDLE,
                            "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithWrongStartFlag( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with parameter iStartFlags = ACQ_START_FLAGS_DEFAULT+1");
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

                        Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT+1, 1);
                        GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED,
                            "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWith10Frames( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with parameter iNumToAcquire = 10");
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

                        Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 10);
                        GENTLTEST_CHECK_MESSAGE("DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithSmallBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with buffer size too small = uiPayLoadSize-1");
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
                        
                        // Get only payloadSize of datastream not from remote device
                        // if dataStream doesn't provide it don't execute this test.
                        // In case of a not standardized remote device a datastream module doesn't know the correct size 
                        // because it cannot access the payload size of the remote device and could not check the correct size.
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSizeByDSGetInfo(hDs);
                        
                        if (uiPayLoadSize > 0)
                        {
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize-1, (void *)this, &hBuffer);

                            Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
                            GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " DeviceID=" << sDeviceID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_BUFFER,
                                "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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

void DataStream_DSStartAcquisition::TestDSStartAcquisitionWithNoBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSStartAcquisition with no buffer");
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
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                        Result = m_ModDS.eDSStartAcquisition(hDs, ACQ_START_FLAGS_DEFAULT, 1);
                        GENTLTEST_CHECK_RESULT("Note: DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_BUFFER,
                            "DSStartAcquisition device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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

void DataStream_DSStartAcquisition::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tDataStreamList &vecDataStreamList)
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
                        stDataStream sDS;
        
                        sDS.sInterfaceID = xInterfaceList[index1];
                        sDS.sDeviceID = sDeviceID;
                        sDS.eAccess = eAccess;
                        sDS.sDataStreamID = sDataStreamID;
                        vecDataStreamList.push_back(sDS);
                    }       
                }
            }
        }
    }

    //GENTLTEST_PRINT("DataStream_DSStartAcquisition::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}
