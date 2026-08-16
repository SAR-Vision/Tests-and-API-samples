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

#ifndef FNEXPORTTEST_H
#define FNEXPORTTEST_H

#include "GenTLTest_Win.h"

#include "GenTL_v1_4.h"

class FnExportTest
{
public:
    virtual ~FnExportTest();
  
    static FnExportTest *poGetInstance();
    static void vExitInstance();
  
public:
    void setUp (void);
    void tearDown (void);

public:
    hModule hGetModuleHandle();
    void *pGCGetInfo( void );
    void *pGCGetLastError( void );
    void *pGCInitLib( void );
    void *pGCCloseLib( void );
    void *pGCReadPort( void );
    void *pGCWritePort( void );
    void *pGCGetPortURL( void );
    void *pGCGetNumPortURLs( void );
    void *pGCGetPortInfo( void );
    void *pGCGetPortURLInfo( void );
    void *pGCReadPortStacked( void );
    void *pGCWritePortStacked( void );
    void *pGCRegisterEvent( void );
    void *pGCUnregisterEvent( void );
    void *pEventGetData( void );
    void *pEventGetDataInfo( void );
    void *pEventGetInfo( void );
    void *pEventFlush( void );
    void *pEventKill( void );
    void *pTLOpen( void );
    void *pTLClose( void );
    void *pTLGetInfo( void );
    void *pTLGetNumInterfaces( void );
    void *pTLGetInterfaceID( void );
    void *pTLGetInterfaceInfo( void );
    void *pTLOpenInterface( void );
    void *pTLUpdateInterfaceList( void );
    void *pIFClose( void );
    void *pIFGetInfo( void );
    void *pIFGetNumDevices( void );
    void *pIFGetDeviceID( void );
    void *pIFUpdateDeviceList( void );
    void *pIFGetDeviceInfo( void );
    void *pIFOpenDevice( void );
    void *pIFGetParentTL( void );
    void *pDevGetPort( void );
    void *pDevGetNumDataStreams( void );
    void *pDevGetDataStreamID( void );
    void *pDevOpenDataStream( void );
    void *pDevGetInfo( void );
    void *pDevClose( void );
    void *pDevGetParentIF( void );
    void *pDSAnnounceBuffer( void );
    void *pDSAllocAndAnnounceBuffer( void );
    void *pDSFlushQueue( void );
    void *pDSStartAcquisition( void );
    void *pDSStopAcquisition( void );
    void *pDSGetInfo( void );
    void *pDSGetBufferID( void );
    void *pDSClose( void );
    void *pDSRevokeBuffer( void );
    void *pDSQueueBuffer( void );
    void *pDSGetBufferInfo( void );
    void *pDSGetBufferChunkData( void );
    void *pDSGetParentDev( void );

private:
    FnExportTest( void );
  
private:
    hModule  m_hModule;
    static FnExportTest *s_poInstance;

    void *m_pGCGetInfo;
    void *m_pGCGetLastError;
    void *m_pGCInitLib;
    void *m_pGCCloseLib;
    void *m_pGCReadPort;
    void *m_pGCWritePort;
    void *m_pGCGetPortURL;
    void *m_pGCGetNumPortURLs;
    void *m_pGCGetPortInfo;
    void *m_pGCGetPortURLInfo;
    void *m_pGCReadPortStacked;
    void *m_pGCWritePortStacked;
    void *m_pGCRegisterEvent;
    void *m_pGCUnregisterEvent;
    void *m_pEventGetData;
    void *m_pEventGetDataInfo;
    void *m_pEventGetInfo;
    void *m_pEventFlush;
    void *m_pEventKill;
    void *m_pTLOpen;
    void *m_pTLClose;
    void *m_pTLGetInfo;
    void *m_pTLGetNumInterfaces;
    void *m_pTLGetInterfaceID;
    void *m_pTLGetInterfaceInfo;
    void *m_pTLOpenInterface;
    void *m_pTLUpdateInterfaceList;
    void *m_pIFClose;
    void *m_pIFGetInfo;
    void *m_pIFGetNumDevices;
    void *m_pIFGetDeviceID;
    void *m_pIFUpdateDeviceList;
    void *m_pIFGetDeviceInfo;
    void *m_pIFOpenDevice;
    void *m_pIFGetParentTL;
    void *m_pDevGetPort;
    void *m_pDevGetNumDataStreams;
    void *m_pDevGetDataStreamID;
    void *m_pDevOpenDataStream;
    void *m_pDevGetInfo;
    void *m_pDevClose;
    void *m_pDevGetParentIF;
    void *m_pDSAnnounceBuffer;
    void *m_pDSAllocAndAnnounceBuffer;
    void *m_pDSFlushQueue;
    void *m_pDSStartAcquisition;
    void *m_pDSStopAcquisition;
    void *m_pDSGetInfo;
    void *m_pDSGetBufferID;
    void *m_pDSClose;
    void *m_pDSRevokeBuffer;
    void *m_pDSQueueBuffer;
    void *m_pDSGetBufferInfo;
    void *m_pDSGetBufferChunkData;
    void *m_pDSGetParentDev;

};

#endif  //FNEXPORTTEST_H
