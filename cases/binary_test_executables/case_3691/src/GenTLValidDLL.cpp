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

#include <iostream>
#include <iomanip>
#include <string>
#include <ostream>
#include <vector>

#include <GenICamVersion.h>
#include "Base/GCLinkage.h"

#define GENTLVALID14_FILE_VERSION               "0.0.0.31"

# pragma message ("compiling version = " GENTLVALID14_FILE_VERSION)

# pragma comment (lib, LIB_NAME( "GCBase" ))
# pragma comment (lib, LIB_NAME( "GenApi" ))

# pragma message ("using library = " LIB_NAME( "GCBase" ))
# pragma message ("using library = " LIB_NAME( "GenApi" ))

#include "GenApi/GenApi.h"

#include "GenTLValidDll.h"
#include "GenTLTestSuite.h"
#include "GenTLTestCases.h"
#include "GenTLTestTools.h"

#ifdef _USING_BOOST
#include "GenTLTestBoost.h"
using namespace  boost::program_options;
#else /* _USING_BOOST */
#include "GenTLUnitTest.h"
#endif /* _USING_BOOST */

//////////////////////////////////////////////////////
// globals
//////////////////////////////////////////////////////

uint64_t g_zCheckedGenTLVersionMajor=0;
uint64_t g_zCheckedGenTLVersionMinor=0;
std::vector<std::string> g_vecGenTLList;
int64_t g_zSpecialTestCaseToDoFrom=ALL_TEST_CASES;
int64_t g_zSpecialTestCaseToDoTo=ALL_TEST_CASES;
uint8_t g_bTestDatastream=1;
uint8_t g_bTestChunkData=0;
uint8_t g_bReusePayloadsize=0;
uint8_t g_bLogCTIInterfaceFlag=0;
std::string g_csGenTLValid14FileVersion=GENTLVALID14_FILE_VERSION;
std::string g_csLogOutputDirectory="";
uint8_t g_bTLNotCompliant=1;

uint8_t g_SkipTypeNULLCheck=1;
uint8_t g_SkipSizeNULLCheck=1;

#ifndef _CONSOLE
nMessageCallback g_pCallBackFunc=NULL;
#endif // _CONSOLE

void *g_pPrivateData=NULL;

std::string g_strTLPath;

//////////////////////////////////////////////////////
// helper
//////////////////////////////////////////////////////

void vCreateDummyArg(int *argc, char **argv[])
{
    std::string sDummyString="GenTLValidDLL14.dll";
    size_t zTotalArgvLength=sDummyString.size()+1;

    *argc = 1;

    *argv = new char*[*argc+1];
    (*argv)[0] = new char[zTotalArgvLength];
    memset((*argv)[0], 0, sizeof(char)*zTotalArgvLength);
    sprintf_s((*argv)[0], sizeof(char)*zTotalArgvLength, sDummyString.c_str());

    (*argv)[1] = NULL;
}

void vSendCallbackMessage(uint64_t zMsgID, std::string sMessage)
{
#ifndef _CONSOLE
    if (g_pCallBackFunc != NULL)
    {
        (*g_pCallBackFunc)(zMsgID, sMessage.c_str(), g_pPrivateData);
    }
#else
    if (zMsgID == DLL_CALLBACK_TESTSUCCEEDED)
        std::cout << sMessage.c_str() << "=OK, ";
    else if (zMsgID == DLL_CALLBACK_FILENAME)
        std::cout << "Log output to: " << sMessage.c_str() << std::endl;
    else if (zMsgID == DLL_CALLBACK_TOTALTESTAMOUNT)
        std::cout << "Running " << sMessage.c_str() << " test cases..." << std::endl;
    else if (zMsgID == DLL_CALLBACK_TESTFAILED)
        std::cout << std::endl << sMessage.c_str() << "=Failed: " << std::endl;
    else if (zMsgID == DLL_CALLBACK_TESTABORTED)
        std::cout << std::endl << "Test aborted: " << sMessage.c_str() << std::endl;
    else if (zMsgID == DLL_CALLBACK_ERROR)
        std::cout << "          Error message: " << sMessage.c_str() << std::endl;
#endif // _CONSOLE
}

