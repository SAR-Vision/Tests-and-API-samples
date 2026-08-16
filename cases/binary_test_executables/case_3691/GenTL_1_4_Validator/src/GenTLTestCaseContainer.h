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

#ifndef GENTLTESTCASECONTAINER_INCLUDE______________
#define GENTLTESTCASECONTAINER_INCLUDE______________

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "GenTL_v1_4.h"

typedef void (TestCase)( void );

class ocGenTLTestCaseContainer 
{
public:
    typedef struct IssueContainer
    {
        std::string file_name;
        long line;
        std::string function;
    };
    typedef std::vector<IssueContainer> vecIssueContainers;
    
    ocGenTLTestCaseContainer(uint32_t zCurrentTestCase, TestCase* pTestCase, std::string csTestCaseName);
    virtual ~ocGenTLTestCaseContainer();

    TestCase* pGetTestCase();
    uint32_t nGetTestCaseNumber();
    std::string csGetTestCaseName();

    uint32_t nGetAssertionsPassed();
    uint32_t nGetAssertionsFailed();
    bool bIsSkipped();
    bool bIsAborted();
    vecIssueContainers xGetAssertionsFailedIssues();
    IssueContainer xGetAbortedIssue();
    
    void vIncAssertionsPassed();
    void vIncAssertionsFailed(std::string file_name, long line_number, std::string function_name);
    void vSetSkipped();
    void vSetAborted(std::string file_name, long line_number, std::string function_name);

private:
    vecIssueContainers m_listAssertionsFailed;
    IssueContainer m_Aborted;
    TestCase* m_pTestCase;
    uint32_t m_nTestCaseNumber;
    std::string m_csTestCaseName;
    uint32_t m_nAssertionsPassed;
    uint32_t m_nAssertionsFailed;
    bool m_bSkipped;
    bool m_bAborted;
};

#endif /* GENTLTESTCASECONTAINER_INCLUDE______________ */
