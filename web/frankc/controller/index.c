#include "frank/controller/index.h"


struct user user_array[MAX_USERS];


struct settings s_settings = {true, 1, 57, NULL};

uint64_t s_boot_timestamp = 0; 

char* s_json_header =
    "Content-Type: application/json\r\n"
    "Cache-Control: no-cache\r\n";

struct mg_mgr mgr;

char GOOGLE_AUTH_ENDPOINT[MAX_REQUEST_URI_LEN] = {0};
char GOOGLE_CLIENT_ID[MAX_CLIENT_ID_LEN] = {0};
char GOOGLE_CLIENT_SECRET[MAX_CLIENT_SECRET_LEN] = {0};
char OAUTH_REDIRECT_URI[MAX_REQUEST_URI_LEN] = {0};


int load_og_api_info(){

    uint8_t buff[MAX_BUFF] = {0};

    int result = read_file_to_buffer(buff, MAX_BUFF, "ogapi.json");

    if (result < 0){

        printf("failed to load og api\n");

        return result;
    }



}


void sntp_fn(struct mg_connection *c, int ev, void *ev_data) {
  uint64_t *expiration_time = (uint64_t *) c->data;
  if (ev == MG_EV_OPEN) {
    *expiration_time = mg_millis() + 3000;  // Store expiration time in 3s
  } else if (ev == MG_EV_SNTP_TIME) {
    uint64_t t = *(uint64_t *) ev_data;
    s_boot_timestamp = t - mg_millis();
    c->is_closing = 1;
  } else if (ev == MG_EV_POLL) {
    if (mg_millis() > *expiration_time) c->is_closing = 1;
  }
}


void timer_sntp_fn(void *param) {  // SNTP timer function. Sync up time
  mg_sntp_connect(param, "udp://time.google.com:123", sntp_fn, NULL);
}



int get_session_status(struct mg_http_message *hm){

    struct mg_str* result_str;

    result_str = mg_http_get_header(hm, "access_token");

    if(result_str == NULL){

        printf("session cookie not found\n");

        return -1;
    } 

    int compare_len = 0;

    if(result_str->len > MAX_USER_ACCESS_TOKEN){

        printf("access token max len exceeded\n");

        return -2;

    } else {

        compare_len = result_str->len;

    }


    int found = 0;

    int idx = -1;

    for(int i = 0; i < MAX_USERS; i++){

        if(user_array[i].occupied != 1){
            continue;
        }

        if(strncmp(user_array[i].access_token, result_str->buf, compare_len) == 0){

            found = 1;

            idx = i;

            break;

        }


    }

    if(found == 0){

        printf("access token not found\n");

        return -3;

    } 

    printf("access token found \n");

    return idx;
}

int add_session_status(struct mg_http_message *hm,  char* user_access_token){

    int idx = -1;
    int found = 0;

    uint8_t randbytes[MAX_USER_ACCESS_TOKEN] = {0};

    for(int i = 0 ; i < MAX_USERS; i ++){

        if(user_array[i].occupied == 1){
            continue;
        }

        found = 1;

        idx = i;

        user_array[i].occupied = 1;

        break;
    }

    if(found != 1){

        printf("available user slot not found \n");

        return -1;
    }


    gen_random_bytestream(randbytes, 64);

    bin2hex(user_access_token, 64, randbytes);

    return 0;
}


int del_session_status(struct mg_http_message *hm, char* user_access_token){

    int found = 0;

    int idx = -1;

    for(int i = 0 ; i < MAX_USERS; i++){

        if(user_array[i].occupied != 1){
            continue;
        }

        if(strcmp(user_array[i].access_token, user_access_token) == 0){

            idx = i;

            found = 1;

            break;
        }

    }

    if(found != 1){

        printf("session to delete not found\n");

        return -1;

    }

    user_array[idx].occupied = 0;

    memset(user_array[idx].access_token, 0, MAX_USER_ACCESS_TOKEN);

    printf("deleted session\n");

    return 0;
}

