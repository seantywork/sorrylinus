#ifndef _SORRYLINUS_H_
#define _SORRYLINUS_H_


#include <string.h>
#include <sys/utsname.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include <errno.h>
#include <poll.h>
#include <time.h>
#include <endian.h>

#include <stdarg.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <openssl/x509v3.h>
#include <openssl/opensslconf.h>


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

#ifndef UNUSED
# define UNUSED(x) ((void)(x))
#endif

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef HOST_NAME
# define HOST_NAME "feebdaed.xyz"
#endif

#ifndef HOST_PORT
# define HOST_PORT "3001"
#endif

#ifndef VERIFICATION_LOCATION
# define VERIFICATION_LOCATION "tls/ca.crt"
#endif

#ifndef HOST_RESOURCE
# define HOST_RESOURCE "/cgi-bin/randbyte?nbytes=32&format=h"
#endif

#ifndef SIGTRAP
# define SIGTRAP 1337
#endif

# define ASSERT(x) { \
  if(!(x)) { \
    fprintf(stderr, "Assertion: %s: function %s, line %d\n", (char*)(__FILE__), (char*)(__func__), (int)__LINE__); \
    exit(SIGTRAP); \
  } \
}

#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})


#if __BIG_ENDIAN__
# define htonll(x) (x)
# define ntohll(x) (x)
#else
# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
# define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif


#define HOST_FULL_ADDRESS "https://feebdaed.xyz:3001"

#define CREDENTIAL_LOCATION "tls/sub_client.crt"

#define MAX_BUFF HUB_BODY_BYTEMAX

#define MAX_TIMESTR_LEN 80
#define MAX_LOG_TXT_LEN 1024 
#define MAX_FMT_ARG_LEN 128

#define SOLI_MAX_CONF_RAW_LEN  1024
#define SOLI_MAX_KEY_LEN 128
#define SOLI_MAX_VAL_LEN 512


#endif