/************************************************************************ 
*  File: KYFGLib_grab_single_nt.cpp
*  Sample Frame Grabber API application
*
*  purpose:
*  purpose:
*   setup grabber, setup camera to default
*   dual channel camera 
*   use 16 Buffers 
*   grab single images, start acquisition, start camera, wait for imagestop acquisition, stop camera 
*
*  use first grabber found
*  can use commandline parameters
*  no user input required for initial loops
*
*  Excelitas PCO GmbH (2023)
*************************************************************************/

#include "stdafx.h"

#include "KYFGLib.h"

#include "Cpco_log.h"



#if !defined(_countof)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#define MAXBOARDS 4
#define MAXCAM    4
#define MAXBUF    2
#define MAXSTREAMBUF 16

#define PT_MONO8        0x01080001
#define PT_MONO8S       0x01080002
#define PT_MONO10       0x01100003
#define PT_MONO12       0x01100005
#define PT_MONO14       0x01100025
#define PT_MONO16       0x01100007

#define PT_MONO10P      0x010A0046
#define PT_MONO12P      0x010C0047
#define PT_MONO14P      0x010E0FF0 //??? nicht pnfc


#define PT_BAYERGB8     0x0108000A
#define PT_BAYERGB10    0x0110000E 
#define PT_BAYERGB12    0x01100012 
#define PT_BAYERGB14    0x01100FF1 //??? nicht pnfc

#define PT_BAYERGB10P   0x010A0054
#define PT_BAYERGB12P   0x010C0059
#define PT_BAYERGB14P   0x010E0FF2 //??? nicht pnfc



int get_yesno(BOOL *val,char *txt);

typedef struct
{
  std::queue <STREAM_BUFFER_HANDLE> qbuf_queue;
  CRITICAL_SECTION qbuf_queue_crisec;
  HANDLE picevent;
}Callback_par;


void streambuffer_callback(STREAM_BUFFER_HANDLE streamBufferHandle, void* userContext)
{
  Callback_par* par=(Callback_par*)userContext;
  if (0 == streamBufferHandle)                                                // callback with indicator for acquisition stop
  {
    printf("callback 0 ");
    EnterCriticalSection(&par->qbuf_queue_crisec);
    while (!par->qbuf_queue.empty())
      par->qbuf_queue.pop();
    LeaveCriticalSection(&par->qbuf_queue_crisec);
    return;
  }
  printf("callback 0x%016llx ", streamBufferHandle);
  EnterCriticalSection(&par->qbuf_queue_crisec);
  par->qbuf_queue.push(streamBufferHandle);
  LeaveCriticalSection(&par->qbuf_queue_crisec);

  if (par->picevent)
    SetEvent(par->picevent);
}


