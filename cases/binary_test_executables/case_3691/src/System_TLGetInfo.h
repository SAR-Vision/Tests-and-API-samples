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

#ifndef SYSTEMTEST_TLGETINFO_H
#define SYSTEMTEST_TLGETINFO_H

#include "GenTL_v1_4.h"

#include "Modules.h"

class System_TLGetInfo
{
public:
    System_TLGetInfo(void);
    ~System_TLGetInfo(void);
    
    void TestTLGetInfo( uint32_t test_id );
    void TestTLGetInfoWithoutGCInitLib( uint32_t test_id );
    void TestTLGetInfoWithoutTLOpen( uint32_t test_id );
    void TestTLGetInfoWithTLHandleNULL( uint32_t test_id );
    void TestTLGetInfoWithInvalidCommand( uint32_t test_id );
    void TestTLGetInfoWithTypeNull( uint32_t test_id );
    void TestTLGetInfoWithSizeNull( uint32_t test_id );
    void TestTLGetInfoWithLowSize( uint32_t test_id );
    void TestTLGetInfoWithBufferSizeNULL( uint32_t test_id );

protected:
    void setUp( void );
    void tearDown( void );

private:
    ModTL   m_ModTL;
};

#endif  //SYSTEMTEST_H
