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

#include <string>
#include <base/gctypes.h>
#include "GenTLTestCaseContainer.h"

ocGenTLTestCaseContainer::ocGenTLTestCaseContainer(uint32_t zCurrentTestCase, TestCase* pTestCase, std::string csTestCaseName)
    : m_pTestCase(pTestCase)
    , m_nTestCaseNumber(zCurrentTestCase)
    , m_csTestCaseName(csTestCaseName)
    , m_nAssertionsPassed(0)
    , m_nAssertionsFailed(0)
    , m_bSkipped(false)
    , m_bAborted(false)
{
}

ocGenTLTestCaseContainer::~ocGenTLTestCaseContainer() 
{ 
}

TestCase* ocGenTLTestCaseContainer::pGetTestCase() 
{ 
    return m_pTestCase; 
}

uint32_t ocGenTLTestCaseContainer::nGetTestCaseNumber() 
{ 
    return m_nTestCaseNumber; 
}

std::string ocGenTLTestCaseContainer::csGetTestCaseName() 
{ 
    return m_csTestCaseName; 
}

uint32_t ocGenTLTestCaseContainer::nGetAssertionsPassed() 
{ 
    return m_nAssertionsPassed; 
}

uint32_t ocGenTLTestCaseContainer::nGetAssertionsFailed() 
{ 
    return m_nAssertionsFailed; 
}

bool ocGenTLTestCaseContainer::bIsSkipped() 
{ 
    return m_bSkipped; 
}

bool ocGenTLTestCaseContainer::bIsAborted() 
{ 
    return m_bAborted; 
}

ocGenTLTestCaseContainer::vecIssueContainers ocGenTLTestCaseContainer::xGetAssertionsFailedIssues()
{
    ocGenTLTestCaseContainer::vecIssueContainers issues;

    for (size_t index=0; index<m_listAssertionsFailed.size(); index++)
    {
        issues.push_back(m_listAssertionsFailed[index]);
    }

    return issues;
}

ocGenTLTestCaseContainer::IssueContainer ocGenTLTestCaseContainer::xGetAbortedIssue()
{
    return m_Aborted;
}
    
void ocGenTLTestCaseContainer::vIncAssertionsPassed() 
{ 
    m_nAssertionsPassed++; 
}

void ocGenTLTestCaseContainer::vIncAssertionsFailed(std::string file_name, long line_number, std::string function_name) 
{ 
    IssueContainer issue;
    issue.file_name = file_name;
    issue.line = line_number;
    issue.function = function_name;
    m_listAssertionsFailed.push_back(issue);
        
    m_nAssertionsFailed++; 
}

void ocGenTLTestCaseContainer::vSetSkipped() 
{ 
    m_bSkipped = true; 
}

void ocGenTLTestCaseContainer::vSetAborted(std::string file_name, long line_number, std::string function_name) 
{ 
    IssueContainer issue;
    issue.file_name = file_name;
    issue.line = line_number;
    issue.function = function_name;
    m_Aborted = issue;
        
    m_bAborted = true; 
};

