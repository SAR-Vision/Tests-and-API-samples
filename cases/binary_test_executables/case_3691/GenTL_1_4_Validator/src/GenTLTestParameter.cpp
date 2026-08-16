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

#include "GenTLTestParameter.h"

#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "LocalParser.h"
#include "FileParser.h"
#include "URLParser.h"
#include "GenTLZip.h"

#include "GenTLTestTools.h"

#include "SFNCPort.h"

ocGenTLTestParameter::ocGenTLTestParameter()
: m_hPort(NULL)
, m_poLocalNodemap(NULL)
, m_sCurrentNodeMapXMLString("")
{
    m_poLocalNodemap = new GenApi::CNodeMapRef();  
}

ocGenTLTestParameter::~ocGenTLTestParameter(void)
{
    if (NULL != m_poLocalNodemap)
        delete m_poLocalNodemap;
}

std::string ocGenTLTestParameter::sGetXMLNodeValue(std::string sNodeName, bool bShowResultMessage)
{
    std::string sValue;
    GenApi::CNodePtr cNode;
    
    if (m_hPort == NULL)
        return "";

    vReadNodeFromXMLString(m_sPortName.c_str(), sNodeName.c_str(), m_sXMLString, cNode);
    
    if (cNode.IsValid())
    {
        sValue = sGetNodeValue(sNodeName, cNode);
    }
    else
    {
        if (bShowResultMessage)
            GENTLTEST_PRINT("Hint: XML node string: portname=" << m_sPortName.c_str() << ", nodename=" << sNodeName.c_str() << " is invalid." << std::endl);
    }

    return sValue.c_str();
}

GenApi::CCommandPtr ocGenTLTestParameter::xGetXMLNodeCommand(std::string sCommandName, bool bShowResultMessage)
{
    GenApi::CNodePtr cNode;
    GenApi::CCommandPtr xResult;
    
    if (m_hPort == NULL)
        return xResult;

    vReadNodeFromXMLString(m_sPortName.c_str(), sCommandName.c_str(), m_sXMLString, cNode);
    
    if (cNode.IsValid() && cNode->GetPrincipalInterfaceType() == GenApi::intfICommand)
    {
        xResult = cNode;
    }
    else
    {
        if (bShowResultMessage)
            GENTLTEST_PRINT("Error: XML node command: portname=" << m_sPortName.c_str() << ", nodename=" << sCommandName.c_str() << " is invalid." << std::endl);
        // only used for "AcquisitionStart" and "AcquisitionStop" so check is neccessary
        GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML command node failed", false);
    }

    return xResult;
}

GenApi::CIntegerPtr ocGenTLTestParameter::xGetXMLNodeInteger(std::string sIntegerName, bool bShowResultMessage)
{
    GenApi::CNodePtr cNode;
    GenApi::CIntegerPtr xResult;
    
    if (m_hPort == NULL)
        return xResult;

    vReadNodeFromXMLString(m_sPortName.c_str(), sIntegerName.c_str(), m_sXMLString, cNode);
    
    if (cNode.IsValid() && cNode->GetPrincipalInterfaceType() == GenApi::intfIInteger)
    {
        xResult = cNode;
    }
    else
    {
        if (bShowResultMessage)
            GENTLTEST_PRINT("Hint: XML node integer: portname=" << m_sPortName.c_str() << ", nodename=" << sIntegerName.c_str() << " is invalid." << std::endl);
    }

    return xResult;
}

GenApi::CBooleanPtr ocGenTLTestParameter::xGetXMLNodeBoolean(std::string sBooleanName, bool bShowResultMessage)
{
    GenApi::CNodePtr cNode;
    GenApi::CBooleanPtr xResult;
    
    if (m_hPort == NULL)
        return xResult;

    vReadNodeFromXMLString(m_sPortName.c_str(), sBooleanName.c_str(), m_sXMLString, cNode);
    
    if (cNode.IsValid() && cNode->GetPrincipalInterfaceType() == GenApi::intfIBoolean)
    {
        xResult = cNode;
    }
    else
    {
        if (bShowResultMessage)
            GENTLTEST_PRINT("Hint: XML node boolean: portname=" << m_sPortName.c_str() << ", nodename=" << sBooleanName.c_str() << " is invalid." << std::endl);
    }

    return xResult;
}

