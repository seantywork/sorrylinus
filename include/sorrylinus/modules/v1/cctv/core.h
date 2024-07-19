#ifndef _SORRYLINUS_MOD_CCTV_H_
#define _SORRYLINUS_MOD_CCTV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sorrylinus/modules/v1/core.h"


#include <gst/gst.h>

#define MUXER_BATCH_TIMEOUT_USEC 400000


#define ENC_SPEED_PRESET_ULTRAFAST 1
#define ENC_TUNE_ZEROLATENCY  0x00000004


#define CCTV_FAILED -1
#define CCTV_READY 0
#define CCTV_STREAMING 1

#define CCTV_THREAD_MSG_LEN 1024

typedef struct CCTV_STREAM_ARG{

    char* stream_key;

} CCTV_STREAM_ARG;


extern pthread_t CCTV_ID;

extern int CCTV_STATUS;

extern char CCTV_MESSAGE[CCTV_THREAD_MSG_LEN];


void cctv_stream_toggle(char* result, char* stream_key);


void* start_cctv_stream(void* varg);

gboolean bus_call (GstBus * bus, GstMessage * msg, gpointer data);


/*

gst-launch-1.0 
  -e v4l2src device=/dev/video0 
  ! queue 
  ! videoconvert 
  ! videoscale 
  ! video/x-raw,width=960,height=720 
  ! x264enc speed-preset=ultrafast tune=zerolatency key-int-max=20 
  ! flvmux streamable=true 
  ! queue 
  ! rtmpsink location='rtmps://localhost:8084/publish/foobar

*/



#ifdef __cplusplus
}
#endif // __cplusplus


#endif