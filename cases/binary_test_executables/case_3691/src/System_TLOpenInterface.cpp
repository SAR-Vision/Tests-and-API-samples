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

#include "System_TLOpenInterface.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_TLOpenInterface::System_TLOpenInterface()
{
}

System_TLOpenInterface::~System_TLOpenInterface()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLOpenInterface::setUp(void)
{
}

void System_TLOpenInterface::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLOpenInterface
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLOpenInterface::TestTLOpenInterface( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLOpenInterface");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    GENTLTEST_REQUIRE_MESSAGE("Test failed, no interfaces found.", xInterfaceList.size() > 0);

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), &hIF);
                        
        GENTLTEST_CHECK_MESSAGE("TLOpenInterface (" << 
                        xInterfaceList[index].c_str() << 
                        ") failed expected result " <<
                        sConvertGCError2String(GC_ERR_SUCCESS).c_str() <<
                        " returned " <<
                        sConvertGCError2String(Result).c_str(), 
                        Result >= GC_ERR_SUCCESS);
        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call interface Handle == GENTL_INVALID_HANDLE", hIF != GENTL_INVALID_HANDLE);

            GENTLTEST_PRINT("Info: " << __FUNCTION__ << " (" << index+1 << "/" << xInterfaceList.size() 
                << "), interface ID='" << xInterfaceList[index].c_str() << "' checked: " << sConvertGCError2String(Result).c_str() << std::endl);
        }
        else
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call interface Handle != GENTL_INVALID_HANDLE", hIF == GENTL_INVALID_HANDLE);

            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceWithInterfaceID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpenInterface with known interface ID (see section 3.2)");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList;
    
    xInterfaceList = oLibSysSetup.xGetTLInterfaceList();
    oLibSysSetup.tearDown();
    
    oLibSysSetup.setUp();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), &hIF);
        GENTLTEST_CHECK_MESSAGE("TLOpenInterface failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        GENTLTEST_CHECK_MESSAGE("Parameter after call interface interface handle == GENTL_INVALID_HANDLE", hIF != GENTL_INVALID_HANDLE);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpenInterface with closed library before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    oLibSysSetup.tearDown();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), &hIF);
        GENTLTEST_CHECK_MESSAGE("TLOpenInterface failed. Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_NOT_INITIALIZED);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call interface Handle == GENTL_INVALID_HANDLE", hIF == GENTL_INVALID_HANDLE);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpenInterface with closed system before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    oLibSysSetup.tearDownSystem();
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), &hIF);
        GENTLTEST_CHECK_RESULT("Note: TLOpenInterface return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "TLOpenInterface failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call interface Handle == GENTL_INVALID_HANDLE", hIF == GENTL_INVALID_HANDLE);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceWithIFNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpenInterface with pointer IF Handle NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), NULL);
        GENTLTEST_CHECK_RESULT("Note: TLOpenInterface return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER,
            "TLOpenInterface failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceWithIFIDNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpenInterface with pointer interface ID NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
    Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), NULL, &hIF);
    GENTLTEST_CHECK_RESULT("Note: TLOpenInterface return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_PARAMETER,
        "TLOpenInterface failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call interface handle != GENTL_INVALID_HANDLE", hIF == GENTL_INVALID_HANDLE);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceWithWrongIFID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLOpenInterface with a phantasy interface ID");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;
    
    Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), "Hugo", &hIF);
    GENTLTEST_CHECK_RESULT("Note: TLOpenInterface return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_ID, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_ID,
        "TLOpenInterface failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call interface handle == GENTL_INVALID_HANDLE", hIF == GENTL_INVALID_HANDLE);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLOpenInterface::TestTLOpenInterfaceDoubleOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double TLOpenInterface");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        IF_HANDLE hIFSave = GENTL_INVALID_HANDLE;
        
        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), &hIF);
        GENTLTEST_CHECK_MESSAGE("TLOpenInterface failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);
        
        hIFSave = hIF;

        Result = m_ModTL.eTLOpenInterface(oLibSysSetup.hGetTLHandle(), xInterfaceList[index].c_str(), &hIF);
        GENTLTEST_CHECK_MESSAGE("TLOpenInterface failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_RESOURCE_IN_USE);

        if (Result == GC_ERR_RESOURCE_IN_USE)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call interface handle != old handle", hIFSave == hIF);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}
