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

#include "Library_PreCondition.h"
#include "LibraryTest.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

LibraryTest::LibraryTest(void)
{
    setUp();
}

LibraryTest::~LibraryTest(void)
{
    tearDown();
}
    
////////////////////////////////////////////////////////////////////////////////////////////
// setUp / tearDown
////////////////////////////////////////////////////////////////////////////////////////////

void LibraryTest :: setUp (void)
{
}

void LibraryTest :: tearDown (void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCInitLib
////////////////////////////////////////////////////////////////////////////////////////////

void LibraryTest::TestGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCInitLib");
    setUp();

    GC_ERROR eResult = m_ModGC.eGCInitLib();
    
    GENTLTEST_CHECK_MESSAGE("GCInitLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);
    
    m_ModGC.eGCCloseLib();

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCInitLibGCCloseLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCInitLib, GCCloseLib");
    setUp();

    GC_ERROR eResult = m_ModGC.eGCInitLib();
    GENTLTEST_CHECK_MESSAGE("GCInitLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);
    
    eResult = m_ModGC.eGCCloseLib();
    GENTLTEST_CHECK_MESSAGE("GCCloseLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);
    
    m_ModGC.eGCCloseLib();

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestDoubleGCInitLibGCCloseLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double GCInitLib (see section 3.1 and 3.2 and 6.1.5)");
    setUp();

    GC_ERROR eResult = m_ModGC.eGCInitLib();
    GENTLTEST_CHECK_MESSAGE("First GCInitLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    eResult = m_ModGC.eGCInitLib();
    GENTLTEST_CHECK_MESSAGE("Second GCInitLib failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_RESOURCE_IN_USE);
    
    eResult = m_ModGC.eGCCloseLib();
    GENTLTEST_CHECK_MESSAGE("GCCloseLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    m_ModGC.eGCCloseLib();

    GENTLTEST_PRINT_RESULT(test_id);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCCloseLib
////////////////////////////////////////////////////////////////////////////////////////////

void LibraryTest::TestGCCloseLibWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCCloseLib without GCInitLib before (see section 3.1 and 3.2 and 6.1.5)");
    
    GC_ERROR eResult = m_ModGC.eGCCloseLib();
    GENTLTEST_CHECK_MESSAGE("GCCloseLib failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_NOT_INITIALIZED);

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCInitLibDoubleGCCloseLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double GCCloseLib (see section 3.1 and 3.2 and 6.1.5)");
    
    GC_ERROR eResult = m_ModGC.eGCInitLib();
    GENTLTEST_CHECK_MESSAGE("GCInitLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);
    
    eResult = m_ModGC.eGCCloseLib();
    GENTLTEST_CHECK_MESSAGE("First GCCloseLib failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    eResult = m_ModGC.eGCCloseLib();
    GENTLTEST_CHECK_MESSAGE("Second GCCloseLib failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_NOT_INITIALIZED);

    GENTLTEST_PRINT_RESULT(test_id);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCGetLastError
////////////////////////////////////////////////////////////////////////////////////////////

void LibraryTest::TestGCGetLastError( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetLastError with double GCInitLib before as error (error value GC_ERR_RESOURCE_IN_USE)");

    // first init lib
    Library_PreCondition oPreCondition;

    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    size_t zSize=0;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    
    // second init lib, create an error to test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_CHECK_MESSAGE("GCGetLastError size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    if (eResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", zSize > 0);

        std::vector<char> sErrorBuffer(zSize);
        std::string sErrorText;

        eResult = m_ModGC.eGCGetLastError(&eErrorCode, &sErrorBuffer[0], &zSize);
        sErrorText = &sErrorBuffer[0];
        GENTLTEST_CHECK_MESSAGE("GCGetLastError with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
            eResult >= GC_ERR_SUCCESS);

        if (eResult >= GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("GCGetLastError error text == ''", 
                sErrorText != "");
            GENTLTEST_CHECK_MESSAGE("GCGetLastError errorcode failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(eErrorCode).c_str(), 
                eErrorCode == GC_ERR_RESOURCE_IN_USE);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorBeforeGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError without GCInitLib before (see section 3.1 and 6.3.1)");
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t zSize=0;
    
    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_CHECK_MESSAGE("GCGetLastError failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_NOT_INITIALIZED);

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorERR_SUCCESS( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError in case of last return value GC_ERR_SUCCESS => expect GC_ERR_SUCCESS.");

    GENTLTEST_PRINT("This test is removed due to different definitions when last error code should be GC_ERR_SUCCESS.\n");

    GENTLTEST_CHECK_MESSAGE("", true);

    /*
    // first init lib
    Library_PreCondition oPreCondition;

    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    size_t zSize=0;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    
    // second init lib, create an error to test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_CHECK_MESSAGE("GCGetLastError failed. Expected result >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    if (eErrorCode == GC_ERR_RESOURCE_IN_USE)
    {
        TL_HANDLE hTl=GENTL_INVALID_HANDLE;
        m_ModTL.eTLOpen(&hTl);

        m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
        GENTLTEST_CHECK_MESSAGE("GCGetLastError failed. Expected errorCode >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eErrorCode).c_str(), 
            eErrorCode >= GC_ERR_SUCCESS);
    }
    */

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorErrorCodeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError with parameter piErrorCode=NULL (see section 6.1.5)");
    
    // first init lib
    Library_PreCondition oPreCondition;

    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t zSize=0;

    // second init lib, create error for test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(NULL, NULL, &zSize);
    GENTLTEST_CHECK_RESULT("Note: GCGetLastError return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_INVALID_PARAMETER,
        "GCGetLastError with piErrorCode=NULL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult < GC_ERR_SUCCESS);

    if (eResult < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter changed after error. zSize != 0", 
            zSize == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorErrorSizeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError with parameter piSize=NULL (see section 6.1.5)");
    Library_PreCondition oPreCondition;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;

    // create error for test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, NULL);
    GENTLTEST_CHECK_RESULT("Note: GCGetLastError return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
        eResult == GC_ERR_INVALID_PARAMETER,
        "GCGetLastError with piSize=NULL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult < GC_ERR_SUCCESS);

    if (eResult < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter changed after error. eErrorCode != 0", 
            eErrorCode == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorWithBufferErrorCodeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError with parameter piErrorCode=NULL at second call with initialized buffer (see section 6.1.5)");
    
    // first init lib
    Library_PreCondition oPreCondition;

    GC_ERROR eResult=GC_ERR_SUCCESS;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    size_t zSize=0;

    // second init lib, create error for test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_CHECK_MESSAGE("GCGetLastError sizecheck failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    if (eResult == GC_ERR_SUCCESS)
    {
        std::vector<char> sErrorText(zSize);

        size_t zSizeSave=zSize;
        eResult = m_ModGC.eGCGetLastError(NULL, &sErrorText[0], &zSize);
        GENTLTEST_CHECK_RESULT("Note: GCGetLastError return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
            eResult == GC_ERR_INVALID_PARAMETER,
            "GCGetLastError with initialized buffer and piErrorCode=NULL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
            eResult < GC_ERR_SUCCESS);

        if (eResult < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter changed after error. zSize != old Size", 
                zSize == zSizeSave);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorWithBufferErrorSizeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError with parameter piSize=NULL at second call with initialized buffer (see section 6.1.5)");
    Library_PreCondition oPreCondition;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    size_t zSize=0;

    // create error for test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_CHECK_MESSAGE("GCGetLastError sizecheck failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    if (eResult == GC_ERR_SUCCESS)
    {
        std::vector<char> sErrorText(zSize);
        GC_ERROR eSaveErrorCode=eErrorCode;

        eResult = m_ModGC.eGCGetLastError(&eErrorCode, &sErrorText[0], NULL);
        GENTLTEST_CHECK_RESULT("Note: GCGetLastError return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(eResult).c_str(), 
            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_INVALID_BUFFER,
            "GCGetLastError with initialized buffer and piSize=NULL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
            eResult < GC_ERR_SUCCESS);

        if (eResult < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter changed after error. eErrorCode != old ErrorCode", 
                eErrorCode == eSaveErrorCode);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void LibraryTest::TestGCGetLastErrorWithBufferErrorLessSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetLastError with parameter piSize=piSize-1 at second call with initialized buffer (see section 6.1.5)");
    Library_PreCondition oPreCondition;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    size_t zSize=0;

    // create error for test
    m_ModGC.eGCInitLib();

    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_CHECK_MESSAGE("GCGetLastError sizecheck failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
        eResult >= GC_ERR_SUCCESS);

    if (eResult == GC_ERR_SUCCESS)
    {
        std::vector<char> sErrorText(zSize);
        GC_ERROR eSaveErrorCode=eErrorCode;

        zSize--;
        eResult = m_ModGC.eGCGetLastError(&eErrorCode, &sErrorText[0], &zSize);
        GENTLTEST_CHECK_RESULT("Note: GCGetLastError return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(eResult).c_str(), 
            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_INVALID_BUFFER,
            "GCGetLastError with initialized buffer and piSize=piSize-1 failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
            eResult < GC_ERR_SUCCESS);

        if (eResult == GC_ERR_INVALID_PARAMETER)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter changed after error. eErrorCode != old ErrorCode", 
                eErrorCode == eSaveErrorCode);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

