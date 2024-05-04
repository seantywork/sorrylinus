#ifndef _FRANK_HUB_SOCK_H_
#define _FRANK_HUB_SOCK_H_



#include "frankhub/core.h"



void sock_listen_and_serve();


void sock_handle_conn();


void sock_handle_client(int cfd);




#endif