#include "stdafx.h"

#include "Cpco_camera.h"


void Extract_Line_10_16_MSBaligned(int width,void *bufout,void* bufin);
void Extract_Line_10_16_LSBaligned(int width,void *bufout,void* bufin);


CPco_Camera::CPco_Camera()
{
  cam_num=-1;
  camHandle=0;
  streamHandle=0;
  streamBufferHandles=NULL;
  picin_event=stop_event=NULL;
  packeddatamode=0;

  act_bufnum=16;  //use 16 queued buffers

  nr_of_buffer=0;
  thread_run=0;
  act_width=act_height=act_bitpix=0;
  acqflag=0;
  stopflag=0;

  clog=NULL;
  Camera_Mutex=NULL;
  Grabber_StopEvent=NULL;

  com_timeout=200;

  QueryPerformanceFrequency((LARGE_INTEGER*)&lpFrequency);
  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount_p2);
  lpPCount_p1=lpPCount_p2;

  InitializeCriticalSection(&inbuf_queue_crisec);
  InitializeCriticalSection(&qbuf_queue_crisec);
}

CPco_Camera::~CPco_Camera()
{
  close();
  DeleteCriticalSection(&inbuf_queue_crisec);
  DeleteCriticalSection(&qbuf_queue_crisec);
}

DWORD CPco_Camera::open(CAMHANDLE cam_handle,int num)
{

  if(FGSTATUS_OK == KYFG_CameraOpen2(cam_handle, 0))
  {
   KYFGCAMERA_INFO info;
   cam_num=num;
   camHandle=cam_handle;
   sprintf(porttxt,"Cam_%01d",cam_num);
   writelog(INIT_M,"Cam_%01d was connected successfully handle 0x%x",cam_num,cam_handle);
   get_camera_bootpar();

   KYFG_CameraInfo(camHandle,&info);
   port=info.master_link;
   writelog(INIT_M,"%s info masterlink 0x%x mask 0x%x",porttxt,info.master_link,info.link_mask);

   Camera_Mutex=CreateMutex(NULL,0,NULL); 

   get_sizes(&act_width,&act_height,&act_bitpix);
   writelog(INIT_M,"%s Resolution: %dx%d bitpix %d",porttxt,act_width,act_height,act_bitpix);

   KYFG_GetGrabberValue(camHandle,"PackedDataMode",&packeddatamode);
   writelog(INIT_M,"%s PackedDataMode is 0x%x ",porttxt,packeddatamode);

   return 0;

  }
  return -1;
}

DWORD CPco_Camera::close()
{

  if(Camera_Mutex)
  {
   CloseHandle(Camera_Mutex);
   Camera_Mutex=NULL;
  }

  if((cam_num>=0)&&(camHandle>0))
  {
   if(FGSTATUS_OK == KYFG_CameraClose(camHandle))
   {
    writelog(INIT_M,"Cam_%01d was closed successfully",cam_num);
    camHandle=0;
    cam_num=-1;
   }
  }
  return 0;
}


DWORD CPco_Camera::start_acquisition()
{
  FGSTATUS fg_status;

  acquired_images=0;

  if(streamHandle!=0)
  {
   uint64_t val;
   __int64 t1,t2;
   double tim;
   WORD recstate;

   if(Camera_Mutex)
   {
    DWORD waitstat;
    waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+500);
    if(waitstat!=WAIT_OBJECT_0)
    {
     writelog(ERROR_M,__FUNCTION__": %s enter camera_mutex failed",porttxt);
     return PCO_ERROR_DRIVER_CAMERA_BUSY;
    }
    writelog(PROCESS_M,__FUNCTION__": %s enter camera_mutex done",porttxt);
   }

   fg_status=KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_INPUT);
   writelog(PROCESS_M,"%s KYFG_BufferQueueAll returned 0x%x",porttxt,fg_status);

   val=1;
   fg_status= KYFG_SetGrabberValueInt(camHandle,"StatisticsCountersReset",val);
   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s set StatisticsCountersReset done %d",porttxt,val);
   else
    writelog(ERROR_M,__FUNCTION__": %s set StatisticsCountersReset failed err 0x%08x",porttxt,fg_status);
   

   QueryPerformanceCounter((LARGE_INTEGER*)&t2);
   fg_status=KYFG_CameraStart(camHandle,streamHandle, 0);
   QueryPerformanceCounter((LARGE_INTEGER*)&t1);
   tim=(double)(t1-t2);
   tim=tim/lpFrequency;
   tim*=1000;

   if(tim>max_starttime)
    max_starttime=tim;

   writelog(PROCESS_M,__FUNCTION__": %s KYFG_CameraStart() time %2.6f maxtime %2.6f",porttxt,tim,max_starttime);

   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s CameraStart done",porttxt);
   else
    writelog(ERROR_M,__FUNCTION__": %s CameraStart failed 0x%x",porttxt,fg_status);

   val=1234;
   fg_status= KYFG_GetGrabberValue(camHandle,"DropPacketCounter",&val);
   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s get DropPacketCounter done %d",porttxt,val);
   else
    writelog(ERROR_M,__FUNCTION__": %s get DropPacketCounter failed err 0x%08x",porttxt,fg_status);

   val=1234;
   fg_status= KYFG_GetGrabberValue(camHandle,"DropFrameCounter",&val);
   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s get DropFrameCounter done %d",porttxt,val);
   else
    writelog(ERROR_M,__FUNCTION__": %s get DropFrameCounter failed err 0x%08x",porttxt,fg_status);


   if(Camera_Mutex)
   {
    ReleaseMutex(Camera_Mutex);
    writelog(PROCESS_M,__FUNCTION__": %s leave camera_mutex done",porttxt);
   }
   lpPCount_ima1=lpPCount_ima2=t1;
  }
  return 0;
}

DWORD CPco_Camera::stop_acquisition()
{
  FGSTATUS fg_status;
  __int64 t1,t2;
  double tim;

//  DWORD old_com_timeout;

  if(streamHandle!=0)
  {
   uint64_t val;

   if(Camera_Mutex)
   {
    DWORD waitstat;
    waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+500);
    if(waitstat!=WAIT_OBJECT_0)
    {
     writelog(ERROR_M,__FUNCTION__": %s enter camera_mutex failed",porttxt);
     return PCO_ERROR_DRIVER_CAMERA_BUSY;
    }
    writelog(PROCESS_M,__FUNCTION__": %s enter camera_mutex done",porttxt);
   }

   if(Grabber_StopEvent)
   {
    DWORD waitstat;
    waitstat=WaitForSingleObject(Grabber_StopEvent,100);
    if(waitstat!=WAIT_OBJECT_0)
    {
     writelog(ERROR_M,__FUNCTION__": %s wait Grabber_StopEvent failed",porttxt);
     return PCO_ERROR_DRIVER_CAMERA_BUSY;
    }
    ResetEvent(Grabber_StopEvent);
    writelog(PROCESS_M,__FUNCTION__": %s wait and ResetEvent Grabber_StopEvent done",porttxt);
   }

   stopflag|=FLAG_STOP_PENDING;

/*
   if(acqflag&FLAG_ADD_STOPTIME)
   {
    old_com_timeout=com_timeout;
    com_timeout+=400;
   }

   SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);//THREAD_PRIORITY_HIGHEST);
   writelog(PROCESS_M,"%s SetThreadPriority %d done com_timeout %d old %d",porttxt,THREAD_PRIORITY_ABOVE_NORMAL,com_timeout,old_com_timeout); //THREAD_PRIORITY_HIGHEST);
*/
   QueryPerformanceCounter((LARGE_INTEGER*)&t2);
   fg_status=KYFG_CameraStop(camHandle);
   QueryPerformanceCounter((LARGE_INTEGER*)&t1);
   tim=(double)(t1-t2);
   tim=tim/lpFrequency;
   tim*=1000;
   writelog(PROCESS_M,__FUNCTION__": %s KYFG_CameraStop() time %2.6f",porttxt,tim);

   if(tim>max_stoptime)
    max_stoptime=tim;

   writelog(PROCESS_M,"%s KYFG_CameraStop returned 0x%x time %2.6f maxtime %2.6f",porttxt,fg_status,tim,max_stoptime);

   stopflag&=~FLAG_STOP_PENDING;
//   for(int a=0;a<3;a++)
   {
    fg_status= KYFG_GetGrabberValue(camHandle,"DropPacketCounter",&val);
    if(fg_status==FGSTATUS_OK)
     writelog(PROCESS_M,__FUNCTION__": %s get DropPacketCounter done %d",porttxt,val);
    else
     writelog(ERROR_M,__FUNCTION__": %s get DropPacketCounter failed err 0x%08x",porttxt,fg_status);

    fg_status= KYFG_GetGrabberValue(camHandle,"DropFrameCounter",&val);
    if(fg_status==FGSTATUS_OK)
     writelog(PROCESS_M,__FUNCTION__": %s get DropFrameCounter done %d",porttxt,val);
    else
     writelog(ERROR_M,__FUNCTION__": %s get DropFrameCounter failed err 0x%08x",porttxt,fg_status);

    fg_status= KYFG_GetGrabberValue(camHandle,"RXFrameCounter",&val);
    if(fg_status==FGSTATUS_OK)
     writelog(PROCESS_M,__FUNCTION__": %s get RXFrameCounter done %d",porttxt,val);
    else
     writelog(ERROR_M,__FUNCTION__": %s get RXFrameCounter failed err 0x%08x",porttxt,fg_status);
   }

/*
   if(acqflag&FLAG_ADD_STOPTIME)
   {
    if(old_com_timeout+400>=com_timeout)
     com_timeout-=400;
   }
*/
   if(Grabber_StopEvent)
   {
    SetEvent(Grabber_StopEvent);
    writelog(PROCESS_M,__FUNCTION__": %s SetEvent GrabberStopEvent done",porttxt);
   }

   if(Camera_Mutex)
   {
    ReleaseMutex(Camera_Mutex);
    writelog(PROCESS_M,__FUNCTION__": %s leave camera_mutex done",porttxt);
   }
  }
  return 0;
}

