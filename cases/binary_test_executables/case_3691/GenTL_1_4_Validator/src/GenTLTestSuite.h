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

#ifndef GENTLTEST_INCLUDE______________
#define GENTLTEST_INCLUDE______________

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "GenTL_v1_4.h"

#ifdef _USING_BOOST

#include "boost/test/unit_test.hpp"

class ocGenTLTestSuite : public boost::unit_test::test_suite
{
public:
    typedef std::vector<boost::unit_test::test_unit_id> tTestUnitIDList;

    ocGenTLTestSuite(boost::unit_test::const_string ts_name);
    
    tTestUnitIDList tGetTestUnitIDList();

    // is called for log output
    uint32_t xGetTestUnitIndex(boost::unit_test::test_unit_id xID);

    boost::unit_test::test_unit_id xGetTestUnitID(std::string tu_name);

    void vAdd(uint32_t uiCurrentTestCaseNumber, boost::unit_test::test_unit* tu);

private:
    std::vector<uint32_t> m_vecLocalTestNumber;
};

#else /* _USING_BOOST */

#include "GenTLTest_Win.h"
#include "GenTLTestCaseContainer.h"

#define UTL_ET_INFO           0
#define UTL_ET_MESSAGE        1
#define UTL_ET_WARNING        2
#define UTL_ET_ERROR          3
#define UTL_ET_FATAL_ERROR    4
typedef unsigned long log_entry_types;

class ocGenTLTestSuite 
{
public:
    typedef std::vector<uint32_t> tTestUnitIDList;
    typedef std::vector<ocGenTLTestCaseContainer> tVecTestCaseContainer;
    
    ocGenTLTestSuite(std::string ts_name);
    virtual ~ocGenTLTestSuite();
    
    tTestUnitIDList tGetTestUnitIDList();

    // is called for log output
    uint32_t xGetTestUnitIndex(uint32_t xID);

    uint32_t xGetTestUnitID(std::string tu_name);

    void vAdd(uint32_t uiCurrentTestCaseNumber, ocGenTLTestCaseContainer* tu);

    void vRun();

    uint32_t nGetTestCount() { return (uint32_t)m_vecLocalTestCases.size(); };
    void vIncTestAssertionFailed(std::string file_name, long line_number, std::string function_name);
    void vIncTestAssertionPassed();
    void vSetTestAborted(std::string file_name, long line_number, std::string function_name);
    void vSetTestSkipped();

    int32_t nGetAssertionsFailed(uint32_t xID);
    int32_t nGetAssertionsPassed(uint32_t xID);
    bool bGetAborted(uint32_t xID);
    bool bGetSkipped(uint32_t xID);
    ocGenTLTestCaseContainer::vecIssueContainers xGetAssertionsFailedIssues(uint32_t xID);
    ocGenTLTestCaseContainer::IssueContainer xGetAbortedIssue(uint32_t xID);
    
    void vSetLogFilename(std::string sLogFilename);
    void vWriteToFile(std::string message, log_entry_types led, std::string file_name="", uint32_t line_num=0);

private:
    void log_entry_start( std::ostream& output, std::string file_name, uint32_t line_num, log_entry_types let );
    void log_entry_value( std::ostream& output, std::string value );
    void log_entry_finish( std::ostream& output);

    ocGenTLTestCaseContainer *poGetTestCase(uint32_t index);
    tVecTestCaseContainer tGetTestCaseContainerCopy();
    
private:
    tVecTestCaseContainer m_vecLocalTestCases;
    std::string m_csTestSuiteName;

    std::string m_Filename;
    FILE *m_poFile;

    log_entry_types m_let;
    std::stringstream m_sOutput;
    std::stringstream m_sMessage;
    uint32_t m_nCurrentTestIndex;
};

#endif /* _USING_BOOST */

#endif /* GENTLTEST_INCLUDE______________ */
