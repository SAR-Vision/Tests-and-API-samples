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

#include "GenTLTest_Win.h"

#include "GenTLTestTools.h"

//////////////////////////////////////////////////////////////////////////////////////
// display last error
//////////////////////////////////////////////////////////////////////////////////////

void vDisplayError(std::string csFunction) 
{ 
    std::stringstream oMsg;
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, 
        NULL);

    oMsg << csFunction.c_str() << " failed with error " << dw << ": " << (const char*)((lpMsgBuf!=NULL)?lpMsgBuf:"---");

    GENTLTEST_PRINT(oMsg.str().c_str() << std::endl);

    LocalFree(lpMsgBuf);
}

////////////////////////////////////////////////////////////////////////////////////////
// converter
////////////////////////////////////////////////////////////////////////////////////////

std::string sConvertGCError2String(GenICam::Client::GC_ERROR nResult)
{
    std::string sResult="<unknown GC_ERROR>";

    switch(nResult)
    { 
    case GenICam::Client::GC_ERR_SUCCESS: sResult = "GC_ERR_SUCCESS"; break;
    case GenICam::Client::GC_ERR_ERROR: sResult = "GC_ERR_ERROR"; break;
    case GenICam::Client::GC_ERR_NOT_INITIALIZED: sResult = "GC_ERR_NOT_INITIALIZED"; break;
    case GenICam::Client::GC_ERR_NOT_IMPLEMENTED: sResult = "GC_ERR_NOT_IMPLEMENTED"; break;
    case GenICam::Client::GC_ERR_RESOURCE_IN_USE: sResult = "GC_ERR_RESOURCE_IN_USE"; break;
    case GenICam::Client::GC_ERR_ACCESS_DENIED: sResult = "GC_ERR_ACCESS_DENIED"; break;
    case GenICam::Client::GC_ERR_INVALID_HANDLE: sResult = "GC_ERR_INVALID_HANDLE"; break;
    case GenICam::Client::GC_ERR_INVALID_ID: sResult = "GC_ERR_INVALID_ID"; break;
    case GenICam::Client::GC_ERR_NO_DATA: sResult = "GC_ERR_NO_DATA"; break;
    case GenICam::Client::GC_ERR_INVALID_PARAMETER: sResult = "GC_ERR_INVALID_PARAMETER"; break;
    case GenICam::Client::GC_ERR_IO: sResult = "GC_ERR_IO"; break;
    case GenICam::Client::GC_ERR_TIMEOUT: sResult = "GC_ERR_TIMEOUT"; break;
    case GenICam::Client::GC_ERR_ABORT: sResult = "GC_ERR_ABORT"; break;
    case GenICam::Client::GC_ERR_INVALID_BUFFER: sResult = "GC_ERR_INVALID_BUFFER"; break;
    case GenICam::Client::GC_ERR_NOT_AVAILABLE: sResult = "GC_ERR_NOT_AVAILABLE"; break;
    case GenICam::Client::GC_ERR_INVALID_ADDRESS: sResult = "GC_ERR_INVALID_ADDRESS"; break;
    case GenICam::Client::GC_ERR_BUFFER_TOO_SMALL: sResult = "GC_ERR_BUFFER_TOO_SMALL"; break;
    case GenICam::Client::GC_ERR_INVALID_INDEX: sResult = "GC_ERR_INVALID_INDEX"; break;
    case GenICam::Client::GC_ERR_PARSING_CHUNK_DATA: sResult = "GC_ERR_PARSING_CHUNK_DATA"; break;
    case GenICam::Client::GC_ERR_INVALID_VALUE: sResult = "GC_ERR_INVALID_VALUE"; break;
    case GenICam::Client::GC_ERR_RESOURCE_EXHAUSTED: sResult = "GC_ERR_RESOURCE_EXHAUSTED"; break;
    case GenICam::Client::GC_ERR_OUT_OF_MEMORY: sResult = "GC_ERR_OUT_OF_MEMORY"; break;
    case GenICam::Client::GC_ERR_CUSTOM_ID: sResult = "GC_ERR_CUSTOM_ID"; break;
    }

    return sResult.c_str();
}

