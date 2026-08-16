/*****************************************************************//**
 * \file   case_3632.cpp
 * \brief  
 * 
 * \author maximn
 * \date   February 2024
 *********************************************************************/

#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <valarray>
#include <atomic>
#include <algorithm>

#ifdef __linux__
#include <cstring> // memset
#endif

#if !defined(_countof)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif


#include <KYFGLib.h>

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
        for (int i = 1; (i + 1) < argc; i+=2)
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


#define KY_MAX_BOARDS 4
FGHANDLE m_FgHandles[KY_MAX_BOARDS];
unsigned int currentGrabberIndex;

#ifdef __linux__ // _aligned_malloc() implementation for __linux__
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

int attachDebugger = 0;
int device_index = 0;
int nCallbacksMaxCount = 10000;
int nAuxWaitTimeSec = 5;
int nStreamsWaitTimeSec = 30;

// Dimensions
int64_t nWidth = 1200;
int64_t nHeight = 720;

// Pixel format
char szPixelFormat[16];
uint32_t sizePixelFormat = sizeof(szPixelFormat);

// Framerate
double fFrameRate = 20;

bool bDoMeasurements = true;
size_t g_frameDataSize, g_frameDataAligment;
STREAM_BUFFER_HANDLE g_streamBufferHandle[16] = { 0 };
void *pBuffer[KY_MAX_CAMERAS][_countof(g_streamBufferHandle)] = { NULL };

struct ValuesArray : public std::valarray<uint64_t>
{
    ValuesArray() 
        :std::valarray<uint64_t>(nCallbacksMaxCount)
    {
    }
};

struct CameraInfo
{
    CameraInfo(CAMHANDLE _camHandle, STREAM_HANDLE _streamHandle)
        :camHandle(_camHandle), streamHandle(_streamHandle)
    {
    }
    CAMHANDLE camHandle;
    STREAM_HANDLE streamHandle;
    int callbackCounter = 0;
    ValuesArray m_FrameTimestampsHw;
    ValuesArray m_InterruptTimestampsHw;
    ValuesArray m_InterruptTimestampsChrono;
    ValuesArray m_CallbackTimestampsHw;
    ValuesArray m_CallbackTimestampsChrono;
};

struct FrameGrabberInfo
{
    FrameGrabberInfo(int _index, const std::string& _name, FGHANDLE _handle)
        :index(_index), name(_name), handle(_handle)
    {

    }
    int index;
    std::string name;
    FGHANDLE handle;
    std::vector<CameraInfo> cameras;

    size_t m_auxCallbackCounter = 0;
    ValuesArray m_AuxTimestampsHw; // internal HW timestamps, NOT taken in the software
    ValuesArray m_AuxInterruptTimestampsHw;
    ValuesArray m_AuxInterruptTimestampsChrono;
    ValuesArray m_AuxCallbackTimestampsHw;
    ValuesArray m_AuxCallbackTimestampsChrono;
};

static std::vector<FrameGrabberInfo> frameGrabberInfoList;
static void Stream_callback_func(STREAM_BUFFER_HANDLE streamBufferHandle, void *cameraContext);

char BlockingInput()
{
    char c = -1;
    #ifdef _MSC_VER
    while (scanf_s(" %c", &c, (unsigned int)(sizeof(char))) == -1);
    #else
    while ( (c == -1) || (c == '\n'))
    {
        c = getc(stdin);
    }
    #endif
    return c;
}

#define MAXBOARDS 4
CAMHANDLE camHandleArray[KY_MAX_BOARDS][KY_MAX_CAMERAS]; // There are maximum KY_MAX_CAMERAS cameras
STREAM_HANDLE cameraStreamHandle = INVALID_STREAMHANDLE;
int nKayaDevicesCount = 0;
static std::atomic_bool bCollectStats(true);

