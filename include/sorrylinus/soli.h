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

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <openssl/x509v3.h>
#include <openssl/opensslconf.h>


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

#define HOST_FULL_ADDRESS "https://feebdaed.xyz:3001"

#define CREDENTIAL_LOCATION "tls/sub_client.crt"

#define MAX_BUFF 4096

int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path);



#endif