uint32_t ocGenTLTestParameter::zSetXMLNodeValue(std::string sNodeName, std::string value)
{
    uint32_t zResult=-1;
    GenApi::CNodePtr cNode;
    
    if (m_hPort == NULL)
        return zResult;

    vReadNodeFromXMLString(m_sPortName.c_str(), sNodeName.c_str(), m_sXMLString, cNode);
    
    if (cNode.IsValid())
    {
        zSetNodeValue(sNodeName, cNode, value);
        zResult = ERROR_SUCCESS;
    }
    else
    {
        GENTLTEST_PRINT("Error: " << m_sPortName.c_str() << ", " << sNodeName.c_str() << " is invalid." << std::endl);
        //GENTLTEST_PRINT("Please check XML file:" << std::endl);
        //GENTLTEST_PRINT(m_sXMLString.c_str());
        GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML standard entries failed", false);
    }

    return zResult;
}

uint32_t ocGenTLTestParameter::zSetXMLNodeSizeT(std::string sNodeName, size_t value)
{
    uint32_t zResult=-1;
    GenApi::CNodePtr cNode;
    std::stringstream oTemp;

    oTemp << value;
    
    if (m_hPort == NULL)
        return zResult;

    vReadNodeFromXMLString(m_sPortName.c_str(), sNodeName.c_str(), m_sXMLString, cNode);
    
    if (cNode.IsValid())
    {
        zSetNodeValue(sNodeName, cNode, oTemp.str());
        zResult = ERROR_SUCCESS;
    }
    else
    {
        GENTLTEST_PRINT("Error: " << m_sPortName.c_str() << ", " << sNodeName.c_str() << " is invalid." << std::endl);
        //GENTLTEST_PRINT("Please check XML file:" << std::endl);
        //GENTLTEST_PRINT(m_sXMLString.c_str());
        GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML standard entries failed", false);
    }

    return zResult;
}

bool ocGenTLTestParameter::bPrepareSystemXML(LibrarySystemSetup &oLibSysSetup)
{
    vResetParameter();

    m_sPortName = "TLPort";
    
    return bPrepareXML(oLibSysSetup, oLibSysSetup.hGetTLHandle());
}

bool ocGenTLTestParameter::bPrepareInterfaceXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::IF_HANDLE hIF)
{
    vResetParameter();

    m_sPortName = "InterfacePort";

    return bPrepareXML(oLibSysSetup, hIF);
}

bool ocGenTLTestParameter::bPrepareDeviceXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::DEV_HANDLE hDev)
{
    vResetParameter();

    m_sPortName = "DevicePort";

    return bPrepareXML(oLibSysSetup, hDev);
}

bool ocGenTLTestParameter::bPrepareRemoteDeviceXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::PORT_HANDLE hPort)
{
    vResetParameter();

    m_sPortName = "Device";
    
    return bPrepareXML(oLibSysSetup, hPort);
}

bool ocGenTLTestParameter::bPrepareDataStreamXML(LibrarySystemSetup &oLibSysSetup, GenICam::Client::DS_HANDLE hDataStream)
{
    vResetParameter();

    m_sPortName = "StreamPort";
    
    return bPrepareXML(oLibSysSetup, hDataStream);
}

bool ocGenTLTestParameter::bPrepareXML(LibrarySystemSetup &oLibSysSetup, void *hPort)
{
    m_hPort = hPort;

    if (hPort != NULL)
    {
        std::vector<char> sURL = sGetPortURL(hPort);
        tStringList sUrlList=vParseURLsToList(sURL);
        
        if (sUrlList.size() > 0)
            m_sXMLString = sReadXMLString(hPort, sUrlList[0]);
    }
    else
    {
        return false;
    }

    m_oLocalPort.vSetSFNCPort(m_ModPort, m_hPort);

    return true;
}