void Stream_callback_func(STREAM_BUFFER_HANDLE streamBufferHandle, void* cameraContext)
{
    if(NULL_STREAM_BUFFER_HANDLE == streamBufferHandle)
    {
        // This callback indicates that acquisition has stopped
        return;
    }
    if (INVALID_STREAMHANDLE != streamBufferHandle) 
    {
        CameraInfo *cameraInfo = reinterpret_cast<CameraInfo *>(cameraContext);

        if (bDoMeasurements && (cameraInfo->callbackCounter < nCallbacksMaxCount))
        {
            ////////////////////////////////////////////
            // Store hardware frame timestamp
            KYFG_BufferGetInfo(streamBufferHandle,
                KY_STREAM_BUFFER_INFO_TIMESTAMP,
                &(cameraInfo->m_FrameTimestampsHw[cameraInfo->callbackCounter]),
                NULL,
                NULL);

            ////////////////////////////////////////////
            // Get frame interrupt timestamp (HW counter)
            KYFG_BufferGetInfo(streamBufferHandle,
                KY_STREAM_BUFFER_INFO_INTERRUPTTIMESTAMP_HW, // Implemented since trunk version 9924 only!
                &(cameraInfo->m_InterruptTimestampsHw[cameraInfo->callbackCounter]),
                NULL,
                NULL);

            // Get frame interrupt timestamp (std::chrono::steady_clock)
            KYFG_BufferGetInfo(streamBufferHandle,
                KY_STREAM_BUFFER_INFO_INTERRUPTTIMESTAMP_CHRONO, // Implemented since trunk version 9924 only!
                &(cameraInfo->m_InterruptTimestampsChrono[cameraInfo->callbackCounter]),
                NULL,
                NULL);

            ////////////////////////////////////////////
            // Get the current time points
            
            // Using KYFG_DeviceDirectHardwareRead of the grabber's "Timestamp" register
            KYFG_DeviceDirectHardwareRead(cameraInfo->camHandle, 0x180, &(cameraInfo->m_CallbackTimestampsHw[cameraInfo->callbackCounter]), sizeof(uint64_t));

            // Using std::chrono::steady_clock
            auto now = std::chrono::steady_clock::now();
            cameraInfo->m_CallbackTimestampsChrono[cameraInfo->callbackCounter] = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

            // auto callbackGenICamTimestamp = KYFG_GetGrabberValueInt(cameraInfo->camHandle, "Timestamp");

            // Increase counter
            cameraInfo->callbackCounter++;
        }//if (cameraInfo->callbackCounter < nCallbacksMaxCount)
    }//if (streamBufferHandle != INVALID_STREAMHANDLE && streamBufferHandle != NULL_STREAMHANDLE)
   
    // Return stream buffer to input queue
    KYFG_BufferToQueue(streamBufferHandle, KY_ACQ_QUEUE_INPUT);
}

int StartCamera(CameraInfo camera)
{
    // Put all buffers to input queue
    KYFG_BufferQueueAll(camera.streamHandle, KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_INPUT);
    printf("Camera %X started\n", camera.camHandle);
    // Start acquisition
    KYFG_CameraStart(camera.camHandle, camera.streamHandle, 0);

    return 0;
}

void CloseGrabbers()
{
    for(FrameGrabberInfo info : frameGrabberInfoList)
    {
        if(INVALID_FGHANDLE !=info.handle)
        {
            if (FGSTATUS_OK != KYFG_Close(info.handle)) // Close the selected device and unregisters all associated routines
            {
                printf("Wasn't able to close grabber #%d\n", info.index);
            }
            else
            {
                printf("Grabber #%d closed\n", info.index);
            }
        }
    }
}

void Close(int returnCode)
{
    printf("Input any char to exit\n");
    BlockingInput();
    exit(returnCode);
}
template<typename T>
void printStatistic(const std::valarray<T> &_periodsArray, const std::string& name)
{
    std::valarray<T> periodsArray(_periodsArray);

    std::ios oldState(nullptr);
    oldState.copyfmt(std::cout);
    std::cout.imbue(std::locale(""));
    static const std::streamsize streamWidth = 11;
    static char chFill = ' ';
    
    std::cout << name 
              << " min "     << std::setfill(chFill) << std::setw(streamWidth) << periodsArray.min() 
              << ", max "    << std::setfill(chFill) << std::setw(streamWidth) << periodsArray.max()
              << ", jitter " << std::setfill(chFill) << std::setw(streamWidth) << (periodsArray.max() - periodsArray.min());

    uint64_t nMedianTime;
    std::sort(std::begin(periodsArray), std::end(periodsArray));
    auto size = periodsArray.size();
    if (size % 2 == 0)
    {
        nMedianTime = (periodsArray[size / 2 - 1] + periodsArray[size / 2]) / 2;
    }
    else
    {
        nMedianTime = periodsArray[size / 2];
    }
    std::cout << ", median "  << std::setfill(chFill) << std::setw(streamWidth) << nMedianTime 
              << ", average " << std::setfill(chFill) << std::setw(streamWidth) << (periodsArray.sum() / size);

    std::cout << std::endl;
    std::cout.copyfmt(oldState);
}

