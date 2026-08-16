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

#include "GenTLTestTools.h"
#include "Interface_IFGetInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "URLParser.h"
#include "LocalParser.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFGetInfo::Interface_IFGetInfo()
{
}

Interface_IFGetInfo::~Interface_IFGetInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetInfo::setUp(void)
{
}

void Interface_IFGetInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFGetInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetInfo::TestIFGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFGetInfo");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF);
        int nCommandCounter=0;

        for (INTERFACE_INFO_CMD eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            size_t iSizeSave = 0;

            Result = m_ModIF.eIFGetInfo(hIF, eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") size check failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                std::vector<char> sBuffer(iSize);

                nCommandCounter++;
                iSizeSave = iSize;

                Result = m_ModIF.eIFGetInfo(hIF, eCommand, &iType, &sBuffer[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                    ") with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index].c_str() << ") " << sConvertInterfaceCommand2String(eCommand).c_str(), iType, &sBuffer[0], iSize);

                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                    switch (eCommand)
                    {
                    case INTERFACE_INFO_ID:
                    case INTERFACE_INFO_DISPLAYNAME:
                        GENTLTEST_CHECK_MESSAGE(sConvertInterfaceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                        break;
                    case INTERFACE_INFO_TLTYPE:
                        GENTLTEST_CHECK_MESSAGE(sConvertInterfaceCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                        
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            std::string sTLType=&sBuffer[0];
                            GENTLTEST_CHECK_MESSAGE(sConvertInterfaceCommand2String(eCommand).c_str() << 
                                " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                        }
                        break;
                    }
                }
            }
            else
            {
                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                if (Result == GC_ERR_NOT_IMPLEMENTED)
                {
                    GENTLTEST_PRINT("Hint: InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                        " " << sConvertGCError2String(Result).c_str() << 
                        " message: '" << sMsg.c_str() << "'" << std::endl);
                }
                else
                {
                    GENTLTEST_PRINT("Error: InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                        " " << sConvertGCError2String(Result).c_str() << 
                        " message: '" << sMsg.c_str() << "'" << std::endl);
                }
            }
        }

        if (nCommandCounter == 0)
        {
            GENTLTEST_CHECK_MESSAGE("IFGetInfo interface index=" << index <<
                " failed. No commands implemented.", false);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetInfo::TestIFGetInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        oLibSysSetup.tearDownLibrary();
        
        GC_ERROR Result = m_ModIF.eIFGetInfo(hIF, ifdev.eCommand, &iType, NULL, &iSize);
        
        GENTLTEST_CHECK_MESSAGE("IFGetInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " (" << sConvertInterfaceCommand2String(ifdev.eCommand).c_str() <<
            ") failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
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

void Interface_IFGetInfo::TestIFGetInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        oLibSysSetup.tearDownSystem();
        
        GC_ERROR Result = m_ModIF.eIFGetInfo(hIF, ifdev.eCommand, &iType, NULL, &iSize);
        
        GENTLTEST_CHECK_RESULT("Note: IFGetInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " (" << sConvertInterfaceCommand2String(ifdev.eCommand).c_str() << 
            ") return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
            "IFGetInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " (" << sConvertInterfaceCommand2String(ifdev.eCommand).c_str() << 
            ") failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
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

void Interface_IFGetInfo::TestIFGetInfoWithHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with interface handel GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF);

        for (INTERFACE_INFO_CMD eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            GC_ERROR Result = m_ModIF.eIFGetInfo(GENTL_INVALID_HANDLE, eCommand, &iType, NULL, &iSize);
            
            GENTLTEST_CHECK_RESULT("Note: IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
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

void Interface_IFGetInfo::TestIFGetInfoWithInvalidCommand( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with invalid command=INTERFACE_INFO_TLTYPE+1");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF);

        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        GC_ERROR Result=GC_ERR_SUCCESS;
        
        Result = m_ModIF.eIFGetInfo(hIF, INTERFACE_INFO_TLTYPE+1, &iType, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << 
            " (INTERFACE_INFO_TLTYPE+1) return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_AVAILABLE,
            "IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << 
            " (INTERFACE_INFO_TLTYPE+1) failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetInfo::TestIFGetInfoWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        {
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF);
        }

        
        for (INTERFACE_INFO_CMD eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            GC_ERROR Result = m_ModIF.eIFGetInfo(hIF, eCommand, &iType, NULL, &iSize);
            
            GENTLTEST_CHECK_RESULT("Note: IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
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

void Interface_IFGetInfo::TestIFGetInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with parameter piSize = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF);

        for (INTERFACE_INFO_CMD eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            GC_ERROR Result = m_ModIF.eIFGetInfo(hIF, eCommand, &iType, NULL, NULL);
            
            GENTLTEST_CHECK_RESULT("Note: IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                ") failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetInfo::TestIFGetInfoWithTypeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetInfo with piType = NULL at second call with initialized buffer");
    
    if (!g_SkipTypeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        GC_ERROR Result=GC_ERR_SUCCESS;
        System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

        for (uint32_t index=0; index<xInterfaceList.size(); index++)
        {
            IF_HANDLE hIF = GENTL_INVALID_HANDLE;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF);

            for (INTERFACE_INFO_CMD eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD)(eCommand+1))
            {
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
                size_t iSizeSave = 0;
                Result = m_ModIF.eIFGetInfo(hIF, eCommand, &iType, NULL, &iSize);
            
                GENTLTEST_CHECK_MESSAGE("IFGetInfo InterfaceID=" << xInterfaceList[index].c_str() << " (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                    ") size check failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                    iSizeSave = iSize;

                    std::vector<char> Buff(iSize);
                    Result = m_ModIF.eIFGetInfo(hIF, eCommand, NULL, &Buff[0], &iSize);

                    GENTLTEST_CHECK_RESULT("Note: IFGetInfo (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                        ") return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_PARAMETER,
                        "IFGetInfo (" << sConvertInterfaceCommand2String(eCommand).c_str() << 
                        ") with initialized buffer failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not old size", iSizeSave == iSize);
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

void Interface_IFGetInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        for (INTERFACE_INFO_CMD eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD)(eCommand+1))
        {
            stIFDevice ifdev;

            ifdev.sInterfaceID = xInterfaceList[index1];
            ifdev.eCommand = eCommand;
            vIFDeviceList.push_back(ifdev);
        }
    }

    //GENTLTEST_PRINT("Interface_IFOpenDevice::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