DWORD CPco_Camera::get_RXFrameCounter()
{
  FGSTATUS fg_status;
  uint32_t RXFramecounter;
  uint32_t val;

  fg_status= KYFG_GetGrabberValue(camHandle,"RXFrameCounter",&RXFramecounter);
  if(fg_status==FGSTATUS_OK)
   writelog(PROCESS_M,__FUNCTION__": %s get RXFrameCounter done %d",porttxt,RXFramecounter);
  else
   writelog(ERROR_M,__FUNCTION__": %s get RXFrameCounter failed err 0x%08x",porttxt,fg_status);

  fg_status= KYFG_GetGrabberValue(camHandle,"DropFrameCounter",&val);
  if(fg_status==FGSTATUS_OK)
   writelog(PROCESS_M,__FUNCTION__": %s get DropFrameCounter done %d",porttxt,val);
  else
   writelog(ERROR_M,__FUNCTION__": %s get DropFrameCounter failed err 0x%08x",porttxt,fg_status);
  return RXFramecounter;
}


DWORD CPco_Camera::set_acquire_size(int width,int height,int bitpix)
{
//get some camera information
  int line_width,dmalength;
  uint32_t val;
  DWORD err=PCO_NOERROR;
  uint64_t val1;
  FGSTATUS fg_status;

//  if((act_width!=width)||(act_height!=height))
  {
   if(Camera_Mutex)
   {
    DWORD waitstat;
    waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+500);
    if(waitstat!=WAIT_OBJECT_0)
    {
     writelog(ERROR_M,__FUNCTION__": %s enter camera_mutex failed",porttxt);
     return PCO_ERROR_DRIVER_CAMERA_BUSY;
    }
    writelog(PROCESS_M,__FUNCTION__": %s enter camera_mutex done",porttxt);
   }


//set grabber image parameter
   fg_status=KYFG_GetGrabberValue(camHandle, "PackedDataMode",&val1);
   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s actual PackedDataMode is 0x%08x",porttxt,val1);
   else
    writelog(ERROR_M,__FUNCTION__": %s get PackedDataMode failed",porttxt);

//calculate line_width in bits
   if(val1==0)
   {
    line_width=width*((act_bitpix+7)/8)*8;
   }
   else
    line_width=width*act_bitpix;

   if(line_width%32)
    writelog(ERROR_M,__FUNCTION__": %s line_width must be a multiple of 32 bits per line %d",porttxt,line_width);

//@@@ set line_width oder width
   fg_status=KYFG_SetGrabberValueInt(camHandle, "Width",width);
   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s Width set to %d",porttxt,width);
   else
    writelog(ERROR_M,__FUNCTION__": %s set Width failed",porttxt);

   fg_status=KYFG_GetGrabberValue(camHandle, "Width",&val1);
   if(fg_status==FGSTATUS_OK)
    writelog(INTERNAL_1_M,__FUNCTION__": %s actual Width is %d 0x%08x",porttxt,val1,val1);
   else
    writelog(ERROR_M,__FUNCTION__": %s get Width failed",porttxt);

   fg_status=KYFG_SetGrabberValueInt(camHandle, "Height",height);
   if(fg_status==FGSTATUS_OK)
    writelog(PROCESS_M,__FUNCTION__": %s Height set to %d",porttxt,height);
   else
    writelog(ERROR_M,__FUNCTION__": %s set Height failed",porttxt);

   fg_status=KYFG_GetGrabberValue(camHandle, "Height",&val1);
   if(fg_status==FGSTATUS_OK)
    writelog(INTERNAL_1_M,__FUNCTION__": %s actual Height is %d 0x%08x",porttxt,val1,val1);
   else
    writelog(ERROR_M,__FUNCTION__": %s get Height failed",porttxt);



   line_width/=8;
   dmalength=(line_width*height);
   writelog(PROCESS_M,__FUNCTION__": %s line_width %d, dmalength %d",porttxt,line_width,dmalength);

   fg_status=KYFG_SetCameraValueInt(camHandle, "Width",width);
   fg_status=KYFG_SetCameraValueInt(camHandle, "Height",height);

   val=KYFG_GetCameraValueInt(camHandle,"PayloadSize");


//   act_line_width=line_width;
   act_width=width;
   act_height=height;

   if(Camera_Mutex)
   {
    ReleaseMutex(Camera_Mutex);
    writelog(PROCESS_M,__FUNCTION__": %s leave camera_mutex done",porttxt);
   }
  }


  return 0;
}



DWORD CPco_Camera::setup_buffer(CPco_Buffer* imabuf)
{
  EnterCriticalSection(&inbuf_queue_crisec);
  ResetEvent(imabuf->get_event());
  imabuf->set_status(PCO_ERROR_DRIVER_BUFFERS_PENDING);
  inbuf_queue.push(imabuf);
  writelog(BUFFER_M,__FUNCTION__": %s imabuf %p ev %d",porttxt,imabuf,imabuf->get_event());
  LeaveCriticalSection(&inbuf_queue_crisec);
  return 0;
}

DWORD CPco_Camera::cancel_buffers()
{
  CPco_Buffer* imabuf;
  EnterCriticalSection(&inbuf_queue_crisec);
  while(!inbuf_queue.empty())
  {
   imabuf=inbuf_queue.front();
   inbuf_queue.pop();
   imabuf->set_status(PCO_ERROR_DRIVER_BUFFER_CANCELLED);
   SetEvent(imabuf->get_event());
   writelog(BUFFER_M,__FUNCTION__": %s cancel imabuf %p ev %d",porttxt,imabuf,imabuf->get_event());
  }
  LeaveCriticalSection(&inbuf_queue_crisec);
  return 0;
}

void streambuffer_callback_func(STREAM_BUFFER_HANDLE streamBufferHandle,void* userContext)
{
  CPco_Camera*Ccam;
  Ccam=(CPco_Camera*)userContext;
//  _RPT1(_CRT_WARN,"streambuffer_callback_func: userContext %p\n",userContext);
  return Ccam->queuedbuffer_callback(streamBufferHandle);
}

void CPco_Camera::queuedbuffer_callback(STREAM_BUFFER_HANDLE streamBufferHandle)
{
  if(0 ==streamBufferHandle)                                                // callback with indicator for acquisition stop
  {
   writelog(PROCESS_M,__FUNCTION__": %s streamBufferHandle 0 = Acquisition Stopped buf_queue.size %d set CancelEvent",porttxt,qbuf_queue.size());
   EnterCriticalSection(&qbuf_queue_crisec);
   while(!qbuf_queue.empty())
    qbuf_queue.pop();
   acquired_images=0;
   LeaveCriticalSection(&qbuf_queue_crisec);
   return;
  }

  EnterCriticalSection(&qbuf_queue_crisec);
  qbuf_queue.push(streamBufferHandle);
  acquired_images++;
  LeaveCriticalSection(&qbuf_queue_crisec);
/*
  FGSTATUS fg_status;
  unsigned char* adr=NULL;
  int index=-1;
  fg_status=KYFG_BufferGetInfo(streamBufferHandle, KY_STREAM_BUFFER_INFO_BASE, &adr, NULL, NULL);
  if(fg_status!=FGSTATUS_OK)
   writelog(ERROR_M,__FUNCTION__": %s KYFG_BufferGetInfo failed 0x%08x",porttxt,fg_status);
  fg_status=KYFG_BufferGetInfo(streamBufferHandle, KY_STREAM_BUFFER_INFO_ID, &index, NULL, NULL);
  if(fg_status!=FGSTATUS_OK)
   writelog(ERROR_M,__FUNCTION__": %s KYFG_BufferGetInfo failed 0x%08x",porttxt,fg_status);

  writelog(BUFFER_M,__FUNCTION__":  %s streamBufferHandle 0x%016llx ok index %d adr %p buf_queue.size() %d",porttxt,streamBufferHandle,index,adr,qbuf_queue.size());
*/
  if(picin_event)
   SetEvent(picin_event);
}


