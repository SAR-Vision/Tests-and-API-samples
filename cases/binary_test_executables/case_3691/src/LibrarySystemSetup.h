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

#ifndef LIBRARYSYSTEMSETUP_H
#define LIBRARYSYSTEMSETUP_H

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

#include "Library_PreCondition.h"
#include "System_PreCondition.h"

class LibrarySystemSetup
{
public:
    LibrarySystemSetup(bool bDoUpdateDeviceList=true);
    ~LibrarySystemSetup();
    
public:
    void setUp();
    void tearDown();

    void setUpLibrary();
    void setUpSystem();

    void tearDownLibrary();
    void tearDownSystem();

    GenICam::Client::TL_HANDLE hGetTLHandle();
    GenICam::Client::GC_ERROR eTLUpdateInterfaceList(GenICam::Client::TL_HANDLE hTL, bool *pbHasChanged, uint64_t uiTimeout);
    uint32_t zGetTLNumberOfInterfaces();
    System_PreCondition::tStringVector xGetTLInterfaceList();
    GenICam::Client::GC_ERROR eTLOpenInterface(const char *sIfaceID, GenICam::Client::IF_HANDLE *phIface);
    GenICam::Client::GC_ERROR eGetLastError();
    std::string sGetLastErrorMessage();
    GenICam::Client::GC_ERROR eGCGetPortInfoPortName(GenICam::Client::PORT_HANDLE hPort, std::string &name);

private:
    Library_PreCondition *m_poLibraryPreCon;
    System_PreCondition *m_poSystemPreCon;

    ModIF     m_ModIF;

    GenICam::Client::TL_HANDLE m_hTL;

    bool m_bDoUpdateDeviceList;
};

#endif  //LIBRARYSYSTEMSETUP_H
