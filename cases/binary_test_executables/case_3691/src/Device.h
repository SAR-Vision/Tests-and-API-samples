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

#ifndef DEVICE_H
#define DEVICE_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "DeviceTest.h"
#include "Device_DevGetInfo.h"
#include "Device_DevGetNumDataStreams.h"
#include "Device_DevGetDataStreamID.h"
#include "Device_DevGetPort.h"
#include "Device_DevOpenDataStream.h"
#include "Device_DevGetParentIF.h"

GENTLTEST_UNIT_DECL(DeviceTest, TestDevClose);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseWithLibraryClosed);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseWithoutTLOpen);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseWithoutInterfaceOpen);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseDouble);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseReopen);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseWithHandleDevNull);
GENTLTEST_UNIT_DECL(DeviceTest, TestDevCloseWithHandleDevWrong);

GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfo);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithOldHandle);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithSizeNull);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithTypeNull);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoWithDevIDNull);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoBufferWithSizeNull);
GENTLTEST_UNIT_DECL(Device_DevGetInfo, TestDevGetInfoBufferWithSizeLow);

GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreams);
GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutTLOpen);
GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutIFOpen);
GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithOldHandle);
GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithNumNull);
GENTLTEST_UNIT_DECL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithDevHandleNull);

GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamID);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutTLOpen);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutIFOpen);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithOldHandle);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithSizeNull);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithDevHandleNull);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithDevGreaterIndex);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDBufferWithoutSize);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDBufferWithSizeNull);
GENTLTEST_UNIT_DECL(Device_DevGetDataStreamID, TestDevGetDataStreamIDPersistence);

GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPort);
GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPortWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPortWithoutTLOpen);
GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPortWithoutIFOpen);
GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPortWithOldHandle);
GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPortWithDevIDNull);
GENTLTEST_UNIT_DECL(Device_DevGetPort, TestDevGetPortWithPortHandleNull);

GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStream);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutTLOpen);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutIFOpen);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithOldHandle);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithDevHandleNull);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithDSHandleNull);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamWithDataStreamIDNull);
GENTLTEST_UNIT_DECL(Device_DevOpenDataStream, TestDevOpenDataStreamDoubleOpen);

GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIF);
GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIFWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIFWithoutTLOpen);
GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIFWithoutIFOpen);
GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIFWithOldHandle);
GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIFWithDevNULL);
GENTLTEST_UNIT_DECL(Device_DevGetParentIF, TestDevGetParentIFWithIFNULL);

#endif  //DEVICE_H