DWORD CPco_Camera::image_acquire_thread()
{
  HANDLE waitobj[2];
  int waittime;
  DWORD waitstat;
  int ima_nr;
  int corr_flow;
  uint32_t RXFrameCounter;

  double time;
  FGSTATUS fg_status;
  CPco_Buffer* imabuf;

  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount_ima2);
  lpPCount_ima1=lpPCount_ima2;


  waittime=60000;
  while(thread_run)
  {
   waitobj[0]=picin_event;
   waitobj[1]=stop_event;

   waitstat=WaitForMultipleObjects(2,waitobj,FALSE,waittime);//PCO_SC2_IMAGE_TIMEOUT_L*2);
   switch(waitstat)
   {
    case WAIT_OBJECT_0+1: //stop
     writelog(INIT_M,__FUNCTION__": %s stop_event break_loop",porttxt);
     thread_run=FALSE;
    break;

    case WAIT_OBJECT_0: //picin
    {
     STREAM_BUFFER_HANDLE streamBufferHandle=0;
     void* bufin=0;
     int index=-1;

     ResetEvent(waitobj[0]);
     while(qbuf_queue.size()>=1)
     {
      imabuf=NULL;

      EnterCriticalSection(&qbuf_queue_crisec);
      streamBufferHandle=qbuf_queue.front();
      LeaveCriticalSection(&qbuf_queue_crisec);
     

      QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount_ima2);
      time=(double)(lpPCount_ima2-lpPCount_ima1);
      time=time/lpFrequency;
      time*=1000;
      lpPCount_ima1=lpPCount_ima2;
      if(acquired_images==1)
      {
       double time_rec;
       time_rec=(double)(lpPCount_ima2-lpPCount_rec_on);
       time_rec=time_rec/lpFrequency;
       time_rec*=1000;
       writelog(PROCESS_M|BUFFER_M|STDOUT_M,__FUNCTION__": %s ImageInEvent set %4.6f %4.6f 0x%016llx count %d",porttxt,time,time_rec,streamBufferHandle,acquired_images);
      }
      else
       writelog(BUFFER_M,__FUNCTION__": %s ImageInEvent set %4.6f 0x%016llx count %d",porttxt,time,streamBufferHandle,acquired_images);

      fg_status=KYFG_BufferGetInfo(streamBufferHandle, KY_STREAM_BUFFER_INFO_BASE, &bufin, NULL, NULL);
      if(fg_status!=FGSTATUS_OK)
       writelog(ERROR_M,__FUNCTION__": %s KYFG_BufferGetInfo failed 0x%08x",porttxt,fg_status);

      fg_status=KYFG_BufferGetInfo(streamBufferHandle, KY_STREAM_BUFFER_INFO_ID, &index, NULL, NULL);
      if(fg_status!=FGSTATUS_OK)
       writelog(ERROR_M,__FUNCTION__": %s KYFG_BufferGetInfo failed 0x%08x",porttxt,fg_status);


      if(camtype==0x1500)
      {
       if(packeddatamode==0)
        ima_nr=image_nr_from_timestamp(bufin,0);
       else
        ima_nr=image_nr_from_timestamp_c(bufin,0);
       if((acquired_images==1))
       {
        if(ima_nr==2)
        {
         corr_flow=1;
         writelog(BUFFER_M,__FUNCTION__": %s set corr_flow to 1",porttxt);
        }
        else
        {
         corr_flow=0;
         writelog(BUFFER_M,__FUNCTION__": %s set corr_flow to 0",porttxt);
        }
       }
       ima_nr-=corr_flow;
      }
      else
       ima_nr=image_nr_from_timestamp(bufin,0);
      writelog(BUFFER_M,__FUNCTION__":  %s Info ok index %d adr %p ts %d buf_queue.size() %d  ",porttxt,index,bufin,ima_nr,qbuf_queue.size());
      EnterCriticalSection(&inbuf_queue_crisec);
      if(!inbuf_queue.empty())
      {
       imabuf=inbuf_queue.front();
       inbuf_queue.pop();
       LeaveCriticalSection(&inbuf_queue_crisec);

       if(imabuf)
       {
        if(camtype==0x1500) 
         imabuf->copy_10(bufin);
        else
         imabuf->copy_16(bufin);
      
        imabuf->set_status(PCO_NOERROR);
        SetEvent(imabuf->get_event());
        writelog(BUFFER_M,__FUNCTION__": %s copy done imabuf %p ev %d",porttxt,imabuf,imabuf->get_event());
       }
      }
      else
      {
       LeaveCriticalSection(&inbuf_queue_crisec);
       writelog(BUFFER_M,__FUNCTION__": %s no output buffer break while ",porttxt);
       break;
      }

      EnterCriticalSection(&qbuf_queue_crisec);
      if(qbuf_queue.size()>=1)
       qbuf_queue.pop();
      LeaveCriticalSection(&qbuf_queue_crisec);

//      if((acquired_images>0)&&(ima_nr!=acquired_images-qbuf_queue.size()))
//       writelog(ERROR_M,__FUNCTION__": %s timestamp of image %d is not equal acquired_images %d",porttxt,ima_nr,acquired_images);

      if((stopflag&FLAG_STOP_PENDING)==0)
      {
       fg_status=KYFG_BufferToQueue(streamBufferHandle, KY_ACQ_QUEUE_INPUT);
       if(fg_status!=FGSTATUS_OK)
        writelog(ERROR_M,__FUNCTION__": %s KYFG_BufferToQueue KY_ACQ_QUEUE_INPUT failed 0x%08x",porttxt,fg_status);
       else
        writelog(BUFFER_M,__FUNCTION__": %s KYFG_BufferToQueue 0x%016llx done buf_queue.size() %d",porttxt,streamBufferHandle,qbuf_queue.size());
      }
     }
     writelog(BUFFER_M,__FUNCTION__": %s end while",porttxt);
    }
    break;

    default:
     writelog(PROCESS_M,__FUNCTION__": %s wait timeout",porttxt);
   }
  }

  thread_run=FALSE;
  writelog(INIT_M,__FUNCTION__": %s return from thread",porttxt);
  return 0;
}


DWORD WINAPI kaya_acquire_thread(LPVOID param)
{
  CPco_Camera *Ccam;
  Ccam=(CPco_Camera*)param;
  return Ccam->image_acquire_thread();
}

DWORD CPco_Camera::start_acquisition_thread()
{
  FGSTATUS fg_status;
  acquired_images=0;
  nr_of_buffer=0;

  if(Camera_Mutex)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+500);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s enter camera_mutex failed",porttxt);
    return PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
   writelog(PROCESS_M,__FUNCTION__": %s enter camera_mutex done",porttxt);
  }


  fg_status=KYFG_StreamCreate(camHandle, &streamHandle,0);
  if(FGSTATUS_OK != fg_status)
  {
   streamHandle=0;
   writelog(ERROR_M,__FUNCTION__": %s KYFG_StreamCreate failed err 0x%08x",porttxt,fg_status);
  }
  else
  {
   size_t frameDataSize, frameDataAligment;

   fg_status=KYFG_StreamBufferCallbackRegister(streamHandle,streambuffer_callback_func,this);
   if(FGSTATUS_OK != fg_status)
    writelog(ERROR_M,__FUNCTION__": %s KYFG_StreamBufferCallbackRegister failed err 0x%08x",porttxt,fg_status);
   else
    writelog(PROCESS_M,__FUNCTION__": %s KYFG_StreamBufferCallbackRegister done userContext %p",porttxt,this);


   fg_status=KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_PAYLOAD_SIZE, &frameDataSize, NULL, NULL);
   if(FGSTATUS_OK != fg_status)
    writelog(ERROR_M,__FUNCTION__": %s KYFG_StreamGetInfo(...,KY_STREAM_INFO_PAYLOAD_SIZE) failed err 0x%08x",porttxt,fg_status);
   fg_status=KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_BUF_ALIGNMENT, &frameDataAligment, NULL, NULL);
   if(FGSTATUS_OK != fg_status)
    writelog(ERROR_M,__FUNCTION__": %s KYFG_StreamGetInfo(...,KY_STREAM_INFO_BUF_ALIGNMENT) failed err 0x%08x",porttxt,fg_status);

   writelog(PROCESS_M,__FUNCTION__": %s KYFG_StreamGetInfo payload %d alingment 0x%x",porttxt,frameDataSize,frameDataAligment);

   streamBufferHandles=(STREAM_BUFFER_HANDLE*)malloc(sizeof(STREAM_BUFFER_HANDLE)*act_bufnum);
   writelog(PROCESS_M,__FUNCTION__": %s %d streamBufferHandles %p allocated",porttxt,act_bufnum, streamBufferHandles);
   if(streamBufferHandles)
   {
    for(int i = 0; i<act_bufnum;i++)
    {
     void* pBuffer = _aligned_malloc(frameDataSize, frameDataAligment);
     streamBufferHandles[i]=NULL;
     fg_status=KYFG_BufferAnnounce(streamHandle,pBuffer, frameDataSize, NULL,&streamBufferHandles[i]);
     if(FGSTATUS_OK != fg_status)
      writelog(ERROR_M,__FUNCTION__": %s KYFG_BufferAnnounce(...,%p,...[%d])  failed err 0x%08x",porttxt,pBuffer,i,fg_status);
     else
      writelog(PROCESS_M,__FUNCTION__": %s KYFG_BufferAnnounce(...,%p,...) done streamBufferHandles[%d] 0 0x%016llx",porttxt,pBuffer,i,streamBufferHandles[i]);
    }
   }
   nr_of_buffer=act_bufnum;
  }

  if(Camera_Mutex)
  {
   ReleaseMutex(Camera_Mutex);
   writelog(PROCESS_M,__FUNCTION__": %s leave camera_mutex done",porttxt);
  }


  picin_event=CreateEvent(NULL,TRUE,FALSE,NULL);
  stop_event=CreateEvent(NULL,TRUE,FALSE,NULL);

  if(streamHandle>0)
  {
   thread_run=TRUE;
   th_acquire=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE )kaya_acquire_thread,(LPVOID)this,0,NULL);
   if(th_acquire)
    writelog(INIT_M,__FUNCTION__": %s acquire thread %p created",porttxt,th_acquire);
  else
   writelog(INIT_M,__FUNCTION__": %s create acquire thread failed",porttxt);
  }

  return 0;
}

