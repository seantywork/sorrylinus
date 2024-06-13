#include "frankhub/v2/core.h"


struct CHANNEL_CONTEXT CHAN_CTX[MAX_CONN];

struct SOCK_CONTEXT SOCK_CTX[MAX_CONN];


struct user user_array[MAX_USERS];


int load_config_json(){

    uint8_t buff[MAX_BUFF] = {0};

    int result = read_file_to_buffer(buff, MAX_BUFF, "config.json");

    if (result < 0){

        printf("failed to load config \n");

        return result;
    }

    cJSON* config_obj = cJSON_Parse(buff);


    cJSON* users = cJSON_GetObjectItemCaseSensitive(config_obj, "users");

    int arr_idx = 0;
    cJSON* resolution = NULL;

    cJSON_ArrayForEach(resolution, users){

        cJSON* u = cJSON_GetArrayItem(users, arr_idx);

        cJSON* id = cJSON_GetObjectItemCaseSensitive(u, "id");

        cJSON* pw = cJSON_GetObjectItemCaseSensitive(u, "pw");

        strcpy(user_array[arr_idx].name, id->valuestring);

        strcpy(user_array[arr_idx].pass, pw->valuestring);

        arr_idx += 1;
    }


    return 0;
}


int make_socket_non_blocking (int sfd){
    int flags, s;

    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror ("fcntl get");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl (sfd, F_SETFL, flags);
    if (s == -1)
    {
        perror ("fcntl set");
        return -2;
    }

    return 0;
}


SSL_CTX *create_context(){

    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx){

    if (SSL_CTX_use_certificate_file(ctx, SERVER_CERT, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int sig_verify(const char* cert_pem, const char* intermediate_pem){

    BIO *b = BIO_new(BIO_s_mem());
    BIO_puts(b, intermediate_pem);
    X509 * issuer = PEM_read_bio_X509(b, NULL, NULL, NULL);
    EVP_PKEY *signing_key=X509_get_pubkey(issuer);
 
    BIO *c = BIO_new(BIO_s_mem());
    BIO_puts(c, cert_pem);
    X509 * x509 = PEM_read_bio_X509(c, NULL, NULL, NULL);
 
    int result = X509_verify(x509, signing_key);
 
    EVP_PKEY_free(signing_key);
    BIO_free(b);
    BIO_free(c);
    X509_free(x509);
    X509_free(issuer);
 
    return result;
}

int extract_common_name(uint8_t* common_name, const char* cert) {

    BIO *b = BIO_new(BIO_s_mem());
    BIO_puts(b, cert);
    X509 * x509 = PEM_read_bio_X509(b, NULL, NULL, NULL);

    int ret_cn = -1;

    int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(x509), NID_commonName, -1);
    if (common_name_loc < 0) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }
    
    X509_NAME_ENTRY * common_name_entry = X509_NAME_get_entry(X509_get_subject_name(x509), common_name_loc);
    if (common_name_entry == NULL) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }
    
    ASN1_STRING * common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (common_name_asn1 == NULL) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }
    
    char const * common_name_str = (char const *) ASN1_STRING_get0_data(common_name_asn1);
    
    if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }

    strcpy(common_name, common_name_str);

    ret_cn = 1;
    
    BIO_free(b);
    X509_free(x509);

    return ret_cn;
}


int user_idpw_verify(char* idpw, char* ret_id){

    int verify = -1;

    char* token;

    char* delim = ":";

    uint8_t id[MAX_ID_LEN] = {0};
    uint8_t pw[MAX_PW_LEN] = {0};

    token = strtok(idpw, delim);
    
    int idx = 0;
    
    while( token != NULL ) {

        if(idx > 1) {
            break;
        }

        if(idx == 0){

            strncpy(id, token, MAX_ID_LEN);

        } else {

            strncpy(pw, token, MAX_PW_LEN);

        }
        
        token = strtok(NULL, delim);

        idx += 1;
    }

    if(idx != 2){
        return -1;
    }

    int user_idx = get_user_by_id(id);

    if(user_idx < 0){

        return -2;

    }

    if(strncmp(user_array[user_idx].pass, pw, MAX_PW_LEN) != 0){

        return -3;

    }


    int verified_idx = user_idx;

    strcpy(ret_id, id);

    return verified_idx;

}


