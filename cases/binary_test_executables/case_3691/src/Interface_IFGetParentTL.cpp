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

#include "Interface_IFGetParentTL.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFGetParentTL::Interface_IFGetParentTL()
{
}

Interface_IFGetParentTL::~Interface_IFGetParentTL()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetParentTL::setUp(void)
{
}

void Interface_IFGetParentTL::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFOpenDevice
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetParentTL::TestIFGetParentTL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFGetParentTL");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int                                 nTotalNumberOfTests = 0;
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR                Result = GC_ERR_SUCCESS;
        IF_HANDLE               hIF = GENTL_INVALID_HANDLE;
        TL_HANDLE               hSystem = GENTL_INVALID_HANDLE;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        Result = m_ModIF.eIFGetParentTL(hIF, &hSystem);

        GENTLTEST_CHECK_MESSAGE("IFGetParentTL failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);
        
        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("IFGetParentTL failed. Expected handle=0x" << std::hex << oLibSysSetup.hGetTLHandle() << " , received 0x" << hSystem, 
                    hSystem == oLibSysSetup.hGetTLHandle());
        }

        nTotalNumberOfTests++;
    }

    GENTLTEST_CHECK_MESSAGE("IFGetParentTL failed. No interface available", nTotalNumberOfTests > 0);

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetParentTL::TestIFGetParentTLWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetParentTL with closed library before");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR                Result = GC_ERR_SUCCESS;
        IF_HANDLE               hIF = GENTL_INVALID_HANDLE;
        TL_HANDLE               hSystem = GENTL_INVALID_HANDLE;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oLibSysSetup.tearDownLibrary();
        
        Result = m_ModIF.eIFGetParentTL(hIF, &hSystem);
        GENTLTEST_CHECK_RESULT("Note: IFGetParentTL recommended return value == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_NOT_INITIALIZED,
            "IFGetParentTL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call hSystem != GENTL_INVALID_HANDLE", hSystem == GENTL_INVALID_HANDLE);
        }

        oLibSysSetup.tearDownSystem();
        oLibSysSetup.setUp();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetParentTL::TestIFGetParentTLWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetParentTL with closed system before");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR                Result=GC_ERR_SUCCESS;
        IF_HANDLE               hIF = GENTL_INVALID_HANDLE;
        TL_HANDLE               hSystem = GENTL_INVALID_HANDLE;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oLibSysSetup.tearDownSystem();
        
        Result = m_ModIF.eIFGetParentTL(hIF, &hSystem);
        GENTLTEST_CHECK_RESULT("Note: IFGetParentTL recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFGetParentTL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call hSystem != GENTL_INVALID_HANDLE", hSystem == GENTL_INVALID_HANDLE);
        }

        oLibSysSetup.setUpSystem();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetParentTL::TestIFGetParentTLWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetParentTL with closed interface before");
    LibrarySystemSetup                  oLibSysSetup;
    GC_ERROR                            Result = GC_ERR_SUCCESS;
    System_PreCondition::tStringVector  xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    TL_HANDLE                           hSystem = GENTL_INVALID_HANDLE;
    IF_HANDLE                           hIF = GENTL_INVALID_HANDLE;
    Interface_PreCondition              oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[0], &hIF, false);

    oLibSysSetup.tearDownSystem();
    
    Result = m_ModIF.eIFGetParentTL(hIF, &hSystem);
    GENTLTEST_CHECK_RESULT("Note: IFGetParentTL recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
        Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
        "IFGetParentTL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
        Result < GC_ERR_SUCCESS);

    if (Result < GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after call hSystem != GENTL_INVALID_HANDLE", hSystem == GENTL_INVALID_HANDLE);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetParentTL::TestIFGetParentTLWithIFNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetParentTL with interface handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList = oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        IF_HANDLE               hIF = GENTL_INVALID_HANDLE;
        TL_HANDLE               hSystem = GENTL_INVALID_HANDLE;
        GC_ERROR                Result = GC_ERR_SUCCESS;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        Result = m_ModIF.eIFGetParentTL(GENTL_INVALID_HANDLE, &hSystem);
        GENTLTEST_CHECK_RESULT("Note: IFGetParentTL recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFUpdateDeviceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call hSystem != GENTL_INVALID_HANDLE", hSystem == GENTL_INVALID_HANDLE);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetParentTL::TestIFGetParentTLWithSystemNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetParentTL with pointer hSystem = NULL");
    LibrarySystemSetup                  oLibSysSetup;
    System_PreCondition::tStringVector  xInterfaceList = oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE               hIF = GENTL_INVALID_HANDLE;
        GC_ERROR                Result = GC_ERR_SUCCESS;
        Interface_PreCondition  oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF, false);

        Result = m_ModIF.eIFGetParentTL(hIF, NULL);
        GENTLTEST_CHECK_RESULT("Note: IFGetParentTL recommended return value == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFUpdateDeviceList failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