std::string sConvertTLInfoCommand2String(GenICam::Client::TL_INFO_CMD eCommand)
{
    std::string sResult="<unknown command";

    switch(eCommand)
    {
    case GenICam::Client::TL_INFO_ID: sResult = "TL_INFO_ID"; break;
    case GenICam::Client::TL_INFO_VENDOR: sResult = "TL_INFO_VENDOR"; break;
    case GenICam::Client::TL_INFO_MODEL: sResult = "TL_INFO_MODEL"; break;
    case GenICam::Client::TL_INFO_VERSION: sResult = "TL_INFO_VERSION"; break;
    case GenICam::Client::TL_INFO_TLTYPE: sResult = "TL_INFO_TLTYPE"; break;
    case GenICam::Client::TL_INFO_NAME: sResult = "TL_INFO_NAME"; break;
    case GenICam::Client::TL_INFO_PATHNAME: sResult = "TL_INFO_PATHNAME"; break;
    case GenICam::Client::TL_INFO_DISPLAYNAME: sResult = "TL_INFO_DISPLAYNAME"; break;
    case GenICam::Client::TL_INFO_CHAR_ENCODING: sResult = "TL_INFO_CHAR_ENCODING"; break;
    case GenICam::Client::TL_INFO_CUSTOM_ID: sResult = "TL_INFO_CUSTOM_ID"; break;
    }

    return sResult.c_str();
}

std::string sConvertPORTCommand2String(GenICam::Client::PORT_INFO_CMD_LIST eCommand)
{
    std::string sResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::PORT_INFO_ID: sResult = "PORT_INFO_ID"; break;
    case GenICam::Client::PORT_INFO_VENDOR: sResult = "PORT_INFO_VENDOR"; break;
    case GenICam::Client::PORT_INFO_MODEL: sResult = "PORT_INFO_MODEL"; break;
    case GenICam::Client::PORT_INFO_TLTYPE: sResult = "PORT_INFO_TLTYPE"; break;
    case GenICam::Client::PORT_INFO_MODULE: sResult = "PORT_INFO_MODULE"; break;
    case GenICam::Client::PORT_INFO_LITTLE_ENDIAN: sResult = "PORT_INFO_LITTLE_ENDIAN"; break;
    case GenICam::Client::PORT_INFO_BIG_ENDIAN: sResult = "PORT_INFO_BIG_ENDIAN"; break;
    case GenICam::Client::PORT_INFO_ACCESS_READ: sResult = "PORT_INFO_ACCESS_READ"; break;
    case GenICam::Client::PORT_INFO_ACCESS_WRITE: sResult = "PORT_INFO_ACCESS_WRITE"; break;
    case GenICam::Client::PORT_INFO_ACCESS_NA: sResult = "PORT_INFO_ACCESS_NA"; break;
    case GenICam::Client::PORT_INFO_ACCESS_NI: sResult = "PORT_INFO_ACCESS_NI"; break;
    case GenICam::Client::PORT_INFO_VERSION: sResult = "PORT_INFO_VERSION"; break;
    case GenICam::Client::PORT_INFO_PORTNAME: sResult = "PORT_INFO_PORTNAME"; break;
    }

    return sResult.c_str();
}

std::string sConvertDataType2String(GenICam::Client::INFO_DATATYPE eDataType)
{
    std::string sResult="<unknown datatype>";

    switch(eDataType)
    {
    case GenICam::Client::INFO_DATATYPE_UNKNOWN: sResult = "INFO_DATATYPE_UNKNOWN"; break;
    case GenICam::Client::INFO_DATATYPE_STRING: sResult = "INFO_DATATYPE_STRING"; break;
    case GenICam::Client::INFO_DATATYPE_STRINGLIST: sResult = "INFO_DATATYPE_STRINGLIST"; break;
    case GenICam::Client::INFO_DATATYPE_INT16: sResult = "INFO_DATATYPE_INT16"; break;
    case GenICam::Client::INFO_DATATYPE_UINT16: sResult = "INFO_DATATYPE_UINT16"; break;
    case GenICam::Client::INFO_DATATYPE_INT32: sResult = "INFO_DATATYPE_INT32"; break;
    case GenICam::Client::INFO_DATATYPE_UINT32: sResult = "INFO_DATATYPE_UINT32"; break;
    case GenICam::Client::INFO_DATATYPE_INT64: sResult = "INFO_DATATYPE_INT64"; break;
    case GenICam::Client::INFO_DATATYPE_UINT64: sResult = "INFO_DATATYPE_UINT64"; break;
    case GenICam::Client::INFO_DATATYPE_FLOAT64: sResult = "INFO_DATATYPE_FLOAT64"; break;
    case GenICam::Client::INFO_DATATYPE_PTR: sResult = "INFO_DATATYPE_PTR"; break;
    case GenICam::Client::INFO_DATATYPE_BOOL8: sResult = "INFO_DATATYPE_BOOL8"; break;
    case GenICam::Client::INFO_DATATYPE_SIZET: sResult = "INFO_DATATYPE_SIZET"; break;
    case GenICam::Client::INFO_DATATYPE_BUFFER: sResult = "INFO_DATATYPE_BUFFER"; break;
    case GenICam::Client::INFO_DATATYPE_PTRDIFF: sResult = "INFO_DATATYPE_PTRDIFF"; break;
    }

    return sResult.c_str();
}