int main(int argc, char* argv[])
{
  unsigned int* info = 0;
  unsigned int grabber_count;
  unsigned int infosize,grabberIndex,cameraIndex;

  BOOL x;
  int64_t dmadQueuedBufferCapable;

  WORD recstate;
  WORD width,height;
  int  bitpix;

  DWORD err;
  DWORD errcount;

  FGHANDLE fghandle;
  int detectedCameras;

  CAMHANDLE camHandleArray[MAXCAM];
  STREAM_HANDLE streamHandle = 0;
  STREAM_BUFFER_HANDLE streamBufferHandles[MAXSTREAMBUF];
  STREAM_BUFFER_HANDLE streamBufferHandle=0;

  WORD camtype;
  DWORD serialnumber;

  FGSTATUS fg_status;
  int lc;
  uint64_t val;

  int loop=1;                     //parameter -I
  int imagecount=100000;          //parameter -C
  int gnum=0;                     //parameter -B
  int loglevel=0x0300FFFF;        //parameter -L
  int iteration = 1000;

  loglevel&=~BUFFER_M;
//  loglevel&=~COMMAND_M;
  loglevel&=~DBGOUT_M;

  CPco_Log *applog;
  DWORD waitstat;
  std::chrono::high_resolution_clock::time_point tp_ima1, tp_ima2, tp_start;
  std::chrono::duration<double, std::ratio<1, 1000000>> usec;

  COORD dwSize = {200, 600};
  HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
  ::SetConsoleScreenBufferSize(hConsole, dwSize);

  HWND hwin=GetConsoleWindow();
  RECT rect; 
  ::GetWindowRect(hwin,&rect);
  if((rect.bottom-rect.top)<600) 
   rect.bottom=rect.top+600;
  if((rect.right-rect.left)<1200)
   rect.right=rect.left+1200;
  ::SetWindowPos(hwin,NULL,0,0,rect.right-rect.left,rect.bottom-rect.top,SWP_NOZORDER|SWP_NOMOVE);
  ::GetWindowRect(hwin,&rect);

  Callback_par param;
  InitializeCriticalSection(&param.qbuf_queue_crisec);
  param.picevent = CreateEvent(NULL, TRUE, FALSE, NULL);

  fghandle=0;
  detectedCameras=0;
  for(int cam=0;cam<MAXCAM;cam++)
   camHandleArray[cam]=0;

  for(int i=0;i< MAXSTREAMBUF;i++)
    streamBufferHandles[i]=0;

  char c = 0;

  for(int i=1;i<argc;i++)
  {
   char *b;
   b=strchr(argv[i],'-'); 

   if(b!=NULL)
   {
    _strupr(b);
    if(b[1]=='B')
    {
     gnum = strtol(b+2,NULL,0);
    }
    if(b[1]=='I')
    {
     loop=strtol(b+2,NULL,0);
     if(loop<1)
      loop=1;
    }
    if(b[1]=='C')
    {
     imagecount=strtol(b+2,NULL,0);
     if(imagecount<10)
      imagecount=10;
    }
    if(b[1]=='L')
    {
     loglevel=strtol(b+2,NULL,0);
    }
    if(b[1]=='H')
    {
     printf("usage: %s parameters \n",argv[0]);
     printf("parameters:\n");
     printf("-B[0 ... 4]      open board number         default: 0\n");
     printf("-I[1 ... ]       loop count                default: 1\n");
     printf("-C[10 ... ]      image count               default: 150\n");
     printf("-L[0x...]        loglevel                  default: 0x0300FFFF\n");
     printf("-H               this message. Any key CR to close\n\n");
     getchar();
     return 0;
    }
   }
  }

  if(gnum>=0)
   printf("Use grabber %d\n",gnum);
  else
   printf("Use first grabber found\n");

  infosize = KYFG_Scan(0,0);   // First scan for device to retrieve the number of
                               // virtual and hardware devices connected to PC
  if((info= (unsigned int*)malloc(sizeof(unsigned int)*infosize)) != 0)
  {
   infosize = KYFG_Scan(info,infosize);   // Scans for frame grabbers currently connected to PC. Returns array with each ones pid
  }
  else
  {
   printf("Info allocation failed\n");
   fflush(stdin);
   getchar();
   return 0;
  }

  applog=new CPco_Log("camera_grab_single.log");

  loglevel|=ERROR_M;
  applog->set_logbits(loglevel);

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Start of grabber open");
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Number of scan results: %d", infosize);

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Actually used parameters:");
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Loop count        %d",loop);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Image count       %d",imagecount);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"loglevel          0x%08x",loglevel);

  grabber_count=0;
  for(int i=0; i<infosize; i++)
  {
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Device %d: %s\n", i,KY_DeviceDisplayName(i));
   KY_DEVICE_INFO deviceInfo;
   KY_DeviceInfo(i, &deviceInfo);
   printf("m_protocol 0x%X\n", deviceInfo.m_Protocol);
   if (deviceInfo.m_Protocol == 1)
	   gnum = i;
   grabber_count++;

  } 

  if(gnum<grabber_count)
  {
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"connect to grabber %d",gnum);
   if((fghandle = KYFG_Open(gnum)) != -1)
   {
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Good connection to grabber #%d, handle=%X",gnum,fghandle);
    dmadQueuedBufferCapable = KYFG_GetGrabberValueInt(fghandle, DEVICE_QUEUED_BUFFERS_SUPPORTED);
    if (1 != dmadQueuedBufferCapable)
    {
     applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"grabber #%d does not support queued buffers",gnum);
     KYFG_Close(fghandle);
     fghandle=0;
    }
   }
   else
   {
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Could not connect to grabber #%d",gnum);
    fghandle=0;
   }
  }
  
  if(fghandle==0)  
  {
   applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"No grabber %d connected",gnum);
   fflush(stdin);
   x=getchar();
   delete applog;
   return 0;
  }

  val=1;
  fg_status=KYFG_SetGrabberValueInt(fghandle,"SilentDiscoveryReg",val);
  if(fg_status==FGSTATUS_OK)
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: set SilentDiscovery ON done",gnum);
  else
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: set SilentDiscovery ON failed",gnum);


