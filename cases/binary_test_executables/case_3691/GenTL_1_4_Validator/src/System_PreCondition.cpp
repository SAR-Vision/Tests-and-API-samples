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

#include "System_PreCondition.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

System_PreCondition::System_PreCondition(GenICam::Client::TL_HANDLE *hTL, bool bDoUpdateDeviceList)
: m_hTL(GENTL_INVALID_HANDLE)
, m_LastResult(GC_ERR_SUCCESS)
{
    std::string sMsg;
    
    m_LastResult = m_ModTL.eTLOpen(&m_hTL);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("TLOpen returned result=" << sConvertGCError2String(m_LastResult).c_str() << 
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    GENTLTEST_REQUIRE_MESSAGE("TLOpen returned tl handle = GENTL_INVALID_HANDLE", hTL != GENTL_INVALID_HANDLE);

    *hTL = m_hTL;

    if (bDoUpdateDeviceList)
    {
        zGetTLNumberOfInterfaces();
    }
}

System_PreCondition::~System_PreCondition(void)
{
    m_ModTL.eTLClose(m_hTL);
}

////////////////////////////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////////////////////////////

GC_ERROR System_PreCondition::eGetLastResult()
{
	return m_LastResult;
}

GenICam::Client::GC_ERROR System_PreCondition::eTLUpdateInterfaceList(GenICam::Client::TL_HANDLE hTL, bool *pbHasChanged, uint64_t uiTimeout)
{
    std::string sMsg;

    m_LastResult = m_ModTL.eTLUpdateInterfaceList(hTL, pbHasChanged, uiTimeout);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("TLUpdateInterfaceList failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'",  
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_TIMEOUT);

    return m_LastResult;
}

uint32_t System_PreCondition::zGetTLNumberOfInterfaces()
{
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    uint32_t uiNumInterfaces = 0;
    std::string sMsg;
    
    eTLUpdateInterfaceList(m_hTL, &bHasChanged, uiTimeout);
    
    m_LastResult = m_ModTL.eTLGetNumInterfaces(m_hTL, &uiNumInterfaces);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("Error: **************************************************************" << std::endl <<
        "Error: * For GenICam validation at least one Interface should exist *" << std::endl <<
        "Error: * Returned uiNumInterfaces = " << uiNumInterfaces << " *" << std::endl <<
        "Error: * Returned result = " << sConvertGCError2String(m_LastResult).c_str() << " *" << std::endl <<
        "Error: * Error message = '" << sMsg.c_str() << "' *" << std::endl <<  
        "Error: **************************************************************" << std::endl, 
        m_LastResult >= GC_ERR_SUCCESS && uiNumInterfaces != 0);

    return uiNumInterfaces;
}

System_PreCondition::tStringVector System_PreCondition::xGetTLInterfaceList()
{
    tStringVector xResult;
    bool bHasChanged = false;
    uint64_t uiTimeout = 1000;
    uint32_t uiNumInterfaces = 0;
    std::string sMsg;
    
    eTLUpdateInterfaceList(m_hTL, &bHasChanged, uiTimeout);

    uiNumInterfaces = zGetTLNumberOfInterfaces();
    
    for (uint32_t index=0; index<uiNumInterfaces; index++)
    {
        size_t iSize = 0;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        
        m_LastResult = m_ModTL.eTLGetInterfaceID(m_hTL, index, NULL, &iSize);

        if (m_LastResult < GC_ERR_SUCCESS)
        {
            sMsg = sGetLastErrorMessage();
        }

        GENTLTEST_REQUIRE_MESSAGE("TLGetInterfaceID size check returned result=" << sConvertGCError2String(m_LastResult).c_str() << 
            " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'",
            m_LastResult >= GC_ERR_SUCCESS);

        GENTLTEST_REQUIRE_MESSAGE("TLGetInterfaceID size check, iSize = 0", iSize > 0);

        if (m_LastResult >= GC_ERR_SUCCESS)
        {
            std::vector<char> InterfaceID(iSize);
            m_LastResult = m_ModTL.eTLGetInterfaceID(m_hTL, index, &InterfaceID[0], &iSize);
            GENTLTEST_REQUIRE_MESSAGE("TLGetInterfaceID buffer returned result="<<sConvertGCError2String(m_LastResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
                m_LastResult >= GC_ERR_SUCCESS);

            xResult.push_back(std::string(&InterfaceID[0]));
        }
    }

    return xResult;
}

GenICam::Client::GC_ERROR System_PreCondition::eTLOpenInterface(GenICam::Client::TL_HANDLE hTL, const char *sIfaceID, GenICam::Client::IF_HANDLE *phIface)
{
    std::string sMsg;

    m_LastResult = m_ModTL.eTLOpenInterface(hTL, sIfaceID, phIface);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("TLOpenInterface returned result=" << sConvertGCError2String(m_LastResult).c_str() << 
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'",
        m_LastResult >= GC_ERR_SUCCESS);

    return m_LastResult;
}

GenICam::Client::GC_ERROR System_PreCondition::eGCGetPortInfoPortName(GenICam::Client::PORT_HANDLE hPort, std::string &name)
{
    std::string sMsg;
    INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
    size_t iSize=0;
    
    m_LastResult = m_ModPort.eGCGetPortInfo(hPort, PORT_INFO_PORTNAME, &iType, NULL, &iSize);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("GCGetPortInfo PORT_INFO_PORTNAME (hPort=0x" << std::hex << hPort << std::dec <<
        ") failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'",
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

        std::vector<char> sBuffer(iSize);
        m_LastResult = m_ModPort.eGCGetPortInfo(hPort, PORT_INFO_PORTNAME, &iType, &sBuffer[0], &iSize);
        GENTLTEST_CHECK_MESSAGE("GCGetPortInfo failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str(),  
            m_LastResult >= GC_ERR_SUCCESS);

        name = &sBuffer[0];
    }

    return m_LastResult;
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

std::string System_PreCondition::sGetLastErrorMessage()
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
