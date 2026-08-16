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

#ifndef DEVICE_PRECONDITION_INCLUDE___
#define DEVICE_PRECONDITION_INCLUDE___

#include <string>

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

class Device_PreCondition
{
public:
    Device_PreCondition();
    Device_PreCondition(GenICam::Client::IF_HANDLE hIF, 
                         std::string &sDeviceID, 
                         GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess, 
                         GenICam::Client::DEV_HANDLE *hDev);
    ~Device_PreCondition(void);

	GenICam::Client::GC_ERROR eGetLastResult();
    
    GenICam::Client::GC_ERROR eIFOpenDevice(GenICam::Client::IF_HANDLE hIF, 
                           std::string &sDeviceID, 
                           GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess, 
                           GenICam::Client::DEV_HANDLE *hDev);
    void vReopen(GenICam::Client::IF_HANDLE hIF, 
                 std::string &sDeviceID, 
                 GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess, 
                 GenICam::Client::DEV_HANDLE *hDev);

    void vClose();
    GenICam::Client::PORT_HANDLE hDevGetPort();
    uint32_t uiGetNumDataStreams();
    std::string sGetDataStreamID(GenICam::Client::DEV_HANDLE hDev, uint32_t uiIndex);
    
private:
    std::string sGetLastErrorMessage();

private:
    ModGC						m_ModGC;
    ModIF						m_ModIF;
    ModDEV						m_ModDev;

    GenICam::Client::DEV_HANDLE m_hDev;
	GenICam::Client::GC_ERROR	m_LastResult;
};

#endif /* DEVICE_PRECONDITION_INCLUDE___ */