//  fg_status=KYFG_CameraScan(fghandle, camHandleArray, &detectedCameras);
  detectedCameras=4;
  fg_status=KYFG_UpdateCameraList(fghandle, camHandleArray, &detectedCameras);

  if(fg_status==FGSTATUS_OK)
  {
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Found %d cameras at grabber %d", detectedCameras,gnum);

    applog->writelog(INIT_M | STDOUT_M | DBGOUT_M, "create camera at grabber %d", gnum);
    if (FGSTATUS_OK == KYFG_CameraOpen2(camHandleArray[0], 0))
    {
      fg_status = KYFG_SetGrabberValueEnum_ByValueName(camHandleArray[0], "TransferControlMode", "UserControlled");
      if (fg_status == FGSTATUS_OK)
        applog->writelog(INIT_M | STDOUT_M | DBGOUT_M, "KYFG_SetGrabberValueEnum_ByValueName(cam,TransferControlMode,UserControlled) done");
      else
        applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "KYFG_SetGrabberValueEnum_ByValueName(cam,TransferControlMode,UserControlled) failed error 0x%08x", fg_status);
    }
    else
    {
      applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "failed to open");
      goto EXIT;
    }

  }
  else
  {
    applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "KYFG_CameraScan failed 0x%x", fg_status);
    goto EXIT;
  }

  width=1024;
  height=258;
  bitpix=16;
  /*fg_status = KYFG_CameraExecuteCommand(camHandleArray[0], "AcquisitionArm");
  if (fg_status != FGSTATUS_OK)
  {
    applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "AcquisitionArm failed 0x%x",fg_status);
    goto EXIT;
  }*/

  val = KYFG_GetCameraValueInt(camHandleArray[0], "PixelFormat");
  char evname[40];
  switch (val)
  {
    case PT_MONO8:      sprintf_s(evname, 20, "PT_MONO8");   bitpix = 8; break;
    case PT_MONO8S:     sprintf_s(evname, 20, "PT_MONO8S");  bitpix = 8; break;
    case PT_MONO10:     sprintf_s(evname, 20, "PT_MONO10");  bitpix = 16; break;
    case PT_MONO12:     sprintf_s(evname, 20, "PT_MONO12");  bitpix = 16; break;
    case PT_MONO14:     sprintf_s(evname, 20, "PT_MONO14");  bitpix = 16; break;
    case PT_MONO16:     sprintf_s(evname, 20, "PT_MONO16");  bitpix = 16; break;
    case PT_MONO10P:    sprintf_s(evname, 20, "PT_MONO10P"); bitpix = 16; break;
    case PT_MONO12P:    sprintf_s(evname, 20, "PT_MONO12P"); bitpix = 16; break;
    case PT_MONO14P:    sprintf_s(evname, 20, "PT_MONO14P"); bitpix = 16; break;
    case PT_BAYERGB8:   sprintf_s(evname, 20, "PT_BAYERGB8"); bitpix = 8; break;
    case PT_BAYERGB10P: sprintf_s(evname, 20, "PT_BAYERGB10P"); bitpix = 16; break;
    case PT_BAYERGB12P: sprintf_s(evname, 20, "PT_BAYERGB12P"); bitpix = 16; break;
    case PT_BAYERGB14P: sprintf_s(evname, 20, "PT_BAYERGB14P"); bitpix = 16; break;
    default:            sprintf_s(evname, 20, "not defined"); bitpix = 8; break;
  }

  val = KYFG_GetCameraValueInt(camHandleArray[0], "Width");
  width = (WORD)val;

  val = KYFG_GetCameraValueInt(camHandleArray[0], "Height");
  height = (WORD)val;

  applog->writelog(PROCESS_M | STDOUT_M | DBGOUT_M, "width %d height %d bitpix %d (%s)", width, height, bitpix,evname);

  int line_width=width * ((bitpix + 7) / 8) * 8;
  fg_status = KYFG_SetGrabberValueInt(camHandleArray[0], "Width", width);
  if (fg_status == FGSTATUS_OK)
    applog->writelog(PROCESS_M,"Width set to %d", width);
  else
    applog->writelog(ERROR_M,"set Width failed");

  fg_status = KYFG_GetGrabberValue(camHandleArray[0], "Width", &val);
  if (fg_status == FGSTATUS_OK)
    applog->writelog(INTERNAL_1_M,"actual Width is %d 0x%08x",val, val);
  else
    applog->writelog(ERROR_M,"get Width failed");

  fg_status = KYFG_SetGrabberValueInt(camHandleArray[0], "Height", height);
  if (fg_status == FGSTATUS_OK)
    applog->writelog(PROCESS_M,"Height set to %d", height);
  else
    applog->writelog(ERROR_M,"set Height failed");

  fg_status = KYFG_GetGrabberValue(camHandleArray[0], "Height", &val);
  if (fg_status == FGSTATUS_OK)
    applog->writelog(INTERNAL_1_M,"actual Height is %d 0x%08x", val, val);
  else
    applog->writelog(ERROR_M,"get Height failed");

  line_width /= 8;
  int dmalength = (line_width * height);
  applog->writelog(PROCESS_M,"line_width %d, dmalength %d",line_width, dmalength);

  val = KYFG_GetCameraValueInt(camHandleArray[0], "PayloadSize");
  applog->writelog(PROCESS_M,"PayloadSize %d",val);


  fg_status = KYFG_StreamCreate(camHandleArray[0], &streamHandle, 0);
  if (FGSTATUS_OK != fg_status)
  {
    streamHandle = 0;
    applog->writelog(ERROR_M,"KYFG_StreamCreate failed err 0x%08x",fg_status);
  }
  else
  {
    size_t frameDataSize, frameDataAligment;

    fg_status = KYFG_StreamBufferCallbackRegister(streamHandle, streambuffer_callback, &param);
    if (FGSTATUS_OK != fg_status)
      applog->writelog(ERROR_M,"KYFG_StreamBufferCallbackRegister failed err 0x%08x", fg_status);
    else
      applog->writelog(PROCESS_M,"KYFG_StreamBufferCallbackRegister done");


    fg_status = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_PAYLOAD_SIZE, &frameDataSize, NULL, NULL);
    if (FGSTATUS_OK != fg_status)
      applog->writelog(ERROR_M,"KYFG_StreamGetInfo(...,KY_STREAM_INFO_PAYLOAD_SIZE) failed err 0x%08x", fg_status);
    fg_status = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_BUF_ALIGNMENT, &frameDataAligment, NULL, NULL);
    if (FGSTATUS_OK != fg_status)
      applog->writelog(ERROR_M,"KYFG_StreamGetInfo(...,KY_STREAM_INFO_BUF_ALIGNMENT) failed err 0x%08x", fg_status);

    applog->writelog(PROCESS_M,"KYFG_StreamGetInfo payload %d alingment 0x%x", frameDataSize, frameDataAligment);

    for (int i = 0; i < MAXSTREAMBUF; i++)
    {
      void* pBuffer = _aligned_malloc(frameDataSize, frameDataAligment);
      streamBufferHandles[i] = 0;
      fg_status = KYFG_BufferAnnounce(streamHandle, pBuffer, frameDataSize, NULL, &streamBufferHandles[i]);
      if (FGSTATUS_OK != fg_status)
        applog->writelog(ERROR_M,"KYFG_BufferAnnounce(...,%p,...[%d])  failed err 0x%08x", pBuffer, i, fg_status);
      else
        applog->writelog(PROCESS_M,"KYFG_BufferAnnounce(...,%p,...) done streamBufferHandles[%d] 0 0x%016llx", pBuffer, i, streamBufferHandles[i]);
    }
  }

  fg_status = KYFG_CameraExecuteCommand(camHandleArray[0], "AcquisitionStop");
  if (fg_status != FGSTATUS_OK)
  {
    applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "Acquisition Stop failed");
  }

  int actcount;
  SYSTEMTIME  st;
  void* bufin;


  tp_ima2 = std::chrono::high_resolution_clock::now();
  tp_ima1 = tp_ima2;
  errcount=0;
  for (actcount=0; actcount<imagecount;)
  {
    tp_ima2 = std::chrono::high_resolution_clock::now();
    usec = (tp_ima2 - tp_ima1);
    tp_ima1 = tp_ima2;

    GetSystemTime(&st);
    printf("%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    printf("%6d.%03dms ", (int)usec.count() / 1000, (int)usec.count()%1000);
//    printf("%.3fms ", usec.count() / 1000);

    printf("%06d ",actcount+1);
    val = 1;
    fg_status = KYFG_SetGrabberValueInt(camHandleArray[0], "StatisticsCountersReset",val);
    if (fg_status == FGSTATUS_OK)
      applog->writelog(PROCESS_M,"set StatisticsCountersReset done");
    else
      applog->writelog(ERROR_M,"set StatisticsCountersReset failed err 0x%08x", fg_status);

    fg_status = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_INPUT);
    applog->writelog(PROCESS_M,"KYFG_BufferQueueAll returned 0x%x", fg_status);

    fg_status = KYFG_CameraStart(camHandleArray[0], streamHandle, 0);
    if (fg_status == FGSTATUS_OK)
      applog->writelog(PROCESS_M,"CameraStart done");
    else
      applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M,"CameraStart failed 0x%x",fg_status);
    printf("start_acq ");

    fg_status = KYFG_CameraExecuteCommand(camHandleArray[0], "AcquisitionStart");
    if (fg_status != FGSTATUS_OK)
    {
      applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "AcquisitionStart failed");
    }
    else
    {
      applog->writelog(PROCESS_M, "AcquisitionStart done");
      printf("start_rec ");
    }

    waitstat = WaitForSingleObject(param.picevent, 500);
    if (waitstat == WAIT_TIMEOUT)
    {
      printf("                                                                                                                    \n");
      tp_ima2 = std::chrono::high_resolution_clock::now();
      usec = (tp_ima2 - tp_ima1);
      GetSystemTime(&st);
      printf("%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
      printf("%.3fms ", usec.count() / 1000);
      printf("ERROR waiting for buffer");
      errcount++;
      break;
    }
    applog->writelog(PROCESS_M, "WaitEvent done");
    actcount++;
    ResetEvent(param.picevent);
	
	while (!param.qbuf_queue.empty())
	{
		
		printf("iteration = %d", iteration);
		if (iteration == 0)
		{
			goto EXIT;
			break;
		}
			
		iteration--;
		EnterCriticalSection(&param.qbuf_queue_crisec);
		streamBufferHandle = param.qbuf_queue.front();
		LeaveCriticalSection(&param.qbuf_queue_crisec);

		printf("ev 0x%016llx ", streamBufferHandle);
		fg_status = KYFG_BufferGetInfo(streamBufferHandle, KY_STREAM_BUFFER_INFO_BASE, &bufin, NULL, NULL);
		if (fg_status != FGSTATUS_OK)
			applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "KYFG_BufferGetInfo failed 0x%08x", fg_status);

		applog->writelog(PROCESS_M | DBGOUT_M, "Info ok 0x%016llx adr %p", streamBufferHandle, bufin);

		EnterCriticalSection(&param.qbuf_queue_crisec);
		param.qbuf_queue.pop();
		LeaveCriticalSection(&param.qbuf_queue_crisec);
		
	}

    fg_status = KYFG_CameraStop(camHandleArray[0]);
    if (fg_status == FGSTATUS_OK)
      applog->writelog(PROCESS_M, "CameraStop done");
    else
      applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "CameraStop failed 0x%x", fg_status);
    printf("stop_acq ");

    fg_status = KYFG_CameraExecuteCommand(camHandleArray[0], "AcquisitionStop");
    if (fg_status != FGSTATUS_OK)
    {
      applog->writelog(ERROR_M | STDOUT_M | DBGOUT_M, "AcquisitionStop failed");
    }
    else
    {
      applog->writelog(PROCESS_M, "AcquisitionStop done");
      printf("stop_rec ");
    }

    if (imagecount <= 50)
      printf("\n");
    else
      printf("\r");
    if (errcount > 10)
      break;
    if (_kbhit())
    {
      char a;
      a = _getch();
      if (a == 0x1b)
      {
        actcount = imagecount + 10;
        errcount = 10;
        break;
      }
    }
  }//end for imacount

