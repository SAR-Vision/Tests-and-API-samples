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

#include "Device_PreCondition.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Device_PreCondition::Device_PreCondition()
: m_hDev(GENTL_INVALID_HANDLE)
, m_LastResult(GC_ERR_SUCCESS)
{
}

Device_PreCondition::Device_PreCondition(GenICam::Client::IF_HANDLE hIF, 
                                         std::string &sDeviceID, 
                                         GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess, 
                                         GenICam::Client::DEV_HANDLE *hDev)
: m_hDev(GENTL_INVALID_HANDLE)
, m_LastResult(GC_ERR_SUCCESS)
{
    std::string sMsg;
    
    m_LastResult = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &m_hDev);
	
    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice returned result=" << sConvertGCError2String(m_LastResult).c_str() <<
        " expected result >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED || m_LastResult == GC_ERR_ACCESS_DENIED);

    *hDev = m_hDev;
}

Device_PreCondition::~Device_PreCondition(void)
{
    vClose();
}

////////////////////////////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////////////////////////////

GC_ERROR Device_PreCondition::eGetLastResult()
{
	return m_LastResult;
}

GC_ERROR Device_PreCondition::eIFOpenDevice(GenICam::Client::IF_HANDLE hIF, 
                                            std::string &sDeviceID, 
                                            GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess, 
                                            GenICam::Client::DEV_HANDLE *hDev)
{
    m_LastResult = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &m_hDev);
	
    if (m_LastResult < GC_ERR_SUCCESS)
    {
        std::string sMsg=sGetLastErrorMessage();

        GENTLTEST_PRINT("Info: IFOpenDevice(" << sConvertDEVICEAccess2String(eAccess).c_str() << 
            ") failed with error " << sConvertGCError2String(m_LastResult) << 
            ", error message = '" << sMsg.c_str() << "'" << std::endl);
    } 
    else
    {
        *hDev = m_hDev;
    }

    return m_LastResult;
}

void Device_PreCondition::vReopen(GenICam::Client::IF_HANDLE hIF, 
                                 std::string &sDeviceID, 
                                 GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess, 
                                 GenICam::Client::DEV_HANDLE *hDev)
{
    std::string sMsg;
    
    vClose();

    m_LastResult = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &m_hDev);
	
    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice returned result=" << sConvertGCError2String(m_LastResult).c_str() << 
        " expected result >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED || m_LastResult == GC_ERR_ACCESS_DENIED);

    *hDev = m_hDev;
}

void Device_PreCondition::vClose()
{
    m_LastResult = m_ModDev.eDevClose(m_hDev);
}

GenICam::Client::PORT_HANDLE Device_PreCondition::hDevGetPort()
{
    GenICam::Client::PORT_HANDLE hPort;
    std::string sMsg;
    
    m_LastResult = m_ModDev.eDevGetPort(m_hDev, &hPort);
	
    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DevGetPort returned result="<<sConvertGCError2String(m_LastResult).c_str()<<
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    GENTLTEST_REQUIRE_MESSAGE("DevGetPort returned port handle = GENTL_INVALID_HANDLE", hPort != GENTL_INVALID_HANDLE);

    return hPort;
}

uint32_t Device_PreCondition::uiGetNumDataStreams()
{
    uint32_t uiNumDataStreams=0;
    std::string sMsg;
    
    m_LastResult = m_ModDev.eDevGetNumDataStreams(m_hDev, &uiNumDataStreams);
	
    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("DevGetNumDataStreams returned result=" << sConvertGCError2String(m_LastResult).c_str() <<
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    return uiNumDataStreams;
}
                    
std::string Device_PreCondition::sGetDataStreamID(GenICam::Client::DEV_HANDLE hDev, uint32_t uiIndex)
{
    std::string sResult;
    size_t iSize = 0;
    std::string sMsg;

    m_LastResult = m_ModDev.eDevGetDataStreamID(hDev, uiIndex, NULL, &iSize);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID size check returned result=" << sConvertGCError2String(m_LastResult).c_str() <<
        " expected result >= GC_ERR_SUCCESS, error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS);

    GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID size check, iSize = 0", iSize > 0);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        std::vector<char> vecDataStreamID(iSize);
        m_LastResult = m_ModDev.eDevGetDataStreamID(hDev, uiIndex, &vecDataStreamID[0], &iSize);
        GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID buffer returned result="<<sConvertGCError2String(m_LastResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
            m_LastResult >= GC_ERR_SUCCESS);

        sResult = &vecDataStreamID[0];
    }

    return sResult.c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

std::string Device_PreCondition::sGetLastErrorMessage()
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
