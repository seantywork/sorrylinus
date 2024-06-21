#ifndef _FRANK_HUB_SOCK_H_
#define _FRANK_HUB_SOCK_H_



#include "frankhub/v2/core.h"



void sock_listen_and_serve(void* varg);


void sock_handle_conn();


void sock_handle_client(int cfd);

void sock_authenticate(int cfd);

void sock_communicate(int chan_idx);

#endif