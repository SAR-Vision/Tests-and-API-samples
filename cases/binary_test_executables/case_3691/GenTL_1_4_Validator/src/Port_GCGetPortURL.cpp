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

#include "Port_GCGetPortURL.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCGetPortURL::Port_GCGetPortURL()
{
}

Port_GCGetPortURL::~Port_GCGetPortURL()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetPortURL::setUp(void)
{
}

void Port_GCGetPortURL::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCGetPortURL
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetPortURL::TestGCGetPortURL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortURL");
    // Version 1
    // TLOpenInterface in front of each IFOpenDevice
    {
        LibrarySystemSetup oLibSysSetup;
        tIFDeviceList vIFDeviceList;
        tIFDeviceList::iterator xIter;

        vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

        for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
        {
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
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
                
                eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("GCGetPortURL size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS);

                if (eResult >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                    std::vector<char> sURL(iSize);
                    eResult=m_ModPort.eGCGetPortURL(hPort, &sURL[0], &iSize);
                    GENTLTEST_CHECK_MESSAGE("GCGetPortURL with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult >= GC_ERR_SUCCESS);

                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize < 2", iSize >= 2);

                    if (eResult >= GC_ERR_SUCCESS)
                    {
                        GenICam::Registry::Impl::CLocalParser oParser;
                        std::string sAddress;

                        GENTLTEST_CHECK_MESSAGE("Parameter after call url missing trailing 0", sURL[sURL.size()-1] == 0);
                        GENTLTEST_CHECK_MESSAGE("Parameter after call url missing trailing 00", sURL[sURL.size()-2] == 0);

                        vParseURL(sURL, oParser);

                        sAddress = oParser.GetFilename();
                        GENTLTEST_CHECK_MESSAGE("Parameter after call no XML filename", sAddress.find(".xml") > 0);
                    }
                }
                else
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << 
                        " LastErrorMessage (" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << ", " << ifdev.sDeviceID.c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                }
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
                    size_t iSize=0;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        hPort = oDevPreCondition.hDevGetPort();     
                        
                        eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
                        GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                            " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult >= GC_ERR_SUCCESS);

                        if (eResult >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                            std::vector<char> sURL(iSize+2);
                            eResult=m_ModPort.eGCGetPortURL(hPort, &sURL[0], &iSize);
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                                " with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS);

                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize < 2", iSize >= 2);

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GenICam::Registry::Impl::CLocalParser oParser;
                                std::string sAddress;

                                GENTLTEST_CHECK_MESSAGE("Parameter after call url missing trailing 0", sURL[sURL.size()-1] == 0);
                                GENTLTEST_CHECK_MESSAGE("Parameter after call url missing trailing 00", sURL[sURL.size()-2] == 0);

                                vParseURL(sURL, oParser);

                                sAddress = oParser.GetFilename();
                                GENTLTEST_CHECK_MESSAGE("Parameter after call no XML filename", sAddress.find(".xml") > 0);

                                GENTLTEST_PRINT("Info: PortURL(" << sConvertDEVICEAccess2String(eAccess).c_str() << ", " << sDeviceID.c_str() << ")='" << &sURL[0] << "'" << std::endl);
                            }
                        }
                        else
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << 
                                " LastErrorMessage (" << sConvertDEVICEAccess2String(eAccess).c_str() << ", " << sDeviceID.c_str() << "): '" << sMsg.c_str() << "'" << std::endl);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with closed library before");
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
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oLibSysSetup.tearDownLibrary();

            eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("GCGetPortURL InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_NOT_INITIALIZED);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with closed system before");
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
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oLibSysSetup.tearDownSystem();

            eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortURL InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetPortURL InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with closed interface before");
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
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oIFPreCondition.vClose();
            
            eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortURL InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetPortURL InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with closed device before");
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
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oDevPreCondition.vClose();

            eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note. GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithPortHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with parameter port handle = GENTL_INVALID_HANDLE");
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
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();
                            
                    eResult=m_ModPort.eGCGetPortURL(GENTL_INVALID_HANDLE, NULL, &iSize);
                    GENTLTEST_CHECK_RESULT("Note: GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                        "GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult < GC_ERR_SUCCESS);

                    if (eResult < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with parameter piSize = NULL");
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
                            
                    eResult=m_ModPort.eGCGetPortURL(hPort, NULL, NULL);
                    GENTLTEST_CHECK_RESULT("Note: GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult == GC_ERR_INVALID_PARAMETER,
                        "GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult < GC_ERR_SUCCESS);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURL::TestGCGetPortURLWithSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURL with parameter iSize = iSize - 1 at second call with initialized buffer");
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
                size_t iSize=0;
                size_t iSizeSave=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();         
                    
                    eResult=m_ModPort.eGCGetPortURL(hPort, NULL, &iSize);
                    GENTLTEST_CHECK_MESSAGE("GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult >= GC_ERR_SUCCESS);

                    if (eResult >= GC_ERR_SUCCESS)
                    {
                        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                        std::vector<char> sURL(iSize);
                        iSize--;
                        iSizeSave = iSize;
                        eResult=m_ModPort.eGCGetPortURL(hPort, &sURL[0], &iSize);
                        GENTLTEST_CHECK_RESULT("Note: GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult == GC_ERR_INVALID_PARAMETER,
                            "GCGetPortURL InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                            " with initialized buffer failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult < GC_ERR_SUCCESS);

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);
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

void Port_GCGetPortURL::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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

void Port_GCGetPortURL::vParseURL(std::vector<char> cURL, GenICam::Registry::Impl::CLocalParser &parser)
{
    // get URL list
    std::vector<std::string> urls;
    for(size_t i = 0; (i < (cURL.size()-1)) && (cURL[i] != '\0'); i+=(strlen(&cURL[i])+1))
    {
        if (strlen(&cURL[i]) > 0)
            urls.push_back(std::string(&cURL[i]));
    }
    GENTLTEST_CHECK_MESSAGE("No URL found", urls.size() > 0);
    std::string strURL = urls[0];

    // pre-parse URL
    {
        GenICam::Registry::Impl::CURLParser parser;
        parser.Parse(strURL);

        if(!_stricmp(parser.GetScheme().c_str(), "local") == 0)
        {
            GENTLTEST_CHECK_MESSAGE("No local XML .. Test not supported yet", cURL.size() != 0);
        }
    }
    parser.Parse(strURL);
}