void vReset()
{
    g_zCheckedGenTLVersionMajor=0;
    g_zCheckedGenTLVersionMinor=0;
    g_bCreateTestEnumerationFlag=0;
    g_vecGenTLList.clear();
    g_zSpecialTestCaseToDoFrom = ALL_TEST_CASES;
    g_zSpecialTestCaseToDoTo = ALL_TEST_CASES;
    g_bTestDatastream = 1;
    g_bTestChunkData = 0;
    g_bReusePayloadsize = 0;
#ifndef _CONSOLE
    g_pCallBackFunc = NULL;
#endif // _CONSOLE
    g_pPrivateData = NULL;
    g_strTLPath = "";
}

//////////////////////////////////////////////////////
// dll access control
//////////////////////////////////////////////////////

const char *DLL_EXPORT pcGetDLLFileVersion()
{
    return g_csGenTLValid14FileVersion.c_str();
}

void DLL_EXPORT vCreateTestEnumeration()
{
    int argc=0;
    char **argv=NULL;
    
    nCreateTestEnumeration();

    // create a dummy entry
    g_vecGenTLList.push_back("Dummy.cti");

    vCreateDummyArg(&argc, &argv);

    vSendCallbackMessage(DLL_CALLBACK_TESTSTARTED, "Starting creation of test case enumeration ...");
    
    nStartTestRunMain(argc, argv);

    vSendCallbackMessage(DLL_CALLBACK_TESTSTOPPED, "Enumeration finished.");
}

void DLL_EXPORT vSetTLFile(const char *pcFile)
{
    g_vecGenTLList.push_back(pcFile);
}

void DLL_EXPORT vSetDLLSpecialTestCaseRange(int64_t zSpecialTestCaseNumberFrom, int64_t zSpecialTestCaseNumberTo)
{
    if (g_zSpecialTestCaseToDoFrom == ALL_TEST_CASES ||
        g_zSpecialTestCaseToDoFrom <= g_zSpecialTestCaseToDoTo)
    {
        g_zSpecialTestCaseToDoFrom = zSpecialTestCaseNumberFrom;
        g_zSpecialTestCaseToDoTo = zSpecialTestCaseNumberTo;
    }
}

void DLL_EXPORT vSetDLLTestTimeout(uint8_t zFlag)
{
    nSwitchTestTimeout(zFlag);
}

void DLL_EXPORT vSetDLLTestDatastream(uint8_t zFlag)
{
    g_bTestDatastream = zFlag;
}

void DLL_EXPORT vSetDLLTestChunkData(uint8_t zFlag)
{
    g_bTestChunkData = zFlag;
}

void DLL_EXPORT vSetDLLReusePayloadsize(uint8_t zFlag)
{
    g_bReusePayloadsize = zFlag;
}

void DLL_EXPORT vSetDLLCTIInterfaceLog(uint8_t zFlag)
{
    g_bLogCTIInterfaceFlag = zFlag;
}

#ifndef _CONSOLE
void DLL_EXPORT vSetDLLCallback(nMessageCallback pFunc, void *pPrivateData)
{
    g_pCallBackFunc = pFunc;
    g_pPrivateData = pPrivateData;
}
#endif // _CONSOLE

void DLL_EXPORT vSetDLLOutputDirectory(const char *pcDirectory)
{
    g_csLogOutputDirectory = pcDirectory;
}

void DLL_EXPORT vStopTLTest()
{
    vSendCallbackMessage(DLL_CALLBACK_MESSAGE, "Aborting validation ...");

    nStopTestRunMain();
}

void DLL_EXPORT vRunTLTest()
{
    int argc=0;
    char **argv=NULL;
     
    vCreateDummyArg(&argc, &argv);

    vSendCallbackMessage(DLL_CALLBACK_TESTSTARTED, "Starting validation ...");
    
    nStartTestRunMain(argc, argv);

    vSendCallbackMessage(DLL_CALLBACK_TESTSTOPPED, "Validation finished.");
}

//////////////////////////////////////////////////////
// dll main
//////////////////////////////////////////////////////

#ifndef _CONSOLE

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        vReset();       
        break;
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

#endif //_CONSOLE

