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


#include "URLParser.h"
#include <cassert>
#include <sstream>
#include <locale>
#include <algorithm>
#include <exception>
#include <vector>
#include "Base/GCException.h"

using namespace GenICam;
using namespace GenICam::Registry::Impl;
using namespace std;

#pragma warning ( push )
#if !defined _MSC_VER || _MSC_VER < 1400
# pragma warning ( disable : 4068 ) // unknown region pragma
#endif

#pragma region Ctor / Dtor

CURLParser::CURLParser() {}

CURLParser::~CURLParser() {}

#pragma endregion

#pragma region Interface Functions

void CURLParser::Parse(const string &URL) { ParseScheme(Normalize(URL)); }

string CURLParser::GetScheme() const { return m_Scheme; }

string CURLParser::GetAuthority() const { return m_Authority; }

string CURLParser::GetPath() const { return m_Path; }

string CURLParser::GetQuery() const { return m_Query; }

string CURLParser::GetFragment() const { return m_Fragment; }

string CURLParser::GetExtension() const { return m_Extension; }


string CURLParser::ToAscii(const string &Text)
{
    stringstream result;
    result.imbue(locale::classic());
    vector<char> cEscape(4, '\0');
    for (string::const_iterator c = Text.begin(); c != Text.end(); ++c)
    {
        if (cEscape[0] == '%')
        { // escape mode
            size_t uiPos = cEscape[1] == '\0' ? 1 : 2;
            if (!::isxdigit((unsigned char)*c))
                throw INVALID_ARGUMENT_EXCEPTION("Invalid percent-encoding '%s' in '%s'", &cEscape[0], Text.c_str());
            cEscape[uiPos] = *c;
            if (uiPos == 2)
            { // percent escape finished
                result << (char)strtoul(&cEscape[1], NULL, 16);
                memset(&cEscape[0], '\0', 3);
            }
        }
        else
        { // normal mode
            if (*c == '%')
            { // start escape
                cEscape[0] = '%';
            } 
            else if (*c =='|')
            {
                result << ':';
            } 
            else
            {
                result << *c;
            }
        }
    }
    return result.str();
}

#pragma endregion

#pragma region Internal Functions

// removes all white space characters from the given url and turn all '\' into '/'
string CURLParser::Normalize(const string &URL)
{
  stringstream result;
  result.imbue(locale::classic());
  for(string::const_iterator c = URL.begin(); c != URL.end(); ++c)
  {
    if(!::isspace((unsigned char)*c))
    {
      if(*c == '\\') result << '/'; else result << *c;
    }
  }
  return result.str();
}

inline void CURLParser::ParseScheme(const string &URL)
{
  size_t uiSchemeEnd = URL.find(':');
  if(uiSchemeEnd == string::npos)
    throw INVALID_ARGUMENT_EXCEPTION("No scheme found in url: %s", URL.c_str());
  m_Scheme.assign(URL.begin(), URL.begin() + uiSchemeEnd);
  ParseAuthority(URL, uiSchemeEnd + 1);
}

inline void CURLParser::ParseAuthority(const string &URL, size_t Offset)
{
  if(Offset >= URL.length()) return; // sanity check

  if(URL.substr(Offset, 2) != "//")
  {
    ParsePath(URL, Offset);
    return;
  }
  Offset += 2;
  size_t uiSlash  = URL.find('/', Offset);
  size_t uiQuot   = URL.find('?', Offset);
  size_t uiNumber = URL.find('#', Offset);

  if(uiSlash < uiQuot && uiSlash < uiNumber)
  { // '/' before '?' and '#'
    m_Authority.assign(URL.begin() + Offset, URL.begin() + uiSlash);
    ParsePath(URL, uiSlash + 1);
  }
  else if(uiQuot < uiSlash && uiQuot < uiNumber)
  { // '?' before '/' and '#'
    m_Authority.assign(URL.begin() + Offset, URL.begin() + uiQuot);
    ParseQuery(URL, uiQuot + 1);
  }
  else if(uiNumber < uiSlash && uiNumber < uiQuot)
  { // '#' before '/' and '?'
    m_Authority.assign(URL.begin() + Offset, URL.begin() + uiNumber);
    ParseFragment(URL, uiNumber + 1);
  }
  else
  { // nothing found
    m_Authority.assign(URL.begin() + Offset, URL.end());
  }
}

inline void CURLParser::ParsePath(const string &URL, size_t Offset)
{
    if(Offset >= URL.length()) return; // sanity check

    // skip trailing '/'s
    while(URL[Offset] == '/')
        ++Offset;

    size_t uiQuot   = URL.find('?', Offset);
    size_t uiNumber = URL.find('#', Offset);

    if(uiQuot < uiNumber)
    { // '?' before '#'
        m_Path.assign(URL.begin() + Offset, URL.begin() + uiQuot);
        ParseQuery(URL, uiQuot + 1);
    }
    else if(uiNumber < uiQuot)
    { // '#' before '?'
        m_Path.assign(URL.begin() + Offset, URL.begin() + uiNumber);
        ParseFragment(URL, uiNumber + 1);
    }
    else
    { // nothing found
        m_Path.assign(URL.begin() + Offset, URL.end());
    }
    
    ParseExtension(URL, Offset);
}

inline void CURLParser::ParseQuery(const string &URL, size_t Offset)
{
  if(Offset >= URL.length()) return; // sanity check

  size_t uiEnd = URL.find('#', Offset);
  if(uiEnd != string::npos)
  { // fragment found
    m_Query.assign(URL.begin() + Offset, URL.begin() + uiEnd);
    ParseFragment(URL, uiEnd + 1);
  }
  else
  {
    m_Query.assign(URL.begin() + Offset, URL.end());
  }
}

inline void CURLParser::ParseFragment(const string &URL, size_t Offset)
{
  if(Offset >= URL.length()) return; // sanity check

  m_Fragment.assign(URL.begin() + Offset, URL.end());
}

inline void CURLParser::ParseExtension(const string &URL, size_t Offset)
{
    if(Offset >= URL.length()) return; // sanity check

    size_t uiEnd = URL.find(';', Offset);
    if(uiEnd == string::npos)
		uiEnd = URL.length();

    if(uiEnd != string::npos)
	{ // end found
        size_t uiStart = URL.rfind('.', uiEnd);
        if(uiStart != string::npos && (uiStart+1) < uiEnd)
        {
            m_Extension.assign(URL.begin() + uiStart + 1, URL.begin() + uiEnd);
        }
	}
}

#pragma endregion

#pragma warning ( pop )

