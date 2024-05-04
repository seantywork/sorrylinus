#include   "frankhub/sock/v1.h"

int SOCK_FD;
int SOCK_SERVLEN;
int SOCK_EPLFD;
struct epoll_event SOCK_EVENT;
struct epoll_event *SOCK_EVENTARRAY;



void sock_listen_and_serve(){


    struct sockaddr_in SERVADDR;

    SSL_library_init();

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

    int done = 0;

    
    int valread;
    int valwrite;
    uint8_t rbuff[MAX_BUFF] = {0}; 
    uint8_t wbuff[MAX_BUFF] = {0}; 
    struct sockaddr_in peeraddr;
    socklen_t peerlen;

    peerlen = sizeof(peeraddr);

    int chan_idx = get_chanctx_by_fd(cfd, ISSOCK);

    if(chan_idx < 0){


        printf("not registered to chan ctx, auth\n");

        valread = sockctx_read(cfd, MAX_BUFF, rbuff);

        printf("valread: %d\n", valread);

        printf("rbuff: %s\n", rbuff);

        strcat(wbuff,"SSL SERVER RESP: HELLO");

        valwrite = sockctx_write(cfd, MAX_BUFF, wbuff);

        printf("valwrite: %d\n", valwrite);

        return;
    }




    

}
