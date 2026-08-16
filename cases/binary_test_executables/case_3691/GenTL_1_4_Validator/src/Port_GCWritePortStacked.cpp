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

#include "Port_GCWritePortStacked.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "Port_PreCondition.h"
#include "GenTLTestTools.h"

#include "SFNCPort.h"

#define TEST_VALUE          0

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCWritePortStacked::Port_GCWritePortStacked()
{
}

Port_GCWritePortStacked::~Port_GCWritePortStacked()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCWritePortStacked::setUp(void)
{
}

void Port_GCWritePortStacked::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCWritePortStacked
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCWritePortStacked::TestGCWritePortStacked( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCWritePortStacked with 'Height' and 'Width' of remote device xml");
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
                        Port_PreCondition::tStringList sNodeNameList;
                        Port_PreCondition::tStringList::iterator xIter;
                        size_t iNumEntries=0;
                        size_t iEntryIndex=0;
    
                        sNodeNameList.push_back("Height");
                        sNodeNameList.push_back("Width");

                        iNumEntries=sNodeNameList.size();

                        std::vector<PORT_REGISTER_STACK_ENTRY> entries(iNumEntries);

                        for (xIter=sNodeNameList.begin(); xIter!=sNodeNameList.end(); xIter++)
                        {
                            oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", *xIter, sXMLString, cNode);
                        
                            if (cNode.IsValid())
                            {
                                if (bFillEntry(oLocalPort, *xIter, cNode, entries[iEntryIndex]) == true)
                                {
                                    iEntryIndex++;
                                }
                            }
                            else
                            {
                                //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                                //GENTLTEST_PRINT(sXMLString.c_str());
                                GENTLTEST_CHECK_MESSAGE("GCReadPort remote device XML standard entries failed, Device node='" << (*xIter).c_str() << "' is invalid.", false);
                            }
                        }

                        GC_ERROR eResult = m_ModPort.eGCReadPortStacked(hPort, &entries[0], &iEntryIndex);
                        GENTLTEST_CHECK_MESSAGE("GCWritePortStacked GCReadPortStacked (device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << 
                            ") failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult >= GC_ERR_SUCCESS);

                        eResult = m_ModPort.eGCWritePortStacked(hPort, &entries[0], &iEntryIndex);
                        if (eAccess == DEVICE_ACCESS_READONLY)
                        {
                            GENTLTEST_CHECK_MESSAGE("GCWritePortStacked (device access=DEVICE_ACCESS_READONLY" << 
                                ") failed. Expected < GC_ERR_SUCCESS in read only mode, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult < GC_ERR_SUCCESS);
                        }
                        else
                        {
                            GENTLTEST_CHECK_MESSAGE("GCWritePortStacked (device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << 
                                ") failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS);
                        }

                        vDeleteEntryBuffers(entries);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCWritePortStacked::TestGCWritePortStackedWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCWritePortStacked device xml with closed library before");
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

            size_t uiNumEntries=1;
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(uiNumEntries);
            entries[0].Address = zNodeAddress;
            entries[0].pBuffer = &cBuffer[0];
            entries[0].Size = zNodeSize;

            eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_MESSAGE("GCWritePortStacked GCReadPortStacked failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult >= GC_ERR_SUCCESS);

            oLibSysSetup.tearDownLibrary();
                
            eResult = m_ModPort.eGCWritePortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_MESSAGE("GCWritePortStacked with closed library failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_NOT_INITIALIZED);

            oLibSysSetup.tearDown();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCWritePortStacked::TestGCWritePortStackedWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCWritePortStacked device xml with closed system before");
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

            size_t uiNumEntries=1;
            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available

            std::vector<PORT_REGISTER_STACK_ENTRY> entries(uiNumEntries);
            entries[0].Address = zNodeAddress;
            entries[0].pBuffer = &cBuffer[0];
            entries[0].Size = zNodeSize;

            eResult = m_ModPort.eGCReadPortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_MESSAGE("GCWritePortStacked GCReadPortStacked failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult >= GC_ERR_SUCCESS);

            oLibSysSetup.tearDownSystem();
                
            eResult = m_ModPort.eGCWritePortStacked(hDev, &entries[0], &uiNumEntries);
            GENTLTEST_CHECK_RESULT("Note: GCWritePortStacked with closed library return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCWritePortStacked with closed library failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult < GC_ERR_SUCCESS);

            oLibSysSetup.tearDown();
            oLibSysSetup.setUp();
        }
    }


    GENTLTEST_PRINT_RESULT(test_id);
}


/////////////////////////////////////////////////////////////////////
// tools
/////////////////////////////////////////////////////////////////////

bool Port_GCWritePortStacked::bFillEntry(SFNCPort &oLocalDevPort, std::string &sNodeName, GenApi::CNodePtr &cNode, PORT_REGISTER_STACK_ENTRY &oEntry)
{
    bool bResult = false;
    GenApi::CIntegerPtr intValue=cNode;
    int64_t xAddr=0;
    int64_t xSize=0;
    
    if (intValue != NULL && 
        (cNode->GetAccessMode() == GenApi::RW || cNode->GetAccessMode() == GenApi::RO))
    {
        try
        {
            intValue->GetValue();
            xAddr = oLocalDevPort.m_LastReadAddress;
            xSize = oLocalDevPort.m_LastReadSize;
            bResult = true;
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

void Port_GCWritePortStacked::vDeleteEntryBuffers(std::vector<PORT_REGISTER_STACK_ENTRY> &oEntries)
{
    for (size_t i=0; i<oEntries.size(); i++)
    {
        delete [] (char*)(oEntries[i].pBuffer);
        oEntries[i].pBuffer = NULL;
    }
}
