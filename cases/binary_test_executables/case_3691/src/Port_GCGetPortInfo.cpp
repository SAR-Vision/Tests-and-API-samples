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

#include "Port_GCGetPortInfo.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

extern uint8_t g_SkipTypeNULLCheck;
extern uint8_t g_SkipSizeNULLCheck;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Port_GCGetPortInfo::Port_GCGetPortInfo()
{
}

Port_GCGetPortInfo::~Port_GCGetPortInfo()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetPortInfo::setUp(void)
{
}

void Port_GCGetPortInfo::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test GCGetPortInfo
////////////////////////////////////////////////////////////////////////////////////////////

void Port_GCGetPortInfo::TestGCGetPortInfoSystem( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortInfo (using system)");
    LibrarySystemSetup oLibSysSetup;
    
    for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
    {
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        GC_ERROR eResult=GC_ERR_SUCCESS;
                
        eResult=m_ModPort.eGCGetPortInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, NULL, &iSize);
        if (eCommand == PORT_INFO_MODULE)
        {
            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo " << sConvertPORTCommand2String(eCommand).c_str() <<
                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS);
        }
        else
        {
            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo " << sConvertPORTCommand2String(eCommand).c_str() <<
                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
        }

        if (eResult >= GC_ERR_SUCCESS)
        {
            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

            std::vector<char> sBuffer(iSize);
            eResult=m_ModPort.eGCGetPortInfo(oLibSysSetup.hGetTLHandle(), eCommand, &iType, &sBuffer[0], &iSize);
            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo " << sConvertPORTCommand2String(eCommand).c_str() <<
                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult >= GC_ERR_SUCCESS);
                                
            if (eResult >= GC_ERR_SUCCESS)
            {
                GENTLTEST_DUMP_ITYPE("Info: " << sConvertPORTCommand2String(eCommand).c_str(),
                        iType, &sBuffer[0], iSize);

                if (iType == INFO_DATATYPE_STRING)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                } else if (iType == INFO_DATATYPE_STRINGLIST)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                }

                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                switch (eCommand)
                {
                case PORT_INFO_ID: 
                case PORT_INFO_VENDOR:
                case PORT_INFO_MODEL:
                case PORT_INFO_VERSION:
                case PORT_INFO_PORTNAME:  
                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                    break;
                case PORT_INFO_TLTYPE:
                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                    if (iType == INFO_DATATYPE_STRING)
                    {
                        std::string sTLType=&sBuffer[0];
                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                            " wrong tltype: expected 'Mixed', 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                            sTLType == TLTypeMixedName || sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                            sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                    }
                    break;
                case PORT_INFO_MODULE:
                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                    if (iType == INFO_DATATYPE_STRING)
                    {
                        std::string sModule=&sBuffer[0];
                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong module: expected 'TLSystem', module=" << sModule.c_str(),
                            sModule == "TLSystem");
                    }
                    break;
                case PORT_INFO_LITTLE_ENDIAN:
                case PORT_INFO_BIG_ENDIAN:
                case PORT_INFO_ACCESS_READ:
                case PORT_INFO_ACCESS_WRITE:
                case PORT_INFO_ACCESS_NA:
                case PORT_INFO_ACCESS_NI:  
                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_BOOL8 type=" << 
                        sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                    break;
                }

            }
        }
        else
        {
            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
            if (eResult == GC_ERR_NOT_IMPLEMENTED)
            {
                GENTLTEST_PRINT("Hint: " << sConvertGCError2String(eResult).c_str() << 
                    " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
            }
            else
            {
                GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << 
                    " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
            }
        }
    }
    
    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoInterface( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortInfo (using interface)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
        {
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            GC_ERROR eResult=GC_ERR_SUCCESS;
                
            eResult=m_ModPort.eGCGetPortInfo(hIF, eCommand, &iType, NULL, &iSize);
            if (eCommand == PORT_INFO_MODULE)
            {
                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS);
            }
            else
            {
                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
            }

            if (eResult >= GC_ERR_SUCCESS)
            {
                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                std::vector<char> sBuffer(iSize);
                eResult=m_ModPort.eGCGetPortInfo(hIF, eCommand, &iType, &sBuffer[0], &iSize);
                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS);
                                
                if (eResult >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                            ") " << sConvertPORTCommand2String(eCommand).c_str(),
                            iType, &sBuffer[0], iSize);

                    if (iType == INFO_DATATYPE_STRING)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                    } else if (iType == INFO_DATATYPE_STRINGLIST)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                    }

                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                    switch (eCommand)
                    {
                    case PORT_INFO_ID: 
                    case PORT_INFO_VENDOR:
                    case PORT_INFO_MODEL:
                    case PORT_INFO_VERSION:
                    case PORT_INFO_PORTNAME:  
                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                        break;
                    case PORT_INFO_TLTYPE:
                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            std::string sTLType=&sBuffer[0];
                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                        }
                        break;
                    case PORT_INFO_MODULE:
                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                            " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            std::string sModule=&sBuffer[0];
                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong module: expected 'TLInterface', module=" << sModule.c_str(),
                                sModule == "TLInterface");
                        }
                        break;
                    case PORT_INFO_LITTLE_ENDIAN:
                    case PORT_INFO_BIG_ENDIAN:
                    case PORT_INFO_ACCESS_READ:
                    case PORT_INFO_ACCESS_WRITE:
                    case PORT_INFO_ACCESS_NA:
                    case PORT_INFO_ACCESS_NI:  
                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_BOOL8 type=" << 
                            sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                        break;
                    }

                }
            }
            else
            {
                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                if (eResult == GC_ERR_NOT_IMPLEMENTED)
                {
                    GENTLTEST_PRINT("Hint: " << sConvertGCError2String(eResult).c_str() << 
                        " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                }
                else
                {
                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << 
                        " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                }
            }
        }
    }
    
    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortInfo (using device)");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                    {
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        size_t iSize=0;
                
                        eResult=m_ModPort.eGCGetPortInfo(hDev, eCommand, &iType, NULL, &iSize);
                        if (eCommand == PORT_INFO_MODULE)
                        {
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS);
                        }
                        else
                        {
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
                        }

                        if (eResult >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                            std::vector<char> sBuffer(iSize);
                            eResult=m_ModPort.eGCGetPortInfo(hDev, eCommand, &iType, &sBuffer[0], &iSize);
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS);
                                
                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                        ", DeviceID=" << sDeviceID.c_str() << 
                                        ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                        ") " << sConvertPORTCommand2String(eCommand).c_str(),
                                        iType, &sBuffer[0], iSize);

                                if (iType == INFO_DATATYPE_STRING)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                } else if (iType == INFO_DATATYPE_STRINGLIST)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                                }

                                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                switch (eCommand)
                                {
                                case PORT_INFO_ID: 
                                case PORT_INFO_VENDOR:
                                case PORT_INFO_MODEL:
                                case PORT_INFO_VERSION:
                                case PORT_INFO_PORTNAME:  
                                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                    break;
                                case PORT_INFO_TLTYPE:
                                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                    if (iType == INFO_DATATYPE_STRING)
                                    {
                                        std::string sTLType=&sBuffer[0];
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                            " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                            sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                            sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                    }
                                    break;
                                case PORT_INFO_MODULE:
                                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                        " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                    if (iType == INFO_DATATYPE_STRING)
                                    {
                                        std::string sModule=&sBuffer[0];
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong module: expected 'TLDevice', module=" << sModule.c_str(),
                                            sModule == "TLDevice");
                                    }
                                    break;
                                case PORT_INFO_LITTLE_ENDIAN:
                                case PORT_INFO_BIG_ENDIAN:
                                case PORT_INFO_ACCESS_READ:
                                case PORT_INFO_ACCESS_WRITE:
                                case PORT_INFO_ACCESS_NA:
                                case PORT_INFO_ACCESS_NI:  
                                    GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_BOOL8 type=" << 
                                        sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                    break;
                                }

                            }
                        }
                        else
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            if (eResult == GC_ERR_NOT_IMPLEMENTED)
                            {
                                GENTLTEST_PRINT("Hint: " << sConvertGCError2String(eResult).c_str() << 
                                    " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                            }
                            else
                            {
                                GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << 
                                    " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                            }
                        }
                    }
                }
            }
        }
    }
    
    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoRemoteDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortInfo (using remote device)");
    // Version 1
    // TLOpenInterface in front of each IFOpenDevice
    {
        LibrarySystemSetup oLibSysSetup;
        tIFDeviceList vIFDeviceList;
        tIFDeviceList::iterator xIter;

        vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

        for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
        {
            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
            size_t iSize=0;
            GC_ERROR eResult=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            IF_HANDLE hIF=GENTL_INVALID_HANDLE;
            PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
            stIFDevice ifdev=(stIFDevice)*xIter;
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
            Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

            if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
            {
                hPort = oDevPreCondition.hDevGetPort();

                eResult=m_ModPort.eGCGetPortInfo(hPort, ifdev.eCommand, &iType, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                    eResult >= GC_ERR_SUCCESS);

                if (eResult >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                    std::vector<char> sBuffer(iSize);
                    eResult=m_ModPort.eGCGetPortInfo(hPort, ifdev.eCommand, &iType, &sBuffer[0], &iSize);
                    GENTLTEST_CHECK_MESSAGE("GCGetPortInfo failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                        eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
                    
                    if (eResult >= GC_ERR_SUCCESS)
                    {
                        if (iType == INFO_DATATYPE_STRING)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                        }

                        if (iType == INFO_DATATYPE_STRINGLIST)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                        }

                    }
                }
                else
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << " LastErrorMessage("<<sConvertDEVICEAccess2String(ifdev.eAccess).c_str()<<", "<<ifdev.sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                }
            }
        }
    }

    // Version 2
    // TLOpenInterface only if neccessary
    {
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
                    GC_ERROR eResult=GC_ERR_SUCCESS;
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        hPort = oDevPreCondition.hDevGetPort();
                        
                        for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                
                            eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, NULL, &iSize);
                            if (eCommand == PORT_INFO_MODULE)
                            {
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS);
                            }
                            else
                            {
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
                            }

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                std::vector<char> sBuffer(iSize);
                                eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS);
                                
                                if (eResult >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                            ", DeviceID=" << sDeviceID.c_str() << 
                                            ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            ") " << sConvertPORTCommand2String(eCommand).c_str(),
                                            iType, &sBuffer[0], iSize);

                                    if (iType == INFO_DATATYPE_STRING)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                    } else if (iType == INFO_DATATYPE_STRINGLIST)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                                    }

                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                    switch (eCommand)
                                    {
                                    case PORT_INFO_ID: 
                                    case PORT_INFO_VENDOR:
                                    case PORT_INFO_MODEL:
                                    case PORT_INFO_VERSION:
                                    case PORT_INFO_PORTNAME:  
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        break;
                                    case PORT_INFO_TLTYPE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sTLType=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                                " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                        }
                                        break;
                                    case PORT_INFO_MODULE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sModule=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong module: expected 'Device', module=" << sModule.c_str(),
                                                sModule == "Device");
                                        }
                                        break;
                                    case PORT_INFO_LITTLE_ENDIAN:
                                    case PORT_INFO_BIG_ENDIAN:
                                    case PORT_INFO_ACCESS_READ:
                                    case PORT_INFO_ACCESS_WRITE:
                                    case PORT_INFO_ACCESS_NA:
                                    case PORT_INFO_ACCESS_NI:  
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_BOOL8 type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    }

                                }
                            }
                            else
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                if (eResult == GC_ERR_NOT_IMPLEMENTED)
                                {
                                    GENTLTEST_PRINT("Hint: " << sConvertGCError2String(eResult).c_str() << 
                                        " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Error: " << sConvertGCError2String(eResult).c_str() << 
                                        " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
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

void Port_GCGetPortInfo::TestGCGetPortInfoDataStream( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortInfo (using datastream)");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                    
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        
                        for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                
                            eResult=m_ModPort.eGCGetPortInfo(hDs, eCommand, &iType, NULL, &iSize);
                            if (eCommand == PORT_INFO_MODULE)
                            {
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS);
                            }
                            else
                            {
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
                            }

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                std::vector<char> sBuffer(iSize);
                                eResult=m_ModPort.eGCGetPortInfo(hDs, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS);
                                
                                if (eResult >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                            ", DeviceID=" << sDeviceID.c_str() << 
                                            ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            ") " << sConvertPORTCommand2String(eCommand).c_str(),
                                            iType, &sBuffer[0], iSize);

                                    if (iType == INFO_DATATYPE_STRING)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                    } else if (iType == INFO_DATATYPE_STRINGLIST)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                                    }

                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                    switch (eCommand)
                                    {
                                    case PORT_INFO_ID: 
                                    case PORT_INFO_VENDOR:
                                    case PORT_INFO_MODEL:
                                    case PORT_INFO_VERSION:
                                    case PORT_INFO_PORTNAME:  
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        break;
                                    case PORT_INFO_TLTYPE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sTLType=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                                " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                        }
                                        break;
                                    case PORT_INFO_MODULE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sModule=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong module: expected 'TLDataStream', module=" << sModule.c_str(),
                                                sModule == "TLDataStream");
                                        }
                                        break;
                                    case PORT_INFO_LITTLE_ENDIAN:
                                    case PORT_INFO_BIG_ENDIAN:
                                    case PORT_INFO_ACCESS_READ:
                                    case PORT_INFO_ACCESS_WRITE:
                                    case PORT_INFO_ACCESS_NA:
                                    case PORT_INFO_ACCESS_NI:  
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_BOOL8 type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    }

                                }
                            }
                            else
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                if (eResult == GC_ERR_NOT_IMPLEMENTED)
                                {
                                    GENTLTEST_PRINT("Hint: device access = " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                                        ", device ID=" << sDeviceID.c_str() <<
                                        ", GCGetPortInfo(" << sConvertPORTCommand2String(eCommand).c_str() << 
                                        " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
             
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Hint: allowed error: " << sConvertGCError2String(eResult).c_str() << 
                                        " LastErrorMessage("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str()<<"): '" << sMsg.c_str() << "'" << std::endl);
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

void Port_GCGetPortInfo::TestGCGetPortInfoBuffer( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard GCGetPortInfo (using buffer)");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                    
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        
                        oDSPreCondition.eDSAllocAndAnnounceBuffer(hDs, uiPayLoadSize, (void*)this, &hBuffer); 

                        for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                
                            eResult=m_ModPort.eGCGetPortInfo(hBuffer, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);
                            
                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                std::vector<char> sBuffer(iSize);
                                eResult=m_ModPort.eGCGetPortInfo(hBuffer, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult >= GC_ERR_SUCCESS);
                                
                                if (eResult >= GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_DUMP_ITYPE("Info: (interfaceID=" << xInterfaceList[index1].c_str() << 
                                            ", DeviceID=" << sDeviceID.c_str() << 
                                            ", device access=" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                                            ") " << sConvertPORTCommand2String(eCommand).c_str(),
                                            iType, &sBuffer[0], iSize);

                                    if (iType == INFO_DATATYPE_STRING)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING iSize < 1", iSize >= 1);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRING missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                    } else if (iType == INFO_DATATYPE_STRINGLIST)
                                    {
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST iSize < 2", iSize >= 2);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 0", sBuffer[sBuffer.size()-1] == 0);
                                        GENTLTEST_CHECK_MESSAGE("Parameter after call INFO_DATATYPE_STRINGLIST missing trailing 00", sBuffer[sBuffer.size()-2] == 0);
                                    }

                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize = 0", iSize != 0);

                                    switch (eCommand)
                                    {
                                    case PORT_INFO_ID: 
                                    case PORT_INFO_VENDOR:
                                    case PORT_INFO_MODEL:
                                    case PORT_INFO_VERSION:
                                    case PORT_INFO_PORTNAME:  
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        break;
                                    case PORT_INFO_TLTYPE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong datatype: expected INFO_DATATYPE_STRING type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sTLType=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                                " wrong tltype: expected 'Custom', 'GEV', 'CL', 'IIDC', 'UVC', 'CXP', 'CLHS', 'U3V', 'Ethernet', 'PCI', TLType=" << sTLType.c_str(), 
                                                sTLType == TLTypeCustomName || sTLType == TLTypeGEVName || sTLType == TLTypeCLName || sTLType == TLTypeIIDCName || sTLType == TLTypeUVCName ||
                                                sTLType == TLTypeCXPName || sTLType == TLTypeCLHSName || sTLType == TLTypeU3VName || sTLType == TLTypeETHERNETName || sTLType == TLTypePCIName);
                                        }
                                        break;
                                    case PORT_INFO_MODULE:
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << 
                                            " wrong datatype: expected INFO_DATATYPE_STRING type=" << sConvertDataType2String(iType), iType == INFO_DATATYPE_STRING); 
                                        
                                        if (iType == INFO_DATATYPE_STRING)
                                        {
                                            std::string sModule=&sBuffer[0];
                                            GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong module: expected 'TLBuffer', module=" << sModule.c_str(),
                                                sModule == "TLBuffer");
                                        }
                                        break;
                                    case PORT_INFO_LITTLE_ENDIAN:
                                    case PORT_INFO_BIG_ENDIAN:
                                    case PORT_INFO_ACCESS_READ:
                                    case PORT_INFO_ACCESS_WRITE:
                                    case PORT_INFO_ACCESS_NA:
                                    case PORT_INFO_ACCESS_NI:  
                                        GENTLTEST_CHECK_MESSAGE(sConvertPORTCommand2String(eCommand).c_str() << " wrong data expected INFO_DATATYPE_BOOL8 type=" << 
                                            sConvertDataType2String(iType), iType == INFO_DATATYPE_BOOL8); 
                                        break;
                                    }

                                }
                            }
                            else
                            {
                                if (eResult != GC_ERR_NOT_IMPLEMENTED)
                                {
                                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                    GENTLTEST_PRINT("Hint: GCGetPortInfo(" << sConvertPORTCommand2String(eCommand).c_str() << 
                                        ") allowed error: " << sConvertGCError2String(eResult).c_str() << 
                                        " message("<<sConvertDEVICEAccess2String(eAccess).c_str()<<", "<<sDeviceID.c_str() <<
                                        "): '" << sMsg.c_str() << "'" << std::endl);
                                }
                                else
                                {
                                    GENTLTEST_PRINT("Hint: device access = " << sConvertDEVICEAccess2String(eAccess).c_str() << 
                                        ", device ID=" << sDeviceID.c_str() <<
                                        ", GCGetPortInfo(" << sConvertPORTCommand2String(eCommand).c_str() << 
                                        ") not implemented." << std::endl);
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

void Port_GCGetPortInfo::TestGCGetPortInfoWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF=GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
        
            oLibSysSetup.tearDownLibrary();

            eResult=m_ModPort.eGCGetPortInfo(hPort, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected == GC_ERR_NOT_INITIALIZED or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_NOT_INITIALIZED || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
        
            oLibSysSetup.tearDownSystem();

            eResult=m_ModPort.eGCGetPortInfo(hPort, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }

            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
        
            oIFPreCondition.vClose();
            
            eResult=m_ModPort.eGCGetPortInfo(hPort, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR eResult=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
        size_t iSize=0;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            hPort = oDevPreCondition.hDevGetPort();
        
            oDevPreCondition.vClose();
            
            eResult=m_ModPort.eGCGetPortInfo(hPort, ifdev.eCommand, &iType, NULL, &iSize);
            GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                "GCGetPortInfo InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " " << sConvertPORTCommand2String(ifdev.eCommand).c_str() <<
                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                eResult < GC_ERR_SUCCESS);

            if (eResult < GC_ERR_SUCCESS)
            {
                GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithPortHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with parameter port handle = GENTL_INVALID_HANDLE");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();
                    
                    for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                    {
                        size_t iSize=0;
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        
                        eResult=m_ModPort.eGCGetPortInfo(GENTL_INVALID_HANDLE, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult == GC_ERR_INVALID_HANDLE || eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                            "GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult < GC_ERR_SUCCESS);

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithInvalidCommand( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with parameter iInfoCmd = PORT_INFO_PORTNAME+1");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();
                    
                    for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                    {
                        size_t iSize=0;
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        
                        eResult=m_ModPort.eGCGetPortInfo(hPort, PORT_INFO_PORTNAME+1, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                            "GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult < GC_ERR_SUCCESS);

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithTypeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with parameter piType = NULL at second call with initialized buffer");
    
    if (!g_SkipTypeNULLCheck)
    {
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
                    GC_ERROR eResult=GC_ERR_SUCCESS;
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        hPort = oDevPreCondition.hDevGetPort();
                    
                        for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                        {
                            size_t iSize=0;
                            size_t iSizeSave=0;
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        
                            eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                " size check failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                iSizeSave = iSize;

                                std::vector<char> sBuffer(iSize);
                                eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, NULL, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult == GC_ERR_INVALID_PARAMETER,
                                    "GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " buffer failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult < GC_ERR_SUCCESS);

                                if (eResult < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with parameter piSize = NULL");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();
                    
                    for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                    {
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        
                        eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, NULL, NULL);
                        GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult == GC_ERR_INVALID_PARAMETER || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE,
                            "GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult < GC_ERR_SUCCESS);

                        if (eResult < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iType != INFO_DATATYPE_UNKNOWN", iType == INFO_DATATYPE_UNKNOWN);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Port_GCGetPortInfo::TestGCGetPortInfoWithSizeLow( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with parameter iSize = iSize - 1 at second call with initialized buffer");
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
                GC_ERROR eResult=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    hPort = oDevPreCondition.hDevGetPort();
                    
                    for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                    {
                        INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                        INFO_DATATYPE iTypeSave=INFO_DATATYPE_UNKNOWN;
                        size_t iSize=0;
                        size_t iSizeSave=0;
                        
                        eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, NULL, &iSize);
                        GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                            " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                            eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                        if (eResult >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                            iTypeSave = iType;

                            if (iSize > 1)
                            {
                                std::vector<char> sBuffer(iSize);
                                iSize--;
                                iSizeSave = iSize;
                                eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult == GC_ERR_INVALID_PARAMETER,
                                    "GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult < GC_ERR_SUCCESS);

                                if (eResult < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);
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

void Port_GCGetPortInfo::TestGCGetPortInfoWithBufferSizeNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test GCGetPortInfo with parameter piSize = NULL at second call with initialized buffer");
    
    if (!g_SkipSizeNULLCheck)
    {
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
                    GC_ERROR eResult=GC_ERR_SUCCESS;
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                    PORT_HANDLE hPort=GENTL_INVALID_HANDLE;
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        hPort = oDevPreCondition.hDevGetPort();
                    
                        for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                        {
                            INFO_DATATYPE iType=INFO_DATATYPE_UNKNOWN;
                            INFO_DATATYPE iTypeSave=INFO_DATATYPE_UNKNOWN;
                            size_t iSize=0;
                            size_t iSizeSave=0;
                        
                            eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                " failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_NOT_AVAILABLE, received " << sConvertGCError2String(eResult).c_str(),  
                                eResult >= GC_ERR_SUCCESS || eResult == GC_ERR_NOT_IMPLEMENTED || eResult == GC_ERR_NOT_AVAILABLE);

                            if (eResult >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize > 0);

                                iTypeSave = iType;

                                std::vector<char> sBuffer(iSize);
                                iSizeSave = iSize;
                                eResult=m_ModPort.eGCGetPortInfo(hPort, eCommand, &iType, &sBuffer[0], NULL);
                                GENTLTEST_CHECK_RESULT("Note: GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult == GC_ERR_INVALID_PARAMETER,
                                    "GCGetPortInfo InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " " << sConvertPORTCommand2String(eCommand).c_str() <<
                                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(eResult).c_str(),  
                                    eResult < GC_ERR_SUCCESS);

                                if (eResult < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iType != old iType", iTypeSave == iType);
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != old iSize", iSizeSave == iSize);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        GENTLTEST_SKIP("Due to convenience reasons of the function. The test is neglectable.");
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Port_GCGetPortInfo::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    tIFDeviceList::iterator xIter;
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID;
            
            sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                for (PORT_INFO_CMD_LIST eCommand=PORT_INFO_ID; eCommand<=PORT_INFO_PORTNAME; eCommand=(PORT_INFO_CMD_LIST)(eCommand+1))
                {
                    stIFDevice ifdev;

                    ifdev.sInterfaceID = xInterfaceList[index1];
                    ifdev.sDeviceID = sDeviceID;
                    ifdev.eAccess = eAccess;
                    ifdev.eCommand = eCommand;
                    vIFDeviceList.push_back(ifdev);
                }
            }
        }
    }

    //GENTLTEST_PRINT("vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
