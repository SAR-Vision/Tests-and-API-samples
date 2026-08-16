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

#include <vector>
#include "GenApi/GenApi.h"

#include "Port_PreCondition.h"
#include "GenTLTestParameter.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "LocalParser.h"
#include "FileParser.h"
#include "URLParser.h"
#include "GenTLZip.h"

using namespace GenICam;
using namespace GenICam::Client;


////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_PreCondition::Port_PreCondition(void)
{
}

Port_PreCondition::~Port_PreCondition(void)
{
}

GC_ERROR Port_PreCondition::eGCGetPortInfo_Buffer_PortInfoModule(GenICam::Client::BUFFER_HANDLE hBuffer)
{
    GC_ERROR eResult=GC_ERR_SUCCESS;
    INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
    size_t iSize=0;
    std::string sMsg;
                
    eResult = m_ModPort.eGCGetPortInfo(hBuffer, PORT_INFO_MODULE, &iType, NULL, &iSize);

    if (eResult < GC_ERR_SUCCESS)
    {
        sMsg = sGetLastErrorMessage();
    }

    GENTLTEST_REQUIRE_MESSAGE("GCGetPortInfo PORT_INFO_MODULE " <<
            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(eResult).c_str() <<
            " error message='" << sMsg.c_str() << "'",  
            eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED);

    return eResult;
}

std::vector<char> Port_PreCondition::sGetPortURL(void* hHandle)
{
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t iSize=0;
    std::vector<char> sURL;

    eResult=m_ModPort.eGCGetPortURL(hHandle, NULL, &iSize);
    GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);

    if (eResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK(iSize > 0);

        sURL.resize(iSize);
        eResult=m_ModPort.eGCGetPortURL(hHandle, &sURL[0], &iSize);
        GENTLTEST_CHECK(eResult >= GC_ERR_SUCCESS);
        GENTLTEST_CHECK(iSize >= 2);
        GENTLTEST_CHECK(sURL[sURL.size()-1] == 0);
        GENTLTEST_CHECK(sURL[sURL.size()-2] == 0);
    }

    return sURL;
}

Port_PreCondition::tStringList Port_PreCondition::vParseURLsToList(std::vector<char> cURL)
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

std::string Port_PreCondition::vParseURL4Scheme(std::string sURL)
{
    GenICam::Registry::Impl::CURLParser oParser;
    oParser.Parse(sURL);

    return oParser.GetScheme().c_str();
}

std::string Port_PreCondition::vParseURL4Extension(std::string sURL)
{
    GenICam::Registry::Impl::CURLParser oParser;
    oParser.Parse(sURL);

    return oParser.GetExtension().c_str();
}

std::string Port_PreCondition::sReadXMLString(void *hHandle, std::string &sUrl)
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

// get XML string from one module represented by hHandle
void Port_PreCondition::vReadNodeFromXMLString(GenApi::CNodeMapRef &oLocalNodemap,
                                              SFNCPort &oLocalDevPort,
                                              std::string sPortName, 
                                              std::string sNodeName, 
                                              std::string &sXMLString, 
                                              GenApi::CNodePtr &cNode)
{
    try
    {
        oLocalNodemap._LoadXMLFromString(sXMLString.c_str());
    }
    catch(GenICam::RuntimeException ex)
    {
        std::string errorString=ex.GetDescription();

        if ("DLL already loaded" != errorString)
        {
            GENTLTEST_PRINT("Info: GenAPI _LoadXMLFromString returned: " << errorString.c_str() << std::endl);
            return;
        }
    }
    
    bool connectResult=oLocalNodemap._Connect(&oLocalDevPort, sPortName.c_str());
    GENTLTEST_CHECK_MESSAGE("Nodemap with name=" << sPortName.c_str() << " is invalid.", connectResult == true)

    cNode = oLocalNodemap._GetNode(sNodeName.c_str());
}