DWORD CPco_Camera::stop_acquisition_thread()
{
  FGSTATUS fg_status;

  stop_acquisition();

  if((thread_run==TRUE)&&(th_acquire))
  {
   thread_run=FALSE;
   SetEvent(stop_event);
   writelog(INIT_M,__FUNCTION__" %s wait 10 seconds until thread %p is closed",porttxt,th_acquire);
   DWORD stat=WaitForSingleObject(th_acquire,10000);
   if(stat==WAIT_OBJECT_0)
    writelog(INIT_M,__FUNCTION__" %s wait for thread %p done",porttxt,th_acquire);
   else
    writelog(ERROR_M,__FUNCTION__" %s wait for thread %p failed",porttxt,th_acquire);
  }

  while(!qbuf_queue.empty())
   qbuf_queue.pop();

  if(Camera_Mutex)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+500);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s enter camera_mutex failed",porttxt);
    return PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
   writelog(PROCESS_M,__FUNCTION__": %s enter camera_mutex done",porttxt);
  }

  if(streamBufferHandles)
  {
   for(int i = 0; i<nr_of_buffer;i++)
   {
    if(streamBufferHandles[i])
    {
     unsigned char* adr=NULL;
     fg_status=KYFG_BufferGetInfo(streamBufferHandles[i], KY_STREAM_BUFFER_INFO_BASE, &adr, NULL, NULL);
     if(FGSTATUS_OK != fg_status)
      writelog(ERROR_M,__FUNCTION__": KYFG_BufferGetInfo(..[%d],KY_STREAM_BUFFER_INFO_BASE) failed err 0x%08x",i,fg_status);
     if(adr)
      _aligned_free(adr);
    }
   }
   free(streamBufferHandles);
   streamBufferHandles=NULL;
   nr_of_buffer=0;
  }

  if(streamHandle>0)
  {
   fg_status=KYFG_StreamBufferCallbackUnregister(streamHandle,streambuffer_callback_func);
   if(FGSTATUS_OK != fg_status)
    writelog(ERROR_M,__FUNCTION__": %s KYFG_StreamBufferCallbackUnregister failed err 0x%08x",porttxt,fg_status);
   fg_status=KYFG_StreamDelete(streamHandle);
   if(FGSTATUS_OK != fg_status)
    writelog(ERROR_M,__FUNCTION__": %s KYFG_StreamDelete failed err 0x%08x",porttxt,fg_status);
   streamHandle=0;
  }

  if(Camera_Mutex)
  {
   ReleaseMutex(Camera_Mutex);
   writelog(PROCESS_M,__FUNCTION__": %s leave camera_mutex done",porttxt);
  }

  return 0;
}


DWORD CPco_Camera::build_checksum(unsigned char *buf,int *size)
{
  unsigned char cks;
  unsigned short *b;
  int x,bsize;

  b=(unsigned short *)buf;//size of packet is second WORD
  b++;
  bsize=*b-1;
  if(bsize>*size)
   return PCO_ERROR_DRIVER_CAMERALINK;
  cks=0;
  for(x=0;x<bsize;x++)
   cks+=buf[x];

  buf[x]=cks;
  *size=x+1;

  return PCO_NOERROR;
}

//return size with checksum
DWORD CPco_Camera::test_checksum(unsigned char *buf,int *size)
{
  unsigned char cks;
  unsigned short *b;
  int x,bsize;
  
  cks=0;
  b=(unsigned short *)buf; //size of packet is second WORD
  b++;
  bsize=(int)*b;
  bsize--;
  if(bsize>*size)
  {
   return PCO_ERROR_DRIVER_CAMERALINK;
  }

  for(x=0;x<bsize;x++)
   cks+=buf[x];

  if(buf[x]!=cks)
  {
   return PCO_ERROR_DRIVER_CAMERALINK;
  }

  *size=x+1;
  return PCO_NOERROR;
}



DWORD CPco_Camera::Control_Command(void *buf_in,DWORD size_in,void *buf_out,DWORD size_out)
{
  DWORD err=PCO_NOERROR;
  unsigned char buffer[PCO_SC2_DEF_BLOCK_SIZE];
  uint32_t size;
  WORD com_in,com_out,data;
  CLHS_GENCP_REG reg;
  FGSTATUS fg_stat;
  int old_com_timeout;

  if(*(WORD*)buf_in==0)
  {
   return PCO_ERROR_DRIVER_CAMERALINK;
  }

  size=*((WORD*)buf_in+1);
  if((size<sizeof(SC2_Simple_Telegram))||(size>=PCO_SC2_DEF_BLOCK_SIZE)||(size>size_in))
  {
   return PCO_ERROR_DRIVER_CAMERALINK;
  }

  writelog(COMMAND_M|PROCESS_M,"%s Control_Command: 0x%0x %d",porttxt,*(WORD*)buf_in,size);

  if(Grabber_StopEvent)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(Grabber_StopEvent,100);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s wait Grabber_StopEvent failed",porttxt);
    return PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
  }

  if(Camera_Mutex)
  {
   DWORD waitstat;
   old_com_timeout=com_timeout;
   waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+100);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s enter camera_Mutex failed",porttxt);
    if(old_com_timeout!=com_timeout)
    {
     writelog(PROCESS_M,__FUNCTION__": %s new com_timeout try again",porttxt);
     waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+100);
     if(waitstat!=WAIT_OBJECT_0)
     {
      writelog(ERROR_M,__FUNCTION__": %s enter camera_Mutex failed",porttxt);
      return PCO_ERROR_DRIVER_CAMERA_BUSY;
     }
    }
    else
     return PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
   writelog(COMMAND_M,__FUNCTION__": %s enter camera_Mutex done",porttxt);
  }

  com_in=*((WORD*)buf_in);
  size=max(size_out,sizeof(SC2_Failure_Response));
  size=PCO_SC2_DEF_BLOCK_SIZE;
  memset(buffer,0,PCO_SC2_DEF_BLOCK_SIZE);

  size=size_in;
  err=build_checksum((unsigned char*)buf_in,(int*)&size);

  reg.adr=0x40000;
//  err=KYFG_WritePortBlock(fghandle,port,reg.adr,buf_in,&size);
  fg_stat=KYFG_CameraWriteReg(camHandle,reg.adr,buf_in,&size);
  if(fg_stat != FGSTATUS_OK)
   writelog(ERROR_M,"%s Control_Command: KYFG_CameraWriteReg failed with %d",porttxt,fg_stat);
  else
   writelog(COMMAND_M,"%s Control_Command: KYFG_CameraWriteReg 0x%04x done",porttxt,com_in);

  if(fg_stat != FGSTATUS_OK)
   goto clhs_com_end;

  reg.adr=0x40000;
  size = 40;
