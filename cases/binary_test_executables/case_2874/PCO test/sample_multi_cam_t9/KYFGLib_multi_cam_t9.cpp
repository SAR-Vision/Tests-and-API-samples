/************************************************************************ 
*  File: KYFGLib_multi_cam_t2.cpp
*  Sample Frame Grabber API application
*
*  purpose:
*  purpose:
*   test threaded application, if more than 2 single line cameras connected
*   and grabber is set to "TransferControlMode","UserControlled"
*   use kaya runtime 4.2.1.3552 and kaya driver 4.2
*   only single camera mutex and no grabber mutex is required
*
*  use first grabber found
*  use as much cameras as connected
*  can use commandline parameters
*  no user input required for initial loops
*  only threaded loop
*
*  PCO AG (2018)
*************************************************************************/

#include "stdafx.h"


#include "Cpco_log.h"
#include "Cpco_camera.h"


#if !defined(_countof)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#define MAXBOARDS 4
#define MAXCAM    4
#define MAXBUF    6

#define COMTHREAD_WAITTIME 333   //time the command thread is waiting, before command list is sent again

typedef struct
{
 DWORD   flags;
 DWORD   imagecount;
 DWORD   actcount;
 DWORD   stime; 
 CPco_Camera *Cam;
 CPco_Log *tlog; 
 HANDLE  start_event;
 HANDLE  loop_event;
}threadpar;

DWORD WINAPI image_thread(LPVOID param);
DWORD WINAPI command_thread(LPVOID param);

int get_yesno(BOOL *val,char *txt);
void set_timer(FGHANDLE fgHandle, float freq);
void set_trigger(FGHANDLE fgHandle, int port, CPco_Log* tlog);