Port_PreCondition::tStringList Port_PreCondition::xReadStandardsFromXMLString(GenApi::CNodeMapRef &oLocalNodemap,
                                              SFNCPort &oLocalDevPort,
                                              std::string sPortName, 
                                              std::string &sXMLString)
{
    GenApi::NodeList_t xNodes;
    Port_PreCondition::tStringList sResultList;
    GenApi::NodeList_t::iterator xIter;

    try
    {
        oLocalNodemap._LoadXMLFromString(sXMLString.c_str());
    }
    catch(GenICam::RuntimeException ex)
    {
        GENTLTEST_PRINT("Info: GenAPI _LoadXMLFromString returned: " << ex.GetDescription() << std::endl);
        return sResultList;
    }
    
    bool connectResult=oLocalNodemap._Connect(&oLocalDevPort, sPortName.c_str());
    GENTLTEST_CHECK_MESSAGE("Nodemap with name=" << sPortName.c_str() << " is invalid.", connectResult == true)

    oLocalNodemap._GetNodes(xNodes); 

    for (xIter=xNodes.begin(); xIter!=xNodes.end(); xIter++)
    {
        GenApi::CNodePtr oNode=*xIter;
            
        if (oNode->GetNameSpace() == GenApi::Standard && 
            (oNode->GetAccessMode() == GenApi::RO || oNode->GetAccessMode() == GenApi::RW))
        {
            sResultList.push_back(oNode->GetName().c_str());
        }
    }

    return sResultList;
}

GC_ERROR Port_PreCondition::eGetAddressOfFirstRegisterNodeOfDeviceXML(Interface_PreCondition &oIFPreCondition,
                                                    IF_HANDLE hIF,
                                                    DEVICE_ACCESS_FLAGS_LIST &eNodeAccess, 
                                                    int64_t &zNodeAddress, 
                                                    size_t &zNodeSize)
{
    GC_ERROR eResult=GC_ERR_ERROR;
    uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

    tStringList sNodeNameList;
    sNodeNameList.push_back("DeviceType");
    sNodeNameList.push_back("DeviceID");
    sNodeNameList.push_back("DeviceVendorName");
    sNodeNameList.push_back("DeviceModelName");
    sNodeNameList.push_back("StreamSelector");
    sNodeNameList.push_back("StreamID");
    
    if (uiNumDevices > 0)
    {
        std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        size_t iSize=0;

        eNodeAccess=DEVICE_ACCESS_UNKNOWN;
            
        // check for readable device access
        for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
        {
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
            {
                eNodeAccess = eAccess;
                break;
            }
        }

        if (eNodeAccess >= DEVICE_ACCESS_READONLY)
        {
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eNodeAccess, &hDev);
            Port_PreCondition oPortPreCondition;
            std::vector<char> sURL = oPortPreCondition.sGetPortURL(hDev);
            
            Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
            if (sUrlList.size() > 0)
            {
                GenApi::CNodeMapRef oLocalNodemap;
                SFNCPort oLocalPort(m_ModPort, hDev);
                GenApi::CNodePtr cNode;
                std::string sXMLString = oPortPreCondition.sReadXMLString(hDev, sUrlList[0]);

                tStringList::iterator xIter;
                for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                {
                    oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "DevicePort", *xIter, sXMLString, cNode);
                        
                    if (cNode.IsValid())
                    {
                        Port_PreCondition oPortPreCondition;
                        std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                        if (sValue != "" && oLocalPort.m_LastReadAddress != 0 && oLocalPort.m_LastReadSize != 0)
                        {
                            zNodeAddress = oLocalPort.m_LastReadAddress;
                            zNodeSize = oLocalPort.m_LastReadSize;
                            eResult=GC_ERR_SUCCESS;
                            break;
                        }
                    }
                }

                if (eResult < GC_ERR_SUCCESS)
                {
                    GENTLTEST_PRINT("Info: lookup address: no register address found" << std::endl);
                }
            }
        }
        else 
        {
            GENTLTEST_PRINT("Info: lookup address: no valid device found" << std::endl);
        }
    }

    return eResult;
}

