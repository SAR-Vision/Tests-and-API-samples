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

#ifndef MODULES_H
#define MODULES_H

#include "Base/GCTypes.h"
#include "GenTL_v1_4.h"

#include "FnExportTest.h"
#include "GenTlTestTools.h"

class ModGC
{
public:
    ModGC ( void )
    : m_pFGCGetInfo ( NULL )
    , m_pFGCGetLastError ( NULL )
    , m_pFGCInitLib ( NULL )
    , m_pFGCCloseLib ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eGCGetInfo( GenICam::Client::TL_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eGCGetLastError( GenICam::Client::GC_ERROR *piErrorCode, char *sErrText, size_t *piSize );
    GenICam::Client::GC_ERROR eGCInitLib( void );
    GenICam::Client::GC_ERROR eGCCloseLib( void );

private:
    GenICam::Client::PGCGetInfo      m_pFGCGetInfo;
  
    GenICam::Client::PGCGetLastError m_pFGCGetLastError;
  
    GenICam::Client::PGCInitLib      m_pFGCInitLib;
    GenICam::Client::PGCCloseLib     m_pFGCCloseLib;

};

class ModTL
{
public:
    ModTL( void )
    : m_pFTLOpen ( NULL )
    , m_pFTLClose ( NULL )
    , m_pFTLGetInfo ( NULL )
    , m_pFTLGetNumInterfaces ( NULL )
    , m_pFTLGetInterfaceID ( NULL )
    , m_pFTLGetInterfaceInfo ( NULL )
    , m_pFTLOpenInterface ( NULL )
    , m_pFTLUpdateInterfaceList ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eTLOpen( GenICam::Client::TL_HANDLE *phTL );
    GenICam::Client::GC_ERROR eTLClose( GenICam::Client::TL_HANDLE hTL );
    GenICam::Client::GC_ERROR eTLGetInfo( GenICam::Client::TL_HANDLE hTL, GenICam::Client::TL_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eTLGetNumInterfaces( GenICam::Client::TL_HANDLE hTL, uint32_t *piNumIfaces );
    GenICam::Client::GC_ERROR eTLGetInterfaceID( GenICam::Client::TL_HANDLE hTL, uint32_t iIndex,  char *sID, size_t *piSize );
    GenICam::Client::GC_ERROR eTLGetInterfaceInfo( GenICam::Client::TL_HANDLE hTL, const char *sIfaceID, GenICam::Client::INTERFACE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eTLOpenInterface( GenICam::Client::TL_HANDLE hTL, const char *sIfaceID, GenICam::Client::IF_HANDLE *phIface );
    GenICam::Client::GC_ERROR eTLUpdateInterfaceList( GenICam::Client::TL_HANDLE hTL, bool8_t *pbChanged, uint64_t iTimeout );

private:
    GenICam::Client::PTLOpen                m_pFTLOpen;
    GenICam::Client::PTLClose               m_pFTLClose;
    GenICam::Client::PTLGetInfo             m_pFTLGetInfo;

    GenICam::Client::PTLGetNumInterfaces    m_pFTLGetNumInterfaces;
    GenICam::Client::PTLGetInterfaceID      m_pFTLGetInterfaceID;
    GenICam::Client::PTLGetInterfaceInfo    m_pFTLGetInterfaceInfo;
    GenICam::Client::PTLOpenInterface       m_pFTLOpenInterface;
    GenICam::Client::PTLUpdateInterfaceList m_pFTLUpdateInterfaceList;


};

class ModIF
{
public:
    ModIF( void )
    : m_pFIFClose ( NULL )
    , m_pFIFGetInfo ( NULL )
    , m_pFIFGetNumDevices ( NULL )
    , m_pFIFGetDeviceID ( NULL )
    , m_pFIFUpdateDeviceList ( NULL )
    , m_pFIFGetDeviceInfo ( NULL )
    , m_pFIFOpenDevice ( NULL )
    , m_pFIFGetParentTL ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eIFClose( GenICam::Client::IF_HANDLE hIface );
    GenICam::Client::GC_ERROR eIFGetInfo( GenICam::Client::IF_HANDLE hIface, GenICam::Client::INTERFACE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eIFGetNumDevices( GenICam::Client::IF_HANDLE hIface, uint32_t *piNumDevices );
    GenICam::Client::GC_ERROR eIFGetDeviceID( GenICam::Client::IF_HANDLE hIface, uint32_t iIndex, char *sIDeviceID, size_t *piSize );
    GenICam::Client::GC_ERROR eIFUpdateDeviceList( GenICam::Client::IF_HANDLE hIface, bool8_t *pbChanged, uint64_t iTimeout );
    GenICam::Client::GC_ERROR eIFGetDeviceInfo( GenICam::Client::IF_HANDLE hIface, const char *sDeviceID, GenICam::Client::DEVICE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eIFOpenDevice( GenICam::Client::IF_HANDLE hIface, const char *sDeviceID, GenICam::Client::DEVICE_ACCESS_FLAGS iOpenFlags, GenICam::Client::DEV_HANDLE *phDevice );
    GenICam::Client::GC_ERROR eIFGetParentTL( GenICam::Client::IF_HANDLE hIface, GenICam::Client::TL_HANDLE *phSystem );

private:
    GenICam::Client::PIFClose            m_pFIFClose;
    GenICam::Client::PIFGetInfo          m_pFIFGetInfo;
  
    GenICam::Client::PIFGetNumDevices    m_pFIFGetNumDevices;
    GenICam::Client::PIFGetDeviceID      m_pFIFGetDeviceID;
    GenICam::Client::PIFUpdateDeviceList m_pFIFUpdateDeviceList;
    GenICam::Client::PIFGetDeviceInfo    m_pFIFGetDeviceInfo;
    GenICam::Client::PIFOpenDevice       m_pFIFOpenDevice;
    GenICam::Client::PIFGetParentTL      m_pFIFGetParentTL;

};

class ModDEV
{
public:
    ModDEV( void )
    : m_pFDevGetPort ( NULL )
    , m_pFDevGetNumDataStreams ( NULL )
    , m_pFDevGetDataStreamID ( NULL )
    , m_pFDevOpenDataStream ( NULL )
    , m_pFDevGetInfo ( NULL )
    , m_pFDevClose ( NULL )
    , m_pFDevGetParentIF ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eDevGetPort( GenICam::Client::DEV_HANDLE hDevice, GenICam::Client::PORT_HANDLE *phRemoteDevice );
    GenICam::Client::GC_ERROR eDevGetNumDataStreams( GenICam::Client::DEV_HANDLE hDevice, uint32_t *piNumDataStreams );
    GenICam::Client::GC_ERROR eDevGetDataStreamID( GenICam::Client::DEV_HANDLE hDevice, uint32_t iIndex, char *sDataStreamID, size_t *piSize );
    GenICam::Client::GC_ERROR eDevOpenDataStream( GenICam::Client::DEV_HANDLE hDevice, const char *sDataStreamID, GenICam::Client::DS_HANDLE *phDataStream );
    GenICam::Client::GC_ERROR eDevGetInfo( GenICam::Client::DEV_HANDLE hDevice, GenICam::Client::DEVICE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eDevClose( GenICam::Client::DEV_HANDLE hDevice );
    GenICam::Client::GC_ERROR eDevGetParentIF( GenICam::Client::DEV_HANDLE hDevice, GenICam::Client::IF_HANDLE *phIface );

private:
    GenICam::Client::PDevGetPort           m_pFDevGetPort;
    GenICam::Client::PDevGetNumDataStreams m_pFDevGetNumDataStreams;
    GenICam::Client::PDevGetDataStreamID   m_pFDevGetDataStreamID;
    GenICam::Client::PDevOpenDataStream    m_pFDevOpenDataStream;
    GenICam::Client::PDevGetInfo           m_pFDevGetInfo;
    GenICam::Client::PDevClose             m_pFDevClose;
    GenICam::Client::PDevGetParentIF       m_pFDevGetParentIF;
};

class ModDS
{
public:
    ModDS( void )
    : m_pFDSAnnounceBuffer ( NULL )
    , m_pFDSAllocAndAnnounceBuffer ( NULL )
    , m_pFDSFlushQueue ( NULL )
    , m_pFDSStartAcquisition ( NULL )
    , m_pFDSStopAcquisition ( NULL )
    , m_pFDSGetInfo ( NULL )
    , m_pFDSGetBufferID ( NULL )
    , m_pFDSClose ( NULL )
    , m_pFDSRevokeBuffer ( NULL )
    , m_pFDSQueueBuffer ( NULL )
    , m_pFDSGetBufferInfo ( NULL )
    , m_pFDSGetBufferChunkData ( NULL )
    , m_pFDSGetParentDev ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eDSAnnounceBuffer( GenICam::Client::DS_HANDLE hDataStream, void *pBuffer, size_t iSize, void *pPrivate, GenICam::Client::BUFFER_HANDLE *phBuffer );
    GenICam::Client::GC_ERROR eDSAllocAndAnnounceBuffer( GenICam::Client::DS_HANDLE hDataStream, size_t iSize, void *pPrivate, GenICam::Client::BUFFER_HANDLE *phBuffer );
    GenICam::Client::GC_ERROR eDSFlushQueue( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::ACQ_QUEUE_TYPE iOperation );
    GenICam::Client::GC_ERROR eDSStartAcquisition( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::ACQ_START_FLAGS iStartFlags, uint64_t iNumToAcquire );
    GenICam::Client::GC_ERROR eDSStopAcquisition( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::ACQ_STOP_FLAGS iStopFlags );
    GenICam::Client::GC_ERROR eDSGetInfo( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::STREAM_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eDSGetBufferID( GenICam::Client::DS_HANDLE hDataStream, uint32_t iIndex, GenICam::Client::BUFFER_HANDLE *phBuffer );
    GenICam::Client::GC_ERROR eDSClose( GenICam::Client::DS_HANDLE hDataStream );
    GenICam::Client::GC_ERROR eDSRevokeBuffer( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer, void **pBuffer, void **pPrivate );
    GenICam::Client::GC_ERROR eDSQueueBuffer( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer );
    GenICam::Client::GC_ERROR eDSGetBufferInfo( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer, GenICam::Client::BUFFER_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eDSGetBufferChunkData( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer, GenICam::Client::SINGLE_CHUNK_DATA *pChunkData, size_t *piNumChunks );
    GenICam::Client::GC_ERROR eDSGetParentDev( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::DEV_HANDLE *phDevice );

private:
    GenICam::Client::PDSAnnounceBuffer         m_pFDSAnnounceBuffer;
    GenICam::Client::PDSAllocAndAnnounceBuffer m_pFDSAllocAndAnnounceBuffer;
    GenICam::Client::PDSFlushQueue             m_pFDSFlushQueue;
    GenICam::Client::PDSStartAcquisition       m_pFDSStartAcquisition;
    GenICam::Client::PDSStopAcquisition        m_pFDSStopAcquisition;
    GenICam::Client::PDSGetInfo                m_pFDSGetInfo;
    GenICam::Client::PDSGetBufferID            m_pFDSGetBufferID;
    GenICam::Client::PDSClose                  m_pFDSClose;
  
    GenICam::Client::PDSRevokeBuffer           m_pFDSRevokeBuffer;
    GenICam::Client::PDSQueueBuffer            m_pFDSQueueBuffer;
    GenICam::Client::PDSGetBufferInfo          m_pFDSGetBufferInfo;
    GenICam::Client::PDSGetBufferChunkData     m_pFDSGetBufferChunkData;
    GenICam::Client::PDSGetParentDev           m_pFDSGetParentDev; 
};

class ModPORT
{
public:
    ModPORT( void )
        : m_pFGCReadPort ( NULL )
    , m_pFGCWritePort ( NULL )
    , m_pFGCGetPortURL ( NULL )
    , m_pFGCGetNumPortURLs ( NULL )
    , m_pFGCGetPortInfo ( NULL )
    , m_pFGCGetPortURLInfo ( NULL )
    , m_pFGCReadPortStacked ( NULL )
    , m_pFGCWritePortStacked ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eGCReadPort( GenICam::Client::PORT_HANDLE hPort, uint64_t iAddress, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eGCWritePort( GenICam::Client::PORT_HANDLE hPort, uint64_t iAddress, const void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eGCGetPortURL( GenICam::Client::PORT_HANDLE hPort, char *sURL, size_t *piSize );
    GenICam::Client::GC_ERROR eGCGetNumPortURLs( GenICam::Client::PORT_HANDLE hPort, uint32_t *piNumURLs );
    GenICam::Client::GC_ERROR eGCGetPortInfo( GenICam::Client::PORT_HANDLE hPort, GenICam::Client::PORT_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eGCGetPortURLInfo( GenICam::Client::PORT_HANDLE hPort, uint32_t iURLIndex, GenICam::Client::URL_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eGCReadPortStacked( GenICam::Client::PORT_HANDLE hPort, GenICam::Client::PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries );
    GenICam::Client::GC_ERROR eGCWritePortStacked( GenICam::Client::PORT_HANDLE hPort, GenICam::Client::PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries );

private:
    GenICam::Client::PGCReadPort         m_pFGCReadPort;
    GenICam::Client::PGCWritePort        m_pFGCWritePort;
    GenICam::Client::PGCGetPortURL       m_pFGCGetPortURL;
    GenICam::Client::PGCGetNumPortURLs   m_pFGCGetNumPortURLs;
    GenICam::Client::PGCGetPortInfo      m_pFGCGetPortInfo;
    GenICam::Client::PGCGetPortURLInfo   m_pFGCGetPortURLInfo;
    GenICam::Client::PGCReadPortStacked  m_pFGCReadPortStacked;
    GenICam::Client::PGCWritePortStacked m_pFGCWritePortStacked;
};

class ModEVENT
{
public:
    ModEVENT( void )
        : m_pFGCRegisterEvent ( NULL )
    , m_pFGCUnregisterEvent ( NULL )
    , m_pFEventGetData ( NULL )
    , m_pFEventGetDataInfo ( NULL )
    , m_pFEventGetInfo ( NULL )
    , m_pFEventFlush ( NULL )
    , m_pFEventKill ( NULL )
    {
    }

    GenICam::Client::GC_ERROR eGCRegisterEvent( GenICam::Client::EVENTSRC_HANDLE hEventSrc, GenICam::Client::EVENT_TYPE iEventID, GenICam::Client::EVENT_HANDLE *phEvent );
    GenICam::Client::GC_ERROR eGCUnregisterEvent( GenICam::Client::EVENTSRC_HANDLE hEventSrc, GenICam::Client::EVENT_TYPE iEventID );
    GenICam::Client::GC_ERROR eEventGetData( GenICam::Client::EVENT_HANDLE hEvent, void *pBuffer, size_t *piSize, uint64_t iTimeout );
    GenICam::Client::GC_ERROR eEventGetDataInfo( GenICam::Client::EVENT_HANDLE hEvent, const void *pInBuffer, size_t iInSize, GenICam::Client::EVENT_DATA_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pOutBuffer, size_t *piOutSize );
    GenICam::Client::GC_ERROR eEventGetInfo( GenICam::Client::EVENT_HANDLE hEvent, GenICam::Client::EVENT_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );
    GenICam::Client::GC_ERROR eEventFlush( GenICam::Client::EVENT_HANDLE hEvent );
    GenICam::Client::GC_ERROR eEventKill( GenICam::Client::EVENT_HANDLE hEvent );

private:
    GenICam::Client::PGCRegisterEvent   m_pFGCRegisterEvent;
    GenICam::Client::PGCUnregisterEvent m_pFGCUnregisterEvent;

    GenICam::Client::PEventGetData      m_pFEventGetData;
    GenICam::Client::PEventGetDataInfo  m_pFEventGetDataInfo;
    GenICam::Client::PEventGetInfo      m_pFEventGetInfo;
    GenICam::Client::PEventFlush        m_pFEventFlush;
    GenICam::Client::PEventKill         m_pFEventKill;
};

#endif  //MODULES_H
