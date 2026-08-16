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

#ifndef GENTLVALIDDLL_INCLUDE___
#define GENTLVALIDDLL_INCLUDE___

#include <base/gctypes.h>

// To keep a GenTLValidCon1x project self-consistent the message tags are doubled 
// keep an eye on the consistency
// 1. GenTLValidation GUI "../../GenTLValidation/src/TLValidInterface.h"
// 2. local "TLValidMessages.h"
#ifndef _CONSOLE
#include "../../GenTLValidation/src/TLValidInterface.h"
#else
#include "TLValidMessages.h"
#endif //_CONSOLE

//////////////////////////////////////////////////////
// declarations
//////////////////////////////////////////////////////

#ifndef _CONSOLE
#define DLL_EXPORT __stdcall
#else 
#define DLL_EXPORT
#endif //_CONSOLE

void vSendCallbackMessage(uint64_t zMsgID, std::string sMessage);

const char *DLL_EXPORT pcGetDLLFileVersion();
void DLL_EXPORT vCreateTestEnumeration();
void DLL_EXPORT vSetTLFile(const char *pcFile);
void DLL_EXPORT vSetDLLSpecialTestCaseRange(int64_t zSpecialTestCaseNumberFrom, int64_t zSpecialTestCaseNumberTo);
void DLL_EXPORT vSetDLLTestTimeout(uint8_t zFlag);
void DLL_EXPORT vSetDLLTestDatastream(uint8_t zFlag);
void DLL_EXPORT vSetDLLTestChunkData(uint8_t zFlag);
void DLL_EXPORT vSetDLLReusePayloadsize(uint8_t zFlag);
void DLL_EXPORT vSetDLLCTIInterfaceLog(uint8_t zFlag);
#ifndef _CONSOLE
void DLL_EXPORT vSetDLLCallback(nMessageCallback pFunc, void* pPrivateData);
#endif // _CONSOLE
void DLL_EXPORT vSetDLLOutputDirectory(const char *pcDirectory);
void DLL_EXPORT vStopTLTest();
void DLL_EXPORT vRunTLTest();

#endif /* GENTLVALIDDLL_INCLUDE___ */