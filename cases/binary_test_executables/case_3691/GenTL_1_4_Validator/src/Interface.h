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

#ifndef INTERFACE_H
#define INTERFACE_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "InterfaceTest.h"
#include "Interface_IFGetInfo.h"
#include "Interface_IFUpdateDeviceList.h"
#include "Interface_IFGetNumDevices.h"
#include "Interface_IFGetDeviceID.h"
#include "Interface_IFOpenDevice.h"
#include "Interface_IFGetDeviceInfo.h"
#include "Interface_IFGetParentTL.h"

GENTLTEST_UNIT_DECL(InterfaceTest, TestIFClose);
GENTLTEST_UNIT_DECL(InterfaceTest, TestIFCloseWithLibraryClosed);
GENTLTEST_UNIT_DECL(InterfaceTest, TestIFCloseWithSystemClosed);
GENTLTEST_UNIT_DECL(InterfaceTest, TestIFCloseDouble);
GENTLTEST_UNIT_DECL(InterfaceTest, TestIFCloseReopen);
GENTLTEST_UNIT_DECL(InterfaceTest, TestIFCloseWithHandleIFNull);
GENTLTEST_UNIT_DECL(InterfaceTest, TestIFCloseWithHandleIFWrong);

GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfo);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithHandleNull);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithInvalidCommand);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithSizeNull);
GENTLTEST_UNIT_DECL(Interface_IFGetInfo, TestIFGetInfoWithTypeNULL);

GENTLTEST_UNIT_DECL(Interface_IFUpdateDeviceList, TestIFUpdateDeviceList);
GENTLTEST_UNIT_DECL(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithHandleNull);
GENTLTEST_UNIT_DECL(Interface_IFUpdateDeviceList, TestIFUpdateDeviceListWithChangedNull);

GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevices);
GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevicesWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevicesWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevicesWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevicesWithHandleNull);
GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevicesWithNumbersNull);
GENTLTEST_UNIT_DECL(Interface_IFGetNumDevices, TestIFGetNumDevicesWithoutUpdate);

GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceID);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithIfHandleNULL);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithSizeNULL1);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithSizeNULL2);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithInvalidIndex);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDWithSizeLow);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceID, TestIFGetDeviceIDPersistence);

GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDevice);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithPublicDeviceID);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithIFNULL);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithDevIDNULL);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithWrongDevID);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceDoubleOpen);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceControlOpen);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceExclusiveOpen);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithDevHandleNULL);
GENTLTEST_UNIT_DECL(Interface_IFOpenDevice, TestIFOpenDeviceWithInvalidAccessMode);

GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfo);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithSizeNull);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithTypeNull);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithIFNull);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithDevIDNull);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoWithWrongDevID);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(Interface_IFGetDeviceInfo, TestIFGetDeviceInfoBufferWithSizeNull);

GENTLTEST_UNIT_DECL(Interface_IFGetParentTL, TestIFGetParentTL);
GENTLTEST_UNIT_DECL(Interface_IFGetParentTL, TestIFGetParentTLWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Interface_IFGetParentTL, TestIFGetParentTLWithoutTLOpen);
GENTLTEST_UNIT_DECL(Interface_IFGetParentTL, TestIFGetParentTLWithOldHandle);
GENTLTEST_UNIT_DECL(Interface_IFGetParentTL, TestIFGetParentTLWithIFNULL);
GENTLTEST_UNIT_DECL(Interface_IFGetParentTL, TestIFGetParentTLWithSystemNULL);

#endif  //INTERFACE_H
