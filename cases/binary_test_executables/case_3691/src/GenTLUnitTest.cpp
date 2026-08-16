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

#include <fstream>
#include <direct.h>
#include <vector>
#include <string>
#include <stdio.h>

#include "GenTLUnitTest.h"
#include "GenTLTestCases.h"
#include "GenTLTestSuite.h"
#include "GenTLTestTools.h"

#include "GenApi/GenApi.h"

#include "GenTLValidDll.h"
#include "FnExportTest.h"

extern std::string g_strTLPath;
extern std::vector<std::string> g_vecGenTLList;
extern std::string g_csGenTLValid14FileVersion;
extern int64_t g_zSpecialTestCaseToDoFrom;
extern int64_t g_zSpecialTestCaseToDoTo;
extern uint8_t g_bReusePayloadsize;
extern uint32_t g_zCurrentTestCase;
extern std::string g_csLogOutputDirectory;
extern uint8_t g_bTLNotCompliant;

ocGenTLTestSuite* g_poGenTLTestSuite=NULL;

uint32_t g_zTotalTestCases=0;
uint8_t g_bStopValidationFlag=0;
uint8_t g_bCreateTestEnumerationFlag=0;

uint32_t g_zMandatoryResult=0;
uint32_t g_zRecommendedResult=0;
uint32_t g_zTestTimeout=TEST_TIMEOUT_DEFAULT;

//////////////////////////////////////////////////////
// Test categories
// adds all tests to test suite
// execution will be done afterwards
//////////////////////////////////////////////////////

