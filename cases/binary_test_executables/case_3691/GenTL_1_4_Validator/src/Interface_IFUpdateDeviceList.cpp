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

#include "Interface_IFUpdateDeviceList.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFUpdateDeviceList::Interface_IFUpdateDeviceList()
{
}

Interface_IFUpdateDeviceList::~Interface_IFUpdateDeviceList()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFUpdateDeviceList::setUp(void)
{
}

void Interface_IFUpdateDeviceList::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLInterfaces
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFUpdateDeviceList::TestIFUpdateDeviceList( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFUpdateDeviceList (using 1000msec timeout)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        Result = m_ModIF.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        GENTLTEST_CHECK_MESSAGE("IFUpdateDeviceList failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS || Result == GC_ERR_TIMEOUT);

        if (Result == GC_ERR_TIMEOUT)
        {
            GENTLTEST_PRINT("Info: IFUpdateDeviceList returned GC_ERR_TIMEOUT, try another call with 10*timeout" << std::endl);
            Result = m_ModIF.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout*10);
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
            GENTLTEST_PRINT("Info: IFUpdateDeviceList returned bHasChanged=" << ((bHasChanged)?"true":"false") << std::endl);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFUpdateDeviceList::TestIFUpdateDeviceListWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFUpdateDeviceList with closed library before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oLibSysSetup.tearDownLibrary();
        
        Result = m_ModIF.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        GENTLTEST_CHECK_MESSAGE("IFUpdateDeviceList failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_NOT_INITIALIZED);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call hasChanged != false", bHasChanged == false);
        }

        oLibSysSetup.tearDownSystem();
        oLibSysSetup.setUp();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFUpdateDeviceList::TestIFUpdateDeviceListWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFUpdateDeviceList with closed system before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oLibSysSetup.tearDownSystem();
        
        Result = m_ModIF.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        GENTLTEST_CHECK_RESULT("Note: IFUpdateDeviceList return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFUpdateDeviceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call hasChanged != false", bHasChanged == false);
        }

        oLibSysSetup.setUpSystem();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFUpdateDeviceList::TestIFUpdateDeviceListWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFUpdateDeviceList with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    IF_HANDLE hIF = GENTL_INVALID_HANDLE;
    Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[0], &hIF, false);

    oLibSysSetup.tearDownSystem();
    
    Result = m_ModIF.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
    GENTLTEST_CHECK_RESULT("Note: IFUpdateDeviceList return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "IFUpdateDeviceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call hasChanged != false", bHasChanged == false);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFUpdateDeviceList::TestIFUpdateDeviceListWithHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFUpdateDeviceList with interface handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;

        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        Result = m_ModIF.eIFUpdateDeviceList(GENTL_INVALID_HANDLE, &bHasChanged, uiTimeout);
        GENTLTEST_CHECK_RESULT("Note: IFUpdateDeviceList return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFUpdateDeviceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call hasChanged != false", bHasChanged == false);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFUpdateDeviceList::TestIFUpdateDeviceListWithChangedNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFUpdateDeviceList with parameter pbChanged = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);
        
        // NULL is allowed and just for an update regardless of the result.
        // Result will be success in any case if hTl is valid
        Result = m_ModIF.eIFUpdateDeviceList(hIF, NULL, uiTimeout);
        GENTLTEST_CHECK_MESSAGE("IFUpdateDeviceList failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

