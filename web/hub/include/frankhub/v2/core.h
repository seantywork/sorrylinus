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
#include <endian.h>
#include <pthread.h>
#include <stdarg.h>
// ep headers
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
// ssl headers
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "mongoose/mongoose.h"
#include "cJSON/cJSON.h"

#define DEBUG_THIS 0

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


#define MAX_DEVICE_NAME 40
#define MAX_EVENTS_NO 400
#define MAX_EVENT_TEXT_SIZE 10
#define EVENTS_PER_PAGE 20

#define MAX_USER_NAME 1024
#define MAX_USER_PASS 2048
#define MAX_USER_ACCESS_TOKEN 1024

#define MAX_COOKIE_LEN 1024
#define MAX_COOKIE_KEYLEN 32

#define MAX_USERS 10

#define MAX_PUBLIC_URI_LEN 512

#define MAX_REQUEST_URI_LEN 1024 * 10
#define MAX_CLIENT_ID_LEN 1024
#define MAX_CLIENT_SECRET_LEN 1024
#define MAX_POST_FIELDS_LEN 1024 * 10

#define MAX_CODELEN 256
#define GOAUTH_TOKENLEN 512

#define MAX_REST_BUFF 1024 * 10
#define MAX_WS_BUFF 1024 * 10

#define WS_MAX_COMMAND_LEN 32
#define WS_MAX_COMMAND_DATA_LEN WS_MAX_COMMAND_LEN * 8
#define WS_MAX_COMMAND_RECV_LEN WS_MAX_COMMAND_LEN * 8 * 8

#define WS_COMMAND_REQ_KEY "REQ_KEY"
#define WS_COMMAND_ROUNDTRIP "ROUNDTRIP"

#define DEFAULT_RANDLEN 64



#ifndef HTTP_URL
#define HTTP_URL "http://0.0.0.0:3000"
#endif


#ifndef HTTPS_URL
#define HTTPS_URL "https://0.0.0.0:3443"
#endif

#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

#if __BIG_ENDIAN__
# define htonll(x) (x)
# define ntohll(x) (x)
#else
# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
# define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

#ifndef SERVER_KEY
# define SERVER_KEY "tls/sub_priv.pem"
#endif

#ifndef SERVER_CERT
# define SERVER_CERT "tls/sub.crt"
#endif

#ifndef HUB_CA_CERT
# define HUB_CA_CERT "tls/ca.crt"
#endif

#define DEFAULT_RANDLEN 64
//#define WAIT 7   


extern char CA_CERT[MAX_PW_LEN];

extern int s_sig_num;

struct settings {
  bool log_enabled;
  int log_level;
  long brightness;
  char *device_name;
};

extern struct settings s_settings;

extern uint64_t s_boot_timestamp; 

extern char *s_json_header;

extern struct mg_mgr mgr;



struct user {

    char name[MAX_USER_NAME]; 
    char pass[MAX_USER_PASS]; 
    char access_token[MAX_USER_ACCESS_TOKEN];

};



struct SOCK_CONTEXT {
    int allocated;
    int sockfd;
    SSL *ssl;
    SSL_CTX *ctx;
};


struct CHANNEL_CONTEXT {
    int allocated;
    char id[MAX_ID_LEN];
    int sockfd;
    SSL *ssl;
    SSL_CTX *ctx;
    int frontid;
    struct mg_connection *frontc;
};



struct HUB_PACKET {

    int ctx_type;
    char id[MAX_ID_LEN];
    int fd;
    uint8_t header[HUB_HEADER_BYTELEN];
    uint64_t body_len;
    uint8_t wbuff[MAX_BUFF];
    uint8_t* rbuff;

    int flag;

};


extern struct CHANNEL_CONTEXT CHAN_CTX[MAX_CONN];



extern struct SOCK_CONTEXT SOCK_CTX[MAX_CONN];


extern struct user user_array[MAX_USERS];



extern FILE* LOGFP;
extern pthread_mutex_t G_MTX;

extern int SOCK_FD;
extern int SOCK_SERVLEN;
extern int SOCK_EPLFD;
extern struct epoll_event SOCK_EVENT;
extern struct epoll_event *SOCK_EVENTARRAY;



extern int MAX_SD;

extern int OPT;


int load_config_json();


int make_socket_non_blocking (int sfd);

SSL_CTX *create_context();

void configure_context(SSL_CTX *ctx);


int sig_verify(const char* cert_pem, const char* intermediate_pem);

int extract_common_name(uint8_t* common_name, const char* cert);

int user_idpw_verify(char* idpw, char* ret_id);

int get_user_by_id(char* id);

int update_chanctx_from_sockctx(int fd, char* id);

int update_chanctx_from_front(struct mg_connection* c, char* id);

int get_chanctx_by_id(char* id);

int get_chanctx_by_fd(int fd);

int get_chanctx_by_fid(int fid);

int set_sockctx_by_fd(int fd);

int get_sockctx_by_fd(int fd);



int calloc_chanctx();

int free_chanctx(int idx);

int calloc_sockctx();

int free_sockctx(int idx, int memfree);



int chanctx_write_sock(char* id, int write_len, uint8_t* wbuff);

int chanctx_read_sock(char* id, int read_len, uint8_t* rbuff);

int sockctx_write(int fd, int write_len, uint8_t* wbuff);

int sockctx_read(int fd, int read_len, uint8_t* rbuff);

void hub_write_packet(struct HUB_PACKET* hp);

void hub_read_packet(struct HUB_PACKET* hp);


#endif