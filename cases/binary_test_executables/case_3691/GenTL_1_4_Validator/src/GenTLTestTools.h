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

#ifndef GENTLTESTTOOLS_INCLUDE___
#define GENTLTESTTOOLS_INCLUDE___

#include <iomanip>
#include <string>
#include <sstream>
#include "GenTL_v1_4.h"
#include <basetsd.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

#include "GenTLTestSuite.h"
#include "GenTLValidDll.h"
#ifdef _USING_BOOST
#include "GenTLTestTimerBoost.h"
#else /* _USING_BOOST */
#include "GenTLTestTimer.h"
#endif /* _USING_BOOST */

#include "GenTLTestUnitTest.h"

class ocGenTLTestSuite;
extern ocGenTLTestSuite* g_poGenTLTestSuite;
extern uint8_t g_bStopValidationFlag;
extern uint8_t g_bCreateTestEnumerationFlag;
extern uint8_t g_bLogCTIInterfaceFlag;

extern uint32_t g_zMandatoryResult;
extern uint32_t g_zRecommendedResult;
extern uint32_t g_zTestTimeout;

/////////////////////////////////////////////////////////////////////////////////////////
// forward declaration section
/////////////////////////////////////////////////////////////////////////////////////////

void vDisplayError(std::string csFunction);
std::string sConvertGCError2String(GenICam::Client::GC_ERROR nResult);
std::string sConvertTLInfoCommand2String(GenICam::Client::TL_INFO_CMD eCommand);
std::string sConvertPORTCommand2String(GenICam::Client::PORT_INFO_CMD_LIST eCommand);
std::string sConvertDeviceCommand2String(GenICam::Client::DEVICE_INFO_CMD_LIST eCommand);
std::string sConvertDEVICEAccess2String(GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess);
std::string sConvertDataType2String(GenICam::Client::INFO_DATATYPE eDataType);
std::string sConvertURLInfoCommand2String(GenICam::Client::URL_INFO_CMD_LIST eCommand);
std::string sConvertInterfaceCommand2String(GenICam::Client::INTERFACE_INFO_CMD eCommand);
std::string sConvertDataStreamCommand2String(GenICam::Client::STREAM_INFO_CMD eCommand);
std::string sConvertDataStreamStartFlag2String(GenICam::Client::ACQ_START_FLAGS eStartFlag);
std::string sConvertDataStreamStopFlag2String(GenICam::Client::ACQ_STOP_FLAGS eStopFlag);
std::string sConvertDataStreamOperation2String(GenICam::Client::ACQ_QUEUE_TYPE eOperation);
std::string sConvertDataStreamBufferCommand2String(GenICam::Client::BUFFER_INFO_CMD eCommand);
std::string sConvertEventID2String(GenICam::Client::EVENT_TYPE_LIST eEventID);
std::string sConvertEventInfoCommand2String(GenICam::Client::EVENT_INFO_CMD eCommand);
std::string sConvertEventDataInfoCommand2String(GenICam::Client::EVENT_DATA_INFO_CMD eCommand);
std::string sConvertAccessStatus2String(GenICam::Client::DEVICE_ACCESS_STATUS eAccess);
std::string sConvertPayloadtype2String(GenICam::Client::PAYLOADTYPE_INFO_IDS ePayloadtype);
std::string csConvertINTHexToDec(std::string sValue);
std::string sConvertEndianness2String(int32_t endianness);

int16_t xConvertBuffer2INT16(const char *buffer);
uint16_t xConvertBuffer2UINT16(const char *buffer);
int32_t xConvertBuffer2INT32(const char *buffer);
uint32_t xConvertBuffer2UINT32(const char *buffer);
int64_t xConvertBuffer2INT64(const char *buffer);
uint64_t xConvertBuffer2UINT64(const char *buffer);
double xConvertBuffer2FLOAT64(const char *buffer);
uint64_t xConvertBuffer2PTR(const char *buffer, const std::size_t iSize);
size_t xConvertBuffer2SIZET(const char *buffer, const std::size_t iSize);
std::string sConvertSizeT2String(size_t value);
std::string sConvertBool82String(bool8_t value);
std::string sConvertUint322String(uint32_t value);
std::string sConvertUint642String(uint64_t value);
std::string sConvertUint642HexString(uint64_t value);
std::string sConvertVoidPointer2String(void *value);

