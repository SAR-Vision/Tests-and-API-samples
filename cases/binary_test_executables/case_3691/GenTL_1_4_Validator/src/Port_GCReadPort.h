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

#ifndef PORT_GCREADPORT_H
#define PORT_GCREADPORT_H

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "URLParser.h"
#include "LocalParser.h"

class LibrarySystemSetup;
class Interface_PreCondition;
class SFNCPort;

class Port_GCReadPort
{
public:
    Port_GCReadPort( void );
    ~Port_GCReadPort( void );
    
    void TestGCReadPortSystem( uint32_t test_id );
    void TestGCReadPortSystemEntry( uint32_t test_id );
    void TestGCReadPortSystemMandatoryEntries( uint32_t test_id );
    void TestGCReadPortInterface( uint32_t test_id );
    void TestGCReadPortInterfaceEntry( uint32_t test_id );
    void TestGCReadPortInterfaceMandatoryEntries( uint32_t test_id );
    void TestGCReadPortDevice( uint32_t test_id );
    void TestGCReadPortDeviceEntry( uint32_t test_id );
    void TestGCReadPortDeviceMandatoryEntries( uint32_t test_id );
    void TestGCReadPortRemoteDevice( uint32_t test_id );
    //void TestGCReadPortRemoteDeviceEntry( uint32_t test_id );
    void TestGCReadPortRemoteDeviceMandatoryEntries( uint32_t test_id );
    void TestGCReadPortDatastream( uint32_t test_id );
    void TestGCReadPortDatastreamEntry( uint32_t test_id );
    void TestGCReadPortDatastreamMandatoryEntries( uint32_t test_id );
    void TestGCReadPortBufferMandatoryEntries( uint32_t test_id );
    void TestGCReadPortWithoutGCInitLib( uint32_t test_id );
    void TestGCReadPortWithoutTLOpen( uint32_t test_id );
    void TestGCReadPortWithoutIFOpen( uint32_t test_id );
    void TestGCReadPortWithOldHandle( uint32_t test_id );
    void TestGCReadPortWithSizeNull( uint32_t test_id );
    void TestGCReadPortWithSizeLow( uint32_t test_id );
    void TestGCReadPortWithBufferNull( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    std::vector<char> sGetPortURL(void* hHandle, bool bMandatory=true);
    
private:
    ModPORT m_ModPort;
};

#endif  //PORT_GCREADPORT_H