void AuxDataCallbackImpl(KYFG_AUX_DATA* pData, void* context)
{
    FrameGrabberInfo* pFrameGrabberInfo = (FrameGrabberInfo*)context;

    if (pFrameGrabberInfo->m_auxCallbackCounter < pFrameGrabberInfo->m_AuxCallbackTimestampsChrono.size())
    {
        size_t& auxCallbackCounter = pFrameGrabberInfo->m_auxCallbackCounter;

        pFrameGrabberInfo->m_AuxTimestampsHw[auxCallbackCounter] = pData->u_data.io_data.timestamp;
        pFrameGrabberInfo->m_AuxInterruptTimestampsHw[auxCallbackCounter] = pData->m_InterruptTimestampHw;
        pFrameGrabberInfo->m_AuxInterruptTimestampsChrono[auxCallbackCounter] = pData->m_InterruptTimestampChrono;

        ////////////////////////////////////////////
        // Get the current time points

        // Using KYFG_DeviceDirectHardwareRead of the grabber's "Timestamp" register
        KYFG_DeviceDirectHardwareRead(pFrameGrabberInfo->handle, 0x180, &(pFrameGrabberInfo->m_AuxCallbackTimestampsHw[auxCallbackCounter]), sizeof(uint64_t));

        auto now = std::chrono::steady_clock::now();
        pFrameGrabberInfo->m_AuxCallbackTimestampsChrono[auxCallbackCounter] =
                std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

        auxCallbackCounter++;
    }
}


