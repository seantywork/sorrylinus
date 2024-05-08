#ifndef _FRANK_H_
#define _FRANK_H_



#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdint.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h>
#include <endian.h>
#include <pthread.h>

#define DEBUG_THIS 0
#define DEBUG_LOCAL 1


#define MAX_ID_LEN 1024
#define MAX_PW_LEN 4096

struct memory {
  char *response;
  size_t size;
};

extern int s_sig_num;

int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path);

int gen_random_bytestream(uint8_t* bytes, size_t num_bytes);

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray);




#endif