int get_user_by_id(char* id){


    for(int i = 0; i < MAX_USERS; i++){

        if(strcmp(user_array[i].name, id) == 0){

            return i;

        }

    }


    return -1;
}




int update_chanctx_from_sockctx(int fd, char* id){


    int ret_idx = -1;


    int sock_idx = get_sockctx_by_fd(fd);

    if(sock_idx < 0){

        printf("no sock fd associated with: %d\n",fd);

        return -1;
    }

    int chan_idx = get_chanctx_by_id(id);


    if(chan_idx < 0){

        ret_idx = calloc_chanctx();

        if(ret_idx < 0){

            printf("sock fd %d failed to calloc chanctx: max conn\n", fd);

            return -2;

        }

        chan_idx = ret_idx;

        CHAN_CTX[chan_idx].sockfd = SOCK_CTX[sock_idx].sockfd;
        CHAN_CTX[chan_idx].ssl = SOCK_CTX[sock_idx].ssl;
        CHAN_CTX[chan_idx].ctx = SOCK_CTX[sock_idx].ctx;

        free_sockctx(sock_idx, 0);

        strcpy(CHAN_CTX[chan_idx].id, id);

        printf("id %s added to chanctx %d from sock\n", id, chan_idx);

    } else {


        CHAN_CTX[chan_idx].sockfd = SOCK_CTX[sock_idx].sockfd;
        CHAN_CTX[chan_idx].ssl = SOCK_CTX[sock_idx].ssl;
        CHAN_CTX[chan_idx].ctx = SOCK_CTX[sock_idx].ctx;

        free_sockctx(sock_idx, 0);

        printf("id %s modified for existing chanctx from sock\n", id);


    }

    ret_idx = chan_idx;

    return ret_idx;

}

int update_chanctx_from_front(struct mg_connection* c, char* id){


    int ret_idx = -1;

    int fid = (int)c->id;

    int chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){


        int new_chan_idx = calloc_chanctx();

        if(new_chan_idx < 0){

            printf("front id %d failed to calloc chanctx: max conn\n", fid);

            return -1;

        }

        CHAN_CTX[new_chan_idx].frontc = c;

        CHAN_CTX[new_chan_idx].frontid = fid;

        strcpy(CHAN_CTX[new_chan_idx].id, id);

        ret_idx = new_chan_idx;

        printf("id %s added to chanctx %d from front\n", id, ret_idx);
    
    } else {


        CHAN_CTX[chan_idx].frontc = c;

        CHAN_CTX[chan_idx].frontid = fid;

        ret_idx = chan_idx;

        printf("id %s modified for existing chanctx from front\n", id);

    }


    return ret_idx;

}


int get_chanctx_by_id(char* id){


    for(int i = 0; i < MAX_CONN; i++){

        if(strcmp(CHAN_CTX[i].id, id) == 0){

            return i;

        }

    }


    return -1;
}



int get_chanctx_by_fd(int fd){




    for(int i = 0; i < MAX_CONN; i++){

        if(CHAN_CTX[i].sockfd == fd){

            return i;

        }

    }


    return -1;
}



int get_chanctx_by_fid(int fid){


    for(int i = 0;i < MAX_CONN; i++){

        if(CHAN_CTX[i].frontid == fid){

            return i;
        }

    }


    return -1;

}


int set_sockctx_by_fd(int fd){


    int new_idx = calloc_sockctx();

    if(new_idx < 0){


        return -1;

    }

    SOCK_CTX[new_idx].sockfd = fd;

    return new_idx;
}




int get_sockctx_by_fd(int fd){


    for(int i = 0; i < MAX_CONN; i++){


        if(SOCK_CTX[i].sockfd == fd){

            return i;

        }


    }



    return -1;
}



