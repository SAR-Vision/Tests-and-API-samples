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

#include "Signaling.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

GENTLTEST_UNIT_IMPL_MSG(Signaling_GCRegisterEvent, TestGCRegisterEventSystem, "Signaling_GCRegisterEvent");
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventInterface);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventDevice);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventDataStream);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithDSIDNull);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithWrongEventID);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventWithEventHandleNULL);
GENTLTEST_UNIT_IMPL(Signaling_GCRegisterEvent, TestGCRegisterEventTwice);

GENTLTEST_UNIT_IMPL_MSG(Signaling_GCUnregisterEvent, TestGCUnregisterEventSystem, "Signaling_GCUnregisterEvent");
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventInterface);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventDevice);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventDataStream);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithDSIDNull);
GENTLTEST_UNIT_IMPL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithWrongEventID);

GENTLTEST_UNIT_IMPL_MSG(Signaling_EventGetInfo, TestEventGetInfo, "Signaling_EventGetInfo");
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithoutRegisterEvent);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithEventHandleNull);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithInvalidCommand);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithTypeNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoWithSizeNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoBufferWithoutSize);
GENTLTEST_UNIT_IMPL(Signaling_EventGetInfo, TestEventGetInfoBufferWithSizeNull);

GENTLTEST_UNIT_IMPL_MSG(Signaling_TLParamsLocked, TestTLParamsLocked, "Signaling_TLParamsLocked");

GENTLTEST_UNIT_IMPL_MSG(Signaling_StartDevice, TestStartDevice, "Signaling_StartDevice");
//GENTLTEST_UNIT_IMPL(Signaling_StartDevice, TestStartDeviceReadOnly);

GENTLTEST_UNIT_IMPL_MSG(Signaling_StopDevice, TestStopDevice, "Signaling_StopDevice");

GENTLTEST_UNIT_IMPL_MSG(Signaling_EventGetData, TestEventGetDataConsumerAlloc, "Signaling_EventGetData");
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataProducerAlloc);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithoutRegisterEvent);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithEventHandleNull);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithBufferNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithBufferSizeNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithBufferSizeLow);
GENTLTEST_UNIT_IMPL(Signaling_EventGetData, TestEventGetDataWithTimeout0);

GENTLTEST_UNIT_IMPL_MSG(Signaling_EventGetDataInfo, TestEventGetDataInfo, "Signaling_EventGetDataInfo");
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutRegisterEvent);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithEventHandleNull);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithInvalidCommand);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithTypeNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithSizeNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoBufferWithoutSize);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoBufferWithSizeNull);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferNULL);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferSize0);
GENTLTEST_UNIT_IMPL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferSizeLow);
    
//GENTLTEST_UNIT_IMPL_MSG(Signaling_EventKill, TestEventKill, "Signaling_EventKill");
GENTLTEST_UNIT_IMPL_MSG(Signaling_EventKill, TestEventKillWithoutGCInitLib, "Signaling_EventKill");
GENTLTEST_UNIT_IMPL(Signaling_EventKill, TestEventKillWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventKill, TestEventKillWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventKill, TestEventKillWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventKill, TestEventKillWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventKill, TestEventKillWithoutRegisterEvent);
GENTLTEST_UNIT_IMPL(Signaling_EventKill, TestEventKillWithEventHandleNull);

GENTLTEST_UNIT_IMPL_MSG(Signaling_EventFlush, TestEventFlush, "Signaling_EventFlush");
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithoutDevOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithoutDSOpen);
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithoutRegisterEvent);
GENTLTEST_UNIT_IMPL(Signaling_EventFlush, TestEventFlushWithEventHandleNull);