std::string sConvertDeviceCommand2String(GenICam::Client::DEVICE_INFO_CMD_LIST eCommand)
{
    std::string sResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::DEVICE_INFO_ID: sResult = "DEVICE_INFO_ID"; break;
    case GenICam::Client::DEVICE_INFO_VENDOR: sResult = "DEVICE_INFO_VENDOR"; break;
    case GenICam::Client::DEVICE_INFO_MODEL: sResult = "DEVICE_INFO_MODEL"; break;
    case GenICam::Client::DEVICE_INFO_TLTYPE: sResult = "DEVICE_INFO_TLTYPE"; break;
    case GenICam::Client::DEVICE_INFO_DISPLAYNAME: sResult = "DEVICE_INFO_DISPLAYNAME"; break;
    case GenICam::Client::DEVICE_INFO_ACCESS_STATUS: sResult = "DEVICE_INFO_ACCESS_STATUS"; break;
    case GenICam::Client::DEVICE_INFO_USER_DEFINED_NAME: sResult = "DEVICE_INFO_USER_DEFINED_NAME"; break;
    case GenICam::Client::DEVICE_INFO_SERIAL_NUMBER: sResult = "DEVICE_INFO_SERIAL_NUMBER"; break;
    case GenICam::Client::DEVICE_INFO_VERSION: sResult = "DEVICE_INFO_VERSION"; break;
    case GenICam::Client::DEVICE_INFO_TIMESTAMP_FREQUENCY: sResult = "DEVICE_INFO_TIMESTAMP_FREQUENCY"; break;
    }

    return sResult.c_str();
}

std::string sConvertDEVICEAccess2String(GenICam::Client::DEVICE_ACCESS_FLAGS_LIST eAccess)
{
    std::string sResult="<unknown device access>";

    switch(eAccess)
    {
    case GenICam::Client::DEVICE_ACCESS_UNKNOWN: sResult = "DEVICE_ACCESS_UNKNOWN"; break;
    case GenICam::Client::DEVICE_ACCESS_NONE: sResult = "DEVICE_ACCESS_NONE"; break;
    case GenICam::Client::DEVICE_ACCESS_READONLY: sResult = "DEVICE_ACCESS_READONLY"; break;
    case GenICam::Client::DEVICE_ACCESS_CONTROL: sResult = "DEVICE_ACCESS_CONTROL"; break;
    case GenICam::Client::DEVICE_ACCESS_EXCLUSIVE: sResult = "DEVICE_ACCESS_EXCLUSIVE"; break;
    }

    return sResult.c_str();
}

std::string sConvertURLInfoCommand2String(GenICam::Client::URL_INFO_CMD_LIST eCommand)
{
    std::string sResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::URL_INFO_URL: sResult = "URL_INFO_URL"; break;
    case GenICam::Client::URL_INFO_SCHEMA_VER_MAJOR: sResult = "URL_INFO_SCHEMA_VER_MAJOR"; break;
    case GenICam::Client::URL_INFO_SCHEMA_VER_MINOR: sResult = "URL_INFO_SCHEMA_VER_MINOR"; break;
    case GenICam::Client::URL_INFO_FILE_VER_MAJOR: sResult = "URL_INFO_FILE_VER_MAJOR"; break;
    case GenICam::Client::URL_INFO_FILE_VER_MINOR: sResult = "URL_INFO_FILE_VER_MINOR"; break;
    case GenICam::Client::URL_INFO_FILE_VER_SUBMINOR: sResult = "URL_INFO_FILE_VER_SUBMINOR"; break;
    case GenICam::Client::URL_INFO_FILE_SHA1_HASH: sResult = "URL_INFO_FILE_SHA1_HASH"; break;
    }

    return sResult.c_str();
}

std::string sConvertInterfaceCommand2String(GenICam::Client::INTERFACE_INFO_CMD eCommand)
{
    std::string oResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::INTERFACE_INFO_ID: oResult = "INTERFACE_INFO_ID"; break;
    case GenICam::Client::INTERFACE_INFO_DISPLAYNAME: oResult = "INTERFACE_INFO_DISPLAYNAME"; break;
    case GenICam::Client::INTERFACE_INFO_TLTYPE: oResult = "INTERFACE_INFO_TLTYPE"; break;
    }

    return oResult.c_str();
}

