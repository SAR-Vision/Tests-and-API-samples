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

#include "Device_DevGetPort.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Device_DevGetPort::Device_DevGetPort()
{
}

Device_DevGetPort::~Device_DevGetPort()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetPort::setUp(void)
{
}

void Device_DevGetPort::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DevGetPort
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetPort::TestDevGetPort( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DevGetPort");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    uint32_t uiTotalNumRemoteDevices = 0;

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
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Result = m_ModDev.eDevGetPort(hDev, &hPort);
                    GENTLTEST_CHECK_MESSAGE("DevGetPort InterfaceID=" << xInterfaceList[index1].c_str() << 
                        " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
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
                        uiTotalNumRemoteDevices++;

                        GENTLTEST_PRINT("Info: DevGetPort InterfaceID=" << xInterfaceList[index1].c_str() << 
                            " DeviceID=" << sDeviceID.c_str() << " DEVICE_ACCESS_FLAG=" << sConvertDEVICEAccess2String(eAccess).c_str() << 
                            " hPort=0x" << std::hex << hPort << std::dec << std::endl);
                    }
                
                    GENTLTEST_CHECK_MESSAGE("Parameter after call port handle == GENTL_INVALID_HANDLE", hPort != GENTL_INVALID_HANDLE);
                }
            }
        }
    }

    // for certification we need at least one remote device
    GENTLTEST_CHECK_MESSAGE(std::endl << "Error: ***********************************************************" << std::endl <<
        "Error: * For GenICam Validation one remote device should exist   *" << std::endl <<
        "Error: * Aborting validation                                     *" << std::endl <<
        "Error: ***********************************************************" << std::endl, 
        uiTotalNumRemoteDevices > 0);

    if (uiTotalNumRemoteDevices == 0)
        g_bStopValidationFlag = true;

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetPort::TestDevGetPortWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetPort with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oLibSysSetup.tearDownLibrary();
        
            Result = m_ModDev.eDevGetPort(hDev, &hPort);
            GENTLTEST_CHECK_MESSAGE("DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call port handle != GENTL_INVALID_HANDLE", hPort == GENTL_INVALID_HANDLE);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetPort::TestDevGetPortWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetPort with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oLibSysSetup.tearDownSystem();
        
            Result = m_ModDev.eDevGetPort(hDev, &hPort);
            GENTLTEST_CHECK_RESULT("Note: DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call port handle != GENTL_INVALID_HANDLE", hPort == GENTL_INVALID_HANDLE);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetPort::TestDevGetPortWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetPort with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oIFPreCondition.vClose();
        
            Result = m_ModDev.eDevGetPort(hDev, &hPort);
            GENTLTEST_CHECK_RESULT("Note: DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call port handle != GENTL_INVALID_HANDLE", hPort == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetPort::TestDevGetPortWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetPort with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oDevPreCondition.vClose();

            Result = m_ModDev.eDevGetPort(hDev, &hPort);
            GENTLTEST_CHECK_RESULT("Note: DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "DevGetPort InterfaceID=" << ifdev.sInterfaceID.c_str() << 
                " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call port handle != GENTL_INVALID_HANDLE", hPort == GENTL_INVALID_HANDLE);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetPort::TestDevGetPortWithDevIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetPort with parameter device handle = GENTL_INVALID_HANDLE");
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
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Result = m_ModDev.eDevGetPort(GENTL_INVALID_HANDLE, &hPort);
                    GENTLTEST_CHECK_RESULT("Note: DevGetPort InterfaceID=" << xInterfaceList[index1].c_str() << 
                        " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                        "DevGetPort InterfaceID=" << xInterfaceList[index1].c_str() << 
                        " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);
                    
                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call port handle != GENTL_INVALID_HANDLE", hPort == GENTL_INVALID_HANDLE);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetPort::TestDevGetPortWithPortHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetPort with parameter pointer port handle = NULL");
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
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Result = m_ModDev.eDevGetPort(hDev, NULL);
                    GENTLTEST_CHECK_RESULT("Note: DevGetPort InterfaceID=" << xInterfaceList[index1].c_str() << 
                        " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                        "DevGetPort InterfaceID=" << xInterfaceList[index1].c_str() << 
                        " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
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

void Device_DevGetPort::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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
            stIFDevice ifdev;

            sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                ifdev.sInterfaceID = xInterfaceList[index1];
                ifdev.sDeviceID = sDeviceID;
                ifdev.eAccess = eAccess;
                vIFDeviceList.push_back(ifdev);
            }
        }
    }
    
    //GENTLTEST_PRINT("Device_DevGetPort::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
