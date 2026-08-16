//-----------------------------------------------------------------//
// Name        | Cpco_log.h                  | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    |                                                   //
//-----------------------------------------------------------------//
// Environment |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Logging class                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 |  from Cpco_cl_com.cpp                  //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//


#if !defined (MAX_PATH)
#define MAX_PATH 1024
#endif

#ifndef PCOLOG_H
#define PCOLOG_H

#ifdef PCO_LOGLIB
#include "C:\PCO_Include\SrcWin\App\Common\log_func.h"

// #error ("halt wegen LOGBUFSIZE")
// extern void writelog(DWORD lev,HANDLE hdriver,LPTSTR str,...);
#endif

class CPco_Log
{
 char logname[MAX_PATH];

 public:

#ifdef PCO_LOGLIB
  CPco_Log(int logbits=0);
#else
  CPco_Log(const char *name=NULL);
#endif
  ~CPco_Log();
  void writelog(DWORD lev,const char *str,...);
  void writelog(DWORD lev,const char *str,va_list args);
  void writelog(DWORD lev,HANDLE hdriver,const char *str,...);
  void writelog(DWORD lev,HANDLE hdriver,const char *str,va_list args);
  void set_logbits(DWORD log);
  DWORD get_logbits(void);
  void flushlog();
  void start_time_mess(void);
  double stop_time_mess(void);

protected:
  int hflog;
  DWORD log_bits;
  __int64 lpFrequency,lpPCount1,lpPCount2,lpPCount_p;
  __int64 stamp1,stamp2;
  DWORD lastpos;
  HANDLE cp_event;

};


#ifndef LOG_LEVEL_DEF
#define LOG_LEVEL_DEF
//loglevels for interface dll
#define ERROR_M      0x00000001
#define INIT_M       0x00000002
#define BUFFER_M     0x00000004
#define PROCESS_M    0x00000008

#define COC_M        0x00000010
#define INFO_M       0x00000020
#define COMMAND_M    0x00000040
#define PCI_M        0x00000080

#define HANDLE_M     0x00000800

#define TIME_M       0x00001000
#define TIME_MD      0x00002000
#define THREAD_ID    0x00004000
#define CPU_ID       0x00008000


#define INTERNAL_1_M 0x00010000
#define INTERNAL_2_M 0x00020000
#define INTERNAL_3_M 0x00040000
#define INTERNAL_4_M 0x00080000
#define INTERNAL_L_M 0x00100000


#define STDOUT_M     0x01000000
#define DBGOUT_M     0x02000000

#else
#define STDOUT_M     0x01000000
#endif






#endif
