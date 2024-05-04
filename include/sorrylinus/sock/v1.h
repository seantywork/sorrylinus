#ifndef _SORRYLINUS_SOCK_H_
#define _SORRYLINUS_SOCK_H_


#include "sorrylinus/soli.h"





extern SSL_CTX* ctx;
extern BIO *web; 

extern SSL *ssl;

extern int SOCK_FD;


int hubc_connect();

int get_socket(char* url);

int verify_callback(int preverify, X509_STORE_CTX* x509_ctx);

void init_openssl_library(void);
void print_cn_name(const char* label, X509_NAME* const name);
void print_san_name(const char* label, X509* const cert);
void print_error_string(unsigned long err, const char* const label);




int hubc_write(int write_len, uint8_t* write_bytes);

int hubc_read(int read_len, uint8_t* read_bytes);













#endif