EXIT:
  while (!param.qbuf_queue.empty())
    param.qbuf_queue.pop();

  for(int i = 0; i < MAXSTREAMBUF; i++)
  {
    if (streamBufferHandles[i])
    {
      unsigned char* adr = NULL;
      fg_status = KYFG_BufferGetInfo(streamBufferHandles[i], KY_STREAM_BUFFER_INFO_BASE, &adr, NULL, NULL);
      if (FGSTATUS_OK != fg_status)
          applog->writelog(ERROR_M,"KYFG_BufferGetInfo(..[%d],KY_STREAM_BUFFER_INFO_BASE) failed err 0x%08x", i, fg_status);
      if (adr)
        _aligned_free(adr);
    }
  }

  if (streamHandle > 0)
  {
    fg_status = KYFG_StreamBufferCallbackUnregister(streamHandle, streambuffer_callback);
    if (FGSTATUS_OK != fg_status)
      applog->writelog(ERROR_M,"KYFG_StreamBufferCallbackUnregister failed err 0x%08x", fg_status);
    fg_status = KYFG_StreamDelete(streamHandle);
    if (FGSTATUS_OK != fg_status)
      applog->writelog(ERROR_M,"KYFG_StreamDelete failed err 0x%08x",fg_status);
    streamHandle = 0;
  }
  DeleteCriticalSection(&param.qbuf_queue_crisec);

  printf("\n");

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Exiting...");

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"close camera at grabber %d",gnum);
  if (FGSTATUS_OK == KYFG_CameraClose(camHandleArray[0]))
    applog->writelog(INIT_M, "Camera was closed successfully");

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"close grabber %d",gnum);
  if(KYFG_Close(fghandle) == FGSTATUS_OK)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d closed successfully",gnum);
  else
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"wasn't able to close grabber #%d",gnum);

  delete applog;

  printf("Press to exit");
  fflush(stdin);
  getchar();
  return 0;
}


int get_yesno(BOOL *val,char *txt)
{
  int i;
  char ch;
//  *val=FALSE;
  printf("%s ",txt);
  for( i = 0; (i < 10) &&  ((ch = getchar()) != EOF) && (ch != '\n'); i++ )
  {
   if((ch=='Y')||(ch=='y'))
    *val=TRUE;
   else if((ch=='N')||(ch=='n'))
    *val=FALSE;
   else
    i--;
  }
  fflush(stdin);
  return i;
}


