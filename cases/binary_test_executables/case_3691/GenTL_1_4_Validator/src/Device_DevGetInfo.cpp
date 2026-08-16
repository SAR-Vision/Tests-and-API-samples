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

#include "Device_DevGetInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Device_DevGetInfo::Device_DevGetInfo()
{
}

Device_DevGetInfo::~Device_DevGetInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetInfo::setUp(void)
{
}

void Device_DevGetInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DevGetInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetInfo::TestDevGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DevGetInfo");
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
                    int nCommandCounter=0;

                    for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                        size_t iSize = 0;
                    
                        Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_MESSAGE("DevGetInfo size check command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " interfaceID=" << xInterfaceList[index1].c_str() <<
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS ||  == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                        
                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                            nCommandCounter++;
                            std::vector<char> sBuffer(iSize);
                            Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, &sBuffer[0], &iSize);
                            GENTLTEST_CHECK_MESSAGE("DevGetInfo with initialized buffer command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                " interfaceID=" << xInterfaceList[index1].c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                                Result >= GC_ERR_SUCCESS);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                std::stringstream sTemp;
                                
                                if (eCommand == DEVICE_INFO_ACCESS_STATUS)
                                {
                                    GENTLTEST_PRINT("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                        ", DeviceID=" << sDeviceID.c_str() << 
                                        ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        ") " << sConvertDeviceCommand2String(eCommand).c_str() <<
                                        " : " << sConvertAccessStatus2String((int)sBuffer[0]).c_str() << std::endl);
                                }
                                else 
                                {
                                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                        ", DeviceID=" << sDeviceID.c_str() << 
                                        ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        ") " << sConvertDeviceCommand2String(eCommand).c_str(),
                                        iType, &sBuffer[0], iSize);
                                }

                                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                switch (eCommand)
                                {
                                case DEVICE_INFO_ID:
                                case DEVICE_INFO_VENDOR:
                                case DEVICE_INFO_MODEL:
                                case DEVICE_INFO_DISPLAYNAME:
                                case DEVICE_INFO_USER_DEFINED_NAME:
                                case DEVICE_INFO_SERIAL_NUMBER:
                                case DEVICE_INFO_VERSION:
                                    GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                    break;
                                case DEVICE_INFO_TLTYPE:
                                    GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                    
                                    if (iType == INFO_DATATYPE_STRING)
                                    {
                                        std::string sTLType=&sBuffer[0];
                                        GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << 
                                            " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                            sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                            sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                    }
                                    break;
                                case DEVICE_INFO_ACCESS_STATUS:
                                    GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_INT32 type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
                                    break;
                                case DEVICE_INFO_TIMESTAMP_FREQUENCY:
                                    GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_UINT64 type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_UINT64); 
                                    break;
                                }
                            }
                        }
                        else
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            if (Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE)
                            {
                                GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                                    " LastErrorMessage(DeviceID="<<sDeviceID.c_str()<<" "<<sConvertDeviceCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                            }
                            else
                            {
                                GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                                    " LastErrorMessage(DeviceID="<<sDeviceID.c_str()<<" "<<sConvertDeviceCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                            }
                        }
                    }

                    if (nCommandCounter == 0)
                    {
                        GENTLTEST_CHECK_MESSAGE("DevGetInfo device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. No commands implemented.", false);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oLibSysSetup.tearDownLibrary();
            
            Result = m_ModDev.eDevGetInfo(hDev, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
            
            if (Result < GC_ERR_SUCCESS)
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

void Device_DevGetInfo::TestDevGetInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oLibSysSetup.tearDownSystem();
            
            Result = m_ModDev.eDevGetInfo(hDev, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                Result < GC_ERR_SUCCESS);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
            
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oIFPreCondition.vClose();
            
            Result = m_ModDev.eDevGetInfo(hDev, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                Result < GC_ERR_SUCCESS);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            oDevPreCondition.vClose();

            Result = m_ModDev.eDevGetInfo(hDev, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "DevGetInfo command=" << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
                " interfaceID=" << ifdev.sInterfaceID.c_str() <<
                " device access=" << sConvertDEVICEAccess2String(ifdev.eAccess).c_str() <<
                " DeviceID=" << ifdev.sDeviceID.c_str() << 
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                Result < GC_ERR_SUCCESS);
            
            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with parameter piSize = NULL");
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
                    for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                        
                        Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, NULL, NULL);
                        GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " interfaceID=" << xInterfaceList[index1].c_str() <<
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                            "DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " interfaceID=" << xInterfaceList[index1].c_str() <<
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                            Result < GC_ERR_SUCCESS);
                    
                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                        }                   
                    }
                }
            }
        }
    }           

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with parameter piType = NULL at second call with initialized buffer");
    
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
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            size_t iSize = 0;
                            size_t iSizeSave = 0;
                            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                        
                            Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                " interfaceID=" << xInterfaceList[index1].c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                                iSizeSave = iSize;

                                std::vector<char> sBuffer(iSize);
                                Result = m_ModDev.eDevGetInfo(hDev, eCommand, NULL, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                    " interfaceID=" << xInterfaceList[index1].c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " result value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(),  
                                    Result == GC_ERR_INVALID_PARAMETER,
                                    "DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                    " interfaceID=" << xInterfaceList[index1].c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                                    Result < GC_ERR_SUCCESS);
                        
                                if (Result < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);
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

void Device_DevGetInfo::TestDevGetInfoWithDevIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with parameter device handle = GENTL_INVALID_HANDLE");
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
                    for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                        size_t iSize = 0;
                        
                        Result = m_ModDev.eDevGetInfo(GENTL_INVALID_HANDLE, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " interfaceID=" << xInterfaceList[index1].c_str() <<
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                            "DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " interfaceID=" << xInterfaceList[index1].c_str() <<
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                            Result < GC_ERR_SUCCESS);
                    
                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                        }                   
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with parameter iSize = 0 at second call with initialized buffer");
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
                    for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                        Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
                        size_t iSize = 0;
                        
                        Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_MESSAGE("DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " interfaceID=" << xInterfaceList[index1].c_str() <<
                            " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            " DeviceID=" << sDeviceID.c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),  
                            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                            iTypeSave = iType;
                        
                            std::vector<char> sBuffer(iSize);
                            iSize = 0;
                            Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, &sBuffer[0], &iSize);
                            GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                " interfaceID=" << xInterfaceList[index1].c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(),  
                                Result == GC_ERR_INVALID_PARAMETER,
                                "DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                " interfaceID=" << xInterfaceList[index1].c_str() <<
                                " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                " DeviceID=" << sDeviceID.c_str() << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                                Result < GC_ERR_SUCCESS);
                        
                            if (Result < GC_ERR_SUCCESS)
                            {
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iType == iTypeSave);
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                            }                   
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetInfo::TestDevGetInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with parameter piSize = NULL at second call with initialized buffer");
    
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
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                
                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                            Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
                            size_t iSize = 0;
                            size_t iSizeSave = 0;
                
                            Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                    
                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                                iTypeSave = iType;
                                iSizeSave = iSize;
                        
                                std::vector<char> sBuffer(iSize);
                                Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, &sBuffer[0], NULL);
                                GENTLTEST_CHECK_RESULT("Note: DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                    " interfaceID=" << xInterfaceList[index1].c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " result value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(),  
                                    Result == GC_ERR_INVALID_PARAMETER,
                                    "DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                    " interfaceID=" << xInterfaceList[index1].c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                                    Result < GC_ERR_SUCCESS);
                        
                                if (Result < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iType == iTypeSave);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSize == iSizeSave);
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