std::vector<char> ocGenTLTestParameter::sGetPortURL(void* hHandle)
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;
    size_t iSize=0;
    std::vector<char> sURL;

    eResult=m_ModPort.eGCGetPortURL(hHandle, NULL, &iSize);
    GENTLTEST_CHECK(eResult >= GenICam::Client::GC_ERR_SUCCESS);

    if (eResult >= GenICam::Client::GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK(iSize > 0);

        sURL.resize(iSize);
        eResult=m_ModPort.eGCGetPortURL(hHandle, &sURL[0], &iSize);
        GENTLTEST_CHECK(eResult >= GenICam::Client::GC_ERR_SUCCESS);
        GENTLTEST_CHECK(iSize >= 2);
        GENTLTEST_CHECK(sURL[sURL.size()-1] == 0);
        GENTLTEST_CHECK(sURL[sURL.size()-2] == 0);
    }

    return sURL;
}

ocGenTLTestParameter::tStringList ocGenTLTestParameter::vParseURLsToList(std::vector<char> cURL)
{
    // get URL list
    tStringList urls;

    if (cURL.size() > 0)
    {
        for(size_t i = 0; (i < (cURL.size()-1)) && (cURL[i] != '\0'); i+=(strlen(&cURL[i])+1))
        {
            if (strlen(&cURL[i]) > 0)
                urls.push_back(string(&cURL[i]));
        }
    }
    GENTLTEST_CHECK_MESSAGE("No URL found", urls.size() > 0);
    
    return urls;
}

std::string ocGenTLTestParameter::sReadXMLString(void *hHandle, std::string &sUrl)
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;
    std::string sXMLString;
    std::string sLocation=vParseURL4Scheme(sUrl);
    std::string sExtension=vParseURL4Extension(sUrl);

    if(_stricmp(sLocation.c_str(), "Local") == 0)
    {
        size_t uiBufferSize=0;
        GenICam::Registry::Impl::CLocalParser oLocalParser;
    
        oLocalParser.Parse(sUrl);
        uiBufferSize = static_cast<size_t>(oLocalParser.GetLength());
        
        std::vector<char> cBuffer(uiBufferSize + 10); // ensure that a 0 termination is available
        eResult = m_ModPort.eGCReadPort(hHandle, oLocalParser.GetAddress(), &cBuffer[0], &uiBufferSize);
        GENTLTEST_CHECK_MESSAGE("GCReadPort failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
            eResult >= GenICam::Client::GC_ERR_SUCCESS);
    
        sXMLString.append(&cBuffer[0], uiBufferSize);
    }
    if(_stricmp(sLocation.c_str(), "File") == 0)
    {
        GenICam::Registry::Impl::CFileParser oFileParser;

        oFileParser.Parse(sUrl);

        sXMLString = oFileParser.GetXMLString();
    }
    if (_stricmp(sExtension.c_str(), "zip") == 0)
    {
        GenTLZip::UncompressGenICamXML(sXMLString);
    }

    return sXMLString.c_str();
}

std::string ocGenTLTestParameter::vParseURL4Scheme(std::string sURL)
{
    GenICam::Registry::Impl::CURLParser oParser;
    oParser.Parse(sURL);

    return oParser.GetScheme().c_str();
}

std::string ocGenTLTestParameter::vParseURL4Extension(std::string sURL)
{
    GenICam::Registry::Impl::CURLParser oParser;
    oParser.Parse(sURL);

    return oParser.GetExtension().c_str();
}