//  err=KYFG_ReadPortBlock(fghandle,port,reg.adr,buffer,&size);
  fg_stat=KYFG_CameraReadReg(camHandle,reg.adr,buffer,&size);
  if(fg_stat != FGSTATUS_OK)
   writelog(ERROR_M,"%s Control_Command: KYFG_ReadReg failed with %d",porttxt,fg_stat);
  else
   writelog(COMMAND_M,"%s Control_Command: KYFG_CameraReadReg done 0x%04x",porttxt,*((WORD*)buffer));

  if(fg_stat != FGSTATUS_OK)
  {
   err=PCO_ERROR_DRIVER_CAMERALINK|PCO_ERROR_DRIVER_DATAERROR;
   goto clhs_com_end;
  }

  if(size==40)
  {
   WORD *b;
   b=(WORD *)buffer; //size of packet is second WORD
   b++;
   size=(int)*b;
   if(size>PCO_SC2_DEF_BLOCK_SIZE)
    size=PCO_SC2_DEF_BLOCK_SIZE;
  }
  else
  {
   writelog(ERROR_M,"%s Control_Command: wrong size %d set to 41 and read again",porttxt,size);
   size=41;
  }

  if(size>40)
  {
   if(size%sizeof(DWORD))
    size=(size/sizeof(DWORD)+1)*sizeof(DWORD);
   writelog(COMMAND_M,"%s Control_Command: read answer again size %d",porttxt,size);
//   err=KYFG_ReadPortBlock(fghandle,port,reg.adr,buffer,&size);
   fg_stat=KYFG_CameraReadReg(camHandle,reg.adr,buffer,&size);
   if(fg_stat != FGSTATUS_OK)
    writelog(ERROR_M,"%s Control_Command: KYFG_ReadReg failed with %d",porttxt,fg_stat);
   else
    writelog(COMMAND_M,"%s Control_Command: KYFG_CameraReadReg done 0x%04x",porttxt,*((WORD*)buffer));

   if(fg_stat != FGSTATUS_OK)
   {
    err=PCO_ERROR_DRIVER_CAMERALINK|PCO_ERROR_DRIVER_DATAERROR;
    goto clhs_com_end;
   }
  }

  com_out=*((WORD*)buffer);
  if(com_in!=(com_out&0xFF3F))
  {
   writelog(ERROR_M,"%s Control_Command: com_in  0x%04x != com_out&0xFF3F 0x%04x",porttxt,com_in,com_out&0xFF3F);
   err=PCO_ERROR_DRIVER_CAMERALINK;
   goto clhs_com_end;
  }

  if((com_out&RESPONSE_ERROR_CODE)==RESPONSE_ERROR_CODE)
  {
   SC2_Failure_Response resp;
   memcpy(&resp,buffer,sizeof(SC2_Failure_Response));
   err=resp.dwerrmess;
   if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
    writelog(INTERNAL_4_M,"%s Control_Command: com 0x%x FIRMWARE_NOT_SUPPORTED",porttxt,com_in);
   else
    writelog(ERROR_M,"%s Control_Command: com 0x%x RESPONSE_ERROR_CODE err 0x%x",porttxt,com_in,err);
  }

  if(err==PCO_NOERROR)
  {
   if(com_out!=(com_in|RESPONSE_OK_CODE))
   {
    err=PCO_ERROR_DRIVER_CAMERALINK;
    writelog(ERROR_M,"%s Control_Command: Data failed com_out 0x%04x should be 0x%04x",porttxt,com_out,com_in|RESPONSE_OK_CODE);
   }
  }

  size=max(size_out,sizeof(SC2_Failure_Response));

  if(test_checksum(buffer,(int*)&size)==PCO_NOERROR)
  {
   size-=1;   
   if(size<(int)size_out)
    size_out=size;
   memcpy(buf_out,buffer,size_out);
  }
  else
   err=test_checksum(buffer,(int*)&size);

  memcpy(&data,buffer+4,sizeof(WORD));

clhs_com_end:
  writelog(COMMAND_M,"%s Control_Command: done 0x%04x data 0x%x err 0x%0x",porttxt,com_out,data,err);
  if(Camera_Mutex)
  {
   ReleaseMutex(Camera_Mutex);
   writelog(COMMAND_M,__FUNCTION__": %s leave camera_Mutex done",porttxt);
  }
  return err;
}


DWORD CPco_Camera::PCO_GetCameraType(WORD *camtype,DWORD *serialnumber)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Camera_Type_Response resp;

  com.wCode=GET_CAMERA_TYPE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  this->camtype=*camtype=resp.wCamType;
  *serialnumber=resp.dwSerialNumber;

  return err;
}

DWORD CPco_Camera::PCO_ArmCamera()
{
  SC2_Arm_Camera_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com_timeout=5000;
  com.wCode=ARM_CAMERA;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Arm_Camera_Response));
  if(err==PCO_NOERROR)
   writelog(COMMAND_M,"%s ARM_CAMERA  done",porttxt);
  com_timeout=200;

  return err;
}


DWORD CPco_Camera::PCO_SetRecordingState(WORD val)
{
  SC2_Recording_State_Response resp;
  SC2_Set_Recording_State com;
  SC2_Simple_Telegram com1;
  DWORD err=PCO_NOERROR;

  writelog(COMMAND_M,"%s SET_RECORDING_STATE %d",porttxt,val);

  com.wCode=SET_RECORDING_STATE;
  com.wSize=sizeof(com);
  com.wState=val;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err!=PCO_NOERROR)
   return err;

  QueryPerformanceCounter((LARGE_INTEGER*)&lpPCount_rec_on);
  for(int i=0;i<10;i++)
  {
   com1.wCode=GET_RECORDING_STATE;
   com1.wSize=sizeof(com1);
   err=Control_Command(&com1,sizeof(com1),&resp,sizeof(resp));
   if(err!=PCO_NOERROR)
    break;
   if(val==resp.wState)
   {
    writelog(COMMAND_M,"%s SET_RECORDING_STATE %d done",porttxt,val);
    break;
   }
  }
  return err;
}

DWORD CPco_Camera::PCO_GetRecordingState(WORD *val)
{
  SC2_Recording_State_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_RECORDING_STATE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
   *val=resp.wState;
  return err;
}

DWORD CPco_Camera::PCO_SetTimestampMode(WORD val)
{
  SC2_Timestamp_Mode_Response resp;
  SC2_Set_Timestamp_Mode com;
  DWORD err=PCO_NOERROR;

  com.wCode=SET_TIMESTAMP_MODE;
  com.wSize=sizeof(com);
  com.wMode=val;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  return err;
}


DWORD CPco_Camera::PCO_GetTemperature(SHORT *sCCDTemp,SHORT *sCAMTemp,SHORT *sExtTemp)
{
  SC2_Temperatures_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_TEMPERATURE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
   *sCCDTemp=resp.sCCDtemp;
   *sCAMTemp=resp.sCamtemp;
   *sExtTemp=resp.sPStemp;
  }
  return err;
}

DWORD CPco_Camera::PCO_GetDescription()
{
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_CAMERA_DESCRIPTION;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&description,sizeof(SC2_Camera_Description_Response));

  if(err!=PCO_NOERROR)
   writelog(ERROR_M,"GET_CAMERA_DESCRIPTION failed with 0x%x",err);

  return err;
}

DWORD CPco_Camera::PCO_GetROI(WORD *RoiX0,WORD *RoiY0,WORD *RoiX1,WORD *RoiY1)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_ROI_Response resp;

  com.wCode=GET_ROI;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
   *RoiX0=resp.wROI_x0;
   *RoiX1=resp.wROI_x1;
   *RoiY0=resp.wROI_y0;
   *RoiY1=resp.wROI_y1;
  }
  return err;
}  

DWORD CPco_Camera::PCO_SetROI(WORD RoiX0,WORD RoiY0,WORD RoiX1,WORD RoiY1)
{
  DWORD err;

  SC2_Set_ROI com;
  SC2_ROI_Response resp;
  com.wCode=SET_ROI;
  com.wSize=sizeof(SC2_Set_ROI);
  com.wROI_x0=RoiX0;
  com.wROI_x1=RoiX1;
  com.wROI_y0=RoiY0;
  com.wROI_y1=RoiY1;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}  


DWORD CPco_Camera::PCO_GetTriggerMode(WORD *mode)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Trigger_Mode_Response resp;

  com.wCode=GET_TRIGGER_MODE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
   *mode=resp.wMode;
    
  return err;
}

DWORD CPco_Camera::PCO_SetTriggerMode(WORD mode)
{
  DWORD err;
  SC2_Set_Trigger_Mode com;
  SC2_Trigger_Mode_Response resp;

  com.wCode=SET_TRIGGER_MODE;
  com.wMode=mode;
  com.wSize=sizeof(SC2_Set_Trigger_Mode);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_Camera::PCO_SetDelayExposure(DWORD delay,DWORD expos)
{
  DWORD err;

  SC2_Set_Delay_Exposure com;
  SC2_Delay_Exposure_Response resp;

  com.wCode=SET_DELAY_EXPOSURE_TIME;
  com.wSize=sizeof(SC2_Set_Delay_Exposure);
  com.dwDelay=delay;
  com.dwExposure=expos;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_Camera::PCO_GetDelayExposure(DWORD *delay,DWORD *expos)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Delay_Exposure_Response resp;

  com.wCode=GET_DELAY_EXPOSURE_TIME;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
   *delay=resp.dwDelay;
   *expos=resp.dwExposure;
  }

  return err;
}

DWORD CPco_Camera::PCO_SetTimebase(WORD delay,WORD expos)
{
  DWORD err;

  SC2_Set_Timebase com;
  SC2_Timebase_Response resp;

  com.wCode=SET_TIMEBASE;
  com.wSize=sizeof(SC2_Set_Timebase);
  com.wTimebaseDelay=delay;
  com.wTimebaseExposure=expos;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}


