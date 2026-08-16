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

#ifndef FNEXPORT_H
#define FNEXPORT_H

#include "GenTL_v1_4.h"

#include "GenTLTesttools.h"

class FnExportTestWrapper;

void vFnExportTest( void );

void vFnExportTestGCGetInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCGetLastError( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCInitLib( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCCloseLib( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCReadPort( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCWritePort( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCGetPortURL( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCGetNumPortURLs( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCGetPortInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCGetPortURLInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCReadPortStacked( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCWritePortStacked( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCRegisterEvent( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestGCUnregisterEvent( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestEventGetData( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestEventGetDataInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestEventGetInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestEventFlush( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestEventKill( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLOpen( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLClose( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLGetInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLGetNumInterfaces( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLGetInterfaceID( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLGetInterfaceInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLOpenInterface( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestTLUpdateInterfaceList( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFClose( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFGetInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFGetNumDevices( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFGetDeviceID( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFUpdateDeviceList( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFGetDeviceInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFOpenDevice( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestIFGetParentTL( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevGetPort( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevGetNumDataStreams( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevGetDataStreamID( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevOpenDataStream( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevGetInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevClose( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDevGetParentIF( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSAnnounceBuffer( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSAllocAndAnnounceBuffer( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSFlushQueue( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSStartAcquisition( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSStopAcquisition( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSGetInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSGetBufferID( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSClose( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSRevokeBuffer( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSQueueBuffer( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSGetBufferInfo( FnExportTestWrapper &oWrapper, uint32_t id );
void vFnExportTestDSGetBufferChunkData(FnExportTestWrapper &oWrapper, uint32_t id);
void vFnExportTestDSGetParentDev( FnExportTestWrapper &oWrapper, uint32_t id );

#endif  //FNEXPORT_H
