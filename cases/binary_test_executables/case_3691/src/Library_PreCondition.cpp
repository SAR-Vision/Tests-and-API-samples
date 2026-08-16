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

#include <vector>
#include "GenApi/GenApi.h"

#include "GenTLTestTools.h"
#include "Library_PreCondition.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Library_PreCondition::Library_PreCondition(void)
{
    GC_ERROR Result=GC_ERR_SUCCESS;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    std::string sMsg;
    
    Result = m_ModGC.eGCInitLib();

    if (Result < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("GCInitLib returned result=" << sConvertGCError2String(Result).c_str() << 
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'",  
        Result >= GC_ERR_SUCCESS);
}

Library_PreCondition::~Library_PreCondition(void)
{
    m_ModGC.eGCCloseLib();
}

////////////////////////////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////////////////////////////
 
GenICam::Client::GC_ERROR Library_PreCondition::eGCCloseLib()
{
    GenICam::Client::GC_ERROR eResult=GC_ERR_SUCCESS;

    eResult = m_ModGC.eGCCloseLib();

    return eResult;
}

GenICam::Client::GC_ERROR Library_PreCondition::eGetLastError()
{
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t zSize=0;
    
    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
        eResult >= GC_ERR_SUCCESS);

    return eErrorCode;
}

std::string Library_PreCondition::sGetLastErrorMessage()
{
    std::string sMsg;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t zSize=0;
    
    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
        eResult >= GC_ERR_SUCCESS);

    if (eResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after GCGetLastError call iSize == 0", zSize > 0);

        if (zSize > 0)
        {
            std::vector<char> sBuffer(zSize);
            eResult = m_ModGC.eGCGetLastError(&eErrorCode, &sBuffer[0], &zSize);
            GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
                eResult >= GC_ERR_SUCCESS);

            if (eResult >= GC_ERR_SUCCESS) 
            {
                sMsg = &sBuffer[0];
            }
        }
    }

    return sMsg;
}