std::string sConvertDataStreamCommand2String(GenICam::Client::STREAM_INFO_CMD eCommand)
{
    std::string oResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::STREAM_INFO_ID: oResult = "STREAM_INFO_ID"; break;
    case GenICam::Client::STREAM_INFO_NUM_DELIVERED: oResult = "STREAM_INFO_NUM_DELIVERED"; break;
    case GenICam::Client::STREAM_INFO_NUM_UNDERRUN: oResult = "STREAM_INFO_NUM_UNDERRUN"; break;
    case GenICam::Client::STREAM_INFO_NUM_ANNOUNCED: oResult = "STREAM_INFO_NUM_ANNOUNCED"; break;
    case GenICam::Client::STREAM_INFO_NUM_QUEUED: oResult = "STREAM_INFO_NUM_QUEUED"; break;
    case GenICam::Client::STREAM_INFO_NUM_AWAIT_DELIVERY: oResult = "STREAM_INFO_NUM_AWAIT_DELIVERY"; break;
    case GenICam::Client::STREAM_INFO_NUM_STARTED: oResult = "STREAM_INFO_NUM_STARTED"; break;
    case GenICam::Client::STREAM_INFO_PAYLOAD_SIZE: oResult = "STREAM_INFO_PAYLOAD_SIZE"; break;
    case GenICam::Client::STREAM_INFO_IS_GRABBING: oResult = "STREAM_INFO_IS_GRABBING"; break;
    case GenICam::Client::STREAM_INFO_DEFINES_PAYLOADSIZE: oResult = "STREAM_INFO_DEFINES_PAYLOADSIZE"; break;
    case GenICam::Client::STREAM_INFO_TLTYPE: oResult = "STREAM_INFO_TLTYPE"; break;
    case GenICam::Client::STREAM_INFO_NUM_CHUNKS_MAX: oResult = "STREAM_INFO_NUM_CHUNKS_MAX"; break;
    case GenICam::Client::STREAM_INFO_BUF_ANNOUNCE_MIN: oResult = "STREAM_INFO_BUF_ANNOUNCE_MIN"; break;
    case GenICam::Client::STREAM_INFO_BUF_ALIGNMENT: oResult = "STREAM_INFO_BUF_ALIGNMENT"; break;
    }

    return oResult.c_str();
}

std::string sConvertDataStreamBufferCommand2String(GenICam::Client::BUFFER_INFO_CMD eCommand)
{
    std::string oResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::BUFFER_INFO_BASE: oResult = "BUFFER_INFO_BASE"; break;
    case GenICam::Client::BUFFER_INFO_SIZE: oResult = "BUFFER_INFO_SIZE"; break;
    case GenICam::Client::BUFFER_INFO_USER_PTR: oResult = "BUFFER_INFO_USER_PTR"; break;
    case GenICam::Client::BUFFER_INFO_TIMESTAMP: oResult = "BUFFER_INFO_TIMESTAMP"; break;
    case GenICam::Client::BUFFER_INFO_NEW_DATA: oResult = "BUFFER_INFO_NEW_DATA"; break;
    case GenICam::Client::BUFFER_INFO_IS_QUEUED: oResult = "BUFFER_INFO_IS_QUEUED"; break;
    case GenICam::Client::BUFFER_INFO_IS_ACQUIRING: oResult = "BUFFER_INFO_IS_ACQUIRING"; break;
    case GenICam::Client::BUFFER_INFO_IS_INCOMPLETE: oResult = "BUFFER_INFO_IS_INCOMPLETE"; break;
    case GenICam::Client::BUFFER_INFO_TLTYPE: oResult = "BUFFER_INFO_TLTYPE"; break;
    case GenICam::Client::BUFFER_INFO_SIZE_FILLED: oResult = "BUFFER_INFO_SIZE_FILLED"; break;
    case GenICam::Client::BUFFER_INFO_WIDTH: oResult = "BUFFER_INFO_WIDTH"; break;
    case GenICam::Client::BUFFER_INFO_HEIGHT: oResult = "BUFFER_INFO_HEIGHT"; break;
    case GenICam::Client::BUFFER_INFO_XOFFSET: oResult = "BUFFER_INFO_XOFFSET"; break;
    case GenICam::Client::BUFFER_INFO_YOFFSET: oResult = "BUFFER_INFO_YOFFSET"; break;
    case GenICam::Client::BUFFER_INFO_XPADDING: oResult = "BUFFER_INFO_XPADDING"; break;
    case GenICam::Client::BUFFER_INFO_YPADDING: oResult = "BUFFER_INFO_YPADDING"; break;
    case GenICam::Client::BUFFER_INFO_FRAMEID: oResult = "BUFFER_INFO_FRAMEID"; break;
    case GenICam::Client::BUFFER_INFO_IMAGEPRESENT: oResult = "BUFFER_INFO_IMAGEPRESENT"; break;
    case GenICam::Client::BUFFER_INFO_IMAGEOFFSET: oResult = "BUFFER_INFO_IMAGEOFFSET"; break;
    case GenICam::Client::BUFFER_INFO_PAYLOADTYPE: oResult = "BUFFER_INFO_PAYLOADTYPE"; break;
    case GenICam::Client::BUFFER_INFO_PIXELFORMAT: oResult = "BUFFER_INFO_PIXELFORMAT"; break;
    case GenICam::Client::BUFFER_INFO_PIXELFORMAT_NAMESPACE: oResult = "BUFFER_INFO_PIXELFORMAT_NAMESPACE"; break;
    case GenICam::Client::BUFFER_INFO_DELIVERED_IMAGEHEIGHT: oResult = "BUFFER_INFO_DELIVERED_IMAGEHEIGHT"; break;
    case GenICam::Client::BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE: oResult = "BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE"; break;
    case GenICam::Client::BUFFER_INFO_CHUNKLAYOUTID: oResult = "BUFFER_INFO_CHUNKLAYOUTID"; break;
    case GenICam::Client::BUFFER_INFO_FILENAME: oResult = "BUFFER_INFO_FILENAME"; break;
    case GenICam::Client::BUFFER_INFO_PIXEL_ENDIANNESS: oResult = "BUFFER_INFO_PIXEL_ENDIANNESS"; break;
    case GenICam::Client::BUFFER_INFO_DATA_SIZE: oResult = "BUFFER_INFO_DATA_SIZE"; break;
    case GenICam::Client::BUFFER_INFO_TIMESTAMP_NS: oResult = "BUFFER_INFO_TIMESTAMP_NS"; break;
    case GenICam::Client::BUFFER_INFO_DATA_LARGER_THAN_BUFFER: oResult = "BUFFER_INFO_DATA_LARGER_THAN_BUFFER"; break;
    case GenICam::Client::BUFFER_INFO_CONTAINS_CHUNKDATA: oResult = "BUFFER_INFO_CONTAINS_CHUNKDATA"; break;
    }

    return oResult.c_str();
}

