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
#include <map>

#include "GenApi/GenApi.h"

#include "Signaling_TLParamsLocked.h"
#include "LibrarySystemSetup.h"
#include "Interface_PreCondition.h"
#include "Device_PreCondition.h"
#include "DataStream_PreCondition.h"
#include "Signaling_PreCondition.h"
#include "GenTLTestTools.h"

using namespace GenICam;
using namespace GenICam::Client;

////////////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
////////////////////////////////////////////////////////////////////////////////////////////

Signaling_TLParamsLocked::Signaling_TLParamsLocked()
{
}

Signaling_TLParamsLocked::~Signaling_TLParamsLocked()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_TLParamsLocked::setUp(void)
{
}

void Signaling_TLParamsLocked::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test StartDevice
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_TLParamsLocked::TestTLParamsLocked( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard remote device feature 'TLParamsLocked'");
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
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                
                    GenApi::CIntegerPtr oTLParamsLocked1 = m_oParameter.xGetXMLNodeInteger("TLParamsLocked");
                    if (oTLParamsLocked1.IsValid())
                    {
                        try 
                        {
                            GENTLTEST_PRINT("Info: try to set IInteger node TLParamsLocked to 1" << std::endl);
                            oTLParamsLocked1->SetValue(1);
                            GENTLTEST_PRINT("Info: set IInteger node TLParamsLocked to 1 without exception." << std::endl);
                        }
                        catch (GenICam::AccessException ex)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Error: lock access problem node='TLParamsLocked', details=" << ex.GetDescription() << std::endl, false);
                        }

                        try 
                        {
                            GENTLTEST_PRINT("Info: try to set IInteger node TLParamsLocked to 0" << std::endl);
                            oTLParamsLocked1->SetValue(0);
                            GENTLTEST_PRINT("Info: set IInteger node TLParamsLocked to 0 without exception." << std::endl);
                        }
                        catch (GenICam::AccessException ex)
                        {
                            GENTLTEST_REQUIRE_MESSAGE("Error: unlock access problem node='TLParamsLocked', details=" << ex.GetDescription() << std::endl, false);
                        }
                    }
                    else
                    {
                        GenApi::CBooleanPtr oTLParamsLocked2 = m_oParameter.xGetXMLNodeBoolean("TLParamsLocked");
                        if (oTLParamsLocked2.IsValid())
                        {
                            try 
                            {
                                GENTLTEST_PRINT("Info: try to set IBoolean node TLParamsLocked to true" << std::endl);
                                oTLParamsLocked2->SetValue(1);
                                GENTLTEST_PRINT("Info: set IBoolean node TLParamsLocked to true without exception." << std::endl);
                            }
                            catch (GenICam::AccessException ex)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Error: access problem node='TLParamsLocked', details=" << ex.GetDescription() << std::endl, false);
                            }
                            try 
                            {
                                GENTLTEST_PRINT("Info: try to set IBoolean node TLParamsLocked to false" << std::endl);
                                oTLParamsLocked2->SetValue(0);
                                GENTLTEST_PRINT("Info: set IBoolean node TLParamsLocked to false without exception." << std::endl);
                            }
                            catch (GenICam::AccessException ex)
                            {
                                GENTLTEST_REQUIRE_MESSAGE("Error: access problem node='TLParamsLocked', details=" << ex.GetDescription() << std::endl, false);
                            }
                        }
                        else
                            GENTLTEST_REQUIRE_MESSAGE("Device TLParamsLocked (IInteger) not rechable.", false);
                    }

                    
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

