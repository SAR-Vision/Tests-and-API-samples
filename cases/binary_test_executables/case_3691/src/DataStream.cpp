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

#include "DataStream.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

GENTLTEST_UNIT_IMPL_MSG(DataStreamTest, TestDataStream, "DataStreamTest");
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamWithHandleNull);
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamDoubleClose);
GENTLTEST_UNIT_IMPL(DataStreamTest, TestDataStreamReopen);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSGetInfo, TestDSGetInfo, "DataStream_DSGetInfo");
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithTypeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoBufferWithoutSize);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoBufferWithSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetInfo, TestDSGetInfoWithInvalidCommand);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSAnnounceBuffer, TestDSAnnounceBuffer, "DataStream_DSAnnounceBuffer");
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBuffer2);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithPayloadSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithPayloadNull);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferPrivatedataNULL);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferWithBufferHandleNull);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferTwice);
GENTLTEST_UNIT_IMPL(DataStream_DSAnnounceBuffer, TestDSAnnounceBufferTwiceToDifferentStreams);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSQueueBuffer, TestDSQueueBuffer, "DataStream_DSQueueBuffer");
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSQueueBuffer, TestDSQueueBufferBufferHandleNULL);
    
GENTLTEST_UNIT_IMPL_MSG(DataStream_DSGetBufferInfo, TestDSGetBufferInfoConsumerBuffer, "DataStream_DSGetBufferInfo");
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoProducerBuffer);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithTypeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoBufferWithoutSize);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoBufferWithSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferInfo, TestDSGetBufferInfoWithInvalidCommand);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSGetBufferID, TestDSGetBufferID, "DataStream_DSGetBufferID");
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithBufferHandleNULL);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferID, TestDSGetBufferIDWithInvalidIndex);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSStartAcquisition, TestDSStartAcquisition, "DataStream_DSStartAcquisition");
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithNumToAcquireNull);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithWrongStartFlag);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWith10Frames);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithSmallBuffer);
GENTLTEST_UNIT_IMPL(DataStream_DSStartAcquisition, TestDSStartAcquisitionWithNoBuffer);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSStopAcquisition, TestDSStopAcquisition, "DataStream_DSStopAcquisition");
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSStopAcquisition, TestDSStopAcquisitionWithWrongStopFlag);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBuffer, "DataStream_DSAllocAndAnnounceBuffer");
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBuffer2);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithPayloadSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferPrivatedataNULL);
GENTLTEST_UNIT_IMPL(DataStream_DSAllocAndAnnounceBuffer, TestDSAllocAndAnnounceBufferWithBufferHandleNull);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSRevokeBuffer, TestDSRevokeBuffer, "DataStream_DSRevokeBuffer");
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBuffer2);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferQueued);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithPrivatedata);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithBufferPointer);
GENTLTEST_UNIT_IMPL(DataStream_DSRevokeBuffer, TestDSRevokeBufferWithBufferHandleNull);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSFlushQueue, TestDSFlushQueue, "DataStream_DSFlushQueue");
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithDSIDNull);
GENTLTEST_UNIT_IMPL(DataStream_DSFlushQueue, TestDSFlushQueueWithWrongOperationFlag);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutGCInitLib, "DataStream_DSGetBufferChunkData");
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutDevOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithSizeNull);
GENTLTEST_UNIT_IMPL(DataStream_DSGetBufferChunkData, TestDSGetBufferChunkDataWithDSIDNull);

GENTLTEST_UNIT_IMPL_MSG(DataStream_DSGetParentDev, TestDSGetParentDev, "DataStream_DSGetParentDev");
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutIFOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithOldHandle);
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithoutDSOpen);
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithDSNULL);
GENTLTEST_UNIT_IMPL(DataStream_DSGetParentDev, TestDSGetParentDevWithDevNULL);
