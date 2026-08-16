/*****************************************************************//**
 * \file   case_4367.cpp
 * \brief  
 * 
 * \author maximn
 * \date   June 2024
 *********************************************************************/

#include <iostream>
#include <string>
#include "KYFGLib.h"
#include <map>
#include <thread>
#include <chrono>
#ifdef __linux__
#include <cstring> // memset
#endif
#pragma pack(push, 1)
#define KYFG_STREAMBUFFER_CALLBACK_FRAMESTART 1
typedef struct _KYFG_StreamBufferCallbackExParameters
{
    uint32_t version; // Version of this structure definition, must be 1

    // since version 1:
    STREAM_HANDLE        streamHandle;
    STREAM_BUFFER_HANDLE streamBufferHandle;
    void *               pUserContext;
    void *               pFrameMemory; // Same as returned by KYFG_BufferGetInfo() with 'cmdStreamBufferInfo' set to KY_STREAM_BUFFER_INFO_BASE
    double               instantFPS;   // Same as returned by KYFG_BufferGetInfo() with 'cmdStreamBufferInfo' set to KY_STREAM_BUFFER_INFO_INSTANTFPS
    uint32_t             bufferId;     // Same as returned by KYFG_BufferGetInfo() with 'cmdStreamBufferInfo' set to KY_STREAM_BUFFER_INFO_ID

    uint16_t      flags; // either 0, which is functional equivalent of "StreamBufferCallback", 
                         // or KYFG_STREAMBUFFER_CALLBACK_FRAMESTART, which indicates additional callback made when DMA starts filling a frame (if supported by the HW)
}KYFG_StreamBufferCallbackExParameters;
#pragma pack(pop)
typedef void(KYFG_CALLCONV *KYFG_StreamBufferCallbackEx)(const KYFG_StreamBufferCallbackExParameters* pParams);
#pragma pack(push, 1)
typedef struct _KYFG_StreamBufferCallbackOptions
{
    uint32_t version; // Version of this structure definition, must be 1

    // since version 1:
    STREAM_HANDLE streamHandle;
    KYFG_StreamBufferCallbackEx userFunc;
    void *        pUserContext;
    uint16_t      flags; // for version 1: only KYFG_STREAMBUFFER_CALLBACK_FRAMESTART (bit 0) is valid, all others must not be set
}KYFG_StreamBufferCallbackOptions;
#pragma pack(pop)

KAYA_API FGSTATUS KYFG_StreamBufferCallbackRegisterEx(const KYFG_StreamBufferCallbackOptions* pOptions);
KAYA_API FGSTATUS KYFG_StreamBufferCallbackUnregisterEx(const KYFG_StreamBufferCallbackOptions* pOptions);



#if !defined(_countof)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#define KY_MAX_BOARDS 4
//FGHANDLE m_FgHandles[KY_MAX_BOARDS];
unsigned int currentGrabberIndex;
int printCxp2Events = 0;
int printHeartbeats = 0;

#ifdef __linux__ // _aligned_malloc() implementation for __linux__
#include <signal.h>
void* _aligned_malloc(size_t size, size_t alignment)
{
    size_t pageAlign = size % 4096;
    if (pageAlign)
    {
        size += 4096 - pageAlign;
    }

#if(GCC_VERSION <= 40407)
    void * memptr = 0;
    posix_memalign(&memptr, alignment, size);
    return memptr;
#else
    return aligned_alloc(alignment, size);
#endif
}
#define _aligned_free free
#endif // #ifdef __linux__

#ifndef _MSC_VER
#define scanf_s scanf
#endif

// Helper functions to get user input
char BlockingInput()
{
    char c = -1;
#ifdef _MSC_VER
    while (scanf_s(" %c", &c, (unsigned int)(sizeof(char))) == -1);
#else
    while ((c == -1) || (c == '\n'))
    {
        c = getc(stdin);
    }
#endif
    return c;
}