int ConnectToGrabber(FrameGrabberInfo &infoGrabber)
{
    int64_t dmaQueuedBufferCapable;
    int64_t interprocessSharingCapable;
    if ((infoGrabber.handle = KYFG_Open(infoGrabber.index)) != INVALID_FGHANDLE) // Connect to selected device
    {
        printf("Good connection to grabber #%d, m_FgHandles=%X\n", infoGrabber.index, infoGrabber.handle);
    }
    else
    {
        printf("Could not connect to grabber #%d\n", infoGrabber.index);
    }

    dmaQueuedBufferCapable = KYFG_GetGrabberValueInt(infoGrabber.handle, DEVICE_QUEUED_BUFFERS_SUPPORTED);
    if (1 != dmaQueuedBufferCapable)
    {
        printf("Grabber #%d does not support queued buffers\n", infoGrabber.index);
    }

    interprocessSharingCapable = KYFG_GetGrabberValueInt(infoGrabber.handle, DEVICE_INTERPROCESS_SHARING_SUPPORTED);
    printf("Grabber #%d %s interprocess sharing\n", infoGrabber.index, interprocessSharingCapable ? "supports" : "does not support");

    // Tune background monitoring thread:
    KYFG_SetGrabberValueInt(infoGrabber.handle, "MonitoringStepsMask", 0);

    if (!(nAuxWaitTimeSec > 0))
    {
        std::cout << "nAuxWaitTimeSec is not greater than 0, I will not measure aux callbacks" << std::endl;
        return 0;
    }

    // Setting value of 'DirectAuxListener'
    KYBOOL bDirectAuxListener = KYTRUE;
    KYFG_SetGrabberValueBool(infoGrabber.handle, "DirectAuxListener", bDirectAuxListener);
    std::cout << "DirectAuxListener: " << (KYTRUE == bDirectAuxListener ? "KYTRUE" : "KYFALSE") << std::endl;

    // 
    KYFG_AuxDataCallbackRegister(infoGrabber.handle, AuxDataCallbackImpl, &infoGrabber);
    KYFG_SetGrabberValueFloat(infoGrabber.handle, "TimerDelay", 50000);
    KYFG_SetGrabberValueFloat(infoGrabber.handle, "TimerDuration", 50000);
    KYFG_SetGrabberValueEnum_ByValueName(infoGrabber.handle, "TimerEventMode", "RisingEdge");
    KYFG_SetGrabberValueEnum_ByValueName(infoGrabber.handle, "TimerTriggerSource", "KY_CONTINUOUS");

    // Wait some time to collect statistical data
    std::chrono::seconds waitTime(nAuxWaitTimeSec);
    std::this_thread::sleep_for(waitTime);

    KYFG_SetGrabberValueEnum_ByValueName(infoGrabber.handle, "TimerTriggerSource", "KY_DISABLED");
    KYFG_SetGrabberValueEnum_ByValueName(infoGrabber.handle, "TimerEventMode", "Disabled");
    KYFG_AuxDataCallbackUnregister(infoGrabber.handle, AuxDataCallbackImpl);

    std::cout << "auxCallbackCounter: " << infoGrabber.m_auxCallbackCounter << std::endl;

    if (infoGrabber.m_auxCallbackCounter >= 2)
    {
        size_t nPeriods = infoGrabber.m_auxCallbackCounter - 1;

        // auxTimestampPeriodsHw
        std::valarray<uint64_t> auxTimestampPeriodsHw(nPeriods);
        for (size_t i = 0; i < nPeriods; i++)
        {
            auxTimestampPeriodsHw[i] = infoGrabber.m_AuxTimestampsHw[i + 1] - infoGrabber.m_AuxTimestampsHw[i];
        }

        // m_AuxInterruptTimestampsHw
        // ... should be multiplied by 8 to convert them to nanoseconds
        /*for (int i = 0; i < infoGrabber.m_AuxInterruptTimestampsHw[i]; i++)
        {
            infoGrabber.m_AuxInterruptTimestampsHw[i] *= 8;
        }*/
        // auxInterruptTimestampPeriodsHw
        std::valarray<uint64_t> auxInterruptTimestampPeriodsHw(nPeriods);
        for (size_t i = 0; i < nPeriods; i++)
        {
            auxInterruptTimestampPeriodsHw[i] = infoGrabber.m_AuxInterruptTimestampsHw[i + 1] - infoGrabber.m_AuxInterruptTimestampsHw[i];
        }

        // m_AuxCallbackTimestampsHw
        // ... should be multiplied by 8 to convert them to nanoseconds
        for (auto& ts : infoGrabber.m_AuxCallbackTimestampsHw)
        {
            ts *= 8;
        }
        // auxCallbackTimestampPeriodsHw
        std::valarray<uint64_t> auxCallbackTimestampPeriodsHw(nPeriods);
        for (size_t i = 0; i < nPeriods; i++)
        {
            auxCallbackTimestampPeriodsHw[i] = infoGrabber.m_AuxCallbackTimestampsHw[i + 1] - infoGrabber.m_AuxCallbackTimestampsHw[i];
        }

        // auxCallbackTimestampPeriodsChrono
        std::valarray<uint64_t> auxCallbackTimestampPeriodsChrono(nPeriods);
        for (size_t i = 0; i < nPeriods; i++)
        {
            auxCallbackTimestampPeriodsChrono[i] = infoGrabber.m_AuxCallbackTimestampsChrono[i + 1] - infoGrabber.m_AuxCallbackTimestampsChrono[i];
        }

        std::valarray<uint64_t> pureTimestampsHw(nPeriods+1);
        for (size_t i = 0; i < nPeriods+1; i++)
        {
            KYFG_DeviceDirectHardwareRead(infoGrabber.handle, 0x180, &(pureTimestampsHw[i]), sizeof(uint64_t));
            //KYFG_DeviceDirectHardwareRead(infoGrabber.handle, 0x180, &(pureTimestampsHw[i]), sizeof(uint32_t));
            //KYFG_DeviceDirectHardwareRead(infoGrabber.handle, 0x184, (uint32_t*)(&(pureTimestampsHw[i])) + 1, sizeof(uint32_t));
        }
        for (size_t i = 0; i < nPeriods+1; i++)
        {
            pureTimestampsHw[i] *= 8;
        }
        std::valarray<uint64_t> pureTimestampPeriodsHw(nPeriods);
        for (size_t i = 0; i < nPeriods; i++)
        {
            pureTimestampPeriodsHw[i] = pureTimestampsHw[i + 1] - pureTimestampsHw[i];
        }

        printStatistic(auxTimestampPeriodsHw,             "auxTimestampPeriodsHw             ");
        printStatistic(auxInterruptTimestampPeriodsHw,    "auxInterruptTimestampPeriodsHw    ");
        printStatistic(auxCallbackTimestampPeriodsHw,     "auxCallbackTimestampPeriodsHw     ");
        printStatistic(auxCallbackTimestampPeriodsChrono, "auxCallbackTimestampPeriodsChrono ");
        printStatistic(pureTimestampPeriodsHw,            "pureTimestampPeriodsHw            ");

        // Aux Latencies
        std::cout << "--------------------" << std::endl;
        std::valarray<uint64_t> auxLatenciesInterruptToFrameHW(infoGrabber.m_auxCallbackCounter);
        for (size_t i = 0; i < auxLatenciesInterruptToFrameHW.size(); i++)
        {
            auxLatenciesInterruptToFrameHW[i] = infoGrabber.m_AuxInterruptTimestampsHw[i] - infoGrabber.m_AuxTimestampsHw[i];
        }
        std::valarray<uint64_t> auxLatenciesCallbackToInterruptHW(infoGrabber.m_auxCallbackCounter);
        for (size_t i = 0; i < auxLatenciesCallbackToInterruptHW.size(); i++)
        {
            auxLatenciesCallbackToInterruptHW[i] = infoGrabber.m_AuxCallbackTimestampsHw[i] - infoGrabber.m_AuxInterruptTimestampsHw[i];
        }
        printStatistic(auxLatenciesInterruptToFrameHW,    "AUX Latencies (Interrupt - HwStamps) HW ");
        printStatistic(auxLatenciesCallbackToInterruptHW, "AUX Latencies (Callback - Interrupt) HW ");

        std::ofstream auxCsvFile;
        auxCsvFile.open("case_3632_aux.csv", std::ofstream::out | std::ofstream::trunc);
        auxCsvFile << "auxTimestampPeriodsHw" 
                   << "," << "auxInterruptTimestampPeriodsHw" 
                   << "," << "auxCallbackTimestampPeriodsHw" 
                   << "," << "pureTimestampPeriodsHw" 
                   << "\n";
        for (size_t i = 0; i < nPeriods; i++)
        {
            auxCsvFile << auxTimestampPeriodsHw[i] 
                       << "," << auxInterruptTimestampPeriodsHw[i] 
                       << "," << auxCallbackTimestampPeriodsHw[i] 
                       << "," << pureTimestampPeriodsHw[i]
                       << "\n";
        }
        auxCsvFile.close();


    }//if (infoGrabber.m_auxCallbackCounter >= 2)
    else
    {
        std::cout << "m_auxCallbackCounter: " << infoGrabber.m_auxCallbackCounter << " - not enough to print Aux statistics" << std::endl;
    }

    return 0;
}