int main(int argc, char* argv[])
{
  unsigned int* info = 0;
  unsigned int grabber_count;
  unsigned int infosize,grabberIndex,cameraIndex;

  BOOL x;
  int cam;
  int64_t dmadQueuedBufferCapable;

  WORD recstate;
  WORD width,height;
  int  bitpix;

  DWORD err;
  DWORD errcount;
  CPco_Camera* Ccam[MAXCAM];

  CPco_Buffer* Cbuf[MAXCAM][MAXBUF];
  HANDLE waitobj[MAXCAM];

  FGHANDLE fghandle;
  int detectedCameras;
  HANDLE Grabber_StopEvent;

  CAMHANDLE camHandleArray[MAXCAM];
  DWORD imacount[MAXCAM];

  WORD camtype;
  DWORD serialnumber;


  FGSTATUS fg_status;
  int lc;

//default values can be changed with input parameters -U
  int acq_user=FLAG_ACQUIRE_USER|FLAG_START_CAM_AFTER_ACQ|FLAG_STOP_CAM_AFTER_ACQ;
//  acq_user|=FLAG_GRABBER_MUTEX;
  acq_user|=FLAG_LOGFILE;
  acq_user|=FLAG_ADD_STOPTIME;

//  acq_user|=FLAG_COMMAND_GRABBERSYNC;
//  acq_user|=FLAG_GRABBER_CRISEC;
//  acq_user|=FLAG_COMMAND_GRABBERSYNC_R;
//  acq_user|=FLAG_ACQUIRE_NOMUTEX;

  int loop=10;                     //parameter -I
  int silent_discovery=1;         //parameter -S
  int manual_config=1;            //parameter -M  must not be changed when working with pco.flow
  int ima_count=5000;           //parameter -C
  int gnum=0;                     //parameter -B
  int loglevel=0x0300FFFF;        //parameter -L
  int triggermode=2;              //parameter -T

  loglevel&=~BUFFER_M;
//  loglevel&=~COMMAND_M;
  loglevel&=~DBGOUT_M;


  CPco_Log *applog;
  DWORD waitstat;

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

  Grabber_StopEvent=NULL;
  fghandle=0;
  detectedCameras=0;
  for(cam=0;cam<MAXCAM;cam++)
  {
   camHandleArray[cam]=0;
   Ccam[cam]=NULL;
   imacount[cam]=0;
   for(int k=0; k<MAXBUF; k++)
   {
    Cbuf[cam][k]=NULL;
    waitobj[cam]=NULL;
   }
  }

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
    if(b[1]=='U')
    {
     acq_user=strtol(b+2,NULL,0);
    }
    if(b[1]=='I')
    {
     loop=strtol(b+2,NULL,0);
     if(loop<1)
      loop=1;
    }
    if(b[1]=='S')
    {
     silent_discovery=strtol(b+2,NULL,0);
    }
    if(b[1]=='M')
    {
     manual_config=strtol(b+2,NULL,0);
    }
    if(b[1]=='C')
    {
     ima_count=strtol(b+2,NULL,0);
     if(ima_count<10)
      ima_count=10;
    }
    if(b[1]=='T')
    {
     triggermode=strtol(b+2,NULL,0);
     if((triggermode<0)||(triggermode>3))
      triggermode=0;
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
     printf("-U[0x... ]       change Flags              default: 0x%x\n",FLAG_ACQUIRE_USER|FLAG_START_CAM_AFTER_ACQ|FLAG_STOP_CAM_AFTER_ACQ|FLAG_GRABBER_MUTEX|FLAG_LOGFILE);
     printf("-I[1 ... ]       loop count                default: 1\n");
     printf("-C[10 ... ]      image count               default: 150\n");
     printf("-L[0x...]        loglevel                  default: 0x0300FFFF\n");
     printf("-S[0...1]        silent discovery on,off   default: on\n");
     printf("-M[0...1]        manual config on,off      default: off\n");
     printf("-T[0...3]        triggermode               default: AUTO   (AUTO=0,SWTRIG=1,FRAME=2,EXPCTRL=3\n");
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

  DWORD dwPriClass = GetPriorityClass(GetCurrentProcess());
  printf("Current priority class is 0x%x\n", dwPriClass);


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

  if(acq_user&FLAG_LOGFILE)
  {
   if(acq_user&FLAG_GRABBER_MUTEX) 
    applog=new CPco_Log("grabber_mutex_t9.log");
   else if(acq_user&FLAG_GRABBER_CRISEC)
    applog=new CPco_Log("grabber_crisec_t9.log");
   else 
    applog=new CPco_Log("camera_mutex_t9.log");
  }
  else
   applog=new CPco_Log(NULL);

  loglevel|=ERROR_M;
  applog->set_logbits(loglevel);

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Current priority class is 0x%x\n", dwPriClass);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Start of grabber open");
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Number of scan results: %d", infosize);

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Actually used parameters:");
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Flags acq_user    0x%08x",acq_user);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Loop count        %d",loop);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Image count       %d",ima_count);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"loglevel          0x%08x",loglevel);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"silent discovery  %d",silent_discovery);
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"manual config     %d",manual_config);
  
  
  KY_DEVICE_INFO* grabbersInfoArray = new KY_DEVICE_INFO[infosize];
  grabber_count=0;
  for(int i=0; i<infosize; i++)
  {
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Device %d: %s", i,KY_DeviceDisplayName(i));
   grabber_count++;
   KY_DeviceInfo(i, &grabbersInfoArray[i]);

   
	KY_DeviceInfo(i, &grabbersInfoArray[i]);
	printf("FGSTATUS = 0x%X\n", KY_DeviceInfo(i, &grabbersInfoArray[i]));
	printf("Protocol 0x%d\n", grabbersInfoArray[i].m_Protocol);
	if (grabbersInfoArray[i].m_Protocol == 1)
			gnum = i;
  } 
  printf("gnum = %d\n", gnum);
  printf("grabber_count = %d\n", grabber_count);
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
   getchar();
   delete applog;
   return 0;
  }

  uint64_t val;
  fg_status=KYFG_GetGrabberValue(fghandle,"MaxLinks",&val);
  if(fg_status==FGSTATUS_OK)
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: Maxlinks             = %d",gnum,val);

  fg_status=KYFG_GetGrabberValue(fghandle,"DevicePciGeneration",&val);
  if(fg_status==FGSTATUS_OK)
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: DevicePciGeneration  = %d",gnum,val);

  fg_status=KYFG_GetGrabberValue(fghandle,"DevicePciLanes",&val);
  if(fg_status==FGSTATUS_OK)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: DevicePciLanes       = %d",gnum,val);

  fg_status=KYFG_GetGrabberValue(fghandle,"SerialNumber",&val);
  if(fg_status==FGSTATUS_OK)
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: SerialNumber         = %d",gnum,val);

  fg_status=KYFG_GetGrabberValue(fghandle,"DeviceMemorySize",&val);
  if(fg_status==FGSTATUS_OK)
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: DeviceMemorySize     = %d",gnum,val);

  fg_status=KYFG_GetGrabberValue(fghandle,"DeviceTemperature",&val);
  if(fg_status==FGSTATUS_OK)
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: DeviceTemperature    = %d",gnum,val);

  if(silent_discovery==1)
  {
   val=1;
   fg_status=KYFG_SetGrabberValueInt(fghandle,"SilentDiscoveryReg",val);
   if(fg_status==FGSTATUS_OK)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: set SilentDiscovery ON done",gnum);
   else
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: set SilentDiscovery ON failed",gnum);
  }

  if(manual_config==1)
  {
   uint64_t val,val1,channel;
   FGSTATUS fg_status=FGSTATUS_OK;
   PORT_STATUS portStatus;

   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: do manual config",gnum);

   channel=0;
   for(int j=0;j<MAXCAM;j++)
   {
    fg_status=KYFG_GetPortStatus(fghandle,j,&portStatus);
    if(fg_status==FGSTATUS_OK)
    {
     if(portStatus==PORT_DISCONNECTED)
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: port[%d] L1 LinkState: PORT_DISCONNECTED",gnum,j);
     else if(portStatus==PORT_SYNCHRONIZED)
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: port[%d] L1 LinkState: PORT_SYNCHRONIZED",gnum,j);
     else if(portStatus==PORT_CONNECTING)
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: port[%d] L1 LinkState: PORT_CONNECTING",gnum,j);
     else if(portStatus==PORT_CONNECTED)
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d: port[%d] L1 LinkState: PORT_CONNECTED",gnum,j);
    }

    if((portStatus==PORT_CONNECTED)||(portStatus==PORT_SYNCHRONIZED))
    {
     val=channel;
     fg_status=KYFG_SetGrabberValueInt(fghandle,"CameraSelector",val);
     if(fg_status==FGSTATUS_OK)
     {
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"set CameraSelector %d done",val);

      val=1;
      fg_status=KYFG_SetGrabberValueInt(fghandle,"ManualCameraMode",val);
      if(fg_status==FGSTATUS_OK)
       applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"set ManualCameraMode ON done");
      else
       applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"set ManualCameraMode ON failed");
     }
     else
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"set CameraSelector %d failed",val);

     if(fg_status==FGSTATUS_OK)
     {
      fg_status=KYFG_GetGrabberValue(fghandle, "ManualCameraConnectionConfig",&val);
      if(fg_status==FGSTATUS_OK)
       applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"actual ManualCameraConnectionConfig is 0x%08x",val);
      else 
       applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"get ManualCameraConnectionConfig failed");

      val&=0x0000FFFF;
      val1=1;
      val|=(val1<<16);
      fg_status=KYFG_SetGrabberValueInt(fghandle, "ManualCameraConnectionConfig",val);
      if(fg_status==FGSTATUS_OK)
       applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"set ManualCameraConnectionConfig 0x%08x done",val);
      else
       applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"set ManualCameraConnectionConfig failed");
     }

     if(fg_status==FGSTATUS_OK)
     {
      val=0;
      fg_status=KYFG_SetGrabberValueInt(fghandle, "ManualCameraChannelSelector",val);
      if(fg_status==FGSTATUS_OK)
       applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"set ManualCameraChannelSelector %d done",val);
      else
       applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"set ManualCameraChannelSelector failed");
     }

     if(fg_status==FGSTATUS_OK)
     {
      val=j;
      fg_status=KYFG_SetGrabberValueInt(fghandle, "ManualCameraFGLink",val);
      if(fg_status==FGSTATUS_OK)
       applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"set ManualCameraFGLink %d done",val);
      else
       applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"set ManualCameraFGLink failed");
     }

     if(fg_status==FGSTATUS_OK)
      channel++;
     else
      break;
    }
   }
  }

  fg_status=KYFG_CameraScan(fghandle, camHandleArray, &detectedCameras);
  if(fg_status==FGSTATUS_OK)
  {
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Found %d cameras at grabber %d", detectedCameras,gnum);

   for(cam=0;cam<detectedCameras;cam++)
   {
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"create class for camera %d at grabber %d",cam,gnum);
    Ccam[cam]=new CPco_Camera;

    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"call open");
    Ccam[cam]->SetLog(applog);
    Ccam[cam]->SetGrabber(fghandle);
    if(Ccam[cam]->open(camHandleArray[cam],cam)!=0)
    {
     applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"failed to open");
     delete Ccam[cam];
     return -1;
    }
