#include "frankhub/core.h"


struct CHANNEL_CONTEXT CHAN_CTX[MAX_CONN];

struct SOCK_CONTEXT SOCK_CTX[MAX_CONN];

struct FRONT_CONTEXT FRONT_CTX[MAX_CONN];



int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path){


    int valread = 0 ;

    int c;

    FILE* fp;

    fp = fopen(file_path, "r");

    while(1) {

        c = fgetc(fp);
        if( feof(fp) ) {
            break;
        }


        buff[valread] = c;

        valread += 1;

        if(valread > max_buff_len){

            return -10;
        }
    
   }

    return valread;
}

int gen_random_bytestream(uint8_t* bytes, size_t num_bytes){
  

    if(num_bytes > MAX_PW_LEN){

    return -1;

    }


    size_t i;

    for (i = 0; i < num_bytes; i++){

    bytes[i] = rand();

    }

    return 0;

}

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray){

    int hexlen = 2;

    int outstrlen = hexlen * arrlen + 1;

    if (outstrlen > MAX_PW_LEN){

        return -1;
    }

    memset(hexarray, 0, outstrlen * sizeof(char));

    unsigned char* ptr = hexarray;

    for(int i = 0 ; i < arrlen; i++){

        sprintf(ptr + 2 * i, "%02X", bytearray[i]);
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


int idpw_verify(char* idpw){

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

    int chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){

        return -2;

    }

    if(strncmp(CHAN_CTX[chan_idx].pw, pw, MAX_PW_LEN) != 0){

        return -3;

    }

    int verified_idx = chan_idx;


    return verified_idx;

}

int update_chanctx_from_userinfo(char* id, char* pw){

    int ret_idx = -1;


    int chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){


        ret_idx = calloc_chanctx();

        if(ret_idx < 0){

            printf("id %s failed to calloc chanctx: max conn\n", id);

            return -2;

        }


        strcpy(CHAN_CTX[ret_idx].id, id);
        strcpy(CHAN_CTX[ret_idx].pw, pw);

        chan_idx = ret_idx;

        printf("id %s added to chanctx from userinfo\n", id);


    } else {


        strcpy(CHAN_CTX[chan_idx].pw, pw);


        printf("id %s modified for existing chanctx from userinfo\n", id);


    }



    ret_idx = chan_idx;


    return ret_idx;

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

int set_chanctx_from_frontctx(int fd, char* id){


    int ret_idx = -1;


    int front_idx = get_frontctx_by_fd(fd);

    if(front_idx < 0){

        printf("no front fd associated with: %d\n",fd);

        return -1;
    }

    int chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){

        printf("no chanctx associated with: %s\n", id);

        return -2;

    } else {

        CHAN_CTX[chan_idx].frontfd = FRONT_CTX[front_idx].frontfd;

        free_frontctx(front_idx);

        printf("id %s modified for existing chanctx from front\n", id);


    }

    ret_idx = chan_idx;

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



int get_chanctx_by_fd(int fd, int type){


    if(type == ISSOCK){

        for(int i = 0; i < MAX_CONN; i++){

            if(CHAN_CTX[i].sockfd == fd){

                return i;

            }

        }


    }

    if(type == ISFRONT){

        for(int i = 0; i < MAX_CONN; i++){

            if(CHAN_CTX[i].frontfd == fd){

                return i;

            }

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

int set_frontctx_by_fd(int fd){

    int new_idx = calloc_frontctx();

    if(new_idx < 0){

        return -1;
    }
    
    FRONT_CTX[new_idx].frontfd = fd;

    return new_idx;
}


int get_frontctx_by_fd(int fd){


    for(int i = 0; i < MAX_CONN; i++){


        if(FRONT_CTX[i].frontfd == fd){

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
            CHAN_CTX[i].frontfd = 0;

            memset(CHAN_CTX[i].id, 0, MAX_ID_LEN * sizeof(char));
            memset(CHAN_CTX[i].pw, 0, MAX_PW_LEN * sizeof(char));        

            CHAN_CTX[i].ssl = NULL;
            CHAN_CTX[i].ctx = NULL;

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
    CHAN_CTX[idx].frontfd = 0;


    memset(CHAN_CTX[idx].id, 0, MAX_ID_LEN * sizeof(char));
    memset(CHAN_CTX[idx].pw, 0, MAX_PW_LEN * sizeof(char));   

    CHAN_CTX[idx].ssl = NULL;
    CHAN_CTX[idx].ctx = NULL;

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


int calloc_frontctx(){

    for(int i = 0; i < MAX_CONN; i ++){

        if(FRONT_CTX[i].allocated == 0){


            FRONT_CTX[i].allocated = 1;


            return i;
        }

    }


    return -1;
}


int free_frontctx(int idx){

    if (idx >= MAX_CONN){
        return -10;
    }

    if(FRONT_CTX[idx].allocated != 1){
        return -1;
    }

    FRONT_CTX[idx].frontfd = 0;
    FRONT_CTX[idx].allocated = 0;

    return 0;
}


int chanctx_write(int type, char* id, int write_len, uint8_t* wbuff){

    if(type == ISSOCK){

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


    if(type == ISFRONT){


        int valwrite = 0;

        int chan_idx = 0;

        chan_idx = get_chanctx_by_id(id);

        if(chan_idx < 0){

            printf("write: no such id: %s\n", id);

            return -1;
        }

        int ffd = CHAN_CTX[chan_idx].frontfd;

        valwrite = write(ffd, (void*)wbuff, write_len);

        if (valwrite <= 0){

            printf("write: client gone: %d\n", valwrite);

            return -2;

        }

        return valwrite;



    }

    printf("invalid chanctx write type: %d\n", type);

    return -100;


}

int chanctx_read(int type, char* id, int read_len, uint8_t* rbuff){

    if(type == ISSOCK){

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

    if(type == ISFRONT){

        int valread = 0;

        int chan_idx = 0;

        int ms_until_deadline = 0;

        chan_idx = get_chanctx_by_id(id);

        if(chan_idx < 0){

            printf("read: no such id: %s\n", id);

            return -1;
        }


        int ffd = CHAN_CTX[chan_idx].frontfd;

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

            valread_tmp = read(ffd, (void*)rbuff_tmp, read_len);

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

    printf("invalid chanctx read type: %d\n", type);

    return -100;


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



int frontctx_write(int fd, int write_len, uint8_t* wbuff){


    int valwrite = 0;

    int front_idx = 0;

    front_idx = get_frontctx_by_fd(fd);

    if(front_idx < 0){

        printf("write: no such fd: %d\n", fd);

        return -1;
    }

    int ffd = FRONT_CTX[front_idx].frontfd;

    valwrite = write(ffd, (void*)wbuff, write_len);

    if (valwrite <= 0){

        printf("write: client gone: %d\n", valwrite);

        return -2;

    }

    return valwrite;



}

int frontctx_read(int fd, int read_len, uint8_t* rbuff){



    int valread = 0;

    int front_idx = 0;

    int ms_until_deadline = 0;

    front_idx = get_frontctx_by_fd(fd);

    if(front_idx < 0){

        printf("read: no such fd: %d\n", fd);

        return -1;
    }


    int ffd = FRONT_CTX[front_idx].frontfd;

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

        valread_tmp = read(ffd, (void*)rbuff_tmp, read_len);

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


void ctx_write_packet(struct HUB_PACKET* hp){

    int valwrite = 0;


    if(hp->ctx_type == CHAN_ISSOCK){
        

        valwrite = chanctx_write(ISSOCK, hp->id, HUB_HEADER_BYTELEN, hp->header);

        if(valwrite <= 0){

            printf("packet send header failed\n");

            hp->flag = valwrite;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len_new = 0;

        body_len_new = htonll(hp->body_len);

        
        memcpy(body_len_byte, &body_len_new, HUB_BODY_BYTELEN);


        valwrite = chanctx_write(ISSOCK, hp->id, HUB_BODY_BYTELEN, body_len_byte);

        if(valwrite <= 0){

            printf("packet send body len failed\n");

            hp->flag = valwrite;

            return;

        }

        valwrite = chanctx_write(ISSOCK, hp->id, hp->body_len, hp->wbuff);


        if(valwrite <= 0){

            printf("packet send buff failed\n");

            hp->flag = valwrite;

            return;

        } 

        hp->flag = valwrite;

        return;


    }


    if(hp->ctx_type == CHAN_ISFRONT){


        valwrite = chanctx_write(ISFRONT, hp->id, HUB_HEADER_BYTELEN, hp->header);

        if(valwrite <= 0){

            printf("packet send header failed\n");

            hp->flag = valwrite;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len_new = 0;

        body_len_new = htonll(hp->body_len);

        memcpy(body_len_byte, &body_len_new, HUB_BODY_BYTELEN);

        valwrite = chanctx_write(ISFRONT, hp->id, HUB_BODY_BYTELEN, body_len_byte);

        if(valwrite <= 0){

            printf("packet send body len failed\n");

            hp->flag = valwrite;

            return;

        }

        valwrite = chanctx_write(ISFRONT, hp->id, hp->body_len, hp->wbuff);


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


    if(hp->ctx_type == ISFRONT){

        valwrite = frontctx_write(hp->fd, HUB_HEADER_BYTELEN, hp->header);

        if(valwrite <= 0){

            printf("packet send header failed\n");

            hp->flag = valwrite;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len_new = 0;

        body_len_new = htonll(hp->body_len);

        memcpy(body_len_byte, &body_len_new, HUB_BODY_BYTELEN);

        valwrite = frontctx_write(hp->fd, HUB_BODY_BYTELEN, body_len_byte);

        if(valwrite <= 0){

            printf("packet send body len failed\n");

            hp->flag = valwrite;

            return;

        }

        valwrite = frontctx_write(hp->fd, hp->body_len, hp->wbuff);

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


void ctx_read_packet(struct HUB_PACKET* hp){

    int valread = 0;

    if(hp->ctx_type == CHAN_ISSOCK){

        valread = chanctx_read(ISSOCK, hp->id, HUB_HEADER_BYTELEN, hp->header);

        if(valread <= 0){

            printf("packet recv header failed\n");

            hp->flag = valread;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len = 0;

        valread = chanctx_read(ISSOCK, hp->id, HUB_BODY_BYTELEN, body_len_byte);


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

        valread = chanctx_read(ISSOCK, hp->id, hp->body_len, hp->rbuff);

        if(valread <= 0){

            printf("packet recv body failed\n");

            free(hp->rbuff);

            hp->flag = valread;

            return;

        }


        hp->flag = valread;

        return;


    }


    if(hp->ctx_type == CHAN_ISFRONT){

        valread = chanctx_read(ISFRONT, hp->id, HUB_HEADER_BYTELEN, hp->header);

        if(valread <= 0){

            printf("packet recv header failed\n");

            hp->flag = valread;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len = 0;

        valread = chanctx_read(ISFRONT, hp->id, HUB_BODY_BYTELEN, body_len_byte);

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

        valread = chanctx_read(ISFRONT, hp->id, hp->body_len, hp->rbuff);

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


    if(hp->ctx_type == ISFRONT){


        valread = frontctx_read(hp->fd, HUB_HEADER_BYTELEN, hp->header);

        if(valread <= 0){

            printf("packet recv header failed\n");

            hp->flag = valread;

            return;

        }

        uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

        uint64_t body_len = 0;

        valread = frontctx_read(hp->fd, HUB_BODY_BYTELEN, body_len_byte);

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

        valread = frontctx_read(hp->fd, hp->body_len, hp->rbuff);

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





