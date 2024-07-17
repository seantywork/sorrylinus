#include   "frankhub/v2/sock/core.h"
#include   "frankhub/v2/utils.h"

int SOCK_FD;
int SOCK_SERVLEN;
int SOCK_EPLFD;
struct epoll_event SOCK_EVENT;
struct epoll_event *SOCK_EVENTARRAY;

char CA_CERT[MAX_PW_LEN] = {0};

void sock_listen_and_serve(void* varg){

    int result = 0;

    SSL_library_init();

    result = read_file_to_buffer(CA_CERT, MAX_PW_LEN, HUB_CA_CERT);

    if(result < 0){

        fmt_logln(LOGFP, "failed to read ca cert");

        return;

    }



    struct sockaddr_in SERVADDR;

    for(int i = 0 ; i < MAX_CONN;i ++){

        CHAN_CTX[i].ssl = NULL;
        CHAN_CTX[i].ctx = NULL;


    }


    //signal(SIGPIPE, SIG_IGN);

    SOCK_FD = socket(AF_INET, SOCK_STREAM, 0); 

    if (SOCK_FD == -1) { 

        fmt_logln(LOGFP, "socket creation failed");

        exit(EXIT_FAILURE); 
    } 
    else {

        fmt_logln(LOGFP, "socket successfully created");
    }
    /*
    if( setsockopt(SOCKFD, SOL_SOCKET, SO_REUSEADDR, (char *)&OPT,  
          sizeof(OPT)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    } 

    */  
     

    bzero(&SERVADDR, sizeof(SERVADDR)); 
   
    SERVADDR.sin_family = AF_INET; 
    SERVADDR.sin_addr.s_addr = htonl(INADDR_ANY); 
    SERVADDR.sin_port = htons(PORT_SOCK); 
   
    if ((bind(SOCK_FD, (struct sockaddr*)&SERVADDR, sizeof(SERVADDR))) != 0) { 
        

        fmt_logln(LOGFP, "socket bind failed");

        exit(EXIT_FAILURE); 
    } 
    
    if(make_socket_non_blocking(SOCK_FD) < 0){

        fmt_logln(LOGFP, "non-blocking failed");

        exit(EXIT_FAILURE);
    }
    

    if ((listen(SOCK_FD, MAX_CONN)) != 0) { 

        fmt_logln(LOGFP, "listen failed");

        exit(EXIT_FAILURE); 
    } 
    else{
        SOCK_SERVLEN = sizeof(SERVADDR); 
    }


    SOCK_EPLFD = epoll_create1(0);

    if(SOCK_EPLFD == -1){

        fmt_logln(LOGFP, "epoll creation failed");

        exit(EXIT_FAILURE);
    }

    SOCK_EVENT.data.fd = SOCK_FD;
    SOCK_EVENT.events = EPOLLIN | EPOLLET;
    
    if (epoll_ctl(SOCK_EPLFD, EPOLL_CTL_ADD, SOCK_FD, &SOCK_EVENT) < 0){

        fmt_logln(LOGFP,"epoll add failed");

        exit(EXIT_FAILURE);
    }    

    SOCK_EVENTARRAY = calloc(MAX_CONN, sizeof(SOCK_EVENT));


    while(TRUE){

        int n, i ;

        n = epoll_wait(SOCK_EPLFD, SOCK_EVENTARRAY, MAX_CONN, -1);

        for (i = 0 ; i < n; i ++){

            if (
                (SOCK_EVENTARRAY[i].events & EPOLLERR) ||
                (SOCK_EVENTARRAY[i].events & EPOLLHUP) ||
                (!(SOCK_EVENTARRAY[i].events & EPOLLIN))
            ){

                fmt_logln(LOGFP, "epoll wait error");

                close(SOCK_EVENTARRAY[i].data.fd);
                
                continue;

            } else if (SOCK_FD == SOCK_EVENTARRAY[i].data.fd){

                sock_handle_conn();

                fmt_logln(LOGFP, "new sock conn successfully handled");

            } else{

                sock_handle_client(SOCK_EVENTARRAY[i].data.fd);

                fmt_logln(LOGFP, "socket data successfully handled");


            }

        }


    }


    free(SOCK_EVENTARRAY);

    close(SOCK_FD);

    close(SOCK_EPLFD);


}





