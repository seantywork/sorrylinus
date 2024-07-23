#ifndef _SORRYLINUS_UTILS_H_
#define _SORRYLINUS_UTILS_H_


#ifdef __cplusplus
extern "C" {
#endif




#include "sorrylinus/soli.h"




int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path);

void get_current_time_string(char* tstr);

void sleepms(long ms);

void fmt_logln(FILE *fp, char* fmt_out, ...);

void stringify_array_int(char* strarray, int arr_len , int* arr);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif