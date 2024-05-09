#ifndef _FRANK_CONTROLLER_CLIENT_H_
#define _FRANK_CONTROLLER_CLIENT_H_




#include "frank/controller/index.h"


int client_connect(int uidx);

int client_get_socket(char* url);




int client_write(int cfd, int write_len, uint8_t* write_bytes);

int client_read(int cfd, int read_len, uint8_t* read_bytes);


int client_write_packet(int uidx, uint8_t* header, uint64_t body_len, uint8_t* wbuff);

uint8_t* client_read_packet(int uidx, uint8_t* header, uint64_t* body_len, int* flag);


int client_exchange(int udix, int* recv_len, uint8_t* recv_buff, int send_len, uint8_t* send_buff);











#endif
