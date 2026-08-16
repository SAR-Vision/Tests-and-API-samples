//-----------------------------------------------------------------//
// Name        | Cpco_log.cpp                | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS                                           //
//-----------------------------------------------------------------//
// Environment |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Logging class                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 16.06.2010 |  new file                              //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#include "stdafx.h"
#include "Cpco_log.h"

const char crlf[3]={0x0d,0x0a,0x00};

#define BUFSIZE 500
#define CP_EVENT_TIME 100

#ifdef PCO_LOGLIB
CPco_Log::CPco_Log(int logbits)
{
  hflog=NULL;
  QueryPerformanceFrequency((LARGE_INTEGER*)&lpFrequency);
  lpPCount1=lpPCount2=0;
  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount1);
  stamp1=stamp2=0;
  log_bits=logbits;
}
#else

CPco_Log::CPco_Log(const char *name)
{
#if defined _DEBUG
  log_bits=0x0003FFFF; //ERROR_M|INIT_M|INTERNAL_1_M|INTERNAL_2_M|TIME_M|TIME_MD;
#else
  log_bits=ERROR_M|INIT_M;
#endif

  hflog=NULL;
  cp_event=NULL;

  QueryPerformanceFrequency((LARGE_INTEGER*)&lpFrequency);
  lpPCount1=lpPCount2=0;
  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount1);
  stamp1=stamp2=0;

  if(name!=NULL)
  {
   strcpy(logname,name);
   lastpos=0;
   hflog=_open(logname,O_CREAT|O_WRONLY|O_TRUNC,0666);
   if(hflog!=-1)
   {
    char fname[MAX_PATH+100];
    SYSTEMTIME  st;
    DWORD z;

    cp_event=CreateEvent(NULL,TRUE,0,NULL);
    GetSystemTime(&st);
//    GetLocalTime(&st);
    sprintf(fname,"%s logfile started\r\n"
                 "%02d:%02d:%02d %02d.%02d.%04d",logname,st.wHour,st.wMinute,st.wSecond,st.wDay,st.wMonth,st.wYear);

    strcat(fname,crlf);
    strcat(fname,crlf);

    _lseek(hflog,0,SEEK_END);
    z=(DWORD)strlen(fname);
    _write(hflog,fname,z);
    SetEvent(cp_event);
   }
   else
    hflog=NULL;
  }
}
#endif

CPco_Log::~CPco_Log()
{
  if(hflog)
  {
   char fname[MAX_PATH+100];
   SYSTEMTIME  st;
   DWORD z;

   GetSystemTime(&st);
//   GetLocalTime(&st);
   sprintf(fname,"Log ended %02d:%02d.%02d %02d.%02d.%04d"
           ,st.wHour,st.wMinute,st.wSecond,st.wDay,st.wMonth,st.wYear);
   strcat(fname,crlf);
   strcat(fname,crlf);

   _lseek(hflog,0,SEEK_END);
   z=(DWORD)strlen(fname);
   _write(hflog,fname,z);

   _commit(hflog);
   _close(hflog);
   hflog=NULL;

   char newname[500];
   sprintf(newname,"cp_%02d_%02d_%02d_%s",st.wHour,st.wMinute,st.wSecond,logname);
   CopyFile(logname,newname,FALSE);
  }
  if(cp_event)
  {
   CloseHandle(cp_event);
   cp_event=NULL;
  }

}


void CPco_Log::writelog(DWORD lev,const char *str,...)
{
  va_list arg;

  if(lev==STDOUT_M)
  {
   fprintf(stdout,"\n");
   return;
  }

  if(cp_event)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(cp_event,CP_EVENT_TIME);
   if(waitstat!=WAIT_OBJECT_0)
   {
    _RPT0(_CRT_WARN,"PCO copy event is in reset state\n");
    return;
   }
  }

  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount2);

  if(lev==0)
   lev+=ERROR_M;
  if(lev&log_bits)
  {
   char buf[BUFSIZE];

   sprintf(buf,"\0");

#ifdef PCO_LOGLIB
   va_start(arg,str);
   vsprintf(buf,str,arg);
   va_end(arg);
   ::writelog(lev,HANDLE(NULL),buf);
#else
   SYSTEMTIME  st;
   DWORD z;
   z=0;

   if(log_bits&TIME_M)
   {
//    GetLocalTime(&st);
    GetSystemTime(&st);
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
   }
   double time;
   time=(double)(lpPCount2-lpPCount1);
   lpPCount1=lpPCount2;

   time=time/lpFrequency;
   time*=1000; //ms
   if(log_bits&TIME_MD)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%6d.%03dms ",(int)time%1000000,(int)((time-(int)time)*1000));
   }

   if(log_bits&THREAD_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"0x%04x ",GetCurrentThreadId());
   }

   if(log_bits&CPU_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d ",GetCurrentProcessorNumber());
   }

   if(lev&ERROR_M)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"ERROR ");
   }

   va_start(arg,str);
   z=(DWORD)strlen(buf);
   vsprintf(buf+z,str,arg);
   va_end(arg);

   if(lev&STDOUT_M)
    fprintf(stdout,"%s\n",buf);

   if(log_bits&DBGOUT_M)
    _RPT1(_CRT_WARN,"PCO Debug  %s\n",buf);


   if(hflog)
   {
    strcat(buf,crlf);
    _lseek(hflog,0,SEEK_END);
    z=(DWORD)strlen(buf);
    _write(hflog,buf,z);
   }