void xRegisterAllTests(std::vector<std::string> &vecGenTLList, int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    int i=0;

    for (size_t index=0; index<vecGenTLList.size(); index++)
    {
        g_strTLPath = vecGenTLList[index];

        vTestFnExportTestWrapper(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestLibrary_GetTLInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestLibraryTest(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestLibrary_GCGetInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestSystemTest(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSystem_TLUpdateInterface(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSystem_TLGetInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSystem_TLGetNumInterfaces(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSystem_TLGetInterfaceID(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSystem_TLOpenInterface(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSystem_TLGetInterfaceInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestInterfaceTest(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFGetInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFUpdateDeviceList(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFGetNumDevices(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFGetDeviceID(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFOpenDevice(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFGetDeviceInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestInterface_IFGetParentTL(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestDeviceTest(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDevice_DevGetInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDevice_DevGetNumDataStreams(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDevice_DevGetDataStreamID(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDevice_DevGetPort(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDevice_DevOpenDataStream(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDevice_DevGetParentIF(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestPort_GCGetPortInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCGetPortURL(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCGetNumPortURLs(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCGetPortURLInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCReadPort(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCWritePort(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCReadPortstacked(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestPort_GCWritePortstacked(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestDataStreamTest(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSGetInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSAnnounceBuffer(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSQueueBuffer(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSGetBufferInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSGetBufferID(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSStartAcquisition(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSStopAcquisition(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSAllocAndAnnounceBuffer(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSFlushQueue(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo); 
        vTestDataStream_DSRevokeBuffer(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSGetBufferChunkData(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestDataStream_DSGetParentDev(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        
        vTestSignaling_GCRegisterEvent(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_GCUnregisterEvent(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestSignaling_TLParamsLocked(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_StartDevice(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_StopDevice(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_EventGetInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_EventGetData(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_EventGetDataInfo(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_EventKill(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
        vTestSignaling_EventFlush(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

        vTestImpl_Complete(zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);
    }
}

//////////////////////////////////////////////////////
// test configuration
// OpenTestConfig will be called before execution of tests
// CloseTestConfig will be called after execution of tests
//////////////////////////////////////////////////////

// utilities
std::string sGetTimeStamp()
{
    struct _timeb timebuffer;
    struct tm timeinfo;
    char tmpbuf[128];

    _tzset();
    _ftime64_s(&timebuffer);
    time_t rawtime = timebuffer.time;
    _localtime64_s(&timeinfo, &rawtime);

    strftime(tmpbuf, 128, "%Y%m%d_%H%M%S", &timeinfo);
    sprintf_s(tmpbuf, 128, "%s_%03d", tmpbuf, timebuffer.millitm);

    return tmpbuf;
}

std::string sGetLogFileName()
{
    std::stringstream sFileName;

    if (!g_bCreateTestEnumerationFlag)
        sFileName << "TLv" << GenTLMajorVersion << GenTLMinorVersion << "_" << sGetTimeStamp().c_str() << ".txt";
    else
        sFileName << "TestCaseEnumeration" << GenTLMajorVersion << GenTLMinorVersion << "_" << g_csGenTLValid14FileVersion.c_str() << ".txt";


    return sFileName.str();
}

std::string sGetDisplayTimeStamp()
{
    char tmpbuf[128];
    std::string sResult;

    _tzset();
    _strtime_s( tmpbuf, 128 );
    sResult += tmpbuf;
    sResult += " ";
    _strdate_s( tmpbuf, 128 );
    sResult += tmpbuf;

    return sResult;
}

void OpenTestConfig()
{
    std::string sOutputFileName;
    std::stringstream sTotalTestCases;

    // Check parameter
    g_zTotalTestCases = g_zCurrentTestCase;

    // now we know the number of total test cases
    if (g_zSpecialTestCaseToDoFrom == TEST_CASES_BORDER)
        g_zSpecialTestCaseToDoFrom = 1;
    if (g_zSpecialTestCaseToDoTo == TEST_CASES_BORDER)
        g_zSpecialTestCaseToDoTo = g_zTotalTestCases;

    // set special output directory if set
    std::string current_path_str = _getcwd(NULL, 0);
    if (g_csLogOutputDirectory != "")
        current_path_str = g_csLogOutputDirectory;
    sOutputFileName = current_path_str + std::string("\\") + sGetLogFileName();

    // send message about filename to host process
    vSendCallbackMessage(DLL_CALLBACK_FILENAME, sOutputFileName);
    // send message about total test amount to host process
    if (g_zSpecialTestCaseToDoFrom != ALL_TEST_CASES)
    {
        if (g_zSpecialTestCaseToDoFrom > 0 && g_zSpecialTestCaseToDoFrom <= g_zTotalTestCases &&
            g_zSpecialTestCaseToDoTo > 0 && g_zSpecialTestCaseToDoTo <= g_zTotalTestCases)
        {
            sTotalTestCases << (g_zSpecialTestCaseToDoTo-g_zSpecialTestCaseToDoFrom+1);
        }
        else
        {
            sTotalTestCases << "out of range";
        }
    }
    else
        sTotalTestCases << g_zTotalTestCases;
    vSendCallbackMessage(DLL_CALLBACK_TOTALTESTAMOUNT, sTotalTestCases.str());
        
    g_poGenTLTestSuite->vSetLogFilename(sOutputFileName);
        
    GENTLTEST_PRINT("======================================================================" << std::endl);
    GENTLTEST_PRINT("GenICam Transport Layer Validation Framework Report" << std::endl);
    GENTLTEST_PRINT("----------------------------------------------------------------------" << std::endl);
    GENTLTEST_PRINT("Date                         : " << sGetDisplayTimeStamp().c_str() << std::endl);
    GENTLTEST_PRINT("Validation Framework Version : " << g_csGenTLValid14FileVersion.c_str() << std::endl);
    if (!g_bCreateTestEnumerationFlag)
        GENTLTEST_PRINT("TL file                      : " << g_strTLPath.c_str() << std::endl);

    if (g_zSpecialTestCaseToDoFrom != ALL_TEST_CASES)
    {
        if (g_zSpecialTestCaseToDoFrom > 0 && g_zSpecialTestCaseToDoFrom <= g_zTotalTestCases &&
            g_zSpecialTestCaseToDoTo > 0 && g_zSpecialTestCaseToDoTo <= g_zTotalTestCases)
        {
            GENTLTEST_PRINT("*************************************" <<std::endl);
            GENTLTEST_PRINT("* Range test mode" << std::endl);
            GENTLTEST_PRINT("* Running test " << g_zSpecialTestCaseToDoFrom << "-" << g_zSpecialTestCaseToDoTo << " of total " << g_zTotalTestCases << std::endl);
            GENTLTEST_PRINT("*************************************" <<std::endl);
        } 
        else
        {
            GENTLTEST_PRINT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<std::endl);
            GENTLTEST_PRINT("!!! Error: Only " << g_zTotalTestCases << " tests avalable, can not run test " << g_zSpecialTestCaseToDoFrom << "-" << g_zSpecialTestCaseToDoTo << std::endl);
            GENTLTEST_PRINT("!!! Error: Running 1st test case to check interface" << std::endl);
            GENTLTEST_PRINT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<std::endl);
                
        }
    }

    GENTLTEST_PRINT("Check for GenICam TL Version = " << GenTLMajorVersion << "." << GenTLMinorVersion << std::endl);
    GENTLTEST_PRINT("Running " << g_poGenTLTestSuite->nGetTestCount() << " test cases..." << std::endl);
}

void CloseTestConfig()
{
    uint32_t uiAssertFailed=0;
    uint32_t uiAssertPassed=0;
    uint32_t uiTestsAborted=0;
    uint32_t uiTestsSkipped=0;
    uint32_t uiTestsPassed=0;
    uint32_t uiTestsFailed=0;
    uint32_t uiTotalNumTests;
    ocGenTLTestSuite::tTestUnitIDList oTestUnitIDList=g_poGenTLTestSuite->tGetTestUnitIDList();
    std::stringstream oStrSkipped;
    std::stringstream oStrAborted;
    
    GENTLTEST_PRINT(std::endl << "======================================================================" << std::endl);
    GENTLTEST_PRINT("Summary" << std::endl);
    if (g_bStopValidationFlag)
        GENTLTEST_PRINT("Validation aborted." << std::endl);
    GENTLTEST_PRINT("----------------------------------------------------------------------" << std::endl);

    uiTotalNumTests = static_cast<uint32_t>(g_poGenTLTestSuite->nGetTestCount());
    
    for (std::size_t xIndex1=0; xIndex1<uiTotalNumTests; xIndex1++)
    {
        unsigned long id= oTestUnitIDList[xIndex1];
        unsigned long oAssertFailed = GENTLUNITTEST_GET_ASSERT_FAILED(id);
        unsigned long oAssertPassed = GENTLUNITTEST_GET_ASSERT_PASSED(id);
        bool oAborted = GENTLUNITTEST_GET_ABORTED(id);
        bool oSkipped = GENTLUNITTEST_GET_SKIPPED(id);
        
        if (oSkipped == true)
        {
            if (oStrSkipped.str() != "")
                oStrSkipped << ", ";
            oStrSkipped << xIndex1+1;

            size_t pos=oStrSkipped.str().rfind("\n");
            if (pos != std::string::npos) {
                std::string oTemp=oStrSkipped.str().substr(pos);
                if (oTemp.size() > 100)
                    oStrSkipped << std::endl;
            } else {
                if (oStrSkipped.str().size() > 100)
                    oStrSkipped << std::endl;
            }
        }
        if (oAborted == true)
        {
            if (oStrAborted.str() != "")
                oStrAborted << ", ";
            oStrAborted << xIndex1+1;

            size_t pos=oStrAborted.str().rfind("\n");
            if (pos != std::string::npos) {
                std::string oTemp=oStrAborted.str().substr(pos);
                if (oTemp.size() > 100)
                    oStrAborted << std::endl;
            } else {
                if (oStrAborted.str().size() > 100)
                    oStrAborted << std::endl;
            }

            ocGenTLTestCaseContainer::IssueContainer issue=g_poGenTLTestSuite->xGetAbortedIssue(id);
            GENTLTEST_PRINT("Test " << xIndex1+1 << " " << "aborted. file:" << issue.file_name << ", line:" << issue.line << ", function:" << issue.function << std::endl);
        }
        if (oAssertFailed > 0 && oAborted == false && oSkipped == false)
        {
            ocGenTLTestCaseContainer::vecIssueContainers issues=g_poGenTLTestSuite->xGetAssertionsFailedIssues(id);
            for (size_t xIndex2=0; xIndex2<issues.size(); xIndex2++)
            {
                GENTLTEST_PRINT("Test " << xIndex1+1 << " " << "failed => file:" << issues[xIndex2].file_name << ", line:" << issues[xIndex2].line << ", function:" << issues[xIndex2].function << std::endl);
            }
        }

        uiAssertFailed += oAssertFailed;
        uiAssertPassed += oAssertPassed;
        uiTestsAborted += oAborted;
        uiTestsSkipped += oSkipped;
        uiTestsPassed += (oAssertFailed==0 && !oAborted && !oSkipped)?1:0;
        uiTestsFailed += (oAssertFailed>0 && !oAborted && !oSkipped)?1:0;

        
    }

    if (oStrAborted.str() != "")
        GENTLTEST_PRINT("Test " << oStrAborted.str().c_str() << " aborted." << std::endl);
    if (oStrSkipped.str() != "")
        GENTLTEST_PRINT("Test " << oStrSkipped.str().c_str() << " " << " skipped." << std::endl);

    GENTLTEST_PRINT("----------------------------------------------------------------------" << std::endl);
    GENTLTEST_PRINT("Number of test units ran             : " << uiTotalNumTests << std::endl);
    GENTLTEST_PRINT("Number of test units passed          : " << uiTestsPassed << std::endl);
    GENTLTEST_PRINT("Number of test units failed          : " << uiTestsFailed << std::endl);
    GENTLTEST_PRINT("Number of test units aborted         : " << uiTestsAborted << std::endl);
    GENTLTEST_PRINT("Number of test units skipped         : " << uiTestsSkipped << std::endl);
    GENTLTEST_PRINT("Number of total assertions succeeded : " << uiAssertPassed << std::endl);
    GENTLTEST_PRINT("Number of total assertions failed    : " << uiAssertFailed << std::endl);
    //SVW TODO: GENTLTEST_PRINT("Number of mandatory result values    : " << g_zMandatoryResult << std::endl);
    //SVW TODO: GENTLTEST_PRINT("Number of recommended result values  : " << g_zRecommendedResult << std::endl);
    
    if (!g_bReusePayloadsize && g_zTotalTestCases == uiTotalNumTests && !g_bStopValidationFlag)
    {
        double dResult=0;
        double dTotalAsserts = uiAssertPassed + uiAssertFailed;
        std::string sVerdict="NOT COMPLIANT";

        if (uiTotalNumTests > 0)
        {
            if (uiTestsPassed > 0)
            {
                std::stringstream oResult;
                dResult = (((double)uiTestsPassed+(double)uiTestsSkipped) / (double)uiTotalNumTests) * 100.0;
                oResult << "Success Rate: " << std::setw(3) << dResult << "%";
                GENTLTEST_PRINT(oResult.str().c_str() << std::endl);
                vSendCallbackMessage(DLL_CALLBACK_MESSAGE, oResult.str());
            }
            else
            {
                GENTLTEST_PRINT("Success Rate: 0.000%." << std::endl);
                vSendCallbackMessage(DLL_CALLBACK_MESSAGE, "Success Rate: 0.000%.");
            }
        }
        else
        {
            GENTLTEST_PRINT("Success Rate: no tests processed." << std::endl);
            vSendCallbackMessage(DLL_CALLBACK_MESSAGE, "Success Rate: no tests processed.");
        }

        if (dResult == 100.0) 
        {
            sVerdict = "COMPLIANT";
            g_bTLNotCompliant = 0;
        }

        GENTLTEST_PRINT("----------------------------------------------------------------------" << std::endl);
        GENTLTEST_PRINT("Verdict: " << sVerdict.c_str() << std::endl);
    
        vSendCallbackMessage(DLL_CALLBACK_TESTRESULT, sVerdict);
        vSendCallbackMessage(DLL_CALLBACK_MESSAGE, std::string("Validation result is ") + sVerdict);
        if (dResult != 100.0) 
            vSendCallbackMessage(DLL_CALLBACK_MESSAGE, "Switch on 'CTI Interface log' and run once more for detailed investigation.");
    }

    GENTLTEST_PRINT("======================================================================" << std::endl);
}

//////////////////////////////////////////////////////
// initializes GenTL test run
//////////////////////////////////////////////////////

void init_unit_test_suite()
{
    int i=0;
    int64_t zSpecialTestCaseToDoFrom=g_zSpecialTestCaseToDoFrom;
    int64_t zSpecialTestCaseToDoTo=g_zSpecialTestCaseToDoTo;

    // check all files
    for (size_t index=0; index<g_vecGenTLList.size(); index++)
    {
        std::string pathname = g_vecGenTLList[index];
        if (pathname.find(".cti") == std::string::npos) 
        {
            GENTLTEST_PRINT("Error: in path #" << ++i << " TL file = " << g_strTLPath.c_str() << std::endl);
            GENTLTEST_PRINT("Missing cti extension." << std::endl);
            continue;
        }
    }

    if (zSpecialTestCaseToDoFrom == TEST_CASES_BORDER)
        zSpecialTestCaseToDoFrom = 1;
    if (zSpecialTestCaseToDoTo == TEST_CASES_BORDER)
        zSpecialTestCaseToDoTo = 999999999;
    xRegisterAllTests(g_vecGenTLList, zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

    if (g_poGenTLTestSuite->nGetTestCount() == 0)
    {
        vSendCallbackMessage(DLL_CALLBACK_ERROR, "No tests available.");
    }
}

//////////////////////////////////////////////////////
// switch test timeout
// for debugging it is possible to switch off the test conrolling timeout
//////////////////////////////////////////////////////

int nSwitchTestTimeout(uint8_t zFlag)
{
    if (zFlag > 0)
        g_zTestTimeout = TEST_TIMEOUT_DEFAULT;
    else
        g_zTestTimeout = 0;

    return 0;
}

//////////////////////////////////////////////////////
// create test enumeration
// creates a file which maps test number to test description
//////////////////////////////////////////////////////

int nCreateTestEnumeration()
{
    g_bCreateTestEnumerationFlag = 1;

    return 0;
}

//////////////////////////////////////////////////////
// stop validation
// sets stop flag, see GENTLTEST_DESCRIPTION in GenTLTestTools.h
// the stop flag will abort every following test without result.
//////////////////////////////////////////////////////

int nStopTestRunMain()
{
    g_bStopValidationFlag = 1;

    return 0;
}

//////////////////////////////////////////////////////
// main access
//////////////////////////////////////////////////////

int nStartTestRunMain(int argc, char* argv[])
{
    g_bStopValidationFlag = 0;

    g_poGenTLTestSuite = new ocGenTLTestSuite("GenICam validation test suite");

    init_unit_test_suite();

    try {
        OpenTestConfig();

        g_poGenTLTestSuite->vRun();

        if (!g_bCreateTestEnumerationFlag)
            CloseTestConfig();

        FnExportTest::vExitInstance();

        if (g_poGenTLTestSuite != NULL)
        {
            delete g_poGenTLTestSuite;
            g_poGenTLTestSuite = NULL;
        }

        return 0;
    }
    catch( ... ) {
        std::stringstream oStream;
        oStream << "Test framework internal error: unknown reason" << std::endl;
        vSendCallbackMessage(DLL_CALLBACK_ERROR, oStream.str());
        
        return -1;
    }
}

