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

#include <string>

#include "GenTLTestTools.h"
#include "LibrarySystemSetup.h"

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

LibrarySystemSetup::LibrarySystemSetup(bool bDoUpdateDeviceList)
: m_hTL(GENTL_INVALID_HANDLE)
, m_bDoUpdateDeviceList(bDoUpdateDeviceList)
, m_poLibraryPreCon(NULL)
, m_poSystemPreCon(NULL)
{
    setUp();
}

LibrarySystemSetup::~LibrarySystemSetup()
{
    tearDown();
}

////////////////////////////////////////////////////////////////////////////////////////////
// setUp / tearDown
////////////////////////////////////////////////////////////////////////////////////////////

void LibrarySystemSetup::setUp (void)
{
    setUpLibrary();

    setUpSystem();
}

void LibrarySystemSetup::tearDown (void)
{
    tearDownSystem();

    tearDownLibrary();
}

void LibrarySystemSetup::setUpLibrary()
{
    if (NULL != m_poLibraryPreCon)
        delete m_poLibraryPreCon;
    m_poLibraryPreCon = new Library_PreCondition();
}

void LibrarySystemSetup::setUpSystem()
{
    if (NULL != m_poSystemPreCon)
        delete m_poSystemPreCon;
    m_poSystemPreCon = new System_PreCondition(&m_hTL, m_bDoUpdateDeviceList);
}

void LibrarySystemSetup::tearDownLibrary()
{
    if (NULL != m_poLibraryPreCon)
        delete m_poLibraryPreCon;
    m_poLibraryPreCon = NULL;
}

void LibrarySystemSetup::tearDownSystem()
{
    if (NULL != m_poSystemPreCon)
        delete m_poSystemPreCon;
    m_poSystemPreCon = NULL;
}

GenICam::Client::TL_HANDLE LibrarySystemSetup::hGetTLHandle()
{
    return m_hTL;
}

GenICam::Client::GC_ERROR LibrarySystemSetup::eTLUpdateInterfaceList(GenICam::Client::TL_HANDLE hTL, bool *pbHasChanged, uint64_t uiTimeout)
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (m_poSystemPreCon != NULL)
    {
        eResult = m_poSystemPreCon->eTLUpdateInterfaceList(hTL, pbHasChanged, uiTimeout);
    }

    return eResult;

}

uint32_t LibrarySystemSetup::zGetTLNumberOfInterfaces()
{
    uint32_t zResult=0;

    if (m_poSystemPreCon != NULL)
    {
        zResult = m_poSystemPreCon->zGetTLNumberOfInterfaces();
    }

    return zResult;
}

System_PreCondition::tStringVector LibrarySystemSetup::xGetTLInterfaceList()
{
    System_PreCondition::tStringVector sResult;

    if (m_poSystemPreCon != NULL)
    {
        sResult = m_poSystemPreCon->xGetTLInterfaceList();
    }

    return sResult;
}

GenICam::Client::GC_ERROR LibrarySystemSetup::eTLOpenInterface(const char *sIfaceID, GenICam::Client::IF_HANDLE *phIface)
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (m_poSystemPreCon != NULL)
    {
        eResult = m_poSystemPreCon->eTLOpenInterface(m_hTL, sIfaceID, phIface);
    }

    return eResult;
}

GenICam::Client::GC_ERROR LibrarySystemSetup::eGetLastError()
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (m_poLibraryPreCon != NULL)
    {
        eResult = m_poLibraryPreCon->eGetLastError();
    }

    return eResult;
}

std::string LibrarySystemSetup::sGetLastErrorMessage()
{
    std::string sResult;

    if (m_poLibraryPreCon != NULL)
    {
        sResult = m_poLibraryPreCon->sGetLastErrorMessage();
    }

    return sResult;
}

GenICam::Client::GC_ERROR LibrarySystemSetup::eGCGetPortInfoPortName(GenICam::Client::PORT_HANDLE hPort, std::string &name)
{
    GenICam::Client::GC_ERROR eResult = GenICam::Client::GC_ERR_SUCCESS;
	
	if (m_poSystemPreCon != NULL)
    {
        eResult = m_poSystemPreCon->eGCGetPortInfoPortName(hPort, name);
    }

    return eResult;
}
