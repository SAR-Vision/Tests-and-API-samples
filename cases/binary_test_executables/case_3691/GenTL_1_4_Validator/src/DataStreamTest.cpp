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

#include "DataStreamTest.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DataStreamTest::DataStreamTest()
{
}

DataStreamTest::~DataStreamTest()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DataStream
////////////////////////////////////////////////////////////////////////////////////////////

void DataStreamTest::TestDataStream( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DSClose");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        
                        Result = m_ModDev.eDevOpenDataStream(hDev, sDataStreamID.c_str(), &hDs);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << sDataStreamID.c_str() <<
                                                " failed. Expected >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                                                Result >= GC_ERR_SUCCESS);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << sDataStreamID.c_str() <<
                            " failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);

                        Result = m_ModDS.eDSClose(hDs);
                        GENTLTEST_CHECK_MESSAGE("DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " failed expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                            Result >= GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            GENTLTEST_PRINT("Error: DataStreamID=" << sDataStreamID.c_str() << " " << sConvertGCError2String(Result).c_str() << 
                                " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSClose with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            Result = m_ModDev.eDevOpenDataStream(hDev, ifdev.sDataStreamID.c_str(), &hDs);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed. expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result >= GC_ERR_SUCCESS);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);
            
            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDS.eDSClose(hDs);
            GENTLTEST_CHECK_MESSAGE("DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed expected result == GC_ERR_NOT_INITIALIZED returned " << sConvertGCError2String(Result).c_str(),
                Result == GC_ERR_NOT_INITIALIZED);
            
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSClose with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            Result = m_ModDev.eDevOpenDataStream(hDev, ifdev.sDataStreamID.c_str(), &hDs);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed. expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result >= GC_ERR_SUCCESS);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);
            
            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDS.eDSClose(hDs);
            GENTLTEST_CHECK_RESULT("Note: DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " return value: expected result == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER returned " << sConvertGCError2String(Result).c_str(),
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed expected result < GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result < GC_ERR_SUCCESS);
            
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSClose with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            Result = m_ModDev.eDevOpenDataStream(hDev, ifdev.sDataStreamID.c_str(), &hDs);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed. expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result >= GC_ERR_SUCCESS);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);
            
            oIFPreCondition.vClose();
        
            Result = m_ModDS.eDSClose(hDs);
            GENTLTEST_CHECK_RESULT("Note: DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " return value: expected result == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER returned " << sConvertGCError2String(Result).c_str(),
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed expected result < GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamWithoutDevOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSClose with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            Result = m_ModDev.eDevOpenDataStream(hDev, ifdev.sDataStreamID.c_str(), &hDs);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed. expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result >= GC_ERR_SUCCESS);
            GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);
            
            oDevPreCondition.vClose();

            Result = m_ModDS.eDSClose(hDs);
            GENTLTEST_CHECK_RESULT("Note: DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " return value: expected result == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER returned " << sConvertGCError2String(Result).c_str(),
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DSClose DataStreamID=" << ifdev.sDataStreamID.c_str() <<
                " failed expected result < GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamWithHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSClose with parameter data stream handle GENTL_INVALID_HANDLE");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        
                        Result = m_ModDev.eDevOpenDataStream(hDev, sDataStreamID.c_str(), &hDs);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << sDataStreamID.c_str() <<
                                                " failed expected result " << sConvertGCError2String(GC_ERR_SUCCESS).c_str() <<
                                                " returned " << sConvertGCError2String(Result).c_str(),
                                                Result >= GC_ERR_SUCCESS);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);

                        Result = m_ModDS.eDSClose(GENTL_INVALID_HANDLE);
                        GENTLTEST_CHECK_RESULT("Note: DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " return value: expected result == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER returned " << sConvertGCError2String(Result).c_str(),
                            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                            "DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " failed expected result < GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                            Result < GC_ERR_SUCCESS);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamDoubleClose( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double DSClose");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        
                        Result = m_ModDev.eDevOpenDataStream(hDev, sDataStreamID.c_str(), &hDs);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << sDataStreamID.c_str() <<
                                                " failed expected result " << sConvertGCError2String(GC_ERR_SUCCESS).c_str() <<
                                                " returned " << sConvertGCError2String(Result).c_str(),
                                                Result >= GC_ERR_SUCCESS);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);

                        Result = m_ModDS.eDSClose(hDs);
                        GENTLTEST_CHECK_MESSAGE("DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " failed expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                            Result >= GC_ERR_SUCCESS);

                        Result = m_ModDS.eDSClose(hDs);
                        GENTLTEST_CHECK_RESULT("Note: DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " return value: expected result == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER returned " << sConvertGCError2String(Result).c_str(),
                            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                            "DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " failed expected result < GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                            Result < GC_ERR_SUCCESS);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DataStreamTest::TestDataStreamReopen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DSClose reopen");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        
                        Result = m_ModDev.eDevOpenDataStream(hDev, sDataStreamID.c_str(), &hDs);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << sDataStreamID.c_str() <<
                                                " failed expected result " << sConvertGCError2String(GC_ERR_SUCCESS).c_str() <<
                                                " returned " << sConvertGCError2String(Result).c_str(),
                                                Result >= GC_ERR_SUCCESS);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);

                        Result = m_ModDS.eDSClose(hDs);
                        GENTLTEST_CHECK_MESSAGE("DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " failed expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                            Result >= GC_ERR_SUCCESS);

                        Result = m_ModDev.eDevOpenDataStream(hDev, sDataStreamID.c_str(), &hDs);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream DataStreamID=" << sDataStreamID.c_str() <<
                                                " failed expected result " << sConvertGCError2String(GC_ERR_SUCCESS).c_str() <<
                                                " returned " << sConvertGCError2String(Result).c_str(),
                                                Result >= GC_ERR_SUCCESS);
                        GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream failed, datastream handle = GENTL_INVALID_HANDLE", hDs != GENTL_INVALID_HANDLE);

                        Result = m_ModDS.eDSClose(hDs);
                        GENTLTEST_CHECK_MESSAGE("DSClose DataStreamID=" << sDataStreamID.c_str() <<
                            " failed expected result >= GC_ERR_SUCCESS returned " << sConvertGCError2String(Result).c_str(),
                            Result >= GC_ERR_SUCCESS);
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

void DataStreamTest::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID;
            
            sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);               
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        size_t iSize = 0;
                        stIFDevice ifdev;
        
                        Result = m_ModDev.eDevGetDataStreamID(hDev, index3, NULL, &iSize);
                        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);
                        
                        std::vector<char> sBuffer(iSize);
                        Result = m_ModDev.eDevGetDataStreamID(hDev, index3, &sBuffer[0], &iSize);
                        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);
                    
                        ifdev.sInterfaceID = xInterfaceList[index1];
                        ifdev.sDeviceID = sDeviceID;
                        ifdev.eAccess = eAccess;
                        ifdev.sDataStreamID = &sBuffer[0];
                        vIFDeviceList.push_back(ifdev);

                    }       
                }
            }
        }
    }

    //GENTLTEST_PRINT("DataStreamTest::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
