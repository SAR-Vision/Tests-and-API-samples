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

#ifndef SFNCPORT_H
#define SFNCPORT_H

#include "GenTL_v1_4.h"

#include "GenApi/GenApi.h"

#include "Modules.h"
#include "GenTlTestTools.h"

using namespace std;

class SFNCPort : public GenApi::IPort
{
public:
    SFNCPort()
    : m_hPort (GENTL_INVALID_HANDLE)
    , m_LastReadAddress(0)
    , m_LastWriteAddress(0)
    , m_LastReadSize(0)
    , m_LastWriteSize(0)
	, m_LastResult(GenICam::Client::GC_ERR_SUCCESS)
    {
    }

    SFNCPort( ModPORT &PortFuncs, GenICam::Client::PORT_HANDLE hPort)
    : m_PortFuncs ( PortFuncs )
    , m_hPort ( hPort )
    , m_LastReadAddress(0)
    , m_LastWriteAddress(0)
    , m_LastReadSize(0)
    , m_LastWriteSize(0)
    , m_LastResult(GenICam::Client::GC_ERR_SUCCESS)
    {

    }

    void vSetSFNCPort(ModPORT &PortFuncs, GenICam::Client::PORT_HANDLE hPort)
    {
        m_PortFuncs = PortFuncs;
        m_hPort = hPort;
    }

	GenICam::Client::GC_ERROR eGetLastResult()
	{
		return m_LastResult;
	}

    //! Reads a chunk of bytes from the port
    virtual void Read(void *pBuffer, int64_t Address, int64_t Length)
    {
        size_t iLen = (size_t)Length;
        
        m_LastReadAddress = 0;
        m_LastReadSize = 0;

        m_LastResult = m_PortFuncs.eGCReadPort(m_hPort, Address, pBuffer, &iLen);
        if (m_LastResult >= GenICam::Client::GC_ERR_SUCCESS)
        {
            m_LastReadAddress = Address;
            m_LastReadSize = (size_t)Length;
        }
        Length = iLen;
        GENTLTEST_CHECK_MESSAGE("Failure GCReadPort (port=0x" << std::hex << m_hPort << 
                                             ", addr=0x" << Address << 
                                             ", buf=0x" << pBuffer << std::dec <<
                                             ", len=" << iLen <<
                                             ") reading register. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str(), 
            m_LastResult >= GenICam::Client::GC_ERR_SUCCESS);
    }

    //! Writes a chunk of bytes to the port
    virtual void Write(const void *pBuffer, int64_t Address, int64_t Length)
    {
        size_t iLen = (size_t)Length;

        m_LastWriteAddress = 0;
        m_LastWriteSize = 0;

        m_LastResult = m_PortFuncs.eGCWritePort(m_hPort, Address, pBuffer, &iLen);
        if (m_LastResult >= GenICam::Client::GC_ERR_SUCCESS)
        {
            m_LastWriteAddress = Address;
            m_LastWriteSize = (size_t)Length;
        }
        Length = iLen;
        GENTLTEST_CHECK_MESSAGE("Failure GCWritePort(port=0x" << std::hex << m_hPort << 
                                             ", addr=0x" << Address << 
                                             ", buf=0x" << pBuffer << std::dec <<
                                             ", len=" << iLen <<
                                             ") writing register. Expected >= GC_ERR_SUCCESS, received " << sConvertGCError2String(m_LastResult).c_str(), 
            m_LastResult >= GenICam::Client::GC_ERR_SUCCESS);
    }

    virtual GenApi::EAccessMode GetAccessMode() const
    {
        return GenApi::RW;
    }

    ModPORT m_PortFuncs;
    GenICam::Client::PORT_HANDLE m_hPort;
    int64_t m_LastReadAddress;
    int64_t m_LastWriteAddress;
    size_t m_LastReadSize;
    size_t m_LastWriteSize;
	GenICam::Client::GC_ERROR m_LastResult;
};

#endif  //SFNCPORT_H
