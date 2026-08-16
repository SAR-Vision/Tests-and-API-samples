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

#ifndef LIBRARY_H
#define LIBRARY_H

#include "GenTL_v1_4.h"
#include "GenApi/GenApi.h"

#include "LibraryTest.h"
#include "Library_GCGetInfo.h"

GENTLTEST_UNIT_DECL(Library_GetTLInfo, vDisplayTLInfo);

GENTLTEST_UNIT_DECL(LibraryTest, TestGCInitLib);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCInitLibGCCloseLib);
GENTLTEST_UNIT_DECL(LibraryTest, TestDoubleGCInitLibGCCloseLib);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCInitLibDoubleGCCloseLib);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCCloseLibWithoutGCInitLib);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastError);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorBeforeGCInitLib);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorERR_SUCCESS);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorErrorCodeNULL);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorErrorSizeNULL);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorWithBufferErrorCodeNULL);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorWithBufferErrorSizeNULL);
GENTLTEST_UNIT_DECL(LibraryTest, TestGCGetLastErrorWithBufferErrorLessSize);

GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfo);
GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfoWithoutGCInitLib);
GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfo1Size0);
GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfo2Size0);
GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfoLowSize);
GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfoTypeNULL);
GENTLTEST_UNIT_DECL(Library_GCGetInfo, TestGCGetInfoWithInvalidCommand);

#endif  //LIBRARY_H
