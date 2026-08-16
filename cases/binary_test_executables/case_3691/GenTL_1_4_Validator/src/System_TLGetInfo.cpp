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

#include "System_TLGetInfo.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_TLGetInfo::System_TLGetInfo()
{
}

System_TLGetInfo::~System_TLGetInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetInfo::setUp(void)
{
}

void System_TLGetInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLGetInfo
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetInfo::TestTLGetInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLGetInfo");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    int nCommandCounter=0;
    
    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
            
            std::vector<char> sBuffer(iSize);

            nCommandCounter++;

            Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, &sBuffer[0], &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << " LastErrorMessage("<<sConvertTLInfoCommand2String(eCommand).c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);

                GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal 0", iSize == 0);
            }
            else
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
            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            if (Result == GC_ERR_NOT_IMPLEMENTED)
            {
                GENTLTEST_PRINT("Hint: (" << sConvertTLInfoCommand2String(eCommand).c_str() << ") : " << sConvertGCError2String(Result).c_str() << 
                    " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
            }
            else
            {
                GENTLTEST_PRINT("Error: (" << sConvertTLInfoCommand2String(eCommand).c_str() << ") : " << sConvertGCError2String(Result).c_str() << 
                    " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
            }

            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal 0", iSize == 0);
        }
    }

    if (nCommandCounter == 0)
    {
        GENTLTEST_CHECK_MESSAGE("TLGetInfo failed. No commands implemented.", false);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = oLibSysSetup.hGetTLHandle();
    
    oLibSysSetup.tearDown();

    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInfo(hTL, eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInfo without TLOpen " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = oLibSysSetup.hGetTLHandle();
    
    oLibSysSetup.tearDownSystem();

    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInfo(hTL, eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
            "TLGetInfo without TLOpen " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithTLHandleNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with TL handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    
    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInfo(GENTL_INVALID_HANDLE, eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_AVAILABLE,
            "TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithInvalidCommand( uint32_t test_id )
{
    //GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with TL info command = TL_INFO_CHAR_ENCODING+1");
    //LibrarySystemSetup oLibSysSetup;
    //GC_ERROR Result=GC_ERR_SUCCESS;
    //TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    //
    //Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
    //size_t iSize = 0;
    //
    //Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), TL_INFO_CHAR_ENCODING+1, &iType, NULL, &iSize);
    //GENTLTEST_CHECK_RESULT("Note: TLGetInfo TL_INFO_CHAR_ENCODING+1" << 
    //    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
    //    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
    //    "TLGetInfo TL_INFO_CHAR_ENCODING+1 failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
    //    Result < GC_ERR_SUCCESS);

    //if (Result < GC_ERR_SUCCESS)
    //{
    //    GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
    //}

    //GENTLTEST_PRINT_RESULT(test_id);

#define TL_INFO_GENTL_VER_MINOR  (10)   /* UINT32    Minor number of the GenTL spec this producer complies with, GenTL v1.5 */

    // In GenTL 1.4 last TL_INFO_CMD was TL_INFO_CHAR_ENCODING.
    // In GenTL 1.5 was added two new cmd TL_INFO_GENTL_VER_MAJOR and TL_INFO_GENTL_VER_MINOR.
    // This test case put invalid cmd for check how to producer perform this case.
    // We should put last cmd + 1 for GenTL 1.5 not 1.4
    // We keep original code above

    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with TL info command = TL_INFO_GENTL_VER_MINOR+1  (Test case edited for GenTL 1.5, more inforamtion in source code commented section)");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result = GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;

    Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
    size_t iSize = 0;

    Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), TL_INFO_GENTL_VER_MINOR + 1, &iType, NULL, &iSize);
    GENTLTEST_CHECK_RESULT("Note: TLGetInfo TL_INFO_GENTL_VER_MINOR+1" <<
        " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(),
        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
        "TLGetInfo TL_INFO_GENTL_VER_MINOR+1 failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(),
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with piType = NULL at second call with initialized buffer");
    
    if (!g_SkipTypeNULLCheck)
    {
        LibrarySystemSetup oLibSysSetup;
        GC_ERROR Result=GC_ERR_SUCCESS;
    
        for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
            size_t iSizeSave = 0;
        
            Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInfo with type null " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
                iSizeSave = iSize;

                std::vector<char> sBuffer(iSize);
                Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, NULL, &sBuffer[0], &iSize);
                GENTLTEST_CHECK_RESULT("Note: TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER,
                    "TLGetInfo with type null " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
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

void System_TLGetInfo::TestTLGetInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with piSize = NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    
    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        
        Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, NULL, 0);
        GENTLTEST_CHECK_RESULT("Note: TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_BUFFER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE,
            "TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithLowSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with initialized buffer and iSize = iSize-1");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    
    for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
    {
        Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInfo with size - 1 " << sConvertTLInfoCommand2String(eCommand).c_str() << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

        if (Result >= GC_ERR_SUCCESS)
        {
            Client::INFO_DATATYPE iTypeSave = iType;
            size_t iSizeSave = 0;
            
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
            
            if (iSize > 1)
            {
                iSize--;
                iSizeSave = iSize;
                std::vector<char> Buff(iSize);
                Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, &Buff[0], &iSize);
                GENTLTEST_CHECK_RESULT("Note: TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_BUFFER,
                    "TLGetInfo with size - 1 " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == iTypeSave);
                    GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal old iSize", iSize == iSizeSave);
                }
            }
        }
        else
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
            GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInfo::TestTLGetInfoWithBufferSizeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInfo with initialized buffer and piSize = NULL");
    
    if (!g_SkipSizeNULLCheck)
    {
	    LibrarySystemSetup oLibSysSetup;
        GC_ERROR Result=GC_ERR_SUCCESS;
    
        for (TL_INFO_CMD_LIST eCommand=TL_INFO_ID; eCommand<=TL_INFO_CHAR_ENCODING; eCommand = (TL_INFO_CMD_LIST)(eCommand+1))
        {
            Client::INFO_DATATYPE iType = INFO_DATATYPE_UNKNOWN;
            size_t iSize = 0;
        
            Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInfo with size - 1 " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_NOT_AVAILABLE);

            if (Result >= GC_ERR_SUCCESS)
            {
                Client::INFO_DATATYPE iTypeSave = iType;
            
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);
            
                std::vector<char> Buff(iSize);
                Result = m_ModTL.eTLGetInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, &Buff[0], NULL);
                GENTLTEST_CHECK_RESULT("Note: TLGetInfo " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER,
                    "TLGetInfo with size - 1 " << sConvertTLInfoCommand2String(eCommand).c_str() << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == iTypeSave);
                }
            }
            else
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call wrong datatype returned", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call size returned not equal 0", iSize == 0);
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

