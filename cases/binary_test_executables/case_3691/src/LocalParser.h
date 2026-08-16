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


#ifndef _DEF_LOCALPARSER_H_
#define _DEF_LOCALPARSER_H_

#include "URLParser.h"
#include "Base/GCTypes.h"

namespace GENICAM_NAMESPACE
{
  namespace Registry
  {
    namespace Impl
    {
      class CLocalParser
        : public CURLParser
      {
      public:
        //! Ctor
        CLocalParser();

        //! Parses the given reference \a URL
        virtual void Parse(const std::string &URL);

        //! Gets the filename of the local URL
        std::string GetFilename() const;

        //! Gets the address where the file is located
        uint64_t GetAddress() const;

        //! Gets the length of the file
        uint64_t GetLength() const;

      private:
        //! Parses the length field
        void ParseLength(const std::string &Path);

        //! Parses the address field
        void ParseAddress(const std::string &Path, size_t To);

        //! Parses the filename field
        void ParseFilename(const std::string &Path, size_t To);

      private:
        std::string m_Filename;
        uint64_t m_Address;
        uint64_t m_Length;
      };
    }
  }
}

#endif // _DEF_LOCALPARSER_H_