#endif
  }
}

void CPco_Log::writelog(DWORD lev,const char *str,va_list args)
{
  if(lev==STDOUT_M)
  {
   fprintf(stdout,"\n");
   return;
  }

  if(cp_event)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(cp_event,CP_EVENT_TIME);
   if(waitstat!=WAIT_OBJECT_0)
   {
    _RPT0(_CRT_WARN,"PCO copy event is in reset state\n");
    return;
   }
  }

  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount2);


  if(lev==0)
   lev+=ERROR_M;
  if(lev&log_bits)
  {
   char buf[BUFSIZE];
//   memset(buf,0,BUFSIZE);
   sprintf(buf,"\0");

#ifdef PCO_LOGLIB
   vsprintf(buf,str,args);
   ::writelog(lev,hdriver,buf);
#else
   SYSTEMTIME  st;
   DWORD z;

   z=0;

   if(log_bits&TIME_M)
   {
    GetSystemTime(&st);
//    GetLocalTime(&st);
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
   }
   double time;
   time=(double)(lpPCount2-lpPCount1);
   lpPCount1=lpPCount2;

   time=time/lpFrequency;
   time*=1000; //ms
   if(log_bits&TIME_MD)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%6d.%03dms ",(int)time%1000000,(int)((time-(int)time)*1000));
   }

   if(log_bits&THREAD_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"0x%04x ",GetCurrentThreadId());
   }

   if(log_bits&CPU_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d ",GetCurrentProcessorNumber());
   }

   if(lev&ERROR_M)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"ERROR ");
   }

   z=(DWORD)strlen(buf);
   vsprintf(buf+z,str,args);

   if(lev&STDOUT_M)
    fprintf(stdout,"%s\n",buf);

   if(log_bits&DBGOUT_M)
    _RPT1(_CRT_WARN,"PCO Debug  %s\n",buf);


   if(hflog)
   {
    strcat(buf,crlf);
    _lseek(hflog,0,SEEK_END);
    z=(DWORD)strlen(buf);
    _write(hflog,buf,z);
   }
#endif
  }
}


void CPco_Log::writelog(DWORD lev,HANDLE hdriver,const char *str,...)
{
  va_list arg;

  if(lev==STDOUT_M)
  {
   fprintf(stdout,"\n");
   return;
  }

  if(cp_event)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(cp_event,CP_EVENT_TIME);
   if(waitstat!=WAIT_OBJECT_0)
   {
    _RPT0(_CRT_WARN,"PCO copy event is in reset state\n");
    return;
   }
  }

  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount2);


  if(lev==0)
   lev+=ERROR_M;
  if(lev&log_bits)
  {
   char buf[BUFSIZE];
//   memset(buf,0,BUFSIZE);
   sprintf(buf,"\0");

#ifdef PCO_LOGLIB
   va_start(arg,str);
   vsprintf(buf,str,arg);
   va_end(arg);
   ::writelog(lev,hdriver,buf);
#else
   SYSTEMTIME  st;
   DWORD z;
   z=0;

   if(log_bits&TIME_M)
   {
//    GetLocalTime(&st);
    GetSystemTime(&st);
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
   }
   double time;
   time=(double)(lpPCount2-lpPCount1);
   lpPCount1=lpPCount2;

   time=time/lpFrequency;
   time*=1000; //ms
   if(log_bits&TIME_MD)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%6d.%03dms ",(int)time%1000000,(int)((time-(int)time)*1000));
   }

   z=(DWORD)strlen(buf);
   sprintf(buf+z,"0x%04x ",(DWORD)hdriver);

   if(log_bits&THREAD_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"0x%04x ",GetCurrentThreadId());
   }

   if(log_bits&CPU_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d ",GetCurrentProcessorNumber());
   }


   if(lev&ERROR_M)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"ERROR ");
   }

   va_start(arg,str);
   z=(DWORD)strlen(buf);
   vsprintf(buf+z,str,arg);
   va_end(arg);

   if(lev&STDOUT_M)
    fprintf(stdout,"%s\n",buf);

   if(log_bits&DBGOUT_M)
    _RPT1(_CRT_WARN,"PCO Debug  %s\n",buf);


   if(hflog)
   {
    strcat(buf,crlf);
    _lseek(hflog,0,SEEK_END);
    z=(DWORD)strlen(buf);
    _write(hflog,buf,z);
   }
