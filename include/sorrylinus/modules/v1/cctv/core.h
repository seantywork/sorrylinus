#ifndef _SORRYLINUS_MOD_CCTV_H_
#define _SORRYLINUS_MOD_CCTV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sorrylinus/modules/v1/core.h"





typedef struct CCTV_STREAM_ARG{

    int result;
    char* stream_key;

} CCTV_STREAM_ARG;


void* start_cctv_stream(void* varg);

void cctv_stream(char* result, char* stream_key);




#ifdef __cplusplus
}
#endif // __cplusplus


#endif