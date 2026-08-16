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

#include <string>

#include "GenApi/GenApi.h"

#include "Library_GetTLInfo.h"
#include "Library.h"
#include "GenTLTestSuite.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

GENTLTEST_UNIT_IMPL(Library_GetTLInfo, vDisplayTLInfo);

GENTLTEST_UNIT_IMPL_MSG(LibraryTest, TestGCInitLib, "LibraryTest");
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCInitLibGCCloseLib);
GENTLTEST_UNIT_IMPL(LibraryTest, TestDoubleGCInitLibGCCloseLib);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCInitLibDoubleGCCloseLib);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCCloseLibWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastError);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorBeforeGCInitLib);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorERR_SUCCESS);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorErrorCodeNULL);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorErrorSizeNULL);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorWithBufferErrorCodeNULL);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorWithBufferErrorSizeNULL);
GENTLTEST_UNIT_IMPL(LibraryTest, TestGCGetLastErrorWithBufferErrorLessSize);

GENTLTEST_UNIT_IMPL_MSG(Library_GCGetInfo, TestGCGetInfo, "Library_GCGetInfo");
GENTLTEST_UNIT_IMPL(Library_GCGetInfo, TestGCGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_IMPL(Library_GCGetInfo, TestGCGetInfo1Size0);
GENTLTEST_UNIT_IMPL(Library_GCGetInfo, TestGCGetInfo2Size0);
GENTLTEST_UNIT_IMPL(Library_GCGetInfo, TestGCGetInfoLowSize);
GENTLTEST_UNIT_IMPL(Library_GCGetInfo, TestGCGetInfoTypeNULL);
GENTLTEST_UNIT_IMPL(Library_GCGetInfo, TestGCGetInfoWithInvalidCommand);
