#ifndef _FRANK_CONTROLLER_H_
#define _FRANK_CONTROLLER_H_


#include "frank/core.h"
#include "frank/admin.h"


#include <curl/curl.h>

#include "mongoose/mongoose.h"
#include "cJSON/cJSON.h"



#ifndef HTTP_URL
#define HTTP_URL "http://0.0.0.0:8000"
#endif


#ifndef HTTPS_URL
#define HTTPS_URL "https://0.0.0.0:8443"
#endif

#define MAX_DEVICE_NAME 40
#define MAX_EVENTS_NO 400
#define MAX_EVENT_TEXT_SIZE 10
#define EVENTS_PER_PAGE 20

#define MAX_USER_NAME 1024
#define MAX_USER_PASS 2048
#define MAX_USER_ACCESS_TOKEN 1024

#define MAX_COOKIE_LEN 1024
#define MAX_USERS 10


#define MAX_REQUEST_URI_LEN 1024 * 10
#define MAX_CLIENT_ID_LEN 1024
#define MAX_CLIENT_SECRET_LEN 1024
#define MAX_POST_FIELDS_LEN 1024 * 10

#define MAX_BUFF 1024 * 10 * 10

#define DEBUG_THIS 0

struct ui_event {
  uint8_t type, prio;
  unsigned long timestamp;
  char text[10];
};

struct user {

    int occupied;
    int ticket;

    char name[MAX_USER_NAME]; 
    char access_token[MAX_USER_ACCESS_TOKEN];
    char pass[MAX_USER_PASS]; 
};

struct settings {
  bool log_enabled;
  int log_level;
  long brightness;
  char *device_name;
};

extern struct settings s_settings;

extern uint64_t s_boot_timestamp; 

extern char *s_json_header;

extern struct mg_mgr mgr;

extern struct user user_array[MAX_USERS];

extern char GOOGLE_AUTH_ENDPOINT[MAX_REQUEST_URI_LEN];
extern char GOOGLE_CLIENT_ID[MAX_CLIENT_ID_LEN];
extern char GOOGLE_CLIENT_SECRET[MAX_CLIENT_SECRET_LEN];
extern char OAUTH_REDIRECT_URI[MAX_REQUEST_URI_LEN];


int load_og_api_info();

void sntp_fn(struct mg_connection *c, int ev, void *ev_data);

void timer_sntp_fn(void *param);


int get_session_status(struct mg_http_message *hm);

int add_session_status(struct mg_http_message *hm, char* user_access_token);

int del_session_status(struct mg_http_message *hm, char* user_access_token);

int get_ticket(struct mg_http_message *hm, char* pass);

int add_ticket(struct mg_http_message *hm, char* name, char* new_pass);

int del_ticket(struct mg_http_message *hm);

void handle_goauth2_login(struct mg_connection *c, struct mg_http_message *hm);

void handle_goauth2_login_callback(struct mg_connection *c, struct mg_http_message *hm);

void handle_logout(struct mg_connection *c, struct mg_http_message *hm);

size_t req_write_callback(void *data, size_t size, size_t nmemb, void *clientp);

int request_post_code(char* result, char* req_url, char* code);

int request_get_url(char* result, char* req_url);

void handle_debug(struct mg_connection *c, struct mg_http_message *hm);

size_t print_int_arr(void (*out)(char, void *), void *ptr, va_list *ap);

size_t print_status(void (*out)(char, void *), void *ptr, va_list *ap);

void route(struct mg_connection *c, int ev, void *ev_data);


void frankc_listen_and_serve();






#endif