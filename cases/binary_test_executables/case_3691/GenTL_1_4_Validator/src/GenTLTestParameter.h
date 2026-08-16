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

#ifndef GENTLTEST_PARAMETER_INCLUDE____
#define GENTLTEST_PARAMETER_INCLUDE____

#include <string>
#include <vector>
#include "GenApi/GenApi.h"
#include "Modules.h"
#include "LocalParser.h"
#include "SFNCPort.h"

class LibrarySystemSetup;

class ocGenTLTestParameter
{
public:
    typedef std::vector<std::string> tStringList;

    ocGenTLTestParameter();
    ~ocGenTLTestParameter(void);

    std::string sGetXMLNodeValue(std::string sNodeName, bool bShowResultMessage=true);
    GenApi::CCommandPtr xGetXMLNodeCommand(std::string sCommandName, bool bShowResultMessage=true);
    GenApi::CIntegerPtr xGetXMLNodeInteger(std::string sIntegerName, bool bShowResultMessage=true);
    GenApi::CBooleanPtr xGetXMLNodeBoolean(std::string sBooleanName, bool bShowResultMessage=true);
    uint32_t zSetXMLNodeValue(std::string sNodeName, std::string value);
    uint32_t zSetXMLNodeSizeT(std::string sNodeName, size_t value);
    bool bPrepareSystemXML(LibrarySystemSetup &oLibSysSetup);
    bool bPrepareInterfaceXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::IF_HANDLE hIF);
    bool bPrepareDeviceXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::DEV_HANDLE hDev);
    bool bPrepareRemoteDeviceXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::PORT_HANDLE hPort);
    bool bPrepareDataStreamXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::DS_HANDLE hDataStream);
    
private:
    bool bPrepareXML(LibrarySystemSetup &oLibSysSetup, void *hPort);
    std::vector<char> sGetPortURL(void* hHandle);
    tStringList vParseURLsToList(std::vector<char> cURL);
    std::string sReadXMLString(void *hHandle, std::string &sUrl);
    std::string vParseURL4Scheme(std::string sURL);
    std::string vParseURL4Extension(std::string sURL);
    void vReadNodeFromXMLString(std::string sPortName, 
                              std::string sNodeName, 
                              std::string &sXMLString, 
                              GenApi::CNodePtr &cNode);
    std::string sGetNodeValue(std::string &sNodeName, GenApi::CNodePtr &cNode);
    uint32_t zSetNodeValue(std::string &sNodeName, GenApi::CNodePtr &cNode, std::string value);
    void vResetParameter();
    
private:
    ModPORT m_ModPort;
    std::string m_sXMLString;
    std::string m_sPortName;
    void *m_hPort;
    GenApi::CNodeMapRef *m_poLocalNodemap;
    std::string m_sCurrentNodeMapXMLString;
    SFNCPort m_oLocalPort;  
};

#endif /* GENTLTEST_PARAMETER_INCLUDE____ */
