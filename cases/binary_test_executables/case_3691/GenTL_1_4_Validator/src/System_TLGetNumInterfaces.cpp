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

#include "System_TLGetNumInterfaces.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_TLGetNumInterfaces::System_TLGetNumInterfaces()
{
}

System_TLGetNumInterfaces::~System_TLGetNumInterfaces()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetNumInterfaces::setUp(void)
{
}

void System_TLGetNumInterfaces::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLGetNumInterfaces
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetNumInterfaces::TestTLGetNumInterfaces( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLGetNumInterfaces");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces = 0;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    
    oLibSysSetup.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);

    Result = m_ModTL.eTLGetNumInterfaces(oLibSysSetup.hGetTLHandle(), &uiNumInterfaces);
    GENTLTEST_CHECK_MESSAGE("TLGetNumInterfaces failed expected result " <<
                    sConvertGCError2String(GC_ERR_SUCCESS).c_str() <<
                    " returned " <<
                    sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);
    if (Result < GC_ERR_SUCCESS)
    {
        std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
        GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
    } 
    
    GENTLTEST_PRINT("Info: " << __FUNCTION__ << " NumInterfaces=" << uiNumInterfaces << " checked: " << sConvertGCError2String(Result).c_str() << std::endl);

    // for certification we need at least one Interface
    GENTLTEST_REQUIRE_MESSAGE("Error: **************************************************************" << std::endl <<
        "Error: * For GenICam validation at least one Interface should exist *" << std::endl <<
        "Error: **************************************************************" << std::endl, 
        uiNumInterfaces != 0);
    
    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetNumInterfaces::TestTLGetNumInterfacesWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetNumInterfaces with closed library before");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    TL_HANDLE hTL = oLibSysSetup.hGetTLHandle();
    uint64_t uiTimeout = 1000;
    uint32_t uiNumInterfaces = 0;
    
    oLibSysSetup.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);

    oLibSysSetup.tearDown();

    Result = m_ModTL.eTLGetNumInterfaces(hTL, &uiNumInterfaces);
    GENTLTEST_CHECK_MESSAGE("TLGetNumInterfaces failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_NOT_INITIALIZED);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter NumInterfaces != 0", uiNumInterfaces == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetNumInterfaces::TestTLGetNumInterfacesWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetNumInterfaces with closed system before");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    TL_HANDLE hTL = oLibSysSetup.hGetTLHandle();
    uint32_t uiNumInterfaces = 0;
    
    oLibSysSetup.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);

    oLibSysSetup.tearDownSystem();

    Result = m_ModTL.eTLGetNumInterfaces(hTL, &uiNumInterfaces);
    GENTLTEST_CHECK_RESULT("Note: TLGetNumInterfaces return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_INITIALIZED,
        "TLGetNumInterfaces failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter NumInterfaces != 0", uiNumInterfaces == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetNumInterfaces::TestTLGetNumInterfacesWithHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetNumInterfaces with TL Handle GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    uint32_t uiNumInterfaces = 0;

    oLibSysSetup.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);

    Result = m_ModTL.eTLGetNumInterfaces(GENTL_INVALID_HANDLE, &uiNumInterfaces);
    GENTLTEST_CHECK_RESULT("Note: TLGetNumInterfaces return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "TLGetNumInterfaces failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call NumInterfaces != 0", uiNumInterfaces == 0);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetNumInterfaces::TestTLGetNumInterfacesWithNumbersNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetNumInterfaces with parameter piNumIfaces NULL");
    LibrarySystemSetup oLibSysSetup(false);
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    TL_HANDLE hTLSave = GENTL_INVALID_HANDLE;

    oLibSysSetup.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);

    hTLSave = oLibSysSetup.hGetTLHandle();

    Result = m_ModTL.eTLGetNumInterfaces(oLibSysSetup.hGetTLHandle(), NULL);
    GENTLTEST_CHECK_RESULT("Note: TLGetNumInterfaces return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_PARAMETER,
        "TLGetNumInterfaces failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call TL Handle != old TL handle", hTLSave == oLibSysSetup.hGetTLHandle());
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetNumInterfaces::TestTLGetNumInterfacesWithoutUpdate( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetNumInterfaces check for number of interfaces > 0");
    LibrarySystemSetup oLibSysSetup(false); // false = without TLUpdateInterfaceList
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    TL_HANDLE hTLSave = GENTL_INVALID_HANDLE;
    uint32_t uiNumInterfaces = 0;
    
    hTLSave = oLibSysSetup.hGetTLHandle();

    Result = m_ModTL.eTLGetNumInterfaces(oLibSysSetup.hGetTLHandle(), &uiNumInterfaces);
    GENTLTEST_CHECK_MESSAGE("TLGetNumInterfaces failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_AVAILABLE);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call, TL handle != old handel", hTLSave == oLibSysSetup.hGetTLHandle());
        GENTLTEST_CHECK_MESSAGE("Parameter after call, iNumInterfaces != 0", uiNumInterfaces == 0);
    }

    if (uiNumInterfaces == 0)
    {
        GENTLTEST_PRINT("Info: TLGetNumInterfaces without TLUpdateInterfaceList returned NumInterfaces=0, try to get more with TLUpdateInterfaceList before." << std::endl);

        oLibSysSetup.eTLUpdateInterfaceList(oLibSysSetup.hGetTLHandle(), &bHasChanged, uiTimeout);
    
        Result = m_ModTL.eTLGetNumInterfaces(oLibSysSetup.hGetTLHandle(), &uiNumInterfaces);
        GENTLTEST_CHECK_MESSAGE("TLGetNumInterfaces failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        GENTLTEST_PRINT("Info: TLGetNumInterfaces with TLUpdateInterfaceList before returned NumInterfaces=" << uiNumInterfaces << "." << std::endl);
    }

    GENTLTEST_CHECK_MESSAGE("TLGetNumInterfaces failed. Expected at least 1 interface, received 0 interfaces.", 
        uiNumInterfaces > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}
