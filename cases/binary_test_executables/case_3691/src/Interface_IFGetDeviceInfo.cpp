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

#include "Interface_IFGetDeviceInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFGetDeviceInfo::Interface_IFGetDeviceInfo()
{
}

Interface_IFGetDeviceInfo::~Interface_IFGetDeviceInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetDeviceInfo::setUp(void)
{
}

void Interface_IFGetDeviceInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLOpenInterface
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFGetDeviceInfo");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    uint32_t uiTotalNumDevices = 0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        uiTotalNumDevices += uiNumDevices;

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
            
                Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " check size failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                
                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                    std::vector<char> sBuffer(iSize);
                    Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, &sBuffer[0], &iSize);
                    GENTLTEST_CHECK_MESSAGE("IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sConvertDeviceCommand2String(eCommand).c_str() << 
                        " with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result >= GC_ERR_SUCCESS);

                    if (Result >= GC_ERR_SUCCESS)
                    {
                        if (eCommand == DEVICE_INFO_ACCESS_STATUS)
                        {
                            GENTLTEST_PRINT("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                    ", DeviceID=" << sDeviceID.c_str() << ") " << sConvertDeviceCommand2String(eCommand).c_str() <<
                                    " : " << sConvertAccessStatus2String((int)sBuffer[0]).c_str() << std::endl);
                        }
                        else
                        {
                            GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                    ", DeviceID=" << sDeviceID.c_str() << ") " << sConvertDeviceCommand2String(eCommand).c_str(),
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
                            GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                            break;
                        case DEVICE_INFO_TLTYPE:
                            GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                            
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
                            GENTLTEST_CHECK_MESSAGE(sConvertDeviceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_INT32 type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
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
                        GENTLTEST_PRINT("Hint: InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                    }
                    else
                    {
                        GENTLTEST_PRINT("Error: InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                    }
                }
            }
        }
    }

    // for certification we need at least one device
    GENTLTEST_CHECK_MESSAGE(std::endl << "Error: ***********************************************************" << std::endl <<
        "Error: * For GenICam Validation at least one device should exist *" << std::endl <<
        "Error: * Returned uiTotalNumDevices = " << uiTotalNumDevices << " *" << std::endl <<
        "Error: * Aborting validation                                     *" << std::endl <<
        "Error: ***********************************************************" << std::endl, 
        uiTotalNumDevices > 0);

    if (uiTotalNumDevices == 0)
        g_bStopValidationFlag = true;

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        oLibSysSetup.tearDownLibrary();
        
        GC_ERROR Result=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
    
        Result = m_ModIF.eIFGetDeviceInfo(hIF, ifdev.sDeviceID.c_str(), ifdev.eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("IFGetDeviceInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " " << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        oLibSysSetup.tearDownSystem();
        
        GC_ERROR Result=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
    
        Result = m_ModIF.eIFGetDeviceInfo(hIF, ifdev.sDeviceID.c_str(), ifdev.eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " " << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
            "IFGetDeviceInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " " << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
        }
        
        oLibSysSetup.setUpSystem();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        {
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        }

        GC_ERROR Result=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
    
        Result = m_ModIF.eIFGetDeviceInfo(hIF, ifdev.sDeviceID.c_str(), ifdev.eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " " << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
            "IFGetDeviceInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " " << sConvertDeviceCommand2String(ifdev.eCommand).c_str() << 
            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with parameter piSize = NULL");
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

            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                
                Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, NULL, NULL);
                GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with parameter piType = NULL at second call with initialized buffer");
    
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

                for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                {
                    GC_ERROR Result=GC_ERR_SUCCESS;
                    size_t iSize = 0;
                    Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                
                    Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, NULL, &iSize);
                    GENTLTEST_CHECK_MESSAGE("IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                    if (Result >= GC_ERR_SUCCESS)
                    {
                        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                        size_t iSaveSize = iSize;
                        std::vector<char> sBuffer(iSize);
                        Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, NULL, &sBuffer[0], &iSize);
                        GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER,
                            "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old size", iSize == iSaveSize);
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

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithIFNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with interface handle GENTL_INVALID_HANDLE");
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

            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
                
                Result = m_ModIF.eIFGetDeviceInfo(GENTL_INVALID_HANDLE, sDeviceID.c_str(), eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithDevIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with pointer sDeviceID = NULL");
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

            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
                
                Result = m_ModIF.eIFGetDeviceInfo(hIF, NULL, eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                    "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoWithWrongDevID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with phantasy sDeviceID");
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

            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
                
                Result = m_ModIF.eIFGetDeviceInfo(hIF, "Hugo", eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_ID or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_ID || Result == GC_ERR_NOT_AVAILABLE,
                    "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with parameter iSize = 0 at second call with initialized buffer");
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

            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
                
                Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
                
                iTypeSave = iType;
                
                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                    std::vector<char> sBuffer(iSize);
                    iSize = 0;
                    Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, &sBuffer[0], &iSize);
                    GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_BUFFER,
                        "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
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

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceInfo::TestIFGetDeviceInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceInfo with parameter piSize = NULL at second call with initialized buffer");
    
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

                for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
                {
                    GC_ERROR Result=GC_ERR_SUCCESS;
                    Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                    Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
                    size_t iSize = 0;
                    size_t iSizeSave = 0;
            
                    Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, NULL, &iSize);
                    GENTLTEST_CHECK_MESSAGE("IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                        " size check failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                    if (Result >= GC_ERR_SUCCESS)
                    {
                        iTypeSave = iType;
                        iSizeSave = iSize;

                        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                        std::vector<char> sBuffer(iSize);
                        Result = m_ModIF.eIFGetDeviceInfo(hIF, sDeviceID.c_str(), eCommand, &iType, &sBuffer[0], NULL);
                        GENTLTEST_CHECK_RESULT("Note: IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER,
                            "IFGetDeviceInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertDeviceCommand2String(eCommand).c_str() << 
                            " with initialized buffer failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
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
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Interface_IFGetDeviceInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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
            for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
            {
                std::string sDeviceID;
                stIFDevice ifdev;

                sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);
                ifdev.sInterfaceID = xInterfaceList[index1];
                ifdev.sDeviceID = sDeviceID;
                ifdev.eCommand = eCommand;
                vIFDeviceList.push_back(ifdev);
            }
        }
    }

    //GENTLTEST_PRINT("Interface_IFGetDeviceInfo::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
