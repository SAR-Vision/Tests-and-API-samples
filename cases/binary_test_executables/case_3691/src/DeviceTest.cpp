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

#include "GenTlTestTools.h"
#include "DeviceTest.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "URLParser.h"
#include "LocalParser.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

DeviceTest::DeviceTest()
{
}

DeviceTest::~DeviceTest()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFClose
////////////////////////////////////////////////////////////////////////////////////////////

void DeviceTest::TestDevClose( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test standard DevClose");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK(hDev != GENTL_INVALID_HANDLE);

                    Result = m_ModDev.eDevClose(hDev);
                    GENTLTEST_CHECK_MESSAGE("DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseWithLibraryClosed( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test DevClose with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);
    
    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        Result = m_ModIF.eIFOpenDevice(hIF, ifdev.sDeviceID.c_str(), ifdev.eAccess, &hDev);
        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

        if (Result >= GC_ERR_SUCCESS)
        {
            hDevSave = hDev;

            oLibSysSetup.tearDownLibrary();
            
            Result = m_ModDev.eDevClose(hDev);
            GENTLTEST_CHECK_MESSAGE("DevClose InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call Device Handle != old handle", hDevSave == hDev);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseWithoutTLOpen( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test DevClose with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        Result = m_ModIF.eIFOpenDevice(hIF, ifdev.sDeviceID.c_str(), ifdev.eAccess, &hDev);
        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

        if (Result >= GC_ERR_SUCCESS)
        {
            hDevSave = hDev;

            oLibSysSetup.tearDownSystem();
            
            Result = m_ModDev.eDevClose(hDev);
            GENTLTEST_CHECK_RESULT("Note: DevClose InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevClose InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call Device Handle != old handle", hDevSave == hDev);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseWithoutInterfaceOpen( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test DevClose with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        Result = m_ModIF.eIFOpenDevice(hIF, ifdev.sDeviceID.c_str(), ifdev.eAccess, &hDev);
        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

        if (Result >= GC_ERR_SUCCESS)
        {
            hDevSave = hDev;

            oIFPreCondition.vClose();
            
            Result = m_ModDev.eDevClose(hDev);
            GENTLTEST_CHECK_RESULT("Note: DevClose InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevClose InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call Device Handle != old handle", hDevSave == hDev);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseDouble( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test double DevClose");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK(hDev != GENTL_INVALID_HANDLE);
                
                    Result = m_ModDev.eDevClose(hDev);
                    GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);

                    hDevSave = hDev;

                    Result = m_ModDev.eDevClose(hDev);
                    GENTLTEST_CHECK_RESULT("Note: DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                        "DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call Device Handle != old handle", hDevSave == hDev);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseReopen( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test DevClose and reopen");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK(hDev != GENTL_INVALID_HANDLE);
                
                    Result = m_ModDev.eDevClose(hDev);
                    GENTLTEST_CHECK_MESSAGE("DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " 1st failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result >= GC_ERR_SUCCESS);

                    Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                    GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);
                    GENTLTEST_CHECK(hDev != GENTL_INVALID_HANDLE);

                    Result = m_ModDev.eDevClose(hDev);
                    GENTLTEST_CHECK_MESSAGE("DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " 2nd failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result >= GC_ERR_SUCCESS);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseWithHandleDevNull( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test DevClose with parameter device handle GENTL_INVALID_HANDLE");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);
                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK(hDev != GENTL_INVALID_HANDLE);
                
                    hDevSave = hDev;

                    Result = m_ModDev.eDevClose(GENTL_INVALID_HANDLE);
                    GENTLTEST_CHECK_RESULT("Note: DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                        "DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call dev handle != old handle", hDevSave == hDev);
                    }

                    m_ModDev.eDevClose(hDev);                       // just do it now for real
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void DeviceTest::TestDevCloseWithHandleDevWrong( uint32_t test_id )
{
    bool bResult=true;
    GENTLTEST_DESCRIPTION(test_id, "Test DevClose with parameter random device handle (0xb7f7b7f7)");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;

                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK(hDev != GENTL_INVALID_HANDLE);
                
                    hDevSave = hDev;

                    Result = m_ModDev.eDevClose((DEV_HANDLE)0xb7f7b7f7);
                    GENTLTEST_CHECK_RESULT("Note: DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE,
                        "DevClose InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call dev handle != old handle", hDevSave == hDev);
                    }

                    m_ModDev.eDevClose(hDev);                       // just do it now for real
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void DeviceTest::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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
            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                std::string sDeviceID;
                stIFDevice ifdev;

                sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);
                ifdev.sInterfaceID = xInterfaceList[index1];
                ifdev.sDeviceID = sDeviceID;
                ifdev.eAccess = eAccess;
                vIFDeviceList.push_back(ifdev);
            }
        }
    }

    //GENTLTEST_PRINT("DeviceTest::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