std::string sConvertEndianness2String(int32_t endianness)
{
    std::string oResult="<unknown endianness>";

    switch(endianness)
    {
    case GenICam::Client::PIXELENDIANNESS_UNKNOWN: oResult = "PIXELENDIANNESS_UNKNOWN"; break;
    case GenICam::Client::PIXELENDIANNESS_LITTLE: oResult = "PIXELENDIANNESS_LITTLE"; break;
    case GenICam::Client::PIXELENDIANNESS_BIG: oResult = "PIXELENDIANNESS_BIG"; break;
    }

    return oResult.c_str();
}

std::string sConvertDataStreamStartFlag2String(GenICam::Client::ACQ_START_FLAGS eStartFlag)
{
    std::string oResult="<unknown startflag>";

    switch(eStartFlag)
    {
    case GenICam::Client::ACQ_START_FLAGS_DEFAULT: oResult = "ACQ_START_FLAGS_DEFAULT"; break;
    }

    return oResult.c_str();
}

std::string sConvertDataStreamStopFlag2String(GenICam::Client::ACQ_STOP_FLAGS eStopFlag)
{
    std::string oResult="<unknown stopflag>";

    switch(eStopFlag)
    {
    case GenICam::Client::ACQ_STOP_FLAGS_DEFAULT: oResult = "ACQ_STOP_FLAGS_DEFAULT"; break;
    case GenICam::Client::ACQ_STOP_FLAGS_KILL: oResult = "ACQ_STOP_FLAGS_KILL"; break;
    }

    return oResult.c_str();
}

std::string sConvertDataStreamOperation2String(GenICam::Client::ACQ_QUEUE_TYPE eOperation)
{
    std::string oResult="<unknown operation>";

    switch(eOperation)
    {
    case GenICam::Client::ACQ_QUEUE_INPUT_TO_OUTPUT: oResult = "ACQ_QUEUE_INPUT_TO_OUTPUT"; break;
    case GenICam::Client::ACQ_QUEUE_OUTPUT_DISCARD: oResult = "ACQ_QUEUE_OUTPUT_DISCARD"; break;
    case GenICam::Client::ACQ_QUEUE_ALL_TO_INPUT: oResult = "ACQ_QUEUE_ALL_TO_INPUT"; break;
    case GenICam::Client::ACQ_QUEUE_UNQUEUED_TO_INPUT: oResult = "ACQ_QUEUE_UNQUEUED_TO_INPUT"; break;
    case GenICam::Client::ACQ_QUEUE_ALL_DISCARD: oResult = "ACQ_QUEUE_ALL_DISCARD"; break;
    }

    return oResult.c_str();
}