//    WORD camtype;
//    DWORD serialnumber;
    if(Ccam[cam]->PCO_GetCameraType(&camtype,&serialnumber)==PCO_NOERROR)
     applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_GetCameraType done 0x%04x %d",camtype,serialnumber);
    else
     applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_GetCameraType failed");

    if(Ccam[cam]->PCO_GetDescription()==PCO_NOERROR)
     applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_GetDescription done");

    Ccam[cam]->set_acqflag(acq_user);
    if(acq_user&FLAG_ACQUIRE_USER)
    {
     fg_status=KYFG_SetGrabberValueEnum_ByValueName(fghandle,"TransferControlMode","UserControlled");
     if(fg_status==FGSTATUS_OK)
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"KYFG_SetGrabberValueEnum_ByValueName(cam%d,TransferControlMode,UserControlled) done",cam);
     else
      applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"KYFG_SetGrabberValueEnum_ByValueName(cam%d,TransferControlMode,UserControlled) failed error 0x%08x",cam,fg_status);
    }
   }
  }
  else
   applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"KYFG_CameraScan failed 0x%x",fg_status);
   

  HANDLE com_hthread[MAXCAM];
  threadpar com_par[MAXCAM];

  for(cam=0;cam<MAXCAM;cam++)
  {
   com_hthread[cam]=NULL;
   memset(&com_par[cam],0,sizeof(threadpar));
  }


  dwPriClass = GetPriorityClass(GetCurrentProcess());
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Current priority class is 0x%x", dwPriClass);

  Grabber_StopEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
  for(cam=0;cam<detectedCameras;cam++)
  {
   Ccam[cam]->SetGrabberStopEvent(Grabber_StopEvent);
  }
  x=FALSE;
