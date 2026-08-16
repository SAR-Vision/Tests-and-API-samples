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

#ifndef SIGNALING_H
#define SIGNALING_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "Signaling_GCRegisterEvent.h"
#include "Signaling_TLParamsLocked.h"
#include "Signaling_StartDevice.h"
#include "Signaling_StopDevice.h"
#include "Signaling_EventGetData.h"
#include "Signaling_EventGetInfo.h"
#include "Signaling_GCUnregisterEvent.h"
#include "Signaling_EventGetDataInfo.h"
#include "Signaling_EventKill.h"
#include "Signaling_EventFlush.h"

GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventSystem);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventInterface);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventDevice);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventDataStream);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithDSIDNull);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithWrongEventID);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventWithEventHandleNULL);
GENTLTEST_UNIT_DECL(Signaling_GCRegisterEvent, TestGCRegisterEventTwice);

GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventSystem);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventInterface);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventDevice);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventDataStream);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithDSIDNull);
GENTLTEST_UNIT_DECL(Signaling_GCUnregisterEvent, TestGCUnregisterEventWithWrongEventID);

GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfo);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithoutRegisterEvent);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithEventHandleNull);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithInvalidCommand);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithTypeNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoWithSizeNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(Signaling_EventGetInfo, TestEventGetInfoBufferWithSizeNull);

GENTLTEST_UNIT_DECL(Signaling_TLParamsLocked, TestTLParamsLocked);

GENTLTEST_UNIT_DECL(Signaling_StartDevice, TestStartDevice);
//GENTLTEST_UNIT_DECL(Signaling_StartDevice, TestStartDeviceReadOnly);

GENTLTEST_UNIT_DECL(Signaling_StopDevice, TestStopDevice);

GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataConsumerAlloc);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataProducerAlloc);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithoutRegisterEvent);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithEventHandleNull);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithBufferNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithBufferSizeNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithBufferSizeLow);
GENTLTEST_UNIT_DECL(Signaling_EventGetData, TestEventGetDataWithTimeout0);

GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfo);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithoutRegisterEvent);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithEventHandleNull);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithInvalidCommand);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithTypeNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithSizeNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoBufferWithSizeNull);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferNULL);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferSize0);
GENTLTEST_UNIT_DECL(Signaling_EventGetDataInfo, TestEventGetDataInfoWithDataBufferSizeLow);

//GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKill);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithoutRegisterEvent);
GENTLTEST_UNIT_DECL(Signaling_EventKill, TestEventKillWithEventHandleNull);

GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlush);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithoutTLOpen);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithoutIFOpen);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithoutDevOpen);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithoutDSOpen);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithoutRegisterEvent);
GENTLTEST_UNIT_DECL(Signaling_EventFlush, TestEventFlushWithEventHandleNull);

#endif  //SIGNALING_H
