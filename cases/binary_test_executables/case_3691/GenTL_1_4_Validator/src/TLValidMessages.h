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

#ifndef TLVALIDMESSAGES_INCLUDE____
#define TLVALIDMESSAGES_INCLUDE____

// callback message tags
#define DLL_CALLBACK_MESSAGE                0
#define DLL_CALLBACK_FILENAME               1
#define DLL_CALLBACK_TESTNUMBER             2
#define DLL_CALLBACK_TOTALTESTAMOUNT        3
#define DLL_CALLBACK_TESTSUCCEEDED          4
#define DLL_CALLBACK_TESTFAILED             5
#define DLL_CALLBACK_TESTSTARTED            6
#define DLL_CALLBACK_TESTSTOPPED            7
#define DLL_CALLBACK_TESTRESULT             8
#define DLL_CALLBACK_TESTABORTED            9
#define DLL_CALLBACK_ERROR                  9999

#endif /* TLVALIDMESSAGES_INCLUDE____ */