void sock_handle_conn(){


    while(TRUE){

        struct sockaddr in_addr;
        socklen_t in_len;
        int infd;
        SSL *ssl;
        SSL_CTX *ctx;

        int ssl_accept_ret;

        in_len = sizeof(in_addr);
   
        infd = accept(SOCK_FD, &in_addr, &in_len);

        if(infd == -1){

            if(
                (errno == EAGAIN) ||
                (errno == EWOULDBLOCK)
            ){

                fmt_logln(LOGFP, "all incoming sock connections handled");

                break;

            } else{
 
                fmt_logln(LOGFP, "error handling incoming sock connection");

                break;
            }
        }

        ctx = create_context();

        configure_context(ctx);

        ssl = SSL_new(ctx);

        SSL_set_fd(ssl, infd);
    
        if((ssl_accept_ret = SSL_accept(ssl)) < 1){

            int sslerr =  SSL_get_error(ssl, 0);

            fmt_logln(LOGFP, "error handling tls handshake");

            if (ssl_accept_ret <=0 && (sslerr == SSL_ERROR_WANT_READ)) {

                perror ("Need to wait until socket is readable.");

                fmt_logln(LOGFP, "ssl: %s", "Need to wait until socket is readable");

            } else if (ssl_accept_ret <=0 && (sslerr == SSL_ERROR_WANT_WRITE)) {

                perror ("Need to wait until socket is writable.");

                fmt_logln(LOGFP, "ssl: %s", "Need to wait until socket is writable");


            } else {
                perror ("Need to wait until socket is ready.");

                fmt_logln(LOGFP, "ssl: %s", "Need to wait until socket is ready");

            }

            shutdown (infd, 2);
            SSL_free (ssl);
            SSL_CTX_free(ctx);

            continue;

        } 


        if(make_socket_non_blocking(infd) < 0){

            fmt_logln(LOGFP, "failed new conn non block");

            exit(EXIT_FAILURE);
        }

        int sock_idx = set_sockctx_by_fd(infd);



        if(sock_idx < 0){

            fmt_logln(LOGFP, "failed new conn sockctx");

            exit(EXIT_FAILURE);

        }

        SOCK_CTX[sock_idx].ctx = ctx;
        SOCK_CTX[sock_idx].ssl = ssl;


        SOCK_EVENT.data.fd = infd;
        SOCK_EVENT.events = EPOLLIN | EPOLLET;

        if (epoll_ctl(SOCK_EPLFD, EPOLL_CTL_ADD, infd, &SOCK_EVENT) < 0){

            fmt_logln(LOGFP,"handle epoll add failed");  


            exit(EXIT_FAILURE);

        }  else {

            fmt_logln(LOGFP,"handle epoll add success"); 

        }



    }



}



void sock_handle_client(int cfd){


    pthread_mutex_lock(&G_MTX);

    int chan_idx = get_chanctx_by_fd(cfd);


    if(chan_idx < 0){
        
        sock_authenticate(cfd);

        pthread_mutex_unlock(&G_MTX);

        return;
    }

    sock_communicate(chan_idx);

    pthread_mutex_unlock(&G_MTX);

    return;

}



void sock_authenticate(int cfd){

    int valread;
    int valwrite;

    struct HUB_PACKET hp;


    uint8_t id[MAX_ID_LEN] = {0};

    int sock_idx = get_sockctx_by_fd(cfd);

    fmt_logln(LOGFP,"not registered to chan ctx, auth"); 

    if(sock_idx < 0){

        fmt_logln(LOGFP,"failed to get sock idx"); 

        return;
    }


    hp.ctx_type = ISSOCK;
    hp.fd = SOCK_CTX[sock_idx].sockfd;
    
    hub_read_packet(&hp);

    if(hp.flag <= 0){


        fmt_logln(LOGFP,"failed to read sock"); 

        free_sockctx(sock_idx, 1);

        return;

    }
    


    int verified = sig_verify(hp.rbuff, CA_CERT);

    if(verified < 1){

        fmt_logln(LOGFP,"invalid signature"); 

        free_sockctx(sock_idx, 1);

        free(hp.rbuff);

        return;

    }

    

    int ret_cn = extract_common_name(id, hp.rbuff);

    if(ret_cn != 1){

        fmt_logln(LOGFP,"invalid id"); 

        free_sockctx(sock_idx, 1);

        free(hp.rbuff);

        return;


    }

    fmt_logln(LOGFP, "id: %s", id);

    free(hp.rbuff);

    int chan_idx = update_chanctx_from_sockctx(cfd, id);

    if (chan_idx < 0){

        fmt_logln(LOGFP, "failed to update chanctx");

        free_sockctx(sock_idx, 1);

        return;

    }


    uint64_t body_len = strlen("success") + 1;

    memset(hp.header, 0, HUB_HEADER_BYTELEN);

    memset(hp.wbuff, 0, MAX_BUFF);

    hp.ctx_type = CHAN_ISSOCK;

    strcpy(hp.header, HUB_HEADER_AUTHSOCK);

    hp.body_len = body_len;

    strcat(hp.wbuff,"success");

    strcpy(hp.id, id);

    fmt_logln(LOGFP, "writing auth result..");
    
    hub_write_packet(&hp);

    if(hp.flag <= 0){

        fmt_logln(LOGFP, "failed to send");

        return;

    }

    fmt_logln(LOGFP, "sent");

    return;


}



void sock_communicate(int chan_idx){

    char ws_buff[MAX_WS_BUFF] = {0};

    uint8_t recv_buff[WS_MAX_COMMAND_RECV_LEN] = {0};

    cJSON* response = cJSON_CreateObject();

    fmt_logln(LOGFP, "incoming sock communication to front");

    struct mg_connection* frontc = CHAN_CTX[chan_idx].frontc;

    if(frontc == NULL){

        fmt_logln(LOGFP, "no front exists for communication");

        return;

    }
    
    fmt_logln(LOGFP, "front exists");

    struct HUB_PACKET hp;

    hp.ctx_type = CHAN_ISSOCK;

    strcpy(hp.id, CHAN_CTX[chan_idx].id);

    hub_read_packet(&hp);

    if(hp.flag <= 0){

        fmt_logln(LOGFP, "failed to communicate sock read");

        return;

    }

    strncpy(recv_buff, hp.rbuff, hp.body_len);

    free(hp.rbuff);


    cJSON_AddItemToObject(response, "status", cJSON_CreateString("success"));
    cJSON_AddItemToObject(response, "data", cJSON_CreateString(recv_buff));
    
    strcpy(ws_buff, cJSON_Print(response));

    int datalen = strlen(ws_buff);

    mg_ws_send(frontc, ws_buff, datalen, WEBSOCKET_OP_TEXT);

    fmt_logln(LOGFP, "send to front");

    return;
}