void measureStreams()
{
    // Detect and connect camera(s)
    printf("\nCamera detection\n");

    for (FrameGrabberInfo &info : frameGrabberInfoList)
    {
        int nDetectedCameras = _countof(camHandleArray[info.index]);

        if (FGSTATUS_OK != KYFG_UpdateCameraList(info.handle, camHandleArray[info.index], &nDetectedCameras))
        {
            printf("Camera detect error. Please try again\n");
            continue;
        }

        printf("Number of cameras connected to the PCI device #%d: %d\n", info.index, nDetectedCameras);

        for (int i = 0; i < nDetectedCameras; ++i)
        {
            info.cameras.emplace_back(camHandleArray[info.index][i], INVALID_STREAMHANDLE);
            std::cout << "Camera found at port " << i << std::endl;
        }
    }//for (FrameGrabberInfo &info : frameGrabberInfoList) KYFG_UpdateCameraList

    printf("\nCamera connection:\n");

    for (FrameGrabberInfo &info : frameGrabberInfoList)
    {
        int cameraIndex = 0;
        for (CameraInfo &camera : info.cameras)
        {
            // Open a connection to the chosen camera
            if (FGSTATUS_OK == KYFG_CameraOpen2(camera.camHandle, NULL))
            {
                std::cout << "Camera 0x" << std::hex << camera.camHandle << " was connected successfully" << std::endl;

                auto cameraHandle = camera.camHandle;
                KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerMode", "Off");
                KYFG_SetCameraValueInt(cameraHandle, "Width", nWidth);
                KYFG_SetCameraValueInt(cameraHandle, "Height", nHeight);
                KYFG_SetCameraValueFloat(cameraHandle, "AcquisitionFrameRate", fFrameRate);
                KYFG_GetCameraValueStringCopy(cameraHandle, "PixelFormat", szPixelFormat, &sizePixelFormat);

                KYFGCAMERA_INFO2 camInfo;
                camInfo.version = 1;
                KYFG_CameraInfo2(cameraHandle, &camInfo);
                std::cout << "Model: " << camInfo.deviceModelName
                          << ", Vendor: " << camInfo.deviceVendorName
                          << ", link_mask: 0x" << std::hex << int(camInfo.link_mask)
                          << ", link_speed: 0x" << std::hex << camInfo.link_speed
                          << ", Width: " << std::dec << nWidth
                          << ", Height: " << std::dec << nHeight
                          << ", PixelFormat: " << szPixelFormat
                          << ", FrameRate: " << fFrameRate
                          << ", VersionUsed: 0x" << std::hex << KYFG_GetCameraValueInt(cameraHandle, "VersionUsed")
                          << std::dec << std::endl;

                // Create stream and assign appropriate runtime acquisition callback function
                KYFG_StreamCreate(cameraHandle, &camera.streamHandle, 0);
                KYFG_StreamBufferCallbackRegister(camera.streamHandle, Stream_callback_func, &camera);

                // Retrieve information about required frame buffer size and alignment 
                KYFG_StreamGetInfo(camera.streamHandle,
                    KY_STREAM_INFO_PAYLOAD_SIZE,
                    &g_frameDataSize,
                    NULL, NULL);

                KYFG_StreamGetInfo(camera.streamHandle,
                    KY_STREAM_INFO_BUF_ALIGNMENT,
                    &g_frameDataAligment,
                    NULL, NULL);

                // Allocate memory for desired number of frame buffers
                for (size_t iFrame = 0; iFrame < _countof(g_streamBufferHandle); iFrame++)
                {

#pragma message("Building with user allocated buffers")
                    pBuffer[cameraIndex][iFrame] = _aligned_malloc(g_frameDataSize, g_frameDataAligment);
                    KYFG_BufferAnnounce(camera.streamHandle,
                        pBuffer[cameraIndex][iFrame],
                        g_frameDataSize,
                        NULL,
                        &g_streamBufferHandle[iFrame]);
                }
                cameraIndex++;;
            }
            else
            {
                printf("Camera isn't connected\n");
                continue;
            }

        }
    }

    // Start stream
    for (FrameGrabberInfo &info : frameGrabberInfoList)
    {
        for (CameraInfo &camera : info.cameras)
        {
            StartCamera(camera);
        }
    }

    // Wait some time to collect statistical data
    std::chrono::seconds waitTime(nStreamsWaitTimeSec);
    std::this_thread::sleep_for(waitTime);

    // Stop measurements before calling KYFG_CameraStop() - 
    // callback arriving while Stop is in progress are not relevant for accurate mesurements
    bDoMeasurements = false;

    // Stop all streams
    for (FrameGrabberInfo &info : frameGrabberInfoList)
    {
        if (INVALID_FGHANDLE == info.handle)
        {
            continue;
        }
        for (CameraInfo &camera : info.cameras)
        {
            if (camera.camHandle)
            {
                KYFG_CameraStop(camera.camHandle);
            }
            // Discard last X measurements that could be collected during KYFG_CameraStop() operation
            camera.callbackCounter -= 5;
        }//for (CameraInfo &camera : info.cameras)
    }// for (FrameGrabberInfo &info : frameGrabberInfoList)

    std::cout << std::endl << "Collected stastics with "
        "streaming session " << nStreamsWaitTimeSec << " seconds"
        << ":" << std::endl;

    // Calculate and print statistics
    for (FrameGrabberInfo &info : frameGrabberInfoList)
    {
        if (INVALID_FGHANDLE == info.handle)
        {
            continue;
        }

        for (CameraInfo &camera : info.cameras)
        {
            if (camera.camHandle)
            {
                std::cout << std::endl << "Camera " << std::hex << camera.camHandle << std::dec << std::endl;

                std::cout << camera.callbackCounter << " callbacks processed" << std::endl;
                if (!(camera.callbackCounter > 0))
                {
                    continue;
                }

                // camera.m_CallbackTimestampsHw should be multiplied by 8 to convert them to nanoseconds
                for (int i = 0; i < camera.callbackCounter; i++)
                {
                    camera.m_CallbackTimestampsHw[i] *= 8;
                }

                // Calculate HW FrameTimestampPeriods
                std::valarray<uint64_t> frameTimestampPeriodsHw(camera.callbackCounter - 1);
                for (size_t i = 0; i < frameTimestampPeriodsHw.size(); i++)
                {
                    frameTimestampPeriodsHw[i] = camera.m_FrameTimestampsHw[i + 1] - camera.m_FrameTimestampsHw[i];
                }

                // Calculate Interrupts Timestamp Periods - both hardware and chrono
                std::valarray<uint64_t> interruptTimestampPeriodsHw(camera.callbackCounter - 1);
                for (size_t i = 0; i < interruptTimestampPeriodsHw.size(); i++)
                {
                    interruptTimestampPeriodsHw[i] = camera.m_InterruptTimestampsHw[i + 1] - camera.m_InterruptTimestampsHw[i];
                }
                std::valarray<uint64_t> interruptTimestampPeriodsChrono(camera.callbackCounter - 1);
                for (size_t i = 0; i < interruptTimestampPeriodsChrono.size(); i++)
                {
                    interruptTimestampPeriodsChrono[i] = camera.m_InterruptTimestampsChrono[i + 1] - camera.m_InterruptTimestampsChrono[i];
                }

                // Calculate Callbacks Timestamp Periods - both hardware and chrono
                std::valarray<uint64_t> callbackTimestampPeriodsHw(camera.callbackCounter - 1);
                for (size_t i = 0; i < callbackTimestampPeriodsHw.size(); i++)
                {
                    callbackTimestampPeriodsHw[i] = camera.m_CallbackTimestampsHw[i + 1] - camera.m_CallbackTimestampsHw[i];
                }
                std::valarray<uint64_t> callbackTimestampPeriodsChrono(camera.callbackCounter - 1);
                for (size_t i = 0; i < callbackTimestampPeriodsChrono.size(); i++)
                {
                    callbackTimestampPeriodsChrono[i] = camera.m_CallbackTimestampsChrono[i + 1] - camera.m_CallbackTimestampsChrono[i];
                }

                printStatistic(frameTimestampPeriodsHw, "Frame timestamp periods HW         ");
                printStatistic(interruptTimestampPeriodsHw, "Interrupt timestamp periods HW     ");
                printStatistic(callbackTimestampPeriodsHw, "Callback timestamp periods HW      ");
                printStatistic(callbackTimestampPeriodsChrono, "Callback timestamp periods Chrono  ");
                printStatistic(interruptTimestampPeriodsChrono, "Interrupt timestamp periods Chrono ");

                std::cout << "--------------------" << std::endl;
                std::valarray<uint64_t> latenciesInterruptToFrameHW(camera.callbackCounter);
                for (size_t i = 0; i < latenciesInterruptToFrameHW.size(); i++)
                {
                    latenciesInterruptToFrameHW[i] = camera.m_InterruptTimestampsHw[i] - camera.m_FrameTimestampsHw[i];
                }
                std::valarray<uint64_t> latenciesCallbackToInterruptHW(camera.callbackCounter);
                for (size_t i = 0; i < latenciesCallbackToInterruptHW.size(); i++)
                {
                    latenciesCallbackToInterruptHW[i] = camera.m_CallbackTimestampsHw[i] - camera.m_InterruptTimestampsHw[i];
                }
                std::valarray<uint64_t> latenciesCallbackToInterruptChrono(camera.callbackCounter);
                for (size_t i = 0; i < latenciesCallbackToInterruptChrono.size(); i++)
                {
                    latenciesCallbackToInterruptChrono[i] = camera.m_CallbackTimestampsChrono[i] - camera.m_InterruptTimestampsChrono[i];
                }
                printStatistic(latenciesInterruptToFrameHW,        "DMA Latencies (Interrupt - HwStamps) HW     ");
                printStatistic(latenciesCallbackToInterruptHW,     "DMA Latencies (Callback - Interrupt) HW     ");
                printStatistic(latenciesCallbackToInterruptChrono, "DMA Latencies (Callback - Interrupt) Chrono ");
                std::cout << "--------------------" << std::endl;

                auto cameraHandle = camera.camHandle;

                std::cout << "RXFrameCounter:    " << KYFG_GetGrabberValueInt(cameraHandle, "RXFrameCounter") << std::endl;
                std::cout << "DropFrameCounter:  " << KYFG_GetGrabberValueInt(cameraHandle, "DropFrameCounter") << std::endl;
                std::cout << "RXPacketCounter:   " << KYFG_GetGrabberValueInt(cameraHandle, "RXPacketCounter") << std::endl;
                std::cout << "DropPacketCounter: " << KYFG_GetGrabberValueInt(cameraHandle, "DropPacketCounter") << std::endl;

                std::cout << "CallbackCounter: " << camera.callbackCounter << std::endl;
            }
        }//for (CameraInfo &camera : info.cameras)
    }//for (FrameGrabberInfo &info : frameGrabberInfoList)

    std::cout << std::endl;
}//void measureStreams()

