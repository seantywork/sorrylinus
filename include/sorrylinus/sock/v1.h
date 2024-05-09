#ifndef _SORRYLINUS_SOCK_H_
#define _SORRYLINUS_SOCK_H_


#include "sorrylinus/soli.h"





extern SSL_CTX* ctx;
extern BIO *web; 

extern SSL *ssl;

extern int SOCK_FD;

extern int WAITING_HEADER;


int hubc_connect();

int verify_callback(int preverify, X509_STORE_CTX* x509_ctx);

void init_openssl_library(void);
void print_cn_name(const char* label, X509_NAME* const name);
void print_san_name(const char* label, X509* const cert);
void print_error_string(unsigned long err, const char* const label);

int get_socket(char* url);



int hubc_communicate();

uint8_t* hubc_handle(uint64_t command_len, uint8_t* command, int* flag);

int hubc_write(int write_len, uint8_t* write_bytes);

int hubc_read(int read_len, uint8_t* read_bytes);


int hubc_write_packet(uint8_t* header, uint64_t body_len, uint8_t* wbuff);

uint8_t* hubc_read_packet(uint8_t* header, uint64_t* body_len, int* flag);










#endif