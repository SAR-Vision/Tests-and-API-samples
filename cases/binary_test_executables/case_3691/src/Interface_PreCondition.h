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

#ifndef INTERFACE_PRECONDITION_INCLUDE___
#define INTERFACE_PRECONDITION_INCLUDE___

#include <string>

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

class Interface_PreCondition
{
public:
    Interface_PreCondition(GenICam::Client::TL_HANDLE hTL, std::string &sInterfaceID, GenICam::Client::IF_HANDLE *hIF, bool bDoUpdateDeviceList=true);
    ~Interface_PreCondition(void);
    
    GenICam::Client::GC_ERROR eGetLastResult();
    
    void vClose();
    GenICam::Client::GC_ERROR eIFUpdateDeviceList(GenICam::Client::IF_HANDLE hIF, bool *pbHasChanged, uint64_t uiTimeout);
    uint32_t zGetIFNumberOfDevices();
    std::string sGetDeviceID(GenICam::Client::IF_HANDLE hIF, uint32_t iIndex);

private:
    std::string sGetLastErrorMessage();

private:
    ModGC                       m_ModGC;
    ModTL                       m_ModTL;
    ModIF                       m_ModIF;

    GenICam::Client::IF_HANDLE  m_hIF;
    GenICam::Client::GC_ERROR	m_LastResult;
};

#endif /* INTERFACE_PRECONDITION_INCLUDE___ */