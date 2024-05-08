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
// ep headers
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
// ssl headers
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

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

extern struct FRONT_CONTEXT FRONT_CTX[MAX_CONN];



extern int SOCK_FD;
extern int SOCK_SERVLEN;
extern int SOCK_EPLFD;
extern struct epoll_event SOCK_EVENT;
extern struct epoll_event *SOCK_EVENTARRAY;


extern int FRONT_FD;
extern int FRONT_SERVLEN;
extern int FRONT_EPLFD;
extern struct epoll_event FRONT_EVENT;
extern struct epoll_event *FRONT_EVENTARRAY;

extern int MAX_SD;

extern int OPT;

int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path);

int gen_random_bytestream(uint8_t* bytes, size_t num_bytes);

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray);

int make_socket_non_blocking (int sfd);

SSL_CTX *create_context();

void configure_context(SSL_CTX *ctx);


int sig_verify(const char* cert_pem, const char* intermediate_pem);

int extract_common_name(uint8_t* common_name, const char* cert);

int idpw_verify(char* idpw);

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



int chanctx_write(int type, char* id, int write_len, uint8_t* wbuff);

int chanctx_read(int type, char* id, int read_len, uint8_t* rbuff);

int sockctx_write(int fd, int write_len, uint8_t* wbuff);

int sockctx_read(int fd, int read_len, uint8_t* rbuff);

int frontctx_write(int fd, int write_len, uint8_t* wbuff);

int frontctx_read(int fd, int read_len, uint8_t* rbuff);


void ctx_write_packet(struct HUB_PACKET* hp);


void ctx_read_packet(struct HUB_PACKET* hp);



#endif