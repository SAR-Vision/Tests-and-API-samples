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

#ifndef PORT_GCGETPORTURLINFO_H
#define PORT_GCGETPORTURLINFO_H

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "URLParser.h"
#include "LocalParser.h"

class LibrarySystemSetup;

class Port_GCGetPortURLInfo
{
public:
    struct stIFDevice
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess;
        uint32_t iNumURLIndex;
        GenICam::Client::URL_INFO_CMD_LIST eCommand;
    };
    typedef std::vector<stIFDevice> tIFDeviceList;

    Port_GCGetPortURLInfo( void );
    ~Port_GCGetPortURLInfo( void );
    
    void TestGCGetPortURLInfo( uint32_t test_id );
    void TestGCGetPortURLInfoWithoutGCInitLib( uint32_t test_id );
    void TestGCGetPortURLInfoWithoutTLOpen( uint32_t test_id );
    void TestGCGetPortURLInfoWithoutIFOpen( uint32_t test_id );
    void TestGCGetPortURLInfoWithOldHandle( uint32_t test_id );
    void TestGCGetPortURLInfoPortHandleNull( uint32_t test_id );
    void TestGCGetPortURLInfoWithSizeNull( uint32_t test_id );
    void TestGCGetPortURLInfoWithTypeNull( uint32_t test_id );
    void TestGCGetPortURLInfoBufferWithoutSize( uint32_t test_id );
    void TestGCGetPortURLInfoBufferWithSizeNull( uint32_t test_id );
    void TestGCGetPortURLInfoBufferWithSizeLow( uint32_t test_id );
    void TestGCGetPortURLInfoWithInvalidCommand(uint32_t test_id );
    void TestGCGetPortURLInfoWithInvalidIndex( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList);
    
private:
    ModPORT m_ModPort;
};

#endif  //PORT_GCGETPORTURLINFO_H
