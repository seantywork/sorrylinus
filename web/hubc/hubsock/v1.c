#include   "frankhub/sock/v1.h"

int SOCK_FD;
int SOCK_SERVLEN;
int SOCK_EPLFD;
struct epoll_event SOCK_EVENT;
struct epoll_event *SOCK_EVENTARRAY;



void sock_listen_and_serve(){

    int result = 0;

    SSL_library_init();

    result = read_file_to_buffer(CA_CERT, MAX_PW_LEN, HUB_CA_CERT);

    if(result < 0){

        printf("failed to read ca cert\n");

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
        printf("socket creation failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else {
        printf("socket successfully created\n"); 
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
        printf("socket bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    
    if(make_socket_non_blocking(SOCK_FD) < 0){
        printf("non-blocking failed\n");
        exit(EXIT_FAILURE);
    }
    

    if ((listen(SOCK_FD, MAX_CONN)) != 0) { 
        printf("listen failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else{
        SOCK_SERVLEN = sizeof(SERVADDR); 
    }


    SOCK_EPLFD = epoll_create1(0);

    if(SOCK_EPLFD == -1){
        printf("epoll creation failed \n");
        exit(EXIT_FAILURE);
    }

    SOCK_EVENT.data.fd = SOCK_FD;
    SOCK_EVENT.events = EPOLLIN | EPOLLET;
    
    if (epoll_ctl(SOCK_EPLFD, EPOLL_CTL_ADD, SOCK_FD, &SOCK_EVENT) < 0){
        printf("epoll add failed\n");
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

                printf("epoll wait error \n");
                close(SOCK_EVENTARRAY[i].data.fd);
                continue;

            } else if (SOCK_FD == SOCK_EVENTARRAY[i].data.fd){

                sock_handle_conn();

                printf("new conn successfully handled\n");

            } else{

                sock_handle_client(SOCK_EVENTARRAY[i].data.fd);

                printf("socket data successfully handled\n");


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
                printf("all incoming sock connections handled\n");
                break;

            } else{

                printf("error handling incoming sock connection\n");
                break;
            }
        }

        ctx = create_context();

        configure_context(ctx);

        ssl = SSL_new(ctx);

        SSL_set_fd(ssl, infd);
    
        if((ssl_accept_ret = SSL_accept(ssl)) < 1){

            int sslerr =  SSL_get_error(ssl, 0);

            printf("error handling tls handshake\n");

            if (ssl_accept_ret <=0 && (sslerr == SSL_ERROR_WANT_READ)) {

                perror ("Need to wait until socket is readable.");

            } else if (ssl_accept_ret <=0 && (sslerr == SSL_ERROR_WANT_WRITE)) {

                perror ("Need to wait until socket is writable.");

            } else {
                perror ("Need to wait until socket is ready.");
            }

            shutdown (infd, 2);
            SSL_free (ssl);
            SSL_CTX_free(ctx);

            continue;

        } 


        if(make_socket_non_blocking(infd) < 0){
            printf("failed new conn non block\n");
            exit(EXIT_FAILURE);
        }

        int sock_idx = set_sockctx_by_fd(infd);



        if(sock_idx < 0){

            printf("failed new conn sockctx\n");
            exit(EXIT_FAILURE);

        }

        SOCK_CTX[sock_idx].ctx = ctx;
        SOCK_CTX[sock_idx].ssl = ssl;


        SOCK_EVENT.data.fd = infd;
        SOCK_EVENT.events = EPOLLIN | EPOLLET;

        if (epoll_ctl(SOCK_EPLFD, EPOLL_CTL_ADD, infd, &SOCK_EVENT) < 0){

            printf("handle epoll add failed\n");
            exit(EXIT_FAILURE);

        }  else {

            printf("handle epoll add success\n");

        }



    }



}



void sock_handle_client(int cfd){


    int chan_idx = get_chanctx_by_fd(cfd, ISSOCK);

    if(chan_idx < 0){
        
        sock_authenticate(cfd);

        return;
    }

    sock_communicate(chan_idx);

    return;

}



void sock_authenticate(int cfd){

    int valread;
    int valwrite;

    struct HUB_PACKET hp;


    uint8_t id[MAX_ID_LEN] = {0};

    int sock_idx = get_sockctx_by_fd(cfd);

    printf("not registered to chan ctx, auth\n");

    if(sock_idx < 0){

        printf("failed to get sock idx\n");

        return;
    }


    hp.ctx_type = ISSOCK;
    hp.fd = SOCK_CTX[sock_idx].sockfd;
    
    ctx_read_packet(&hp);

    if(hp.flag <= 0){


        printf("failed to read sock\n");

        free_sockctx(sock_idx, 1);

        return;

    }
    


    int verified = sig_verify(hp.rbuff, CA_CERT);

    if(verified < 1){

        printf("invalid signature\n");

        free_sockctx(sock_idx, 1);

        free(hp.rbuff);

        return;

    }

    

    int ret_cn = extract_common_name(id, hp.rbuff);

    if(ret_cn != 1){

        printf("invalid id\n");

        free_sockctx(sock_idx, 1);

        free(hp.rbuff);

        return;


    }

    printf("id: %s\n", id);

    free(hp.rbuff);

    int chan_idx = update_chanctx_from_sockctx(cfd, id);

    if (chan_idx < 0){

        printf("failed to update chanctx\n");

        free_sockctx(sock_idx, 1);

        return;

    }


    uint64_t body_len = (uint64_t)(strlen("SUCCESS") + 1);

    memset(hp.header, 0, HUB_HEADER_BYTELEN);

    memset(hp.wbuff, 0, MAX_BUFF);

    hp.ctx_type = CHAN_ISSOCK;

    strcpy(hp.header, HUB_HEADER_AUTHSOCK);

    hp.body_len = body_len;

    strcat(hp.wbuff,"SUCCESS");

    strcpy(hp.id, id);

    printf("writing...\n");
    
    ctx_write_packet(&hp);

    if(hp.flag <= 0){

        printf("failed to send\n");

        return;

    }

    printf("sent\n");

    return;


}



void sock_communicate(int chan_idx){

    printf("incoming sock communication to front\n");

    int frontfd = CHAN_CTX[chan_idx].frontfd;

    if(frontfd == 0){

        printf("no front exists for communication\n");

        return;

    }
    
    printf("front exists\n");

    struct HUB_PACKET hp;

    hp.ctx_type = CHAN_ISSOCK;

    strcpy(hp.id, CHAN_CTX[chan_idx].id);

    ctx_read_packet(&hp);

    if(hp.flag <= 0){

        printf("failed to communicate sock read\n");

        return;

    }

    memset(hp.header, 0, HUB_HEADER_BYTELEN);

    memset(hp.wbuff, 0, MAX_BUFF);

    hp.ctx_type = CHAN_ISFRONT;

    strcpy(hp.header, HUB_HEADER_RECVFRONT);

    strncpy(hp.wbuff, hp.rbuff, hp.body_len);

    hp.flag = 0;

    free(hp.rbuff);

    ctx_write_packet(&hp);

    if(hp.flag <= 0){

        printf("failed to send to front\n");

        return;
    } 

    printf("sent to front\n");

    return;
}