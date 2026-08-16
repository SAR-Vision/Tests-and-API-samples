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

#include "Library_precondition.h"
#include "Library_gcgetinfo.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Library_GCGetInfo::Library_GCGetInfo()
{
    setUp();
}

Library_GCGetInfo::~Library_GCGetInfo()
{
    tearDown();
}

////////////////////////////////////////////////////////////////////////////////////////////
// setUp / tearDown
////////////////////////////////////////////////////////////////////////////////////////////

void Library_GCGetInfo :: setUp (void)
{
}

void Library_GCGetInfo :: tearDown (void)
{
}


////////////////////////////////////////////////////////////////////////////////////////////
// Test GCGetInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Library_GCGetInfo::TestGCGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetInfo");
    Library_PreCondition oPreCondition;
    bool bErrorFlag=false;
    int nCommandCounter=0;

    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        eResult = m_ModGC.eGCGetInfo(eCommand, &iType, NULL, &iSize);

        GENTLTEST_CHECK_MESSAGE("GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand) << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
            eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

        if (eResult >= GC_ERR_SUCCESS)
        {
            size_t iSaveSize = iSize;
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

            nCommandCounter++;

            std::vector<char> sBuffer(iSize);
            eResult = m_ModGC.eGCGetInfo(eCommand, &iType, &sBuffer[0], &iSize);

            GENTLTEST_CHECK_MESSAGE("GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand) << 
                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS);
            GENTLTEST_CHECK_MESSAGE("Parameter changed during call. Size != old Size", iSaveSize == iSize);

            if (eResult >= GC_ERR_SUCCESS)
            {
                GENTLTEST_DUMP_ITYPE("Info: " << sConvertTLInfoCommand2String(eCommand).c_str(), iType, &sBuffer[0], iSize);

                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                switch (eCommand)
                {
                case TL_INFO_ID:
                case TL_INFO_VENDOR:
                case TL_INFO_MODEL:
                case TL_INFO_VERSION:
                case TL_INFO_NAME:
                case TL_INFO_PATHNAME:
                case TL_INFO_DISPLAYNAME:
                    GENTLTEST_CHECK_MESSAGE(sConvertTLInfoCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                    break;
                case TL_INFO_TLTYPE:
                    GENTLTEST_CHECK_MESSAGE(sConvertTLInfoCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                    
                    if (iType == INFO_DATATYPE_STRING)
                    {
                        std::string sTLType=&sBuffer[0];
                        GENTLTEST_CHECK_MESSAGE(sConvertTLInfoCommand2String(eCommand).c_str() << 
                            " wrong tltype: expected 'Mixed', 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                            sTLType == TLTypeMixedName || sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                            sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                    }
                    break;
                case TL_INFO_CHAR_ENCODING:
                    GENTLTEST_CHECK_MESSAGE(sConvertTLInfoCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_INT32 type=" << 
                        sConvertDataType2String(iType), iType == INFO_DATATYPE_INT32); 
                    break;
                }
            }
        }
        else 
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after error. iSize != 0", iSize == 0);
            std::string sMsg=oPreCondition.sGetLastErrorMessage();
            if (eResult == GC_ERR_NOT_IMPLEMENTED)
            {
                GENTLTEST_PRINT("Hint: " << sConvertGCError2String(eResult).c_str() << " LastErrorMessage("<<sConvertTLInfoCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
            }
            else
            {
                GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << " LastErrorMessage("<<sConvertTLInfoCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
            }
        }
    }

    GENTLTEST_CHECK_MESSAGE("GCGetInfo failed. No commands implemented.", nCommandCounter > 0);
    
    GENTLTEST_PRINT_RESULT(test_id);
}

void Library_GCGetInfo::TestGCGetInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo without GCInitLib before");

    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;

        eResult = m_ModGC.eGCGetInfo(eCommand, &iType, NULL, &iSize);

        GENTLTEST_CHECK_RESULT("Note: GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " return value: Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
            eResult == GC_ERR_NOT_INITIALIZED || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
            "GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " without GCInitLib failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
            eResult < GC_ERR_SUCCESS);

        if (eResult == GC_ERR_NOT_INITIALIZED)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call. iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Library_GCGetInfo::TestGCGetInfo1Size0( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo with parameter piSize=NULL at call with initialized buffer");
    
    if (!g_SkipSizeNULLCheck)
    {
		Library_PreCondition oPreCondition;

        for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
        {
            GC_ERROR eResult=GC_ERR_SUCCESS;
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
        
            eResult = m_ModGC.eGCGetInfo(eCommand, &iType, NULL, &iSize);

            GENTLTEST_CHECK_MESSAGE("GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand).c_str() << 
                " with size=0 failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

            if (eResult >= GC_ERR_SUCCESS)
            {
                size_t iSaveSize = iSize;
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                std::vector<char> Buff(iSize);
                eResult = m_ModGC.eGCGetInfo(eCommand, &iType, &Buff[0], NULL);

                GENTLTEST_CHECK_RESULT("Note: GCGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult == GC_ERR_INVALID_PARAMETER,
                    "GCGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " with iSize=0 failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult < GC_ERR_SUCCESS);
                GENTLTEST_CHECK_MESSAGE("Parameter changed during call. iSize != old iSize", iSaveSize == iSize);
            }
            else 
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call. iSize != 0", iSize == 0);
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Library_GCGetInfo::TestGCGetInfo2Size0( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo with parameter piSize=NULL at call for size check");
    Library_PreCondition oPreCondition;
    
    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        
        eResult = m_ModGC.eGCGetInfo(eCommand, &iType, NULL, NULL);

        GENTLTEST_CHECK_RESULT("Note: GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
            "GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " with size=0 failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
            eResult < GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Library_GCGetInfo::TestGCGetInfoLowSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo with parameter iSize=iSize-1 at call with initialized buffer");
    Library_PreCondition oPreCondition;
    
    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        eResult = m_ModGC.eGCGetInfo(eCommand, &iType, NULL, &iSize);

        GENTLTEST_CHECK_MESSAGE("GCGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
            eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

        if (eResult >= GC_ERR_SUCCESS)
        {
            size_t iSaveSize=0;
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call. iSize == 0", iSize != 0);

            if (iSize > 1)
            {
                iSize--;
                iSaveSize = iSize;
                std::vector<char> Buff(iSize);
                eResult = m_ModGC.eGCGetInfo(eCommand, &iType, &Buff[0], &iSize);

                GENTLTEST_CHECK_RESULT("Note: GCGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_INVALID_BUFFER || eResult == GC_ERR_NOT_AVAILABLE,
                    "GCGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " with iSize=iSize-1 failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult < GC_ERR_SUCCESS);

                GENTLTEST_CHECK_MESSAGE("Parameter after call. iSize != old iSize", iSaveSize == iSize);
            }
        }
        else 
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call. iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Library_GCGetInfo::TestGCGetInfoTypeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo with piType = NULL at second call with initialized buffer");

    if (!g_SkipTypeNULLCheck)
    {
        Library_PreCondition oPreCondition;
        bool bErrorFlag=false;

        for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
        {
            GC_ERROR eResult=GC_ERR_SUCCESS;
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
        
            eResult = m_ModGC.eGCGetInfo(eCommand, &iType, NULL, &iSize);

            GENTLTEST_CHECK_MESSAGE("GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand) << 
                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

            if (eResult >= GC_ERR_SUCCESS)
            {
                size_t iSaveSize = iSize;
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call. iSize == 0", iSize != 0);

                std::vector<char> Buff(iSize);
                eResult = m_ModGC.eGCGetInfo(eCommand, NULL, &Buff[0], &iSize);

                GENTLTEST_CHECK_RESULT("Note: GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand) << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult == GC_ERR_INVALID_PARAMETER,
                    "GCGetInfo TL_INFO_CMD=" << sConvertTLInfoCommand2String(eCommand) << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult < GC_ERR_SUCCESS);
                GENTLTEST_CHECK_MESSAGE("Parameter after call. iSize != old iSize", iSaveSize == iSize);
            }
            else 
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call. iSize != 0", iSize == 0);
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Library_GCGetInfo::TestGCGetInfoWithInvalidCommand( uint32_t test_id )
{
	// Original GenTL 1.4 code
    /* GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo with parameter iInfoCmd=TL_INFO_CHAR_ENCODING+1");
    Library_PreCondition oPreCondition;
    
    GC_ERROR eResult=GC_ERR_SUCCESS;
    Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
    size_t iSize = 0;
    
    eResult = m_ModGC.eGCGetInfo(TL_INFO_CHAR_ENCODING+1, &iType, NULL, &iSize);

    GENTLTEST_CHECK_RESULT("Note: GCGetInfo TL_INFO_CMD=TL_INFO_CHAR_ENCODING+1" << 
        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
        eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
        "GCGetInfo TL_INFO_CMD=TL_INFO_CHAR_ENCODING+1" << 
        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
        eResult < GC_ERR_SUCCESS);

    if (eResult < GC_ERR_SUCCESS)
    {
        GENTLTEST_REQUIRE_MESSAGE("Parameter after call. iSize != 0", iSize == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id); */
	
	// GenTL 1.5 edited code

#define TL_INFO_GENTL_VER_MINOR  (10)   /* UINT32    Minor number of the GenTL spec this producer complies with, GenTL v1.5 */

    // In GenTL 1.4 last TL_INFO_CMD was TL_INFO_CHAR_ENCODING.
    // In GenTL 1.5 was added two new cmd TL_INFO_GENTL_VER_MAJOR and TL_INFO_GENTL_VER_MINOR.
    // This test case put invalid cmd for check how to producer perform this case.
    // We should put last cmd + 1 for GenTL 1.5 not 1.4
    // We keep original code above

	GENTLTEST_DESCRIPTION(test_id, "Test GCGetInfo with parameter iInfoCmd=TL_INFO_GENTL_VER_MINOR+1 (Test case edited for GenTL 1.5, more inforamtion in source code commented section)");
    Library_PreCondition oPreCondition;
    
    GC_ERROR eResult=GC_ERR_SUCCESS;
    Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
    size_t iSize = 0;
    
    eResult = m_ModGC.eGCGetInfo(TL_INFO_GENTL_VER_MINOR+1, &iType, NULL, &iSize);

    GENTLTEST_CHECK_RESULT("Note: GCGetInfo TL_INFO_CMD=TL_INFO_GENTL_VER_MINOR+1" << 
        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
        eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
        "GCGetInfo TL_INFO_CMD=TL_INFO_GENTL_VER_MINOR+1" << 
        " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
        eResult < GC_ERR_SUCCESS);

    if (eResult < GC_ERR_SUCCESS)
    {
        GENTLTEST_REQUIRE_MESSAGE("Parameter after call. iSize != 0", iSize == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