void vTestPixelFormat(std::string sPixelFormat);
void vTestAcquisitionMode(std::string sAcquisitionMode);

void vDumpiTypeData(const std::string sMsgText, 
                    const GenICam::Client::INFO_DATATYPE iType, 
                    const char *pcBuff, 
                    const std::size_t iSize);

void GenTLTest_timer_handler(ocGenTLTestTimer *poTimer);

/////////////////////////////////////////////////////////////////////////////////////////
// definition section
/////////////////////////////////////////////////////////////////////////////////////////

#define TEST_TIMEOUT_DEFAULT    720000

#define ALL_TEST_CASES          GENTL_INFINITE
#define TEST_CASES_BORDER       -2

#define GENTLTEST_LOG( message )                                                                                                            \
{                                                                                                                                           \
    if (g_bLogCTIInterfaceFlag)                                                                                                             \
    {                                                                                                                                       \
        struct _timeb timebuffer;                                                                                                           \
        std::stringstream oStreamOut;                                                                                                       \
        struct tm timeinfo;                                                                                                                 \
                                                                                                                                            \
        _ftime64_s(&timebuffer);                                                                                                            \
        time_t rawtime = timebuffer.time;                                                                                                   \
        localtime_s(&timeinfo, &rawtime );                                                                                                  \
                                                                                                                                            \
        oStreamOut << "Log: [" ;                                                                                                            \
        oStreamOut << std::setfill('0') << std::setw(2) << timeinfo.tm_hour << ":";                                                         \
        oStreamOut << std::setfill('0') << std::setw(2) << timeinfo.tm_min << ":";                                                          \
        oStreamOut << std::setfill('0') << std::setw(2) << timeinfo.tm_sec << ".";                                                          \
        oStreamOut << std::setfill('0') << std::setw(3) << timebuffer.millitm;                                                              \
        oStreamOut << "] " << message;                                                                                                      \
        GENTLUNITTEST_MESSAGE(oStreamOut.str().c_str());                                                                                    \
    }                                                                                                                                       \
}

#define GENTLTEST_PRINT( message )                                                                                                          \
{                                                                                                                                           \
    std::stringstream oStreamOut;                                                                                                           \
                                                                                                                                            \
    oStreamOut << message;                                                                                                                  \
    GENTLUNITTEST_MESSAGE(oStreamOut.str().c_str());                                                                                        \
}

#define GENTLTEST_DESCRIPTION_NOTE( message )                                                                                               \
{                                                                                                                                           \
    std::stringstream oStream;                                                                                                              \
                                                                                                                                            \
    oStream << message;                                                                                                                     \
    GENTLTEST_PRINT(oStream.str().c_str());                                                                                                 \
}

#define GENTLTEST_PRINT_RESULT( id )                                                                                                        \
{                                                                                                                                           \
    std::stringstream oTestNumber;                                                                                                          \
    unsigned long oAssertFailed=0;                                                                                                          \
    bool oAborted=false;                                                                                                                    \
                                                                                                                                            \
    oTestNumber << g_poGenTLTestSuite->xGetTestUnitIndex(id);                                                                               \
                                                                                                                                            \
    oAssertFailed = GENTLUNITTEST_GET_ASSERT_FAILED(id)                                                                                     \
    oAborted = GENTLUNITTEST_GET_ABORTED(id);                                                                                               \
    if (id == -1)                                                                                                                           \
    {                                                                                                                                       \
        GENTLTEST_PRINT("Test Unit ID not found\n");                                                                                        \
    }                                                                                                                                       \
    if (oAborted == 0 && oAssertFailed == 0)                                                                                                \
    {                                                                                                                                       \
        GENTLTEST_PRINT("OK\n");                                                                                                            \
                                                                                                                                            \
        vSendCallbackMessage(DLL_CALLBACK_TESTSUCCEEDED, oTestNumber.str());                                                                \
    }                                                                                                                                       \
    else                                                                                                                                    \
    {                                                                                                                                       \
        vSendCallbackMessage(DLL_CALLBACK_TESTFAILED, oTestNumber.str());                                                                   \
    }                                                                                                                                       \
}

