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

#include "Signaling_StopDevice.h"
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

Signaling_StopDevice::Signaling_StopDevice()
{
}

Signaling_StopDevice::~Signaling_StopDevice()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPP unit test leading and trailing functions
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_StopDevice::setUp(void)
{
}

void Signaling_StopDevice::tearDown(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
// Test StopDevice
////////////////////////////////////////////////////////////////////////////////////////////

void Signaling_StopDevice::TestStopDevice( uint32_t test_id )
{
    GENTLTEST_DESCRIPTION(test_id, "Test standard stop the acquisition inside the remote device");
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

            GENTLTEST_PRINT("Info: DeviceID = " << sDeviceID.c_str() << std::endl);

            for (DEVICE_ACCESS_FLAGS_LIST eAccess=DEVICE_ACCESS_CONTROL; eAccess<=DEVICE_ACCESS_EXCLUSIVE; eAccess=(DEVICE_ACCESS_FLAGS_LIST)(eAccess+1))
            {
                DEV_HANDLE hDev=GENTL_INVALID_HANDLE;
                Device_PreCondition oDevPreCondition(hIF, sDeviceID, eAccess, &hDev);

                if (oDevPreCondition.eGetLastResult() >= GC_ERR_SUCCESS)
                {
                    m_oParameter.bPrepareRemoteDeviceXML(oLibSysSetup, oDevPreCondition.hDevGetPort());
                
                    uint32_t uiNumDataStreams=oDevPreCondition.uiGetNumDataStreams();
                    
                    for (uint32_t index3=0; index3<uiNumDataStreams; index3++)
                    {
                        DS_HANDLE hDs=GENTL_INVALID_HANDLE;
                        BUFFER_HANDLE hBuffer=GENTL_INVALID_HANDLE;
                        uint32_t nBufferCount=10;
                        EVENT_HANDLE hEvent=GENTL_INVALID_HANDLE;
                        std::string sDataStreamID=oDevPreCondition.sGetDataStreamID(hDev, index3);
                        DataStream_PreCondition oDSPreCondition(hDev, sDataStreamID, &hDs);
                        size_t uiPayLoadSize=oDSPreCondition.uiGetPayLoadSize(oLibSysSetup, hDs, oDevPreCondition.hDevGetPort());
                        Signaling_PreCondition oSigPreCondition;
                        
                        for (uint32_t i=0; i<nBufferCount; i++)
                        {
                            oDSPreCondition.eDSAnnounceBufferAndQueue(hDs, uiPayLoadSize, (void *)this, &hBuffer);
                        }

                        oDSPreCondition.eLockParameter(m_oParameter, true);
                        oDSPreCondition.eDSStartAcquisition(hDs, GENTL_INFINITE);
                        oSigPreCondition.eGCRegisterEvent(hDs, EVENT_NEW_BUFFER, &hEvent);
                        oSigPreCondition.eStartDevice(m_oParameter);

                        GenApi::CCommandPtr oStopAcq = m_oParameter.xGetXMLNodeCommand("AcquisitionStop");
                        GENTLTEST_CHECK_MESSAGE("Device AcquisitionStop not valid", oStopAcq.IsValid());
                        try 
                        {
                            oStopAcq->Execute();
                            GENTLTEST_CHECK(true);
                            GENTLTEST_PRINT("Info: device access=" << sConvertDEVICEAccess2String(eAccess).c_str() << " DeviceID=" << sDeviceID.c_str() << 
                                    " DataStreamID=" << sDataStreamID.c_str() << " node='AcquisitionStop', successful executed." << std::endl);
                        }
                        catch (GenICam::AccessException ex)
                        {
                            GENTLTEST_CHECK_MESSAGE("Error: access problem node='AcquisitionStop', details=" << ex.GetDescription() << std::endl, false);
                        }
                    }
                }
            }
        }
    }

    GENTLTEST_PRINT_RESULT(test_id);
}