//  get_yesno(&x,"start command threads y/n <CR> (default Yes)");   //uncomment this for always starting commmand thread
  if(x==TRUE)
  {
   for(cam=0;cam<detectedCameras;cam++)
   {
    com_par[cam].start_event=CreateEvent(NULL,TRUE,FALSE,NULL);
    com_par[cam].loop_event=CreateEvent(NULL,TRUE,FALSE,NULL);
    com_par[cam].flags=0;
    com_par[cam].Cam=Ccam[cam];
    com_par[cam].tlog=applog;
    com_par[cam].stime=COMTHREAD_WAITTIME+cam*3;
    com_hthread[cam]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE )command_thread,&com_par[cam],0,NULL);
    Sleep(22);
   }
   applog->writelog(PROCESS_M|STDOUT_M|DBGOUT_M,"CommandThreads started",cam);
  }

  for(cam=0;cam<detectedCameras;cam++)
  {
   DWORD del = cam; //*100;
   DWORD exp = 1000;
   err=Ccam[cam]->PCO_SetRecordingState(0);
   if(err!=PCO_NOERROR)
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Cam_%d PCO_SetRecordingState(0) failed",cam);
   err=Ccam[cam]->PCO_GetRecordingState(&recstate);
   if(err!=PCO_NOERROR)
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Cam_%d PCO_GetRecordingState(0) failed",cam);
   else
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Cam_%d PCO_GetRecordingState returned %d",cam,recstate);

   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M," ");
   Ccam[cam]->PCO_Get_Trigger_Options();
   Ccam[cam]->PCO_Enable_Trigger();
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M," ");


//set equal time to both cameras
   if(Ccam[cam]->PCO_SetTimebase(1,1)==PCO_NOERROR)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetTimebase done ");
   else
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetTimebase failed");
   
   if(Ccam[cam]->PCO_SetDelayExposure(del,exp)==PCO_NOERROR)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetDelayExposure(%d,%d) done ",del,exp);
   else
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetDelayExposure failed");

   if(Ccam[cam]->PCO_SetTriggerMode(triggermode)==PCO_NOERROR)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetTrigger(%d) done ",triggermode);
   else
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetTrigger failed");

   if(Ccam[cam]->PCO_SetBitAlignment(1)==PCO_NOERROR)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetBitAlignment(1) done ",triggermode);
   else
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetBitAlignment(1) failed");

   if(Ccam[cam]->PCO_SetROI(1,1,1280,256)==PCO_NOERROR)
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetROI(1,1,1280,256) done ");
   else
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetROI(1,1,1280,256)failed");

   err=Ccam[cam]->PCO_SetTimestampMode(2);
   if(err!=PCO_NOERROR)
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Cam_%d PCO_SetTimestampMode(2) failed",cam);

   err=Ccam[cam]->PCO_ArmCamera();
   if(err!=PCO_NOERROR)
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_ArmCamera() failed");

   width=1280;
   height=256;
   bitpix=16;

//error in flow ??
   Ccam[cam]->get_sizes(&width,&height,&bitpix);
   err=Ccam[cam]->set_acquire_size(width,height,bitpix);
   if(err!=PCO_NOERROR)
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Cam_%d set_acquire_size(%d,%d,%d) failed",cam,width,height,bitpix);
   else
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Cam_%d set_acquire_size(%d,%d,%d) done",cam,width,height,bitpix);
 
   for(int k=0; k<MAXBUF; k++)
   {
    Cbuf[cam][k]=new CPco_Buffer();
    Cbuf[cam][k]->allocate_buffer(width,height,bitpix);
   }

/*
    if(grabber_use_unpack)
    {
     fg_status=Clib->KYFG_SetGrabberValueEnum_ByValueName(camHandleArray[cam],"PackedDataMode","Unpacked");
     if(fg_status==FGSTATUS_OK)
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,": KYFG_SetGrabberValueEnum_ByValueName(Cam_%d,PackedDataMode,Unpacked) done",cam);
     else
      applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,": KYFG_SetGrabberValueEnum_ByValueName(Cam_%d,PackedDataMode,Unpacked) failed error 0x%08x",cam,fg_status);
    }
    else
    {
     fg_status=Clib->KYFG_SetGrabberValueEnum_ByValueName(camHandle[i],"PackedDataMode","Packed_RowAligned32");
     if(fg_status==FGSTATUS_OK)
      writelog(INIT_M|STDOUT_M|DBGOUT_M,": KYFG_SetGrabberValueEnum_ByValueName(Cam_%d PackedDataMode,Packed_RowAligned32) done",i);
     else
      writelog(ERROR_M,|STDOUT_M|DBGOUT_M,": KYFG_SetGrabberValueEnum_ByValueName(Cam_%d PackedDataMode,Packed_RowAligned32) failed error 0x%08x",cam,fg_status);
  }
*/
  }

  set_timer(fghandle,250.0);

  for(cam=0;cam<detectedCameras;cam++)
   set_trigger(fghandle,Ccam[cam]->get_port(),applog);