void CharInput(char expected_char, const char* error_str)
{
    int valid = 0;
    char c;

    while (!valid)
    {
        c = BlockingInput();

        valid = expected_char == c;

        if (!valid)
        {
            printf("%s", error_str);
        }
        else
        {
            break;
        }
    }
}

void NumberInRangeInput(int min, int max, int* value, const char* error_str)
{
    int valid = 0;
    char c;

    while (!valid)
    {
        c = BlockingInput();

        *value = c - '0';

        valid = *value >= min && *value <= max;

        if (!valid)
        {
            printf("%s", error_str);
        }
        else
        {
            break;
        }
    }
}

//#define FGLIB_ALLOCATED_BUFFERS // Uncomment this #definition to use buffers allocated by KYFGLib
#define MINIMAL_CALLBACK

#define MAXBOARDS 4
CAMHANDLE camHandleArray[KY_MAX_BOARDS][KY_MAX_CAMERAS]; // There are maximum KY_MAX_CAMERAS cameras
STREAM_HANDLE cameraStreamHandle = INVALID_STREAMHANDLE;
FGHANDLE m_FgHandles[MAXBOARDS];
int nKayaDevicesCount = 0;
int64_t dmaImageIdCapable = 0;
int grabberIndex = 0, cameraIndex = 0, argsCameraIndex = 0;
int gCallbackCounter = 0;
int gCallbackCounter_with_Flag = 0;
int gCallbackCounter_without_Flag = 0;
int bufferIdArray_array_index = 0;
KYBOOL isUnnattended;
std::string bufferIdArray_array[2000];
void StreamBufferCallbackEx(const KYFG_StreamBufferCallbackExParameters* pParams)
{
    if (pParams->flags == 1)
    {
        //printf("FIRST INTERRUPT \n");
        gCallbackCounter_with_Flag += 1;

        //printf("BIFFER ID FIRST INTERRUPT: %d\n", pParams->bufferId);
        std::string str = "BIFFER ID: " + std::to_string(pParams->bufferId) + " FIRST INTERRUPT";
        bufferIdArray_array[bufferIdArray_array_index] = str;
        bufferIdArray_array_index++;
    }
    else
    {
        //printf("not first \n");
        gCallbackCounter_without_Flag += 1;
        //printf("BIFFER ID NOT FIRST INTERRUPT: %d\n", pParams->bufferId);
        std::string str = "BIFFER ID: " + std::to_string(pParams->bufferId) + " NOT FIRST INTERRUPT";
        bufferIdArray_array[bufferIdArray_array_index] = str;
        bufferIdArray_array_index++;
    }
    gCallbackCounter += 1;
    KYFG_BufferToQueue(pParams->streamBufferHandle, KY_ACQ_QUEUE_INPUT);

}

int ConnectToGrabber(unsigned int _grabberIndex)
{
    int64_t dmaQueuedBufferCapable;
    int64_t interprocessSharingCapable;

    if ((m_FgHandles[_grabberIndex] = KYFG_Open(_grabberIndex)) != -1) // Connect to selected device
    {
        printf("Good connection to grabber #%d, m_FgHandles=%X\n", _grabberIndex, m_FgHandles[_grabberIndex]);
    }
    else
    {
        printf("Could not connect to grabber #%d\n", _grabberIndex);
        BlockingInput();
        return 0;
    }

    dmaQueuedBufferCapable = KYFG_GetGrabberValueInt(m_FgHandles[_grabberIndex], DEVICE_QUEUED_BUFFERS_SUPPORTED);
    interprocessSharingCapable = KYFG_GetGrabberValueInt(m_FgHandles[_grabberIndex], DEVICE_INTERPROCESS_SHARING_SUPPORTED);

    if (1 != dmaQueuedBufferCapable)
    {
        printf("Grabber #%d does not support queued buffers\n", _grabberIndex);
        BlockingInput();
        return 0;
    }

    dmaImageIdCapable = KYFG_GetGrabberValueInt(m_FgHandles[_grabberIndex], DEVICE_IMAGEID_SUPPORTED);

    printf("Grabber #%d, KY_STREAM_BUFFER_INFO_IMAGEID %ssupported\n", _grabberIndex, dmaImageIdCapable ? "" : "not ");

    currentGrabberIndex = _grabberIndex;


    return 1;
}

