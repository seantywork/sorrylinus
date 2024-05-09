#include "frank/controller/index.h"
#include "frank/controller/client.h"


struct user user_array[MAX_USERS];


struct settings s_settings = {true, 1, 57, NULL};

uint64_t s_boot_timestamp = 0; 

char* s_json_header =
    "Content-Type: application/json\r\n"
    "Cache-Control: no-cache\r\n";

struct mg_mgr mgr;

char GOOGLE_AUTH_ENDPOINT[MAX_REQUEST_URI_LEN] = {0};
char GOOGLE_TOKEN_ENDPOINT[MAX_REQUEST_URI_LEN] = {0};
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

    cJSON* ogapi_obj = cJSON_Parse(buff);

    cJSON* web_obj = cJSON_GetObjectItemCaseSensitive(ogapi_obj, "web");

    cJSON* auth_uri = cJSON_GetObjectItemCaseSensitive(web_obj, "auth_uri");

    cJSON* token_uri = cJSON_GetObjectItemCaseSensitive(web_obj, "token_uri");

    cJSON* client_id = cJSON_GetObjectItemCaseSensitive(web_obj, "client_id");


    cJSON* client_secret = cJSON_GetObjectItemCaseSensitive(web_obj, "client_secret");

    cJSON* redirect_uris = cJSON_GetObjectItemCaseSensitive(web_obj, "redirect_uris");

    strcpy(GOOGLE_AUTH_ENDPOINT, auth_uri->valuestring);

    strcpy(GOOGLE_TOKEN_ENDPOINT, token_uri->valuestring);

    strcpy(GOOGLE_CLIENT_ID, client_id->valuestring);

    strcpy(GOOGLE_CLIENT_SECRET, client_secret->valuestring);


    int arr_idx = 0;
    cJSON* resolution = NULL;

    cJSON_ArrayForEach(resolution, redirect_uris){

        cJSON* uri = cJSON_GetArrayItem(redirect_uris, arr_idx);
        

        if(DEBUG_LOCAL == 1){

            if(arr_idx == 0){

                strcpy(OAUTH_REDIRECT_URI, uri->valuestring);

                break;

            }

        }

        if(arr_idx == 1){

            
            strcpy(OAUTH_REDIRECT_URI, uri->valuestring);


            break;

        }

        arr_idx += 1;
    }


    printf("auth uri: %s\n", GOOGLE_AUTH_ENDPOINT);
    printf("token uri: %s\n", GOOGLE_TOKEN_ENDPOINT);
    printf("client id: %s\n", GOOGLE_CLIENT_ID);
    printf("client secret: %s\n", GOOGLE_CLIENT_SECRET);
    printf("redirect uri: %s\n", OAUTH_REDIRECT_URI);

    return arr_idx;

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

int cookie_parse(char* result, char* cookie, char* key){


    char* ptr = NULL;

    char key_check[MAX_COOKIE_KEYLEN] = {0};

    int key_check_len = 0;

    strcpy(key_check, key);

    strcat(key_check, "=");

    ptr = strstr(cookie, key_check);

    if(ptr == NULL){
        return -1;
    }

    key_check_len = strlen(key_check);

    char* result_ptr = result;

    int keylen = DEFAULT_RANDLEN * 2;

    ptr = ptr + key_check_len;

    if(strlen(ptr) < keylen){

        printf("access token format invalid\n");

        return -2;

    }

    for(int i = 0; i < keylen; i++){


        *result_ptr = *ptr;

        result_ptr += 1;

        ptr += 1;

    }

    return keylen;

}


int code_parse(char* result, char* query){

    int codelen = 0;

    char* key_start = "code=";

    char* key_end = "&";

    char* start_ptr;
    char* end_ptr;
    char* ptr;
    char* result_ptr = result;


    start_ptr = strstr(query, key_start);

    if(start_ptr == NULL){
        return -1;
    }
    end_ptr = strstr(query, key_end);

    if(end_ptr == NULL){

        return -2;

    }

    start_ptr += strlen(key_start);


    int diff = end_ptr - start_ptr;

    if(diff <= 0){
        return -3;
    }


    ptr = start_ptr;

    for(int i = 0;i < MAX_CODELEN; i++){

        if(*ptr == '&'){
            break;
        }

        *result_ptr = *ptr;
        
        result_ptr += 1;
        ptr += 1;

        codelen += 1;
    }


    return codelen;



}