/*
  printf("Press to start acquisition_thread");
  fflush(stdin);
  getchar();
*/
  for(cam=0;cam<detectedCameras;cam++)
  {
   Ccam[cam]->start_acquisition_thread();
   imacount[cam]=Ccam[cam]->get_acquired_images();

   err=Ccam[cam]->PCO_GetRecordingState(&recstate);
   if(err!=PCO_NOERROR)
    applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Cam_%d PCO_GetRecordingState(0) failed",cam);
   else
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Cam_%d PCO_GetRecordingState returned %d",cam,recstate);
  }


  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"image counts: %05d  %05d  %05d  %05d",imacount[0],imacount[1],imacount[2],imacount[3]);

//when looping long enough i get errors with this loop too
  fflush(stdin);
  x=TRUE;
  get_yesno(&x,"acquire with threads y/n <CR> (default Yes)");
  if(x==TRUE)
  {
   threadpar par[MAXCAM];
   HANDLE hthread[MAXCAM];
   HANDLE startev[MAXCAM];
   DWORD waitstat;

   for(lc=0;lc<loop;lc++)
   {
    errcount=0;

//we could suspend com threads while copying logfile
//but it should be not necessary
//  for(cam=0;cam<detectedCameras;cam++)
//    SuspendThread(com_hthread[cam]);
    applog->flushlog();
//  for(cam=0;cam<detectedCameras;cam++)
//    ResumeThread(com_hthread[cam]);


    if(loop>1)
     applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Start Loop %d of %d",lc+1,loop);

    for(cam=0;cam<MAXCAM;cam++)
    {
     hthread[cam]=startev[cam]=NULL;
    }

    for(cam=0;cam<detectedCameras;cam++)
    {
     par[cam].start_event=startev[cam]=CreateEvent(NULL,TRUE,FALSE,NULL);
     par[cam].loop_event=CreateEvent(NULL,TRUE,FALSE,NULL);
     par[cam].flags=0;
     par[cam].Cam=Ccam[cam];
     par[cam].imagecount=ima_count;
     par[cam].actcount=0;
     par[cam].tlog=applog;
     hthread[cam]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE )image_thread,&par[cam],0,NULL);
    }

    waitstat=WaitForMultipleObjects(detectedCameras,startev,TRUE,5000);
    if(waitstat==WAIT_OBJECT_0)
     printf("All threads started loop %d of %d\n",lc+1,loop);

    for(cam=0;cam<detectedCameras;cam++)
     ResetEvent(par[cam].start_event);

    for(cam=0;cam<detectedCameras;cam++)
     SetEvent(par[cam].loop_event);

    Sleep(50);
    for(cam=0;cam<detectedCameras;cam++)
    {
     err=Ccam[cam]->PCO_SetRecordingState(1);
     if(err!=PCO_NOERROR)
     {
      applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetRecordingState(1) failed");
      errcount++;
     }
     else 
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetRecordingState(1) done");
    }

    while(TRUE)
    {
     Sleep(100);
     for(cam=0;cam<detectedCameras;cam++)
      printf("cam%d count % 5d   ",cam,par[cam].actcount);
     printf("\r");
     if(_kbhit())
     {
      char a;
      a=_getch();
      if(a==0x1b)
      {
       for(cam=0;cam<detectedCameras;cam++)
        par[cam].actcount=par[cam].imagecount+10;
       errcount=10;
//       break;
      }
     }
//does trigger command thread to do its command list
     if((par[0].actcount>=ima_count-10)||(par[1].actcount>=ima_count-10))
     {
      for(cam=detectedCameras-1;cam>=0;cam--)
      {
       SetEvent(com_par[cam].start_event);
      }
     }

     waitstat=WaitForMultipleObjects(detectedCameras,startev,TRUE,0);
     if(waitstat==WAIT_OBJECT_0)
     {
      printf("All threads all images done loop %d of %d\n",lc+1,loop);
      break;
     }
    }

    for(cam=0;cam<detectedCameras;cam++)
     WaitForSingleObject(hthread[cam],4000);

    for(cam=0;cam<detectedCameras;cam++)
    {
     CloseHandle(par[cam].start_event);
     CloseHandle(par[cam].loop_event);
     CloseHandle(hthread[cam]);
    }

    for(cam=0;cam<detectedCameras;cam++)
    {
     err=Ccam[cam]->PCO_SetRecordingState(0);
     if(err!=PCO_NOERROR)
     {
      applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetRecordingState(0) cam%d failed",cam);
      errcount++;
     }
     else 
      applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"PCO_SetRecordingState(0) cam%d done",cam);
    }

    for(cam=0;cam<detectedCameras;cam++)
    {
     err=com_par[cam].imagecount;
     if(err!=PCO_NOERROR)
      errcount++;
    }

    if(errcount>0)
    {
     applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"Break loops on error");
     break;
    }

    if(loop<=1)
    {
     int x=TRUE;
     get_yesno(&x,"acquire thread again y/n <CR> (default Yes)");
     if(x==TRUE)
      lc=-1;
    }
    else
    {
     applog->writelog(INIT_M|STDOUT_M|DBGOUT_M," ");
     if(lc==loop-1)
     {
      int x=TRUE;
      get_yesno(&x,"loop acquire threads again y/n <CR> (default Yes)");
      if(x==TRUE)
       lc=-1;
     }
    }
   }
  }