#define GENTLTEST_PRINT_RESULT_NOTE( id )                                                                                                   \
{                                                                                                                                           \
    unsigned long oAssertFailed=0;                                                                                                          \
    bool oAborted=false;                                                                                                                    \
                                                                                                                                            \
    oAssertFailed = GENTLUNITTEST_GET_ASSERT_FAILED(id)                                                                                     \
    oAborted = GENTLUNITTEST_GET_ABORTED(id);                                                                                               \
    if (id == -1)                                                                                                                           \
    {                                                                                                                                       \
        GENTLTEST_PRINT("Test Unit ID not found");                                                                                          \
    }                                                                                                                                       \
    if (oAborted == 0 && oAssertFailed == 0)                                                                                                \
    {                                                                                                                                       \
        GENTLTEST_PRINT("OK");                                                                                                              \
    }                                                                                                                                       \
}

#define GENTLTEST_DESCRIPTION( id, message )                                                                                                \
{                                                                                                                                           \
    std::stringstream oStream;                                                                                                              \
    std::stringstream oTestNumber;                                                                                                          \
                                                                                                                                            \
    oTestNumber << g_poGenTLTestSuite->xGetTestUnitIndex(id);                                                                               \
                                                                                                                                            \
    if (!g_bCreateTestEnumerationFlag)                                                                                                      \
    {                                                                                                                                       \
        oStream << std::endl << "======================================================================" << std::endl                       \
                << oTestNumber.str().c_str() << ". " << message  << std::endl;                                                              \
    }                                                                                                                                       \
    else                                                                                                                                    \
    {                                                                                                                                       \
        oStream << oTestNumber.str().c_str() << ". " << message  << std::endl;                                                              \
    }                                                                                                                                       \
    GENTLTEST_PRINT(oStream.str().c_str());                                                                                                 \
                                                                                                                                            \
    vSendCallbackMessage(DLL_CALLBACK_TESTNUMBER, oTestNumber.str());                                                                       \
    if (g_bCreateTestEnumerationFlag)                                                                                                       \
    {                                                                                                                                       \
        GENTLTEST_REQUIRE(true);                                                                                                            \
        return;                                                                                                                             \
    }                                                                                                                                       \
                                                                                                                                            \
    if (g_bStopValidationFlag)                                                                                                              \
    {                                                                                                                                       \
        GENTLTEST_SKIP("Validation aborted.");                                                                                              \
        return;                                                                                                                             \
    }                                                                                                                                       \
}

#define GENTLTEST_CHECK( condition )                                                                                                        \
{                                                                                                                                           \
    GENTLUNITTEST_CHECK( (condition) );                                                                                                     \
    if (condition)                                                                                                                          \
    {                                                                                                                                       \
        g_zRecommendedResult++;                                                                                                             \
    }                                                                                                                                       \
}

#define GENTLTEST_CHECK_MESSAGE( message, condition )                                                                                       \
{                                                                                                                                           \
    std::stringstream oMessageStream;                                                                                                       \
                                                                                                                                            \
    oMessageStream << __FUNCTION__ << ": " << message;                                                                                      \
    GENTLUNITTEST_CHECK_MESSAGE( (condition), oMessageStream.str().c_str() )                                                                \
    if (condition)                                                                                                                          \
    {                                                                                                                                       \
        g_zRecommendedResult++;                                                                                                             \
    }                                                                                                                                       \
}                                                                           

