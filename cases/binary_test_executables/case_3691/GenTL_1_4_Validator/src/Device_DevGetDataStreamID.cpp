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

#include "Device_DevGetDataStreamID.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Device_DevGetDataStreamID::Device_DevGetDataStreamID()
{
}

Device_DevGetDataStreamID::~Device_DevGetDataStreamID()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetDataStreamID::setUp(void)
{
}

void Device_DevGetDataStreamID::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test DevGetDataStreamID
////////////////////////////////////////////////////////////////////////////////////////////

void Device_DevGetDataStreamID::TestDevGetDataStreamID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard DevGetDataStreamID");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                    
                        Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, &iSize);
                        GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                            " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result >= GC_ERR_SUCCESS);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                            std::vector<char> sBuffer(iSize);

                            Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, &sBuffer[0], &iSize);
                            GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_PRINT("Info: InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3+1 << "/" << uiNumDataStreams << 
                                    " datastreamID=" << &sBuffer[0] << std::endl);
                            }
                        }
                        
                        if (Result < GC_ERR_SUCCESS)
                        {
                            std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                            GENTLTEST_PRINT("Error: InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " " << sConvertGCError2String(Result).c_str() << " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                        }
                    }

                    if (0 == uiNumDataStreams)
                    {
                        // just to check if a call will return "not implemented"
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                    
                        Result = m_ModDev.eDevGetDataStreamID(hDev, 0, NULL, &iSize);
                        GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID failed. Expected == GC_ERR_NOT_IMPLEMENTED, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_NOT_IMPLEMENTED);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);

        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
            
            oLibSysSetup.tearDownLibrary();
            
            for (uint32_t iIndex=0; iIndex<uiNumDataStreams; iIndex++)
            {
                size_t iSize = 0;
            
                Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex, NULL, &iSize);
                GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_NOT_INITIALIZED);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                }
            }
            
            oLibSysSetup.tearDownSystem();
            oLibSysSetup.setUp();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);
    
    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
            
            oLibSysSetup.tearDownSystem();
            
            for (uint32_t iIndex=0; iIndex<uiNumDataStreams; iIndex++)
            {
                size_t iSize = 0;
            
                Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                    "DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                }
            }
            
            oLibSysSetup.setUpSystem();
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithoutIFOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
            
            oIFPreCondition.vClose();

            for (uint32_t iIndex=0; iIndex<uiNumDataStreams; iIndex++)
            {
                size_t iSize = 0;
            
                Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                    "DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with closed device before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDataStreams=0;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        Device_PreCondition oDevPreCondition(hIF, ifdev.sDeviceID, ifdev.eAccess, &hDev);
        
        if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
        {
            uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
            
            oDevPreCondition.vClose();

            for (uint32_t iIndex=0; iIndex<uiNumDataStreams; iIndex++)
            {
                size_t iSize = 0;
            
                Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex, NULL, &iSize);
                GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                    "DevGetDataStreamID InterfaceID=" << ifdev.sInterfaceID.c_str() << " DeviceID=" << ifdev.sDeviceID.c_str() << " " << 
                    sConvertDEVICEAccess2String(ifdev.eAccess).c_str() << " index=" << iIndex << "/" << uiNumDataStreams << 
                    " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with parameter piSize = NULL");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        
                        Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, NULL);
                        GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                            " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_PARAMETER,
                            "DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithDevHandleNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with parameter device handle = GENTL_INVALID_HANDLE");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        
                        Result = m_ModDev.eDevGetDataStreamID(GENTL_INVALID_HANDLE, iIndex3, NULL, &iSize);
                        GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                            " return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                            "DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                            sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                            " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                            Result < GC_ERR_SUCCESS);

                        if (Result < GC_ERR_SUCCESS)
                        {
                            GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDWithDevGreaterIndex( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with parameter index > uiNumDataStreams");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    if (uiNumDataStreams > 0) 
                    {
                        for (uint32_t iIndex3=uiNumDataStreams; iIndex3<uiNumDataStreams+10; iIndex3++)
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            size_t iSize = 0;
                        
                            Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, &iSize);
                            GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER,
                                "DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result < GC_ERR_SUCCESS);
    
                                if (Result < GC_ERR_SUCCESS)
                                {
                                    GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                                }
                        }
                    }
                    else
                    {
                        GENTLTEST_CHECK(true);  // Dummy for boost to count this as done
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDBufferWithoutSize( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with parameter iSize = 0 at second call with initialized buffer");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        
                        Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, &iSize);
                        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                            std::vector<char> sBuffer(iSize);
                            iSize = 0;
                            Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, &sBuffer[0], &iSize);
                            GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER,
                                "DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result < GC_ERR_SUCCESS);

                            if (Result < GC_ERR_SUCCESS)
                            {
                                GENTLTEST_CHECK_MESSAGE("Parameter after call iSize != 0", iSize == 0);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDBufferWithSizeNull( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID with parameter piSize = NULL at second call with initialized buffer");
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
                uint32_t uiNumDataStreams=0;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                
                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    uiNumDataStreams = oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                    {
                        GC_ERROR Result=GC_ERR_SUCCESS;
                        size_t iSize = 0;
                        
                        Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, &iSize);
                        GENTLTEST_CHECK(Result >= GC_ERR_SUCCESS);

                        if (Result >= GC_ERR_SUCCESS)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                            std::vector<char> sBuffer(iSize);

                            Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, &sBuffer[0], NULL);
                            GENTLTEST_CHECK_RESULT("Note: DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                                Result == GC_ERR_INVALID_PARAMETER,
                                "DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result < GC_ERR_SUCCESS);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Device_DevGetDataStreamID::TestDevGetDataStreamIDPersistence( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test DevGetDataStreamID persistence between two sessions.");
    std::vector<std::string> vecStreamIDs;

    LibrarySystemSetup oLibSysSetup;
        
    {
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
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                    
                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                        
                        for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            size_t iSize = 0;
                        
                            Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                                std::vector<char> sBuffer(iSize);

                                Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS);

                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    vecStreamIDs.push_back(&sBuffer[0]);
                                }
                            }
                            
                            if (Result < GC_ERR_SUCCESS)
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                GENTLTEST_PRINT("Error: InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                    " " << sConvertGCError2String(Result).c_str() << " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                            }
                        }
                    }
                }
            }
        }
    }

    {
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
                    Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);           
                    
                    if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                    {
                        uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                        
                        for (uint32_t iIndex3=0; iIndex3<uiNumDataStreams; iIndex3++)
                        {
                            GC_ERROR Result=GC_ERR_SUCCESS;
                            size_t iSize = 0;
                        
                            Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, NULL, &iSize);
                            GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                Result >= GC_ERR_SUCCESS);

                            if (Result >= GC_ERR_SUCCESS)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Parameter after call iSize == 0", iSize != 0);

                                std::vector<char> sBuffer(iSize);

                                Result = m_ModDev.eDevGetDataStreamID(hDev, iIndex3, &sBuffer[0], &iSize);
                                GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                    " failed. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                                    Result >= GC_ERR_SUCCESS);

                                if (Result >= GC_ERR_SUCCESS)
                                {
                                    std::vector<std::string>::iterator xIter;

                                    for (xIter=vecStreamIDs.begin(); xIter!=vecStreamIDs.end(); xIter++)
                                    {
                                        if (*xIter == std::string(&sBuffer[0]))
                                        {
                                            break;
                                        }
                                    }

                                    GENTLTEST_CHECK_MESSAGE("DevGetDataStreamID InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                        sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                        " cannot find datastreamID=" << &sBuffer[0] << " in previous saved id list.",
                                        xIter != vecStreamIDs.end())
                                }
                            }
                            
                            if (Result < GC_ERR_SUCCESS)
                            {
                                std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                                GENTLTEST_PRINT("Error: InterfaceID=" << xInterfaceList[index1].c_str() << " DeviceID=" << sDeviceID.c_str() << " " << 
                                    sConvertDEVICEAccess2String(eAccess).c_str() << " index=" << iIndex3 << "/" << uiNumDataStreams << 
                                    " " << sConvertGCError2String(Result).c_str() << " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                            }
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Device_DevGetDataStreamID::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
{
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        uint32_t uiNumDevices;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        
        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                for (DEVICE_INFO_CMD_LIST eCommand=DEVICE_INFO_ID; eCommand<=DEVICE_INFO_TIMESTAMP_FREQUENCY; eCommand=(DEVICE_INFO_CMD_LIST)(eCommand+1))
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

    //GENTLTEST_PRINT("Device_DevGetDataStreamID::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