int get_ticket(struct mg_http_message *hm, char* pass){

    int result = get_session_status(hm);

    if(result < 0){

        printf("failed to get ticket: no session\n");

        return -1;

    }

    if(user_array[result].ticket != 1){

        printf("failed to get ticket: not yet issued\n");

        return -2;

    }

    strcpy(pass, user_array[result].pass);


    return 0;
}

int add_ticket(struct mg_http_message *hm, char* name, char* new_pass){
 

    int result = get_session_status(hm);

    if(result < 0){

        printf("failed to add ticket: no session\n");

        return -1;

    }

    
    result = admin_insert_user(name, new_pass);

    if(result < 0){

        printf("failed to add ticket: admin insert\n");

        return -2;

    }

    strcpy(user_array[result].pass, new_pass);

    user_array[result].ticket = 1;


    return 1;
}

int del_ticket(struct mg_http_message *hm){



    int result = get_session_status(hm);

    if(result < 0){

        printf("failed to del ticket: no session\n");

        return -1;

    }


    
    result = admin_eject_user(user_array[result].name);

    if(result < 0){

        printf("failed to del ticket: admin eject\n");

        return -2;

    }


    user_array[result].ticket = 0;

    return 0;
}




void handle_goauth2_login(struct mg_connection *c, struct mg_http_message *hm) {
  char cookie[MAX_COOKIE_LEN];
  mg_snprintf(cookie, sizeof(cookie),
              "Set-Cookie: access_token=%s; Path=/; "
              "%sHttpOnly; SameSite=Lax; Max-Age=%d\r\n",
              "CHANGE", c->is_tls ? "Secure; " : "", 3600 * 24);
  mg_http_reply(c, 200, cookie, "{%m:%m}", MG_ESC("user"), MG_ESC("CAHNGE"));
}

void handle_goauth2_login_callback(struct mg_connection *c, struct mg_http_message *hm) {
  char cookie[MAX_COOKIE_LEN];
  mg_snprintf(cookie, sizeof(cookie),
              "Set-Cookie: access_token=%s; Path=/; "
              "%sHttpOnly; SameSite=Lax; Max-Age=%d\r\n",
              "CHANGE", c->is_tls ? "Secure; " : "", 3600 * 24);
  mg_http_reply(c, 200, cookie, "{%m:%m}", MG_ESC("user"), MG_ESC("CHANGE"));
}


void handle_logout(struct mg_connection *c, struct mg_http_message *hm) {
  char cookie[MAX_COOKIE_LEN];
  mg_snprintf(cookie, sizeof(cookie),
              "Set-Cookie: access_token=; Path=/; "
              "Expires=Thu, 01 Jan 1970 00:00:00 UTC; "
              "%sHttpOnly; Max-Age=0; \r\n",
              c->is_tls ? "Secure; " : "");
  mg_http_reply(c, 200, cookie, "true\n");
}


size_t req_write_callback(void *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = size * nmemb;

    memcpy(clientp, data, realsize);

    return realsize;
}

int request_post_code(char* result, char* req_url, char* code){

    //const char* c_req_url = req_url.c_str();

    CURL *curl;
    CURLcode res;

    char post_fields[MAX_POST_FIELDS_LEN] = {0};


    strcat(post_fields, "code=");
    strcat(post_fields, code);
    strcat(post_fields, "&");
    
    strcat(post_fields, "client_id=");
    strcat(post_fields, GOOGLE_CLIENT_ID);
    strcat(post_fields, "&");

    strcat(post_fields, "client_secret=");
    strcat(post_fields, GOOGLE_CLIENT_SECRET);
    strcat(post_fields, "&");

    strcat(post_fields, "redirect_uri=");
    strcat(post_fields, OAUTH_REDIRECT_URI);
    strcat(post_fields, "&");

    strcat(post_fields, "grant_type=authorization_code");

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, req_url);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_fields);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)result);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);

    } else {
        strcpy(result,"curl init failed");
        return -1;
    }


    return 0;
}

