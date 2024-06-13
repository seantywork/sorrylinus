#ifndef _FRANK_HUB_UTILS_H_
#define _FRANK_HUB_UTILS_H_



#include "frankhub/v2/core.h"


#define MAX_TIMESTR_LEN 80
#define MAX_LOG_TXT_LEN 1024 
#define MAX_FMT_ARG_LEN 128


int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path);

int gen_random_bytestream(uint8_t* bytes, size_t num_bytes);

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray);







void get_current_time_string(char* tstr);

void sleepms(long ms);

void fmt_logln(FILE *fp, char* fmt_out, ...);


#endif
