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

#include "Port_GCReadPortStacked.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "Port_PreCondition.h"
#include "GenTLTestTools.h"

#include "SFNCPort.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCReadPortStacked::Port_GCReadPortStacked()
{
}

Port_GCReadPortStacked::~Port_GCReadPortStacked()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCReadPortStacked::setUp(void)
{
}

void Port_GCReadPortStacked::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCReadPortStacked
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCReadPortStacked::TestGCReadPortStackedSystemEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPortStacked all standard entries in 'TLPort' of system xml");
    LibrarySystemSetup oLibSysSetup;
    Port_PreCondition oPortPreCondition;
    std::vector<char> sURL = oPortPreCondition.sGetPortURL(oLibSysSetup.hGetTLHandle());
    
    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
    for (size_t i=0; i<sUrlList.size(); i++)
    {
        GenApi::CNodeMapRef oLocalNodemap;
        SFNCPort oLocalPort(m_ModPort, oLibSysSetup.hGetTLHandle());
        GenApi::CNodePtr cNode;
        std::string sXMLString = oPortPreCondition.sReadXMLString(oLibSysSetup.hGetTLHandle(), sUrlList[i]);
        Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "TLPort", sXMLString);
        Port_PreCondition::tStringList::iterator xIter;
        size_t iNumEntries=sNodeNameList.size();
        size_t iEntryIndex=0;
        Port_PreCondition::tStringList sFilledNodeNameList;

        std::vector<PORT_REGISTER_STACK_ENTRY> entries(iNumEntries);

        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
        {
            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "TLPort", *xIter, sXMLString, cNode);
        
            if (cNode.IsValid())
            {
                if (bFillEntry(*xIter, cNode, entries[iEntryIndex]) == true)
                {
                    iEntryIndex++;
                    sFilledNodeNameList.push_back(*xIter);
                }
            }
            else
            {
                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                //GENTLTEST_PRINT(sXMLString.c_str());
                GENTLTEST_CHECK_MESSAGE("GCReadPortstacked System XML standard entries failed, TLPort node='" << (*xIter).c_str() << "' is invalid.", false);
            }
        }

        GC_ERROR eResult = m_ModPort.eGCReadPortStacked(oLibSysSetup.hGetTLHandle(), &entries[0], &iEntryIndex);
        GENTLTEST_CHECK_MESSAGE("GCReadPortStacked url=" << sUrlList[i].c_str() << 
            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
            eResult >= GC_ERR_SUCCESS);

        GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << std::endl);
        xIter = sFilledNodeNameList.begin();
        for (size_t index1=0; index1<iEntryIndex; index1++)
        {
            std::stringstream sTemp;

            if (entries[index1].pBuffer != NULL)
                sTemp << (char*)(entries[index1].pBuffer);
            else
                sTemp << "<null>";
            GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sTemp.str().c_str() << std::endl);
            xIter++;
        }

        if (eResult < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call iNumEntries has not changed", iEntryIndex != iNumEntries);
        }
        
        vDeleteEntryBuffers(entries);
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedInterfaceEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked all standard entries in 'InterfacePort' of interface xml");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);
        Port_PreCondition oPortPreCondition;
        std::vector<char> sURL = oPortPreCondition.sGetPortURL(hIF);
    
        Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
        for (size_t i=0; i<sUrlList.size(); i++)
        {
            GenApi::CNodeMapRef oLocalNodemap;
            SFNCPort oLocalPort(m_ModPort, hIF);
            GenApi::CNodePtr cNode;
            std::string sXMLString = oPortPreCondition.sReadXMLString(hIF, sUrlList[i]);
            Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "InterfacePort", sXMLString);
            Port_PreCondition::tStringList::iterator xIter;
            size_t iNumEntries=sNodeNameList.size();
            size_t iEntryIndex=0;
            Port_PreCondition::tStringList sFilledNodeNameList;

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(iNumEntries);


            for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
            {
                oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "InterfacePort", *xIter, sXMLString, cNode);
            
                if (cNode.IsValid())
                {
                    if (bFillEntry(*xIter, cNode, entries[iEntryIndex]) == true)
                    {
                        iEntryIndex++;
                        sFilledNodeNameList.push_back(*xIter);
                    }
                }
                else
                {
                    //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                    //GENTLTEST_PRINT(sXMLString.c_str());
                    GENTLTEST_CHECK_MESSAGE("GCReadPort Interface XML standard entries failed, InterfacePort node='" << (*xIter).c_str() << "' is invalid.", false);
                }
            }

            GC_ERROR eResult = m_ModPort.eGCReadPortStacked(hIF, &entries[0], &iEntryIndex);
            GENTLTEST_CHECK_MESSAGE("GCReadPortStacked url=" << sUrlList[i].c_str() << 
                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS);
            
            GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << " read entries=" << iEntryIndex << std::endl);
            xIter = sFilledNodeNameList.begin();
            for (size_t index2=0; index2<iEntryIndex; index2++)
            {
                std::stringstream sTemp;

                if (entries[index2].pBuffer != NULL)
                    sTemp << (char*)(entries[index2].pBuffer);
                else
                    sTemp << "<null>";
                GENTLTEST_PRINT("Info: " << (*xIter).c_str() << " = " << sTemp.str().c_str() << std::endl);
                xIter++;
            }

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iNumEntries has not changed", iEntryIndex != iNumEntries);
            }

            vDeleteEntryBuffers(entries);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedDeviceEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPortStacked all standard entries in 'DevicePort' of device xml");
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
                    std::vector<char> sURL = oPortPreCondition.sGetPortURL(hDev);
            
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        GenApi::CNodeMapRef oLocalNodemap;
                        SFNCPort oLocalPort(m_ModPort, hDev);
                        GenApi::CNodePtr cNode;
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hDev, sUrlList[i]);
                        Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "DevicePort", sXMLString);
                        Port_PreCondition::tStringList::iterator xIter;
                        size_t iNumEntries=sNodeNameList.size();
                        size_t iEntryIndex=0;
                        Port_PreCondition::tStringList sFilledNodeNameList;
    
                        std::vector<PORT_REGISTER_STACK_ENTRY> entries(iNumEntries);

                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "DevicePort", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                if (bFillEntry(*xIter, cNode, entries[iEntryIndex]) == true)
                                {
                                    iEntryIndex++;
                                    sFilledNodeNameList.push_back(*xIter);
                                }
                            }
                            else
                            {
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPortStacked device XML standard entries failed, DevicePort " << (*xIter).c_str() << " is invalid.", false);
                            }
                        }

                        GC_ERROR eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &iEntryIndex);
                        GENTLTEST_CHECK_MESSAGE("GCReadPortStacked (device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << ") url=" << sUrlList[i].c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult >= GC_ERR_SUCCESS);
                        
                        GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << " read entries=" << iEntryIndex << std::endl);
                        xIter = sFilledNodeNameList.begin();
                        for (size_t index3=0; index3<iEntryIndex; index3++)
                        {
                            std::stringstream sTemp;

                            if (entries[index3].pBuffer != NULL)
                                sTemp << (char*)(entries[index3].pBuffer);
                            else
                                sTemp << "<null>";
                            GENTLTEST_PRINT("Info: (device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << ") " << (*xIter).c_str() << " = " << sTemp.str().c_str() << std::endl);
                            xIter++;
                        }

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iNumEntries has not changed", iEntryIndex != iNumEntries);
                        }

                        vDeleteEntryBuffers(entries);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}