EXIT:
  for(cam=0;cam<detectedCameras;cam++)
  {
   Ccam[cam]->stop_acquisition_thread();
  }

  for(cam=0;cam<detectedCameras;cam++)
  {
   if(com_hthread[cam])
   {
    SetEvent(com_par[cam].loop_event);
    WaitForSingleObject(com_hthread[cam],20000);
    CloseHandle(com_par[cam].start_event);
    CloseHandle(com_par[cam].loop_event);
    applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"camera%d command_thread closed",cam);
   }
  }

  printf("Press to exit");
  fflush(stdin);
  getchar();

  printf("\n");
  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"Exiting...");

  for(cam=0;cam<detectedCameras;cam++)
  {
   for(int k=0; k<MAXBUF; k++)
   {
    if(Cbuf[cam][k])
     delete Cbuf[cam][k];
   }
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"close camera %d at grabber %d",cam,gnum);
   if(Ccam[cam])
   {
    err=Ccam[cam]->PCO_SetRecordingState(0);
    if(err!=PCO_NOERROR)
     applog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"PCO_SetRecordingState(0) failed");
    Ccam[cam]->close();
    delete Ccam[cam];
   }
  }

  CloseHandle(Grabber_StopEvent);

  applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"close grabber %d",gnum);
  if(KYFG_Close(fghandle) == FGSTATUS_OK)
  {
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"grabber #%d closed successfully",gnum);
  }
  else
  {
   applog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"wasn't able to close grabber #%d",gnum);
  }

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


DWORD WINAPI image_thread(LPVOID param)
{
  DWORD err,waitstat;
  double dtime,mtime,freq;
  int x,m,e,b,ima_nr;
  int test,next;
  int bufnum;
  int camnum; 
  int miss,first_ima_nr,last_ima_nr;
  __int64 Count_p0,Count_p1,Count_p2,lpFrequency;
  int errcount;
  WORD recstate;
  WORD width,height;
  int  bitpix;
  WORD camtype;
  DWORD serialnumber;
  uint64_t pdm;

  CPco_Buffer* Cbuf[MAXBUF];
  HANDLE waitobj[MAXBUF];

  threadpar* tpar;
  tpar=(threadpar*)param;
  camnum=tpar->Cam->get_camnum();

  QueryPerformanceFrequency((LARGE_INTEGER*)&lpFrequency);
  Count_p0=Count_p1=Count_p2=0;

  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d  thread is starting",tpar->Cam->get_camnum());

  if(tpar->imagecount<MAXBUF)
   bufnum=tpar->imagecount;
  else
   bufnum=MAXBUF;

  tpar->Cam->get_sizes(&width,&height,&bitpix);
  tpar->Cam->PCO_GetCameraType(&camtype,&serialnumber);
  pdm=tpar->Cam->get_packeddatamode();

  for(int k=0;k<bufnum;k++)
  {
   Cbuf[k]=new CPco_Buffer();
   Cbuf[k]->allocate_buffer(width,height,bitpix);
   waitobj[k]=Cbuf[k]->get_event();
  }

  for(int k=0;k<bufnum;k++)
  {
   tpar->Cam->setup_buffer(Cbuf[k]);
  }

  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d setup_buffer done",camnum);


  tpar->Cam->start_acquisition();

  SetEvent(tpar->start_event);
  waitstat=WaitForSingleObject(tpar->loop_event,1000);

  QueryPerformanceCounter((LARGE_INTEGER*)&Count_p2);
  Count_p0=Count_p1=Count_p2;

  if(waitstat==WAIT_OBJECT_0)
  {
   tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d  thread start image grabbing",camnum);
   test=next=0;
   miss=first_ima_nr=last_ima_nr=0;
   errcount=0;

   for(;tpar->actcount<tpar->imagecount;)
   {
    m=0;
    waitstat=WaitForMultipleObjects(bufnum,waitobj,FALSE,1000);
    if(waitstat==WAIT_TIMEOUT)
    {
     tpar->tlog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"cam%d  \nerror in waiting for buffer",camnum);
     errcount++;
     break;  
    }
//    printf("cam%1d WaitMultiple ok\n",tpar->Cam->board);

    test=next;
    for(b=0;b<bufnum;b++)
    {
     QueryPerformanceCounter((LARGE_INTEGER*)&Count_p2);
     dtime=(double)(Count_p2-Count_p1);
     dtime=dtime/lpFrequency;
     dtime*=1000;
     Count_p1=Count_p2;

     waitstat=WaitForSingleObject(waitobj[test],0);
     e=0;
     if(waitstat==WAIT_OBJECT_0)
     {
//      printf("cam%1d WaitSingle ok\n",tpar->Cam->board);
      m++;
      ResetEvent(waitobj[test]);
//      GetLocalTime(&st);
//      printf("%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
//      printf("%4d.%03d ",(int)dtime%1000000,(int)((dtime-(int)dtime)*1000));
      if(Cbuf[test]->get_status()==PCO_NOERROR)
      {
//       printf("buf%02d status %08x ",test,dwStatus[test]); 
       if((camtype==0x1500)&&(pdm==1))
        ima_nr=Cbuf[test]->image_nr_from_timestamp_c(0);
       else 
        ima_nr=Cbuf[test]->image_nr_from_timestamp(0);
//        printf("ts ima_nr: %06d ",ima_nr); 
       if(tpar->actcount==0)
       {
        first_ima_nr=ima_nr;
        Count_p0=Count_p2;
       }
       if(tpar->actcount==(tpar->imagecount-1))
        last_ima_nr=ima_nr;
       if(first_ima_nr+tpar->actcount!=ima_nr-miss)
       {
        tpar->tlog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"cam%d  actcount %d missing image %06d != %06d-miss %d",camnum,tpar->actcount,first_ima_nr+tpar->actcount,ima_nr,miss);
        miss=ima_nr-(first_ima_nr+tpar->actcount);
       }
//       printf("m %02d\n",m); 
      }
      else
      {
       tpar->tlog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"cam%d  buf%02d error status %08x",camnum,test,Cbuf[test]->get_status());
       errcount++;
      }

/*
      if(tpar->actcount%tpar->displaycount==0)
      {
       tpar->Cam->winthread->Set_Actual_pic(tpar->Cam->pic[test]);
       tpar->Cam->winthread->Convert();
      }
*/
      tpar->actcount++;
      err=tpar->Cam->setup_buffer(Cbuf[test]);
      if(err!=PCO_NOERROR)
      {
       tpar->tlog->writelog(ERROR_M|STDOUT_M|DBGOUT_M,"cam%d  \nerror 0x%xin setup_buffer %d",camnum,err,test);
       printf("\nerror in PCO_AddBuffer\n");
       break;
      }
     }
     else
      break;
     test++;
     if(test>=MAXBUF)
      test=0;
    }
    next=test;
    if(m>1)
     tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"\ncam%d: multi %d at tpar->actcount %d\n",camnum,m,tpar->actcount);
