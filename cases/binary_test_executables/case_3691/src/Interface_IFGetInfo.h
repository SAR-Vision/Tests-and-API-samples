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

#ifndef INTERFACE_IFGETINFO_INCLUDE____
#define INTERFACE_IFGETINFO_INCLUDE____

#include "GenApi/GenApi.h"

#include "GenTL_v1_4.h"

#include "Modules.h"

class LibrarySystemSetup;

class Interface_IFGetInfo
{
public:
    struct stIFDevice
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        GenICam::Client::INTERFACE_INFO_CMD eCommand;
    };
    typedef std::vector<stIFDevice> tIFDeviceList;

    Interface_IFGetInfo( void );
    ~Interface_IFGetInfo( void );
    
    void TestIFGetInfo( uint32_t test_id );
    void TestIFGetInfoWithoutGCInitLib( uint32_t test_id );
    void TestIFGetInfoWithoutTLOpen( uint32_t test_id );
    void TestIFGetInfoWithHandleNull( uint32_t test_id );
    void TestIFGetInfoWithInvalidCommand( uint32_t test_id );
    void TestIFGetInfoWithOldHandle( uint32_t test_id );
    void TestIFGetInfoWithSizeNull( uint32_t test_id );
    void TestIFGetInfoWithTypeNULL( uint32_t test_id );
    
protected:
    void setUp          ( void );
    void tearDown       ( void );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList);

private:
    ModIF     m_ModIF;
};

#endif  //INTERFACE_IFGETINFO_INCLUDE____