int StartCamera(unsigned int _grabberIndex, unsigned int _cameraIndex)
{
    // Put all buffers to input queue
    KYFG_BufferQueueAll(cameraStreamHandle, KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_INPUT);

    // Start acquisition
    KYFG_CameraStart(camHandleArray[_grabberIndex][_cameraIndex], cameraStreamHandle, 0);

    return 0;
}

void CloseGrabbers()
{
    for (int i = 0; i < nKayaDevicesCount; i++)
    {
        if (INVALID_FGHANDLE != m_FgHandles[i])
        {
            if (FGSTATUS_OK != KYFG_Close(m_FgHandles[i])) // Close the selected device and unregisters all associated routines
            {
                printf("Wasn't able to close grabber #%d\n", i);
            }
            else
            {
                printf("Grabber #%d closed\n", i);
            }
        }
    }
}

void Close(int returnCode)
{
    printf("Input any char to exit\n");
    if (!isUnnattended)
    {
        BlockingInput();
    }
    exit(returnCode);
}

void stopStream()
{
    if (camHandleArray[grabberIndex][cameraIndex] != INVALID_CAMHANDLE)
    {
        if (FGSTATUS_OK == KYFG_CameraStop(camHandleArray[grabberIndex][cameraIndex]))
        {
            printf("\nCamera successfully stoped\n");
        }
    }
    if (cameraStreamHandle != INVALID_STREAMHANDLE)
    {
        if (FGSTATUS_OK == KYFG_StreamDelete(cameraStreamHandle))
        {
            printf("Stream successfully deleted\n");
        }
    }

}
#ifdef __linux__
void crash_handler(int signum)
{
    switch (signum)
        /// from https://pubs.opengroup.org/onlinepubs/009695399/basedefs/signal.h.html
    {
    case SIGABRT:
    case SIGFPE:
    case SIGTERM:
    {
        stopStream();
        Close(0);
        break;
    }
    }
}
#endif

void printStatistic()
{
    int64_t RXFrameCounter = KYFG_GetGrabberValueInt(camHandleArray[grabberIndex][cameraIndex], "RXFrameCounter");
    int64_t DropFrameCounter = KYFG_GetGrabberValueInt(camHandleArray[grabberIndex][cameraIndex], "DropFrameCounter");
    int64_t RXPacketCounter = KYFG_GetGrabberValueInt(camHandleArray[grabberIndex][cameraIndex], "RXPacketCounter");
    int64_t DropPacketCounter = KYFG_GetGrabberValueInt(camHandleArray[grabberIndex][cameraIndex], "DropPacketCounter");

    printf("\nStream statistic:\n");
    printf("RXFrameCounter: %" PRId64 "\n", RXFrameCounter);
    printf("gCallbackCounter: %d\n", gCallbackCounter);
    printf("gCallbackCounter_with_Flag: %d\n", gCallbackCounter_with_Flag);
    printf("gCallbackCounter_without_Flag: %d\n", gCallbackCounter_without_Flag);
    printf("DropFrameCounter: %" PRId64 "\n", DropFrameCounter);
    printf("RXPacketCounter: %" PRId64 "\n", RXPacketCounter);
    printf("DropPacketCounter: %" PRId64 "\n", DropPacketCounter);

    
    for (int i = 0; i < bufferIdArray_array_index; i++)
    {
        std::cout << bufferIdArray_array[i] << std::endl;
    }
}

