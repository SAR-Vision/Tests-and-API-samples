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

#include "Interface_IFGetDeviceID.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFGetDeviceID::Interface_IFGetDeviceID()
{
}

Interface_IFGetDeviceID::~Interface_IFGetDeviceID()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetDeviceID::setUp(void)
{
}

void Interface_IFGetDeviceID::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFGetDeviceID
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFGetDeviceID::TestIFGetDeviceID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFGetDeviceID");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    uint32_t uiTotalNumDevices = 0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        uiTotalNumDevices += uiNumDevices;

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;
            
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                std::vector<char> sDeviceID(iSize);
                Result = m_ModIF.eIFGetDeviceID(hIF, index2, &sDeviceID[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_PRINT("Info: (interfaceID=" << xInterfaceList[index1].c_str() << ") device index=" << index2+1 << "/" << uiNumDevices <<
                            " = '" << &sDeviceID[0] << "'" << std::endl);
                }
            }
            else
            {
                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                GENTLTEST_PRINT("Error: (interfaceID=" << xInterfaceList[index1].c_str() << ") device index=" << index2+1 << "/" << uiNumDevices << " " << 
                    sConvertGCError2String(Result).c_str() << 
                    " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
            }
        }
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

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with closed library before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        oLibSysSetup.tearDownLibrary();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;
            
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_NOT_INITIALIZED);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call size != 0", iSize == 0);
            }
        }

        oLibSysSetup.tearDownSystem();
        oLibSysSetup.setUp();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}


void Interface_IFGetDeviceID::TestIFGetDeviceIDWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with closed system before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        oLibSysSetup.tearDownSystem();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;
            
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);

            if (Result < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call wrong size != 0", iSize == 0);
            }
        }

        oLibSysSetup.setUpSystem();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        {
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

            uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        }

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;
            
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithIfHandleNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with parameter interface handle GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;

            Result = m_ModIF.eIFGetDeviceID(GENTL_INVALID_HANDLE, index2, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE,
                "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithSizeNULL1( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with parameter piSize = NULL in size check call");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, NULL);
            GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER,
                "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithSizeNULL2( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with parameter piSize = NULL in call with initialized buffer");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;

            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                std::vector<char> vDeviceID(iSize);
                Result = m_ModIF.eIFGetDeviceID(hIF, index2, &vDeviceID[0], NULL);
                GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " return result: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER,
                    "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " with initialized buffer failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithInvalidIndex( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with parameter index > number of devices");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=uiNumDevices; index2<uiNumDevices+10; index2++)
        {
            size_t iSize = 0;

            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER,
                "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDWithSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID with parameter iSize = iSize-1 in call with initialized buffer");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;

            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                std::vector<char> vDeviceID(iSize);
                iSize--;
                Result = m_ModIF.eIFGetDeviceID(hIF, index2, &vDeviceID[0], &iSize);
                GENTLTEST_CHECK_RESULT("Note: IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER,
                    "IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " with initialized buffer failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFGetDeviceID::TestIFGetDeviceIDPersistence( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFGetDeviceID persistence after IFUpdateDeviceList");
    LibrarySystemSetup oLibSysSetup;
    std::vector<std::string> vecDeviceIDList;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;
            
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                std::vector<char> sDeviceID(iSize);
                Result = m_ModIF.eIFGetDeviceID(hIF, index2, &sDeviceID[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

                if (Result >= GC_ERR_SUCCESS)
                {
                    vecDeviceIDList.push_back(&sDeviceID[0]);
                }
            }
        }

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            size_t iSize = 0;
            
            Result = m_ModIF.eIFGetDeviceID(hIF, index2, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                " size check failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                std::vector<char> sDeviceID(iSize);
                Result = m_ModIF.eIFGetDeviceID(hIF, index2, &sDeviceID[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("IFGetDeviceID InterfaceID=" << xInterfaceList[index1].c_str() << " device index=" << index2+1 << "/" << uiNumDevices <<
                    " with initialized buffer failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS);

                if (Result >= GC_ERR_SUCCESS)
                {
                    bool bFound=false;
                    std::vector<std::string>::iterator xIter;
                    for (xIter=vecDeviceIDList.begin(); xIter!=vecDeviceIDList.end(); xIter++)
                    {
                        if (*xIter == std::string(&sDeviceID[0]))
                        {
                            bFound = true;
                            break;
                        }
                    }
                    GENTLTEST_CHECK_MESSAGE("DeviceID = " << &sDeviceID[0] << " not found", bFound);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

