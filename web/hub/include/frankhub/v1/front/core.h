#ifndef _FRANK_HUB_FRONT_H_
#define _FRANK_HUB_FRONT_H_

#include "frankhub/v1/core.h"


void front_listen_and_serve(void* varg);


void front_handle_conn();


void front_handle_client(int cfd);

void front_authenticate(int cfd);

void front_communicate(int chan_idx);






#endif