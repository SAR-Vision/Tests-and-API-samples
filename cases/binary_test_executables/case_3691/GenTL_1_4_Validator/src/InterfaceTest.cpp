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
#include "InterfaceTest.h"
#include "LibrarySystemSetup.h"
#include "URLParser.h"
#include "LocalParser.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

InterfaceTest::InterfaceTest()
{
}

InterfaceTest::~InterfaceTest()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// setUp / tearDown
////////////////////////////////////////////////////////////////////////////////////////////

void InterfaceTest :: setUp (void)
{
}

void InterfaceTest :: tearDown (void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFClose
////////////////////////////////////////////////////////////////////////////////////////////

void InterfaceTest::TestIFClose( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFClose");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;

        oLibSysSetup.eTLOpenInterface(xInterfaceList[index].c_str(), &hIF);
        
        Result = m_ModIF.eIFClose(hIF);
        GENTLTEST_CHECK_MESSAGE("TestIFClose failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void InterfaceTest::TestIFCloseWithLibraryClosed( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFClose if library closed before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;

    oLibSysSetup.eTLOpenInterface(xInterfaceList[0].c_str(), &hIF);
    
    oLibSysSetup.tearDown();
    
    Result = m_ModIF.eIFClose(hIF);
    GENTLTEST_CHECK_MESSAGE("TestIFClose failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_NOT_INITIALIZED || Result == GC_ERR_INVALID_HANDLE);

    GENTLTEST_PRINT_RESULT(test_id);
}

void InterfaceTest::TestIFCloseWithSystemClosed( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFClose if system closed before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;

    oLibSysSetup.eTLOpenInterface(xInterfaceList[0].c_str(), &hIF);
    
    oLibSysSetup.tearDownSystem();
    
    Result = m_ModIF.eIFClose(hIF);
    GENTLTEST_CHECK_RESULT("Note: TestIFClose return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "TestIFClose failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

void InterfaceTest::TestIFCloseDouble( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double IFClose");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        IF_HANDLE hIFSave = GENTL_INVALID_HANDLE;

        oLibSysSetup.eTLOpenInterface(xInterfaceList[index].c_str(), &hIF);
        
        hIFSave = hIF;

        Result = m_ModIF.eIFClose(hIF);
        GENTLTEST_CHECK_MESSAGE("TestIFClose failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        Result = m_ModIF.eIFClose(hIF);
        GENTLTEST_CHECK_RESULT("Note: TestIFClose return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "TestIFClose failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call handle IF != old handle", hIFSave == hIF);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void InterfaceTest::TestIFCloseReopen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test reopen IFClose");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        oLibSysSetup.eTLOpenInterface(xInterfaceList[index].c_str(), &hIF);
        
        Result = m_ModIF.eIFClose(hIF);
        GENTLTEST_CHECK_MESSAGE("TestIFClose failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        oLibSysSetup.eTLOpenInterface(xInterfaceList[index].c_str(), &hIF);
        
        Result = m_ModIF.eIFClose(hIF);
        GENTLTEST_CHECK_MESSAGE("TestIFClose failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void InterfaceTest::TestIFCloseWithHandleIFNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFClose with interface handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
    oLibSysSetup.eTLOpenInterface(xInterfaceList[0].c_str(), &hIF);
    
    Result = m_ModIF.eIFClose(GENTL_INVALID_HANDLE);
    GENTLTEST_CHECK_RESULT("Note: TestIFClose return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "TestIFClose failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

void InterfaceTest::TestIFCloseWithHandleIFWrong( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFClose with phantasy interface handle (0xb7f7b7f7)");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
    Result = oLibSysSetup.eTLOpenInterface(xInterfaceList[0].c_str(), &hIF);
    GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);

    Result = m_ModIF.eIFClose((IF_HANDLE)0xb7f7b7f7);
    GENTLTEST_CHECK_RESULT("Note: TestIFClose return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "TestIFClose failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    GENTLTEST_PRINT_RESULT(test_id);
}

