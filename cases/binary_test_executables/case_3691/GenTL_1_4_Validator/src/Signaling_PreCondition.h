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

#ifndef SIGNALING_PRECONDITION_INCLUDE___
#define SIGNALING_PRECONDITION_INCLUDE___

#include <string>
#include <vector>

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

#define EVENTGETDATA_TIMEOUT        60000

class ocGenTLTestParameter;

class Signaling_PreCondition
{
public:
    Signaling_PreCondition();
    Signaling_PreCondition(GenICam::Client::DS_HANDLE hDs, 
                           GenICam::Client::EVENT_TYPE_LIST eEventID, 
                           GenICam::Client::EVENT_HANDLE *hEvent);
    ~Signaling_PreCondition(void);

	GenICam::Client::GC_ERROR eGetLastResult();

    GenICam::Client::GC_ERROR eGCRegisterEvent(GenICam::Client::DS_HANDLE hDs, 
                                               GenICam::Client::EVENT_TYPE_LIST eEventID, 
                                               GenICam::Client::EVENT_HANDLE *hEvent);
    GenICam::Client::GC_ERROR eGCUnregisterEvent(GenICam::Client::DS_HANDLE hDs, 
                                                 GenICam::Client::EVENT_TYPE_LIST eEventID);
    GenICam::Client::GC_ERROR eStartDevice(ocGenTLTestParameter &oParameter);
    GenICam::Client::GC_ERROR eStopDevice(ocGenTLTestParameter &oParameter);
    GenICam::Client::GC_ERROR eEventGetInfoSizeMax(GenICam::Client::EVENT_HANDLE hEvent, size_t *piSize);
    GenICam::Client::GC_ERROR eEventGetInfoEventType(GenICam::Client::EVENT_HANDLE hEvent, int32_t *piEventType);
    GenICam::Client::GC_ERROR eEventGetData(GenICam::Client::EVENT_HANDLE hEvent,
                                                void *pvData,
                                                size_t *iDataSize,
                                                uint64_t uiTimeOut);
    std::string sGetLastErrorMessage();

private:
    ModGC   m_ModGC;
    ModEVENT m_ModEvent;

    GenICam::Client::EVENT_TYPE_LIST m_eEventID;
    GenICam::Client::DS_HANDLE m_hDataStream;
	GenICam::Client::GC_ERROR m_LastResult;
};

#endif /* SIGNALING_PRECONDITION_INCLUDE___ */