int get_session_status(struct mg_http_message *hm){


    struct mg_str* header_str;
    

    header_str = mg_http_get_header(hm, "Cookie");

    if(header_str == NULL){

        printf("header cookie not found\n");

        return -1;
    } 

    char result_str[MAX_USER_ACCESS_TOKEN] = {0};


    int cookie_len = cookie_parse(result_str, header_str->buf, "access_token");

    if(cookie_len < 0){

        printf("session cookie not found\n");

        return -1;
    } 

    printf("result str: %s\n", result_str);
    

    int compare_len = cookie_len;

    int found = 0;

    int idx = -1;

    for(int i = 0; i < MAX_USERS; i++){

        if(user_array[i].occupied != 1){
            continue;
        }

        if(strncmp(user_array[i].access_token, result_str, MAX_USER_ACCESS_TOKEN) == 0){

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


    gen_random_bytestream(randbytes, DEFAULT_RANDLEN);

    bin2hex(user_access_token, DEFAULT_RANDLEN, randbytes);

    return idx;
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
    
    int result = -1;

    int result_idx = get_session_status(hm);

    if(result_idx < 0){

        printf("failed to add ticket: no session\n");

        return -1;

    }

    if(DEBUG_THIS == 1){

        result = 0;


    } else {

        result = admin_insert_user(name, new_pass);
    }


    if(result < 0){

        printf("failed to add ticket: admin insert\n");

        return -2;

    }

    result = client_connect(result_idx);

    if(result < 0){

        printf("failed to add ticket: connect to front\n");

        return -3;

    }


    uint8_t header[HUB_HEADER_BYTELEN]= {0};
    uint64_t body_len = 0;
    int flag = 0;
    uint8_t* rbuff;
    uint8_t wbuff[MAX_BUFF] = {0};


    strcpy(header, HUB_HEADER_AUTHFRONT);

    strcat(wbuff, name);
    strcat(wbuff, ":");
    strcat(wbuff, new_pass);

    body_len = strlen(wbuff) + 1;


    result = client_write_packet(result_idx, header, body_len, wbuff);

    if(result < 0){

        printf("failed to add ticket: send auth to front\n");

        return -4;
    
    }

    memset(header, 0, HUB_HEADER_BYTELEN);

    body_len = 0;

    rbuff = client_read_packet(result_idx, header, &body_len, &flag);

    if(flag <= 0){

        printf("failed to add ticket: read auth from front: %d\n", flag);

        return -5;

    }

    printf("auth result: %s\n", rbuff);

    if(strcmp(rbuff, "SUCCESS")!=0){

        printf("failed to add ticket: auth failed: %s\n", rbuff);

        free(rbuff);

        return -6;

    }


    strcpy(user_array[result_idx].name, name);

    strcpy(user_array[result_idx].pass, new_pass);

    user_array[result_idx].ticket = 1;

    return 1;
}

int del_ticket(struct mg_http_message *hm){

    int result = 0;

    int result_idx = get_session_status(hm);

    if(result_idx < 0){

        printf("failed to del ticket: no session\n");

        return -1;

    }

    if(DEBUG_THIS == 1){

        result = 0;

    } else {

        result = admin_eject_user(user_array[result].name);
    }
    

    if(result < 0){

        printf("failed to del ticket: admin eject\n");

        return -2;

    }


    user_array[result_idx].ticket = 0;

    memset(user_array[result_idx].name, 0, MAX_USER_NAME);
    memset(user_array[result_idx].pass, 0, MAX_USER_PASS);

    return 0;
}


int get_ticket_ws(char* token, char* ticket){

    int found = 0;
    int idx = -1;

    for(int i = 0 ; i < MAX_USERS; i ++){


        if(user_array[i].ticket != 1){
            continue;
        }

        if(strcmp(user_array[i].access_token, token) == 0){

            found = 1;

            idx = i;

            break;
        }


    }

    if(found != 1){

        printf("failed to get ticket ws\n");

        return -1;
    }

    strcpy(ticket, user_array[idx].pass);

    return idx;
}


void handle_goauth2_login(struct mg_connection *c, struct mg_http_message *hm) {

    char redirect_header[MAX_REQUEST_URI_LEN] = {0};


    char request_uri[MAX_REQUEST_URI_LEN] = {0};


    char cookie[MAX_COOKIE_LEN];


    int user_idx = get_session_status(hm);

    if(user_idx < 0){

        char user_token[MAX_USER_ACCESS_TOKEN] = {0};

        int new_idx = add_session_status(hm, user_token);

        if(new_idx < 0){

            printf("connection maxed out\n");

            mg_http_reply(c, 500, "","%m", MG_ESC("connection full"));

            return;

        }

        user_idx = new_idx;

        strcpy(user_array[user_idx].access_token, user_token);

        printf("access token: %s\n", user_array[user_idx].access_token);

        mg_snprintf(cookie, sizeof(cookie),
                    "Set-Cookie: access_token=%s; Path=/;\r\n",
                    user_array[user_idx].access_token);
        

    } else {

        if(user_array[user_idx].ticket == 1){

            strcpy(request_uri,"/frank.html");

            mg_snprintf(redirect_header, sizeof(redirect_header),
                                    "Location: %s\r\n",
                                    request_uri);
            

            mg_http_reply(c, 302, redirect_header,"");

            return;

        }

    }

    strcat(request_uri, GOOGLE_AUTH_ENDPOINT);
    strcat(request_uri, "?");

    strcat(request_uri, "client_id=");
    strcat(request_uri, GOOGLE_CLIENT_ID);
    strcat(request_uri, "&");

    strcat(request_uri, "redirect_uri=");
    strcat(request_uri, OAUTH_REDIRECT_URI);
    strcat(request_uri, "&");

    strcat(request_uri, "scope=https://www.googleapis.com/auth/userinfo.email&");
    strcat(request_uri, "response_type=code");

    printf("oauth2 login request: %s\n", request_uri);


    mg_snprintf(redirect_header, sizeof(redirect_header),
                            "Location: %s\r\n%s",
                            request_uri,
                            cookie);
    

    mg_http_reply(c, 302, redirect_header,"");


}


void handle_healtiness_probe(struct mg_connection *c, struct mg_http_message *hm){

    char* ticket[MAX_USER_PASS] = {0};

    char rest_buff[MAX_REST_BUFF] = {0};


    cJSON* response = cJSON_CreateObject();

    int datalen = 0;


    int result = get_ticket(hm, ticket);


    if(result < 0){
        printf("failed to get ticket\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));

        strcpy(rest_buff, cJSON_Print(response));

        datalen = strlen(rest_buff);

        mg_http_reply(c, 403, "", rest_buff);

        return;

    }

    cJSON_AddItemToObject(response, "status", cJSON_CreateString("SUCCESS"));
    cJSON_AddItemToObject(response, "data", cJSON_CreateString("fine"));

    strcpy(rest_buff, cJSON_Print(response));

    datalen = strlen(rest_buff);

    mg_http_reply(c, 200, "", rest_buff);



}


void handle_goauth2_login_callback(struct mg_connection *c, struct mg_http_message *hm) {

    char request_uri[MAX_REQUEST_URI_LEN] = {0};

    char redirect_header[MAX_REQUEST_URI_LEN] = {0};

    char buff[MAX_BUFF] = {0};

    int user_idx = get_session_status(hm);

    if(user_idx < 0){

        printf("invalid access\n");

        mg_http_reply(c, 403, "","%m", MG_ESC("invalid access"));

        return;

    }

    printf("callback access: %s\n", user_array[user_idx].access_token);

    char code[MAX_PW_LEN] = {0};


    int codelen = code_parse(code, hm->query.buf);

    if(codelen < 0){

        printf("invalid code\n");

        mg_http_reply(c, 403, "","%m", MG_ESC("invalid access"));

        return;

    }
    
    printf("code: %s\n", code);


    int post_result = request_post_code(buff, GOOGLE_TOKEN_ENDPOINT, code);

    if(post_result < 0){

        printf("failed to post code\n");

        mg_http_reply(c, 403, "","%m", MG_ESC("invalid access"));

        return;
    }

    char goauth_token[GOAUTH_TOKENLEN] = {0}; 

    cJSON* oadata = cJSON_Parse(buff);

    cJSON* goauth_token_obj = cJSON_GetObjectItemCaseSensitive(oadata, "access_token");

    strcpy(request_uri, GOAUTH_USER_INFO_API);

    strcat(request_uri, goauth_token_obj->valuestring);
    
    memset(buff, 0, MAX_BUFF);

    int token_result = request_get_url(buff, request_uri);

    if(token_result < 0){

        printf("failed to auth token\n");

        mg_http_reply(c, 403, "","%m", MG_ESC("invalid access"));

        return;
    }


    cJSON* uidata = cJSON_Parse(buff);

    cJSON* error = cJSON_GetObjectItemCaseSensitive(uidata, "error");

    if(error != NULL){

        printf("failed to authenticate \n");

        mg_http_reply(c, 403, "","%m", MG_ESC("invalid access"));

        return;

    }

    cJSON* email = cJSON_GetObjectItemCaseSensitive(uidata, "email");


    uint8_t pw[MAX_PW_LEN] = {0};

    if(DEBUG_THIS == 1){

        strcpy(pw, "blahblah");

        printf("pw: %s\n", pw);

    }

#ifdef ALLOWED_USER

    if(strcmp(ALLOWED_USER, email->valuestring) != 0){

        printf("user %s not allowed\n", email->valuestring);

        mg_http_reply(c, 403, "","%m", MG_ESC("invalid access"));


    }

#endif

    int ticket_result = add_ticket(hm, email->valuestring, pw);

    if(ticket_result < 0){

        printf("failed to issue ticket \n");

        mg_http_reply(c, 500, "","%m", MG_ESC("internal error"));

        return;

    }

    //char cookie[MAX_COOKIE_LEN] = {0};



    mg_snprintf(redirect_header, sizeof(redirect_header),
                            "Location: %s\r\n",
                            "/frank.html");
    

    mg_http_reply(c, 302, redirect_header,"");


}


void handle_logout(struct mg_connection *c, struct mg_http_message *hm) {

    char header[MAX_REQUEST_URI_LEN] = {0};

    char rest_buff[MAX_REST_BUFF] = {0};

    int user_idx = get_session_status(hm);

    int datalen = 0;

    cJSON* response = cJSON_CreateObject();

    if(user_idx < 0){

        printf("not a connection to logout\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));

        strcpy(rest_buff, cJSON_Print(response));

        datalen = strlen(rest_buff);

        mg_http_reply(c, 403, "", rest_buff);

        return;

    }

    if(user_array[user_idx].ticket != 1){

        printf("no valid ticket to remove \n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));

        strcpy(rest_buff, cJSON_Print(response));

        datalen = strlen(rest_buff);

        mg_http_reply(c, 403, "", rest_buff);

        return;

    }

    int del_result = del_ticket(hm);

    if (del_result < 0){

        printf("failed to remove ticket\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));

        strcpy(rest_buff, cJSON_Print(response));

        datalen = strlen(rest_buff);

        mg_http_reply(c, 500, "", rest_buff);

        return;

    }

    int del_sess = del_session_status(hm, user_array[user_idx].access_token);

    if (del_result < 0){

        printf("failed to remove ticket\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));

        strcpy(rest_buff, cJSON_Print(response));

        datalen = strlen(rest_buff);

        mg_http_reply(c, 500, "", rest_buff);

        return;

    }

    printf("logout successful\n");

    char cookie[MAX_COOKIE_LEN] = {0};

    mg_snprintf(cookie, sizeof(cookie),
                "Set-Cookie: access_token=; Path=/; \r\n");

    mg_snprintf(header, sizeof(header),"%s", cookie);
    

    cJSON_AddItemToObject(response, "status", cJSON_CreateString("SUCCESS"));
    cJSON_AddItemToObject(response, "data", cJSON_CreateString("SUCCESS"));

    strcpy(rest_buff, cJSON_Print(response));

    datalen = strlen(rest_buff);

    mg_http_reply(c, 200, header, rest_buff);
}


void handle_web_root(struct mg_connection *c, struct mg_http_message *hm) {


    struct mg_http_serve_opts opts;
    memset(&opts, 0, sizeof(opts));
#if MG_ARCH == MG_ARCH_UNIX || MG_ARCH == MG_ARCH_WIN32
    opts.root_dir = "web_root";  // On workstations, use filesystem
    
#else
    opts.root_dir = "/web_root";  // On embedded, use packed files
    opts.fs = &mg_fs_packed;
#endif
    

    mg_http_serve_dir(c, hm, &opts);


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

        handle_healtiness_probe(c, hm);

    } else if (mg_match(hm->uri, mg_str("/oauth2/google/login"), NULL)) {

        handle_goauth2_login(c, hm);

    } else if (mg_match(hm->uri, mg_str("/oauth2/google/callback"), NULL)) {

        handle_goauth2_login_callback(c, hm);

    } else if (mg_match(hm->uri, mg_str("/signout"), NULL)) {

        handle_logout(c, hm);

    } else if (mg_match(hm->uri, mg_str("/front-client"), NULL)){

        printf("WS UPGRADE!!!!!\n");

        mg_ws_upgrade(c, hm, NULL);


    }  else {

        handle_web_root(c, hm);
    }
    if(DEBUG_THIS == 1){

        MG_DEBUG(("%lu %.*s %.*s -> %.*s", c->id, (int) hm->method.len,
                hm->method.buf, (int) hm->uri.len, hm->uri.buf, (int) 3,
                &c->send.buf[9]));

    }
  } else if (ev == MG_EV_WS_MSG) {

    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    
    ws_handler(c, wm);

  }
}




void ws_handler(struct mg_connection *c, struct mg_ws_message *wm){

    char ws_buff[MAX_WS_BUFF] = {0};

    char ws_command[WS_MAX_COMMAND_LEN] = {0};

    char ws_access_token[MAX_USER_ACCESS_TOKEN] = {0};

    cJSON* response = cJSON_CreateObject();

    int datalen = 0;

    if(wm->data.len > MAX_WS_BUFF){

        printf("failed handle ws: data too big\n");
        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;
    }


    cJSON* req_obj = cJSON_Parse(wm->data.buf);

    if(req_obj == NULL){

        printf("failed handle ws: data invalid\n");


        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;

    }

    cJSON* command = cJSON_GetObjectItemCaseSensitive(req_obj, "command");

    if(command == NULL){

        printf("failed handle ws: data invalid\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;

    }

    cJSON* token = cJSON_GetObjectItemCaseSensitive(req_obj, "access_token");

    if(token == NULL){

        printf("failed handle ws: data invalid\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;

    }

    printf("command: %s\n", command->valuestring);

    datalen = strlen(command->valuestring);

    if(datalen > WS_MAX_COMMAND_LEN){

        printf("failed handle ws: command too long\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;
    }

    datalen = strlen(token->valuestring);

    if(datalen > MAX_USER_ACCESS_TOKEN){

        printf("failed handle ws: token too long\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;
    }
    strcpy(ws_command, command->valuestring);

    strcpy(ws_access_token, token->valuestring);

    char ws_ticket[MAX_USER_PASS] = {0};

    int ticket_result = get_ticket_ws(ws_access_token, ws_ticket);

    if(ticket_result < 0){

        printf("failed handle ws: no ticket\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;
    }


    if(strcmp(ws_command, WS_COMMAND_REQ_KEY) == 0){

        printf("sending req key\n");


        cJSON_AddItemToObject(response, "status", cJSON_CreateString("SUCCESS"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString(ws_ticket));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);   

    } else if (strcmp(ws_command, WS_COMMAND_ROUNDTRIP) == 0) {

        printf("roundtrip\n");

        cJSON* rt_data = cJSON_GetObjectItemCaseSensitive(req_obj, "data");

        if(rt_data == NULL){

            printf("failed handle ws: no data field\n");

            cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
            cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
            
            strcpy(ws_buff, cJSON_Print(response));

            datalen = strlen(ws_buff);

            mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
            
            return;

        }

        int rt_data_len = strlen(rt_data->valuestring);

        if(rt_data_len > WS_MAX_COMMAND_DATA_LEN){

            printf("failed handle ws: data len too long\n");

            cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
            cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
            
            strcpy(ws_buff, cJSON_Print(response));

            datalen = strlen(ws_buff);

            mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
            
            return;

        }

        int uidx = ticket_result;

        int recv_len = 0;

        uint8_t header[HUB_HEADER_BYTELEN] = {0};
        uint64_t body_len = rt_data_len;
        uint8_t recv_buff[WS_MAX_COMMAND_RECV_LEN] = {0};

        int rt_result = client_exchange(uidx, &recv_len, recv_buff, body_len, rt_data->valuestring);

        if(rt_result < 0){
            printf("failed handle ws: exchange failed\n");

            cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
            cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
            
            strcpy(ws_buff, cJSON_Print(response));

            datalen = strlen(ws_buff);

            mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
            
            return;

        }

        printf("exchange success\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("SUCCESS"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString(recv_buff));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);


    } else {

        printf("failed handle ws: no such command\n");

        cJSON_AddItemToObject(response, "status", cJSON_CreateString("FAIL"));
        cJSON_AddItemToObject(response, "data", cJSON_CreateString("null"));
        
        strcpy(ws_buff, cJSON_Print(response));

        datalen = strlen(ws_buff);

        mg_ws_send(c, ws_buff, datalen, WEBSOCKET_OP_TEXT);
        
        return;

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