int calloc_chanctx(){

    for(int i = 0; i < MAX_CONN; i++){


        if(CHAN_CTX[i].allocated == 0){

            CHAN_CTX[i].allocated = 1;

            CHAN_CTX[i].sockfd = 0;
            CHAN_CTX[i].frontid = -1;

            memset(CHAN_CTX[i].id, 0, MAX_ID_LEN * sizeof(char));

            CHAN_CTX[i].ssl = NULL;
            CHAN_CTX[i].ctx = NULL;

            CHAN_CTX[i].frontc = NULL;

            return i;

        }

    }


    return -1;
}


int free_chanctx(int idx){

    if(idx >= MAX_CONN){

        return -10;
    }

    if(CHAN_CTX[idx].allocated != 1){

        return -1;

    }

    if (CHAN_CTX[idx].ssl != NULL){

        SSL_shutdown(CHAN_CTX[idx].ssl);
        SSL_free(CHAN_CTX[idx].ssl);

    }

    if (CHAN_CTX[idx].ctx != NULL){

        SSL_CTX_free(CHAN_CTX[idx].ctx);

    }

    CHAN_CTX[idx].sockfd = 0;
    CHAN_CTX[idx].frontid = -1;


    memset(CHAN_CTX[idx].id, 0, MAX_ID_LEN * sizeof(char));

    CHAN_CTX[idx].ssl = NULL;
    CHAN_CTX[idx].ctx = NULL;
    CHAN_CTX[idx].frontc = NULL;

    CHAN_CTX[idx].allocated = 0;

    return 0;
}


int calloc_sockctx(){

    for(int i = 0; i < MAX_CONN; i++){

        if(SOCK_CTX[i].allocated == 0){

            SOCK_CTX[i].ctx = NULL;
            SOCK_CTX[i].ssl = NULL;
            SOCK_CTX[i].sockfd = 0;

            SOCK_CTX[i].allocated = 1;

            return i;

        }


    }


    return -1;
}



int free_sockctx(int idx, int memfree){


    if (idx >= MAX_CONN){
        return -10;
    }

    if(SOCK_CTX[idx].allocated != 1){
        return -1;
    }

    if(memfree == 1){

        SSL_shutdown(SOCK_CTX[idx].ssl);
        SSL_free(SOCK_CTX[idx].ssl);
        SSL_CTX_free(SOCK_CTX[idx].ctx);

    } else {

        SOCK_CTX[idx].ssl = NULL;
        SOCK_CTX[idx].ctx = NULL;

    }

    SOCK_CTX[idx].sockfd = 0;
    SOCK_CTX[idx].allocated = 0;


    return 0;
}



int chanctx_write_sock(char* id, int write_len, uint8_t* wbuff){


    int valwrite = 0;

    int chan_idx = 0;

    chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){

        printf("write: no such id: %s\n", id);

        return -1;
    }


    SSL* sslfd = CHAN_CTX[chan_idx].ssl;


    valwrite = SSL_write(sslfd, wbuff, write_len);


    if (valwrite <= 0){

        printf("write: client gone: %d\n", valwrite);

        return -2;

    }

    return valwrite;


}

