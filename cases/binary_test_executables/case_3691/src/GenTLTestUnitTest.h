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

#ifndef GENTLTESTUNITTEST_INCLUDE______________
#define GENTLTESTUNITTEST_INCLUDE______________

#ifdef _USING_BOOST

#include "boost/test/unit_test.hpp"
#include "boost/test/parameterized_test.hpp"
#include "boost/test/results_collector.hpp"
#include "boost/thread.hpp"

#define GENTLUNITTEST_ADD_TEST_UNIT(currentTestCase, testMethodName);                                                           \
    g_poGenTLTestSuite->vAdd(currentTestCase, GENTLUNITTEST_MAKETESTCASE(testMethodName));

#define GENTLUNITTEST_MESSAGE( message )                                                                                        \
{                                                                                                                               \
    BOOST_MESSAGE ( message );                                                                                                  \
}

#define GENTLUNITTEST_CHECK( condition )                                                                                        \
{                                                                                                                               \
    BOOST_CHECK( condition );                                                                                                   \
}

#define GENTLUNITTEST_CHECK_MESSAGE( condition, message )                                                                       \
{                                                                                                                               \
    BOOST_CHECK_MESSAGE( condition, message );                                                                                  \
}

#define GENTLUNITTEST_REQUIRE( condition )                                                                                      \
{                                                                                                                               \
    BOOST_REQUIRE( condition );                                                                                                 \
}

#define GENTLUNITTEST_REQUIRE_MESSAGE( condition, message )                                                                     \
{                                                                                                                               \
    BOOST_REQUIRE_MESSAGE( condition, message );                                                                                \
}

#define GENTLUNITTEST_SKIP( message )                                                                                           \
{                                                                                                                               \
    BOOST_MESSAGE( message );                                                                                                   \
    BOOST_CHECK( true );                                                                                                        \
}

#define GENTLUNITTEST_GET_ASSERT_FAILED(id) boost::unit_test::results_collector_t::instance().results(id).p_assertions_failed;     
#define GENTLUNITTEST_GET_ASSERT_PASSED(id) boost::unit_test::results_collector_t::instance().results(id).p_assertions_passed;     
#define GENTLUNITTEST_GET_ABORTED(id) boost::unit_test::results_collector_t::instance().results(id).p_aborted;                     
#define GENTLUNITTEST_GET_SKIPPED(id) boost::unit_test::results_collector_t::instance().results(id).p_skipped;                     

#define GENTLUNITTEST_MAKETESTCASE( method ) BOOST_TEST_CASE(&method)

#define GENTLUNITTEST_GETTHREADID boost::this_thread::get_id()
#define GENTLUNITTEST_SLEEP( msec ) boost::this_thread::sleep(boost::posix_time::milliseconds(msec))

#else /* _USING_BOOST */

#include "GenTLTestSuite.h"
#include "Base/GCException.h"

extern ocGenTLTestSuite* g_poGenTLTestSuite;

#define GENTLUNITTEST_ADD_TEST_UNIT(currentTestCase, testMethodName);                                                           \
    g_poGenTLTestSuite->vAdd(currentTestCase, GENTLUNITTEST_MAKETESTCASE(currentTestCase, testMethodName));

#define GENTLUNITTEST_MESSAGE( message )                                                                                        \
{                                                                                                                               \
    g_poGenTLTestSuite->vWriteToFile(message, UTL_ET_MESSAGE);                                                                  \
}

#define GENTLUNITTEST_CHECK( condition )                                                                                        \
{                                                                                                                               \
    if(!condition)                                                                                                              \
    {                                                                                                                           \
        std::stringstream csMessage;                                                                                            \
        csMessage << "critical " << #condition << " failed";                                                                    \
        g_poGenTLTestSuite->vIncTestAssertionFailed(__FILE__, __LINE__, __FUNCTION__);                                          \
        g_poGenTLTestSuite->vWriteToFile(csMessage.str().c_str(), UTL_ET_ERROR, __FILE__, __LINE__);                            \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        g_poGenTLTestSuite->vIncTestAssertionPassed();                                                                          \
    }                                                                                                                           \
}

#define GENTLUNITTEST_CHECK_MESSAGE( condition, message )                                                                       \
{                                                                                                                               \
    if (!condition)                                                                                                             \
    {                                                                                                                           \
        g_poGenTLTestSuite->vIncTestAssertionFailed(__FILE__, __LINE__, __FUNCTION__);                                          \
        g_poGenTLTestSuite->vWriteToFile(message, UTL_ET_ERROR, __FILE__, __LINE__);                                            \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        g_poGenTLTestSuite->vIncTestAssertionPassed();                                                                          \
    }                                                                                                                           \
}

#define GENTLUNITTEST_REQUIRE( condition )                                                                                      \
{                                                                                                                               \
    if(!condition)                                                                                                              \
    {                                                                                                                           \
        std::stringstream csMessage;                                                                                            \
        csMessage << "critical check " << #condition << " failed";                                                              \
        g_poGenTLTestSuite->vSetTestAborted(__FILE__, __LINE__, __FUNCTION__);                                                  \
        g_poGenTLTestSuite->vWriteToFile(csMessage.str().c_str(), UTL_ET_FATAL_ERROR, __FILE__, __LINE__);                      \
        throw GENERIC_EXCEPTION("GENTLUNITTEST_REQUIRE test failed");                                                           \
    }                                                                                                                           \
}

#define GENTLUNITTEST_REQUIRE_MESSAGE( condition, message )                                                                     \
{                                                                                                                               \
    if (!condition)                                                                                                             \
    {                                                                                                                           \
        g_poGenTLTestSuite->vSetTestAborted(__FILE__, __LINE__, __FUNCTION__);                                                  \
        g_poGenTLTestSuite->vWriteToFile(message, UTL_ET_FATAL_ERROR, __FILE__, __LINE__);                                      \
        throw GENERIC_EXCEPTION("GENTLUNITTEST_REQUIRE failed");                                                                \
    }                                                                                                                           \
}

#define GENTLUNITTEST_SKIP( message )                                                                                           \
{                                                                                                                               \
    g_poGenTLTestSuite->vSetTestSkipped();                                                                                      \
    g_poGenTLTestSuite->vWriteToFile(message, UTL_ET_MESSAGE);                                                                  \
}

#define GENTLUNITTEST_GET_ASSERT_FAILED(id) g_poGenTLTestSuite->nGetAssertionsFailed(id);
#define GENTLUNITTEST_GET_ASSERT_PASSED(id) g_poGenTLTestSuite->nGetAssertionsPassed(id);
#define GENTLUNITTEST_GET_ABORTED(id) g_poGenTLTestSuite->bGetAborted(id);
#define GENTLUNITTEST_GET_SKIPPED(id) g_poGenTLTestSuite->bGetSkipped(id);

#define GENTLUNITTEST_MAKETESTCASE( currentTestCase, method ) new ocGenTLTestCaseContainer(currentTestCase, &method, #method)

#define GENTLUNITTEST_GETTHREADID GetCurrentThreadId()

#define GENTLUNITTEST_SLEEP( msec ) usleep(msec)

#endif /* _USING_BOOST */

#endif /* GENTLTESTUNITTEST_INCLUDE______________ */