DWORD CPco_Camera::PCO_GetTimebase(WORD *delay,WORD *expos)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Timebase_Response resp;

  com.wCode=GET_TIMEBASE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
   *delay=resp.wTimebaseDelay;
   *expos=resp.wTimebaseExposure;
  }

  return err;
}


DWORD CPco_Camera::PCO_ResetSettingsToDefault()
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Reset_Settings_To_Default_Response resp;

  com.wCode=RESET_SETTINGS_TO_DEFAULT;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_Camera::PCO_GetBitAlignment(WORD *align)
{
  SC2_Bit_Alignment_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_BIT_ALIGNMENT;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  *align=resp.wAlignment;
  return err;

}

DWORD CPco_Camera::PCO_SetBitAlignment(WORD align)
{
  DWORD err;

  SC2_Set_Bit_Alignment com;
  SC2_Bit_Alignment_Response resp;

  com.wCode=SET_BIT_ALIGNMENT;
  com.wAlignment=align;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}


DWORD CPco_Camera::PCO_GetHWIOSignal(WORD SignalNum,WORD *Enabled,WORD *Type,WORD *Polarity,WORD *FilterSetting,WORD *Selected)
{
  DWORD err;
  SC2_Get_HW_IO_Signal com;
  SC2_Get_HW_IO_Signal_Response resp;

  com.wCode=GET_HW_IO_SIGNAL;
  com.wSize=sizeof(SC2_Get_HW_IO_Signal);
  com.wNumSignal=SignalNum;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
  {
    if(Enabled)
      *Enabled=resp.wEnabled;
    if(Type)
      *Type=resp.wType;
    if(Polarity)
      *Polarity=resp.wPolarity;
    if(FilterSetting)
      *FilterSetting=resp.wFilterSetting;
    if(Selected)
      *Selected=resp.wSelected;
  }

  return err;
}

DWORD CPco_Camera::PCO_SetHWIOSignal(WORD SignalNum,WORD Enabled,WORD Type,WORD Polarity,WORD FilterSetting,WORD Selected)
{
  DWORD err;
  SC2_Set_HW_IO_Signal com;
  SC2_Set_HW_IO_Signal_Response resp;

  com.wCode=SET_HW_IO_SIGNAL;
  com.wSize=sizeof(SC2_Set_HW_IO_Signal);
  com.wNumSignal=SignalNum;
  com.wEnabled=Enabled;
  com.wType=Type;
  com.wPolarity=Polarity;
  com.wFilterSetting=FilterSetting;
  com.wSelected=Selected;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    if(Enabled!=resp.wEnabled)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      writelog(ERROR_M,"returned wrong Enable status %d!=%d",Enabled,resp.wEnabled);
    }
    if(Polarity!=resp.wPolarity)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      writelog(ERROR_M,"returned wrong Polarity %d!=%d",Polarity,resp.wPolarity);
    }
    if(FilterSetting!=resp.wFilterSetting)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      writelog(ERROR_M,"returned wrong FilterSetting %d!=%d",FilterSetting,resp.wFilterSetting);
    }
    if(Selected!=resp.wSelected)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      writelog(ERROR_M,"returned wrong Selected %d!=%d",Selected,resp.wSelected);
    }
  }

  return err;
}

DWORD CPco_Camera::PCO_GetHWIOSignalDescriptor(WORD SignalNum,SC2_Get_HW_IO_Signal_Descriptor_Response *SignalDesc)
{
  DWORD err;
  SC2_Get_HW_IO_Signal_Descriptor com;
  SC2_Get_HW_IO_Signal_Descriptor_Response resp;

  com.wCode=GET_HW_IO_SIGNAL_DESCRIPTION;
  com.wSize=sizeof(SC2_Get_HW_IO_Signal_Descriptor);
  com.wNumSignal=SignalNum;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
    memcpy(SignalDesc,&resp,sizeof(SC2_Get_HW_IO_Signal_Descriptor_Response));
  else
  {
   memset(SignalDesc,0,sizeof(SC2_Get_HW_IO_Signal_Descriptor_Response));
   writelog(ERROR_M,"GET_HW_IO_SIGNAL_DESCRIPTION failed 0x%x",err);
  }
  return err;
}

DWORD CPco_Camera::PCO_Get_Trigger_Options()
{
  DWORD err;
  WORD signal_count;
  SC2_Simple_Telegram com;
  SC2_Get_Num_HW_IO_Signals_Response resp;

  memset(trigger_options,0,sizeof(trigger_options));

  com.wCode=GET_NUMBER_HW_IO_SIGNALS;
  com.wSize=sizeof(SC2_Simple_Telegram);

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err!=PCO_NOERROR)
  {
   HWIO_available=0;
  }
  else
  {
   signal_count=resp.wNumOfSignals;
   HWIO_available=1;
   int in=0;
   for(int i=0;i<signal_count;i++)
   {
    SC2_Get_HW_IO_Signal_Descriptor_Response SignalDesc;
    PCO_GetHWIOSignalDescriptor(i,&SignalDesc);

    writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"HWIO Line %d has following options:",i);
    for(int j=0;j<NUM_SIGNALS;j++)
    {
     char txt[40];
     if(SignalDesc.wSignalDefinitions&0x02)
      sprintf_s(txt,sizeof(txt),"output ");
     else
      sprintf_s(txt,sizeof(txt),"input ");
     if(SignalDesc.wSignalDefinitions&0x01)
     {
      strcat_s(txt,sizeof(txt),"enable/disable allowed");
      if(strstr(SignalDesc.szSignalName[j],"Trigger")!=0)
      {
       trigger_options[in].enabled=0;
       trigger_options[in].selection=j;
       trigger_options[in].signal_num=i;
       strcpy_s(trigger_options[in].name,sizeof(trigger_options[in].name),SignalDesc.szSignalName[j]);
       in++;
      }
     }
     if(strlen(SignalDesc.szSignalName[j]))
     {
      writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"%s as %s",SignalDesc.szSignalName[j],txt);
     }
    }
   }
  }
  return err;
}

//enable all Triggerlines
DWORD CPco_Camera::PCO_Enable_Trigger()
{
  DWORD err;
  if(HWIO_available==0)
   return PCO_ERROR_DRIVER_CAMERALINK;

  int in=0;
  for(in=0;in<10;in++)
  {
   if(strlen(trigger_options[in].name))
   {
    WORD Enabled,Type,Polarity,FilterSetting,Selected;

    err=PCO_GetHWIOSignal(trigger_options[in].signal_num,&Enabled,&Type,&Polarity,&FilterSetting,&Selected);
    if(err==PCO_NOERROR)
    {
     err=PCO_SetHWIOSignal(trigger_options[in].signal_num,1,Type,Polarity,FilterSetting,0);
     if(err==PCO_NOERROR)
     {
      writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"Trigger Line %d: %s enabled",trigger_options[in].signal_num,trigger_options[in].name);
      trigger_options[in].enabled=1;
     }
    }
   }
   else
    break;
  }


  {
   FGSTATUS fg_status;
   fg_status = FGSTATUS_OK;

   char buf[200];
   uint32_t bufsize;

   if(fg_status == FGSTATUS_OK)
   {
    bufsize=sizeof(buf);
    memset(buf,0,bufsize);
    fg_status=KYFG_GetCameraValueStringCopy(camHandle,"TriggerMode",buf,&bufsize);
    writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"Trigger Mode is %s size %d",buf,bufsize);

    if(!strstr(buf,"OFF"))
    {
//     fg_status=KYFG_SetCameraValueString(camHandle,"TriggerMode","ON");
     fg_status=KYFG_SetCameraValueInt(camHandle,"TriggerMode",1);
     bufsize=sizeof(buf);
     memset(buf,0,bufsize);
     fg_status=KYFG_GetCameraValueStringCopy(camHandle,"TriggerMode",buf,&bufsize);
     writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"Trigger Mode is %s size %d",buf,bufsize);
    }
   }

   if(fg_status == FGSTATUS_OK)
   {
    bufsize=sizeof(buf);
    memset(buf,0,bufsize);
    fg_status=KYFG_GetCameraValueStringCopy(camHandle,"TriggerSource",buf,&bufsize);
    writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"Trigger Source is %s size %d",buf,bufsize);

    if(!strstr(buf,"PulseMessage"))
    {
     fg_status=KYFG_SetCameraValueInt(camHandle,"TriggerSource",2);
     bufsize=sizeof(buf);
     memset(buf,0,bufsize);
     fg_status=KYFG_GetCameraValueStringCopy(camHandle,"TriggerSource",buf,&bufsize);
     writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"Trigger Source is %s size %d",buf,bufsize);
    }
   }

   if(fg_status == FGSTATUS_OK)
   {
    fg_status=KYFG_CameraExecuteCommand(camHandle,"AcquisitionArm");
    writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"AcquisitionArm done");
   }

   if(fg_status == FGSTATUS_OK)
   {
    int val;
    val=KYFG_GetCameraValueInt(camHandle,"ExposureTime");
    writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"ExposureTime is %d",val);
   }


  }

  return err;
}