int printHelp()
{
    std::cout << "Available parameters:" << std::endl;
    std::cout << "--unattended:     0-Disable 1-Enable; " << std::endl;
    std::cout << "--device_index:   delect grabber ingex if unattended mode enable" << std::endl;
    std::cout << "--frameRate:      Expected frame rate" << std::endl;
    std::cout << "--cameraIndex:    Index of camera" << std::endl;
    std::cout << "--help --h:       print help" << std::endl;
    return 0;
}
/*
This code is inspired by https://stackoverflow.com/a/868894/1468415
The class InputParser is used for parsing command line arguments
*/
class InputParser
{
public:
    InputParser()
    {
    }
    typedef std::initializer_list<std::pair<const std::string, std::string>> InitilizerList;
    InputParser(const InitilizerList & _list)
        :m_mapOptions(_list)
    {
    }

    void ReadOptions(int &argc, char **argv)
    {
        for (int i = 0; i < argc; i += 1)
        {
            if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "--h") == 0))
            {
                printHelp();
                Close(0);
            }
        }
        for (int i = 1; (i + 1) < argc; i += 2)
        {
            m_mapOptions[argv[i]] = argv[i + 1];
        }
    }

    void PrintOptions() const
    {
        for (const auto& n : m_mapOptions)
        {
            std::cout << n.first << " = " << n.second << "; ";
        }
        std::cout << std::endl;
    }

    const std::string& getCmdOption(const std::string &option) const
    {
        static const std::string empty_string("");
        try
        {
            return m_mapOptions.at(option);
        }
        catch (...)
        {
            return empty_string;
        }
    }

    bool cmdOptionExists(const std::string &option) const
    {
        return m_mapOptions.count(option) > 0;
    }
private:
    std::map<std::string, std::string> m_mapOptions;
}; // class InputParser 

uint32_t expectedFPS;