std::string Port_PreCondition::sGetNodeValueToString(SFNCPort &oLocalPort, std::string sNodeName, GenApi::CNodePtr &cNode)
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
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfIBoolean:
        {
            GenApi::CBooleanPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfICommand:
        {
            GenApi::CCommandPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfIString:
        {
            GenApi::CStringPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfIInteger:
        {
            GenApi::CIntegerPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfIEnumeration:
        {
            GenApi::CEnumerationPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfIRegister:
        {
            GenApi::CRegisterPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfICategory:
        {
            GenApi::CCategoryPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
            }
        }
        break;
    case GenApi::intfIEnumEntry:
        {
            GenApi::CEnumEntryPtr cValue=cNode;

            try
            {
                sValue = cValue->ToString();
                vCheckResult(oLocalPort, sNodeName);
            }
            catch(GenICam::AccessException ex)
            {
                vCheckException(sNodeName, ex.GetDescription());
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

    return sValue;
}

bool Port_PreCondition::bCheck4Necessity(std::string sNodeName)
{
    bool bResult=false;

    if (sNodeName == "Width" || sNodeName == "Height" || sNodeName == "PixelFormat" || sNodeName == "PayloadSize" || sNodeName == "AcquisitionMode" ||      // remotedevice
        sNodeName == "StreamType" || sNodeName == "StreamAnnounceBufferMinimum" ||                                                                          // datastream
        sNodeName == "StreamAnnouncedBufferCount" || sNodeName == "StreamID" || sNodeName == "StreamBufferHandlingMode" ||
        sNodeName == "StreamID" || sNodeName == "StreamSelector" || sNodeName == "DeviceType" || sNodeName == "DeviceModelName" ||                          // device
        sNodeName == "DeviceVendorName" || sNodeName == "DeviceID" || 
        sNodeName == "DeviceAccessStatus" || sNodeName == "DeviceModelName" || sNodeName == "DeviceVendorName" || sNodeName == "DeviceID" ||                // interface
        sNodeName == "DeviceSelector" || sNodeName == "InterfaceType" || sNodeName == "InterfaceID" || 
        sNodeName == "TLVendorName" || sNodeName == "TLModelName" || sNodeName == "TLID" || sNodeName == "TLVersion" || sNodeName == "TLPath" ||            // system
        sNodeName == "TLType" || sNodeName == "GenTLVersionMinor" || sNodeName == "GenTLVersionMajor" || 
        sNodeName == "InterfaceSelector" || sNodeName == "InterfaceID")
    {
        bResult = true;
    }

    return bResult;
}

void Port_PreCondition::vCheckException(std::string sNodeName, std::string sExceptionDescr)
{
    if (bCheck4Necessity(sNodeName))
    {
        GENTLTEST_CHECK_MESSAGE("Error: access problem mandatory node='" << sNodeName.c_str() << "', details=" << sExceptionDescr.c_str() << std::endl, false);
    }
    else
    {
        GENTLTEST_PRINT("Hint: access problem optional node='" << sNodeName.c_str() << "', details=" << sExceptionDescr.c_str() << std::endl);
    }
}

void Port_PreCondition::vCheckResult(SFNCPort &oLocalPort, std::string sNodeName)
{
    if (oLocalPort.eGetLastResult() < GC_ERR_SUCCESS)
    {
        if (bCheck4Necessity(sNodeName))
        {
            GENTLTEST_PRINT("Error: can not read mandatory node='" << sNodeName.c_str() << "', details=" << sGetLastErrorMessage().c_str() << std::endl);
        }
        else
        {
            GENTLTEST_PRINT("Hint: can not read optional node='" << sNodeName.c_str() << "', details=" << sGetLastErrorMessage().c_str() << std::endl);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////////

std::string Port_PreCondition::sGetLastErrorMessage()
{
    std::string sMsg;
    GC_ERROR eErrorCode=GC_ERR_SUCCESS;
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t zSize=0;
    
    eResult = m_ModGC.eGCGetLastError(&eErrorCode, NULL, &zSize);
    GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
        eResult >= GC_ERR_SUCCESS);

    if (eResult >= GC_ERR_SUCCESS)
    {
        GENTLTEST_CHECK_MESSAGE("Parameter after GCGetLastError call iSize == 0", zSize > 0);

        if (zSize > 0)
        {
            std::vector<char> sBuffer(zSize);
            eResult = m_ModGC.eGCGetLastError(&eErrorCode, &sBuffer[0], &zSize);
            GENTLTEST_REQUIRE_MESSAGE("GCGetLastError returned result="<<sConvertGCError2String(eResult).c_str()<<" expected result >= GC_ERR_SUCCESS", 
                eResult >= GC_ERR_SUCCESS);

            if (eResult >= GC_ERR_SUCCESS) 
            {
                sMsg = &sBuffer[0];
            }
        }
    }

    return sMsg;
}
