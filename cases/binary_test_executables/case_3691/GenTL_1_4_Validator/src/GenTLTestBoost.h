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

#ifndef GENTLTESTBOOST_INCLUDE___
#define GENTLTESTBOOST_INCLUDE___

#include <string>
#include "GenTL_v1_4.h"
#include <basetsd.h>

#include "GenTLTestSuite.h"

#include "boost/test/unit_test.hpp"
#include "boost/test/output_test_stream.hpp"
#include "boost/test/unit_test_log.hpp"
#include "boost/test/unit_test_suite.hpp"
#include "boost/test/framework.hpp"
#include "boost/test/detail/unit_test_parameters.hpp"
#include "boost/test/output/compiler_log_formatter.hpp"
#include "boost/test/output/plain_report_formatter.hpp"
#include "boost/test/results_reporter.hpp"
#include "boost/test/results_collector.hpp"

#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>


#include "boost/thread.hpp"

class ocGenTLTestSuite;

/////////////////////////////////////////////////////////////////////////////////////////
// tests forward declaration section
/////////////////////////////////////////////////////////////////////////////////////////

void vBoostTestSummary();
boost::unit_test::test_suite* xDoTest(std::vector<std::string> &vecGenTLList);
boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[]);
int nSwitchTestTimeout(uint8_t zFlag);
int nCreateTestEnumeration();
int nStopTestRunMain();
int nStartTestRunMain(int argc, char* argv[]);

#endif  /* GENTLTESTBOOST_INCLUDE___ */