#define GENTLTEST_CHECK_RESULT( recommended_message, recommended_condition, mandatory_message, mandatory_condition )                        \
{                                                                                                                                           \
    std::stringstream oMandatoryMessage;                                                                                                    \
    std::stringstream oRecommendedMessage;                                                                                                  \
    bool bRecCondition=recommended_condition;                                                                                               \
                                                                                                                                            \
    oMandatoryMessage << __FUNCTION__ << ": " << mandatory_message;                                                                         \
    GENTLUNITTEST_CHECK_MESSAGE( (mandatory_condition), oMandatoryMessage.str().c_str() );                                                  \
    if (bRecCondition)                                                                                                                      \
    {                                                                                                                                       \
        g_zRecommendedResult++;                                                                                                             \
    }                                                                                                                                       \
    else                                                                                                                                    \
    {                                                                                                                                       \
        oRecommendedMessage << recommended_message;                                                                                         \
        g_zMandatoryResult++;                                                                                                               \
        GENTLTEST_PRINT(oRecommendedMessage.str().c_str() << std::endl);                                                                    \
    }                                                                                                                                       \
}

#define GENTLTEST_REQUIRE( condition )                                                                                                      \
{                                                                                                                                           \
    GENTLUNITTEST_REQUIRE( (condition) );                                                                                                   \
}

#define GENTLTEST_SKIP( message )                                                                                                           \
{                                                                                                                                           \
    std::stringstream oMessageStream;                                                                                                       \
                                                                                                                                            \
    oMessageStream << "Skipped: " << message << "\n";                                                                                       \
                                                                                                                                            \
    GENTLUNITTEST_SKIP( oMessageStream.str().c_str() );                                                                                     \
}

#define GENTLTEST_REQUIRE_MESSAGE( message, condition )                                                                                     \
{                                                                                                                                           \
    std::stringstream oMessageStream;                                                                                                       \
                                                                                                                                            \
    oMessageStream << __FUNCTION__ << ": " << message;                                                                                      \
    GENTLUNITTEST_REQUIRE_MESSAGE( (condition), oMessageStream.str().c_str() );                                                             \
}       

#define GENTLTEST_EXCEPTION_MESSAGE( func )                                                                                                 \
{                                                                                                                                           \
    std::stringstream oMessageStream;                                                                                                       \
    std::stringstream oErrorStream;                                                                                                         \
                                                                                                                                            \
    oMessageStream << __FUNCTION__ << std::endl;                                                                                            \
    oMessageStream << "***************************************" << std::endl;                                                               \
    oMessageStream << "* FATAL ERROR:" << std::endl;                                                                                        \
    oMessageStream << "* " << func << " raised an exception" << std::endl;                                                                  \
    oMessageStream << "***************************************" << std::endl;                                                               \
    oMessageStream << "* Validation will abort." << std::endl;                                                                              \
    GENTLUNITTEST_MESSAGE( oMessageStream.str().c_str() );                                                                                  \
    oErrorStream << " FATAL ERROR: " << func << " raised an exception";                                                                     \
    vSendCallbackMessage(DLL_CALLBACK_ERROR, oErrorStream.str());                                                                           \
    vSendCallbackMessage(DLL_CALLBACK_ERROR, "Validation will abort.");                                                                     \
}       