//disable all Triggerlines
DWORD CPco_Camera::PCO_Disable_Trigger()
{
  DWORD err;
  if(HWIO_available==0)
   return PCO_ERROR_DRIVER_CAMERALINK;

  int in=0;
  for(in=0;in<10;in++)
  {
   if(strlen(trigger_options[in].name))
   {
    WORD Enabled,Type,Polarity,FilterSetting,Selected;

    err=PCO_GetHWIOSignal(trigger_options[in].signal_num,&Enabled,&Type,&Polarity,&FilterSetting,&Selected);
    if(err==PCO_NOERROR)
    {
     err=PCO_SetHWIOSignal(trigger_options[in].signal_num,0,Type,Polarity,FilterSetting,0);
     if(err==PCO_NOERROR)
     {
      writelog(COMMAND_M|STDOUT_M|DBGOUT_M,"Trigger Line %d: %s disabled",trigger_options[in].signal_num,trigger_options[in].name);
      trigger_options[in].enabled=0;
     }
    }
   }
   else
    break;
  }
  return err;
}


DWORD CPco_Camera::PCO_GetAcquiredImageNumber(DWORD *num)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Camera_Sensor_Signal_Status_Response resp;

  com.wCode=GET_SENSOR_SIGNAL_STATUS;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
   *num=resp.dwImageCount;
  return err;
}


int CPco_Camera::image_nr_from_timestamp(void *buf,int shift)
{
  unsigned short *b;
  unsigned short c;
  int y;
  int image_nr=0;
  b=(unsigned short *)(buf);
  y=100*100*100;
  for(;y>0;y/=100)
  {
   c=*b>>shift;
   image_nr+= (((c&0x00F0)>>4)*10 + (c&0x000F))*y;
   b++;
  }
  return image_nr;
}

int CPco_Camera::image_nr_from_timestamp_c(void *bufin,int shift)
{
  unsigned short *b;
  unsigned short c;
  int y;
  int image_nr=0;
  unsigned short buf[128];

  Extract_Line_10_16_LSBaligned(32,buf,bufin);

  b=buf;
  y=100*100*100;
  for(;y>0;y/=100)
  {
   c=*b>>shift;
   image_nr+= (((c&0x00F0)>>4)*10 + (c&0x000F))*y;
   b++;
  }
  return image_nr;
}


int CPco_Camera::time_from_timestamp(void *buf,int shift,SYSTEMTIME *st)
{
  unsigned short *b;
  unsigned short c;
  int x,us;

  memset(st,0,sizeof(SYSTEMTIME));
  b=(unsigned short *)buf;
//counter
  for(x=0;x<4;x++) 
  {
   c=*(b+x)>>shift;
  }

  x=4;
//year
  c=*(b+x)>>shift;
  st->wYear+=(c>>4)*1000;
  st->wYear+=(c&0x0F)*100;
  x++;
  c=*(b+x)>>shift;
  st->wYear+=(c>>4)*10;
  st->wYear+=(c&0x0F);
  x++;

//month
  c=*(b+x)>>shift;
  st->wMonth+=(c>>4)*10;
  st->wMonth+=(c&0x0F);
  x++;


//day
  c=*(b+x)>>shift;
  st->wDay+=(c>>4)*10;
  st->wDay+=(c&0x0F);
  x++;

//hour
  c=*(b+x)>>shift;
  st->wHour+=(c>>4)*10;
  st->wHour+=(c&0x0F);
  x++;

//min   
  c=*(b+x)>>shift;
  st->wMinute+=(c>>4)*10;
  st->wMinute+=(c&0x0F);
  x++;

//sec   
  c=*(b+x)>>shift;
  st->wSecond+=(c>>4)*10;
  st->wSecond+=(c&0x0F);
  x++;

//us   
  us=0;
  c=*(b+x)>>shift;
  us+=(c>>4)*100000;
  us+=(c&0x0F)*10000;
  x++;
  c=*(b+x)>>shift;
  us+=(c>>4)*1000;
  us+=(c&0x0F)*100;
  x++;
  c=*(b+x)>>shift;
  us+=(c>>4)*10;
  us+=(c&0x0F);
  x++;
  st->wMilliseconds=us/1000;
  return 0;
}




void CPco_Camera::get_sizes(WORD* width,WORD* height,int* bitpix)
{ 
  DWORD err;
  uint64_t val;

  if(Camera_Mutex)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+100);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s enter Command_Mutex failed",porttxt);
    return;// PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
   writelog(PROCESS_M,__FUNCTION__": %s enter Command_Mutex done",porttxt);
  }

  *width=act_width;
  *height=act_height;

  WORD RoiX0,RoiY0,RoiX1,RoiY1;
  PCO_GetROI(&RoiX0,&RoiY0,&RoiX1,&RoiY1);

  *width=RoiX1-RoiX0+1;
  *height=RoiY1-RoiY0+1;
  *bitpix=act_bitpix;
/*
  val=KYFG_GetCameraValueInt(camHandle,"Width");
  *width=(WORD)val; 

  val=KYFG_GetCameraValueInt(camHandle,"Height");
  *height=(WORD)val; 

  val=KYFG_GetCameraValueInt(camHandle,"PayloadSize");
  *bitpix=(WORD)((val*8)/(*width * *height));
  writelog(INIT_M,"PayloadSize %d",val);
*/


  if(Camera_Mutex)
  {
   ReleaseMutex(Camera_Mutex);
   writelog(PROCESS_M,__FUNCTION__": %s leave Command_Mutex done",porttxt);
  }
}


void CPco_Camera::get_camera_bootpar()
{
  unsigned char buf[100];
  DWORD err,data;
  CLHS_GENCP_REG reg;

  if(Camera_Mutex)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+100);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s enter Command_Mutex failed",porttxt);
    return;// PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
   writelog(PROCESS_M,__FUNCTION__": %s enter Command_Mutex done",porttxt);
  }


  ABRM_VENDOR(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Vendor: %s",buf);

  ABRM_MODEL(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Model: %s",buf);

  ABRM_FAMILY(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Family: %s",buf);

  ABRM_DEVICE_VERSION(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Device_Version: %s",buf);

  ABRM_MANUFACTOR(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Manufactor: %s",buf);

  ABRM_SERIAL(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Serial: %s",buf);

  ABRM_USER_NAME(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"Username: %s",buf);

  ABRM_MAX_RESPONSE(reg)
  memset(buf,0,sizeof(buf));
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"Max Response Time:  %d 0x%04x",data,data);

  SBRM_PIXEL_TYPE(reg)
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"PixelType:  %d 0x%04x",data,data);

  SBRM_BIT_DEPTH(reg)
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"BitDepth:   %d 0x%04x",data,data);
  act_bitpix=data;

  SBRM_SENSOR_WIDTH(reg)
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"Cam_width:  %d 0x%04x",data,data);

  SBRM_SENSOR_HEIGHT(reg)
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"Cam_height:  %d 0x%04x",data,data);


  SBRM_ACTUAL_DEVICE_CONFIG(reg)
  KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"Actual Device Configuration:  %d 0x%04x",data,data);


  if(Camera_Mutex)
  {
   ReleaseMutex(Camera_Mutex);
   writelog(PROCESS_M,__FUNCTION__": %s leave Command_Mutex done",porttxt);
  }
}


//@@@testcode wrong register
void CPco_Camera::test_gencp()
{
  CLHS_GENCP_REG reg;
  uint32_t data;
  DWORD erri;
  unsigned char buf[10];

  if(Camera_Mutex)
  {
   DWORD waitstat;
   waitstat=WaitForSingleObject(Camera_Mutex,com_timeout+100);
   if(waitstat!=WAIT_OBJECT_0)
   {
    writelog(ERROR_M,__FUNCTION__": %s enter Command_Mutex failed",porttxt);
    return;// PCO_ERROR_DRIVER_CAMERA_BUSY;
   }
   writelog(PROCESS_M,__FUNCTION__": %s enter Command_Mutex done",porttxt);
  }

  SBRM_SENSOR_WIDTH(reg)
  erri=KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"SBRM_SENSOR_WIDTH:  %d 0x%04x  erri 0x%x %d",data,data,erri,erri);

  data=0;
  reg.adr=0x00030038;
  reg.size=4;
  erri=KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"read reg_adr 0x00030038:  %d 0x%04x  erri 0x%x %d",data,data,erri,erri);

  SBRM_SENSOR_WIDTH(reg)
  erri=KYFG_CameraReadReg(camHandle,reg.adr,buf,&reg.size);
  data=*((DWORD*)buf);
  data=be32toh(data);
  writelog(INIT_M,"SBRM_SENSOR_WIDTH:  %d 0x%04x  erri 0x%x %d",data,data,erri,erri);

  memset(buf,0,sizeof(buf));
  reg.adr=0x00030038;
  reg.size=4;
  erri=KYFG_CameraWriteReg(camHandle,reg.adr,buf,&reg.size);
  writelog(INIT_M,"write reg_adr 0x00030038:  %d 0x%04x  erri 0x%x %d",data,data,erri,erri);

  if(Camera_Mutex)
  {
   ReleaseMutex(Camera_Mutex);
   writelog(PROCESS_M,__FUNCTION__": %s leave Command_Mutex done",porttxt);
  }

  WORD camtype;
  DWORD serialnumber;
  PCO_GetCameraType(&camtype,&serialnumber);

}






