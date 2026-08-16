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

#include "DataStream_DSAnnounceBuffer.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "GenTLTestTools.h"
#include "LocalParser.h"

#include "SFNCPort.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DataStream_DSAnnounceBuffer::DataStream_DSAnnounceBuffer()
{
}

DataStream_DSAnnounceBuffer::~DataStream_DSAnnounceBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSAnnounceBuffer::setUp(void)
{
}

void DataStream_DSAnnounceBuffer::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DSAnnounceBuffer
////////////////////////////////////////////////////////////////////////////////////////////

void DataStream_DSAnnounceBuffer::TestDSAnnounceBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSAnnounceBuffer (using PayloadSize of device module)");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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
                                " DataStreamID=" << sDataStreamID.c_str() << " 1 buffer with payloadsize=0x" << std::hex << uiPayLoadSize << std::dec << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBuffer2( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSAnnounceBuffer (using PayloadSize of datastream module)");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
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
                                " DataStreamID=" << sDataStreamID.c_str() << " 1 buffer with payloadsize=0x" << std::hex << uiPayLoadSize << std::dec << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with closed library before");
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
            std::vector<char> vecPayLoad(uiPayLoadSize);

            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
            GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED);
        
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with closed system before");
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
            std::vector<char> vecPayLoad(uiPayLoadSize);

            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
            GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE,
                "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with closed interface before");
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
            std::vector<char> vecPayLoad(uiPayLoadSize);

            oIFPreCondition.vClose();
        
            Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
            GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE,
                "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with closed device before");
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
            std::vector<char> vecPayLoad(uiPayLoadSize);

            oDevPreCondition.vClose();

            Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
            GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE,
                "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithoutDSOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with closed datastream before");
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
            std::vector<char> vecPayLoad(uiPayLoadSize);

            oDSPreCondition.vClose();

            Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
            GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE,
                "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(sDS.eAccess).c_str() <<
                " DataStreamID=" << sDS.sDataStreamID.c_str() << 
                " DeviceID=" << sDS.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithPayloadSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with parameter payload iSize = 0");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], 0, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER,
                            "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithPayloadNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with parameter payload pBuffer = NULL");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, NULL, uiPayLoadSize, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER,
                            "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithDSIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with datastream handle = GENTL_INVALID_HANDLE");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(GENTL_INVALID_HANDLE, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                            "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer != GENTL_INVALID_HANDLE", hBuffer == GENTL_INVALID_HANDLE);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferPrivatedataNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with parameter pPrivate = NULL");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, NULL, &hBuffer);
                        GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call hBuffer == GENTL_INVALID_HANDLE", hBuffer != GENTL_INVALID_HANDLE);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferWithBufferHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer with parameter phBuffer = NULL");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void*)this, NULL);
                        GENTLTEST_CHECK_RESULT("Note: DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER,
                            "DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferTwice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer twice with the same buffer to a single stream (see section 3.6)");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            Result = m_ModDS.eDSAnnounceBuffer(hDs, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                            GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer twice device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID.c_str() << 
                                " failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_RESOURCE_IN_USE);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStream_DSAnnounceBuffer::TestDSAnnounceBufferTwiceToDifferentStreams( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSAnnounceBuffer twice with the same buffer to different streams (see section 3.6)");
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
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());

                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    if (uiNumDataStreams >= 2)
                    {
                        DS_HANDLE hDs1=GENTL_INVALID_HANDLE;
                        DS_HANDLE hDs2=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID1=oDevPreCondition.sGetDataStreamID(hDev, 0);
                        std::string sDataStreamID2=oDevPreCondition.sGetDataStreamID(hDev, 1);
                        DataStream_PreCondition oDSPreCondition1(hDev, sDataStreamID1, &hDs1);
                        DataStream_PreCondition oDSPreCondition2(hDev, sDataStreamID2, &hDs2);
                        size_t uiPayLoadSize=oDSPreCondition1.uiGetPayLoadSize(oLibSysSetup, hDs1, oDevPreCondition.hDevGetPort());
                        size_t uiPayLoadSize2=oDSPreCondition2.uiGetPayLoadSize(oLibSysSetup, hDs2, oDevPreCondition.hDevGetPort());
                        
                        if (uiPayLoadSize2 > uiPayLoadSize)
                            uiPayLoadSize = uiPayLoadSize2;

                        std::vector<char> vecPayLoad(uiPayLoadSize);

                        Result = m_ModDS.eDSAnnounceBuffer(hDs1, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                        GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " DataStreamID=" << sDataStreamID1.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            Result = m_ModDS.eDSAnnounceBuffer(hDs2, &vecPayLoad[0], uiPayLoadSize, (void *)this, &hBuffer);
                            GENTLTEST_CHECK_MESSAGE("DSAnnounceBuffer second, device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " DataStreamID=" << sDataStreamID1.c_str() << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result < GC_ERR_SUCCESS);
                        }
                    }
                    else
                    {
                        GENTLTEST_PRINT("Info: Test skipped NumDataStreams < 2" << std::endl);
                        GENTLTEST_CHECK(true); // dummy for boost to count this test
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

void DataStream_DSAnnounceBuffer::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tDataStreamList &vecDataStreamList)
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
                        stDataStream sDS;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
    
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

    //GENTLTEST_PRINT("DataStream_DSAnnounceBuffer::vCreateTestCaseList " << vecDataStreamList.size() << " testcases created" << std::endl);
}

