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

#ifndef INTERFACE_IFGETDEVICEID_H
#define INTERFACE_IFGETDEVICEID_H

#include "GenTL_v1_4.h"

#include "Modules.h"

class Interface_IFGetDeviceID
{
public:
    Interface_IFGetDeviceID( void );
    ~Interface_IFGetDeviceID( void );
    
    void TestIFGetDeviceID( uint32_t test_id );
    void TestIFGetDeviceIDWithoutGCInitLib( uint32_t test_id );
    void TestIFGetDeviceIDWithoutTLOpen( uint32_t test_id );
    void TestIFGetDeviceIDWithOldHandle( uint32_t test_id );
    void TestIFGetDeviceIDWithIfHandleNULL( uint32_t test_id );
    void TestIFGetDeviceIDWithSizeNULL1( uint32_t test_id );
    void TestIFGetDeviceIDWithSizeNULL2( uint32_t test_id );
    void TestIFGetDeviceIDWithInvalidIndex( uint32_t test_id );
    void TestIFGetDeviceIDWithSizeLow( uint32_t test_id );
    void TestIFGetDeviceIDPersistence( uint32_t test_id );

protected:
    void setUp( void );
    void tearDown( void );

private:
    ModIF   m_ModIF;
};

#endif  //INTERFACE_IFGETDEVICEID_H
