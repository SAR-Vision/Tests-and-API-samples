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

#include "GenTLTestSuite.h"

#ifdef _USING_BOOST

ocGenTLTestSuite::ocGenTLTestSuite(boost::unit_test::const_string ts_name)
: boost::unit_test::test_suite(ts_name)
{
}
    
ocGenTLTestSuite::tTestUnitIDList ocGenTLTestSuite::tGetTestUnitIDList()
{
    tTestUnitIDList oResult;

    for (std::size_t xIndex=0; xIndex<size(); xIndex++)
    {
        oResult.push_back(m_members[xIndex]);       
    }

    return oResult;
};

// is called for log output
uint32_t ocGenTLTestSuite::xGetTestUnitIndex(boost::unit_test::test_unit_id xID)
{
    uint32_t xResultIndex=0;

    for (std::size_t xIndex=0; xIndex<size(); xIndex++)
    {
        if (m_members[xIndex] == xID) 
        {
            // m_members is mapped to m_vecLocalTestNumber
            xResultIndex = m_vecLocalTestNumber[xIndex];
            break;
        }
    }

    return xResultIndex;
}

boost::unit_test::test_unit_id ocGenTLTestSuite::xGetTestUnitID(std::string tu_name)
{
    boost::unit_test::test_unit_id xResult;
        
    xResult = get(tu_name.c_str());     
        
    return xResult;
};

void ocGenTLTestSuite::vAdd(uint32_t uiCurrentTestCaseNumber, boost::unit_test::test_unit* tu)
{
    add(tu);
    m_vecLocalTestNumber.push_back(uiCurrentTestCaseNumber);
}

#else /* _USING_BOOST */

#include <ostream>
#include "GenApi/GenApi.h"
#include "Base/GCException.h"
#include "GenTLValidDll.h"

extern uint8_t g_bStopValidationFlag;
extern uint8_t g_bCreateTestEnumerationFlag;

/****************************************************************/
/* Constructor/Destructor                                       */
/****************************************************************/
ocGenTLTestSuite::ocGenTLTestSuite(std::string ts_name)
    : m_poFile(NULL)
    , m_nCurrentTestIndex(0)
{
    m_csTestSuiteName = ts_name;
}

ocGenTLTestSuite::~ocGenTLTestSuite()
{
    
}

/****************************************************************/
/* Test session analyse tools                                   */
/****************************************************************/
ocGenTLTestSuite::tTestUnitIDList ocGenTLTestSuite::tGetTestUnitIDList()
{
    tTestUnitIDList oResult;

    for (std::size_t xIndex=0; xIndex<m_vecLocalTestCases.size(); xIndex++)
    {
        oResult.push_back(m_vecLocalTestCases[xIndex].nGetTestCaseNumber());       
    }

    return oResult;
};

// is called for log output
uint32_t ocGenTLTestSuite::xGetTestUnitIndex(uint32_t xID)
{
    return xID;
}

uint32_t ocGenTLTestSuite::xGetTestUnitID(std::string tu_name)
{
    uint32_t xResultIndex=0;
        
    for (std::size_t xIndex=0; xIndex<m_vecLocalTestCases.size(); xIndex++)
    {
        if (m_vecLocalTestCases[xIndex].csGetTestCaseName() == tu_name) 
        {
            // m_members is mapped to m_vecLocalTestNumber
            xResultIndex = m_vecLocalTestCases[xIndex].nGetTestCaseNumber();
            break;
        }
    }
    
    return xResultIndex;
};

ocGenTLTestSuite::tVecTestCaseContainer ocGenTLTestSuite::tGetTestCaseContainerCopy()
{
    tVecTestCaseContainer oResult;

    for (std::size_t xIndex=0; xIndex<m_vecLocalTestCases.size(); xIndex++)
    {
        oResult.push_back(m_vecLocalTestCases[xIndex]);       
    }

    return oResult;
};

int32_t ocGenTLTestSuite::nGetAssertionsFailed(uint32_t xID)
{
    int32_t nResult=-1;
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(xID);

    if (NULL != poTestCase)
        nResult = poTestCase->nGetAssertionsFailed();

    return nResult;
}

int32_t ocGenTLTestSuite::nGetAssertionsPassed(uint32_t xID)
{
    int32_t nResult=-1;
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(xID);

    if (NULL != poTestCase)
        nResult = poTestCase->nGetAssertionsPassed();

    return nResult;
}

bool ocGenTLTestSuite::bGetAborted(uint32_t xID)
{
    bool bResult=false;
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(xID);

    if (NULL != poTestCase)
        bResult = poTestCase->bIsAborted();

    return bResult;
}

bool ocGenTLTestSuite::bGetSkipped(uint32_t xID)
{
    bool bResult=false;
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(xID);

    if (NULL != poTestCase)
        bResult = poTestCase->bIsSkipped();

    return bResult;
}

ocGenTLTestCaseContainer::vecIssueContainers ocGenTLTestSuite::xGetAssertionsFailedIssues(uint32_t xID)
{
    ocGenTLTestCaseContainer::vecIssueContainers issues;
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(xID);

    if (NULL != poTestCase)
        issues = poTestCase->xGetAssertionsFailedIssues();

    return issues;
}

