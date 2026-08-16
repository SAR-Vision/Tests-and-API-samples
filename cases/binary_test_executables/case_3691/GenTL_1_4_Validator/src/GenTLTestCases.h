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

#ifndef GENTLTESTCASES_INCLUDE___
#define GENTLTESTCASES_INCLUDE___

#include <string>
#include "GenTL_v1_4.h"
#include <basetsd.h>

#include "GenApi/GenApi.h"

/////////////////////////////////////////////////////////////////////////////////////////
// tests forward declaration section
/////////////////////////////////////////////////////////////////////////////////////////

void vTestFnExportTestWrapper(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestLibrary_GetTLInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestLibraryTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestLibrary_GCGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystemTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystem_TLUpdateInterface(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystem_TLGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystem_TLGetNumInterfaces(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystem_TLGetInterfaceID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystem_TLOpenInterface(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSystem_TLGetInterfaceInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterfaceTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFUpdateDeviceList(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFGetNumDevices(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFGetDeviceID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFOpenDevice(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFGetDeviceInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestInterface_IFGetParentTL(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDeviceTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDevice_DevGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDevice_DevGetNumDataStreams(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDevice_DevGetDataStreamID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDevice_DevGetPort(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDevice_DevOpenDataStream(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDevice_DevGetParentIF(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCGetPortInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCGetPortURL(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCGetNumPortURLs(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCGetPortURLInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCReadPort(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCWritePort(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCReadPortstacked(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestPort_GCWritePortstacked(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStreamTest(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSAnnounceBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSQueueBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSGetBufferInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSGetBufferID(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSStartAcquisition(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSStopAcquisition(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSAllocAndAnnounceBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSFlushQueue(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSRevokeBuffer(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSGetBufferChunkData(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestDataStream_DSGetParentDev(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_GCRegisterEvent(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_GCUnregisterEvent(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_EventGetInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_TLParamsLocked(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_StartDevice(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_StopDevice(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_EventGetData(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_EventGetDataInfo(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_EventKill(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestSignaling_EventFlush(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);
void vTestImpl_Complete(int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo);


#endif  /* GENTLTESTCASES_INCLUDE___ */
