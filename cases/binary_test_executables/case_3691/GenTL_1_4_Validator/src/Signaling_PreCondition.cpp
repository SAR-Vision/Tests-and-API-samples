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

#include "Signaling_PreCondition.h"
#include "GenTLTestParameter.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Signaling_PreCondition::Signaling_PreCondition()
: m_eEventID(GenICam::Client::EVENT_ERROR)
, m_hDataStream(GENTL_INVALID_HANDLE)
, m_LastResult(GC_ERR_SUCCESS)
{
}

Signaling_PreCondition::Signaling_PreCondition(GenICam::Client::DS_HANDLE hDs, 
                                               GenICam::Client::EVENT_TYPE_LIST eEventID, 
                                               GenICam::Client::EVENT_HANDLE *hEvent)
{
    m_eEventID = eEventID;
    m_hDataStream = hDs;
    eGCRegisterEvent(hDs, eEventID, hEvent);
}

Signaling_PreCondition::~Signaling_PreCondition(void)
{
    if (m_hDataStream != GENTL_INVALID_HANDLE)
    {
        eGCUnregisterEvent(m_hDataStream, m_eEventID);
    }
}

GC_ERROR Signaling_PreCondition::eGetLastResult()
{
	return m_LastResult;
}

GenICam::Client::GC_ERROR Signaling_PreCondition::eGCRegisterEvent(GenICam::Client::DS_HANDLE hDs, 
                                                                   GenICam::Client::EVENT_TYPE_LIST eEventID, 
                                                                   GenICam::Client::EVENT_HANDLE *hEvent)
{
    std::string sMsg;

    m_LastResult = m_ModEvent.eGCRegisterEvent(hDs, eEventID, hEvent);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("GCRegisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message ='" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);
    
    return m_LastResult;
}

GenICam::Client::GC_ERROR Signaling_PreCondition::eGCUnregisterEvent(GenICam::Client::DS_HANDLE hDs, 
                                                                     GenICam::Client::EVENT_TYPE_LIST eEventID)
{
    m_LastResult = m_ModEvent.eGCUnregisterEvent(hDs, eEventID);
/*  GENTLTEST_REQUIRE_MESSAGE("GCUnregisterEvent eventID=" << sConvertEventID2String(eEventID).c_str() << 
        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str(), 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);*/

    m_eEventID = GenICam::Client::EVENT_ERROR;
    m_hDataStream = GENTL_INVALID_HANDLE;

    return m_LastResult;
}

GenICam::Client::GC_ERROR Signaling_PreCondition::eStartDevice(ocGenTLTestParameter &oParameter)
{
    GenApi::CCommandPtr oStartAcq = oParameter.xGetXMLNodeCommand("AcquisitionStart");
    GENTLTEST_REQUIRE_MESSAGE("Device AcquisitionStart not valid", oStartAcq.IsValid());
    try 
    {
        oStartAcq->Execute();
        m_LastResult = GC_ERR_SUCCESS;
    }
    catch (GenICam::AccessException ex)
    {
        GENTLTEST_REQUIRE_MESSAGE("Error: access problem node='AcquisitionStart', details=" << ex.GetDescription() << std::endl, false);
		m_LastResult = GC_ERR_ERROR;
    }

    return m_LastResult;
}

GenICam::Client::GC_ERROR Signaling_PreCondition::eStopDevice(ocGenTLTestParameter &oParameter)
{
    GenApi::CCommandPtr oStopAcq = oParameter.xGetXMLNodeCommand("AcquisitionStop");
    GENTLTEST_REQUIRE_MESSAGE("Device AcquisitionStop not valid", oStopAcq.IsValid());
    try 
    {
        oStopAcq->Execute();
        m_LastResult = GC_ERR_SUCCESS;
    }
    catch (GenICam::AccessException ex)
    {
        GENTLTEST_REQUIRE_MESSAGE("Error: access problem node='AcquisitionStop', details=" << ex.GetDescription() << std::endl, false);
		m_LastResult = GC_ERR_ERROR;
    }

    return m_LastResult;
}

GenICam::Client::GC_ERROR Signaling_PreCondition::eEventGetInfoSizeMax(GenICam::Client::EVENT_HANDLE hEvent, size_t *piSizeMax)
{
    INFO_DATATYPE iType=0;
    size_t iSize=0;
    std::string sMsg;
    
    m_LastResult = m_ModEvent.eEventGetInfo(hEvent, EVENT_SIZE_MAX, &iType, NULL, &iSize);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("EventGetInfo size check command=EVENT_SIZE_MAX" << 
        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message = '" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);
    
    GENTLTEST_REQUIRE_MESSAGE("EventGetInfo size check, iSize = 0", iSize != 0);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        std::vector<char> sBuffer(iSize);

        iSize = sizeof (size_t);
        m_LastResult = m_ModEvent.eEventGetInfo(hEvent, EVENT_SIZE_MAX, &iType, piSizeMax, &iSize);
        GENTLTEST_REQUIRE_MESSAGE("EventGetInfo with initialized buffer command=EVENT_SIZE_MAX" << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str(), 
            m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);
    }

    return m_LastResult;
}

GenICam::Client::GC_ERROR Signaling_PreCondition::eEventGetInfoEventType(GenICam::Client::EVENT_HANDLE hEvent, int32_t *piEventType)
{
    INFO_DATATYPE iType=0;
    size_t iSize=0;
    std::string sMsg;
    
    m_LastResult = m_ModEvent.eEventGetInfo(hEvent, EVENT_EVENT_TYPE, &iType, NULL, &iSize);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("EventGetInfo size check command=EVENT_EVENT_TYPE" << 
        " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str() <<
        ", error message = '" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);
    
    GENTLTEST_REQUIRE_MESSAGE("EventGetInfo size check, iSize = 0", iSize != 0);

    if (m_LastResult >= GC_ERR_SUCCESS)
    {
        std::vector<char> sBuffer(iSize);

        iSize = sizeof (size_t);
        m_LastResult = m_ModEvent.eEventGetInfo(hEvent, EVENT_EVENT_TYPE, &iType, piEventType, &iSize);
        GENTLTEST_REQUIRE_MESSAGE("EventGetInfo with initialized buffer command=EVENT_EVENT_TYPE" << 
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(m_LastResult).c_str(), 
            m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_NOT_IMPLEMENTED);
    }

    return m_LastResult;
}


GenICam::Client::GC_ERROR Signaling_PreCondition::eEventGetData(GenICam::Client::EVENT_HANDLE hEvent,
                                                                void *pvData,
                                                                size_t *iDataSize,
                                                                uint64_t uiTimeOut)
{
    std::string sMsg;

    m_LastResult = m_ModEvent.eEventGetData(hEvent, pvData, iDataSize, uiTimeOut);

    if (m_LastResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("EventGetData  failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_TIMEOUT, received " << 
        sConvertGCError2String(m_LastResult).c_str() <<
        ", error message = '" << sMsg.c_str() << "'", 
        m_LastResult >= GC_ERR_SUCCESS || m_LastResult == GC_ERR_TIMEOUT);

    return m_LastResult;
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

std::string Signaling_PreCondition::sGetLastErrorMessage()
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