ocGenTLTestCaseContainer::IssueContainer ocGenTLTestSuite::xGetAbortedIssue(uint32_t xID)
{
    ocGenTLTestCaseContainer::IssueContainer issue;
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(xID);

    if (NULL != poTestCase)
        issue = poTestCase->xGetAbortedIssue();

    return issue;
}

/****************************************************************/
/* Test session Test Case filler                                */
/****************************************************************/
void ocGenTLTestSuite::vAdd(uint32_t uiCurrentTestCaseNumber, ocGenTLTestCaseContainer* tu)
{
    m_vecLocalTestCases.push_back(*tu);
}

/****************************************************************/
/* Test session Run                                             */
/****************************************************************/
void ocGenTLTestSuite::vRun()
{
    tVecTestCaseContainer vecLocalTestCases=tGetTestCaseContainerCopy();

    for (size_t index=0; index<vecLocalTestCases.size(); index++)
    {
        ocGenTLTestCaseContainer *poTest=&(vecLocalTestCases[index]);
        if (NULL != poTest)
        {
            TestCase *poTestCase=poTest->pGetTestCase();

            m_nCurrentTestIndex = poTest->nGetTestCaseNumber();
            
            try
            {
                poTestCase();
            }
            catch(GenICam::GenericException)
            {
            }

            usleep(0);
        }
    }
}

/****************************************************************/
/* Test tools                                                   */
/****************************************************************/
void ocGenTLTestSuite::vIncTestAssertionFailed(std::string file_name, long line_number, std::string function_name)
{
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(m_nCurrentTestIndex);

    if (NULL != poTestCase)
        poTestCase->vIncAssertionsFailed(file_name, line_number, function_name);
}

void ocGenTLTestSuite::vIncTestAssertionPassed()
{
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(m_nCurrentTestIndex);

    if (NULL != poTestCase)
        poTestCase->vIncAssertionsPassed();
}

void ocGenTLTestSuite::vSetTestAborted(std::string file_name, long line_number, std::string function_name)
{
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(m_nCurrentTestIndex);

    if (NULL != poTestCase)
        poTestCase->vSetAborted(file_name, line_number, function_name);
}

void ocGenTLTestSuite::vSetTestSkipped()
{
    ocGenTLTestCaseContainer *poTestCase=poGetTestCase(m_nCurrentTestIndex);

    if (NULL != poTestCase)
        poTestCase->vSetSkipped();
}

/****************************************************************/
/* Test log file                                                */
/****************************************************************/
void ocGenTLTestSuite::vSetLogFilename(std::string sLogFilename)
{ 
    m_Filename = sLogFilename; 
}

void ocGenTLTestSuite::vWriteToFile(std::string message, log_entry_types let, std::string file_name, uint32_t line_num)
{
    std::stringstream output;

    if (fopen_s(&m_poFile, m_Filename.c_str(), "at") != 0)
        throw ACCESS_EXCEPTION("Given file path and name could not be opened, scheme: %s", m_Filename.c_str());

    fseek(m_poFile, 0L, SEEK_END);

    log_entry_start(output, file_name, line_num, let);
    log_entry_value(output, message);
    log_entry_finish(output);

    fwrite(output.str().c_str(), sizeof(char), output.str().size(), m_poFile);
    fflush(m_poFile);

    fclose(m_poFile);
    m_poFile = NULL;
}

/****************************************************************/
/* Helper                                                       */
/****************************************************************/

void ocGenTLTestSuite::log_entry_start( std::ostream& output, std::string file_name, uint32_t line_num, log_entry_types let )
{
    m_sMessage.str("");
    m_sOutput.str("");
    m_sOutput << "           details: ";
    m_let = let;
    if (let >= UTL_ET_ERROR)
    {
        output << "*** FAILURE ***" << std::endl
                << "           source : '" << file_name << "'" << std::endl
                << "           line   : " << line_num << std::endl;
    }
    else if (let == UTL_ET_MESSAGE)
    {
        output << "";
    }
    else 
    {
        output << file_name << "(" << line_num << ")";
    }
}

void ocGenTLTestSuite::log_entry_value( std::ostream& output, std::string value )
{
    if (m_let >= UTL_ET_ERROR)
    {
        m_sOutput << value;
        m_sMessage << value;
    }
    else
    {
        output << value;
    }
}

void ocGenTLTestSuite::log_entry_finish( std::ostream& output)
{
    if (m_let == UTL_ET_ERROR)
    {
        if (!g_bStopValidationFlag && !g_bCreateTestEnumerationFlag)
        {
            vSendCallbackMessage(DLL_CALLBACK_ERROR, m_sMessage.str());
        }
        output << m_sOutput.str().c_str() << std::endl;
        output << "-";
        output << std::endl;
    }
    if (m_let == UTL_ET_FATAL_ERROR)
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

ocGenTLTestCaseContainer *ocGenTLTestSuite::poGetTestCase(uint32_t number)
{
    ocGenTLTestCaseContainer *poTestCase=NULL;

    for (size_t index=0; index<m_vecLocalTestCases.size(); index++)
    {
        if (m_vecLocalTestCases[index].nGetTestCaseNumber() == number)
        {
            poTestCase = &(m_vecLocalTestCases[index]);
            break;
        }
    }

    return poTestCase;
}

#endif /* _USING_BOOST */