int request_get_url(char* result, char* req_url){

    //const char* c_req_url = req_url.c_str();

    CURL *curl;
    CURLcode res;


    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, req_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, result);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);

    } else {
        strcpy(result, "curl init failed");
        return -1;
    }

    return 0;
}

void handle_debug(struct mg_connection *c, struct mg_http_message *hm) {
  int level = mg_json_get_long(hm->body, "$.level", MG_LL_DEBUG);
  mg_log_set(level);
  mg_http_reply(c, 200, "", "Debug level set to %d\n", level);
}

size_t print_int_arr(void (*out)(char, void *), void *ptr, va_list *ap) {
  size_t i, len = 0, num = va_arg(*ap, size_t);  // Number of items in the array
  int *arr = va_arg(*ap, int *);              // Array ptr
  for (i = 0; i < num; i++) {
    len += mg_xprintf(out, ptr, "%s%d", i == 0 ? "" : ",", arr[i]);
  }
  return len;
}

size_t print_status(void (*out)(char, void *), void *ptr, va_list *ap) {
  int fw = va_arg(*ap, int);
  return mg_xprintf(out, ptr, "{%m:%d,%m:%c%lx%c,%m:%u,%m:%u}\n",
                    MG_ESC("status"), mg_ota_status(fw), MG_ESC("crc32"), '"',
                    mg_ota_crc32(fw), '"', MG_ESC("size"), mg_ota_size(fw),
                    MG_ESC("timestamp"), mg_ota_timestamp(fw));
}



void route(struct mg_connection *c, int ev, void *ev_data) {
  
  if (ev == MG_EV_ACCEPT) {
  
    if (c->fn_data != NULL) {  
      struct mg_tls_opts opts = {0};
      opts.cert = mg_unpacked("/certs/server_cert.pem");
      opts.key = mg_unpacked("/certs/server_key.pem");
      mg_tls_init(c, &opts);
    }
  
  } else if (ev == MG_EV_HTTP_MSG) {

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;


    if (mg_match(hm->uri, mg_str("/test/frank-health"), NULL)) {



        mg_http_reply(c, 200, "", "{%m:%m}", MG_ESC("message"), MG_ESC("fine"));

    } else if (mg_match(hm->uri, mg_str("/oauth2/google/login"), NULL)) {

        handle_goauth2_login(c, hm);

    } else if (mg_match(hm->uri, mg_str("/oauth2/google/callback"), NULL)) {

        handle_goauth2_login_callback(c, hm);

    } else if (mg_match(hm->uri, mg_str("/signout"), NULL)) {

        handle_logout(c, hm);

    } else if (mg_match(hm->uri, mg_str("/front-client"), NULL)){

        mg_ws_upgrade(c, hm, NULL);


    } else {

        struct mg_http_serve_opts opts;
        memset(&opts, 0, sizeof(opts));
#if MG_ARCH == MG_ARCH_UNIX || MG_ARCH == MG_ARCH_WIN32
      opts.root_dir = "web_root";  // On workstations, use filesystem
#else
      opts.root_dir = "/web_root";  // On embedded, use packed files
      opts.fs = &mg_fs_packed;
#endif


        mg_http_serve_dir(c, ev_data, &opts);
    }
    MG_DEBUG(("%lu %.*s %.*s -> %.*s", c->id, (int) hm->method.len,
              hm->method.buf, (int) hm->uri.len, hm->uri.buf, (int) 3,
              &c->send.buf[9]));
  } else if (ev == MG_EV_WS_MSG) {

    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    mg_ws_send(c, wm->data.buf, wm->data.len, WEBSOCKET_OP_TEXT);

  }
}



void frankc_listen_and_serve(){

    mg_mgr_init(&mgr);

    s_settings.device_name = strdup("frankc");

    mg_http_listen(&mgr, HTTP_URL, route, NULL);


    mg_timer_add(&mgr, 3600 * 1000, MG_TIMER_RUN_NOW | MG_TIMER_REPEAT,
                timer_sntp_fn, &mgr);
    
    while (s_sig_num == 0) {
        mg_mgr_poll(&mgr, 50);
    }

    mg_mgr_free(&mgr);

}

