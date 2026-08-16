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

#ifndef DEVICETEST_H
#define DEVICETEST_H

#include "GenApi/GenApi.h"

#include "GenTL_v1_4.h"

#include "Modules.h"

class LibrarySystemSetup;

class DeviceTest
{
public:
    struct stIFDevice
    {
        std::string sInterfaceID;
        std::string sDeviceID;
        GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess;
    };
    typedef std::vector<stIFDevice> tIFDeviceList;

    DeviceTest();
    ~DeviceTest();
    
    void TestDevClose( uint32_t test_id );
    void TestDevCloseWithLibraryClosed( uint32_t test_id );
    void TestDevCloseWithoutTLOpen( uint32_t test_id );
    void TestDevCloseWithoutInterfaceOpen( uint32_t test_id );
    void TestDevCloseDouble( uint32_t test_id );
    void TestDevCloseReopen( uint32_t test_id );
    void TestDevCloseWithHandleDevNull( uint32_t test_id );
    void TestDevCloseWithHandleDevWrong( uint32_t test_id );

    void vCreateTestCaseList(LibrarySystemSetup &oLibSysSetup, tIFDeviceList &vIFDeviceList);

private:
    ModIF       m_ModIF;
    ModDEV      m_ModDev;
};

#endif  //DEVICETEST_H
