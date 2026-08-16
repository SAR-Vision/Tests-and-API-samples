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

#include "GenApi/GenApi.h"

#include "Library_GetTLInfo.h"
#include "LibrarySystemSetup.h"
#include "GenTLTestTools.h"

extern uint64_t g_zCheckedGenTLVersionMajor;
extern uint64_t g_zCheckedGenTLVersionMinor;
extern std::vector<std::string> g_vecGenTLList;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Library_GetTLInfo::Library_GetTLInfo( void )
{
}

Library_GetTLInfo::~Library_GetTLInfo( void )
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Library_GetTLInfo::setUp(void)
{
}

void Library_GetTLInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////////////////

void Library_GetTLInfo::vDisplayTLInfo( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "System information of current Transport Layer");

    if (!g_bCreateTestEnumerationFlag)
    {
        std::string sTLVendorName;
        std::string sTLModelName;
        std::string sTLID;
        std::string sTLVersion;
        std::string sTLPath;
        std::string sTLType;
        LibrarySystemSetup oLibSysSetup;
        
        g_zCheckedGenTLVersionMajor = zGetGenICamVersionMajor(oLibSysSetup);
        g_zCheckedGenTLVersionMinor = zGetGenICamVersionMinor(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: GenTLVersion = " << g_zCheckedGenTLVersionMajor << "." << g_zCheckedGenTLVersionMinor << std::endl);
        sTLVendorName = sGetTLVendorName(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: TLVendorName = " << sTLVendorName.c_str() << std::endl);
        sTLModelName = sGetTLModelName(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: TLModelName  = " << sTLModelName.c_str() << std::endl);
        sTLID = sGetTLID(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: TLID         = " << sTLID.c_str() << std::endl);
        sTLVersion = sGetTLVersion(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: TLVersion    = " << sTLVersion.c_str() << std::endl);
        sTLPath = sGetTLPath(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: TLPath       = " << sTLPath.c_str() << std::endl);
        sTLType = sGetTLType(oLibSysSetup);
        GENTLTEST_PRINT("TL-Info: TLType       = " << sTLType.c_str() << std::endl);

#define GenTL_1_5_MinorVersion 5

        GENTLTEST_CHECK_MESSAGE("Error: GenICam TL version mismatch: required version is " << GenTLMajorVersion << "." 
                << GenTL_1_5_MinorVersion << " current dll version is " << g_zCheckedGenTLVersionMajor << "." << g_zCheckedGenTLVersionMinor,
                g_zCheckedGenTLVersionMajor == GenTLMajorVersion && g_zCheckedGenTLVersionMinor == GenTL_1_5_MinorVersion);

        std::vector<std::string>::iterator xIter;
        bool bFound = false;
        std::stringstream oTLList;
        oTLList << std::endl << "TL list:" << std::endl;
        for (xIter=g_vecGenTLList.begin(); xIter!=g_vecGenTLList.end(); xIter++)
        {
            std::string pathname=*xIter;
    
            oTLList << pathname.c_str() << std::endl;
            if (pathname == sTLPath) 
            {
                bFound = true;
                break;
            }
        }

        // Suppressed finding TLPath in TLList
        // GENTLTEST_CHECK_MESSAGE("Error: GenICam TL TLPath not found in TL list" << oTLList.str().c_str(), bFound == true);

        GENTLTEST_PRINT_RESULT(test_id);
    }
    else
    {
        GENTLTEST_CHECK(true);
    }
}

int64_t Library_GetTLInfo::zGetGenICamVersionMajor(LibrarySystemSetup &oLibSysSetup)
{
    int64_t zValue=0;
    std::string sValue;
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("GenTLVersionMajor");
        
    zValue = atol(sValue.c_str());

    return zValue;
}

int64_t Library_GetTLInfo::zGetGenICamVersionMinor(LibrarySystemSetup &oLibSysSetup)
{
    int64_t zValue=0;
    std::string sValue;
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("GenTLVersionMinor");

    zValue = atol(sValue.c_str());

    return zValue;
}

std::string Library_GetTLInfo::sGetTLVendorName(LibrarySystemSetup &oLibSysSetup)
{
    std::string sValue="";
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("TLVendorName");

    return sValue.c_str();
}

std::string Library_GetTLInfo::sGetTLModelName(LibrarySystemSetup &oLibSysSetup)
{
    std::string sValue="";
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("TLModelName");

    return sValue.c_str();
}

std::string Library_GetTLInfo::sGetTLID(LibrarySystemSetup &oLibSysSetup)
{
    std::string sValue="";
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("TLID");

    return sValue.c_str();
}

std::string Library_GetTLInfo::sGetTLVersion(LibrarySystemSetup &oLibSysSetup)
{
    std::string sValue="";
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("TLVersion");

    return sValue.c_str();
}

std::string Library_GetTLInfo::sGetTLPath(LibrarySystemSetup &oLibSysSetup)
{
    std::string sValue="";
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("TLPath");

    return sValue.c_str();
}
    
std::string Library_GetTLInfo::sGetTLType(LibrarySystemSetup &oLibSysSetup)
{
    std::string sValue="";
    
    m_oParameter.bPrepareSystemXML(oLibSysSetup);
    sValue = m_oParameter.sGetXMLNodeValue("TLType");

    return sValue.c_str();
}

