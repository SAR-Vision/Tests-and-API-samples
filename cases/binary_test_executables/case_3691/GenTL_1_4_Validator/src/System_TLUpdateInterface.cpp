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

#include "System_TLUpdateInterface.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_TLUpdateInterface::System_TLUpdateInterface( void )
{
}

System_TLUpdateInterface::~System_TLUpdateInterface( void )
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLUpdateInterface::setUp(void)
{
}

void System_TLUpdateInterface::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLInterfaces
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLUpdateInterface::TestTLUpdateInterfaceList( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLUpdateInterfaceList (using 1000msec timeout)");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    
    Result = m_ModTL.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);
    GENTLTEST_CHECK_MESSAGE("TLUpdateInterfaceList failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);

    if (Result == GC_ERR_TIMEOUT)
    {
        GENTLTEST_PRINT("Info: TLUpdateInterfaceList returned GC_ERR_TIMEOUT, try another call with 10*timeout" << std::endl);
        Result = m_ModTL.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout*10);
        GENTLTEST_CHECK_MESSAGE("TLUpdateInterfaceList failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);
    }

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call bHasChanged != false", bHasChanged == false);

        std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
        GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
    }
    else
    {
        GENTLTEST_PRINT("Info: TLUpdateInterfaceList returned bHasChanged=" << ((bHasChanged)?"true":"false") << std::endl);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLUpdateInterface::TestTLUpdateInterfaceListWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLUpdateInterfaceList with closed library before");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    
    oLibSysSetup.tearDown();

    Result = m_ModTL.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);
    GENTLTEST_CHECK_MESSAGE("TLUpdateInterfaceList failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_NOT_INITIALIZED);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call bHasChanged != false", bHasChanged == false);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLUpdateInterface::TestTLUpdateInterfaceListWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLUpdateInterfaceList with closed system before");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    
    oLibSysSetup.tearDownSystem();

    Result = m_ModTL.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);
    GENTLTEST_CHECK_RESULT("Note: TLUpdateInterfaceList return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_INITIALIZED,
        "TLUpdateInterfaceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call bHasChanged != false", bHasChanged == false);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLUpdateInterface::TestTLUpdateInterfaceListWithHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLUpdateInterfaceList with TL Handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    
    Result = m_ModTL.eTLUpdateInterfaceList(GENTL_INVALID_HANDLE, &bHasChanged, uiTimeout);
    GENTLTEST_CHECK_RESULT("Note: TLUpdateInterfaceList return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "TLUpdateInterfaceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call bHasChanged != false", bHasChanged == false);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLUpdateInterface::TestTLUpdateInterfaceListWithChangedNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLUpdateInterfaceList with pbChanged = NULL");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint64_t uiTimeout = 1000;
    
    // NULL is allowed and just for an update regardless of the result.
    // Result will be success in any case if hTl is valid
    Result = m_ModTL.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), NULL, uiTimeout);
    GENTLTEST_CHECK_MESSAGE("TLUpdateInterfaceList failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

