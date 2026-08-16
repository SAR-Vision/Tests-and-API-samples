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


#ifndef _DEF_URLPARSERIMPL_H_
#define _DEF_URLPARSERIMPL_H_

#include <string>
#include "Base/GCTypes.h"

namespace GENICAM_NAMESPACE
{
  namespace Registry
  {
    namespace Impl
    {
      class CURLParser
      {
      public:
        //! Ctor
        CURLParser();

        //! Dtor
        virtual ~CURLParser();

        //! Parses the given \a URL
        virtual void Parse(const std::string &URL);

        //! Gets the scheme type (e.g. file)
        std::string GetScheme() const;

        //! Gets the authority (e.g. host address)
        std::string GetAuthority() const;

        //! Gets the path (dependent on the authority)
        std::string GetPath() const;

        //! Gets the query (resource identifier for e.g. a database)
        std::string GetQuery() const;

        //! Gets the fragment (indirect identification of a secondary resource)
        std::string GetFragment() const;

        //! Gets the extension (e.g. XML or ZIP)
        std::string GetExtension() const;

        //! Parses the location part of the URL to be ASCII compliant.
        static std::string ToAscii(const std::string &Text);

        //! Performs a normalization of the url
        static std::string Normalize(const std::string &URL);

      private:
        //! Processes the scheme part from the whole \a URL.
        void ParseScheme(const std::string &URL);

        //! Process authority part from the whole \a URL
        void ParseAuthority(const std::string &URL, size_t Offset);

        //! Process the path part from the whole \a URL
        void ParsePath(const std::string &URL, size_t Offset);

        //! Process the query part from the whole \a URL
        void ParseQuery(const std::string &URL, size_t Offset);

        //! Process the fragment part from the whole \a URL
        void ParseFragment(const std::string &URL, size_t Offset);

        //! Process the extension part from the whole \a URL
        void ParseExtension(const std::string &URL, size_t Offset);

      private:
        std::string m_Scheme;
        std::string m_Authority;
        std::string m_Path;
        std::string m_Query;
        std::string m_Fragment;
        std::string m_Extension;
      };
    }
  }
}

#endif // _DEF_URLPARSERIMPL_H_