// Example of command line arguments:
// --aux_wait_time_sec 30 --stream_wait_time_sec 30 --callbacks_maxcount 10000 --attach_debugger 0
// (default values, see InputParser options initialization below)
int main(int argc, char **argv)
{
    InputParser options
    (
        {
            { "--attach_debugger",      "0"},      // do not wait for attaching debugger
            { "--device_index",         "0"},      // use 0-th PCI device
            { "--callbacks_maxcount",   "10000" }, // collect maximum 10000 callbacks
            { "--aux_wait_time_sec",    "30" },    // wait 30 seconds to collect AUX callbacks (for each grabber sequentially)
            { "--stream_wait_time_sec", "30" },    // wait 30 seconds to collect stream callbacks (for all grabbers and cameras, in parallel)
            { "--width",                "640" },   //
            { "--height",               "480" },   //
            { "--frameRate",            "60" },    //
        }
    );
    options.ReadOptions(argc, argv);
    options.PrintOptions();

    // "--attach_debugger"
    const std::string &sattachDebugger = options.getCmdOption("--attach_debugger");
    if (!sattachDebugger.empty())
    {
        attachDebugger = std::stoi(sattachDebugger);
    }
    if (attachDebugger)
    {
        printf("Input any char to continue (1-st chance to attach a debugger)\n"); 
        BlockingInput();
    }

    // "--device_index"
    const std::string &sdeviceIndex = options.getCmdOption("--device_index");
    if (!sdeviceIndex.empty())
    {
        device_index = std::stoi(sdeviceIndex);
    }

    // "--callbacks_maxcount"
    const std::string &sStreamCallbacksMaxCount = options.getCmdOption("--callbacks_maxcount");
    if (!sStreamCallbacksMaxCount.empty())
    {
        nCallbacksMaxCount = std::stoi(sStreamCallbacksMaxCount);
    }

    // "--aux_wait_time_sec"
    const std::string &sAuxWaitTimeSec = options.getCmdOption("--aux_wait_time_sec");
    if (!sAuxWaitTimeSec.empty())
    {
        nAuxWaitTimeSec = std::stoi(sAuxWaitTimeSec);
    }

    // "--stream_wait_time_sec"
    const std::string &sStreamWaitTimeSec = options.getCmdOption("--stream_wait_time_sec");
    if (!sStreamWaitTimeSec.empty())
    {
        nStreamsWaitTimeSec = std::stoi(sStreamWaitTimeSec);
    }

    // "--width"
    const std::string &sWidth = options.getCmdOption("--width");
    if (!sWidth.empty())
    {
        nWidth = std::stoi(sWidth);
    }

    // "--height"
    const std::string &sHeight = options.getCmdOption("--height");
    if (!sHeight.empty())
    {
        nHeight = std::stoi(sHeight);
    }

    // "--frameRate"
    const std::string &sFrameRate = options.getCmdOption("--frameRate");
    if (!sFrameRate.empty())
    {
        fFrameRate = std::stof(sFrameRate);
    }

    KYFGLib_InitParameters kyInit;

    // Initialize library

    memset((void*)(&kyInit), 0, sizeof(kyInit));
    kyInit.version = 2;
    kyInit.concurrency_mode = 0;
    kyInit.logging_mode = 0;
    kyInit.noVideoStreamProcess = KYFALSE;

    if(FGSTATUS_OK != KYFGLib_Initialize(&kyInit))
    {
        printf("Library initialization failed \n ");
        Close(-1);
    }

    for(int i = 0; i < KY_MAX_BOARDS; i++)
    {
        m_FgHandles[i] = INVALID_FGHANDLE;
    }

    // Scan for grabbers

    KY_DeviceScan(&nKayaDevicesCount);    // Retrieve the number of virtual and hardware devices connected to PC

    if(!nKayaDevicesCount)
    {
        printf("No PCI devices found\n");
        Close(0);
    }

    for(int i = 0; i < nKayaDevicesCount; i++)
    {
        std::string name(KY_DeviceDisplayName(i));
        KY_DEVICE_INFO deviceInfo;
        deviceInfo.version = 4;
        if (FGSTATUS_OK == KY_DeviceInfo(i, &deviceInfo) && i == device_index)
        {
            if (deviceInfo.m_Protocol == KY_DEVICE_PROTOCOL_CoaXPress)
            {
                frameGrabberInfoList.emplace_back(i, name, INVALID_FGHANDLE);
                std::cout << "FG detected. index=" << i << ", name=" << name << std::endl;
            
            }
        }
        
    }

    for (FrameGrabberInfo &info : frameGrabberInfoList)
    {
        ConnectToGrabber(info);
    }

    if (nStreamsWaitTimeSec > 0)
    {
        measureStreams();
    }
    else
    {
        std::cout << "nStreamsWaitTimeSec is not greater than 0, I will not measure streams" << std::endl;
    }

    // Close grabbers
    CloseGrabbers();
    //Close(0);
    exit(0);
} // int main(int argc, char **argv)
