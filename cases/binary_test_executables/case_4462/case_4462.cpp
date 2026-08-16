/*****************************************************************//**
 * \file   case_4462.cpp
 * \brief  
 * 
 * \author maximn
 * \date   July 2024
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

uint64_t    number_of_tests = 0;
uint8_t     device_index;
uint8_t     isHelp;
FGHANDLE    grabberHandle;
KYBOOL      isUnattended;

struct ValuesArray : public std::valarray<uint64_t>
{
    ValuesArray()
        :std::valarray<uint64_t>(number_of_tests)
    {
    }
};

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


void printStatistic(std::valarray<uint64_t> &periodsArray, const std::string& name)
{
    //std::valarray<uint64_t> periodsArray(_periodsArray);

    std::ios oldState(nullptr);
    oldState.copyfmt(std::cout);
    std::cout.imbue(std::locale(""));
    static const std::streamsize streamWidth = 11;
    static char chFill = ' ';

    std::cout << name
        << " min " << std::setfill(chFill) << std::setw(streamWidth) << periodsArray.min()
        << ", max " << std::setfill(chFill) << std::setw(streamWidth) << periodsArray.max()
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
    std::cout << ", median " << std::setfill(chFill) << std::setw(streamWidth) << nMedianTime
        << ", average " << std::setfill(chFill) << std::setw(streamWidth) << (periodsArray.sum() / size);

    std::cout << std::endl;
    std::cout.copyfmt(oldState);
}

int printHelp()
{
    std::cout << "Available parameters:" << std::endl;
    std::cout << "--unattended:         0-Disable 1-Enable; " << std::endl;
    std::cout << "--device_index:       Detect grabber index if unattended mode enable" << std::endl;
    std::cout << "--number_of_tests:    Number of function calls" << std::endl;
    std::cout << "--help 1:             print help" << std::endl;
    return 0;
}
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
void Close(int returnCode)
{
    if (!isUnattended)
    {
        printf("Input any char to exit\n");
        BlockingInput();
    }
    
    exit(returnCode);
}
int ConnectToGrabber(unsigned int grabberIndex)
{

    if ((grabberHandle = KYFG_Open(grabberIndex)) != -1) // Connect to selected device
    {
        printf("Good connection to grabber #%d, m_FgHandles=%X\n", grabberIndex, grabberHandle);
    }
    else
    {
        printf("Could not connect to grabber #%d\n", grabberIndex);
        BlockingInput();
        return 0;
    }
    return 1;
}
void NumberInRangeInput_uint64(int min, int max, uint64_t* value, const char* error_str)
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

int main(int argc, char **argv)
{
    
    KYFGLib_InitParameters kyInit;
    
    InputParser options
    (
        {
            { "--unattended",       "0"     },      
            { "--device_index",     "0"     },      
            { "--number_of_tests",  "10000" }, 
            { "--help",             "0"     },    
        }
    );
    options.ReadOptions(argc, argv);
    options.PrintOptions();
    const std::string &sunattended = options.getCmdOption("--unattended");
    if (!sunattended.empty())
    {
        isUnattended = std::stoi(sunattended);
    }
    const std::string &sdeviceIndex = options.getCmdOption("--device_index");
    if (!sdeviceIndex.empty())
    {
        device_index = std::stoi(sdeviceIndex);
    }
    const std::string &sNumOfTests = options.getCmdOption("--number_of_tests");
    if (!sNumOfTests.empty())
    {
        number_of_tests = std::stoi(sNumOfTests);
    }
    const std::string &sIsHelp = options.getCmdOption("--help");
    if (!sIsHelp.empty())
    {
        isHelp = std::stoi(sIsHelp);
    }
    if (isHelp)
    {
        printHelp();
        return 0;
    }
    
    // Initialize library

    memset(&kyInit, 0, sizeof(kyInit));
    kyInit.version = 2;
    kyInit.concurrency_mode = 0;

    int grabberIndex = 0;
    kyInit.logging_mode = 0;
    kyInit.noVideoStreamProcess = KYFALSE;
    char c = 0;

    if (FGSTATUS_OK != KYFGLib_Initialize(&kyInit))
    {
        printf("Library initialization failed \n ");
        Close(-1);
    }
    int nKayaDevicesCount = 0;
    KY_DeviceScan(&nKayaDevicesCount);    // Retrieve the number of virtual and hardware devices connected to PC
    if (!nKayaDevicesCount)
    {
        printf("No PCI devices found\n");
        Close(0);
    }
    for (int i = 0; i < nKayaDevicesCount; i++)
    {
        KY_DEVICE_INFO grabberInfo;
        memset(&grabberInfo, 0, sizeof(KY_DEVICE_INFO));
        if (FGSTATUS_OK != KY_DeviceInfo(i, &grabberInfo))
        {
            printf("Wasn't able to retrive information from device #%d\n", i);
            continue;
        }
        else
        {
            printf("[%d] %s on PCI slot {%d:%d:%d}: Protocol 0x%X, Generation %d\n",
                i,
                grabberInfo.szDeviceDisplayName,
                grabberInfo.nBus,
                grabberInfo.nSlot,
                grabberInfo.nFunction,
                grabberInfo.m_Protocol,
                grabberInfo.DeviceGeneration);
        }
    }
    KYBOOL inputValid = KYFALSE;
    if (!isUnattended)
    {
        while (!inputValid)
        {
            NumberInRangeInput(0, nKayaDevicesCount - 1, &grabberIndex, "Invalid index\n");

            printf("Selected grabber #%d\n", grabberIndex);
            inputValid = KYTRUE;
            break;
        }
    }
    if (!ConnectToGrabber(grabberIndex))
    {
        std::cout << "Fail to open Grabber" << std::endl;
        Close(-1);
    }
    if (!number_of_tests)
    {
        number_of_tests = 1000;
    }
    uint8_t value[4] = { 1,1,1,1 };
    uint32_t size = sizeof(char) * 4;
    std::valarray<uint64_t> periodsArray(number_of_tests);
    std::valarray<uint64_t> directPeriodsArray(number_of_tests);

    for (int i = 0; i < number_of_tests; i++)
    {
        auto before = std::chrono::steady_clock::now();
        KYFG_GrabberWriteReg(grabberHandle, 0xA0000000 + 0x00000018, value, &size);
        auto after = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count();
        periodsArray[i] = duration;
    }
    

    printStatistic(periodsArray, "WriteReg timestamps           ");
    
    for (int i = 0; i < number_of_tests; i++)
    {
        auto before = std::chrono::steady_clock::now();
        KYFG_DeviceDirectHardwareWrite(grabberHandle, 0x000018, value, size); //FGHANDLE handle, uint64_t address, const void* pBuffer, uint32_t bytes
        auto after = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count();
        directPeriodsArray[i] = duration;
    }
    printStatistic(directPeriodsArray, "DirecWrite timestamps         ");
    KYFG_Close(grabberHandle);

    Close(0);
}