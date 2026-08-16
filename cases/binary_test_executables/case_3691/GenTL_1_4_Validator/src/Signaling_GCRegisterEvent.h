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

#ifndef SIGNALING_GCREGISTEREVENT_INCLUDE___
#define SIGNALING_GCREGISTEREVENT_INCLUDE___

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "GenTLTestParameter.h"

class LibrarySystemSetup;

class Signaling_GCRegisterEvent
{
public:
    struct stSignaling
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        std::string sDataStreamID;
        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess;
        GenICam::Client::EVENT_TYPE_LIST eEventID;
    };
    typedef std::vector<stSignaling> tSignalingList;

    Signaling_GCRegisterEvent( void );
    ~Signaling_GCRegisterEvent( void );
    
    void TestGCRegisterEventSystem( uint32_t test_id );
    void TestGCRegisterEventInterface( uint32_t test_id );
    void TestGCRegisterEventDevice( uint32_t test_id );
    void TestGCRegisterEventDataStream( uint32_t test_id );
    void TestGCRegisterEventWithoutGCInitLib( uint32_t test_id );
    void TestGCRegisterEventWithoutTLOpen( uint32_t test_id );
    void TestGCRegisterEventWithoutIFOpen( uint32_t test_id );
    void TestGCRegisterEventWithoutDevOpen( uint32_t test_id );
    void TestGCRegisterEventWithoutDSOpen( uint32_t test_id );
    void TestGCRegisterEventWithDSIDNull( uint32_t test_id );
    void TestGCRegisterEventWithWrongEventID( uint32_t test_id );
    void TestGCRegisterEventWithEventHandleNULL( uint32_t test_id );
    void TestGCRegisterEventTwice( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tSignalingList &vecSignalingList);

private:
    ModEVENT m_ModEvent;
    ocGenTLTestParameter m_oParameter;
};

#endif  //SIGNALING_GCREGISTEREVENT_INCLUDE___
