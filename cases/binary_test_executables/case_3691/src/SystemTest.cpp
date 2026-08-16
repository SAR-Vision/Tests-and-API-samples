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

#include "SystemTest.h"
#include "GenTLTestTools.h"
#include "Library_PreCondition.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

SystemTest::SystemTest( void )
{
}

SystemTest::~SystemTest( void )
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void SystemTest::setUp(void)
{
}

void SystemTest::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLOpen
////////////////////////////////////////////////////////////////////////////////////////////

void SystemTest::TestTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLOpen TLClose");
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    Library_PreCondition oLibrary_PreCondition;
    
    Result = m_ModTL.eTLOpen(&hTL);
    GENTLTEST_CHECK_MESSAGE("TLOpen failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS);
    if (Result < GC_ERR_SUCCESS)
    {
        std::string sMsg=oLibrary_PreCondition.sGetLastErrorMessage();
        GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
    }

    if (Result >= GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call. TL handle = GENTL_INVALID_HANDLE", hTL != GENTL_INVALID_HANDLE);
    }
    else 
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after faulty call. TL handle != GENTL_INVALID_HANDLE", hTL == GENTL_INVALID_HANDLE);
    }

    Result = m_ModTL.eTLClose(hTL);
    GENTLTEST_CHECK_MESSAGE("TLClose failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

void SystemTest::TestTLOpenWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpen with closed library");
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    Library_PreCondition oLibrary_PreCondition;
    
    oLibrary_PreCondition.eGCCloseLib();

    Result = m_ModTL.eTLOpen(&hTL);
    GENTLTEST_CHECK_RESULT("Note: TLOpen return value: Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_NOT_INITIALIZED,
        "TLOpen without GCInitLib failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    GENTLTEST_CHECK_MESSAGE("Parameter after call. TL handle != GENTL_INVALID_HANDLE", hTL == GENTL_INVALID_HANDLE);

    GENTLTEST_PRINT_RESULT(test_id);
}

void SystemTest::TestTLOpenTwice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double TLOpen (see section 3.2 and 6.1.5)");
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    TL_HANDLE hTLSave = GENTL_INVALID_HANDLE;
    Library_PreCondition oLibrary_PreCondition;
    
    Result = m_ModTL.eTLOpen(&hTL);
    GENTLTEST_REQUIRE_MESSAGE("TLOpen failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS);
    
    hTLSave = hTL;
    Result = m_ModTL.eTLOpen(&hTL);
    GENTLTEST_CHECK_MESSAGE("TLOpen twice failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_RESOURCE_IN_USE);

    GENTLTEST_CHECK_MESSAGE("Parameter after call. TL handle != first TL handle", hTLSave == hTL);
    
    Result = m_ModTL.eTLClose(hTL);

    GENTLTEST_PRINT_RESULT(test_id);
}

void SystemTest::TestTLOpenHandleNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpen with handle pointer NULL");
    GC_ERROR Result=GC_ERR_SUCCESS;
    Library_PreCondition oLibrary_PreCondition;
    
    Result = m_ModTL.eTLOpen(NULL);
    GENTLTEST_CHECK_RESULT("Note: TLOpen return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_PARAMETER,
        "TLOpen without GCInitLib failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLClose
////////////////////////////////////////////////////////////////////////////////////////////

void SystemTest::TestTLCloseWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLClose with closed library");
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL=GENTL_INVALID_HANDLE;
    Library_PreCondition oLibrary_PreCondition;
    
    Result = m_ModTL.eTLOpen(&hTL);
    GENTLTEST_REQUIRE_MESSAGE("TLOpen failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS);

    oLibrary_PreCondition.eGCCloseLib();

    Result = m_ModTL.eTLClose(hTL);
    GENTLTEST_CHECK_MESSAGE("TLClose failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_NOT_INITIALIZED);

    GENTLTEST_PRINT_RESULT(test_id);
}

void SystemTest::TestTLCloseWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLClose without TLOpen before");
    GC_ERROR Result=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    Library_PreCondition oLibrary_PreCondition; 
    
    Result = m_ModTL.eTLClose(hTL);
    GENTLTEST_CHECK_RESULT("Note: TLClose return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE,
        "TLClose without TLOpen failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

void SystemTest::TestTLCloseTwice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double TLClose with TLOpen before");
    GC_ERROR eResult=GC_ERR_SUCCESS;
    TL_HANDLE hTL = GENTL_INVALID_HANDLE;
    Library_PreCondition oLibrary_PreCondition; 
    
    eResult = m_ModTL.eTLOpen(&hTL);
    GENTLTEST_REQUIRE_MESSAGE("TLOpen failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);
    
    eResult = m_ModTL.eTLClose(hTL);
    GENTLTEST_CHECK_MESSAGE("TLClose failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    eResult = m_ModTL.eTLClose(hTL);
    GENTLTEST_CHECK_RESULT("Note: TLClose return value: Expected == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_INVALID_HANDLE,
        "TLClose twice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult < GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