#define GENTLTEST_ADD_TEST_UNIT( testObject, testMethod )                                                                                   \
    g_zCurrentTestCase++;                                                                                                                   \
    if (zSpecialTestCaseToDoFrom == ALL_TEST_CASES ||                                                                                       \
        (g_zCurrentTestCase >= zSpecialTestCaseToDoFrom && g_zCurrentTestCase <= zSpecialTestCaseToDoTo))                                   \
    {                                                                                                                                       \
        GENTLUNITTEST_ADD_TEST_UNIT(g_zCurrentTestCase, vTest_ ## testObject ## _ ## testMethod);                                           \
    }

#define GENTLTEST_GET_TEST_UNIT_ID( function )                                                                                              \
    g_poGenTLTestSuite->xGetTestUnitID(function);

#define GENTLTEST_UNIT_DECL( testObject, testMethod )                                                                                       \
void vTest_ ## testObject ## _ ## testMethod();

#define GENTLTEST_UNIT_IMPL( testObject, testMethod )                                                                                       \
void vTest_ ## testObject ## _ ## testMethod()                                                                                              \
{                                                                                                                                           \
    ocGenTLTestTimer oTimer;                                                                                                                \
                                                                                                                                            \
    if (!g_bStopValidationFlag && g_zTestTimeout > 0)                                                                                       \
    {                                                                                                                                       \
        oTimer.set_interval(g_zTestTimeout);                                                                                                \
        oTimer.set_threadid(GENTLUNITTEST_GETTHREADID);                                                                                     \
        oTimer.connect(GenTLTest_timer_handler);                                                                                            \
        oTimer.start();                                                                                                                     \
    }                                                                                                                                       \
                                                                                                                                            \
    uint32_t id = GENTLTEST_GET_TEST_UNIT_ID(std::string("vTest_")+std::string(#testObject)+std::string("_")+std::string(#testMethod));     \
    testObject oTest;                                                                                                                       \
    oTest.testMethod(id);                                                                                                                   \
                                                                                                                                            \
    if (g_zTestTimeout > 0)                                                                                                                 \
    {                                                                                                                                       \
        oTimer.stop();                                                                                                                      \
    }                                                                                                                                       \
}

#define GENTLTEST_UNIT_IMPL_MSG( testObject, testMethod, message )                                                                          \
void vTest_ ## testObject ## _ ## testMethod()                                                                                              \
{                                                                                                                                           \
    ocGenTLTestTimer oTimer;                                                                                                                \
                                                                                                                                            \
    if (!g_bStopValidationFlag && g_zTestTimeout > 0)                                                                                       \
    {                                                                                                                                       \
        oTimer.set_interval(g_zTestTimeout);                                                                                                \
        oTimer.set_threadid(GENTLUNITTEST_GETTHREADID);                                                                                     \
        oTimer.connect(GenTLTest_timer_handler);                                                                                            \
        oTimer.start();                                                                                                                     \
    }                                                                                                                                       \
                                                                                                                                            \
    GENTLTEST_PRINT(std::endl << "**********************************************************************" << std::endl);                    \
    GENTLTEST_PRINT("==> Running " << message << ":" << std::endl);                                                                         \
    uint32_t id = GENTLTEST_GET_TEST_UNIT_ID(std::string("vTest_")+std::string(#testObject)+std::string("_")+std::string(#testMethod));     \
    testObject oTest;                                                                                                                       \
    oTest.testMethod(id);                                                                                                                   \
                                                                                                                                            \
    if (g_zTestTimeout > 0)                                                                                                                 \
    {                                                                                                                                       \
        oTimer.stop();                                                                                                                      \
    }                                                                                                                                       \
}

#define GENTLTEST_SLEEP( msec )  GENTLUNITTEST_SLEEP( msec ); 

#define GENTLTEST_DUMP_ITYPE( message, type, buffer, size )                                                                                 \
{                                                                                                                                           \
    std::stringstream sMsgText;                                                                                                             \
                                                                                                                                            \
    sMsgText << message;                                                                                                                    \
    vDumpiTypeData(sMsgText.str(), type, buffer, size);                                                                                     \
}

/////////////////////////////////////////////////////////////////////////////////////////
// class GenTLTestTools
/////////////////////////////////////////////////////////////////////////////////////////

class GenTLTestTools
{
public:
    GenTLTestTools(void);
    ~GenTLTestTools(void);
};

#endif  /* GENTLTESTTOOLS_INCLUDE___ */