//    fflush(stdout);
   }//end for imacount
  }

  if(tpar->imagecount>1)
  {
   DWORD fx= tpar->Cam->get_RXFrameCounter();

   QueryPerformanceCounter((LARGE_INTEGER*)&Count_p2);
   mtime=(double)(Count_p2-Count_p0);
   mtime=mtime/lpFrequency;
   mtime*=1000;

   printf("\n");
   tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d  first %d last %d fx %d",camnum,first_ima_nr,last_ima_nr,fx);
   tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: %d images grabbed, %d images missing, %d image failed",camnum,last_ima_nr-first_ima_nr+1,miss,errcount);

   freq=(last_ima_nr-first_ima_nr)*1000/mtime;
   tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: %.3fs freq: %.3f %.3fMB/sec",camnum,mtime/1000,freq,(freq*width*height*2)/(1024*1024)); 
  }

  Sleep(100);
  QueryPerformanceCounter((LARGE_INTEGER*)&Count_p1);
  err=tpar->Cam->stop_acquisition();
  QueryPerformanceCounter((LARGE_INTEGER*)&Count_p2);
  mtime=(double)(Count_p2-Count_p1);
  mtime=mtime/lpFrequency;
  mtime*=1000;
  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: stop_acquisition done 0x%x in %.1fms",camnum,err,mtime);
  if(mtime>300)
  {
   tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: WARNING stop_acquisition time >300  %.1fms",camnum,mtime);
  }

  err=tpar->Cam->cancel_buffers();
  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: cancel images done 0x%x",camnum,err);

  
  for(int k=0;k<bufnum;k++)
  {
   if(Cbuf[k])
    delete Cbuf[k];
  }

/*
  err=PCO_GetPendingBuffer(tpar->Cam->hCamera,&count);
  GetLocalTime(&st);
  printf("%02d:%02d:%02d.%03d ",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
  printf("pending buffer after Cancel %d\n",count);
  if(count>0)
  {
   err=PCO_CancelImages(tpar->Cam->hCamera);
   printf("PCO_CancelImages() done 0x%x\n",err);
  }
*/
  SetEvent(tpar->start_event);
  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: thread is closing",camnum,err);
  return 0;
}


