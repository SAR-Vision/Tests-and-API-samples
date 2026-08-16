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

#include <stdio.h>
#include <vector>
#include "FileParser.h"
#include "Base/GCException.h"

using namespace GenICam::Registry::Impl;
using namespace std;

// ---------------------------------------------------------------------------
CFileParser::CFileParser()
: m_Length(0)
, poFile(NULL)
{
}

// ---------------------------------------------------------------------------
void CFileParser::Parse(const string &URL)
{
    CURLParser::Parse(URL);
    if (_stricmp(GetScheme().c_str(), "File") != 0)
        throw INVALID_ARGUMENT_EXCEPTION("Given URL type is not of file scheme: %s", URL.c_str());
    ParseLength(GetPath());
}

// ---------------------------------------------------------------------------
string CFileParser::GetFilename() const 
{ 
    return m_Filename; 
}

// ---------------------------------------------------------------------------
uint64_t CFileParser::GetLength() const 
{ 
    return m_Length; 
}

// ---------------------------------------------------------------------------
std::string CFileParser::GetXMLString() const
{
    return m_sXMLString;
}

// ---------------------------------------------------------------------------
void CFileParser::ParseLength(const string &sPath)
{
    int nReadCount=0;
    std::string sTemp=ToAscii(sPath);

    if (sTemp.find(".xml") != (sTemp.size()-4) &&
        sTemp.find(".zip") != (sTemp.size()-4))
    {   // path is no expected filename
        // try to get file name
        size_t uiStart   = sTemp.find(':', 0);
        size_t uiEnd   = sTemp.find(';', 0);
        if (uiStart != string::npos && uiEnd != string::npos && (uiStart+1) < uiEnd)
        {
            sTemp.assign(sTemp.begin() + uiStart + 1, sTemp.begin() + uiEnd);
        }
        else if (uiStart == string::npos && uiEnd != string::npos)
        {
            sTemp.assign(sTemp.begin(), sTemp.begin() + uiEnd);
        }
        else
        { // nothing found
            throw ACCESS_EXCEPTION("Given file path contains no filename, path: %s", sPath.c_str());
        }
    }

    if (fopen_s(&poFile, sTemp.c_str(), "rb") != 0)
        throw ACCESS_EXCEPTION("Given file path and name could not be opened, scheme: %s", sTemp.c_str());

    m_Filename = sTemp;

    fseek(poFile, 0L, SEEK_END);
    m_Length = ftell(poFile);
    fseek(poFile, 0L, SEEK_SET);

    if (m_Length > 0)
    {
        std::vector<char>sBuffer((size_t)m_Length+1);
        memset(&sBuffer[0], 0, (size_t)(m_Length+1));
        fread(&sBuffer[0], sizeof(char), (size_t)m_Length, poFile);
        m_sXMLString.append(&sBuffer[0], (size_t)m_Length);
    }
    else
    {
        m_sXMLString = "";
    }

    fclose(poFile);
    poFile = NULL;
}

// ---------------------------------------------------------------------------
void CFileParser::ParseFilename(const string &Path, size_t To)
{
  m_Filename = Path.substr(0, To + 1);
}