std::string sConvertEventID2String(GenICam::Client::EVENT_TYPE_LIST eEventID)
{
    std::string oResult="<unknown event ID>";

    switch(eEventID)
    {
    case GenICam::Client::EVENT_ERROR: oResult = "EVENT_ERROR"; break;
    case GenICam::Client::EVENT_NEW_BUFFER: oResult = "EVENT_NEW_BUFFER"; break;
    case GenICam::Client::EVENT_FEATURE_INVALIDATE: oResult = "EVENT_FEATURE_INVALIDATE"; break;
    case GenICam::Client::EVENT_FEATURE_CHANGE: oResult = "EVENT_FEATURE_CHANGE"; break;
    case GenICam::Client::EVENT_REMOTE_DEVICE: oResult = "EVENT_REMOTE_DEVICE"; break;
    case GenICam::Client::EVENT_MODULE: oResult = "EVENT_MODULE"; break;
    }

    return oResult.c_str();
}

std::string sConvertEventInfoCommand2String(GenICam::Client::EVENT_INFO_CMD eCommand)
{
    std::string oResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::EVENT_EVENT_TYPE: oResult = "EVENT_EVENT_TYPE"; break;
    case GenICam::Client::EVENT_NUM_IN_QUEUE: oResult = "EVENT_NUM_IN_QUEUE"; break;
    case GenICam::Client::EVENT_NUM_FIRED: oResult = "EVENT_NUM_FIRED"; break;
    case GenICam::Client::EVENT_SIZE_MAX: oResult = "EVENT_SIZE_MAX"; break;
    case GenICam::Client::EVENT_INFO_DATA_SIZE_MAX: oResult = "EVENT_INFO_DATA_SIZE_MAX"; break;
    }

    return oResult.c_str();
}

std::string sConvertEventDataInfoCommand2String(GenICam::Client::EVENT_DATA_INFO_CMD eCommand)
{
    std::string oResult="<unknown command>";

    switch(eCommand)
    {
    case GenICam::Client::EVENT_DATA_ID: oResult = "EVENT_DATA_ID"; break;
    case GenICam::Client::EVENT_DATA_VALUE: oResult = "EVENT_DATA_VALUE"; break;
    case GenICam::Client::EVENT_DATA_NUMID: oResult = "EVENT_DATA_NUMID"; break;
    }

    return oResult.c_str();
}

std::string sConvertAccessStatus2String(GenICam::Client::DEVICE_ACCESS_STATUS eAccess)
{
    std::string oResult="<unknown access>";

    switch(eAccess)
    {
    case GenICam::Client::DEVICE_ACCESS_STATUS_UNKNOWN: oResult = "DEVICE_ACCESS_STATUS_UNKNOWN"; break;
    case GenICam::Client::DEVICE_ACCESS_STATUS_READWRITE: oResult = "DEVICE_ACCESS_STATUS_READWRITE"; break;
    case GenICam::Client::DEVICE_ACCESS_STATUS_READONLY: oResult = "DEVICE_ACCESS_STATUS_READONLY"; break;
    case GenICam::Client::DEVICE_ACCESS_STATUS_NOACCESS: oResult = "DEVICE_ACCESS_STATUS_NOACCESS"; break;
    }

    return oResult.c_str();
}

std::string sConvertPayloadtype2String(GenICam::Client::PAYLOADTYPE_INFO_IDS ePayloadtype)
{
    std::string oResult="<unknown payloadtype>";

    switch(ePayloadtype)
    {
    case GenICam::Client::PAYLOAD_TYPE_UNKNOWN: oResult = "PAYLOAD_TYPE_UNKNOWN"; break;
    case GenICam::Client::PAYLOAD_TYPE_IMAGE: oResult = "PAYLOAD_TYPE_IMAGE"; break;
    case GenICam::Client::PAYLOAD_TYPE_RAW_DATA: oResult = "PAYLOAD_TYPE_RAW_DATA"; break;
    case GenICam::Client::PAYLOAD_TYPE_FILE: oResult = "PAYLOAD_TYPE_FILE"; break;
    case GenICam::Client::PAYLOAD_TYPE_CHUNK_DATA: oResult = "PAYLOAD_TYPE_CHUNK_DATA"; break;
    case GenICam::Client::PAYLOAD_TYPE_JPEG: oResult = "PAYLOAD_TYPE_JPEG"; break;
    case GenICam::Client::PAYLOAD_TYPE_JPEG2000: oResult = "PAYLOAD_TYPE_JPEG2000"; break;
    case GenICam::Client::PAYLOAD_TYPE_H264: oResult = "PAYLOAD_TYPE_H264"; break;
    case GenICam::Client::PAYLOAD_TYPE_CHUNK_ONLY: oResult = "PAYLOAD_TYPE_CHUNK_ONLY"; break;
    case GenICam::Client::PAYLOAD_TYPE_DEVICE_SPECIFIC: oResult = "PAYLOAD_TYPE_DEVICE_SPECIFIC"; break;
    }

    return oResult.c_str();
}

