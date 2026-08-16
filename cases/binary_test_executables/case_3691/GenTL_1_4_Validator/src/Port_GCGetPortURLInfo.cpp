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

#include "Port_GCGetPortURLInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

#define SHA1_HASH_SIZE      20

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCGetPortURLInfo::Port_GCGetPortURLInfo()
{
}

Port_GCGetPortURLInfo::~Port_GCGetPortURLInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetPortURLInfo::setUp(void)
{
}

void Port_GCGetPortURLInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCGetPortURLInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetPortURLInfo::TestGCGetPortURLInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortURLInfo");
    // Version 1
    // TLOpenInterface in front of each IFOpenDevice
    {
        LibrarySystemSetup oLibSysSetup;
        tIFDeviceList vIFDeviceList;
        tIFDeviceList::iterator xIter;

        vCreateTestCaseList(oLibSysSetup, vIFDeviceList);
        
        for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
        {
            INFO_DATATYPE iType;
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

                eResult=m_ModPort.eGCGetPortURLInfo(hPort, ifdev.iNumURLIndex, ifdev.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("GCGetPortURLInfo failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                if (eResult >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                    std::vector<char> sURLInfo(iSize);
                    eResult=m_ModPort.eGCGetPortURLInfo(hPort, ifdev.iNumURLIndex, ifdev.eCommand, &iType, &sURLInfo[0], &iSize);
                    GENTLTEST_CHECK_MESSAGE("GCGetPortURLInfo failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED);

                    if (eResult >= GC_ERR_SUCCESS)
                    {
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize < 1", iSize >= 1);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call missing trailing 0", sURLInfo[sURLInfo.size()-1] == 0);

                            //GENTLTEST_PRINT("\nInfo: GCGetPortURLInfo(%s, INFO_DATATYPE_STRING) = '%s'\n", sConvertURLInfoCommand2String(ifdev.eCommand).c_str(), &sURLInfo[0]);
                        } 
                        else if (iType == INFO_DATATYPE_STRINGLIST)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize < 2", iSize >= 2);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call missing trailing 0", sURLInfo[sURLInfo.size()-1] == 0);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call missing trailing 00", sURLInfo[sURLInfo.size()-2] == 0);

                            //GENTLTEST_PRINT("\nInfo: GCGetPortURLInfo(%s, INFO_DATATYPE_STRINGLIST) = '%s'\n", sConvertURLInfoCommand2String(ifdev.eCommand).c_str(), &sURLInfo[0]);
                        }
                        else {
                            //GENTLTEST_PRINT("\nInfo: GCGetPortURLInfo(%s, %s) = '%d'\n", sConvertURLInfoCommand2String(ifdev.eCommand).c_str(), sConvertDataType2String(iType).c_str(), sURLInfo[0]);
                        }
                    }
                }
                else
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    GENTLTEST_PRINT("Info: GCGetPortURLInfo of " << sConvertURLInfoCommand2String(ifdev.eCommand) << " returned " << sConvertGCError2String(eResult).c_str() << 
                        " LastErrorMessage("<<sConvertDEVICEAccess2String(ifdev.eAccess).c_str()<<", "<<ifdev.sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
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
                    uint32_t iNumURLs=0;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        hPort = oDevPreCondition.hDevGetPort();

                        eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
                        GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                                
                        for (uint32_t index3=0; index3<iNumURLs; index3++)
                        {
                            for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                            {
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;
                        
                                eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_MESSAGE("GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                                if (eResult >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    std::vector<char> sURLInfo(iSize);
                                    eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, &sURLInfo[0], &iSize);
                                    GENTLTEST_CHECK_MESSAGE("GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult >= GC_ERR_SUCCESS);

                                    if (eResult >= GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                            ", DeviceID=" << sDeviceID.c_str() << 
                                            ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            ") " << sConvertURLInfoCommand2String(eCommand).c_str(),
                                            iType, &sURLInfo[0], iSize);

                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize < 1", iSize >= 1);
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call missing trailing 0", sURLInfo[sURLInfo.size()-1] == 0);
                                        } 
                                        else if (iType == INFO_DATATYPE_STRINGLIST)
                                        {
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize < 2", iSize >= 2);
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call missing trailing 0", sURLInfo[sURLInfo.size()-1] == 0);
                                            GENTLTEST_CHECK_MESSAGE("Parameter after call missing trailing 00", sURLInfo[sURLInfo.size()-2] == 0);
                                        }

                                        switch (eCommand)
                                        {
                                        case URL_INFO_URL: 
                                            GENTLTEST_CHECK_MESSAGE(sConvertURLInfoCommand2String(eCommand).c_str() << " wrong data: expected INFO_DATATYPE_STRING type=" << 
                                                sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                            break;
                                        case URL_INFO_SCHEMA_VER_MAJOR:
                                        case URL_INFO_SCHEMA_VER_MINOR:
                                        case URL_INFO_FILE_VER_MAJOR:
                                        case URL_INFO_FILE_VER_MINOR:
                                        case URL_INFO_FILE_VER_SUBMINOR: 
                                            GENTLTEST_CHECK_MESSAGE(sConvertURLInfoCommand2String(eCommand).c_str() << " wrong data: expected INFO_DATATYPE_INT32 type=" << 
                                                sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
                                            break;
                                        case URL_INFO_FILE_SHA1_HASH:
                                            GENTLTEST_CHECK_MESSAGE(sConvertURLInfoCommand2String(eCommand).c_str() << " wrong data: expected INFO_DATATYPE_BUFFER type=" << 
                                                sConvertDataType2String(iType), iType == INFO_DATATYPE_BUFFER); 

                                            if (iType == INFO_DATATYPE_BUFFER)
                                            {
                                                GENTLTEST_CHECK_MESSAGE(sConvertURLInfoCommand2String(eCommand).c_str() << " wrong data size received, expected 20 Bytes, received " << iSize << " Bytes.",
                                                    iSize == SHA1_HASH_SIZE);
                                            }
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    if (eResult != GC_ERR_NOT_IMPLEMENTED && eResult != GC_ERR_NOT_AVAILABLE)
                                    {
                                        std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                        GENTLTEST_PRINT("Info: GCGetPortURLInfo of " << sConvertURLInfoCommand2String(eCommand) << " returned " << sConvertGCError2String(eResult).c_str() << 
                                            " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                    }
                                    else
                                    {
                                        GENTLTEST_PRINT("Hint: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                            " returned GC_ERR_NOT_IMPLEMENTED" << std::endl);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with closed library before");
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
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oLibSysSetup.tearDownLibrary();

            eResult=m_ModPort.eGCGetPortURLInfo(hPort, ifdev.iNumURLIndex, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_NOT_INITIALIZED || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with closed system before");
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
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
        
            oLibSysSetup.tearDownSystem();

            eResult=m_ModPort.eGCGetPortURLInfo(hPort, ifdev.iNumURLIndex, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with closed interface before");
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
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
        
            oIFPreCondition.vClose();
            
            eResult=m_ModPort.eGCGetPortURLInfo(hPort, ifdev.iNumURLIndex, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with closed device before");
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
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
            
            oDevPreCondition.vClose();
            
            eResult=m_ModPort.eGCGetPortURLInfo(hPort, ifdev.iNumURLIndex, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoPortHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with port handle = GENTL_INVALID_HANDLE");
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
        
            eResult=m_ModPort.eGCGetPortURLInfo(GENTL_INVALID_HANDLE, ifdev.iNumURLIndex, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortURLInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertURLInfoCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);
            
            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter piSize = NULL");
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
                    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                    for (uint32_t index3=0; index3<iNumURLs; index3++)
                    {
                        for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            
                            eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, NULL, NULL);
                            GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                                "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult < GC_ERR_SUCCESS);

                            if (eResult < GC_ERR_SUCCESS)
                            {
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter piType = NULL at second call with initialized buffer");
    
    if (!g_SkipTypeNULLCheck)
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
                        GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                        for (uint32_t index3=0; index3<iNumURLs; index3++)
                        {
                            for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                            {
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;
                            
                                eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK_MESSAGE("GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                                if (eResult >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    size_t iSaveSize = iSize;
                                    std::vector<char> sBuffer(iSize);
                                    eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, NULL, &sBuffer[0], &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult == GC_ERR_INVALID_PARAMETER,
                                        "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult < GC_ERR_SUCCESS);

                                    if (eResult < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old size", iSize == iSaveSize);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter iSize = 0 at second call with initialized buffer");
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
                    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                    for (uint32_t index3=0; index3<iNumURLs; index3++)
                    {
                        for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            INFO_DATATYPE iTypeSave=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                            
                            eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                iTypeSave = iType;

                                std::vector<char> sURLInfo(iSize);
                                iSize = 0;
                                eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, &sURLInfo[0], &iSize);
                                GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED,
                                    "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult < GC_ERR_SUCCESS);

                                if (eResult < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call isize != 0", iSize == 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter piSize = NULL at second call with initialized buffer");
    
    if (!g_SkipSizeNULLCheck)
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
                        GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                        for (uint32_t index3=0; index3<iNumURLs; index3++)
                        {
                            for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                            {
                                INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                                INFO_DATATYPE iTypeSave=INFO_DATATYPE_UNKNOWN;
                                size_t iSize=0;
                            
                                eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, NULL, &iSize);
                                GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                                if (eResult >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                    iTypeSave = iType;

                                    std::vector<char> sURLInfo(iSize);
                                    eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, &sURLInfo[0], NULL);
                                    GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult == GC_ERR_INVALID_PARAMETER,
                                        "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult < GC_ERR_SUCCESS);

                                    if (eResult < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoBufferWithSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter iSize = iSize - 1 at second call with initialized buffer");
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
                    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                    for (uint32_t index3=0; index3<iNumURLs; index3++)
                    {
                        for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            INFO_DATATYPE iTypeSave=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                            
                            eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                iTypeSave = iType;

                                std::vector<char> sURLInfo(iSize);
                                iSize --;
                                if (iSize > 0)
                                {
                                    eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, eCommand, &iType, &sURLInfo[0], &iSize);
                                    GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult == GC_ERR_INVALID_PARAMETER,
                                        "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                                        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                        eResult < GC_ERR_SUCCESS);

                                    if (eResult < GC_ERR_SUCCESS)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithInvalidCommand(uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter iInfoCmd = URL_INFO_FILE_SHA1_HASH + 1");
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
                    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                    for (uint32_t index3=0; index3<iNumURLs; index3++)
                    {
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        size_t iSize=0;
                        
                        eResult=m_ModPort.eGCGetPortURLInfo(hPort, index3, URL_INFO_FILE_SHA1_HASH+1, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " URL_INFO_FILE_SHA1_HASH+1" <<
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                            "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << index3+1 << "/" << iNumURLs << " URL_INFO_FILE_SHA1_HASH+1" <<
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult < GC_ERR_SUCCESS);

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", INFO_DATATYPE_UNKNOWN == iType);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortURLInfo::TestGCGetPortURLInfoWithInvalidIndex( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortURLInfo with parameter iURLIndex = iNumURLs + 1");
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
                    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
                    
                    for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                    {
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        INFO_DATATYPE iTypeSave=INFO_DATATYPE_UNKNOWN;
                        size_t iSize=0;
                        
                        eResult=m_ModPort.eGCGetPortURLInfo(hPort, iNumURLs+1, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=iNumURLs+1" << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                            "GCGetPortURLInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=iNumURLs+1" << "/" << iNumURLs << " " << sConvertURLInfoCommand2String(eCommand).c_str() <<
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult < GC_ERR_SUCCESS);

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// tools
/////////////////////////////////////////////////////////////////////

void Port_GCGetPortURLInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    tIFDeviceList::iterator xIter;
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID;
            
            sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                uint32_t iNumURLs=0;
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();         
                    
                    eResult=m_ModPort.eGCGetNumPortURLs(hPort, &iNumURLs);
                    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);

                    for (uint32_t index3=0; index3<iNumURLs; index3++)
                    {
                        for (URL_INFO_CMD_LIST eCommand=URL_INFO_URL; eCommand<=URL_INFO_FILE_SHA1_HASH; eCommand=(URL_INFO_CMD_LIST)(eCommand+1))
                        {
                            stIFDevice ifdev;

                            ifdev.sInterfaceID = xInterfaceList[index1];
                            ifdev.sDeviceID = sDeviceID;
                            ifdev.eAccess = eAccess;
                            ifdev.iNumURLIndex = index3;
                            ifdev.eCommand = eCommand;
                            vIFDeviceList.push_back(ifdev);
                        }
                    }
                }
            }
        }
    }

    //GENTLTEST_PRINT("vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