#endif
  }
}

void CPco_Log::writelog(DWORD lev,HANDLE hdriver,const char *str,va_list args)
{

  if(lev==STDOUT_M)
  {
   fprintf(stdout,"\n");
   return;
  }

  if(cp_event)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(cp_event,CP_EVENT_TIME);
   if(waitstat!=WAIT_OBJECT_0)
   {
    _RPT0(_CRT_WARN,"PCO copy event is in reset state\n");
    return;
   }
  }

  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount2);


  if(lev==0)
   lev+=ERROR_M;
  if(lev&log_bits)
  {
   char buf[BUFSIZE];
//   memset(buf,0,BUFSIZE);
   sprintf(buf,"\0");

#ifdef PCO_LOGLIB
   vsprintf(buf,str,args);
   ::writelog(lev,hdriver,buf);
#else
   SYSTEMTIME  st;
   DWORD z;

   z=0;

   if(log_bits&TIME_M)
   {
//    GetLocalTime(&st);
    GetSystemTime(&st);
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
   }
   double time;
   time=(double)(lpPCount2-lpPCount1);
   lpPCount1=lpPCount2;

   time=time/lpFrequency;
   time*=1000; //ms
   if(log_bits&TIME_MD)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%6d.%03dms ",(int)time%1000000,(int)((time-(int)time)*1000));
   }

   z=(DWORD)strlen(buf);
   sprintf(buf+z,"0x%04x ",(DWORD)hdriver);

   if(log_bits&THREAD_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"0x%04x ",GetCurrentThreadId());
   }

   if(log_bits&CPU_ID)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"%02d ",GetCurrentProcessorNumber());
   }


   if(lev&ERROR_M)
   {
    z=(DWORD)strlen(buf);
    sprintf(buf+z,"ERROR ");
   }

   z=(DWORD)strlen(buf);
   vsprintf(buf+z,str,args);

   if(lev&STDOUT_M)
    fprintf(stdout,"%s\n",buf);

   if(log_bits&DBGOUT_M)
    _RPT1(_CRT_WARN,"PCO Debug  %s\n",buf);


   if(hflog)
   {
    strcat(buf,crlf);
    _lseek(hflog,0,SEEK_END);
    z=(DWORD)strlen(buf);
    _write(hflog,buf,z);
   }
#endif
  }
}


void CPco_Log::set_logbits(DWORD log)
{
  log_bits=log;
  if(log&DBGOUT_M)
   _CrtSetReportMode(_CRT_WARN,_CRTDBG_MODE_DEBUG);

}

DWORD CPco_Log::get_logbits(void)
{
  return log_bits;
}

void CPco_Log::flushlog()
{
  if(hflog)
  {
   DWORD pos;
   pos=_tell(hflog);
   if(pos>(lastpos+64*1024))
   {
    _commit(hflog);
    lastpos=pos;
    _RPT0(_CRT_WARN,"PCO flushlog _commit\n");
   }
   if(pos>(4*1024*1024))
   {
    SYSTEMTIME  st;
    char newname[500];

//we might lose some messages during copy
//we reset the event here so all waiting functions will return Timeout and logging is stopped
    if(cp_event)
    {
     ResetEvent(cp_event);
    }

    _RPT0(_CRT_WARN,"PCO copy file entered\n");

    _commit(hflog);
    _close(hflog);
    GetSystemTime(&st);
    sprintf(newname,"cp_%02d_%02d_%02d_%s",st.wHour,st.wMinute,st.wSecond,logname);
    CopyFile(logname,newname,FALSE);

    hflog=_open(logname,O_CREAT|O_WRONLY|O_TRUNC,0666);
    if(hflog!=-1)
    {
     DWORD z;
     sprintf(newname,"%s logfile continued\r\n"
                 "%02d:%02d:%02d %02d.%02d.%04d",logname,st.wHour,st.wMinute,st.wSecond,st.wDay,st.wMonth,st.wYear);

     strcat(newname,crlf);
     strcat(newname,crlf);

     _lseek(hflog,0,SEEK_END);
      z=(DWORD)strlen(newname);
      _write(hflog,newname,z);
     lastpos=0;
    }
    else
     hflog=NULL;
//we set the event her so all logging will be done
    SetEvent(cp_event);
    _RPT0(_CRT_WARN,"PCO copy file done\n");
   }
  }
}

void CPco_Log::start_time_mess(void)
{
  QueryPerformanceCounter((LARGE_INTEGER*)&stamp1);
}

double CPco_Log::stop_time_mess(void)
{
  double time;
  QueryPerformanceCounter((LARGE_INTEGER*)&stamp2);
  time=(double)(stamp2-stamp1);
  time=time/lpFrequency;
  time*=1000; //ms
  return time;
}