int16_t xConvertBuffer2INT16(const char *buffer)
{
    int32_t xResult=0;

    xResult = xConvertBuffer2UINT16(buffer);

    return xResult;
}

uint16_t xConvertBuffer2UINT16(const char *buffer)
{
    int32_t xResult=0;

    xResult |= buffer[1] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[0] & 0xFF;

    return xResult;
}

int32_t xConvertBuffer2INT32(const char *buffer)
{
    int32_t xResult=0;

    xResult = xConvertBuffer2UINT32(buffer);

    return xResult;
}

uint32_t xConvertBuffer2UINT32(const char *buffer)
{
    uint32_t xResult=0;

    xResult |= buffer[3] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[2] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[1] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[0] & 0xFF;

    return xResult;
}

double xConvertBuffer2FLOAT64(const char *buffer)
{
    double xResult=0;

    xResult = (double)xConvertBuffer2UINT64(buffer);

    return xResult;
}

int64_t xConvertBuffer2INT64(const char *buffer)
{
    uint64_t xResult=0;

    xResult = xConvertBuffer2UINT64(buffer);

    return xResult;
}

uint64_t xConvertBuffer2UINT64(const char *buffer)
{
    uint64_t xResult=0;

    xResult |= buffer[7] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[6] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[5] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[4] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[3] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[2] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[1] & 0xFF;
    xResult <<= 8;
    xResult |= buffer[0] & 0xFF;

    return xResult;
}

uint64_t xConvertBuffer2PTR(const char *buffer, const std::size_t iSize)
{
    return xConvertBuffer2SIZET(buffer, iSize);
}

size_t xConvertBuffer2SIZET(const char *buffer, const std::size_t iSize)
{
    size_t xResult;

    if (iSize >= 8)
        xResult = (size_t)xConvertBuffer2UINT64(buffer);
    else
        xResult = (size_t)xConvertBuffer2UINT32(buffer);

    return xResult;
}

// SVW used in macro modules.cpp
std::string sConvertSizeT2String(size_t value)
{
    std::stringstream oTemp;

    oTemp << value;

    return oTemp.str();
}

// SVW used in macro modules.cpp
std::string sConvertBool82String(bool8_t value)
{
    std::stringstream oTemp;

    oTemp << value;

    return oTemp.str();
}

// SVW used in macro modules.cpp
std::string sConvertUint322String(uint32_t value)
{
    std::stringstream oTemp;

    oTemp << value;

    return oTemp.str();
}

// SVW used in macro modules.cpp
std::string sConvertUint642String(uint64_t value)
{
    std::stringstream oTemp;

    oTemp << value;

    return oTemp.str();
}

// SVW used in macro modules.cpp
std::string sConvertUint642HexString(uint64_t value)
{
    std::stringstream oTemp;

    oTemp << std::hex << value << std::dec;

    return oTemp.str();
}

// SVW used in macro modules.cpp
std::string sConvertVoidPointer2String(void *value)
{
    std::stringstream oTemp;

    oTemp << std::hex << value << std::dec;

    return oTemp.str();
}

std::string csConvertINTHexToDec(std::string sValue)
{
    std::stringstream csResult;
    uint64_t nResult=0; 

    if (sValue.find("0x", 0) == 0)
    {
        std::stringstream ss; 
        ss << std::hex << sValue.substr(2); 
        ss >> nResult;
        
        csResult << nResult;
    }
    else
    {
        csResult << sValue;
    }

    return csResult.str();
}

////////////////////////////////////////////////////////////////////////////////////////
// testing
////////////////////////////////////////////////////////////////////////////////////////

void vTestPixelFormat(std::string sPixelFormat)
{
    if (sPixelFormat == "Mono8" ||
        sPixelFormat == "Mono8Signed" ||
        sPixelFormat == "Mono10" ||
        sPixelFormat == "Mono10Packed" ||
        sPixelFormat == "Mono12" ||
        sPixelFormat == "Mono12Packed" ||
        sPixelFormat == "Mono16" ||
        sPixelFormat == "BayerGR8" ||
        sPixelFormat == "BayerRG8" ||
        sPixelFormat == "BayerGB8" ||
        sPixelFormat == "BayerBG8" ||
        sPixelFormat == "BayerGR10" ||
        sPixelFormat == "BayerRG10" ||
        sPixelFormat == "BayerGB10" ||
        sPixelFormat == "BayerBG10" ||
        sPixelFormat == "BayerGR12" ||
        sPixelFormat == "BayerRG12" ||
        sPixelFormat == "BayerGB12" ||
        sPixelFormat == "BayerBG12" ||
        sPixelFormat == "RGB8Packed" ||
        sPixelFormat == "BGR8Packed" ||
        sPixelFormat == "RGBA8Packed" ||
        sPixelFormat == "BGRA8Packed" ||
        sPixelFormat == "RGB10Packed" ||
        sPixelFormat == "BGR10Packed" ||
        sPixelFormat == "RGB12Packed" ||
        sPixelFormat == "BGR12Packed" ||
        sPixelFormat == "RGB10V1Packed" ||
        sPixelFormat == "RGB10V2Packed" ||
        sPixelFormat == "YUV411Packed" ||
        sPixelFormat == "YUV422Packed" ||
        sPixelFormat == "YUV444Packed" ||
        sPixelFormat == "RGB8Planar" ||
        sPixelFormat == "RGB10Planar" ||
        sPixelFormat == "RGB12Planar" ||
        sPixelFormat == "RGB16Planar")
    {
        GENTLTEST_CHECK(true);
    } 
    else
    {
        GENTLTEST_CHECK_MESSAGE("Wrong pixelformat " << sPixelFormat.c_str(), false);
    }
}