/*
void Port_GCReadPortStacked::TestGCReadPortStackedRemoteDeviceEntry( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCReadPortStacked all standard entries in 'Device' of remote device xml");
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

                    std::vector<char> sURL = oPortPreCondition.sGetPortURL(hPort);
        
                    Port_PreCondition::tStringList sUrlList=oPortPreCondition.vParseURLsToList(sURL);
                    for (size_t i=0; i<sUrlList.size(); i++)
                    {
                        GenApi::CNodeMapRef oLocalNodemap;
                        SFNCPort oLocalPort(m_ModPort, hPort);
                        GenApi::CNodePtr cNode;
                        std::string sXMLString = oPortPreCondition.sReadXMLString(hPort, sUrlList[i]);
                        Port_PreCondition::tStringList sNodeNameList=oPortPreCondition.xReadStandardsFromXMLString(oLocalNodemap, oLocalPort, "Device", sXMLString);
                        Port_PreCondition::tStringList::iterator xIter;
                        size_t iNumEntries=sNodeNameList.size();
                        size_t iEntryIndex=0;
                        Port_PreCondition::tStringList sFilledNodeNameList;
    
                        std::vector<PORT_REGISTER_STACK_ENTRY> entries(iNumEntries);

                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                if (bFillEntry(*xIter, cNode, entries[iEntryIndex]) == true)
                                {
                                    iEntryIndex++;
                                    sFilledNodeNameList.push_back(*xIter);
                                }
                            }
                            else
                            {
                                GENTLTEST_PRINT("Device " << (*xIter).c_str() << " is invalid." << std::endl);
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML standard entries failed", false);
                            }
                        }

                        GC_ERROR eResult = m_ModPort.eGCReadPortStacked(hPort, &entries[0], &iEntryIndex);
                        GENTLTEST_CHECK_MESSAGE("GCReadPortStacked (device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << ") url=" << sUrlList[i].c_str() << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult >= GC_ERR_SUCCESS);
                        
                        GENTLTEST_PRINT("Info: url=" << sUrlList[i].c_str() << " read entries=" << iEntryIndex << std::endl);
                        xIter = sFilledNodeNameList.begin();
                        for (size_t index3=0; index3<iEntryIndex; index3++)
                        {
                            std::stringstream sTemp;

                            if (entries[index3].pBuffer != NULL)
                                sTemp << (char*)(entries[index3].pBuffer);
                            else
                                sTemp << "<null>";
                            GENTLTEST_PRINT("Info: (device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << ") " << (*xIter).c_str() << " = " << sTemp.str().c_str() << std::endl);
                            xIter++;
                        }

                        vDeleteEntryBuffers(entries);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}
*/
void Port_GCReadPortStacked::TestGCReadPortStackedWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked device xml with closed library before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        // SVW if eResult < GC_ERR_SUCCESS no address is found 
        // this depends on TL implementation
        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value, the device exists.
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oLibSysSetup.tearDownLibrary();

            size_t uiNumEntries=1;
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(uiNumEntries);
            entries[0].Address = zNodeAddress;
            entries[0].pBuffer = &cBuffer[0];
            entries[0].Size = zNodeSize;

            eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_MESSAGE("GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_NOT_INITIALIZED);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call uiNumEntries != 0", 0 == uiNumEntries);
            }
        
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked device xml with closed system before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        // SVW if eResult < GC_ERR_SUCCESS no address is found 
        // this depends on TL implementation
        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value, the device exists.
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oLibSysSetup.tearDownSystem();

            size_t uiNumEntries=1;
            std::vector<char> cBuffer(zNodeSize + 1000); // ensure that a 0 termination is available

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(uiNumEntries);
            entries[0].Address = zNodeAddress;
            entries[0].pBuffer = &cBuffer[0];
            entries[0].Size = zNodeSize;

            eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_RESULT("Note: GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call uiNumEntries != 0", 0 == uiNumEntries);
            }
            
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked device xml with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        // SVW if eResult < GC_ERR_SUCCESS no address is found 
        // this depends on TL implementation
        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value, the device exists.
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oIFPreCondition.vClose();
            
            size_t uiNumEntries=1;
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(uiNumEntries);
            entries[0].Address = zNodeAddress;
            entries[0].pBuffer = &cBuffer[0];
            entries[0].Size = zNodeSize;

            eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_RESULT("Note: GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call uiNumEntries != 0", 0 == uiNumEntries);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked device xml with closed device before");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        int64_t zNodeAddress;
        size_t zNodeSize;
        DEVICE_ACCESS_FLAGS_LIST eNodeAccess;
        Port_PreCondition oPortPreCondition;
        GC_ERROR eResult=oPortPreCondition.eGetAddressOfFirstRegisterNodeOfDeviceXML(oIFPreCondition, hIF, eNodeAccess, zNodeAddress, zNodeSize);

        // SVW if eResult < GC_ERR_SUCCESS no address is found 
        // this depends on TL implementation
        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value, the device exists.
            Device_PreCondition oDevPreCondition(hIF, oIFPreCondition.sGetDeviceID(hIF, 0), eNodeAccess, &hDev);

            oDevPreCondition.vClose();
            
            size_t uiNumEntries=1;
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(uiNumEntries);
            entries[0].Address = zNodeAddress;
            entries[0].pBuffer = &cBuffer[0];
            entries[0].Size = zNodeSize;

            eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_RESULT("Note: GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call uiNumEntries != 0", 0 == uiNumEntries);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedWithNumentriesNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked device xml with parameter piNumEntries = NULL");
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

        // SVW if eResult < GC_ERR_SUCCESS no address is found 
        // this depends on TL implementation
        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value, the device exists.
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);
            
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eNodeAccess, &hDev);

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
            {
                std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available

                std::vector<PORT_REGISTER_STACK_ENTRY> entries(zNodeSize);
                entries[0].Address = zNodeAddress;
                entries[0].pBuffer = &cBuffer[0];
                entries[0].Size = zNodeSize;

                eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], NULL);
                GENTLTEST_CHECK_RESULT("Note: GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                    eResult == GC_ERR_INVALID_PARAMETER,
                    "GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                    eResult < GC_ERR_SUCCESS);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCReadPortStacked::TestGCReadPortStackedWithEntriesNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCReadPortStacked device xml with parameter pEntries = NULL");
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

        // SVW if eResult < GC_ERR_SUCCESS no address is found 
        // this depends on TL implementation
        if (eResult >= GC_ERR_SUCCESS)
        {
            // SVW if there is an address to obtain a value, the device exists.
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, 0);

            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            Device_PreCondition oDevPreCondition(hIF, sDeviceID, eNodeAccess, &hDev);

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
            {
                GC_ERROR eResult=GC_ERR_SUCCESS;
                size_t uiNumEntries=1;
            
                eResult = m_ModPort.eGCReadPortStacked(hDev, NULL, &uiNumEntries);
                GENTLTEST_CHECK_RESULT("Note: GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                    eResult == GC_ERR_INVALID_PARAMETER,
                    "GCReadPortStacked with 1 entry and address=0x" << std::hex << zNodeAddress << std::dec <<
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                    eResult < GC_ERR_SUCCESS);

                if (eResult < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call uiNumEntries != 0", 0 == uiNumEntries);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// tools
/////////////////////////////////////////////////////////////////////

bool Port_GCReadPortStacked::bFillEntry(std::string &sNodeName, GenApi::CNodePtr &cNode, PORT_REGISTER_STACK_ENTRY &oEntry)
{
    bool bResult = false;
    GenApi::CRegisterPtr regValue=cNode;
    int64_t xAddr=0;
    int64_t xSize=0;
    
    if (regValue != NULL && 
        (cNode->GetAccessMode() == GenApi::RO || cNode->GetAccessMode() == GenApi::RW))
    {
        try
        {
            xAddr = regValue->GetAddress();
            bResult = true;
        }
        catch (GenICam::AccessException ex)
        {
            GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
        }

        try
        {
            xSize = regValue->GetLength();
        }
        catch (GenICam::AccessException ex)
        {
            GENTLTEST_CHECK_MESSAGE("Error: access problem node='" << sNodeName.c_str() << "', details=" << ex.GetDescription() << std::endl, false);
        }


        oEntry.Address = xAddr;
        oEntry.Size = static_cast<size_t>(xSize);
        oEntry.pBuffer = new char[oEntry.Size];
    }

    return bResult;
}

void Port_GCReadPortStacked::vDeleteEntryBuffers(std::vector<PORT_REGISTER_STACK_ENTRY> &oEntries)
{
    for (size_t i=0; i<oEntries.size(); i++)
    {
        delete [] (char*)(oEntries[i].pBuffer);
        oEntries[i].pBuffer = NULL;
    }
}
