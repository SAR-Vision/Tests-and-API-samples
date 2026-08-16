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

#ifndef DEVICE_DEVGETINFO_H
#define DEVICE_DEVGETINFO_H

#include "GenTL_v1_4.h"

#include "Modules.h"

class LibrarySystemSetup;

class Device_DevGetInfo
{
public:
    struct stIFDevice
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess;
        GenICam::Client::DEVICE_INFO_CMD_LIST eCommand;
    };
    typedef std::vector<stIFDevice> tIFDeviceList;

    Device_DevGetInfo( void );
    ~Device_DevGetInfo( void );
    
    void TestDevGetInfo( uint32_t test_id );
    void TestDevGetInfoWithoutGCInitLib( uint32_t test_id );
    void TestDevGetInfoWithoutTLOpen( uint32_t test_id );
    void TestDevGetInfoWithoutIFOpen( uint32_t test_id );
    void TestDevGetInfoWithOldHandle( uint32_t test_id );
    void TestDevGetInfoWithSizeNull( uint32_t test_id );
    void TestDevGetInfoWithTypeNull( uint32_t test_id );
    void TestDevGetInfoWithDevIDNull( uint32_t test_id );
    void TestDevGetInfoBufferWithoutSize( uint32_t test_id );
    void TestDevGetInfoBufferWithSizeNull( uint32_t test_id );
    void TestDevGetInfoBufferWithSizeLow( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList);

private:
    ModDEV  m_ModDev;
};

#endif  //DEVICE_DEVGETINFO_H
