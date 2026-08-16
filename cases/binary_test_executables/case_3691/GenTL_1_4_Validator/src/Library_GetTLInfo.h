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

#ifndef LIBRARY_GetTLINFO_H
#define LIBRARY_GetTLINFO_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "GenTLTestParameter.h"

class LibrarySystemSetup;
class SFNCPort;

class Library_GetTLInfo
{
public:
    typedef std::vector<std::string> tStringList;

    Library_GetTLInfo( void );
    ~Library_GetTLInfo( void );
    
    void vDisplayTLInfo( uint32_t test_id );

protected:
    void setUp( void );
    void tearDown( void );

    int64_t zGetGenICamVersionMajor(LibrarySystemSetup &oLibSysSetup);
    int64_t zGetGenICamVersionMinor(LibrarySystemSetup &oLibSysSetup);
    std::string sGetTLVendorName(LibrarySystemSetup &oLibSysSetup);
    std::string sGetTLModelName(LibrarySystemSetup &oLibSysSetup);
    std::string sGetTLID(LibrarySystemSetup &oLibSysSetup);
    std::string sGetTLVersion(LibrarySystemSetup &oLibSysSetup);
    std::string sGetTLPath(LibrarySystemSetup &oLibSysSetup);
    std::string sGetTLType(LibrarySystemSetup &oLibSysSetup);
    
protected:
    ocGenTLTestParameter m_oParameter;

};

#endif  //LIBRARY_GetTLINFO_H
