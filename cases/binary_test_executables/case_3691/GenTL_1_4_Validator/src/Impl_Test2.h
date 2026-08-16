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

#ifndef IMPL_TEST2_INCLUDE___
#define IMPL_TEST2_INCLUDE___

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "GenTLTestParameter.h"

class LibrarySystemSetup;

class Impl_Test2
{
public:
    const static int NUMBER_BUFFERS=10;
    const static int NUMBER_EVENTCHUNKDATA_CYCLES=100;
    
    Impl_Test2( void );
    ~Impl_Test2( void );
    
    void TestAcquisitionConsumerBufferDSGetBufferChunkData( uint32_t test_id );
    void TestAcquisitionProducerBufferDSGetBufferChunkData( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

private:
    bool bCheckForInvalidFrame(GenICam::Client::DS_HANDLE hDs, 
                                GenICam::Client::BUFFER_HANDLE hBuffer, 
                                int index);
    uint32_t uiTryToGetChunkData(LibrarySystemSetup &oLibSysSetup, 
                             GenICam::Client::DS_HANDLE hDs, 
                             GenICam::Client::BUFFER_HANDLE hBuffer, 
                             int index,
                             uint32_t &uiNumPayloadUnknown,
                             uint32_t &uiNumPayloadImage,
                             uint32_t &uiNumPayloadRawdata,
                             uint32_t &uiNumPayloadFile,
                             uint32_t &uiNumPayloadChunkdata,
                             uint32_t &uiNumPayloadJPEG,
                             uint32_t &uiNumPayloadJPEG2000,
                             uint32_t &uiNumPayloadH264,
                             uint32_t &uiNumPayloadChunkOnly,
                             uint32_t &uiNumPayloadDeviceSpecific);

private:
    ModDS   m_ModDS;
    ModIF   m_ModIF;
    ocGenTLTestParameter m_oParameter;
};

#endif  // IMPL_TEST2_INCLUDE___