DWORD WINAPI command_thread(LPVOID param)
{
  DWORD err,waitstat;
  WORD recstate;
  WORD camtyp;
  DWORD serialnumber;
  SHORT CCDTemp,CamTemp,ExtTemp;
  int camnum,again;
  HANDLE waitobj[2];

  threadpar* tpar;
  tpar=(threadpar*)param;
  camnum=tpar->Cam->get_camnum();

  again=0;
  waitobj[0]=tpar->loop_event;
  waitobj[1]=tpar->start_event;

  tpar->imagecount=0;
  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d  command thread is starting",tpar->Cam->get_camnum());
  while(TRUE)
  {
   tpar->tlog->writelog(INIT_M|DBGOUT_M,"cam%d commands",camnum);
   err=tpar->Cam->PCO_GetCameraType(&camtyp,&serialnumber);
//   if(err==PCO_NOERROR)
//   tpar->Cam->PCO_GetDescription();
   if(err==PCO_NOERROR)
    err=tpar->Cam->PCO_GetTemperature(&CCDTemp,&CamTemp,&ExtTemp);
   if(err==PCO_NOERROR)
    err=tpar->Cam->PCO_GetRecordingState(&recstate);
   if(err==PCO_NOERROR)
    err=tpar->Cam->PCO_GetRecordingState(&recstate);
   if(err==PCO_NOERROR)
    err=tpar->Cam->PCO_GetRecordingState(&recstate);
   if(err!=PCO_NOERROR)
   {
    tpar->tlog->writelog(ERROR_M|DBGOUT_M,"cam%d commands err 0x%x",camnum,err);
    tpar->imagecount=err;
   }
   waitstat=WaitForMultipleObjects(2,waitobj,FALSE,tpar->stime);
//   waitstat=WaitForSingleObject(tpar->loop_event,tpar->stime);
   if(waitstat==WAIT_OBJECT_0)
   {
    tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d command thread break loop",camnum);
    break;
   }
   else if(waitstat==WAIT_OBJECT_0+1)
   {
    if(again++>3)
    {
     ResetEvent(tpar->start_event);
     again=0;
    }
    tpar->tlog->writelog(INIT_M|DBGOUT_M,"cam%d command thread start_event %d",camnum,again);
   }
  }
  SetEvent(tpar->start_event);
  tpar->tlog->writelog(INIT_M|STDOUT_M|DBGOUT_M,"cam%d: command thread is closing",camnum);
  return 0;
}


void set_timer(FGHANDLE fgHandle, float freq)
{
  float time = (1 / freq) * 1000000;

  FGSTATUS fg_status;
  fg_status=KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "TimerSelector", "Timer0");
  fg_status=KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "TimerTriggerSource", "KY_CONTINUOUS");
//  fg_status=KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "TimerTriggerSource", "KY_SOFTWARE");  ??
  fg_status=KYFG_SetGrabberValueFloat(fgHandle, "TimerDelay", time / 2);
  fg_status=KYFG_SetGrabberValueFloat(fgHandle, "TimerDuration", time / 2);

}


void set_trigger(FGHANDLE fgHandle, int port, CPco_Log* tlog)
{

  FGSTATUS fg_status;
  char txt[20];
  int val;
  fg_status = FGSTATUS_OK;

  if (fg_status == FGSTATUS_OK)
  {
   val = port;
   fg_status = KYFG_SetGrabberValueInt(fgHandle, "PulseMessageSelector", val);
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageSelector %d done", port, val);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageSelector %d failed", port, val);
  }

  if (fg_status == FGSTATUS_OK)
  {
   fg_status = KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "PulseMessageEnable", "On");
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageEnable On done", port);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageEnable On failed", port);
  }

  if (fg_status == FGSTATUS_OK)
  {
   sprintf_s(txt, sizeof(txt), "FallingEdge");

   fg_status = KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "PulseMessageActivation", txt);
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageActivation %s done", port, txt);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageActivation %s failed", port, txt);
  }

  sprintf_s(txt, sizeof(txt), "KY_TIMER_ACTIVE_0");
  if (fg_status == FGSTATUS_OK)
  {
   fg_status = KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "PulseMessageSource", txt);
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageSource %s done", port, txt);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageSource %s failed", port, txt);
  }

  if (fg_status == FGSTATUS_OK)
  {
   val = 1 << port;
   fg_status = KYFG_SetGrabberValueInt(fgHandle, "PulseMessageLinkMaskEnable", val);
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageLinkMaskEnable 0x%04x done", port, val);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageLinkMaskEnable 0x%04x failed", port, val);
  }

  if (fg_status == FGSTATUS_OK)
  {
   fg_status = KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "PulseMessagePulseMode", "Mode1");
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessagePulseMode Mode1", port);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessagePulseMode Mode1", port);
  }


  //Second message, trailing edge of trigger signal set to off
  if (fg_status == FGSTATUS_OK)
  {
   val = port + 4;
   fg_status = KYFG_SetGrabberValueInt(fgHandle, "PulseMessageSelector", val);
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageSelector %d done", port, val);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageSelector %d failed", port, val);
  }

  if (fg_status == FGSTATUS_OK)
  {
   fg_status = KYFG_SetGrabberValueEnum_ByValueName(fgHandle, "PulseMessageEnable", "Off");
   if (fg_status == FGSTATUS_OK)
    tlog->writelog(PROCESS_M, __FUNCTION__": port%d PulseMessageEnable Off done", port);
   else
    tlog->writelog(ERROR_M, __FUNCTION__": port%d PulseMessageEnable Off failed", port);
  }
}

