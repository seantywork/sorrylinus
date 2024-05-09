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
#include <sys/time.h>
#include <endian.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

#define ALLOWED_USER "seantywork@gmail.com"

#define DEBUG_THIS 0
#define DEBUG_LOCAL 0


#define TIMEOUT_SEC 5
#define TIMEOUT_MS TIMEOUT_SEC * 1000

#define HUB_WORD           8
#define HUB_HEADER_BYTELEN HUB_WORD * 3
#define HUB_BODY_BYTELEN   HUB_WORD * 1
#define HUB_BODY_BYTEMAX   HUB_WORD * 1280 //10KB
#define HUB_TIMEOUT_MS 5000
#define HUB_HEADER_AUTHSOCK "AUTHSOCK"
#define HUB_HEADER_AUTHFRONT "AUTHFRONT"
#define HUB_HEADER_AUTHFRANK "AUTHFRANK"
#define HUB_HEADER_SENDSOCK "SENDSOCK"
#define HUB_HEADER_RECVSOCK "RECVSOCK"
#define HUB_HEADER_SENDFRONT "SENDFRONT"
#define HUB_HEADER_RECVFRONT "RECVFRONT"
#define HUB_HEADER_SENDFRANK "SENDFRANK"
#define HUB_HEADER_RECVFRANK "RECVFRANK"

#define TRUE 1
#define FALSE 0
#define MAX_BUFF HUB_BODY_BYTEMAX
#define MAX_CONN 80
#define MAX_ID_LEN 1024
#define MAX_PW_LEN 4096
#define PORT_FRONT 3000
#define PORT_SOCK 3001 


#define ISSOCK 1
#define ISFRONT 2
#define CHAN_ISSOCK 3
#define CHAN_ISFRONT 4

#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

#if __BIG_ENDIAN__
# define htonll(x) (x)
# define ntohll(x) (x)
#else
# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
# define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif


#if DEBUG_LOCAL
#define HOST_FULL_ADDRESS "tcp://feebdaed.xyz:2999"
#define HOST_FULL_ADDRESS_CLIENT "tcp://feebdaed.xyz:3000"
#else
#define HOST_FULL_ADDRESS "tcp://frankhub:2999"
#define HOST_FULL_ADDRESS_CLIENT "tcp://feebdaed.xyz:3000"
#endif


extern int s_sig_num;


#endif


