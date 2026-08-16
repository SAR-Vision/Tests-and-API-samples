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

#include "System_TLGetInterfaceInfo.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_TLGetInterfaceInfo::System_TLGetInterfaceInfo()
{
}

System_TLGetInterfaceInfo::~System_TLGetInterfaceInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetInterfaceInfo::setUp(void)
{
}

void System_TLGetInterfaceInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLOpenInterface
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLGetInterfaceInfo");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nCommandCounter=0;
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            
            Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo INTERFACE_INFO_CMD=" << sConvertInterfaceCommand2String(eCommand).c_str() <<
                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
            
            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
            
                std::vector<char> sBuffer(iSize);
                
                nCommandCounter++;

                Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, &sBuffer[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo INTERFACE_INFO_CMD=" << sConvertInterfaceCommand2String(eCommand).c_str() <<
                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

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
                    GENTLTEST_PRINT("Hint: (" << sConvertInterfaceCommand2String(eCommand).c_str() << ") : " << sConvertGCError2String(Result).c_str() << 
                        " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                }
                else
                {
                    GENTLTEST_PRINT("Error: (" << sConvertInterfaceCommand2String(eCommand).c_str() << ") : " << sConvertGCError2String(Result).c_str() << 
                        " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                }
            }
        }
    }

    if (nCommandCounter == 0)
    {
        GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo failed. No commands implemented.", false);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    TL_HANDLE hTL=oLibSysSetup.hGetTLHandle();

    oLibSysSetup.tearDown();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            
            Result = m_ModTL.eTLGetInterfaceInfo(hTL, xInterfaceList[index].c_str(), eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    TL_HANDLE hTL=oLibSysSetup.hGetTLHandle();

    oLibSysSetup.tearDownSystem();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            
            Result = m_ModTL.eTLGetInterfaceInfo(hTL, xInterfaceList[index].c_str(), eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceInfo return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "TLGetInterfaceInfo failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
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

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with piSize = NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList;
    
    xInterfaceList = oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            
            Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, NULL, NULL);
            GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceInfo return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
                "TLGetInterfaceInfo failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with piType = NULL at second call with initialized buffer");
    
    if (!g_SkipTypeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        GC_ERROR Result=GC_ERR_SUCCESS;
        System_PreCondition::tStringVector xInterfaceList;
    
        xInterfaceList = oLibSysSetup.xGetTLInterfaceList();

        for (uint32_t index=0; index<xInterfaceList.size(); index++)
        {
            for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
            {
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
            
                Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                    size_t iSaveSize = iSize;
                    std::vector<char> oBuffer(iSize);
                    Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, NULL, &oBuffer[0], &iSize);
                    GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceInfo return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_PARAMETER,
                        "TLGetInterfaceInfo failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old size", iSize == iSaveSize);
                    }
                } 
                else
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    if (Result == GC_ERR_NOT_IMPLEMENTED)
                    {
                        GENTLTEST_PRINT("Hint: " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                    }
                    else
                    {
                        GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
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

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoWithIFIDNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with pointer interface ID = NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList;
    
    for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), NULL, eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceInfo return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_INVALID_ID or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_INVALID_ID || Result == GC_ERR_NOT_AVAILABLE,
            "TLGetInterfaceInfo failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with initialized buffer and size = 0");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            
            Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
            
            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
                iTypeSave = iType;

                std::vector<char> oBuffer(iSize);
                iSize = 0;
                Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, &oBuffer[0], &iSize);
                GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceInfo result value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_BUFFER,
                    "TLGetInterfaceInfo failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != oldType", iTypeSave == iType);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceInfo::TestTLGetInterfaceInfoBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceInfo with initialized buffer and piSize = NULL");
    
    if (!g_SkipSizeNULLCheck)
    {
	    LibrarySystemSetup oLibSysSetup;
        GC_ERROR Result=GC_ERR_SUCCESS;
        System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

        for (uint32_t index=0; index<xInterfaceList.size(); index++)
        {
            for (INTERFACE_INFO_CMD_LIST eCommand=INTERFACE_INFO_ID; eCommand<=INTERFACE_INFO_TLTYPE; eCommand = (INTERFACE_INFO_CMD_LIST)(eCommand+1))
            {
                Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
                Client::INFO_DATATYPE iTypeSave = INFO_DATATYPE_UNKNOWN;
                size_t iSize = 0;
            
                Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("TLGetInterfaceInfo failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);
            
                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
                    iTypeSave = iType;

                    std::vector<char> oBuffer(iSize);
                    Result = m_ModTL.eTLGetInterfaceInfo(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), eCommand, &iType, &oBuffer[0], NULL);
                    GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceInfo result value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_INVALID_PARAMETER,
                        "TLGetInterfaceInfo failed. Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                        Result < GC_ERR_SUCCESS);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call iType != oldType", iTypeSave == iType);
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
