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


#include "LocalParser.h"
#include "Base/GCException.h"

using namespace GenICam::Registry::Impl;
using namespace std;

// ---------------------------------------------------------------------------
CLocalParser::CLocalParser()
  : m_Address(0)
  , m_Length(0)
{
}

// ---------------------------------------------------------------------------
void CLocalParser::Parse(const string &URL)
{
  CURLParser::Parse(URL);
  if(_stricmp(GetScheme().c_str(), "Local") != 0)
    throw INVALID_ARGUMENT_EXCEPTION("Given URL type is not of local scheme: %s", URL.c_str());
  ParseLength(GetPath());
}

// ---------------------------------------------------------------------------
string CLocalParser::GetFilename() const { return m_Filename; }

// ---------------------------------------------------------------------------
uint64_t CLocalParser::GetAddress() const { return m_Address; }

// ---------------------------------------------------------------------------
uint64_t CLocalParser::GetLength() const { return m_Length; }

// ---------------------------------------------------------------------------
void CLocalParser::ParseLength(const string &Path)
{
  size_t uiLastSemicolon = Path.find_last_of(';');
  if(uiLastSemicolon == string::npos)
    throw INVALID_ARGUMENT_EXCEPTION("Given local scheme has missing address and length: %s", Path.c_str());
  char *szErr = NULL;
  m_Length = _strtoui64(Path.c_str() + uiLastSemicolon + 1, &szErr, 16);
  if(szErr != NULL && *szErr != '\0')
    throw RUNTIME_EXCEPTION("Length field of local URL could not be read: %s", Path.c_str());
  ParseAddress(Path, uiLastSemicolon - 1);
}

// ---------------------------------------------------------------------------
void CLocalParser::ParseAddress(const string &Path, size_t To)
{
  size_t uiLastSemicolon = Path.find_last_of(';', To);
  if(uiLastSemicolon == string::npos)
    throw INVALID_ARGUMENT_EXCEPTION("Given local scheme has a missing length: %s", Path.c_str());
  char *szErr = NULL;
  m_Address = _strtoui64(Path.substr(uiLastSemicolon + 1, To - uiLastSemicolon).c_str(), &szErr, 16);
  if(szErr != NULL && *szErr != '\0')
    throw RUNTIME_EXCEPTION("Address field of local URL could not be read: %s", Path.c_str());
  ParseFilename(Path, uiLastSemicolon - 1);
}

// ---------------------------------------------------------------------------
void CLocalParser::ParseFilename(const string &Path, size_t To)
{
  m_Filename = Path.substr(0, To + 1);
}
