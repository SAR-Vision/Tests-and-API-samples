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

#ifndef PORT_H
#define PORT_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "Port_GCGetPortInfo.h"
#include "Port_GCGetPortURL.h"
#include "Port_GCGetNumPortURLs.h"
#include "Port_GCGetPortURLInfo.h"
#include "Port_GCReadPort.h"
#include "Port_GCWritePort.h"
#include "Port_GCReadPortStacked.h"
#include "Port_GCWritePortStacked.h"

GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoSystem);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoInterface);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoDevice);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoRemoteDevice);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoDataStream);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoBuffer);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithOldHandle);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithPortHandleNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithInvalidCommand);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithTypeNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithSizeNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithSizeLow);
GENTLTEST_UNIT_DECL(Port_GCGetPortInfo, TestGCGetPortInfoWithBufferSizeNULL);

GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURL);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithoutTLOpen);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithoutIFOpen);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithOldHandle);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithPortHandleNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithSizeNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortURL, TestGCGetPortURLWithSizeLow);

GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLs);
GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithoutTLOpen);
GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithoutIFOpen);
GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithOldHandle);
GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLsPortHandleNull);
GENTLTEST_UNIT_DECL(Port_GCGetNumPortURLs, TestGCGetNumPortURLsWithNumURLsNull);

GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfo);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithoutTLOpen);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithoutIFOpen);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithOldHandle);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoPortHandleNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithSizeNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithTypeNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoBufferWithoutSize);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoBufferWithSizeNull);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoBufferWithSizeLow);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithInvalidCommand);
GENTLTEST_UNIT_DECL(Port_GCGetPortURLInfo, TestGCGetPortURLInfoWithInvalidIndex);

GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortSystem);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortSystemEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortSystemMandatoryEntries);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortInterface);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortInterfaceEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortInterfaceMandatoryEntries);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortDevice);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortDeviceEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortDeviceMandatoryEntries);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortRemoteDevice);
//GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortRemoteDeviceEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortRemoteDeviceMandatoryEntries);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortDatastream);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortDatastreamEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortDatastreamMandatoryEntries);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortBufferMandatoryEntries);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithoutTLOpen);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithoutIFOpen);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithOldHandle);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithSizeNull);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithSizeLow);
GENTLTEST_UNIT_DECL(Port_GCReadPort, TestGCReadPortWithBufferNull);

GENTLTEST_UNIT_DECL(Port_GCWritePort, TestGCWritePort);
GENTLTEST_UNIT_DECL(Port_GCWritePort, TestGCWritePortWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCWritePort, TestGCWritePortWithoutTLOpen);

GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedSystemEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedInterfaceEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedDeviceEntry);
//GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedRemoteDeviceEntry);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedWithoutTLOpen);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedWithoutIFOpen);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedWithOldHandle);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedWithNumentriesNull);
GENTLTEST_UNIT_DECL(Port_GCReadPortStacked, TestGCReadPortStackedWithEntriesNull);

GENTLTEST_UNIT_DECL(Port_GCWritePortStacked, TestGCWritePortStacked);
GENTLTEST_UNIT_DECL(Port_GCWritePortStacked, TestGCWritePortStackedWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Port_GCWritePortStacked, TestGCWritePortStackedWithoutTLOpen);

#endif  //PORT_H
