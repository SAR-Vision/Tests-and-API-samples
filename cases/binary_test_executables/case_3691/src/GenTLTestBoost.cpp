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
#include <vector>
#include <string>
#include <stdio.h>

#include "GenTLTestBoost.h"
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
// total summary of boost unit test
// called by BoostTestConfig destructor
//////////////////////////////////////////////////////

void vBoostTestSummary()
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

    uiTotalNumTests = static_cast<uint32_t>(g_poGenTLTestSuite->size());
    
    for (std::size_t xIndex=0; xIndex<g_poGenTLTestSuite->size(); xIndex++)
    {
        boost::unit_test::test_unit_id id= oTestUnitIDList[xIndex];
        boost::unit_test::test_results::counter_prop oAssertFailed = boost::unit_test::results_collector_t::instance().results(id).p_assertions_failed;
        boost::unit_test::test_results::counter_prop oAssertPassed = boost::unit_test::results_collector_t::instance().results(id).p_assertions_passed;
        //boost::unit_test::test_results::counter_prop oExpectedFailures = boost::unit_test::results_collector_t::instance().results(id).p_expected_failures;
        //boost::unit_test::test_results::counter_prop oCasesPassed = boost::unit_test::results_collector_t::instance().results(id).p_test_cases_passed;
        //boost::unit_test::test_results::counter_prop oCasesFailed = boost::unit_test::results_collector_t::instance().results(id).p_test_cases_failed;
        //boost::unit_test::test_results::counter_prop oCasesSkipped = boost::unit_test::results_collector_t::instance().results(id).p_test_cases_skipped;
        //boost::unit_test::test_results::counter_prop oCasesAborted = boost::unit_test::results_collector_t::instance().results(id).p_test_cases_aborted;
        boost::unit_test::test_results::bool_prop oAborted = boost::unit_test::results_collector_t::instance().results(id).p_aborted;
        boost::unit_test::test_results::bool_prop oSkipped = boost::unit_test::results_collector_t::instance().results(id).p_skipped;
        
        if (oSkipped == true)
        {
            if (oStrSkipped.str() != "")
                oStrSkipped << ", ";
            oStrSkipped << xIndex+1;

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
            oStrAborted << xIndex+1;

            size_t pos=oStrAborted.str().rfind("\n");
            if (pos != std::string::npos) {
                std::string oTemp=oStrAborted.str().substr(pos);
                if (oTemp.size() > 100)
                    oStrAborted << std::endl;
            } else {
                if (oStrAborted.str().size() > 100)
                    oStrAborted << std::endl;
            }
        }
        if (oAssertFailed > 0 && oAborted == false && oSkipped == false)
        {
            GENTLTEST_PRINT("Test " << xIndex+1 << " " << "failed." << std::endl);
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
    
    if (!g_bReusePayloadsize && !g_bStopValidationFlag)
    {
        double dResult=0;
        double dTotalAsserts = uiAssertPassed + uiAssertFailed;
        std::string sVerdict="NOT COMPLIANT";

        if (uiTotalNumTests > 0)
        {
            if (uiTestsPassed > 0)
            {
                std::stringstream oResult;
                dResult = ((double)uiTestsPassed / (double)uiTotalNumTests) * 100.0;
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
// Test categories
// adds all tests to boost test suite
// execution will be done afterwards
//////////////////////////////////////////////////////

boost::unit_test::test_suite* xDoTest(std::vector<std::string> &vecGenTLList, int64_t zSpecialTestCaseToDoFrom, int64_t zSpecialTestCaseToDoTo)
{
    int i=0;

    BOOST_FOREACH(std::string pathname, vecGenTLList)
    {
        g_strTLPath = pathname;

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

    return g_poGenTLTestSuite;
}

//////////////////////////////////////////////////////
// boost test output formatter
// derived boost class to format the output
//////////////////////////////////////////////////////

struct this_test_log_formatter : public boost::unit_test::output::compiler_log_formatter
{
    this_test_log_formatter()
    : m_let(BOOST_UTL_ET_INFO)
    {
    }

    void log_entry_start( std::ostream& output, boost::unit_test::log_entry_data const& logData, log_entry_types let )
    {
        m_sMessage.str("");
        m_sOutput.str("");
        m_sOutput << "           details: ";
        m_let = let;
        if (let >= BOOST_UTL_ET_ERROR)
        {
            output << "*** FAILURE ***" << std::endl
                   << "           source : '" << logData.m_file_name << "'" << std::endl
                   << "           line   : " << logData.m_line_num << std::endl;
        }
        else if (let == BOOST_UTL_ET_MESSAGE)
        {
            output << "";
        }
        else 
        {
            output << logData.m_file_name << "(" << logData.m_line_num << ")";
        }
    }
    void log_entry_value( std::ostream& output, boost::unit_test::const_string value )
    {
        if (m_let >= BOOST_UTL_ET_ERROR)
        {
            m_sOutput << value;
            m_sMessage << value;
        }
        else
        {
            output << value;
        }
    }
    void log_entry_value( std::ostream& output, boost::unit_test::lazy_ostream const& value )
    {
        if (m_let >= BOOST_UTL_ET_ERROR)
        {
            m_sOutput << value;
            m_sMessage << value;
        }
        else
        {
            output << value;
        }
    }
    void log_entry_finish( std::ostream& output)
    {
        if (m_let == BOOST_UTL_ET_ERROR)
        {
            if (!g_bStopValidationFlag && !g_bCreateTestEnumerationFlag)
            {
                vSendCallbackMessage(DLL_CALLBACK_ERROR, m_sMessage.str());
            }
            output << m_sOutput.str().c_str() << std::endl;
            output << "-";
            output << std::endl;
        }
        if (m_let == BOOST_UTL_ET_FATAL_ERROR)
        {
            m_sMessage << "  !!! Fatal error: test aborted !!!";
            m_sOutput << std::endl << "!!! Fatal error: test aborted !!!" << std::endl;
            output << m_sOutput.str().c_str();
            if (!g_bStopValidationFlag && !g_bCreateTestEnumerationFlag)
            {
                vSendCallbackMessage(DLL_CALLBACK_TESTABORTED, m_sMessage.str());
            }
        }
    }
    
private:
    log_entry_types m_let;
    std::stringstream m_sOutput;
    std::stringstream m_sMessage;
};

//////////////////////////////////////////////////////
// boost test configuration
// main test class for boost unit test configuration
// constructor will be called before execution of tests
// destructor will be called after execution of tests
//////////////////////////////////////////////////////

struct BoostTestConfig
{
public:
    BoostTestConfig()
    {
        std::stringstream sTotalTestCases;

        // Check parameter
        g_zTotalTestCases = g_zCurrentTestCase;

        // now we know the number of total test cases
        if (g_zSpecialTestCaseToDoFrom == TEST_CASES_BORDER)
            g_zSpecialTestCaseToDoFrom = 1;
        if (g_zSpecialTestCaseToDoTo == TEST_CASES_BORDER)
            g_zSpecialTestCaseToDoTo = g_zTotalTestCases;

        // set logging formats
        boost::unit_test::unit_test_log.set_format(boost::unit_test::CLF);
        boost::unit_test::unit_test_log.set_formatter(new this_test_log_formatter);
        
        boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages);

        // set special output directory if set
        std::string current_path_str = boost::filesystem::current_path().string(); 
        if (g_csLogOutputDirectory != "")
            current_path_str = g_csLogOutputDirectory;
        m_sOutputFile = current_path_str + std::string("\\") + sGetLogFileName();

        // send message about filename to host process
        vSendCallbackMessage(DLL_CALLBACK_FILENAME, m_sOutputFile);
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
        
        logFile.open(m_sOutputFile.c_str()); 
        boost::unit_test::unit_test_log.set_stream(logFile); 
        boost::unit_test::results_reporter::set_stream(logFile); 

        GENTLTEST_PRINT("======================================================================" << std::endl);
        GENTLTEST_PRINT("GenICam Transport Layer Validation Framework Report" << std::endl);
        GENTLTEST_PRINT("----------------------------------------------------------------------" << std::endl);
        GENTLTEST_PRINT("Date                         : " << sGetDisplayTimeStamp().c_str() << std::endl);
        GENTLTEST_PRINT("Validation Framework Version : " << g_csGenTLValid14FileVersion.c_str() << " (BOOST)" << std::endl);
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
    }
    ~BoostTestConfig()
    {
        if (g_zSpecialTestCaseToDoFrom == ALL_TEST_CASES && g_bCreateTestEnumerationFlag == 0)
            vBoostTestSummary();

        //boost::unit_test::results_reporter::detailed_report();

        boost::unit_test::results_reporter::set_stream(std::cout); 
        boost::unit_test::unit_test_log.set_stream(std::cout); 

        if (!logFile.bad())
        {
            logFile << std::flush; 
            logFile.close(); 
        }
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


private:
    std::ofstream logFile; 
    std::string m_sOutputFile;
};

BOOST_GLOBAL_FIXTURE(BoostTestConfig);

//////////////////////////////////////////////////////
// boost main (now called by local process main, see below)
//////////////////////////////////////////////////////

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
    int i=0;
    boost::unit_test::test_suite* xSuite=NULL;
    int64_t zSpecialTestCaseToDoFrom=g_zSpecialTestCaseToDoFrom;
    int64_t zSpecialTestCaseToDoTo=g_zSpecialTestCaseToDoTo;

    // check all files
    BOOST_FOREACH(std::string pathname, g_vecGenTLList)
    {
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
    xSuite = xDoTest(g_vecGenTLList, zSpecialTestCaseToDoFrom, zSpecialTestCaseToDoTo);

    if (g_poGenTLTestSuite->size() == 0)
    {
        vSendCallbackMessage(DLL_CALLBACK_ERROR, "No tests available.");
    }
    
    // return list of tests
    return xSuite;
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
// a running boost unit test is not stoppable
// sets stop flag, see GENTLTEST_DESCRIPTION in GenTLTestTools.h
// the stop flag will abort every following test without result.
//////////////////////////////////////////////////////

int nStopTestRunMain()
{
    g_bStopValidationFlag = 1;

    return 0;
}

//////////////////////////////////////////////////////
// initialize boost main
//////////////////////////////////////////////////////

int nStartTestRunMain(int argc, char* argv[])
{
    g_bStopValidationFlag = 0;

    g_poGenTLTestSuite = new ocGenTLTestSuite("GenICam validation test suite");

    // SVW this is a copy of boost main internal to get access to original command line parameter
    // during boost::unit_test::framework::init the command line parameter will change if a parameter in "" has
    // space characters. This parameter will be interpreted as two parameter.
    // To avoid this we catch the parameter before boost::unit_test::framework::init will be called.
    boost::unit_test::init_unit_test_func init_func = &init_unit_test_suite;

    try {
        boost::unit_test::framework::init( init_func, argc, argv );
        
        boost::unit_test::framework::run();

        boost::unit_test::results_reporter::make_report();

        FnExportTest::vExitInstance();

        if (g_poGenTLTestSuite != NULL)
        {
            delete g_poGenTLTestSuite;
            g_poGenTLTestSuite = NULL;
        }

        boost::unit_test::framework::reset_observers();

        return boost::unit_test::runtime_config::no_result_code() 
                    ? boost::exit_success
                    : boost::unit_test::results_collector.results( boost::unit_test::framework::master_test_suite().p_id ).result_code();
    }
    catch( boost::unit_test::framework::nothing_to_test const& ) {
        vSendCallbackMessage(DLL_CALLBACK_ERROR, "Nothing to test");

        return boost::exit_success;
    }
    catch( boost::unit_test::framework::internal_error const& ex ) {
        std::stringstream oStream;
        oStream << "Boost.Test framework internal error: " << ex.what() << std::endl;
        boost::unit_test::results_reporter::get_stream() << oStream;
        vSendCallbackMessage(DLL_CALLBACK_ERROR, oStream.str());
        
        return boost::exit_exception_failure;
    }
    catch( boost::unit_test::framework::setup_error const& ex ) {
        std::stringstream oStream;
        oStream << "Test setup error: " << ex.what() << std::endl;
        boost::unit_test::results_reporter::get_stream() << oStream;
        vSendCallbackMessage(DLL_CALLBACK_ERROR, oStream.str());

        return boost::exit_exception_failure;
    }
    catch( ... ) {
        std::stringstream oStream;
        oStream << "Boost.Test framework internal error: unknown reason" << std::endl;
        boost::unit_test::results_reporter::get_stream() << oStream;
        vSendCallbackMessage(DLL_CALLBACK_ERROR, oStream.str());
        
        return boost::exit_exception_failure;
    }
}