int chanctx_read_sock(char* id, int read_len, uint8_t* rbuff){


    int valread = 0;

    int chan_idx = 0;

    int ms_until_deadline = 0;

    chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){

        printf("read: no such id: %s\n", id);

        return -1;
    }


    SSL* sslfd = CHAN_CTX[chan_idx].ssl;

    uint8_t* rbuff_tmp = (uint8_t*)malloc(read_len * sizeof(uint8_t));

    memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

    int valread_tmp = 0;

    struct timespec rnow;

    clock_gettime(CLOCK_MONOTONIC_RAW, &rnow);

    struct timespec rdeadline;

    while(valread < read_len){

        clock_gettime(CLOCK_MONOTONIC_RAW, &rdeadline);

        ms_until_deadline = ((rdeadline.tv_sec - rnow.tv_sec) * 1000 + (rdeadline.tv_nsec - rnow.tv_nsec) / 1000000);

        if(ms_until_deadline > HUB_TIMEOUT_MS){
            
            printf("time limit exceeded\n");

            free(rbuff_tmp);

            return -10;
        }

        valread_tmp = SSL_read(sslfd, (void*)rbuff_tmp, read_len);

        if(valread_tmp <= 0){
            
            if(errno == EAGAIN){

                memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

                valread_tmp = 0;

                continue;
            }

            printf("read: client gone: %d\n", valread);

            free(rbuff_tmp);

            return -2;
        }

        for(int i = 0 ; i < valread_tmp; i++){

            int idx = valread + i;

            rbuff[idx] = rbuff_tmp[i];

        }

        valread += valread_tmp;        

        memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

        valread_tmp = 0;

    }


    free(rbuff_tmp);

    return valread;



}


int sockctx_write(int fd, int write_len, uint8_t* wbuff){


    int valwrite = 0;

    int sock_idx = 0;

    sock_idx = get_sockctx_by_fd(fd);

    if(sock_idx < 0){

        printf("write: no such fd: %d\n", fd);

        return -1;
    }

    SSL* sslfd = SOCK_CTX[sock_idx].ssl;


    valwrite = SSL_write(sslfd, (void*)wbuff, write_len);

    if (valwrite <= 0){

        printf("write: client gone: %d\n", valwrite);

        return -2;

    }

    return valwrite;



}

int sockctx_read(int fd, int read_len, uint8_t* rbuff){



    int valread = 0;

    int sock_idx = 0;

    int ms_until_deadline = 0;

    sock_idx = get_sockctx_by_fd(fd);

    if(sock_idx < 0){

        printf("read: no such fd: %d\n", fd);

        return -1;
    }


    SSL* sslfd = SOCK_CTX[sock_idx].ssl;

    uint8_t* rbuff_tmp = (uint8_t*)malloc(read_len * sizeof(uint8_t));

    memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

    int valread_tmp = 0;

    struct timespec rnow;

    clock_gettime(CLOCK_MONOTONIC_RAW, &rnow);

    struct timespec rdeadline;

    while(valread < read_len){

        clock_gettime(CLOCK_MONOTONIC_RAW, &rdeadline);

        ms_until_deadline = ((rdeadline.tv_sec - rnow.tv_sec) * 1000 + (rdeadline.tv_nsec - rnow.tv_nsec) / 1000000);

        if(ms_until_deadline > HUB_TIMEOUT_MS){
            
            printf("time limit exceeded\n");

            free(rbuff_tmp);

            return -10;
        }

        valread_tmp = SSL_read(sslfd, (void*)rbuff_tmp, read_len);

        if(valread_tmp <= 0){

            if(errno == EAGAIN){

                memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

                valread_tmp = 0;

                continue;
            }

            printf("read: client gone: %d\n", valread);

            free(rbuff_tmp);

            return -2;
        }

        for(int i = 0 ; i < valread_tmp; i++){

            int idx = valread + i;

            rbuff[idx] = rbuff_tmp[i];

        }

        valread += valread_tmp;        

        memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

        valread_tmp = 0;

    }

    free(rbuff_tmp);

    return valread;


}



