#include "frankhub/core.h"


struct CHANNEL_CONTEXT CHAN_CTX[MAX_CONN];

struct SOCK_CONTEXT SOCK_CTX[MAX_CONN];

struct FRONT_CONTEXT FRONT_CTX[MAX_CONN];



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

int update_chanctx_from_userinfo(char* id, char* pw){

    int ret_idx = -1;


    int chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){


        ret_idx = calloc_chanctx();

        if(ret_idx < 0){

            printf("id %s failed to calloc chanctx: max conn\n", id);

            return -2;

        }


        strcpy(CHAN_CTX[chan_idx].id, id);
        strcpy(CHAN_CTX[chan_idx].pw, pw);


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

        CHAN_CTX[chan_idx].sockfd = SOCK_CTX[sock_idx].sockfd;
        CHAN_CTX[chan_idx].ssl = SOCK_CTX[sock_idx].ssl;
        CHAN_CTX[chan_idx].ctx = SOCK_CTX[sock_idx].ctx;

        free_sockctx(sock_idx, 0);

        strcpy(CHAN_CTX[chan_idx].id, id);

        printf("id %s added to chanctx from sock\n", id);

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


int chanctx_write(char* id, int write_len, uint8_t* wbuff){


    int valwrite = 0;

    int chan_idx = 0;

    chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){

        printf("write: no such id: %s\n", id);

        return -1;
    }

    SSL* sslfd = CHAN_CTX[chan_idx].ssl;


    valwrite = SSL_write(sslfd, (void*)wbuff, write_len);

    if (valwrite <= 0){

        printf("write: client gone: %d\n", valwrite);

        return -2;

    }

    return valwrite;



}

int chanctx_read(char* id, int read_len, uint8_t* rbuff){



    int valread = 0;

    int chan_idx = 0;

    chan_idx = get_chanctx_by_id(id);

    if(chan_idx < 0){

        printf("read: no such id: %s\n", id);

        return -1;
    }


    SSL* sslfd = CHAN_CTX[chan_idx].ssl;

    uint8_t* rbuff_tmp = (uint8_t*)malloc(read_len * sizeof(uint8_t));

    memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

    int valread_tmp = 0;

    while(valread < read_len){

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

    sock_idx = get_sockctx_by_fd(fd);

    if(sock_idx < 0){

        printf("read: no such fd: %d\n", fd);

        return -1;
    }


    SSL* sslfd = SOCK_CTX[sock_idx].ssl;

    uint8_t* rbuff_tmp = (uint8_t*)malloc(read_len * sizeof(uint8_t));

    memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

    int valread_tmp = 0;

    while(valread < read_len){

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




