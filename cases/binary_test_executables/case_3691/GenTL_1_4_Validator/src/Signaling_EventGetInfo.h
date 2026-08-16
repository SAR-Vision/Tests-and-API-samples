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

#ifndef SIGNALING_EVENTGETINFO_INCLUDE___
#define SIGNALING_EVENTGETINFO_INCLUDE___

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "GenTLTestParameter.h"

class LibrarySystemSetup;

class Signaling_EventGetInfo
{
public:
    struct stSignaling
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        std::string sDataStreamID;
        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess;
        GenICam::Client::EVENT_TYPE_LIST eEventID;
        GenICam::Client::EVENT_INFO_CMD eCommand;
    };
    typedef std::vector<stSignaling> tSignalingList;

    Signaling_EventGetInfo( void );
    ~Signaling_EventGetInfo( void );
    
    void TestEventGetInfo( uint32_t test_id );
    void TestEventGetInfoWithoutGCInitLib( uint32_t test_id );
    void TestEventGetInfoWithoutTLOpen( uint32_t test_id );
    void TestEventGetInfoWithoutIFOpen( uint32_t test_id );
    void TestEventGetInfoWithoutDevOpen( uint32_t test_id );
    void TestEventGetInfoWithoutDSOpen( uint32_t test_id );
    void TestEventGetInfoWithoutRegisterEvent( uint32_t test_id );
    void TestEventGetInfoWithEventHandleNull( uint32_t test_id );
    void TestEventGetInfoWithInvalidCommand( uint32_t test_id );
    void TestEventGetInfoWithTypeNULL( uint32_t test_id );
    void TestEventGetInfoWithSizeNULL( uint32_t test_id );
    void TestEventGetInfoBufferWithoutSize( uint32_t test_id );
    void TestEventGetInfoBufferWithSizeNull( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tSignalingList &vecSignalingList);

private:
    ModEVENT m_ModEvent;
    ocGenTLTestParameter m_oParameter;
};

#endif  //SIGNALING_EVENTGETINFO_INCLUDE___
