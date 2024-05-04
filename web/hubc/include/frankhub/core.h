#ifndef _FRANK_HUB_H_
#define _FRANK_HUB_H_



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
// ep headers
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
// ssl headers
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#define TRUE 1
#define FALSE 0
#define MAX_BUFF 10240
#define MAX_CONN 80
#define MAX_ID_LEN 1024
#define MAX_PW_LEN 4096
#define PORT_FRONT 3000
#define PORT_SOCK 3001 

#define ISSOCK 1
#define ISFRONT 2


#ifndef SERVER_KEY
# define SERVER_KEY "tls/sub_priv.pem"
#endif

#ifndef SERVER_CERT
# define SERVER_CERT "tls/sub.crt"
#endif

//#define WAIT 7   

struct CHANNEL_CONTEXT {
    int allocated;
    int sockfd;
    int frontfd;
    char id[MAX_ID_LEN];
    char pw[MAX_PW_LEN];
    SSL *ssl;
    SSL_CTX *ctx;
};

struct SOCK_CONTEXT {
    int allocated;
    int sockfd;
    SSL *ssl;
    SSL_CTX *ctx;
};

struct FRONT_CONTEXT {
    int allocated;
    int frontfd;

};


extern struct CHANNEL_CONTEXT CHAN_CTX[MAX_CONN];

extern struct SOCK_CONTEXT SOCK_CTX[MAX_CONN];

extern struct FRONT_CONTEXT FRONT_CTX[MAX_CONN];



extern int SOCK_FD;
extern int SOCK_SERVLEN;
extern int SOCK_EPLFD;
extern struct epoll_event SOCK_EVENT;
extern struct epoll_event *SOCK_EVENTARRAY;


extern int FRONT_FD;
extern int FRONT_EPLFD;
extern struct epoll_event FRONT_EVENT;
extern struct epoll_event *FRONT_EVENTARRAY;

extern int MAX_SD;

extern int OPT;



int gen_random_bytestream(uint8_t* bytes, size_t num_bytes);

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray);

int make_socket_non_blocking (int sfd);

SSL_CTX *create_context();

void configure_context(SSL_CTX *ctx);

int update_chanctx_from_userinfo(char* id, char* pw);

int update_chanctx_from_sockctx(int fd, char* id);

int set_chanctx_from_frontctx(int fd, char* id);

int get_chanctx_by_id(char* id);

int get_chanctx_by_fd(int fd, int type);

int set_sockctx_by_fd(int fd);

int get_sockctx_by_fd(int fd);

int set_frontctx_by_fd(int fd);

int get_frontctx_by_fd(int fd);

int calloc_chanctx();

int free_chanctx(int idx);

int calloc_sockctx();

int free_sockctx(int idx, int memfree);

int calloc_frontctx();

int free_frontctx(int idx);



int chanctx_write(char* id, int write_len, uint8_t* wbuff);

int chanctx_read(char* id, int read_len, uint8_t* rbuff);

int sockctx_write(int fd, int write_len, uint8_t* wbuff);

int sockctx_read(int fd, int read_len, uint8_t* rbuff);




#endif