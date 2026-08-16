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

#ifndef Port_GCWritePortStacked_H
#define Port_GCWritePortStacked_H

#include "GenTL_v1_4.h"

#include "Modules.h"
#include "URLParser.h"
#include "LocalParser.h"

class LibrarySystemSetup;
class Interface_PreCondition;
class SFNCPort;

class Port_GCWritePortStacked
{
public:
    Port_GCWritePortStacked( void );
    ~Port_GCWritePortStacked( void );
    
    void TestGCWritePortStacked( uint32_t test_id );
    void TestGCWritePortStackedWithoutGCInitLib( uint32_t test_id );
    void TestGCWritePortStackedWithoutTLOpen( uint32_t test_id );
    
protected:
    void setUp( void );
    void tearDown( void );

    bool bFillEntry(SFNCPort &oLocalDevPort, std::string &sNodeName, GenApi::CNodePtr &cNode, GenICam::Client::PORT_REGISTER_STACK_ENTRY &oEntry);
    void vDeleteEntryBuffers(std::vector<GenICam::Client::PORT_REGISTER_STACK_ENTRY> &oEntries);

private:
    ModPORT m_ModPort;
};

#endif  //Port_GCWritePortStacked_H
