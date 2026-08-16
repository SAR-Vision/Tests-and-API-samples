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

#include "Modules.h"
#include "GenTLTestTools.h"

//////////////////////////////////////////////////////////
// ModGC
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModGC::eGCGetInfo( GenICam::Client::TL_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;
    
    if (NULL == m_pFGCGetInfo)
        m_pFGCGetInfo = (GenICam::Client::PGCGetInfo)FnExportTest::poGetInstance()->pGCGetInfo();
    GENTLTEST_REQUIRE_MESSAGE("GCGetInfo not exported", m_pFGCGetInfo != NULL);

    GENTLTEST_LOG("<In > GCGetInfo(" << 
        "iInfoCmd=" << sConvertTLInfoCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCGetInfo(iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCGetInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> GCGetInfo(" << 
        "iInfoCmd=" << sConvertTLInfoCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModGC::eGCGetLastError( GenICam::Client::GC_ERROR *piErrorCode, char *sErrText, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCGetLastError)
        m_pFGCGetLastError = (GenICam::Client::PGCGetLastError)FnExportTest::poGetInstance()->pGCGetLastError();
    GENTLTEST_REQUIRE_MESSAGE("GCGetLastError not exported", m_pFGCGetLastError != NULL);

    GENTLTEST_LOG("<In > GCGetLastError(" << 
        ((piErrorCode != NULL)?(std::string("*piErrorCode=")+sConvertGCError2String(*piErrorCode)).c_str():"piErrorCode=NULL") <<
        ", sErrText=" << ((sErrText != NULL)?sErrText:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()).c_str():", piSize=NULL") <<
        ") " << std::endl);

    try 
    {
        eResult = m_pFGCGetLastError(piErrorCode, sErrText, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCGetLastError");
        throw;
    }

    GENTLTEST_LOG("<Out> GCGetLastError(" << 
        ((piErrorCode != NULL)?(std::string("*piErrorCode=")+sConvertGCError2String(*piErrorCode)).c_str():"piErrorCode=NULL") <<
        ", sErrText=" << ((sErrText != NULL)?sErrText:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()).c_str():", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModGC::eGCInitLib( void )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCInitLib)
        m_pFGCInitLib = (GenICam::Client::PGCInitLib)FnExportTest::poGetInstance()->pGCInitLib();
    GENTLTEST_REQUIRE_MESSAGE("GCInitLib not exported", m_pFGCInitLib != NULL);

    GENTLTEST_LOG("<In > GCInitLib()" << std::endl);

    try 
    {
        eResult = m_pFGCInitLib();
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCInitLib");
        throw;
    }

    GENTLTEST_LOG("<Out> GCInitLib() result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModGC::eGCCloseLib( void )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCCloseLib)
        m_pFGCCloseLib = (GenICam::Client::PGCCloseLib)FnExportTest::poGetInstance()->pGCCloseLib();
    GENTLTEST_REQUIRE_MESSAGE("GCCloseLib not exported", m_pFGCCloseLib != NULL);

    GENTLTEST_LOG("<In > GCCloseLib()" << std::endl);

    try 
    {
        eResult = m_pFGCCloseLib();
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCCloseLib");
        throw;
    }

    GENTLTEST_LOG("<Out> GCCloseLib() result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

//////////////////////////////////////////////////////////
// ModTL
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModTL::eTLOpen( GenICam::Client::TL_HANDLE *phTL )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLOpen)
        m_pFTLOpen = (GenICam::Client::PTLOpen)FnExportTest::poGetInstance()->pTLOpen();
    GENTLTEST_REQUIRE_MESSAGE("TLOpen not exported", m_pFTLOpen != NULL);

    GENTLTEST_LOG("<In > TLOpen(" << 
        ((phTL != NULL)?(std::string("*phTL=0x")+sConvertVoidPointer2String(*phTL).c_str()):"phTL=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLOpen(phTL);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLOpen");
        throw;
    }

    GENTLTEST_LOG("<Out> TLOpen(" << 
        ((phTL != NULL)?(std::string("*phTL=0x")+sConvertVoidPointer2String(*phTL).c_str()):"phTL=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLClose( GenICam::Client::TL_HANDLE hTL )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLClose)
        m_pFTLClose = (GenICam::Client::PTLClose)FnExportTest::poGetInstance()->pTLClose();
    GENTLTEST_REQUIRE_MESSAGE("TLClose not exported", m_pFTLClose != NULL);

    GENTLTEST_LOG("<In > TLClose(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLClose(hTL);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLClose");
        throw;
    }

    GENTLTEST_LOG("<Out> TLClose(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLGetInfo( GenICam::Client::TL_HANDLE hTL, GenICam::Client::TL_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLGetInfo)
        m_pFTLGetInfo = (GenICam::Client::PTLGetInfo)FnExportTest::poGetInstance()->pTLGetInfo();
    GENTLTEST_REQUIRE_MESSAGE("GCGetInfo not exported", m_pFTLGetInfo != NULL);

    GENTLTEST_LOG("<In > TLGetInfo(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", iInfoCmd=" << sConvertTLInfoCommand2String(iInfoCmd).c_str()<< 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLGetInfo(hTL, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLGetInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> TLGetInfo(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", iInfoCmd=" << sConvertTLInfoCommand2String(iInfoCmd).c_str()<< 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLGetNumInterfaces( GenICam::Client::TL_HANDLE hTL, uint32_t *piNumIfaces )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLGetNumInterfaces)
        m_pFTLGetNumInterfaces = (GenICam::Client::PTLGetNumInterfaces)FnExportTest::poGetInstance()->pTLGetNumInterfaces();
    GENTLTEST_REQUIRE_MESSAGE("TLGetNumInterfaces not exported", m_pFTLGetNumInterfaces != NULL);

    GENTLTEST_LOG("<In > TLGetNumInterfaces(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ((piNumIfaces != NULL)?(std::string(", *piNumIfaces=")+sConvertUint322String(*piNumIfaces).c_str()):", piNumIfaces=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLGetNumInterfaces(hTL, piNumIfaces);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLGetNumInterfaces");
        throw;
    }

    GENTLTEST_LOG("<Out> TLGetNumInterfaces(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ((piNumIfaces != NULL)?(std::string(", *piNumIfaces=")+sConvertUint322String(*piNumIfaces).c_str()):", piNumIfaces=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLGetInterfaceID( GenICam::Client::TL_HANDLE hTL, uint32_t iIndex,  char *sID, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLGetInterfaceID)
        m_pFTLGetInterfaceID = (GenICam::Client::PTLGetInterfaceID)FnExportTest::poGetInstance()->pTLGetInterfaceID();
    GENTLTEST_REQUIRE_MESSAGE("TLGetInterfaceID not exported", m_pFTLGetInterfaceID != NULL);

    GENTLTEST_LOG("<In > TLGetInterfaceID(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", iIndex=" << iIndex <<
        ", sID=" << ((sID != NULL)?sID:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLGetInterfaceID(hTL, iIndex,  sID, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLGetInterfaceID");
        throw;
    }

    GENTLTEST_LOG("<Out> TLGetInterfaceID(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", iIndex=" << iIndex <<
        ", sID=" << ((sID != NULL)?sID:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLGetInterfaceInfo( GenICam::Client::TL_HANDLE hTL, const char *sIfaceID, GenICam::Client::INTERFACE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLGetInterfaceInfo)
        m_pFTLGetInterfaceInfo = (GenICam::Client::PTLGetInterfaceInfo)FnExportTest::poGetInstance()->pTLGetInterfaceInfo();
    GENTLTEST_REQUIRE_MESSAGE("TLGetInterfaceInfo not exported", m_pFTLGetInterfaceInfo != NULL);

    GENTLTEST_LOG("<In > TLGetInterfaceInfo(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", sIfaceID=" << ((sIfaceID != NULL)?sIfaceID:"<null>") <<
        ", iInfoCmd=" << sConvertInterfaceCommand2String(iInfoCmd).c_str()<< 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLGetInterfaceInfo(hTL, sIfaceID, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLGetInterfaceInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> TLGetInterfaceInfo(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", sIfaceID=" << ((sIfaceID != NULL)?sIfaceID:"<null>") <<
        ", iInfoCmd=" << sConvertInterfaceCommand2String(iInfoCmd).c_str()<< 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLOpenInterface( GenICam::Client::TL_HANDLE hTL, const char *sIfaceID, GenICam::Client::IF_HANDLE *phIface )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLOpenInterface)
        m_pFTLOpenInterface = (GenICam::Client::PTLOpenInterface)FnExportTest::poGetInstance()->pTLOpenInterface();
    GENTLTEST_REQUIRE_MESSAGE("TLOpenInterface not exported", m_pFTLOpenInterface != NULL);

    GENTLTEST_LOG("<In > TLOpenInterface(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", sIfaceID=" << ((sIfaceID != NULL)?sIfaceID:"<null>") <<
        ((phIface != NULL)?(std::string(", *phIface=0x")+sConvertVoidPointer2String(*phIface).c_str()):", phIface=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLOpenInterface(hTL, sIfaceID, phIface);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLOpenInterface");
        throw;
    }

    GENTLTEST_LOG("<Out> TLOpenInterface(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ", sIfaceID=" << ((sIfaceID != NULL)?sIfaceID:"<null>") <<
        ((phIface != NULL)?(std::string(", *phIface=0x")+sConvertVoidPointer2String(*phIface).c_str()):", phIface=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModTL::eTLUpdateInterfaceList( GenICam::Client::TL_HANDLE hTL, bool8_t *pbChanged, uint64_t iTimeout )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFTLUpdateInterfaceList)
        m_pFTLUpdateInterfaceList = (GenICam::Client::PTLUpdateInterfaceList)FnExportTest::poGetInstance()->pTLUpdateInterfaceList();
    GENTLTEST_REQUIRE_MESSAGE("TLUpdateInterfaceList not exported", m_pFTLUpdateInterfaceList != NULL);

    GENTLTEST_LOG("<In > TLUpdateInterfaceList(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ((pbChanged != NULL)?(std::string(", *pbChanged=")+sConvertBool82String(*pbChanged).c_str()):", pbChanged=NULL") <<
        ", iTimeout=" << sConvertUint642String(iTimeout).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFTLUpdateInterfaceList(hTL, pbChanged, iTimeout);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("TLUpdateInterfaceList");
        throw;
    }

    GENTLTEST_LOG("<Out> TLUpdateInterfaceList(" << 
        "hTL=0x" << sConvertVoidPointer2String(hTL).c_str() <<
        ((pbChanged != NULL)?(std::string(", *pbChanged=")+sConvertBool82String(*pbChanged).c_str()):", pbChanged=NULL") <<
        ", iTimeout=" << sConvertUint642String(iTimeout).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

//////////////////////////////////////////////////////////
// ModIF
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModIF::eIFClose( GenICam::Client::IF_HANDLE hIface )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFClose)
        m_pFIFClose = (GenICam::Client::PIFClose)FnExportTest::poGetInstance()->pIFClose();
    GENTLTEST_REQUIRE_MESSAGE("IFClose not exported", m_pFIFClose != NULL);

    GENTLTEST_LOG("<In > IFClose(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFClose(hIface);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFClose");
        throw;
    }

    GENTLTEST_LOG("<Out> IFClose(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFGetInfo( GenICam::Client::IF_HANDLE hIface, GenICam::Client::INTERFACE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFGetInfo)
        m_pFIFGetInfo = (GenICam::Client::PIFGetInfo)FnExportTest::poGetInstance()->pIFGetInfo();
    GENTLTEST_REQUIRE_MESSAGE("IFGetInfo not exported", m_pFIFGetInfo != NULL);

    GENTLTEST_LOG("<In > IFGetInfo(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", iInfoCmd=" << sConvertInterfaceCommand2String(iInfoCmd).c_str()<< 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFGetInfo(hIface, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFGetInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> IFGetInfo(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", iInfoCmd=" << sConvertInterfaceCommand2String(iInfoCmd).c_str()<< 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFGetNumDevices( GenICam::Client::IF_HANDLE hIface, uint32_t *piNumDevices )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFGetNumDevices)
        m_pFIFGetNumDevices = (GenICam::Client::PIFGetNumDevices)FnExportTest::poGetInstance()->pIFGetNumDevices();
    GENTLTEST_REQUIRE_MESSAGE("IFGetNumDevices not exported", m_pFIFGetNumDevices != NULL);

    GENTLTEST_LOG("<In > IFGetNumDevices(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ((piNumDevices != NULL)?(std::string(", *piNumDevices=")+sConvertUint322String(*piNumDevices).c_str()):", piNumDevices=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFGetNumDevices(hIface, piNumDevices);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFGetNumDevices");
        throw;
    }

    GENTLTEST_LOG("<Out> IFGetNumDevices(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ((piNumDevices != NULL)?(std::string(", *piNumDevices=")+sConvertUint322String(*piNumDevices).c_str()):", piNumDevices=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFGetDeviceID( GenICam::Client::IF_HANDLE hIface, uint32_t iIndex, char *sIDeviceID, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFGetDeviceID)
        m_pFIFGetDeviceID = (GenICam::Client::PIFGetDeviceID)FnExportTest::poGetInstance()->pIFGetDeviceID();
    GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceID not exported", m_pFIFGetDeviceID != NULL);

    GENTLTEST_LOG("<In > IFGetDeviceID(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", iIndex=" << iIndex <<
        ", sIDeviceID=" << ((sIDeviceID != NULL)?sIDeviceID:"<null>") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFGetDeviceID(hIface, iIndex, sIDeviceID, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFGetDeviceID");
        throw;
    }

    GENTLTEST_LOG("<Out> IFGetDeviceID(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", iIndex=" << iIndex <<
        ", sIDeviceID=" << ((sIDeviceID != NULL)?sIDeviceID:"<null>") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFUpdateDeviceList( GenICam::Client::IF_HANDLE hIface, bool8_t *pbChanged, uint64_t iTimeout )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFUpdateDeviceList)
        m_pFIFUpdateDeviceList = (GenICam::Client::PIFUpdateDeviceList)FnExportTest::poGetInstance()->pIFUpdateDeviceList();
    GENTLTEST_REQUIRE_MESSAGE("IFUpdateDeviceList not exported", m_pFIFUpdateDeviceList != NULL);

    GENTLTEST_LOG("<In > IFUpdateDeviceList(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ((pbChanged != NULL)?(std::string(", *pbChanged=")+sConvertBool82String(*pbChanged).c_str()):", pbChanged=NULL") <<
        ", iTimeout=" << sConvertUint642String(iTimeout).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFUpdateDeviceList(hIface, pbChanged, iTimeout);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFUpdateDeviceList");
        throw;
    }

    GENTLTEST_LOG("<Out> IFUpdateDeviceList(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ((pbChanged != NULL)?(std::string(", *pbChanged=")+sConvertBool82String(*pbChanged).c_str()):", pbChanged=NULL") <<
        ", iTimeout=" << sConvertUint642String(iTimeout).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFGetDeviceInfo( GenICam::Client::IF_HANDLE hIface, const char *sDeviceID, GenICam::Client::DEVICE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFGetDeviceInfo)
        m_pFIFGetDeviceInfo = (GenICam::Client::PIFGetDeviceInfo)FnExportTest::poGetInstance()->pIFGetDeviceInfo();
    GENTLTEST_REQUIRE_MESSAGE("IFGetDeviceInfo not exported", m_pFIFGetDeviceInfo != NULL);

    GENTLTEST_LOG("<In > IFGetDeviceInfo(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", sDeviceID=" << ((sDeviceID != NULL)?sDeviceID:"<null>") <<
        ", iInfoCmd=" << sConvertDeviceCommand2String((GenICam::Client::DEVICE_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFGetDeviceInfo(hIface, sDeviceID, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFGetDeviceInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> IFGetDeviceInfo(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", sDeviceID=" << ((sDeviceID != NULL)?sDeviceID:"<null>") <<
        ", iInfoCmd=" << sConvertDeviceCommand2String((GenICam::Client::DEVICE_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFOpenDevice( GenICam::Client::IF_HANDLE hIface, const char *sDeviceID, GenICam::Client::DEVICE_ACCESS_FLAGS iOpenFlags, GenICam::Client::DEV_HANDLE *phDevice )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFOpenDevice)
        m_pFIFOpenDevice = (GenICam::Client::PIFOpenDevice)FnExportTest::poGetInstance()->pIFOpenDevice();
    GENTLTEST_REQUIRE_MESSAGE("IFOpenDevice not exported", m_pFIFOpenDevice != NULL);

    GENTLTEST_LOG("<In > IFOpenDevice(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", sDeviceID=" << ((sDeviceID != NULL)?sDeviceID:"<null>") <<
        ", iOpenFlags=" << sConvertDEVICEAccess2String((GenICam::Client::DEVICE_ACCESS_FLAGS_LIST)iOpenFlags).c_str() << 
        ((phDevice != NULL)?(std::string(", *phDevice=0x")+sConvertVoidPointer2String(*phDevice).c_str()):", phDevice=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFOpenDevice(hIface, sDeviceID, iOpenFlags, phDevice);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFOpenDevice");
        throw;
    }

    GENTLTEST_LOG("<Out> IFOpenDevice(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ", sDeviceID=" << ((sDeviceID != NULL)?sDeviceID:"<null>") <<
        ", iOpenFlags=" << sConvertDEVICEAccess2String((GenICam::Client::DEVICE_ACCESS_FLAGS_LIST)iOpenFlags).c_str() << 
        ((phDevice != NULL)?(std::string(", *phDevice=0x")+sConvertVoidPointer2String(*phDevice).c_str()):", phDevice=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModIF::eIFGetParentTL( GenICam::Client::IF_HANDLE hIface, GenICam::Client::TL_HANDLE *phSystem )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFIFGetParentTL)
        m_pFIFGetParentTL = (GenICam::Client::PIFGetParentTL)FnExportTest::poGetInstance()->pIFGetParentTL();
    GENTLTEST_REQUIRE_MESSAGE("IFGetParentTL not exported", m_pFIFGetParentTL != NULL);

    GENTLTEST_LOG("<In > IFGetParentTL(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ((phSystem != NULL)?(std::string(", *phSystem=0x")+sConvertVoidPointer2String(*phSystem).c_str()):", phSystem=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFIFGetParentTL(hIface, phSystem);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("IFGetParentTL");
        throw;
    }

    GENTLTEST_LOG("<Out> IFGetParentTL(" << 
        "hIface=0x" << sConvertVoidPointer2String(hIface).c_str() <<
        ((phSystem != NULL)?(std::string(", *phSystem=0x")+sConvertVoidPointer2String(*phSystem).c_str()):", phSystem=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

//////////////////////////////////////////////////////////
// ModDEV
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModDEV::eDevGetPort( GenICam::Client::DEV_HANDLE hDevice, GenICam::Client::PORT_HANDLE *phRemoteDevice )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevGetPort)
        m_pFDevGetPort = (GenICam::Client::PDevGetPort)FnExportTest::poGetInstance()->pDevGetPort();
    GENTLTEST_REQUIRE_MESSAGE("DevGetPort not exported", m_pFDevGetPort != NULL);

    GENTLTEST_LOG("<In > DevGetPort(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ((phRemoteDevice != NULL)?(std::string(", *phRemoteDevice=0x")+sConvertVoidPointer2String(*phRemoteDevice).c_str()):", phRemoteDevice=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevGetPort(hDevice, phRemoteDevice);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevGetPort");
        throw;
    }

    GENTLTEST_LOG("<Out> DevGetPort(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ((phRemoteDevice != NULL)?(std::string(", *phRemoteDevice=0x")+sConvertVoidPointer2String(*phRemoteDevice).c_str()):", phRemoteDevice=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDEV::eDevGetNumDataStreams( GenICam::Client::DEV_HANDLE hDevice, uint32_t *piNumDataStreams )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevGetNumDataStreams)
        m_pFDevGetNumDataStreams = (GenICam::Client::PDevGetNumDataStreams)FnExportTest::poGetInstance()->pDevGetNumDataStreams();
    GENTLTEST_REQUIRE_MESSAGE("DevGetNumDataStreams not exported", m_pFDevGetNumDataStreams != NULL);
    
    GENTLTEST_LOG("<In > DevGetNumDataStreams(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ((piNumDataStreams != NULL)?(std::string(", *piNumDataStreams=")+sConvertUint322String(*piNumDataStreams).c_str()):", piNumDataStreams=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevGetNumDataStreams(hDevice, piNumDataStreams);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevGetNumDataStreams");
        throw;
    }

    GENTLTEST_LOG("<Out> DevGetNumDataStreams(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ((piNumDataStreams != NULL)?(std::string(", *piNumDataStreams=")+sConvertUint322String(*piNumDataStreams).c_str()):", piNumDataStreams=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDEV::eDevGetDataStreamID( GenICam::Client::DEV_HANDLE hDevice, uint32_t iIndex, char *sDataStreamID, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevGetDataStreamID)
        m_pFDevGetDataStreamID = (GenICam::Client::PDevGetDataStreamID)FnExportTest::poGetInstance()->pDevGetDataStreamID();
    GENTLTEST_REQUIRE_MESSAGE("DevGetDataStreamID not exported", m_pFDevGetDataStreamID != NULL);

    GENTLTEST_LOG("<In > DevGetDataStreamID(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ", iIndex=" << iIndex <<
        ", sDataStreamID=" << ((sDataStreamID != NULL)?sDataStreamID:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevGetDataStreamID(hDevice, iIndex, sDataStreamID, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevGetDataStreamID");
        throw;
    }

    GENTLTEST_LOG("<Out> DevGetDataStreamID(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ", iIndex=" << iIndex <<
        ", sDataStreamID=" << ((sDataStreamID != NULL)?sDataStreamID:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDEV::eDevOpenDataStream( GenICam::Client::DEV_HANDLE hDevice, const char *sDataStreamID, GenICam::Client::DS_HANDLE *phDataStream )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevOpenDataStream)
        m_pFDevOpenDataStream = (GenICam::Client::PDevOpenDataStream)FnExportTest::poGetInstance()->pDevOpenDataStream();
    GENTLTEST_REQUIRE_MESSAGE("DevOpenDataStream not exported", m_pFDevOpenDataStream != NULL);

    GENTLTEST_LOG("<In > DevOpenDataStream(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ", sDataStreamID=" << ((sDataStreamID != NULL)?sDataStreamID:"<null>") <<
        ((phDataStream != NULL)?(std::string(", *phDataStream=0x")+sConvertVoidPointer2String(*phDataStream).c_str()):", phDataStream=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevOpenDataStream(hDevice, sDataStreamID, phDataStream);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevOpenDataStream");
        throw;
    }

    GENTLTEST_LOG("<Out> DevOpenDataStream(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ", sDataStreamID=" << ((sDataStreamID != NULL)?sDataStreamID:"<null>") <<
        ((phDataStream != NULL)?(std::string(", *phDataStream=0x")+sConvertVoidPointer2String(*phDataStream).c_str()):", phDataStream=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDEV::eDevGetInfo( GenICam::Client::DEV_HANDLE hDevice, GenICam::Client::DEVICE_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevGetInfo)
        m_pFDevGetInfo = (GenICam::Client::PDevGetInfo)FnExportTest::poGetInstance()->pDevGetInfo();
    GENTLTEST_REQUIRE_MESSAGE("DevGetInfo not exported", m_pFDevGetInfo != NULL);

    GENTLTEST_LOG("<In > DevGetInfo(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ", iInfoCmd=" << sConvertDeviceCommand2String((GenICam::Client::DEVICE_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevGetInfo(hDevice, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevGetInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> DevGetInfo(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ", iInfoCmd=" << sConvertDeviceCommand2String((GenICam::Client::DEVICE_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDEV::eDevClose( GenICam::Client::DEV_HANDLE hDevice )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevClose)
        m_pFDevClose = (GenICam::Client::PDevClose)FnExportTest::poGetInstance()->pDevClose();
    GENTLTEST_REQUIRE_MESSAGE("DevClose not exported", m_pFDevClose != NULL);
    
    GENTLTEST_LOG("<In > DevClose(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevClose(hDevice);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevClose");
        throw;
    }

    GENTLTEST_LOG("<Out> DevClose(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDEV::eDevGetParentIF( GenICam::Client::DEV_HANDLE hDevice, GenICam::Client::IF_HANDLE *phIface )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDevGetParentIF)
        m_pFDevGetParentIF = (GenICam::Client::PDevGetParentIF)FnExportTest::poGetInstance()->pDevGetParentIF();
    GENTLTEST_REQUIRE_MESSAGE("DevGetParentIF not exported", m_pFDevGetParentIF != NULL);

    GENTLTEST_LOG("<In > DevGetParentIF(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ((phIface != NULL)?(std::string(", *phIface=0x")+sConvertVoidPointer2String(*phIface).c_str()):", phIface=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDevGetParentIF(hDevice, phIface);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DevGetParentIF");
        throw;
    }

    GENTLTEST_LOG("<Out> DevGetParentIF(" << 
        "hDevice=0x" << sConvertVoidPointer2String(hDevice).c_str() <<
        ((phIface != NULL)?(std::string(", *phIface=0x")+sConvertVoidPointer2String(*phIface).c_str()):", phIface=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

//////////////////////////////////////////////////////////
// ModDS
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModDS::eDSAnnounceBuffer( GenICam::Client::DS_HANDLE hDataStream, void *pBuffer, size_t iSize, void *pPrivate, GenICam::Client::BUFFER_HANDLE *phBuffer )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSAnnounceBuffer)
        m_pFDSAnnounceBuffer = (GenICam::Client::PDSAnnounceBuffer)FnExportTest::poGetInstance()->pDSAnnounceBuffer();
    GENTLTEST_REQUIRE_MESSAGE("DSAnnounceBuffer not exported", m_pFDSAnnounceBuffer != NULL);

    GENTLTEST_LOG("<In > DSAnnounceBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ", iSize=" << iSize <<
        ", pPrivate=0x" << sConvertVoidPointer2String(pPrivate).c_str() <<
        ((phBuffer != NULL)?(std::string(", *phBuffer=0x")+sConvertVoidPointer2String(*phBuffer).c_str()):", phBuffer=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSAnnounceBuffer(hDataStream, pBuffer, iSize, pPrivate, phBuffer);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSAnnounceBuffer");
        throw;
    }

    GENTLTEST_LOG("<Out> DSAnnounceBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ", iSize=" << iSize <<
        ", pPrivate=0x" << sConvertVoidPointer2String(pPrivate).c_str() <<
        ((phBuffer != NULL)?(std::string(", *phBuffer=0x")+sConvertVoidPointer2String(*phBuffer).c_str()):", phBuffer=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSAllocAndAnnounceBuffer( GenICam::Client::DS_HANDLE hDataStream, size_t iSize, void *pPrivate, GenICam::Client::BUFFER_HANDLE *phBuffer )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSAllocAndAnnounceBuffer)
        m_pFDSAllocAndAnnounceBuffer = (GenICam::Client::PDSAllocAndAnnounceBuffer)FnExportTest::poGetInstance()->pDSAllocAndAnnounceBuffer();
    GENTLTEST_REQUIRE_MESSAGE("DSAllocAndAnnounceBuffer not exported", m_pFDSAllocAndAnnounceBuffer != NULL);

    GENTLTEST_LOG("<In > DSAllocAndAnnounceBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iSize=" << iSize <<
        ", pPrivate=0x" << sConvertVoidPointer2String(pPrivate).c_str() <<
        ((phBuffer != NULL)?(std::string(", *phBuffer=0x")+sConvertVoidPointer2String(*phBuffer).c_str()):", phBuffer=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSAllocAndAnnounceBuffer(hDataStream, iSize, pPrivate, phBuffer);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSAllocAndAnnounceBuffer");
        throw;
    }

    GENTLTEST_LOG("<Out> DSAllocAndAnnounceBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iSize=" << iSize <<
        ", pPrivate=0x" << sConvertVoidPointer2String(pPrivate).c_str() <<
        ((phBuffer != NULL)?(std::string(", *phBuffer=0x")+sConvertVoidPointer2String(*phBuffer).c_str()):", phBuffer=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSFlushQueue( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::ACQ_QUEUE_TYPE iOperation )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSFlushQueue)
        m_pFDSFlushQueue = (GenICam::Client::PDSFlushQueue)FnExportTest::poGetInstance()->pDSFlushQueue();
    GENTLTEST_REQUIRE_MESSAGE("DSFlushQueue not exported", m_pFDSFlushQueue != NULL);

    GENTLTEST_LOG("<In > DSFlushQueue(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iOperation=" << sConvertDataStreamOperation2String(iOperation).c_str() << 
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSFlushQueue(hDataStream, iOperation);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSFlushQueue");
        throw;
    }

    GENTLTEST_LOG("<Out> DSFlushQueue(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iOperation=" << sConvertDataStreamOperation2String(iOperation).c_str() << 
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSStartAcquisition( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::ACQ_START_FLAGS iStartFlags, uint64_t iNumToAcquire )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSStartAcquisition)
        m_pFDSStartAcquisition = (GenICam::Client::PDSStartAcquisition)FnExportTest::poGetInstance()->pDSStartAcquisition();
    GENTLTEST_REQUIRE_MESSAGE("DSStartAcquisition not exported", m_pFDSStartAcquisition != NULL);

    GENTLTEST_LOG("<In > DSStartAcquisition(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iStartFlags=" << sConvertDataStreamStartFlag2String(iStartFlags).c_str() << 
        ", iNumToAcquire=" << iNumToAcquire <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSStartAcquisition(hDataStream, iStartFlags, iNumToAcquire);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSStartAcquisition");
        throw;
    }

    GENTLTEST_LOG("<Out> DSStartAcquisition(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iStartFlags=" << sConvertDataStreamStartFlag2String(iStartFlags).c_str() << 
        ", iNumToAcquire=" << iNumToAcquire <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSStopAcquisition( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::ACQ_STOP_FLAGS iStopFlags )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSStopAcquisition)
        m_pFDSStopAcquisition = (GenICam::Client::PDSStopAcquisition)FnExportTest::poGetInstance()->pDSStopAcquisition();
    GENTLTEST_REQUIRE_MESSAGE("DSStopAcquisition not exported", m_pFDSStopAcquisition != NULL);

    GENTLTEST_LOG("<In > DSStopAcquisition(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iStopFlags=" << sConvertDataStreamStopFlag2String(iStopFlags).c_str() << 
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSStopAcquisition(hDataStream, iStopFlags);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSStopAcquisition");
        throw;
    }

    GENTLTEST_LOG("<Out> DSStopAcquisition(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iStopFlags=" << sConvertDataStreamStopFlag2String(iStopFlags).c_str() << 
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSGetInfo( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::STREAM_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSGetInfo)
        m_pFDSGetInfo = (GenICam::Client::PDSGetInfo)FnExportTest::poGetInstance()->pDSGetInfo();
    GENTLTEST_REQUIRE_MESSAGE("DSGetInfo not exported", m_pFDSGetInfo != NULL);

    GENTLTEST_LOG("<In > DSGetInfo(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iInfoCmd=" << sConvertDataStreamCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSGetInfo(hDataStream, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSGetInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> DSGetInfo(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iInfoCmd=" << sConvertDataStreamCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSGetBufferID( GenICam::Client::DS_HANDLE hDataStream, uint32_t iIndex, GenICam::Client::BUFFER_HANDLE *phBuffer )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSGetBufferID)
        m_pFDSGetBufferID = (GenICam::Client::PDSGetBufferID)FnExportTest::poGetInstance()->pDSGetBufferID();
    GENTLTEST_REQUIRE_MESSAGE("DSGetBufferID not exported", m_pFDSGetBufferID != NULL);

    GENTLTEST_LOG("<In > DSGetBufferID(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iIndex=" << iIndex <<
        ((phBuffer != NULL)?(std::string(", *phBuffer=0x")+sConvertVoidPointer2String(*phBuffer).c_str()):", phBuffer=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSGetBufferID(hDataStream, iIndex, phBuffer );
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSGetBufferID");
        throw;
    }

    GENTLTEST_LOG("<Out> DSGetBufferID(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", iIndex=" << iIndex <<
        ((phBuffer != NULL)?(std::string(", *phBuffer=0x")+sConvertVoidPointer2String(*phBuffer).c_str()):", phBuffer=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSClose( GenICam::Client::DS_HANDLE hDataStream )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSClose)
        m_pFDSClose = (GenICam::Client::PDSClose)FnExportTest::poGetInstance()->pDSClose();
    GENTLTEST_REQUIRE_MESSAGE("DSClose not exported", m_pFDSClose != NULL);

    GENTLTEST_LOG("<In > DSClose(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSClose(hDataStream);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSClose");
        throw;
    }

    GENTLTEST_LOG("<Out> DSClose(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSRevokeBuffer( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer, void **pBuffer, void **pPrivate )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSRevokeBuffer)
        m_pFDSRevokeBuffer = (GenICam::Client::PDSRevokeBuffer)FnExportTest::poGetInstance()->pDSRevokeBuffer();
    GENTLTEST_REQUIRE_MESSAGE("DSRevokeBuffer not exported", m_pFDSRevokeBuffer != NULL);
    
    GENTLTEST_LOG("<In > DSRevokeBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ((pBuffer != NULL)?(std::string(", *pBuffer=0x")+sConvertVoidPointer2String(*pBuffer).c_str()):", pBuffer=NULL") <<
        ((pPrivate != NULL)?(std::string(", *pPrivate=0x")+sConvertVoidPointer2String(*pPrivate).c_str()):", pPrivate=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSRevokeBuffer(hDataStream, hBuffer, pBuffer, pPrivate);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSRevokeBuffer");
        throw;
    }

    GENTLTEST_LOG("<Out> DSRevokeBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ((pBuffer != NULL)?(std::string(", *pBuffer=0x")+sConvertVoidPointer2String(*pBuffer).c_str()):", pBuffer=NULL") <<
        ((pPrivate != NULL)?(std::string(", *pPrivate=0x")+sConvertVoidPointer2String(*pPrivate).c_str()):", pPrivate=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSQueueBuffer( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSQueueBuffer)
        m_pFDSQueueBuffer = (GenICam::Client::PDSQueueBuffer)FnExportTest::poGetInstance()->pDSQueueBuffer();
    GENTLTEST_REQUIRE_MESSAGE("DSQueueBuffer not exported", m_pFDSQueueBuffer != NULL);

    GENTLTEST_LOG("<In > DSQueueBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSQueueBuffer(hDataStream, hBuffer);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSQueueBuffer");
        throw;
    }

    GENTLTEST_LOG("<Out> DSQueueBuffer(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSGetBufferInfo( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer, GenICam::Client::BUFFER_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSGetBufferInfo)
        m_pFDSGetBufferInfo = (GenICam::Client::PDSGetBufferInfo)FnExportTest::poGetInstance()->pDSGetBufferInfo();
    GENTLTEST_REQUIRE_MESSAGE("DSGetBufferInfo not exported", m_pFDSGetBufferInfo != NULL);

    GENTLTEST_LOG("<In > DSGetBufferInfo(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ", iInfoCmd=" << sConvertDataStreamBufferCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSGetBufferInfo(hDataStream, hBuffer, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSGetBufferInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> DSGetBufferInfo(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ", iInfoCmd=" << sConvertDataStreamBufferCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSGetBufferChunkData( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::BUFFER_HANDLE hBuffer, GenICam::Client::SINGLE_CHUNK_DATA *pChunkData, size_t *piNumChunks )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSGetBufferChunkData)
        m_pFDSGetBufferChunkData = (GenICam::Client::PDSGetBufferChunkData)FnExportTest::poGetInstance()->pDSGetBufferChunkData();
    GENTLTEST_REQUIRE_MESSAGE("DSGetBufferChunkData not exported", m_pFDSGetBufferChunkData != NULL);

    GENTLTEST_LOG("<In > DSGetBufferChunkData(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ((pChunkData != NULL)?(std::string(", pChunkData->ChunkID=0x")+sConvertUint642HexString(pChunkData->ChunkID).c_str()+
                               std::string(", pChunkData->ChunkOffset=")+sConvertSizeT2String(pChunkData->ChunkOffset).c_str()+
                               std::string(", pChunkData->ChunkLength=")+sConvertSizeT2String(pChunkData->ChunkLength).c_str()):", pChunkData=NULL") <<
        ((piNumChunks != NULL)?(std::string(", *piNumChunks=")+sConvertSizeT2String(*piNumChunks).c_str()):", piNumChunks=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSGetBufferChunkData(hDataStream, hBuffer, pChunkData, piNumChunks);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSGetBufferChunkData");
        throw;
    }

    GENTLTEST_LOG("<Out> DSGetBufferChunkData(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ", hBuffer=0x" << sConvertVoidPointer2String(hBuffer).c_str() <<
        ((pChunkData != NULL)?(std::string(", pChunkData->ChunkID=0x")+sConvertUint642HexString(pChunkData->ChunkID).c_str()+
                               std::string(", pChunkData->ChunkOffset=")+sConvertSizeT2String(pChunkData->ChunkOffset).c_str()+
                               std::string(", pChunkData->ChunkLength=")+sConvertSizeT2String(pChunkData->ChunkLength).c_str()):", pChunkData=NULL") <<
        ((piNumChunks != NULL)?(std::string(", *piNumChunks=")+sConvertSizeT2String(*piNumChunks).c_str()):", piNumChunks=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModDS::eDSGetParentDev( GenICam::Client::DS_HANDLE hDataStream, GenICam::Client::DEV_HANDLE *phDevice )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFDSGetParentDev)
        m_pFDSGetParentDev = (GenICam::Client::PDSGetParentDev)FnExportTest::poGetInstance()->pDSGetParentDev();
    GENTLTEST_REQUIRE_MESSAGE("DSGetParentDev not exported", m_pFDSGetParentDev != NULL);

    GENTLTEST_LOG("<In > DSGetParentDev(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ((phDevice != NULL)?(std::string(", *phDevice=0x")+sConvertVoidPointer2String(*phDevice).c_str()):", phDevice=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFDSGetParentDev(hDataStream, phDevice);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("DSGetParentDev");
        throw;
    }

    GENTLTEST_LOG("<Out> DSGetParentDev(" << 
        "hDataStream=0x" << sConvertVoidPointer2String(hDataStream).c_str() <<
        ((phDevice != NULL)?(std::string(", *phDevice=0x")+sConvertVoidPointer2String(*phDevice).c_str()):", phDevice=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

//////////////////////////////////////////////////////////
// ModPORT
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModPORT::eGCReadPort( GenICam::Client::PORT_HANDLE hPort, uint64_t iAddress, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCReadPort)
        m_pFGCReadPort = (GenICam::Client::PGCReadPort)FnExportTest::poGetInstance()->pGCReadPort();
    GENTLTEST_REQUIRE_MESSAGE("GCReadPort not exported", m_pFGCReadPort != NULL);

    GENTLTEST_LOG("<In > GCReadPort(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iAddress=0x" << sConvertUint642HexString(iAddress).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCReadPort(hPort, iAddress, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCReadPort");
        throw;
    }

    GENTLTEST_LOG("<Out> GCReadPort(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iAddress=0x" << sConvertUint642HexString(iAddress).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCWritePort( GenICam::Client::PORT_HANDLE hPort, uint64_t iAddress, const void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCWritePort)
        m_pFGCWritePort = (GenICam::Client::PGCWritePort)FnExportTest::poGetInstance()->pGCWritePort();
    GENTLTEST_REQUIRE_MESSAGE("GCWritePort not exported", m_pFGCWritePort != NULL);

    GENTLTEST_LOG("<In > GCWritePort(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iAddress=0x" << sConvertUint642HexString(iAddress).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String((void*)pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCWritePort(hPort, iAddress, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCWritePort");
        throw;
    }

    GENTLTEST_LOG("<Out> GCWritePort(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iAddress=0x" << sConvertUint642HexString(iAddress).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String((void*)pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCGetPortURL( GenICam::Client::PORT_HANDLE hPort, char *sURL, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCGetPortURL)
        m_pFGCGetPortURL = (GenICam::Client::PGCGetPortURL)FnExportTest::poGetInstance()->pGCGetPortURL();
    GENTLTEST_REQUIRE_MESSAGE("GCGetPortURL not exported", m_pFGCGetPortURL != NULL);

    GENTLTEST_LOG("<In > GCGetPortURL(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", sURL=" << ((sURL != NULL)?sURL:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCGetPortURL(hPort, sURL, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCGetPortURL");
        throw;
    }

    GENTLTEST_LOG("<Out> GCGetPortURL(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", sURL=" << ((sURL != NULL)?sURL:"<null>") <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCGetNumPortURLs( GenICam::Client::PORT_HANDLE hPort, uint32_t *piNumURLs )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCGetNumPortURLs)
        m_pFGCGetNumPortURLs = (GenICam::Client::PGCGetNumPortURLs)FnExportTest::poGetInstance()->pGCGetNumPortURLs();
    GENTLTEST_REQUIRE_MESSAGE("GCGetNumPortURLs not exported", m_pFGCGetNumPortURLs != NULL);

    GENTLTEST_LOG("<In > GCGetNumPortURLs(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ((piNumURLs != NULL)?(std::string(", *piNumURLs=")+sConvertUint322String(*piNumURLs).c_str()):", piNumURLs=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCGetNumPortURLs(hPort, piNumURLs);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCGetNumPortURLs");
        throw;
    }

    GENTLTEST_LOG("<Out> GCGetNumPortURLs(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ((piNumURLs != NULL)?(std::string(", *piNumURLs=")+sConvertUint322String(*piNumURLs).c_str()):", piNumURLs=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCGetPortInfo( GenICam::Client::PORT_HANDLE hPort, GenICam::Client::PORT_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCGetPortInfo)
        m_pFGCGetPortInfo = (GenICam::Client::PGCGetPortInfo)FnExportTest::poGetInstance()->pGCGetPortInfo();
    GENTLTEST_REQUIRE_MESSAGE("GCGetPortInfo not exported", m_pFGCGetPortInfo != NULL);

    GENTLTEST_LOG("<In > GCGetPortInfo(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iInfoCmd=" << sConvertPORTCommand2String((GenICam::Client::PORT_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCGetPortInfo(hPort, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCGetPortInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> GCGetPortInfo(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iInfoCmd=" << sConvertPORTCommand2String((GenICam::Client::PORT_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCGetPortURLInfo( GenICam::Client::PORT_HANDLE hPort, uint32_t iURLIndex, GenICam::Client::URL_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCGetPortURLInfo)
        m_pFGCGetPortURLInfo = (GenICam::Client::PGCGetPortURLInfo)FnExportTest::poGetInstance()->pGCGetPortURLInfo();
    GENTLTEST_REQUIRE_MESSAGE("GCGetPortURLInfo not exported", m_pFGCGetPortURLInfo != NULL);

    GENTLTEST_LOG("<In > GCGetPortURLInfo(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iURLIndex=" << iURLIndex <<
        ", iInfoCmd=" << sConvertURLInfoCommand2String((GenICam::Client::URL_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCGetPortURLInfo(hPort, iURLIndex, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCGetPortURLInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> GCGetPortURLInfo(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ", iURLIndex=" << iURLIndex <<
        ", iInfoCmd=" << sConvertURLInfoCommand2String((GenICam::Client::URL_INFO_CMD_LIST)iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCReadPortStacked( GenICam::Client::PORT_HANDLE hPort, GenICam::Client::PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCReadPortStacked)
        m_pFGCReadPortStacked = (GenICam::Client::PGCReadPortStacked)FnExportTest::poGetInstance()->pGCReadPortStacked();
    GENTLTEST_REQUIRE_MESSAGE("GCReadPortStacked not exported", m_pFGCReadPortStacked != NULL);

    GENTLTEST_LOG("<In > GCReadPortStacked(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ((pEntries != NULL)?(std::string(", pEntries->Address=0x")+sConvertUint642HexString(pEntries->Address).c_str()+
                             std::string(", pEntries->pBuffer=0x")+sConvertVoidPointer2String(pEntries->pBuffer).c_str()+
                             std::string(", pEntries->Size=")+sConvertSizeT2String(pEntries->Size).c_str()):", pEntries=NULL") <<
        ((piNumEntries != NULL)?(std::string(", *piNumEntries=")+sConvertSizeT2String(*piNumEntries).c_str()):", piNumEntries=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCReadPortStacked(hPort, pEntries, piNumEntries);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCReadPortStacked");
        throw;
    }

    GENTLTEST_LOG("<Out> GCReadPortStacked(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ((pEntries != NULL)?(std::string(", pEntries->Address=0x")+sConvertUint642HexString(pEntries->Address).c_str()+
                             std::string(", pEntries->pBuffer=0x")+sConvertVoidPointer2String(pEntries->pBuffer).c_str()+
                             std::string(", pEntries->Size=")+sConvertSizeT2String(pEntries->Size).c_str()):", pEntries=NULL") <<
        ((piNumEntries != NULL)?(std::string(", *piNumEntries=")+sConvertSizeT2String(*piNumEntries).c_str()):", piNumEntries=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModPORT::eGCWritePortStacked( GenICam::Client::PORT_HANDLE hPort, GenICam::Client::PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCWritePortStacked)
        m_pFGCWritePortStacked = (GenICam::Client::PGCWritePortStacked)FnExportTest::poGetInstance()->pGCWritePortStacked();
    GENTLTEST_REQUIRE_MESSAGE("GCWritePortStacked not exported", m_pFGCWritePortStacked != NULL);

    GENTLTEST_LOG("<In > GCWritePortStacked(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ((pEntries != NULL)?(std::string(", pEntries->Address=0x")+sConvertUint642HexString(pEntries->Address).c_str()+
                             std::string(", pEntries->pBuffer=0x")+sConvertVoidPointer2String(pEntries->pBuffer).c_str()+
                             std::string(", pEntries->Size=")+sConvertSizeT2String(pEntries->Size).c_str()):", pEntries=NULL") <<
        ((piNumEntries != NULL)?(std::string(", *piNumEntries=")+sConvertSizeT2String(*piNumEntries).c_str()):", piNumEntries=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCWritePortStacked(hPort, pEntries, piNumEntries);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCWritePortStacked");
        throw;
    }

    GENTLTEST_LOG("<Out> GCWritePortStacked(" << 
        "hPort=0x" << sConvertVoidPointer2String(hPort).c_str() <<
        ((pEntries != NULL)?(std::string(", pEntries->Address=0x")+sConvertUint642HexString(pEntries->Address).c_str()+
                             std::string(", pEntries->pBuffer=0x")+sConvertVoidPointer2String(pEntries->pBuffer).c_str()+
                             std::string(", pEntries->Size=")+sConvertSizeT2String(pEntries->Size).c_str()):", pEntries=NULL") <<
        ((piNumEntries != NULL)?(std::string(", *piNumEntries=")+sConvertSizeT2String(*piNumEntries).c_str()):", piNumEntries=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

//////////////////////////////////////////////////////////
// ModEVENT
//////////////////////////////////////////////////////////

GenICam::Client::GC_ERROR ModEVENT::eGCRegisterEvent( GenICam::Client::EVENTSRC_HANDLE hEventSrc, GenICam::Client::EVENT_TYPE iEventID, GenICam::Client::EVENT_HANDLE *phEvent )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCRegisterEvent)
        m_pFGCRegisterEvent = (GenICam::Client::PGCRegisterEvent)FnExportTest::poGetInstance()->pGCRegisterEvent();
    GENTLTEST_REQUIRE_MESSAGE("GCRegisterEvent not exported", m_pFGCRegisterEvent != NULL);

    GENTLTEST_LOG("<In > GCRegisterEvent(" << 
        "hEventSrc=0x" << sConvertVoidPointer2String(hEventSrc).c_str() <<
        ", iEventID=" << sConvertEventID2String((GenICam::Client::EVENT_TYPE_LIST)iEventID).c_str() <<
        ((phEvent != NULL)?(std::string(", *phEvent=0x")+sConvertVoidPointer2String(*phEvent).c_str()):", phEvent=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCRegisterEvent(hEventSrc, iEventID, phEvent);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCRegisterEvent");
        throw;
    }

    GENTLTEST_LOG("<Out> GCRegisterEvent(" << 
        "hEventSrc=0x" << sConvertVoidPointer2String(hEventSrc).c_str() <<
        ", iEventID=" << sConvertEventID2String((GenICam::Client::EVENT_TYPE_LIST)iEventID).c_str() <<
        ((phEvent != NULL)?(std::string(", *phEvent=0x")+sConvertVoidPointer2String(*phEvent).c_str()):", phEvent=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModEVENT::eGCUnregisterEvent( GenICam::Client::EVENTSRC_HANDLE hEventSrc, GenICam::Client::EVENT_TYPE iEventID )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFGCUnregisterEvent)
        m_pFGCUnregisterEvent = (GenICam::Client::PGCUnregisterEvent)FnExportTest::poGetInstance()->pGCUnregisterEvent();
    GENTLTEST_REQUIRE_MESSAGE("GCUnregisterEvent not exported", m_pFGCUnregisterEvent != NULL);

    GENTLTEST_LOG("<In > GCUnregisterEvent(" << 
        "hEventSrc=0x" << sConvertVoidPointer2String(hEventSrc).c_str() <<
        ", iEventID=" << sConvertEventID2String((GenICam::Client::EVENT_TYPE_LIST)iEventID).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFGCUnregisterEvent(hEventSrc, iEventID);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("GCUnregisterEvent");
        throw;
    }

    GENTLTEST_LOG("<Out> GCUnregisterEvent(" << 
        "hEventSrc=0x" << sConvertVoidPointer2String(hEventSrc).c_str() <<
        ", iEventID=" << sConvertEventID2String((GenICam::Client::EVENT_TYPE_LIST)iEventID).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModEVENT::eEventGetData( GenICam::Client::EVENT_HANDLE hEvent, void *pBuffer, size_t *piSize, uint64_t iTimeout )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFEventGetData)
        m_pFEventGetData = (GenICam::Client::PEventGetData)FnExportTest::poGetInstance()->pEventGetData();
    GENTLTEST_REQUIRE_MESSAGE("GEventGetData not exported", m_pFEventGetData != NULL);

    GENTLTEST_LOG("<In > EventGetData(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ", iTimeout=" << sConvertUint642String(iTimeout).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFEventGetData(hEvent, pBuffer, piSize, iTimeout);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("EventGetData");
        throw;
    }

    GENTLTEST_LOG("<Out> EventGetData(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ", iTimeout=" << sConvertUint642String(iTimeout).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModEVENT::eEventGetDataInfo( GenICam::Client::EVENT_HANDLE hEvent, const void *pInBuffer, size_t iInSize, GenICam::Client::EVENT_DATA_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pOutBuffer, size_t *piOutSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFEventGetDataInfo)
        m_pFEventGetDataInfo = (GenICam::Client::PEventGetDataInfo)FnExportTest::poGetInstance()->pEventGetDataInfo();
    GENTLTEST_REQUIRE_MESSAGE("EventGetDataInfo not exported", m_pFEventGetDataInfo != NULL);

    GENTLTEST_LOG("<In > EventGetDataInfo(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ", pInBuffer=0x" << sConvertVoidPointer2String((void*)pInBuffer).c_str() <<
        ", iInSize=" << iInSize <<
        ", iInfoCmd=" << sConvertEventDataInfoCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pOutBuffer=0x" << sConvertVoidPointer2String(pOutBuffer).c_str() <<
        ((piOutSize != NULL)?(std::string(", *piOutSize=")+sConvertSizeT2String(*piOutSize).c_str()):", piOutSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFEventGetDataInfo(hEvent, pInBuffer, iInSize, iInfoCmd, piType, pOutBuffer, piOutSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("EventGetDataInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> EventGetDataInfo(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ", pInBuffer=0x" << sConvertVoidPointer2String((void*)pInBuffer).c_str() <<
        ", iInSize=" << iInSize <<
        ", iInfoCmd=" << sConvertEventDataInfoCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pOutBuffer=0x" << sConvertVoidPointer2String(pOutBuffer).c_str() <<
        ((piOutSize != NULL)?(std::string(", *piOutSize=")+sConvertSizeT2String(*piOutSize).c_str()):", piOutSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModEVENT::eEventGetInfo( GenICam::Client::EVENT_HANDLE hEvent, GenICam::Client::EVENT_INFO_CMD iInfoCmd, GenICam::Client::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFEventGetInfo)
        m_pFEventGetInfo = (GenICam::Client::PEventGetInfo)FnExportTest::poGetInstance()->pEventGetInfo();
    GENTLTEST_REQUIRE_MESSAGE("EventGetInfo not exported", m_pFEventGetInfo != NULL);

    GENTLTEST_LOG("<In > EventGetInfo(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ", iInfoCmd=" << sConvertEventInfoCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFEventGetInfo(hEvent, iInfoCmd, piType, pBuffer, piSize);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("EventGetInfo");
        throw;
    }

    GENTLTEST_LOG("<Out> EventGetInfo(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ", iInfoCmd=" << sConvertEventInfoCommand2String(iInfoCmd).c_str() << 
        ((piType != NULL)?(std::string(", *piType=")+sConvertDataType2String(*piType).c_str()):", piType=NULL") <<
        ", pBuffer=0x" << sConvertVoidPointer2String(pBuffer).c_str() <<
        ((piSize != NULL)?(std::string(", *piSize=")+sConvertSizeT2String(*piSize).c_str()):", piSize=NULL") <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModEVENT::eEventFlush( GenICam::Client::EVENT_HANDLE hEvent )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFEventFlush)
        m_pFEventFlush = (GenICam::Client::PEventFlush)FnExportTest::poGetInstance()->pEventFlush();
    GENTLTEST_REQUIRE_MESSAGE("EventFlush not exported", m_pFEventFlush != NULL);

    GENTLTEST_LOG("<In > EventFlush(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFEventFlush(hEvent);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("EventFlush");
        throw;
    }

    GENTLTEST_LOG("<Out> EventFlush(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

GenICam::Client::GC_ERROR ModEVENT::eEventKill( GenICam::Client::EVENT_HANDLE hEvent )
{
    GenICam::Client::GC_ERROR eResult=GenICam::Client::GC_ERR_SUCCESS;

    if (NULL == m_pFEventKill)
        m_pFEventKill = (GenICam::Client::PEventKill)FnExportTest::poGetInstance()->pEventKill();
    GENTLTEST_REQUIRE_MESSAGE("EventKill not exported", m_pFEventKill != NULL);

    GENTLTEST_LOG("<In > EventKill(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ")" << std::endl);

    try 
    {
        eResult = m_pFEventKill(hEvent);
    }
    catch(...)
    {
        GENTLTEST_EXCEPTION_MESSAGE("EventKill");
        throw;
    }

    GENTLTEST_LOG("<Out> EventKill(" << 
        "hEvent=0x" << sConvertVoidPointer2String(hEvent).c_str() <<
        ") result=" << sConvertGCError2String(eResult).c_str() << std::endl);

    return eResult;
}

