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

#include "GenApi/GenApi.h"

#include "Interface_PreCondition.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_PreCondition::Interface_PreCondition(GenICam::Client::TL_HANDLE hTL, std::string &sInterfaceID, GenICam::Client::IF_HANDLE *hIF, bool bDoUpdateDeviceList)
: m_hIF(GENTL_INVALID_HANDLE)
, m_LastResult(GC_ERR_SUCCESS)
{
    std::string sMsg;
    
    m_LastResult = m_ModTL.eTLOpenInterface(hTL, sInterfaceID.c_str(), &m_hIF);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("TLOpenInterface returned result=" << sConvertGCError2String(m_LastResult).c_str() <<
        ", expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    GENTLTEST_REQUIRE_MESSAGE("TLOpenInterface returned interface handle = GENTL_INVALID_HANDLE", hIF != GENTL_INVALID_HANDLE);

    *hIF = m_hIF;

    if (bDoUpdateDeviceList)
    {
        zGetIFNumberOfDevices();
    }
}

Interface_PreCondition::~Interface_PreCondition(void)
{
    vClose();
}

////////////////////////////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////////////////////////////

GC_ERROR Interface_PreCondition::eGetLastResult()
{
	return m_LastResult;
}

void Interface_PreCondition::vClose()
{
    m_ModIF.eIFClose(m_hIF);
}

GenICam::Client::GC_ERROR Interface_PreCondition::eIFUpdateDeviceList(GenICam::Client::IF_HANDLE hIF, bool *pbHasChanged, uint64_t uiTimeout)
{
    std::string sMsg;

    m_LastResult = m_ModIF.eIFUpdateDeviceList(hIF, pbHasChanged, uiTimeout);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("IFUpdateDeviceList returned result=" << sConvertGCError2String(m_LastResult).c_str() << 
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

uint32_t Interface_PreCondition::zGetIFNumberOfDevices()
{
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    uint32_t uiNumDevices = 0;
    std::string sMsg;
    
    eIFUpdateDeviceList(m_hIF, &bHasChanged, uiTimeout);
    
    m_LastResult = m_ModIF.eIFGetNumDevices(m_hIF, &uiNumDevices);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }
    
    GENTLTEST_REQUIRE_MESSAGE("IFGetNumDevices returned result = " << sConvertGCError2String(m_LastResult).c_str() << 
        " expected result >= GC_ERR_SUCCESS, error message = '" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return uiNumDevices;
}

std::string Interface_PreCondition::sGetDeviceID(GenICam::Client::IF_HANDLE hIF, uint32_t iIndex)
{
    std::string sResult;
    size_t iSize = 0;
    std::string sMsg;
            
    m_LastResult = m_ModIF.eIFGetDeviceID(hIF, iIndex, NULL, &iSize);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID size check returned result=" << sConvertGCError2String(m_LastResult).c_str() <<
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID size check, iSize = 0", iSize > 0);

        std::vector<char> vDeviceID(iSize);
        m_LastResult = m_ModIF.eIFGetDeviceID(hIF, iIndex, &vDeviceID[0], &iSize);
        GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID buffer returned result="<<sConvertGCError2String(m_LastResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
            m_LastResult >= GC_ERR_SUCCESS);

        sResult = &vDeviceID[0];
    }

    return sResult.c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

std::string Interface_PreCondition::sGetLastErrorMessage()
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
