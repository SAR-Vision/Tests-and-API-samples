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

#include "Interface_IFOpenDevice.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Interface_IFOpenDevice::Interface_IFOpenDevice()
{
}

Interface_IFOpenDevice::~Interface_IFOpenDevice()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFOpenDevice::setUp(void)
{
}

void Interface_IFOpenDevice::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test IFOpenDevice
////////////////////////////////////////////////////////////////////////////////////////////

void Interface_IFOpenDevice::TestIFOpenDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard IFOpenDevice");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    uint32_t uiTotalNumDevices = 0;

    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        uiTotalNumDevices += uiNumDevices;

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            int nDeviceCount = 0;
            tCheckedDeviceAccessModes vecCheckedDeviceAccessModes;
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                    
                GENTLTEST_CHECK_MESSAGE("IFOpenDevice (" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                    ") failed expected result >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED returned " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                if (Result >= GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Device Handle == GENTL_INVALID_HANDLE", hDev != GENTL_INVALID_HANDLE);
                    nDeviceCount++;
                }
                else
                {
                    std::string sMsg=oLibSysSetup.sGetLastErrorMessage();
                    if (Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED)
                    {
                        GENTLTEST_PRINT("Hint: IFOpenDevice (" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            ") returned " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                    }
                    else
                    {
                        GENTLTEST_PRINT("Error: IFOpenDevice (" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                            ") returned " << sConvertGCError2String(Result).c_str() << 
                            " LastErrorMessage: '" << sMsg.c_str() << "'" << std::endl);
                    }
                }

                stCheckedResult xResult;
                xResult.eAccess = eAccess;
                xResult.eResult = Result;
                vecCheckedDeviceAccessModes.push_back(xResult);
                
                m_ModDev.eDevClose(hDev);
            }

            if (nDeviceCount == 0)
            {
                GENTLTEST_CHECK_MESSAGE("Error: no device with any access mode implemented.", false);
            }

            tCheckedDeviceAccessModes::iterator xIter;
            std::string sMessage;
            
            for (xIter=vecCheckedDeviceAccessModes.begin(); xIter!=vecCheckedDeviceAccessModes.end(); xIter++)
            {
                stCheckedResult xResult=*xIter;
                sMessage += sConvertDEVICEAccess2String(xResult.eAccess) + std::string("=") + sConvertGCError2String(xResult.eResult) + std::string(", ");
            }

            GENTLTEST_PRINT("Info: " << __FUNCTION__ << " (" << index2+1 << "/" << uiNumDevices << ") interfaceID=" << xInterfaceList[index1].c_str()
                        << " DeviceID=" << sDeviceID.c_str() << " following access modes checked: " << sMessage.c_str() << std::endl);
        }
    }

    // for certification we need at least one device
    GENTLTEST_CHECK_MESSAGE(std::endl << "Error: ***********************************************************" << std::endl <<
        "Error: * For GenICam Validation at least one device should exist *" << std::endl <<
        "Error: * Returned uiTotalNumDevices = " << uiTotalNumDevices << " *" << std::endl <<
        "Error: * Aborting validation                                     *" << std::endl <<
        "Error: ***********************************************************" << std::endl, 
        uiTotalNumDevices > 0);

    if (uiTotalNumDevices == 0)
        g_bStopValidationFlag = true;

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithPublicDeviceID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with known device ID (see section 3.3)");
    LibrarySystemSetup oLibSysSetup;
    System_PreCondition::tStringVector xInterfaceList=oLibSysSetup.xGetTLInterfaceList();
    tCheckListInterfaceKnownDevice vecInterfaceDevices;

    // get all device IDs
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        stCheckListKnownDevice xKnownDevices;
        uint32_t uiNumDevices;
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF);

        uiNumDevices = oIFPreCondition.zGetIFNumberOfDevices();
        xKnownDevices.uiNumDevices = uiNumDevices;

        for (uint32_t index2=0; index2<uiNumDevices; index2++)
        {
            std::string sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);
            xKnownDevices.vecDeviceIDList.push_back(sDeviceID);
        }

        vecInterfaceDevices.push_back(xKnownDevices);
    }

    // try to access them
    for (uint32_t index1=0; index1<xInterfaceList.size(); index1++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), xInterfaceList[index1], &hIF, false);

        if (vecInterfaceDevices[index1].uiNumDevices > 0)
        {
            for (uint32_t index2=0; index2<vecInterfaceDevices[index1].vecDeviceIDList.size(); index2++)
            {
                // this is an interface with device(s) --> we expect one accessmode should pass.
                std::string sDeviceID = vecInterfaceDevices[index1].vecDeviceIDList[index2];

                int nTestsPassed=0;
                
                for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
                {
                    GC_ERROR Result=GC_ERR_SUCCESS;
                    DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                
                    Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                
                    GENTLTEST_CHECK_MESSAGE("IFOpenDevice (" << sConvertDEVICEAccess2String(eAccess).c_str() << 
                        ") failed expected result >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED returned " << sConvertGCError2String(Result), 
                        Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                    if (Result >= GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Parameter after call device handle == GENTL_INVALID_HANDLE", hDev != GENTL_INVALID_HANDLE);

                        nTestsPassed++;
                    }

                    m_ModDev.eDevClose(hDev);
                }

                GENTLTEST_CHECK_MESSAGE("No access mode returned success for device=" << sDeviceID.c_str(), nTestsPassed > 0);
            }
        }
        else
        {
            // this is an interface without device(s) --> we cannot do anything
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithoutGCInitLib( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with closed library before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;

    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        oLibSysSetup.tearDownLibrary();
        
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;

        Result = m_ModIF.eIFOpenDevice(hIF, ifdev.sDeviceID.c_str(), ifdev.eAccess, &hDev);
        GENTLTEST_CHECK_MESSAGE("IFOpenDevice failed. Expected == GC_ERR_NOT_INITIALIZED, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_NOT_INITIALIZED);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
        }
        
        oLibSysSetup.tearDownSystem();
        oLibSysSetup.setUp();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithoutTLOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with closed system before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);

        oLibSysSetup.tearDownSystem();
        
        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;

        Result = m_ModIF.eIFOpenDevice(hIF, ifdev.sDeviceID.c_str(), ifdev.eAccess, &hDev);
        GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
        }
        
        oLibSysSetup.setUpSystem();
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithOldHandle( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with closed interface before");
    LibrarySystemSetup oLibSysSetup;
    tIFDeviceList vIFDeviceList;
    tIFDeviceList::iterator xIter;
    
    vCreateTestCaseList(oLibSysSetup, vIFDeviceList);

    for (xIter=vIFDeviceList.begin(); xIter!=vIFDeviceList.end(); xIter++)
    {
        IF_HANDLE hIF = GENTL_INVALID_HANDLE;
        stIFDevice ifdev=(stIFDevice)*xIter;
        {
            Interface_PreCondition oIFPreCondition(oLibSysSetup.hGetTLHandle(), ifdev.sInterfaceID, &hIF);
        }

        GC_ERROR Result=GC_ERR_SUCCESS;
        DEV_HANDLE hDev=GENTL_INVALID_HANDLE;

        Result = m_ModIF.eIFOpenDevice(hIF, ifdev.sDeviceID.c_str(), ifdev.eAccess, &hDev);
        GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
            Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
            "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
            Result < GC_ERR_SUCCESS);

        if (Result < GC_ERR_SUCCESS)
        {
            GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithIFNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with interface handle = GENTL_INVALID_HANDLE");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(GENTL_INVALID_HANDLE, sDeviceID.c_str(), eAccess, &hDev);
                GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                    "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithDevIDNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with pointer sDeviceID = NULL");
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
            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;

                Result = m_ModIF.eIFOpenDevice(hIF, NULL, eAccess, &hDev);
                GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER,
                    "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithWrongDevID( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with phantasy sDeviceID");
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
            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(hIF, "Hugo", eAccess, &hDev);
                GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_INVALID_ID, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_INVALID_ID,
                    "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceDoubleOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test double IFOpenDevice (see section 3.4)");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                DEV_HANDLE hDevSave=GENTL_INVALID_HANDLE;
                
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                
                GENTLTEST_CHECK_MESSAGE("IFOpenDevice 1 (" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                    ") failed expected result >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED returned " << sConvertGCError2String(Result).c_str(), 
                    Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

                if (Result >= GC_ERR_SUCCESS)
                {
                    hDevSave = hDev;

                    Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, &hDev);
                    
                    GENTLTEST_CHECK_MESSAGE("IFOpenDevice 2 (" << sConvertDEVICEAccess2String(eAccess).c_str() <<
                        ") failed expected result == GC_ERR_RESOURCE_IN_USE returned " << sConvertGCError2String(Result).c_str(), 
                        Result == GC_ERR_RESOURCE_IN_USE);

                    if (Result < GC_ERR_SUCCESS)
                    {
                        GENTLTEST_CHECK_MESSAGE("Device Handle has changed", hDevSave == hDev);
                    }

                    m_ModDev.eDevClose(hDev);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceControlOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test multiple IFOpenDevice (mode DEVICE_ACCESS_CONTROL and mode DEVICE_ACCESS_READONLY)");
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

            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev1=GENTL_INVALID_HANDLE;
            DEV_HANDLE hDev2=GENTL_INVALID_HANDLE;
            
            Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), DEVICE_ACCESS_CONTROL, &hDev1);
            GENTLTEST_CHECK_MESSAGE("IFOpenDevice 1 (" << sConvertDEVICEAccess2String(DEVICE_ACCESS_CONTROL).c_str() << 
                  ") failed expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED returned " << sConvertGCError2String(Result).c_str(), 
                  Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

            if (Result >= GC_ERR_SUCCESS)
            {
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), DEVICE_ACCESS_READONLY, &hDev2);
                
                GENTLTEST_CHECK_MESSAGE("IFOpenDevice 2 (" << sConvertDEVICEAccess2String(DEVICE_ACCESS_READONLY).c_str() << 
                    ") failed expected result == GC_ERR_RESOURCE_IN_USE or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED returned " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_RESOURCE_IN_USE || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);
            }
            
            m_ModDev.eDevClose(hDev1);
            m_ModDev.eDevClose(hDev2);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceExclusiveOpen( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test multiple IFOpenDevice (mode DEVICE_ACCESS_EXCLUSIVE and mode DEVICE_ACCESS_READONLY and mode DEVICE_ACCESS_CONTROL)");
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

            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev1=GENTL_INVALID_HANDLE;
            DEV_HANDLE hDev2=GENTL_INVALID_HANDLE;
            DEV_HANDLE hDev3=GENTL_INVALID_HANDLE;
            
            Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), DEVICE_ACCESS_EXCLUSIVE, &hDev1);
            GENTLTEST_CHECK_MESSAGE("IFOpenDevice failed. Expected >= GC_ERR_SUCCESS or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED, received " << sConvertGCError2String(Result).c_str(), 
                Result >= GC_ERR_SUCCESS || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED);

            if (Result >= GC_ERR_SUCCESS)
            {
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), DEVICE_ACCESS_CONTROL, &hDev2);
                GENTLTEST_CHECK_MESSAGE("IFOpenDevice failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_RESOURCE_IN_USE);

                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), DEVICE_ACCESS_READONLY, &hDev3);
                GENTLTEST_CHECK_MESSAGE("IFOpenDevice failed. Expected == GC_ERR_RESOURCE_IN_USE, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_RESOURCE_IN_USE);

                m_ModDev.eDevClose(hDev2);
                m_ModDev.eDevClose(hDev3);
            }
            else 
            {
                if (Result == GC_ERR_NOT_IMPLEMENTED)
                    GENTLTEST_PRINT("Hint: Test not possible, DEVICE_ACCESS_EXCLUSIVE not implemented.");
            }

            m_ModDev.eDevClose(hDev1);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithDevHandleNULL( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with pointer device handle = NULL");
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
                GC_ERROR Result=GC_ERR_SUCCESS;
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
            
                Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), eAccess, NULL);
                GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_HANDLE or == GC_ERR_INVALID_PARAMETER, received " << sConvertGCError2String(Result).c_str(), 
                    Result == GC_ERR_INVALID_HANDLE || Result == GC_ERR_INVALID_PARAMETER,
                    "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                    Result < GC_ERR_SUCCESS);

                if (Result < GC_ERR_SUCCESS)
                {
                    GENTLTEST_CHECK_MESSAGE("Parameter after call device handle != GENTL_INVALID_HANDLE", hDev == GENTL_INVALID_HANDLE);
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

void Interface_IFOpenDevice::TestIFOpenDeviceWithInvalidAccessMode( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test IFOpenDevice with invalid access mode = DEVICE_ACCESS_EXCLUSIVE+1");
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

            GC_ERROR Result=GC_ERR_SUCCESS;
            DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
        
            Result = m_ModIF.eIFOpenDevice(hIF, sDeviceID.c_str(), DEVICE_ACCESS_EXCLUSIVE+1, &hDev);
            GENTLTEST_CHECK_RESULT("Note: IFOpenDevice return value: Expected == GC_ERR_INVALID_PARAMETER or == GC_ERR_NOT_IMPLEMENTED or == GC_ERR_ACCESS_DENIED, received " << sConvertGCError2String(Result).c_str(), 
                Result == GC_ERR_INVALID_PARAMETER || Result == GC_ERR_NOT_IMPLEMENTED || Result == GC_ERR_ACCESS_DENIED,
                "IFOpenDevice failed. Expected < GC_ERR_SUCCESS, received " << sConvertGCError2String(Result).c_str(), 
                Result < GC_ERR_SUCCESS);
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

/////////////////////////////////////////////////////////////////////
// helper
/////////////////////////////////////////////////////////////////////

void Interface_IFOpenDevice::vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList)
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
            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_READONLY; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                std::string sDeviceID;
                stIFDevice ifdev;

                sDeviceID = oIFPreCondition.sGetDeviceID(hIF, index2);
                ifdev.sInterfaceID = xInterfaceList[index1];
                ifdev.sDeviceID = sDeviceID;
                ifdev.eAccess = eAccess;
                vIFDeviceList.push_back(ifdev);
            }
        }
    }

    //GENTLTEST_PRINT("Interface_IFOpenDevice::vCreateTestCaseList %d testcases created\n", vIFDeviceList.size());
}