void hub_write_packet(struct HUB_PACKET* hp){

    int valwrite = 0;


    if(hp->ctx_type == CHAN_ISSOCK){
        

        valwrite = chanctx_write_sock(hp->id, HUB_HEADER_BYTELEN, hp->header);

        if(valwrite <= 0){

            printf("packet send header failed\n");

            hp->flag = valwrite;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len_new = 0;

        body_len_new = htonll(hp->body_len);

        
        memcpy(body_len_byte, &body_len_new, HUB_BODY_BYTELEN);


        valwrite = chanctx_write_sock(hp->id, HUB_BODY_BYTELEN, body_len_byte);

        if(valwrite <= 0){

            printf("packet send body len failed\n");

            hp->flag = valwrite;

            return;

        }

        valwrite = chanctx_write_sock(hp->id, hp->body_len, hp->wbuff);


        if(valwrite <= 0){

            printf("packet send buff failed\n");

            hp->flag = valwrite;

            return;

        } 

        hp->flag = valwrite;

        return;


    }


    if(hp->ctx_type == ISSOCK){

        valwrite = sockctx_write(hp->fd, HUB_HEADER_BYTELEN, hp->header);

        if(valwrite <= 0){

            printf("packet send header failed\n");

            hp->flag = valwrite;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len_new = 0;

        body_len_new = htonll(hp->body_len);

        memcpy(body_len_byte, &body_len_new, HUB_BODY_BYTELEN);

        valwrite = sockctx_write(hp->fd, HUB_BODY_BYTELEN, body_len_byte);

        if(valwrite <= 0){

            printf("packet send body len failed\n");

            hp->flag = valwrite;

            return;

        }

        valwrite = sockctx_write(hp->fd, hp->body_len, hp->wbuff);

        if(valwrite <= 0){

            printf("packet send buff failed\n");

            hp->flag = valwrite;

            return;

        } 

        hp->flag = valwrite;

        return;


    }


    printf("invalid ctx write packet type: %d\n", hp->ctx_type);

    hp->flag = valwrite;


    return;

}


void hub_read_packet(struct HUB_PACKET* hp){

    int valread = 0;

    if(hp->ctx_type == CHAN_ISSOCK){

        valread = chanctx_read_sock(hp->id, HUB_HEADER_BYTELEN, hp->header);

        if(valread <= 0){

            printf("packet recv header failed\n");

            hp->flag = valread;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len = 0;

        valread = chanctx_read_sock(hp->id, HUB_BODY_BYTELEN, body_len_byte);


        if(valread <= 0){

            printf("packet recv body len failed\n");

            hp->flag = valread;

            return;

        }

        memcpy(&body_len, body_len_byte, HUB_BODY_BYTELEN);

        body_len = ntohll(body_len);

        if(body_len > HUB_BODY_BYTEMAX){

            printf("packet body len too long \n");

            hp->flag = -10;

            return;
        }

        hp->body_len = body_len;

        hp->rbuff = (uint8_t*)malloc(hp->body_len * sizeof(uint8_t));

        memset(hp->rbuff, 0, hp->body_len * sizeof(uint8_t));

        valread = chanctx_read_sock(hp->id, hp->body_len, hp->rbuff);

        if(valread <= 0){

            printf("packet recv body failed\n");

            free(hp->rbuff);

            hp->flag = valread;

            return;

        }


        hp->flag = valread;

        return;


    }




    if(hp->ctx_type == ISSOCK){

        valread = sockctx_read(hp->fd, HUB_HEADER_BYTELEN, hp->header);

        if(valread <= 0){

            printf("packet recv header failed\n");

            hp->flag = valread;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len = 0;

        valread = sockctx_read(hp->fd, HUB_BODY_BYTELEN, body_len_byte);

        if(valread <= 0){

            printf("packet recv body len failed\n");

            hp->flag = valread;

            return;

        }

        memcpy(&body_len, body_len_byte, HUB_BODY_BYTELEN);

        body_len = ntohll(body_len);

        if(body_len > HUB_BODY_BYTEMAX){

            printf("packet body len too long \n");

            hp->flag = -10;

            return;
        }

        hp->body_len = body_len;

        hp->rbuff = (uint8_t*)malloc(hp->body_len * sizeof(uint8_t));

        memset(hp->rbuff, 0, hp->body_len * sizeof(uint8_t));

        valread = sockctx_read(hp->fd, hp->body_len, hp->rbuff);

        if(valread <= 0){

            printf("packet recv body failed\n");

            free(hp->rbuff);

            hp->flag = valread;

            return;

        }

        hp->flag = valread;

        return;


    }

    printf("invalid ctx read packet type: %d\n", hp->ctx_type);

    hp->flag = valread;

    return;
}





