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

#include "Port_GCReadPort.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "Port_PreCondition.h"
#include "GenTLTestTools.h"

#include "SFNCPort.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_bTestDatastream;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCReadPort::Port_GCReadPort()
{
}

Port_GCReadPort::~Port_GCReadPort()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCReadPort::setUp(void)
{
}

void Port_GCReadPort::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCReadPortSystem
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCReadPort::TestGCReadPortSystem( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort system xml");
    LibrarySystemSetup oLibSysSetup;
    Port_PreCondition oPortPreCondition;
    std::vector<char> sURL = sGetPortURL(oLibSysSetup.hGetTLHandle());
    
    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
    for (size_t i=0; i<sUrlList.size(); i++)
    {
        std::string sXMLString = oPortPreCondition.sReadXMLString(oLibSysSetup.hGetTLHandle(), sUrlList[i]);
        size_t pos=sXMLString.find("RegisterDescription", 0);
        GENTLTEST_CHECK_MESSAGE("GCReadPort System XML failed, TAG 'RegisterDescription' not found.", 
            pos != std::string::npos);

        pos = sXMLString.find("/RegisterDescription", pos);
        GENTLTEST_CHECK_MESSAGE("GCReadPort System XML failed, TAG '/RegisterDescription' not found.", 
            pos != std::string::npos);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortSystemEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all standard entries in 'TLPort' of system xml");
    LibrarySystemSetup oLibSysSetup;
    Port_PreCondition oPortPreCondition;
    std::vector<char> sURL = sGetPortURL(oLibSysSetup.hGetTLHandle());
    
    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
    for (size_t i=0; i<sUrlList.size(); i++)
    {
        GenApi::CNodeMapRef oLocalNodemap;
        SFNCPort oLocalPort(m_ModPort, oLibSysSetup.hGetTLHandle());
        GenApi::CNodePtr cNode;
        std::string sXMLString = oPortPreCondition.sReadXMLString(oLibSysSetup.hGetTLHandle(), sUrlList[i]);
        Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "TLPort", sXMLString);
        Port_PreCondition::tStringList::iterator xIter;

        GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << std::endl);

        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
        {
            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "TLPort", *xIter, sXMLString, cNode);
        
            if (cNode.IsValid())
            {
                std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sValue.c_str() << std::endl);
            }
            else
            {
                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                //GENTLTEST_PRINT(sXMLString.c_str());
                GENTLTEST_CHECK_MESSAGE("GCReadPort System XML standard entry failed, TLPort node='" << (*xIter).c_str() << "' is invalid.", false);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortSystemMandatoryEntries( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all mandatory entries in 'TLPort' of system xml");
    LibrarySystemSetup oLibSysSetup;
    Port_PreCondition oPortPreCondition;
    std::vector<char> sURL = sGetPortURL(oLibSysSetup.hGetTLHandle());
    
    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
    for (size_t i=0; i<sUrlList.size(); i++)
    {
        GenApi::CNodeMapRef oLocalNodemap;
        SFNCPort oLocalPort(m_ModPort, oLibSysSetup.hGetTLHandle());
        GenApi::CNodePtr cNode;
        std::string sXMLString = oPortPreCondition.sReadXMLString(oLibSysSetup.hGetTLHandle(), sUrlList[i]);
        Port_PreCondition::tStringList sNodeNameList;
        Port_PreCondition::tStringList::iterator xIter;

        sNodeNameList.push_back("TLVendorName");
        sNodeNameList.push_back("TLModelName");
        sNodeNameList.push_back("TLID");
        sNodeNameList.push_back("TLVersion");
        sNodeNameList.push_back("TLPath");
        sNodeNameList.push_back("TLType");
        sNodeNameList.push_back("GenTLVersionMinor");
        sNodeNameList.push_back("GenTLVersionMajor");
        //sNodeNameList.push_back("InterfaceUpdateList"); // mandatory entry but not necessarily readable, GenTL doc: (R)/W
        sNodeNameList.push_back("InterfaceSelector");
        sNodeNameList.push_back("InterfaceID");
        
        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
        {
            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "TLPort", *xIter, sXMLString, cNode);
        
            if (cNode.IsValid())
            {
                std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                GENTLTEST_CHECK_MESSAGE("GCReadPort system XML mandatory entry failed, node='" << (*xIter).c_str() << "', value="".", sValue != "");
            }
            else
            {
                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                //GENTLTEST_PRINT(sXMLString.c_str());
                GENTLTEST_CHECK_MESSAGE("GCReadPort System XML mandatory entry TLPort node='" << (*xIter).c_str() << "' invalid.", false);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortInterface( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort interface xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        std::vector<char> sURL = sGetPortURL(hIF);
        Port_PreCondition oPortPreCondition;
    
        Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
        for (size_t i=0; i<sUrlList.size(); i++)
        {
            std::string sXMLString = oPortPreCondition.sReadXMLString(hIF, sUrlList[i]);
            size_t pos=sXMLString.find("RegisterDescription", 0);
            GENTLTEST_CHECK_MESSAGE("GCReadPort Interface XML failed, TAG 'RegisterDescription' not found.", pos != -1);

            pos = sXMLString.find("/RegisterDescription", pos);
            GENTLTEST_CHECK_MESSAGE("GCReadPort Interface XML failed, TAG '/RegisterDescription' not found.", pos != -1);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortInterfaceEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort all standard entries in 'InterfacePort' of interface xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        std::vector<char> sURL = sGetPortURL(hIF);
        Port_PreCondition oPortPreCondition;
    
        Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
        for (size_t i=0; i<sUrlList.size(); i++)
        {
            GenApi::CNodeMapRef oLocalNodemap;
            SFNCPort oLocalPort(m_ModPort, hIF);
            GenApi::CNodePtr cNode;
            std::string sXMLString = oPortPreCondition.sReadXMLString(hIF, sUrlList[i]);
            Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "InterfacePort", sXMLString);
            Port_PreCondition::tStringList::iterator xIter;

            GENTLTEST_PRINT("Info: standard entries of interface='" << xInterfaceList[index1].c_str() << "'" << std::endl);
            GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << std::endl);

            for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
            {
                oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "InterfacePort", *xIter, sXMLString, cNode);
            
                if (cNode.IsValid())
                {
                    std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                    GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sValue.c_str() << std::endl);
                }
                else
                {
                    //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                    //GENTLTEST_PRINT(sXMLString.c_str());
                    GENTLTEST_CHECK_MESSAGE("GCReadPort Interface XML standard entry failed, InterfacePort node='" << (*xIter).c_str() << "' is invalid.", false);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortInterfaceMandatoryEntries( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort all mandatory entries in 'InterfacePort' of interface xml (see section 7.1.2 preconditions)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        if (oIFPreCondition.zGetIFNumberOfDevices() > 0)
        {
            std::vector<char> sURL = sGetPortURL(hIF);
            Port_PreCondition oPortPreCondition;
    
            Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
            for (size_t i=0; i<sUrlList.size(); i++)
            {
                GenApi::CNodeMapRef oLocalNodemap;
                SFNCPort oLocalPort(m_ModPort, hIF);
                GenApi::CNodePtr cNode;
                std::string sXMLString = oPortPreCondition.sReadXMLString(hIF, sUrlList[i]);
                Port_PreCondition::tStringList sNodeNameList;
                Port_PreCondition::tStringList::iterator xIter;

                sNodeNameList.push_back("InterfaceID");
                sNodeNameList.push_back("InterfaceType");
                //sNodeNameList.push_back("DeviceUpdateList");  // mandatory entry but not necessarily readable, GenTL doc: (R)/W
                sNodeNameList.push_back("DeviceSelector");
                sNodeNameList.push_back("DeviceID");
                sNodeNameList.push_back("DeviceVendorName");
                sNodeNameList.push_back("DeviceModelName");
                sNodeNameList.push_back("DeviceAccessStatus");

                for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                {
                    oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "InterfacePort", *xIter, sXMLString, cNode);
            
                    if (cNode.IsValid())
                    {
                        std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);

                        GENTLTEST_CHECK_MESSAGE("GCReadPort Interface XML mandatory entry failed, node='" << (*xIter).c_str() << "', value = "".", sValue != "");
                    }
                    else
                    {
                        //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                        //GENTLTEST_PRINT(sXMLString.c_str());
                        GENTLTEST_CHECK_MESSAGE("GCReadPort Interface XML mandatory entry failed, InterfacePort node='" << (*xIter).c_str() << "' is invalid.", false);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort device xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Port_PreCondition oPortPreCondition;

                    std::vector<char> sURL = sGetPortURL(hDev);
            
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hDev, sUrlList[i]);
                        size_t pos=sXMLString.find("RegisterDescription", 0);
                        GENTLTEST_CHECK_MESSAGE("GCReadPort device XML failed, TAG 'RegisterDescription' not found.", pos != -1);

                        pos = sXMLString.find("/RegisterDescription", pos);
                        GENTLTEST_CHECK_MESSAGE("GCReadPort device XML failed, TAG '/RegisterDescription' not found.", pos != -1);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortDeviceEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all standard entries in 'DevicePort' of device xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Port_PreCondition oPortPreCondition;

                    std::vector<char> sURL = sGetPortURL(hDev);
            
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        GenApi::CNodeMapRef oLocalNodemap;
                        SFNCPort oLocalPort(m_ModPort, hDev);
                        GenApi::CNodePtr cNode;
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hDev, sUrlList[i]);

                        GENTLTEST_PRINT("Info: standard entries of device='" << sDeviceID.c_str() << "'" << std::endl);
                        GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << std::endl);

                        Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "DevicePort", sXMLString);
                        Port_PreCondition::tStringList::iterator xIter;

                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "DevicePort", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                                GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sValue.c_str() << std::endl);
                            }
                            else
                            {
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPort device XML standard entry failed, DevicePort node='" << (*xIter).c_str() << "' is invalid.", false);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortDeviceMandatoryEntries( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all mandatory entries in 'DevicePort' of device xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Port_PreCondition oPortPreCondition;

                    std::vector<char> sURL = sGetPortURL(hDev);
            
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        GenApi::CNodeMapRef oLocalNodemap;
                        SFNCPort oLocalPort(m_ModPort, hDev);
                        GenApi::CNodePtr cNode;
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hDev, sUrlList[i]);

                        Port_PreCondition::tStringList sNodeNameList;
                        Port_PreCondition::tStringList::iterator xIter;

                        sNodeNameList.push_back("DeviceID");
                        sNodeNameList.push_back("DeviceVendorName");
                        sNodeNameList.push_back("DeviceModelName");
                        sNodeNameList.push_back("DeviceType");
                        sNodeNameList.push_back("StreamSelector");
                        sNodeNameList.push_back("StreamID");
                        
                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "DevicePort", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                                GENTLTEST_CHECK_MESSAGE("GCReadPort device XML mandatory entry failed, node='" << (*xIter).c_str() << "', value = "".", sValue != "");
                            }
                            else
                            {
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPort device XML standard entry failed, DevicePort node='" << (*xIter).c_str() << "' is invalid.", false);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortRemoteDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort remote device xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Port_PreCondition oPortPreCondition;

                    hPort = oDevPreCondition.hDevGetPort();

                    std::vector<char> sURL = sGetPortURL(hPort);
            
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hPort, sUrlList[i]);
                        size_t pos=sXMLString.find("RegisterDescription", 0);
                        GENTLTEST_CHECK_MESSAGE("GCReadPort (2) remote device XML failed, TAG 'RegisterDescription' not found.", pos != -1);

                        pos = sXMLString.find("/RegisterDescription", pos);
                        GENTLTEST_CHECK_MESSAGE("GCReadPort (2) remote device XML failed, TAG '/RegisterDescription' not found.", pos != -1);
                    }
                }
            }
        }
    }
    
    GENTLTEST_PRINT_RESULT(test_id);
}
/*
void Port_GCReadPort::TestGCReadPortRemoteDeviceEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all standard entries in 'Device' of remote device xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Port_PreCondition oPortPreCondition;

                    hPort = oDevPreCondition.hDevGetPort();

                    std::vector<char> sURL = sGetPortURL(hPort);
        
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        Port_PreCondition::tStringList sNodeNameList;
                        GenApi::CNodePtr cNode;
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hPort, sUrlList[i]);
                        Port_PreCondition::tStringList::iterator xIter;

                        GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << std::endl);

                        {
                            GenApi::CNodeMapRef oLocalNodemap;
                            SFNCPort oLocalPort(m_ModPort, hPort);
                            sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "Device", sXMLString);
                        }
                        
                        GenApi::CNodeMapRef oLocalNodemap;
                        SFNCPort oLocalPort(m_ModPort, hPort);
                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                std::string sValue=oPortPreCondition.sGetNodeValueToString(oLibSysSetup, *xIter, cNode);
                                GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sValue.c_str() << std::endl);
                            }
                            else
                            {
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML standard entries failed, Device node='" << (*xIter).c_str() << "' is invalid.", false);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}
*/
void Port_GCReadPort::TestGCReadPortRemoteDeviceMandatoryEntries( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all mandatory entries in 'Device' of remote device xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    Port_PreCondition oPortPreCondition;

                    hPort = oDevPreCondition.hDevGetPort();

                    std::vector<char> sURL = sGetPortURL(hPort);
        
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        Port_PreCondition::tStringList sNodeNameList;
                        GenApi::CNodePtr cNode;
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hPort, sUrlList[i]);
                        Port_PreCondition::tStringList::iterator xIter;
                        GenApi::CNodeMapRef oLocalNodemap;
                        SFNCPort oLocalPort(m_ModPort, hPort);
                        long nDeviceSFNCVersionMajor=0;
                        long nDeviceSFNCVersionMinor=0;
                        
                        // attempt to get "DeviceSFNCVersionMajor"
                        oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", "DeviceSFNCVersionMajor", sXMLString, cNode);
                        if (cNode.IsValid())
                        {
                            std::string sDeviceSFNCVersionMajor=oPortPreCondition.sGetNodeValueToString(oLocalPort, "DeviceSFNCVersionMajor", cNode);
                            nDeviceSFNCVersionMajor = atol(sDeviceSFNCVersionMajor.c_str());
                        }

                        // attempt to get "DeviceSFNCVersionMinor"
                        oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", "DeviceSFNCVersionMinor", sXMLString, cNode);
                        if (cNode.IsValid())
                        {
                            std::string sDeviceSFNCVersionMinor=oPortPreCondition.sGetNodeValueToString(oLocalPort, "DeviceSFNCVersionMinor", cNode);
                            nDeviceSFNCVersionMinor = atol(sDeviceSFNCVersionMinor.c_str());
                        }

                        // if SFNC version less version 2.0 test it
                        // in the following GenICam SFNC versions the following features are not mandatory but recommended
                        if (nDeviceSFNCVersionMajor < 2) 
                        {
                            sNodeNameList.push_back("Width");
                            sNodeNameList.push_back("Height");
                            sNodeNameList.push_back("PixelFormat");
                            sNodeNameList.push_back("PayloadSize");
                            sNodeNameList.push_back("AcquisitionMode");
                            if (nDeviceSFNCVersionMajor == 1 && nDeviceSFNCVersionMinor >= 5)
                                sNodeNameList.push_back("TLParamsLocked");
                        }
                        else
                        {
                            sNodeNameList.push_back("TLParamsLocked");
                        }

                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                                if (*xIter == "Width" || *xIter == "Height" || *xIter == "PayloadSize")
                                {
                                    uint32_t uiValue=atol(sValue.c_str());
                                    GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML mandatory entry failed, node='" << (*xIter).c_str() << "', value = 0.", uiValue > 0);
                                }
                                if (*xIter == "PixelFormat")
                                {
                                    vTestPixelFormat(sValue);
                                }
                                if (*xIter == "AcquisitionMode")
                                {
                                    vTestAcquisitionMode(sValue);
                                }
                                if (*xIter == "TLParamsLocked")
                                {
                                    GENTLTEST_CHECK(true);
                                }
                            }
                            else
                            {
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML mandatory entry failed, Device node='" << (*xIter).c_str() << "' is invalid.", false);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortDatastream( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort datastream xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        std::vector<char> sURL = sGetPortURL(hDs);
                        Port_PreCondition oPortPreCondition;
                
                        Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                        for (size_t i=0; i<sUrlList.size(); i++)
                        {
                            std::string sXMLString = oPortPreCondition.sReadXMLString(hDs, sUrlList[i]);
                            size_t pos=sXMLString.find("RegisterDescription", 0);
                            GENTLTEST_CHECK_MESSAGE("GCReadPort device XML failed, TAG 'RegisterDescription' not found.", pos != -1);

                            pos = sXMLString.find("/RegisterDescription", pos);
                            GENTLTEST_CHECK_MESSAGE("GCReadPort device XML failed, TAG '/RegisterDescription' not found.", pos != -1);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortDatastreamEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all standard entries in 'StreamPort' of datastream xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        std::vector<char> sURL = sGetPortURL(hDs);
                        Port_PreCondition oPortPreCondition;
                    
                        Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                        for (size_t i=0; i<sUrlList.size(); i++)
                        {
                            GenApi::CNodeMapRef oLocalNodemap;
                            SFNCPort oLocalPort(m_ModPort, hDs);
                            GenApi::CNodePtr cNode;
                            std::string sXMLString = oPortPreCondition.sReadXMLString(hDs, sUrlList[i]);

                            GENTLTEST_PRINT("Info: standard entries of stream='" << sDataStreamID.c_str() << "'" << std::endl);
                            GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << std::endl);

                            Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "StreamPort", sXMLString);
                            Port_PreCondition::tStringList::iterator xIter;

                            for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                            {
                                oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "StreamPort", *xIter, sXMLString, cNode);
                            
                                if (cNode.IsValid())
                                {
                                    std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                                    GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sValue.c_str() << std::endl);
                                }
                                else
                                {
                                    //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                    //GENTLTEST_PRINT(sXMLString.c_str());
                                    GENTLTEST_CHECK_MESSAGE("GCReadPort device XML standard entry failed, StreamPort node='" << (*xIter).c_str() << "' is invalid.", false);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortDatastreamMandatoryEntries( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all mandatory entries in 'StreamPort' of datastream xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        std::vector<char> sURL = sGetPortURL(hDs);
                        Port_PreCondition oPortPreCondition;
                    
                        Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                        for (size_t i=0; i<sUrlList.size(); i++)
                        {
                            GenApi::CNodeMapRef oLocalNodemap;
                            SFNCPort oLocalPort(m_ModPort, hDs);
                            GenApi::CNodePtr cNode;
                            std::string sXMLString = oPortPreCondition.sReadXMLString(hDs, sUrlList[i]);

                            Port_PreCondition::tStringList sNodeNameList;
                            Port_PreCondition::tStringList::iterator xIter;

                            sNodeNameList.push_back("StreamID");
                            sNodeNameList.push_back("StreamAnnouncedBufferCount");
                            sNodeNameList.push_back("StreamBufferHandlingMode");
                            sNodeNameList.push_back("StreamAnnounceBufferMinimum");
                            sNodeNameList.push_back("StreamType");
                            
                            for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                            {
                                oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "StreamPort", *xIter, sXMLString, cNode);
                            
                                if (cNode.IsValid())
                                {
                                    std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                                    GENTLTEST_CHECK_MESSAGE("GCReadPort datastream XML mandatory entry failed, node='" << (*xIter).c_str() << "', value = "".", sValue != "");
                                }
                                else
                                {
                                    //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                    //GENTLTEST_PRINT(sXMLString.c_str());
                                    GENTLTEST_CHECK_MESSAGE("GCReadPort datastream XML standard entry failed, StreamPort node='" << (*xIter).c_str() << "' is invalid.", false);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortBufferMandatoryEntries( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPort all mandatory entries in 'BufferPort' if buffer supports it (see section 4.1 and 4.1.1)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    int nURLExists=0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        uint32_t uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                size_t iSize=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        GC_ERROR eResult=GC_ERR_SUCCESS;
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        Port_PreCondition oPortPreCondition;
                        std::vector<char> sURL;

                        oDSPreCondition.eDSAllocAndAnnounceBuffer(hDs, 100, (void *)this, &hBuffer);
                        
                        eResult = oPortPreCondition.eGCGetPortInfo_Buffer_PortInfoModule(hBuffer);

                        if (eResult >= GC_ERR_SUCCESS)
                        {
                            Port_PreCondition oPortPreCondition;

                            sURL = sGetPortURL(hBuffer, false);
                    
                            Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                            for (size_t i=0; i<sUrlList.size(); i++)
                            {
                                GenApi::CNodeMapRef oLocalNodemap;
                                SFNCPort oLocalPort(m_ModPort, hBuffer);
                                GenApi::CNodePtr cNode;
                                std::string sXMLString = oPortPreCondition.sReadXMLString(hBuffer, sUrlList[i]);
                                int nAccessibleCount=0;

                                nURLExists = 1;

                                Port_PreCondition::tStringList sNodeNameList;
                                Port_PreCondition::tStringList::iterator xIter;
								std::string sMissingNodeName = "";

                                sNodeNameList.push_back("BufferData");
                                sNodeNameList.push_back("BufferUserData");
                            
                                for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                                {
                                    oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "BufferPort", *xIter, sXMLString, cNode);
                            
                                    if (cNode.IsValid())
                                    {
                                        std::string sValue=oPortPreCondition.sGetNodeValueToString(oLocalPort, *xIter, cNode);
                                        if (sValue != "")
                                        {
                                            nAccessibleCount++;
                                        }
										else
										{
											sMissingNodeName = *xIter;
										}
                                    }
									else
									{
										sMissingNodeName = *xIter;
									}
                                }

                                // It's OK if provider supports both or none.
                                // but not only one of them.
                                // nAccessibleCount == 2    ==> OK, both existing 
                                // nAccessibleCount == 0    ==> OK, both not existing 
                                GENTLTEST_CHECK_MESSAGE("GCReadPort BufferPort XML standard entry failed, node='" << sMissingNodeName.c_str() << "'", nAccessibleCount == 2 || nAccessibleCount == 0);
                            }
                        }
                    }
                }
            }
        }
    }

    if (nURLExists == 0 && g_bTestDatastream)
    {
        GENTLTEST_PRINT("Info: BufferPort not supported" << std::endl);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with closed library before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        
        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);
        
        if (eResult >= GC_ERR_SUCCESS)
        {
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oLibSysSetup.tearDownLibrary();

            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_MESSAGE("GCReadPort with closed library failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_NOT_INITIALIZED);
            
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with closed system before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        
        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);
        
        if (eResult >= GC_ERR_SUCCESS)
        {
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oLibSysSetup.tearDownSystem();

            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_RESULT("Note: GCReadPort with closed system module return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPort with closed system module failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);
            
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        
        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);
        
        if (eResult >= GC_ERR_SUCCESS)
        {
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oIFPreCondition.vClose();
            
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_RESULT("Note: GCReadPort with closed interface module return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPort with closed interface module failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with closed device before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        
        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);
        
        if (eResult >= GC_ERR_SUCCESS)
        {
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oDevPreCondition.vClose();
            
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_RESULT("Note: GCReadPort with closed device return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPort with closed device failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with parameter piSize = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value the device exists.
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eNodeAccess, &hDev);
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            std::vector<char> cBufferSave(zNodeSize + 10); // ensure that a 0 termination is available
            memset(&cBuffer[0], 0, zNodeSize + 10);
            cBufferSave = cBuffer;

            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], NULL);
            GENTLTEST_CHECK_RESULT("Note: GCReadPort return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPort with parameter piSize = NULL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call cBuffer has not the original pointer", cBufferSave == cBuffer);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with parameter iSize = iSize / 2");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value the device exists.
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR eResult=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eNodeAccess, &hDev);
            size_t uiBufferSize = zNodeSize;
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            memset(&cBuffer[0], 0, zNodeSize + 10);

            uiBufferSize /= 2;

            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &uiBufferSize);
            GENTLTEST_CHECK_MESSAGE("GCReadPort failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult >= GC_ERR_SUCCESS);

            if (eResult >= GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call cBuffer[uiBufferSize] != 0", cBuffer[uiBufferSize] == 0);
                GENTLTEST_CHECK_MESSAGE("Parameter after call cBuffer[uiBufferSize] != 00", cBuffer[uiBufferSize+1] == 0);
                GENTLTEST_CHECK_MESSAGE("Parameter after call cBuffer[uiBufferSize] != 000", cBuffer[uiBufferSize+2] == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPort::TestGCReadPortWithBufferNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPort device xml with parameter pBuffer = NULL");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value the device exists.
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            GC_ERROR eResult=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eNodeAccess, &hDev);
            size_t uiBufferSizeSave=zNodeSize;

            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, NULL, &zNodeSize);
            GENTLTEST_CHECK_RESULT("Note: GCReadPort return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_BUFFER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_INVALID_BUFFER,
                "GCReadPort with parameter pBuffer = NULL failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call uiBufferSize != old uiBufferSize", uiBufferSizeSave == zNodeSize);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// tools
/////////////////////////////////////////////////////////////////////

std::vector<char> Port_GCReadPort::sGetPortURL(void* hHandle, bool bMandatory)
{
    GC_ERROR eResult=GC_ERR_SUCCESS;
    size_t iSize=0;
    std::vector<char> sURL;

    eResult=m_ModPort.eGCGetPortURL(hHandle, NULL, &iSize);
    if (bMandatory)
    {
        GENTLTEST_CHECK_MESSAGE("GCGetPortURL failed handle=0x"<<std::hex<<hHandle<<std::dec, eResult >= GC_ERR_SUCCESS);
    }

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

