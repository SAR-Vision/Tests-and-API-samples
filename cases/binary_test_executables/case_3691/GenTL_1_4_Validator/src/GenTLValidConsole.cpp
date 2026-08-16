// GenTLValidCon12.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <map>
#include <tchar.h>
#include "GenApi/GenApi.h"

#include "GenTLValidDll.h"
#include "GenTLTestTools.h"

typedef std::map<std::string, std::string> tParameterRamp;

extern INT64 g_zSpecialTestCaseToDoFrom;
extern UINT8 g_bStopValidationFlag;
extern UINT8 g_bCreateTestEnumerationFlag;
extern UINT8 g_bTLNotCompliant;

void vPrintUsage()
{
    std::cout << "gentlvalidcon14 <cti filename>" << std::endl;
    std::cout << "    /r <testnumber range> (e.g.: 10 or 10- or -10 or 10-20, e.g. '/r 10-20')." << std::endl;
    std::cout << "    /o <output directory> (e.g.: '/o C:\\')." << std::endl;
    std::cout << "    /nodatastream : without all datastream tests." << std::endl;
    std::cout << "    /chunkdata : enables chunkdata functional tests." << std::endl;
    std::cout << "    /log activate TL interface logging" << std::endl;
    std::cout << "    /notesttimeout deactivate timeout of each test (debugging)" << std::endl;
    std::cout << "    /e creates a test enumeration file" << std::endl;
    std::cout << "    /? help" << std::endl;
    std::cout << "    e.g. : gentlvalidcon14 C:\\gentl.cti /log /r 10-20" << std::endl;
}

INT8 nCheckCommandParameter(int &index, tParameterRamp &mapParameterRamp, int argc, _TCHAR* argv[])
{
    INT8 nResult=-1;
    std::string csCommand=argv[index];

    if ((index+1) < argc)
    {
        std::string csCommandParameter=argv[index+1];
        if (csCommandParameter[0] != '/')
        {
            mapParameterRamp[csCommand] = csCommandParameter;
            nResult = 0;
            index++;
        }
        else
        {
            mapParameterRamp.clear();
        }
    }
    else
    {
        mapParameterRamp.clear();
    }

    return nResult;
}

tParameterRamp mapGetParameterRamp(int argc, _TCHAR* argv[])
{
    tParameterRamp mapParameterRamp;

    if (argc > 1)
    {
        for (int index=1; index<argc; index++)
        {
            std::string csCommand=argv[index];
            if (csCommand == "/r" || csCommand == "/o")
            {
                if (nCheckCommandParameter(index, mapParameterRamp, argc, argv) != 0)
                {
                    mapParameterRamp.clear();
                    break;
                }
            }
            else
            {
                mapParameterRamp[csCommand] = "";
            }
        }
    }

    return mapParameterRamp;
}

INT8 nCheckRange(std::string csRange, INT64 &nRangeFrom, INT64 &nRangeTo)
{
    INT8 nResult=ERROR_SUCCESS;
    
    std::string cstrFrom;
    std::string cstrTo;
    size_t nPos=csRange.find('-', 0);

    nRangeFrom=ALL_TEST_CASES;
    nRangeTo=ALL_TEST_CASES;

    if (csRange != "")
    {
        if (nPos != -1)
        {
            cstrFrom = csRange.substr(0, nPos);
            cstrTo = csRange.substr(nPos+1, csRange.size()-nPos-1);
            if (cstrFrom != "" || cstrTo != "")
            {
                if (cstrFrom == "")
                    nRangeFrom = TEST_CASES_BORDER;
                else 
                    nRangeFrom = _atoi64(cstrFrom.c_str());
                
                if (cstrTo == "")
                    nRangeTo = TEST_CASES_BORDER;
                else 
                    nRangeTo = _atoi64(cstrTo.c_str());
            }
        }
        else
        {
            cstrFrom = csRange;
            
            nRangeFrom = _atoi64(cstrFrom.c_str());
            nRangeTo = nRangeFrom;
        }
    }
    else
    {
        nRangeFrom = ALL_TEST_CASES;
        nRangeTo = ALL_TEST_CASES;
    }

    // validate values
    if ((nRangeFrom != ALL_TEST_CASES && nRangeTo != ALL_TEST_CASES) &&
        (nRangeFrom != TEST_CASES_BORDER && nRangeTo != TEST_CASES_BORDER) &&
        (nRangeFrom > 0 && nRangeTo > 0 && nRangeTo < nRangeFrom))
    {
        nResult = ERROR_BAD_FORMAT;
    }

    return nResult;
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc > 1)
    {
        tParameterRamp mapParameterRamp=mapGetParameterRamp(argc, argv);
        tParameterRamp::iterator xIter;
        if (mapParameterRamp.size() >= 1)
        {
            for (xIter=mapParameterRamp.begin(); xIter!=mapParameterRamp.end(); xIter++)
            {
                if (xIter->first == "/?")
                {
                    vPrintUsage();
                    return g_bTLNotCompliant;
                } else if (xIter->first == "/r")
                {
                    INT64 nRangeFrom=ALL_TEST_CASES;
                    INT64 nRangeTo=ALL_TEST_CASES;
                    if (nCheckRange(xIter->second, nRangeFrom, nRangeTo) == ERROR_SUCCESS)
                        vSetDLLSpecialTestCaseRange(nRangeFrom, nRangeTo);
                    else
                    {
                        std::cout << "Invalid parameter '/r " << xIter->second.c_str() << "'" << std::endl;
                        return g_bTLNotCompliant;
                    }
                } else if (xIter->first == "/o")
                {
                    vSetDLLOutputDirectory(xIter->second.c_str());
                } else if (xIter->first == "/nodatastream")
                {
                    vSetDLLTestDatastream(false);
                } else if (xIter->first == "/chunkdata")
                {
                    vSetDLLTestChunkData(true);
                } else if (xIter->first == "/notesttimeout")
                {
                    vSetDLLTestTimeout(false);
                } else if (xIter->first == "/log")
                {
                    vSetDLLCTIInterfaceLog(true);
                } else if (xIter->first == "/e")
                {
                    vCreateTestEnumeration();
                } else if (xIter->first[0] != '/')
                {
                    vSetTLFile(xIter->first.c_str());
                }
            }
            if (!g_bCreateTestEnumerationFlag)
                vRunTLTest();
        }
        else
        {
            std::cout << "Invalid parameter." << std::endl;
            vPrintUsage();
        }
    }
    else
    {
        vPrintUsage();
    }

    if (g_zSpecialTestCaseToDoFrom != ALL_TEST_CASES || g_bStopValidationFlag != 0 || g_bCreateTestEnumerationFlag != 0)
        g_bTLNotCompliant = 1;

    return g_bTLNotCompliant;
}

