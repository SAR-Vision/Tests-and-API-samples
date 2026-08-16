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

#ifndef DATASTREAM_H
#define DATASTREAM_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"
#include "DataStreamTest.h"
#include "DataStream_DSGetInfo.h"
#include "DataStream_DSAnnounceBuffer.h"
#include "DataStream_DSQueueBuffer.h"
#include "DataStream_DSGetBufferInfo.h"
#include "DataStream_DSGetBufferID.h"
#include "DataStream_DSStartAcquisition.h"
#include "DataStream_DSStopAcquisition.h"
#include "DataStream_DSAllocAndAnnounceBuffer.h"
#include "DataStream_DSRevokeBuffer.h"
#include "DataStream_DSFlushQueue.h"
#include "DataStream_DSGetBufferChunkData.h"
#include "DataStream_DSGetParentDev.h"

GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStream);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamWithHandleNull);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamDoubleClose);
GENTLTEST_UNIT_DECL(DataStreamTest, TestDataStreamReopen);

GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfo);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithTypeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoBufferWithSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetInfo, TestDSGetInfoWithInvalidCommand);

GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBuffer2);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithPayloadSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithPayloadNull);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferPrivatedataNULL);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithBufferHandleNull);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferTwice);
GENTLTEST_UNIT_DECL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferTwiceToDifferentStreams);

GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSQueueBuffer, TestDSQueueBufferBufferHandleNULL);

GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoConsumerBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoProducerBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithTypeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoBufferWithSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithInvalidCommand);

GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferID);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithBufferHandleNULL);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferID, TestDSGetBufferIDWithInvalidIndex);

GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisition);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithNumToAcquireNull);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithWrongStartFlag);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWith10Frames);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithSmallBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithNoBuffer);

GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisition);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithWrongStopFlag);

GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBuffer2);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithPayloadSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferPrivatedataNULL);
GENTLTEST_UNIT_DECL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithBufferHandleNull);

GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBuffer);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBuffer2);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferQueued);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithPrivatedata);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithBufferPointer);
GENTLTEST_UNIT_DECL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithBufferHandleNull);

GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueue);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithDSIDNull);
GENTLTEST_UNIT_DECL(DataStream_DSFlushQueue, TestDSFlushQueueWithWrongOperationFlag);

GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutDevOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithSizeNull);
GENTLTEST_UNIT_DECL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithDSIDNull);

GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDev);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutGCInitLib);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutTLOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutIFOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithOldHandle);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutDSOpen);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithDSNULL);
GENTLTEST_UNIT_DECL(DataStream_DSGetParentDev, TestDSGetParentDevWithDevNULL);

#endif  //DATASTREAM_H
