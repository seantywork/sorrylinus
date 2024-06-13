#ifndef _FRANK_HUB_FRONT_H_
#define _FRANK_HUB_FRONT_H_

#include "frankhub/v2/core.h"



void sntp_fn(struct mg_connection *c, int ev, void *ev_data);

void timer_sntp_fn(void *param);

size_t print_int_arr(void (*out)(char, void *), void *ptr, va_list *ap);

size_t print_status(void (*out)(char, void *), void *ptr, va_list *ap);


void handle_healtiness_probe(struct mg_connection *c, struct mg_http_message *hm);



void route(struct mg_connection *c, int ev, void *ev_data);


void front_listen_and_serve();

void front_handler(struct mg_connection *c, struct mg_ws_message *wm);

int front_authenticate(struct mg_connection* c, struct mg_ws_message *wm, int* initial, char* command, char* data);

void front_communicate(int chan_idx, char* command, char* data);




#endif