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

#include "Interface_IFGetNumDevices.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFGetNumDevices::Interface_IFGetNumDevices()
{
}

Interface_IFGetNumDevices::~Interface_IFGetNumDevices()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetNumDevices::setUp(void)
{
}

void Interface_IFGetNumDevices::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLGetNumInterfaces
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetNumDevices::TestIFGetNumDevices( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFGetNumDevices");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    uint32_t uiTotalNumDevices = 0;

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices = 0;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        
        Result = m_ModIF.eIFGetNumDevices(hIF, &uiNumDevices);
        GENTLTEST_CHECK_MESSAGE("IFGetNumDevices failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
        }
        else
        {
            uiTotalNumDevices += uiNumDevices;
        }

        GENTLTEST_PRINT("Info: " << __FUNCTION__ 
                        << " interfaceID=" << xInterfaceList[index].c_str() 
                        << " NumDevices=" << uiNumDevices 
                        << " checked: " << sConvertGCError2String(Result).c_str() 
                        << std::endl);

        
    }

    // for certification we need at least one device
    GENTLTEST_CHECK_MESSAGE(std::endl << "Error: ***********************************************************" << std::endl <<
        "Error: * For GenICam Validation at least one device should exist *" << std::endl <<
        "Error: * Returned uiTotalNumDevices = " << uiTotalNumDevices << " *" << std::endl <<
        "Error: * Aborting validation                                     *" << std::endl <<
        "Error: ***********************************************************" << std::endl, 
        uiTotalNumDevices > 0);

    if (uiTotalNumDevices == 0)
        g_bStopValidationFlag = true;

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetNumDevices::TestIFGetNumDevicesWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetNumDevices with closed library before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices = 0;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        bool bHasChanged = false;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        
        oLibSysSetup.tearDownLibrary();
        
        Result = m_ModIF.eIFGetNumDevices(hIF, &uiNumDevices);
        GENTLTEST_CHECK_MESSAGE("IFGetNumDevices failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_NOT_INITIALIZED);
        
        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iNumDevices != 0", uiNumDevices == 0);
        }

        oLibSysSetup.tearDownSystem();
        oLibSysSetup.setUp();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetNumDevices::TestIFGetNumDevicesWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetNumDevices with closed system before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices = 0;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        bool bHasChanged = false;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);

        oLibSysSetup.tearDownSystem();
        
        Result = m_ModIF.eIFGetNumDevices(hIF, &uiNumDevices);
        GENTLTEST_CHECK_RESULT("Note: IFGetNumDevices return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFGetNumDevices failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);
        
        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iNumDevices != 0", uiNumDevices == 0);
        }

        oLibSysSetup.setUpSystem();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetNumDevices::TestIFGetNumDevicesWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetNumDevices with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices = 0;
        {
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

            oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        }
        
        Result = m_ModIF.eIFGetNumDevices(hIF, &uiNumDevices);
        GENTLTEST_CHECK_RESULT("Note: IFGetNumDevices return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFGetNumDevices failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iNumDevices != 0", uiNumDevices == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetNumDevices::TestIFGetNumDevicesWithHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetNumDevices with interface handle GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices = 0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);
        
        oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        
        Result = m_ModIF.eIFGetNumDevices(GENTL_INVALID_HANDLE, &uiNumDevices);
        GENTLTEST_CHECK_RESULT("Note: IFGetNumDevices return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFGetNumDevices failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iNumDevices != 0", uiNumDevices == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetNumDevices::TestIFGetNumDevicesWithNumbersNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetNumDevices with parameter piNumDevices = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        IF_HANDLE hIFSave = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        hIFSave = hIF;

        oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);
        
        Result = m_ModIF.eIFGetNumDevices(hIF, NULL);
        GENTLTEST_CHECK_RESULT("Note: IFGetNumDevices return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER,
            "IFGetNumDevices failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call interface handle != input interface handle", hIFSave == hIF);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetNumDevices::TestIFGetNumDevicesWithoutUpdate( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetNumDevices without update device list before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index=0; index<xInterfaceList.size(); index++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        bool bHasChanged = false;
        uint64_t uiTimeout = 1000;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        IF_HANDLE hIFSave = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices = 0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index], &hIF, false);

        hIFSave = hIF;

        Result = m_ModIF.eIFGetNumDevices(hIF, &uiNumDevices);
        GENTLTEST_CHECK_MESSAGE("IFGetNumDevices failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_AVAILABLE);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong handle returned", hIFSave == hIF);
            GENTLTEST_CHECK_MESSAGE("Parameter after call wrong number returned", uiNumDevices == 0);
        }

        oIFPreCondition.eIFUpdateDeviceList(hIF, &bHasChanged, uiTimeout);

        Result = m_ModIF.eIFGetNumDevices(hIF, &uiNumDevices);
        GENTLTEST_CHECK_MESSAGE("IFGetNumDevices failed", 
            Result >= GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

