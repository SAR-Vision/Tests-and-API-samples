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

#ifndef DATASTREAM_PRECONDITION_INCLUDE___
#define DATASTREAM_PRECONDITION_INCLUDE___

#include <string>
#include <vector>

#include "GenApi/GenApi.h"
#include "GenTL_v1_4.h"

#include "Modules.h"

class ocGenTLTestParameter;
class LibrarySystemSetup;

class DataStream_PreCondition
{
public:
    DataStream_PreCondition(GenICam::Client::DEV_HANDLE hDev, 
                         std::string &sDataStreamID, 
                         GenICam::Client::DS_HANDLE *hDs);
    ~DataStream_PreCondition(void);
    
    GenICam::Client::GC_ERROR eGetLastResult();
    
    void vClose();
    GenICam::Client::GC_ERROR eDSAnnounceBuffer(GenICam::Client::DS_HANDLE hDs, 
                                                 size_t uiPayLoadSize, 
                                                 void *pvPrivate, 
                                                 GenICam::Client::BUFFER_HANDLE *hBuffer,
                                                 void **ppvPayLoad=NULL);
    GenICam::Client::GC_ERROR eDSAnnounceBufferAndQueue(GenICam::Client::DS_HANDLE hDs, 
                                                        size_t uiPayLoadSize, 
                                                        void *pvPrivate, 
                                                        GenICam::Client::BUFFER_HANDLE *hBuffer,
                                                        void **ppvPayLoad=NULL);
    GenICam::Client::GC_ERROR eDSAllocAndAnnounceBufferAndQueue(GenICam::Client::DS_HANDLE hDs, 
                                                                size_t uiPayLoadSize, 
                                                                void *pvPrivate, 
                                                                GenICam::Client::BUFFER_HANDLE *hBuffer);
    GenICam::Client::GC_ERROR eDSAllocAndAnnounceBuffer(GenICam::Client::DS_HANDLE hDs, 
                                                        size_t uiPayLoadSize, 
                                                        void *pvPrivate, 
                                                        GenICam::Client::BUFFER_HANDLE *hBuffer);
    GenICam::Client::GC_ERROR eDSStartAcquisition(GenICam::Client::DS_HANDLE hDs, 
                                                 uint64_t uiNumToAcquire);
    GenICam::Client::GC_ERROR eDSStopAcquisition(GenICam::Client::DS_HANDLE hDs, 
                                                   GenICam::Client::ACQ_STOP_FLAGS_LIST eStopFlag);
    GenICam::Client::GC_ERROR eDSFlushQueue(GenICam::Client::DS_HANDLE hDs, 
                                                   GenICam::Client::ACQ_QUEUE_TYPE_LIST eOperation);
    GenICam::Client::GC_ERROR eDSRevokeBuffer(GenICam::Client::DS_HANDLE hDs, 
                                               GenICam::Client::BUFFER_HANDLE hBuffer,
                                               void **pvBuffer,
                                               void **pvPrivateData);
    GenICam::Client::GC_ERROR eDSQueueBuffer(GenICam::Client::DS_HANDLE hDs, 
                                              GenICam::Client::BUFFER_HANDLE hBuffer);
    size_t uiGetPayLoadSize(LibrarySystemSetup &oLibSysSetup,
                            GenICam::Client::DS_HANDLE hDs,
                            GenICam::Client::PORT_HANDLE hPort);
    size_t uiGetPayLoadSizeByDSGetInfo(GenICam::Client::DS_HANDLE hDs);
    
    size_t uiGetPayLoadListSize();
    GenICam::Client::GC_ERROR eLockParameter(ocGenTLTestParameter &oParameter, bool bLock);
    GenICam::Client::GC_ERROR eSetChunkModeActive(ocGenTLTestParameter &oParameter);
    size_t uiDSGetInfoMinBuffers(GenICam::Client::DS_HANDLE hDs);

private:
    std::string sGetLastErrorMessage();

private:
    typedef std::vector<void *> tPayloadList;
    tPayloadList m_vecPayloadList;
    static enum eLastPayloadSizeSource
    {
        LAST_PAYLOADSIZE_SOURCE_NA,
        LAST_PAYLOADSIZE_SOURCE_DSGETINFO,
        LAST_PAYLOADSIZE_SOURCE_XMLDATASTREAM,
        LAST_PAYLOADSIZE_SOURCE_XMLREMOTEDEVICE,
    };
    static eLastPayloadSizeSource m_eLastPayloadSizeSource;
    static size_t m_uiFoundPayloadSize;

    ModGC   m_ModGC;
    ModDEV  m_ModDev;
    ModDS   m_ModDS;

    GenICam::Client::DS_HANDLE m_hDs;
    GenICam::Client::GC_ERROR	m_LastResult;
};

#endif /* DATASTREAM_PRECONDITION_INCLUDE___ */