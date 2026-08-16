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

#include "System_TLGetInterfaceID.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_TLGetInterfaceID::System_TLGetInterfaceID()
{
}

System_TLGetInterfaceID::~System_TLGetInterfaceID()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetInterfaceID::setUp(void)
{
}

void System_TLGetInterfaceID::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test TLGetInterfaceID
////////////////////////////////////////////////////////////////////////////////////////////

void System_TLGetInterfaceID::TestTLGetInterfaceID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard TLGetInterfaceID");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

            std::vector<char> sInterfaceID(iSize);
            Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, &sInterfaceID[0], &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                GENTLTEST_PRINT("Info: " << __FUNCTION__ << " (" << index+1 << "/" << uiNumInterfaces 
                    << "), interface ID='" << &sInterfaceID[0] << "' checked: " << sConvertGCError2String(Result).c_str() << std::endl);
            }
        }
        else
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call, iSize != 0", iSize == 0);

            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            GENTLTEST_PRINT("Error: " << sConvertGCError2String(Result).c_str() << 
                " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID with closed library before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    TL_HANDLE hTL=oLibSysSetup.hGetTLHandle();
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    oLibSysSetup.tearDown();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInterfaceID(hTL, index, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_NOT_INITIALIZED);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call, iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID with closed system before");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    TL_HANDLE hTL=oLibSysSetup.hGetTLHandle();
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    oLibSysSetup.tearDownSystem();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;
        
        Result = m_ModTL.eTLGetInterfaceID(hTL, index, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceID return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "TLGetInterfaceID failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call, iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDWithTLHandleNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID with TL handle = GENTL_INVALID_HANDLE");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;

        Result = m_ModTL.eTLGetInterfaceID(GENTL_INVALID_HANDLE, index, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceID return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_HANDLE, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_HANDLE,
            "TLGetInterfaceID failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call, iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDWithSizeNULL1( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID with piSize NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, NULL, NULL);
        GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceID return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER,
            "TLGetInterfaceID failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDWithSizeNULL2( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID with initialized buffer and piSize NULL");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;

        Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

            std::vector<char> InterfaceID(iSize);
            Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, &InterfaceID[0], NULL);
            GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceID return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER,
                "TLGetInterfaceID failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
        else
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call, iSize != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDWithInvalidIndex( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID with index > number interfaces");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=uiNumInterfaces; index<uiNumInterfaces+10; index++)
    {
        size_t iSize = 0;

        Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, NULL, &iSize);
        GENTLTEST_CHECK_RESULT("Note: TLGetInterfaceID return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_PARAMETER,
            "TLGetInterfaceID failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call Size != 0", iSize == 0);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void System_TLGetInterfaceID::TestTLGetInterfaceIDPersistence( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test TLGetInterfaceID persistence after TLUpdateInterfaceList");
    LibrarySystemSetup oLibSysSetup;
    GC_ERROR Result=GC_ERR_SUCCESS;
    uint32_t uiNumInterfaces=0;
    std::vector<std::string> vecInterfaceIDList;
    
    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;

        Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

            std::vector<char> sInterfaceID(iSize);
            Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, &sInterfaceID[0], &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                vecInterfaceIDList.push_back(&sInterfaceID[0]);
            }
        }
    }

    uiNumInterfaces = oLibSysSetup.zGetTLNumberOfInterfaces();

    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;

        Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, NULL, &iSize);
        GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result >= GC_ERR_SUCCESS);

        if (Result >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

            std::vector<char> sInterfaceID(iSize);
            Result = m_ModTL.eTLGetInterfaceID(oLibSysSetup.hGetTLHandle(), index, &sInterfaceID[0], &iSize);
            GENTLTEST_CHECK_MESSAGE("TLGetInterfaceID failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS);

            if (Result >= GC_ERR_SUCCESS)
            {
                bool bFound=false;
                std::vector<std::string>::iterator xIter;
                for (xIter=vecInterfaceIDList.begin(); xIter!=vecInterfaceIDList.end(); xIter++)
                {
                    if (*xIter == std::string(&sInterfaceID[0]))
                    {
                        bFound = true;
                        break;
                    }
                }
                GENTLTEST_CHECK_MESSAGE("InterfaceID = " << &sInterfaceID[0] << " not found", bFound);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

