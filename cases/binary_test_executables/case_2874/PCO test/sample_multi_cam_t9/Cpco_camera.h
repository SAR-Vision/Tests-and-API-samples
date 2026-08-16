
#include <queue>

#include "KYFGLib.h"
#include "../pco_err.h"
#include "../clhs_register.h"
#include "../portable_endian.h"
#include "../sc2_command.h"
#include "../sc2_telegram.h"

#include "Cpco_log.h"

#define PCO_SC2_DEF_BLOCK_SIZE 512

#define FLAG_ACQUIRE_USER          0x0001
#define FLAG_GRABBER_CRISEC        0x0002
#define FLAG_GRABBER_MUTEX         0x0004
#define FLAG_LOGFILE               0x0008
#define FLAG_START_CAM_BEFORE_ACQ  0x0010
#define FLAG_START_CAM_AFTER_ACQ   0x0020
#define FLAG_STOP_CAM_BEFORE_ACQ   0x0100
#define FLAG_STOP_CAM_AFTER_ACQ    0x0200
#define FLAG_ACQUIRE_NOMUTEX       0x0400
#define FLAG_COMMAND_NOMUTEX       0x0800
#define FLAG_COMMAND_GRABBERSYNC   0x1000
#define FLAG_COMMAND_GRABBERSYNC_R 0x2000
#define FLAG_ADD_STOPTIME          0x4000


#define FLAG_STOP_PENDING           0x0001

#define NUM_MAX_SIGNALS     20         // Maximum number of signals available
#define NUM_SIGNALS 4
#define NUM_SIGNAL_NAMES 4

typedef struct
{
  int signal_num;
  int selection;
  char name[25];
  int enabled;
}trigger_opt;



class CPco_Buffer
{
 private:
  int act_width;
  int act_height;
  int act_bitpix;
  int act_size;
  DWORD act_status;
  void* act_adr;
  HANDLE ima_event;

 public:
  CPco_Buffer();
  ~CPco_Buffer();

  DWORD allocate_buffer(int width,int height,int bitpix);
  DWORD free_buffer();
  DWORD copy_16(void *bufin);
  DWORD copy_10(void *bufin);
  HANDLE get_event();
  DWORD get_status();
  void set_status(DWORD status);
  DWORD image_nr_from_timestamp(int shift);
  DWORD image_nr_from_timestamp_c(int shift);
};

class CPco_Camera
{
 private:
  int cam_num;
  CAMHANDLE camHandle;
  STREAM_HANDLE streamHandle;
  STREAM_BUFFER_HANDLE* streamBufferHandles;
  FGHANDLE fghandle;
  int port;
  int acqflag;
  WORD camtype;
  uint64_t packeddatamode;
  int HWIO_available;
  trigger_opt trigger_options[10];



  std::queue <STREAM_BUFFER_HANDLE> qbuf_queue;
  std::queue <CPco_Buffer*> inbuf_queue;
  CRITICAL_SECTION inbuf_queue_crisec;
  CRITICAL_SECTION qbuf_queue_crisec;

  HANDLE Camera_Mutex;
  HANDLE Grabber_StopEvent;

  int com_timeout;

  int nr_of_buffer,act_bufnum,act_bitpix;
  WORD act_width,act_height;
  int acquired_images;
  int stopflag;

  HANDLE picin_event;
  HANDLE stop_event;
  HANDLE th_acquire;
  int thread_run;

  CPco_Log *clog;
  char porttxt[20];
  SC2_Camera_Description_Response description;

  double max_starttime;
  double max_stoptime;

  __int64 lpFrequency;
  __int64 lpPCount_ima1,lpPCount_ima2,lpPCount_rec_on;
  __int64 lpPCount_p0,lpPCount_p1,lpPCount_p2;

 public:
  CPco_Camera();
  ~CPco_Camera();

  void SetGrabber(FGHANDLE handle_in){fghandle=handle_in;};
  void SetCommandTime(int time){com_timeout=time;};
  void SetGrabberStopEvent(HANDLE event_in){Grabber_StopEvent=event_in;};

  void SetLog(CPco_Log *elog){clog=elog;};
  void writelog(DWORD lev,const char *str,...);

  DWORD get_acquired_images(){return acquired_images;};
  DWORD get_camnum(){return cam_num;};
  uint64_t get_packeddatamode(){return packeddatamode;};
  void set_acqflag(int flag){acqflag=flag;};

  DWORD open(CAMHANDLE cam_handle,int num);
  DWORD close();
  DWORD start_acquisition();
  DWORD stop_acquisition();
  DWORD get_RXFrameCounter();
  int get_port(){return port;}

  DWORD set_acquire_size(int width,int height,int bitpix);
  DWORD setup_buffer(CPco_Buffer* inbuf);
  DWORD cancel_buffers();

  DWORD start_acquisition_thread();
  DWORD stop_acquisition_thread();
  DWORD image_acquire_thread();
  void queuedbuffer_callback(STREAM_BUFFER_HANDLE streamBufferHandle);

  DWORD build_checksum(unsigned char *buf,int *size);
  DWORD test_checksum(unsigned char *buf,int *size);

  DWORD Control_Command(void *buf_in,DWORD size_in,void *buf_out,DWORD size_out);

  DWORD PCO_GetCameraType(WORD *camtype,DWORD *serialnumber);
  DWORD PCO_ArmCamera();
  DWORD PCO_SetRecordingState(WORD val);
  DWORD PCO_GetRecordingState(WORD *val);
  DWORD PCO_SetTimestampMode(WORD val);
  DWORD PCO_GetTemperature(SHORT *sCCDTemp,SHORT *sCAMTemp,SHORT *sExtTemp);
  DWORD PCO_GetDescription();
  DWORD PCO_GetROI(WORD *RoiX0,WORD *RoiY0,WORD *RoiX1,WORD *RoiY1);
  DWORD PCO_SetROI(WORD RoiX0,WORD RoiY0,WORD RoiX1,WORD RoiY1);
  DWORD PCO_GetTriggerMode(WORD *mode);
  DWORD PCO_SetTriggerMode(WORD mode);
  DWORD PCO_GetTimebase(WORD *delay,WORD *expos);
  DWORD PCO_SetTimebase(WORD delay,WORD expos);
  DWORD PCO_GetDelayExposure(DWORD *delay,DWORD *expos);
  DWORD PCO_SetDelayExposure(DWORD delay,DWORD expos);
  DWORD PCO_ResetSettingsToDefault();
  DWORD PCO_GetAcquiredImageNumber(DWORD *num);
  DWORD PCO_SetBitAlignment(WORD align);
  DWORD PCO_GetBitAlignment(WORD *align);

  DWORD PCO_Disable_Trigger();
  DWORD PCO_Enable_Trigger();
  DWORD PCO_Get_Trigger_Options();
  DWORD PCO_GetHWIOSignalDescriptor(WORD SignalNum,SC2_Get_HW_IO_Signal_Descriptor_Response *SignalDesc);
  DWORD PCO_GetHWIOSignal(WORD SignalNum,WORD *Enabled,WORD *Type,WORD *Polarity,WORD *FilterSetting,WORD *Selected);
  DWORD PCO_SetHWIOSignal(WORD SignalNum,WORD Enabled,WORD Type,WORD Polarity,WORD FilterSetting,WORD Selected);



  int image_nr_from_timestamp(void *buf,int shift);
  int image_nr_from_timestamp_c(void *bufin,int shift);

  int time_from_timestamp(void *buf,int shift,SYSTEMTIME *st);

  void get_sizes(WORD* width,WORD* height,int* bitpix);
  void get_camera_bootpar();
  void test_gencp();
};


