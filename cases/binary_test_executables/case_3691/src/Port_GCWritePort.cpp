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

#include "Port_GCWritePort.h"
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

Port_GCWritePort::Port_GCWritePort()
{
}

Port_GCWritePort::~Port_GCWritePort()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCWritePort::setUp(void)
{
}

void Port_GCWritePort::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCWritePort
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCWritePort::TestGCWritePort( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCWritePort node 'Height' in romte device xml");
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

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
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
                        oPortPreCondition.vReadNodeFromXMLString(oLocalNodemap, oLocalPort, "Device", "Height", sXMLString, cNode);
                        
                        if (cNode.IsValid())
                        {

                            GenApi::CIntegerPtr cValue=cNode;
                            if (cValue != NULL && cValue->GetAccessMode() == GenApi::RW)
                            {
                                int64_t zSaveValue;
                                int64_t zNewValue;
                                int64_t zValue;

                                zSaveValue = cValue->GetValue();
                                zNewValue = zSaveValue-2;
                                cValue->SetValue(zNewValue);
                                zValue = cValue->GetValue();
                                GENTLTEST_CHECK_MESSAGE("GCWritePort remote device XML port='Device' name='Height' failed", zNewValue == zValue);
                                cValue->SetValue(zSaveValue);
                            }
                        }
                        else
                        {
                            //GENTLTEST_PRINT("Please check XML file:" << std::endl);
                            //GENTLTEST_PRINT(sXMLString.c_str());
                            GENTLTEST_CHECK_MESSAGE("GCWritePort remote device XML standard entries failed, port='Device' name='Height' is invalid.", false);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCWritePort::TestGCWritePortWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCWritePort device xml with closed library before");
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

            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_MESSAGE("GCWritePort GCReadPort failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult >= GC_ERR_SUCCESS);

            oLibSysSetup.tearDownLibrary();

            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_MESSAGE("GCWritePort with closed library failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_NOT_INITIALIZED);
        
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCWritePort::TestGCWritePortWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCWritePort device xml with closed system before");
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

            std::vector<char> cBuffer(zNodeSize + 10); // ensure that a 0 termination is available
            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_MESSAGE("GCWritePort GCReadPort failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
                eResult >= GC_ERR_SUCCESS);

            oLibSysSetup.tearDownSystem();

            eResult = m_ModPort.eGCReadPort(hDev, zNodeAddress, &cBuffer[0], &zNodeSize);
            GENTLTEST_CHECK_RESULT("Note: GCWritePort with closed library return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(), 
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER,
                "GCWritePort with closed library failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(), 
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