void CPco_Camera::writelog(DWORD lev,const char *str,...)
{
 if(clog)
 {
  va_list arg;
  va_start(arg,str);
//  lev|=(DBGOUT_M);
  clog->writelog(lev,str,arg);
  va_end(arg);
 }
}


CPco_Buffer::CPco_Buffer()
{
  act_width=act_height=act_bitpix=act_size=0;
  act_adr=NULL;
  ima_event=NULL;
  act_status=0;
}

CPco_Buffer::~CPco_Buffer()
{
  free_buffer();
}


DWORD CPco_Buffer::allocate_buffer(int width,int height,int bitpix)
{
  if(ima_event==NULL)
  {
   ima_event=CreateEvent(NULL,TRUE,FALSE,NULL);
   if(ima_event==NULL)
    return PCO_ERROR_APPLICATION|PCO_ERROR_NOMEMORY;
  }

  if((act_width==width)&&(act_height==height)&&(act_bitpix==bitpix))
   return PCO_NOERROR;

  act_size=width*height*((bitpix+7)/8);

  if(act_adr)
   free(act_adr);

  act_adr=malloc(act_size);
  if(act_adr)
  {
   act_width=width;
   act_height=height;
   act_bitpix=bitpix;
   return PCO_NOERROR;
  }

  act_size=0;
  return PCO_ERROR_APPLICATION|PCO_ERROR_NOMEMORY;
}

DWORD CPco_Buffer::free_buffer()
{
  if(ima_event)
  {
   CloseHandle(ima_event);
   ima_event=NULL;
  }

  if(act_adr)
   free(act_adr);
  act_width=act_height=act_bitpix=act_size=0;

  return PCO_NOERROR;
}

DWORD CPco_Buffer::copy_16(void *bufin)
{
  unsigned short* adr_in;
  unsigned short* adr_out;

  adr_in=(unsigned short*)bufin;
  adr_out=(unsigned short*)act_adr;

  for(int y=0;y<act_height;y++)
  {
   memcpy(adr_out,adr_in,act_width*2);
   adr_out+=act_width;
   adr_in+=act_width;
  }
  return PCO_NOERROR;
}

DWORD CPco_Buffer::copy_10(void *bufin)
{
  unsigned short* adr_in;
  unsigned short* adr_out;
  int w=act_width*10/8;

  adr_in=(unsigned short*)bufin;
  adr_out=(unsigned short*)act_adr;

  for(int y=0;y<act_height/2;y++)
  {
   memcpy(adr_out,adr_in,w);
   adr_out+=w;
   adr_in+=w;
  }
  return PCO_NOERROR;
}


HANDLE CPco_Buffer::get_event()
{
  return ima_event;
}

DWORD CPco_Buffer::get_status()
{
  return act_status;
}

void CPco_Buffer::set_status(DWORD status)
{
  act_status=status;
}

DWORD CPco_Buffer::image_nr_from_timestamp(int shift)
{
  unsigned short *b;
  unsigned short c;
  int y;
  int image_nr=0;
  b=(unsigned short *)(act_adr);
  y=100*100*100;
  for(;y>0;y/=100)
  {
   c=*b>>shift;
   image_nr+= (((c&0x00F0)>>4)*10 + (c&0x000F))*y;
   b++;
  }
  return image_nr;
}


DWORD CPco_Buffer::image_nr_from_timestamp_c(int shift)
{
  unsigned short *b;
  unsigned short c;
  int y;
  int image_nr=0;
  unsigned short buf[128];

  Extract_Line_10_16_LSBaligned(32,buf,act_adr);

  b=buf;
  y=100*100*100;
  for(;y>0;y/=100)
  {
   c=*b>>shift;
   image_nr+= (((c&0x00F0)>>4)*10 + (c&0x000F))*y;
   b++;
  }
  return image_nr;
}

void Extract_Line_10_16_MSBaligned(int width,void *bufout,void* bufin)
{
  DWORD *lineadr_in;
  DWORD *lineadr_out;
  DWORD *lineadr_out_end;
  DWORD a;

  lineadr_in=(DWORD *)bufin; 
  lineadr_out=(DWORD *)bufout;
  lineadr_out_end=lineadr_out+width/2;

  for(;lineadr_out<lineadr_out_end;)
  {
    a  = (*lineadr_in&0x000003FF)<< 6;  // low  (0)
    a |= (*lineadr_in&0x000FFC00)<<12;  // high (1)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0x3FF00000)>>14;  // low  (2)
    a |= (*lineadr_in&0xC0000000)>> 8;  // high (3)
    lineadr_in++;
    a |= (*lineadr_in&0x000000FF)<<24;  // high (3)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0x0003FF00)>> 2;  // low  (4)
    a |= (*lineadr_in&0x0FFC0000)<< 4;  // high (5)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0xF0000000)>>22;  // low  (6)
    lineadr_in++;
    a |= (*lineadr_in&0x0000003F)<<10;  // low  (6)
    a |= (*lineadr_in&0x0000FFC0)<<16;  // high (7)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0x03FF0000)>>10;  // low  (8)
    a |= (*lineadr_in&0xFC000000)>> 4;  // high (9)
    lineadr_in++;
    a |= (*lineadr_in&0x0000000F)<<28;  // high (9)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0x00003FF0)<< 2;  // low  (10)
    a |= (*lineadr_in&0x00FFC000)<< 8;  // high (11)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0xFF000000)>>18;  // low  (12)
    lineadr_in++;
    a |= (*lineadr_in&0x00000003)<<14;  // low  (12)
    a |= (*lineadr_in&0x00000FFC)<<20;  // high (13)
    *lineadr_out=a;
    lineadr_out++;

    a  = (*lineadr_in&0x003FF000)>> 6;  // low  (14)
    a |= (*lineadr_in&0xFFC00000);      // high (15)
    *lineadr_out=a;
    lineadr_out++;
    lineadr_in++;
  }
}


void Extract_Line_10_16_LSBaligned(int width,void *bufout,void* bufin)
{
  DWORD *lineadr_in;
  DWORD *lineadr_out;
  DWORD *lineadr_out_end;
  DWORD a;

  lineadr_in=(DWORD *)bufin;
  lineadr_out=(DWORD *)bufout;
  lineadr_out_end=lineadr_out+width/2;

  for(;lineadr_out<lineadr_out_end;)
  {
    a  = (*lineadr_in&0x000003FF);      // low  (0)
    a |= (*lineadr_in&0x000FFC00)<< 6;  // high (1)
    *lineadr_out++=a;

    a  = (*lineadr_in&0x3FF00000)>>20;  // low  (2)
    a |= (*lineadr_in&0xC0000000)>>14;  // high (3)
    lineadr_in++;
    a |= (*lineadr_in&0x000000FF)<<18;  // high (3)
    *lineadr_out++=a;

    a  = (*lineadr_in&0x0003FF00)>> 8;  // low  (4)
    a |= (*lineadr_in&0x0FFC0000)>> 2;  // high (5)
    *lineadr_out++=a;

    a  = (*lineadr_in&0xF0000000)>>28;  // low  (6)
    lineadr_in++;
    a |= (*lineadr_in&0x0000003F)<< 4;  // low  (6)
    a |= (*lineadr_in&0x0000FFC0)<<10;  // high (7)
    *lineadr_out++=a;

    a  = (*lineadr_in&0x03FF0000)>>16;  // low  (8)
    a |= (*lineadr_in&0xFC000000)>>10;  // high (9)
    lineadr_in++;
    a |= (*lineadr_in&0x0000000F)<<22;  // high (9)
    *lineadr_out++=a;

    a  = (*lineadr_in&0x00003FF0)>> 4;  // low  (10)
    a |= (*lineadr_in&0x00FFC000)<< 2;  // high (11)
    *lineadr_out++=a;

    a  = (*lineadr_in&0xFF000000)>>24;  // low  (12)
    lineadr_in++;
    a |= (*lineadr_in&0x00000003)<< 8;  // low  (12)
    a |= (*lineadr_in&0x00000FFC)<<14;  // high (13)
    *lineadr_out++=a;

    a  = (*lineadr_in&0x003FF000)>>12;  // low  (14)
    a |= (*lineadr_in&0xFFC00000)>> 6;  // high (15)
    *lineadr_out++=a;
    lineadr_in++;
  }
}

