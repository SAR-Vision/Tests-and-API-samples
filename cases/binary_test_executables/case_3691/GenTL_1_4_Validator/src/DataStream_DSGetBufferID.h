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

#ifndef DATASTREAM_DSGETBUFFERID_INCLUDE___
#define DATASTREAM_DSGETBUFFERID_INCLUDE___

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "GenTLTestParameter.h"

class LibrarySystemSetup;

class DataStream_DSGetBufferID
{
public:
    struct stDataStream
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        std::string sDataStreamID;
        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess;
        GenICam::Client::STREAM_INFO_CMD_LIST eCommand;
    };
    typedef std::vector<stDataStream> tDataStreamList;

    DataStream_DSGetBufferID( void );
    ~DataStream_DSGetBufferID( void );
    
    void TestDSGetBufferID( uint32_t test_id );
    void TestDSGetBufferIDWithoutGCInitLib( uint32_t test_id );
    void TestDSGetBufferIDWithoutTLOpen( uint32_t test_id );
    void TestDSGetBufferIDWithoutIFOpen( uint32_t test_id );
    void TestDSGetBufferIDWithoutDevOpen( uint32_t test_id );
    void TestDSGetBufferIDWithoutDSOpen( uint32_t test_id );
    void TestDSGetBufferIDWithBufferHandleNULL( uint32_t test_id );
    void TestDSGetBufferIDWithDSIDNull( uint32_t test_id );
    void TestDSGetBufferIDWithInvalidIndex( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tDataStreamList &vecDataStreamList);

private:
    ModDS   m_ModDS;
    ocGenTLTestParameter m_oParameter;
};

#endif  //DATASTREAM_DSGETBUFFERID_INCLUDE___