void ocGenTLTestParameter::vReadNodeFromXMLString(std::string sPortName, 
                                              std::string sNodeName, 
                                              std::string &sXMLString, 
                                              GenApi::CNodePtr &cNode)
{
    // GenApi::CNodeMapRef is not able to replace once existing nodemaps with a new one
    // we need to renew the object in order to be able to load a new nodemap from XML.
    // So we keep the current XML string in mind and renew the object if the string changes
    if (m_sCurrentNodeMapXMLString != sXMLString)
    {
        delete m_poLocalNodemap;
        m_poLocalNodemap = new GenApi::CNodeMapRef();

        try
        {
            m_poLocalNodemap->_LoadXMLFromString(sXMLString.c_str());
            m_sCurrentNodeMapXMLString = sXMLString;
        }
        catch(GenICam::RuntimeException ex)
        {
            std::string errorString=ex.GetDescription();

            if ("DLL already loaded" != errorString)
            {
                GENTLTEST_PRINT("Info: GenAPI _LoadXMLFromString returned: " << errorString.c_str() << std::endl);

                // reset the workaround for GenApi::CNodeMapRef
                m_sCurrentNodeMapXMLString = "";
                return;
            }
        }
    }

    bool connectResult=m_poLocalNodemap->_Connect(&m_oLocalPort, sPortName.c_str());
    GENTLTEST_CHECK_MESSAGE("Nodemap with name=" << sPortName.c_str() << " is invalid.", connectResult == true)

    cNode = m_poLocalNodemap->_GetNode(sNodeName.c_str());
}

std::string ocGenTLTestParameter::sGetNodeValue(std::string &sNodeName, GenApi::CNodePtr &cNode)
{
    std::string sValue;

    switch(cNode->GetPrincipalInterfaceType())
    {
    case GenApi::intfIFloat:
        {
            GenApi::CFloatPtr cValue=cNode;
            
            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIBoolean:
        {
            GenApi::CBooleanPtr cValue=cNode;
            
            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfICommand:
        {
            GenApi::CCommandPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIString:
        {
            GenApi::CStringPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIInteger:
        {
            GenApi::CIntegerPtr cValue=cNode;
            
            try
            {
                sValue = cValue->ToString();
                if (cValue->GetRepresentation() == GenApi::HexNumber)
                    sValue = csConvertINTHexToDec(sValue);
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIEnumeration:
        {
            GenApi::CEnumerationPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIRegister:
        {
            GenApi::CRegisterPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfICategory:
        {
            GenApi::CCategoryPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIEnumEntry:
        {
            GenApi::CEnumEntryPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIPort:
        {
            GenApi::CPortPtr ciValue=cNode;
        }
        break;
    default:
        {
            GENTLTEST_CHECK_MESSAGE(sNodeName.c_str() << " not implemented enum " << cNode->GetPrincipalInterfaceType() << std::endl, false);
        }
        break;
    }

    return sValue.c_str();
}

uint32_t ocGenTLTestParameter::zSetNodeValue(std::string &sNodeName, GenApi::CNodePtr &cNode, std::string value)
{
    uint32_t zResult=-1;

    switch(cNode->GetPrincipalInterfaceType())
    {
    case GenApi::intfIFloat:
        {
            GenApi::CFloatPtr cValue=cNode;
            double dValue=atof(value.c_str());
            
            try
            {
                cValue->SetValue(dValue);
                zResult = ERROR_SUCCESS;
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIBoolean:
        {
            GenApi::CBooleanPtr cValue=cNode;
            bool bValue=(value == "true" || value == "1" || value == "yes")? true:false;
            
            try
            {
                cValue->SetValue(bValue);
                zResult = ERROR_SUCCESS;
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIString:
        {
            GenApi::CStringPtr cValue=cNode;

            try
            {
                cValue->SetValue(value.c_str());
                zResult = ERROR_SUCCESS;
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIInteger:
        {
            GenApi::CIntegerPtr cValue=cNode;
            int64_t zValue=_atoi64(value.c_str());
            
            try
            {
                cValue->SetValue(zValue);
                zResult = ERROR_SUCCESS;
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    case GenApi::intfIEnumeration:
        {
            GenApi::CEnumerationPtr cValue=cNode;
            int64_t zValue=_atoi64(value.c_str());

            try
            {
                cValue->SetIntValue(zValue);
                zResult = ERROR_SUCCESS;
            }
            catch(GenICam::AccessException ex)
            {
                GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
            }
        }
        break;
    default:
        {
            GENTLTEST_CHECK_MESSAGE(sNodeName.c_str() << " not implemented enum " << cNode->GetPrincipalInterfaceType() << std::endl, false);
        }
        break;
    }

    return zResult;
}

void ocGenTLTestParameter::vResetParameter()
{
    m_sXMLString = "";
    m_sPortName = "";
    m_hPort = NULL;
}

