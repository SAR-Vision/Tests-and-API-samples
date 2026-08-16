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

#ifndef PORT_PRECONDITION_INCLUDE___
#define PORT_PRECONDITION_INCLUDE___

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

class LibrarySystemSetup;
class Interface_PreCondition;
class SFNCPort;

class Port_PreCondition
{
public:
    typedef std::vector<std::string> tStringList;

    Port_PreCondition(void);
    ~Port_PreCondition(void);
    
    GenICam::Client::GC_ERROR eGCGetPortInfo_Buffer_PortInfoModule(GenICam::Client::BUFFER_HANDLE hBuffer);
    std::vector<char> sGetPortURL(void* hHandle);
    tStringList vParseURLsToList(std::vector<char> cURL);
    std::string sReadXMLString(void *hHandle, std::string &sUrl);
    void vReadNodeFromXMLString(GenApi::CNodeMapRef &oLocalNodemap,
                              SFNCPort &oLocalDevPort,
                              std::string sPortName, 
                              std::string sNodeName, 
                              std::string &sXMLString, 
                              GenApi::CNodePtr &cNode);
    tStringList xReadStandardsFromXMLString(GenApi::CNodeMapRef &oLocalNodemap,
                              SFNCPort &oLocalDevPort,
                              std::string sPortName, 
                              std::string &sXMLString);
    GenICam::Client::GC_ERROR eGetAddressOfFirstRegisterNodeOfDeviceXML(Interface_PreCondition &oIFPreCondition, 
                                                        GenICam::Client::IF_HANDLE hIF,
                                                        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST &eNodeAccess, 
                                                        int64_t &zNodeAddress, 
                                                        size_t &zNodeSize);
    std::string sGetNodeValueToString(SFNCPort &oLocalPort, std::string sNodeName, GenApi::CNodePtr &cNode);
    
private:
    std::string vParseURL4Scheme(std::string sURL);
    std::string vParseURL4Extension(std::string sURL);

	bool bCheck4Necessity(std::string sNodeName);
    void vCheckException(std::string sNodeName, std::string sExceptionDescr);
    void vCheckResult(SFNCPort &oLocalPort, std::string sNodeName);

private:
    ModGC   m_ModGC;
    ModPORT m_ModPort;

    std::string sGetLastErrorMessage();
};

#endif /* PORT_PRECONDITION_INCLUDE___ */