void vTestAcquisitionMode(std::string sAcquisitionMode)
{
    if (sAcquisitionMode == "SingleFrame" ||
        sAcquisitionMode == "MultiFrame" ||
        sAcquisitionMode == "Continuous")
    {
        GENTLTEST_CHECK(true);
    }
    else
    {
        GENTLTEST_CHECK_MESSAGE("Wrong acquisitionmode " << sAcquisitionMode.c_str(), false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// helper
////////////////////////////////////////////////////////////////////////////////////////

void vDumpiTypeData(const std::string sMsgText, 
                    const GenICam::Client::INFO_DATATYPE iType, 
                    const char *pcBuff, 
                    const std::size_t iSize)
{
    std::stringstream sOutput;
    std::size_t nPos=0;

    sOutput << sMsgText.c_str() << " : (" << sConvertDataType2String(iType) << ") ";
    
    switch (iType)
    {
    case GenICam::Client::INFO_DATATYPE_STRING:
        sOutput << "'" << pcBuff << "'";
        break;
    case GenICam::Client::INFO_DATATYPE_STRINGLIST:
        sOutput << "'";
        while (nPos < iSize)
        {
            if (pcBuff[nPos] == '\0')
                sOutput << "'" << std::endl << "'";
            else
                sOutput << pcBuff[nPos];
            nPos++;
        }
        sOutput << "'";
        break;
    case GenICam::Client::INFO_DATATYPE_PTR:
    case GenICam::Client::INFO_DATATYPE_SIZET:
        sOutput << std::hex << "0x";
        if (iSize >= 8)
            sOutput << xConvertBuffer2UINT64(pcBuff);
        else
            sOutput << xConvertBuffer2UINT32(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_INT32:
        sOutput << std::hex << "0x" << xConvertBuffer2INT32(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_PTRDIFF:
    case GenICam::Client::INFO_DATATYPE_UINT32:
        sOutput << std::hex << "0x" << xConvertBuffer2UINT32(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_INT64:
        sOutput << std::hex << "0x" << xConvertBuffer2INT64(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_UINT64:
        sOutput << std::hex << "0x" << xConvertBuffer2UINT64(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_INT16:
        sOutput << std::hex << "0x" << xConvertBuffer2INT16(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_UINT16:
        sOutput << std::hex << "0x" << xConvertBuffer2UINT16(pcBuff);
        break;
    case GenICam::Client::INFO_DATATYPE_BOOL8:
        sOutput << (int)pcBuff[0];
        break;
    case GenICam::Client::INFO_DATATYPE_BUFFER:
    case GenICam::Client::INFO_DATATYPE_UNKNOWN:
        sOutput << std::hex << "{";
        while (nPos < iSize)
        {
            sOutput << "0x" << (int)pcBuff[nPos++] << ",";
        }
        sOutput << "}";
        break;
    case GenICam::Client::INFO_DATATYPE_FLOAT64:
        sOutput << xConvertBuffer2FLOAT64(pcBuff);
        break;
    default:
        sOutput << "unknown data type";
    }

    GENTLTEST_PRINT(sOutput.str().c_str() << std::endl);

    sOutput << std::dec;
}

void GenTLTest_timer_handler(ocGenTLTestTimer *poTimer)
{
    GENTLTEST_PRINT("******************************************\n");
    GENTLTEST_PRINT("* Watchdog timer timeout.\n");
    GENTLTEST_PRINT("* Current test hangs.\n");
    GENTLTEST_PRINT("* Stop process.\n");
    GENTLTEST_PRINT("******************************************\n");
    
    exit (-1);
}

////////////////////////////////////////////////////////////////////////////////////////
// GenTLTestTools
////////////////////////////////////////////////////////////////////////////////////////

GenTLTestTools::GenTLTestTools(void)
{
}

GenTLTestTools::~GenTLTestTools(void)
{
}