int main(int argc, char **argv)
{
    size_t frameDataSize, frameDataAligment;
    STREAM_BUFFER_HANDLE streamBufferHandle[16] = { 0 };
    void *pBuffer[_countof(streamBufferHandle)] = { NULL };
    int detectedCameras[KY_MAX_BOARDS];
    char c = 0;
    KYFGLib_InitParameters kyInit;
    KY_DEVICE_INFO* grabbersInfoArray = NULL;
    KYFGCAMERA_INFO2* cameraInfoArray = NULL;
    KYBOOL streamRunning = KYFALSE;
    // Register Signals
#ifdef __linux__
    signal(SIGABRT, crash_handler);
    signal(SIGFPE, crash_handler);
    signal(SIGTERM, crash_handler);
#endif 
    // Initialize library

    memset(&kyInit, 0, sizeof(kyInit));
    kyInit.version = 2;
    kyInit.concurrency_mode = 0;
    kyInit.logging_mode = 0;
    kyInit.noVideoStreamProcess = KYFALSE;

    if (FGSTATUS_OK != KYFGLib_Initialize(&kyInit))
    {
        printf("Library initialization failed \n ");
        Close(-1);
    }

    for (int i = 0; i < KY_MAX_BOARDS; i++)
    {
        m_FgHandles[i] = INVALID_FGHANDLE;
    }

    // Scan for grabbers

    KY_DeviceScan(&nKayaDevicesCount);    // Retrieve the number of virtual and hardware devices connected to PC

    if (!nKayaDevicesCount)
    {
        printf("No PCI devices found\n");
        Close(0);
    }

    grabbersInfoArray = static_cast<KY_DEVICE_INFO*>(malloc(nKayaDevicesCount * sizeof(KY_DEVICE_INFO)));

    for (int i = 0; i < nKayaDevicesCount; i++)
    {
        grabbersInfoArray[i].version = KY_MAX_DEVICE_INFO_VERSION;
        if (FGSTATUS_OK != KY_DeviceInfo(i, &grabbersInfoArray[i]))
        {
            printf("Wasn't able to retrive information from device #%d\n", i);
            continue;
        }
    }
    KYBOOL grabberReady = KYFALSE;
    KYBOOL cameraReady = KYFALSE;
    InputParser options
    (
        {
            {"--unattended",            "0"},      // 0-False 1-true
            {"--device_index",          "0"},      // use 0-th PCI device
            {"--frameRate",              "" },          
            {"--cameraIndex",            "" },          
        }
    );
    options.ReadOptions(argc, argv);
    options.PrintOptions();
    
    // "--unattended"
    const std::string &sunattended = options.getCmdOption("--unattended");
    if (!sunattended.empty())
    {
        isUnnattended = std::stoi(sunattended);
    }
    if (isUnnattended)
    {
        std::cout << "Script starts in unattended mode !!!" << std::endl;

        std::string sexpectedFPS = options.getCmdOption("--frameRate");
        if (!sexpectedFPS.empty())
        {
            expectedFPS = std::stoi(sexpectedFPS);
            std::cout << "expectedFPS " << expectedFPS << std::endl;
        }
            

        const std::string &sdeviceIndex = options.getCmdOption("--device_index");
        if (!sdeviceIndex.empty())
        {
            grabberIndex = std::stoi(sdeviceIndex);
            std::cout << "Grabber index " << grabberIndex << std::endl;
        }
        const std::string &scameraIndex = options.getCmdOption("--cameraIndex");
        argsCameraIndex = std::stoi(scameraIndex);
        if (ConnectToGrabber(grabberIndex))
        {
            grabberReady = KYTRUE;
        }
        int nDetectedCameras = _countof(camHandleArray[0]);

        if (FGSTATUS_OK != KYFG_UpdateCameraList(m_FgHandles[grabberIndex], camHandleArray[grabberIndex], &nDetectedCameras))
        {
            printf("Camera detect error. Please try again\n");
            Close(-1);
        }
        if (!nDetectedCameras)
        {
            printf("No cameras detected. Please connect at least one camera\n");
            Close(-1);
        }
        printf("Number of cameras connected to the PCI device #%d: %d\n", grabberIndex, nDetectedCameras);

        detectedCameras[grabberIndex] = nDetectedCameras;

        if (cameraInfoArray)
        {
            free(cameraInfoArray);
        }

        cameraInfoArray = static_cast<KYFGCAMERA_INFO2*>(malloc(nDetectedCameras * sizeof(KYFGCAMERA_INFO2)));

        for (int i = 0; i < nDetectedCameras; i++)
        {
            cameraInfoArray[i].version = 1;
            KYFG_CameraInfo2(camHandleArray[grabberIndex][i], &cameraInfoArray[i]);
        }
        printf("\nSelect and connect camera:\n");
        for (cameraIndex = 0; cameraIndex < detectedCameras[grabberIndex]; cameraIndex++)
        {
            printf("[%d] %s: Firmware %s\n",
                cameraIndex,
                cameraInfoArray[cameraIndex].deviceModelName,
                cameraInfoArray[cameraIndex].deviceFirmwareVersion);
        }
        cameraIndex = argsCameraIndex;
        /////////////////////////////////////////////
    }
    else
    {
        while (!grabberReady)
        {
            // Select framegrabber

            printf("Select and open grabber:\n");
            for (int i = 0; i < nKayaDevicesCount; i++)
            {
                printf("[%d] %s on PCI slot {%d:%d:%d}: Protocol 0x%X, Generation %d\n",
                    i,
                    grabbersInfoArray[i].szDeviceDisplayName,
                    grabbersInfoArray[i].nBus,
                    grabbersInfoArray[i].nSlot,
                    grabbersInfoArray[i].nFunction,
                    grabbersInfoArray[i].m_Protocol,
                    grabbersInfoArray[i].DeviceGeneration);
            }

            KYBOOL inputValid = KYFALSE;

            while (!inputValid)
            {
                NumberInRangeInput(0, nKayaDevicesCount - 1, &grabberIndex, "Invalid index\n");

                printf("Selected grabber #%d\n", grabberIndex);
                inputValid = KYTRUE;
                break;
            }

            // Connect to framegrabber

            if (ConnectToGrabber(grabberIndex))
            {
                grabberReady = KYTRUE;

                break;
            }

        }
        while (!cameraReady)
        {
            KYBOOL cameraScaned = KYFALSE;

            while (!cameraScaned)
            {
                printf("\nPress [d] to detect cameras\n");

                CharInput('d', "Invalid input\n");

                //int nDetectedCameras = _countof(camHandleArray[0]);
                int nDetectedCameras = 16;

                if (FGSTATUS_OK != KYFG_UpdateCameraList(m_FgHandles[grabberIndex], camHandleArray[grabberIndex], &nDetectedCameras))
                {
                    printf("Camera detect error. Please try again\n");
                    continue;
                }

                if (!nDetectedCameras)
                {
                    printf("No cameras detected. Please connect at least one camera\n");
                    continue; // No cameras were detected
                }

                printf("Number of cameras connected to the PCI device #%d: %d\n", grabberIndex, nDetectedCameras);

                detectedCameras[grabberIndex] = nDetectedCameras;

                if (cameraInfoArray)
                {
                    free(cameraInfoArray);
                }

                cameraInfoArray = static_cast<KYFGCAMERA_INFO2*>(malloc(nDetectedCameras * sizeof(KYFGCAMERA_INFO2)));

                for (int i = 0; i < nDetectedCameras; i++)
                {
                    cameraInfoArray[i].version = 1;
                    KYFG_CameraInfo2(camHandleArray[grabberIndex][i], &cameraInfoArray[i]);
                }

                cameraScaned = KYTRUE;
                cameraReady = KYTRUE;
                break;
            }

            KYBOOL cameraConnected = KYFALSE;
        }

        printf("\nSelect and connect camera:\n");
        for (cameraIndex = 0; cameraIndex < detectedCameras[grabberIndex]; cameraIndex++)
        {
            printf("[%d] %s: Firmware %s\n",
                cameraIndex,
                cameraInfoArray[cameraIndex].deviceModelName,
                cameraInfoArray[cameraIndex].deviceFirmwareVersion);
        }

        NumberInRangeInput(0, detectedCameras[grabberIndex] - 1, &cameraIndex, "Invalid index\n");
    }
    // Open a connection to the chosen camera
    if (FGSTATUS_OK == KYFG_CameraOpen2(camHandleArray[grabberIndex][cameraIndex], NULL))
    {
        printf("Camera #%d was connected successfully\n", cameraIndex);

        // Update camera/grabber buffer dimensions parameters before stream creation
        KYFG_SetCameraValueInt(camHandleArray[grabberIndex][cameraIndex], "Width", 640); // set camera width 
        KYFG_SetCameraValueInt(camHandleArray[grabberIndex][cameraIndex], "Height", 480); // set camera height
        if (expectedFPS)
        {
            KYFG_SetCameraValueFloat(camHandleArray[grabberIndex][cameraIndex], "AcquisitionFrameRate", expectedFPS);
        }
        
        //KYFG_SetCameraValueEnum_ByValueName(camHandleArray[grabberIndex][cameraIndex], "PixelFormat", "Mono8"); // set camera pixel format

        // Create stream and assign appropriate runtime acquisition callback function
        KYFG_StreamCreate(camHandleArray[grabberIndex][cameraIndex], &cameraStreamHandle, 0);
        //KYFG_StreamBufferCallbackRegister(cameraStreamHandle, Stream_callback_func, NULL);
        KYFG_StreamBufferCallbackOptions callbackOptions;
        callbackOptions.flags = KYFG_STREAMBUFFER_CALLBACK_FRAMESTART;
        callbackOptions.version = 1;
        callbackOptions.userFunc = StreamBufferCallbackEx;
        callbackOptions.streamHandle = cameraStreamHandle;
        callbackOptions.pUserContext = NULL;
        int res = KYFG_StreamBufferCallbackRegisterEx(&callbackOptions);
        printf("RES: %x\n", res);
        KYFG_SetGrabberValueBool(m_FgHandles[grabberIndex], "FrameStartCallbacks", KYTRUE);
        //KYFG_SetGrabberValueBool(m_FgHandles[grabberIndex], "FrameStartCallbacks", KYFALSE);
        KYBOOL boolres = KYFG_GetGrabberValueBool(m_FgHandles[grabberIndex], "FrameStartCallbacks");
        printf("FrameStartCallbacks = %d\n", boolres);
        // Retrieve information about required frame buffer size and alignment 
        KYFG_StreamGetInfo(cameraStreamHandle,
            KY_STREAM_INFO_PAYLOAD_SIZE,
            &frameDataSize,
            NULL, NULL);

        KYFG_StreamGetInfo(cameraStreamHandle,
            KY_STREAM_INFO_BUF_ALIGNMENT,
            &frameDataAligment,
            NULL, NULL);

        // Allocate memory for desired number of frame buffers
        for (int iFrame = 0; iFrame < _countof(streamBufferHandle); iFrame++)
        {
#ifdef FGLIB_ALLOCATED_BUFFERS
#pragma message("Building with KYFGLib allocated buffers")
            KYFG_BufferAllocAndAnnounce(cameraStreamHandle,
                frameDataSize,
                NULL,
                &streamBufferHandle[iFrame]);
#else
#pragma message("Building with user allocated buffers")
            pBuffer[iFrame] = _aligned_malloc(frameDataSize, frameDataAligment);
            KYFG_BufferAnnounce(cameraStreamHandle,
                pBuffer[iFrame],
                frameDataSize,
                NULL,
                &streamBufferHandle[iFrame]);
#endif 

        }

    }
    else
    {
        printf("Camera is not opened\n");
        Close(-1);
    }
    if (isUnnattended)
    {
        StartCamera(grabberIndex, cameraIndex);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        KYFG_CameraStop(camHandleArray[grabberIndex][cameraIndex]);
        printStatistic();
    }
    else
    {
        while (c != 'e')
        {
            printf("\nSelect option:\n");
            printf("[s] %s stream\n", streamRunning ? "Stop" : "Start");
            printf("[e] Exit\n");

            c = BlockingInput();

            if ('s' == c)
            {
                streamRunning = 1 - streamRunning;

                if (streamRunning)
                {
                    StartCamera(grabberIndex, cameraIndex);
                }
                else
                {
                    KYFG_CameraStop(camHandleArray[grabberIndex][cameraIndex]);
                    printStatistic();
                }
            }

            if ('h' == c)
            {
                printHeartbeats = 1 - printHeartbeats; // Toggle 'printHeartbeats' between 0 and 1
            }

            if ('v' == c)
            {
                printCxp2Events = 1 - printCxp2Events; // Toggle 'printCxp2Events' between 0 and 1
            }
#ifdef __linux__
            if ('a' == c)
            {
                abort();
            }
            if ('f' == c)
            {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
                int fail = 5 / 0;
#pragma GCC diagnostic pop
            }
#endif
        }
    }
    CloseGrabbers();

    if (grabbersInfoArray)
    {
        free(grabbersInfoArray); // Release grabbers info
    }

    if (cameraInfoArray)
    {
        free(cameraInfoArray); // Release cameras info
    }

    // Release pBuffer

    for (int iFrame = 0; iFrame < _countof(streamBufferHandle); iFrame++)
    {
        if (pBuffer[iFrame])
        {
            _aligned_free(pBuffer[iFrame]);
            pBuffer[iFrame] = NULL;
        }
    }

    isUnnattended ==KYTRUE? exit(0) : Close(0);
}
