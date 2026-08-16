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

#include "Device.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

GENTLTEST_UNIT_IMPL_MSG(DeviceTest, TestDevClose, "DeviceTest");
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseWithLibraryClosed);
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseWithoutTLOpen);
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseWithoutInterfaceOpen);
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseDouble);
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseReopen);
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseWithHandleDevNull);
GENTLTEST_UNIT_IMPL(DeviceTest, TestDevCloseWithHandleDevWrong);

GENTLTEST_UNIT_IMPL_MSG(Device_DevGetInfo, TestDevGetInfo, "Device_DevGetInfo");
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithOldHandle);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithSizeNull);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithTypeNull);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoWithDevIDNull);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoBufferWithoutSize);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoBufferWithSizeNull);
GENTLTEST_UNIT_IMPL(Device_DevGetInfo, TestDevGetInfoBufferWithSizeLow);

GENTLTEST_UNIT_IMPL_MSG(Device_DevGetNumDataStreams, TestDevGetNumDataStreams, "Device_DevGetNumDataStreams");
GENTLTEST_UNIT_IMPL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithOldHandle);
GENTLTEST_UNIT_IMPL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithNumNull);
GENTLTEST_UNIT_IMPL(Device_DevGetNumDataStreams, TestDevGetNumDataStreamsWithDevHandleNull);

GENTLTEST_UNIT_IMPL_MSG(Device_DevGetDataStreamID, TestDevGetDataStreamID, "Device_DevGetDataStreamID");
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithOldHandle);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithSizeNull);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithDevHandleNull);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDWithDevGreaterIndex);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDBufferWithoutSize);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDBufferWithSizeNull);
GENTLTEST_UNIT_IMPL(Device_DevGetDataStreamID, TestDevGetDataStreamIDPersistence);

GENTLTEST_UNIT_IMPL_MSG(Device_DevGetPort, TestDevGetPort, "Device_DevGetPort");
GENTLTEST_UNIT_IMPL(Device_DevGetPort, TestDevGetPortWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Device_DevGetPort, TestDevGetPortWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetPort, TestDevGetPortWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetPort, TestDevGetPortWithOldHandle);
GENTLTEST_UNIT_IMPL(Device_DevGetPort, TestDevGetPortWithDevIDNull);
GENTLTEST_UNIT_IMPL(Device_DevGetPort, TestDevGetPortWithPortHandleNull);

GENTLTEST_UNIT_IMPL_MSG(Device_DevOpenDataStream, TestDevOpenDataStream, "Device_DevOpenDataStream");
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithOldHandle);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithDevHandleNull);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithDSHandleNull);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamWithDataStreamIDNull);
GENTLTEST_UNIT_IMPL(Device_DevOpenDataStream, TestDevOpenDataStreamDoubleOpen);

GENTLTEST_UNIT_IMPL_MSG(Device_DevGetParentIF, TestDevGetParentIF, "Device_DevGetParentIF");
GENTLTEST_UNIT_IMPL(Device_DevGetParentIF, TestDevGetParentIFWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Device_DevGetParentIF, TestDevGetParentIFWithoutTLOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetParentIF, TestDevGetParentIFWithoutIFOpen);
GENTLTEST_UNIT_IMPL(Device_DevGetParentIF, TestDevGetParentIFWithOldHandle);
GENTLTEST_UNIT_IMPL(Device_DevGetParentIF, TestDevGetParentIFWithDevNULL);
GENTLTEST_UNIT_IMPL(Device_DevGetParentIF, TestDevGetParentIFWithIFNULL);
