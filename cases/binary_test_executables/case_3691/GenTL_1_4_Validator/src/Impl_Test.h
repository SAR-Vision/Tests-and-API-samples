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

#ifndef IMPL_TEST_INCLUDE___
#define IMPL_TEST_INCLUDE___

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "GenTLTestParameter.h"

class LibrarySystemSetup;

class Impl_Test
{
public:
    const static int NUMBER_BUFFERS=10;
    const static int NUMBER_EVENTGETDATA_CYCLES=100;
    const static int NUMBER_SHOW_INFOS=2;

    Impl_Test( void );
    ~Impl_Test( void );
    
    void TestNormalAcquisitionConsumerBuffer( uint32_t test_id );
    void TestNormalAcquisitionConsumerBufferForAquiredFrames( uint32_t test_id );
    void TestNormalAcquisitionConsumerBufferDSGetInfo( uint32_t test_id );
    void TestNormalAcquisitionProducerBuffer( uint32_t test_id );
    void TestCommandNewDataProducerBuffer( uint32_t test_id );
    void TestGCGetPortInfoPortName( uint32_t test_id );
    void TestBufferPoolLockDuringAcquisition( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

private:
    bool bCheckForInvalidFrame(GenICam::Client::DS_HANDLE hDs, 
                                GenICam::Client::BUFFER_HANDLE hBuffer, 
                                int index);
    void vDumpBufferInfosConsumer(LibrarySystemSetup &oLibSysSetup, 
                                  GenICam::Client::DS_HANDLE hDs, 
                                  GenICam::Client::BUFFER_HANDLE hBuffer, 
                                  std::vector<void *> &vecPayLoad, 
                                  size_t uiPayLoadSize,
                                  int index);
    void vDumpBufferInfosProducer(LibrarySystemSetup &oLibSysSetup, 
                                  GenICam::Client::DS_HANDLE hDs, 
                                  GenICam::Client::BUFFER_HANDLE hBuffer, 
                                  int index);


private:
    ModDS   m_ModDS;
    ModIF   m_ModIF;
    ocGenTLTestParameter m_oParameter;
};

#endif  // IMPL_TEST_INCLUDE___
