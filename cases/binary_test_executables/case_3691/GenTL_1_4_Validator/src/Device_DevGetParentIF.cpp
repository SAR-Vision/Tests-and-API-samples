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

#include "Device_DevGetParentIF.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Device_DevGetParentIF::Device_DevGetParentIF()
{
}

Device_DevGetParentIF::~Device_DevGetParentIF()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetParentIF::setUp(void)
{
}

void Device_DevGetParentIF::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFOpenDevice
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetParentIF::TestDevGetParentIF( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DevGetParentIF");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int                                 nTotalNumberOfTests = 0;
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t                uiNumDevices;
        IF_HANDLE               hInterface = GENTL_INVALID_HANDLE;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hInterface);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string         sDeviceID = oIFPreCondition.sGetDeviceID(hInterface, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE              hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition     oDevPreCondition(hInterface, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    GC_ERROR                Result = GC_ERR_SUCCESS;
                    IF_HANDLE               hIf = GENTL_INVALID_HANDLE;
                    
                    Result = m_ModDev.eDevGetParentIF(hDev, &hIf);

                    GENTLTEST_CHECK_MESSAGE("DevGetParentIF failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS);
        
                    if (Result >= GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("DevGetParentIF failed. Expected handle=0x" << std::hex << hInterface << " , received 0x" << hIf, 
                                hIf == hInterface);
                    }

                    nTotalNumberOfTests++;
                }
            }
        }
    }

    GENTLTEST_CHECK_MESSAGE("DevGetParentIF failed. No device available", nTotalNumberOfTests > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetParentIF::TestDevGetParentIFWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetParentIF with closed library before");
    LibrarySystemSetup          oLibSysSetup;
    tIFDeviceList               vIFDeviceList;
    tIFDeviceList::iterator     xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR                Result = GC_ERR_SUCCESS;
        DEV_HANDLE              hDev = GENTL_INVALID_HANDLE;
        IF_HANDLE               hInterface = GENTL_INVALID_HANDLE;
        stIFDevice              ifdev = (stIFDevice)*xIter;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hInterface);
        Device_PreCondition     oDevPreCondition(hInterface, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            IF_HANDLE hIf = GENTL_INVALID_HANDLE;
        
            oLibSysSetup.tearDownLibrary();
            
            Result = m_ModDev.eDevGetParentIF(hDev, &hIf);
            GENTLTEST_CHECK_RESULT("Note: DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " recommended return value == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED,
                "DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hIf != GENTL_INVALID_HANDLE", hIf == GENTL_INVALID_HANDLE);
            }
            
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetParentIF::TestDevGetParentIFWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetParentIF with closed system before");
    LibrarySystemSetup          oLibSysSetup;
    tIFDeviceList               vIFDeviceList;
    tIFDeviceList::iterator     xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hInterface = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hInterface);
        Device_PreCondition oDevPreCondition(hInterface, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            IF_HANDLE hIf = GENTL_INVALID_HANDLE;
        
            oLibSysSetup.tearDownSystem();
            
            Result = m_ModDev.eDevGetParentIF(hDev, &hIf);
            GENTLTEST_CHECK_RESULT("Note: DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hIf != GENTL_INVALID_HANDLE", hIf == GENTL_INVALID_HANDLE);
            }
            
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetParentIF::TestDevGetParentIFWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetParentIF with closed interface before");
    LibrarySystemSetup          oLibSysSetup;
    tIFDeviceList               vIFDeviceList;
    tIFDeviceList::iterator     xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR                Result = GC_ERR_SUCCESS;
        DEV_HANDLE              hDev = GENTL_INVALID_HANDLE;
        IF_HANDLE               hInterface = GENTL_INVALID_HANDLE;
        stIFDevice              ifdev = (stIFDevice)*xIter;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hInterface);
        Device_PreCondition     oDevPreCondition(hInterface, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            IF_HANDLE hIf = GENTL_INVALID_HANDLE;
        
            oIFPreCondition.vClose();

            Result = m_ModDev.eDevGetParentIF(hDev, &hIf);
            GENTLTEST_CHECK_RESULT("Note: DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hIf != GENTL_INVALID_HANDLE", hIf == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetParentIF::TestDevGetParentIFWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetParentIF with closed device before");
    LibrarySystemSetup          oLibSysSetup;
    tIFDeviceList               vIFDeviceList;
    tIFDeviceList::iterator     xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR                Result = GC_ERR_SUCCESS;
        DEV_HANDLE              hDev = GENTL_INVALID_HANDLE;
        IF_HANDLE               hInterface = GENTL_INVALID_HANDLE;
        stIFDevice              ifdev = (stIFDevice)*xIter;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hInterface);
        Device_PreCondition     oDevPreCondition(hInterface, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            IF_HANDLE hIf = GENTL_INVALID_HANDLE;
        
            oDevPreCondition.vClose();

            Result = m_ModDev.eDevGetParentIF(hDev, &hIf);
            GENTLTEST_CHECK_RESULT("Note: DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevGetParentIF DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call hIf != GENTL_INVALID_HANDLE", hIf == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetParentIF::TestDevGetParentIFWithDevNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetParentIF with device handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hInterface = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hInterface);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hInterface, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hInterface, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    IF_HANDLE       hIf = GENTL_INVALID_HANDLE;
                    GC_ERROR        Result = GC_ERR_SUCCESS;
                        
                    Result = m_ModDev.eDevGetParentIF(GENTL_INVALID_HANDLE, &hIf);
                    GENTLTEST_CHECK_RESULT("Note: DevGetParentIF DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                        "DevGetParentIF DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call hIf != GENTL_INVALID_HANDLE", hIf == GENTL_INVALID_HANDLE);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetParentIF::TestDevGetParentIFWithIFNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetParentIF with pointer hIf = NULL");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hInterface = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hInterface);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hInterface, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hInterface, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    GC_ERROR        Result = GC_ERR_SUCCESS;
                        
                    Result = m_ModDev.eDevGetParentIF(hDev, NULL);
                    GENTLTEST_CHECK_RESULT("Note: DevGetParentIF DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                        "DevGetParentIF DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Device_DevGetParentIF::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                stIFDevice ifdev;

                ifdev.sInterfaceID = xInterfaceList[index1];
                ifdev.sDeviceID = sDeviceID;
                ifdev.eAccess = eAccess;
                vIFDeviceList.push_back(ifdev);
            }
        }
    }

    //GENTLTEST_PRINT("Device_DevGetDataStreamID::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
