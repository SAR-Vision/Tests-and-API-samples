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

#include "Port_GCGetNumPortURLs.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCGetNumPortURLs::Port_GCGetNumPortURLs()
{
}

Port_GCGetNumPortURLs::~Port_GCGetNumPortURLs()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetNumPortURLs::setUp(void)
{
}

void Port_GCGetNumPortURLs::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCGetNumPortURLs
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetNumPortURLs::TestGCGetNumPortURLs( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetNumPortURLs");
    // Version 1
    // TLOpenInterface in front of each IFOpenDevice
    {
        LibrarySystemSetup oLibSysSetup;
        tIFDeviceList vIFDeviceList;
        tIFDeviceList::iterator xIter;

        vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

        for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
        {
            uint32_t iNumURLs=0;
            GC_ERROR eResult=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            IF_HANDLE hIF=GENTL_INVALID_HANDLE;
            PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
            stIFDevice ifdev=(stIFDevice)*xIter;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
            Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
            {
                hPort = oDevPreCondition.hDevGetPort();

                eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
                GENTLTEST_CHECK_MESSAGE("GCGetNumPortURLs failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS);
            }
        }
    }

    // Version 2
    // TLOpenInterface only if neccessary
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

                for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
                {
                    GC_ERROR eResult=GC_ERR_SUCCESS;
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                    uint32_t iNumURLs=0;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        hPort = oDevPreCondition.hDevGetPort();
                
                        eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
                        GENTLTEST_CHECK_MESSAGE("GCGetNumPortURLs InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult >= GC_ERR_SUCCESS);

                        if (eResult >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_PRINT("Info: InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " iNumURLs=" << iNumURLs << std::endl);
                        }
                        else
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetNumPortURLs::TestGCGetNumPortURLsWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetNumPortURLs with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        uint32_t iNumURLs=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();

            oLibSysSetup.tearDownLibrary();

            eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
            GENTLTEST_CHECK_MESSAGE("GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_NOT_INITIALIZED);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iNumURLs != 0", iNumURLs == 0);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetNumPortURLs::TestGCGetNumPortURLsWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetNumPortURLs with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        uint32_t iNumURLs=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();

            oLibSysSetup.tearDownSystem();

            eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
            GENTLTEST_CHECK_RESULT("Note: GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iNumURLs != 0", iNumURLs == 0);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetNumPortURLs::TestGCGetNumPortURLsWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetNumPortURLs with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        uint32_t iNumURLs=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oIFPreCondition.vClose();
            
            eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
            GENTLTEST_CHECK_RESULT("Note. GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iNumURLs != 0", iNumURLs == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetNumPortURLs::TestGCGetNumPortURLsWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetNumPortURLs with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        uint32_t iNumURLs=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();

            oDevPreCondition.vClose();

            eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
            GENTLTEST_CHECK_RESULT("Note: GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iNumURLs != 0", iNumURLs == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetNumPortURLs::TestGCGetNumPortURLsPortHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetNumPortURLs with port handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        uint32_t iNumURLs=0;
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();

            eResult=m_ModPort.eGCGetNumPortURLs(GENTL_INVALID_HANDLE, &iNumURLs);
            GENTLTEST_CHECK_RESULT("Note: GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetNumPortURLs InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iNumURLs != 0", iNumURLs == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetNumPortURLs::TestGCGetNumPortURLsWithNumURLsNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetNumPortURLs with parameter piNumURLs = NULL");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();
                
                    eResult=m_ModPort.eGCGetNumPortURLs(hPort, NULL);
                    GENTLTEST_CHECK_RESULT("Note: GCGetNumPortURLs InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult == GC_ERR_INVALID_PARAMETER,
                        "GCGetNumPortURLs InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult < GC_ERR_SUCCESS);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// tools
/////////////////////////////////////////////////////////////////////

void Port_GCGetNumPortURLs::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    tIFDeviceList::iterator xIter;
    
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
                stIFDevice ifdev;

                ifdev.sInterfaceID = xInterfaceList[index1];
                ifdev.sDeviceID = sDeviceID;
                ifdev.eAccess = eAccess;
                vIFDeviceList.push_back(ifdev);
            }
        }
    }

    //GENTLTEST_PRINT("vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
