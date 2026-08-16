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

#ifndef SYSTEM_PRECONDITION_INCLUDE___
#define SYSTEM_PRECONDITION_INCLUDE___

#include <string>
#include <vector>

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

class System_PreCondition
{
public:
    typedef std::vector<std::string> tStringVector;

    System_PreCondition(GenICam::Client::TL_HANDLE *hTL, bool bDoUpdateDeviceList=true);
    ~System_PreCondition(void);

    GenICam::Client::GC_ERROR eGetLastResult();
    
    GenICam::Client::GC_ERROR eTLUpdateInterfaceList(GenICam::Client::TL_HANDLE hTL, bool *bHasChanged, uint64_t uiTimeout);
    uint32_t zGetTLNumberOfInterfaces();
    tStringVector xGetTLInterfaceList();
    GenICam::Client::GC_ERROR eTLOpenInterface(GenICam::Client::TL_HANDLE hTL, const char *sIfaceID, GenICam::Client::IF_HANDLE *phIface);
    GenICam::Client::GC_ERROR eGCGetPortInfoPortName(GenICam::Client::PORT_HANDLE hPort, std::string &name);
    
private:
    std::string sGetLastErrorMessage();

private:
    ModGC                       m_ModGC;
    ModTL                       m_ModTL;
    ModPORT                     m_ModPort;

    GenICam::Client::TL_HANDLE  m_hTL;
    GenICam::Client::GC_ERROR	m_LastResult;
};

#endif /* SYSTEM_PRECONDITION_INCLUDE___ */