void Device_DevGetInfo::TestDevGetInfoBufferWithSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetInfo with parameter iSize = iSize-1 at second call with initialized buffer");
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
                    for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                        Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
                        size_t iSize = 0;
                        size_t iSizeSave = 0;
                
                        Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                    
                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                            iTypeSave = iType;
                            
                            if (iSize > 1)
                            {
                                std::vector<char> sBuffer(iSize);
                                iSize--;
                            
                                iSizeSave = iSize;
                        
                                Result = m_ModDev.eDevGetInfo(hDev, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_RESULT("DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                    " interfaceID=" << xInterfaceList[index1].c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(),  
                                    Result == GC_ERR_INVALID_PARAMETER,
                                    "DevGetInfo command=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                                    " interfaceID=" << xInterfaceList[index1].c_str() <<
                                    " device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                    " DeviceID=" << sDeviceID.c_str() << 
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),  
                                    Result < GC_ERR_SUCCESS);
                        
                                if (Result < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iType == iTypeSave);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSize == iSizeSave);
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

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Device_DevGetInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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
                for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                {
                    stIFDevice ifdev;

                    ifdev.sInterfaceID = xInterfaceList[index1];
                    ifdev.sDeviceID = sDeviceID;
                    ifdev.eAccess = eAccess;
                    ifdev.eCommand = eCommand;
                    vIFDeviceList.push_back(ifdev);
                }
            }
        }
    }

    //GENTLTEST_PRINT("Device_DevGetInfo::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
