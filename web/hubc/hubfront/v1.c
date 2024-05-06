#include   "frankhub/front/v1.h"


int FRONT_FD;
int FRONT_SERVLEN;
int FRONT_EPLFD;
struct epoll_event FRONT_EVENT;
struct epoll_event *FRONT_EVENTARRAY;

void front_listen_and_serve(){


    struct sockaddr_in SERVADDR;



    FRONT_FD = socket(AF_INET, SOCK_STREAM, 0); 

    if (FRONT_FD == -1) { 
        printf("socket creation failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else {
        printf("socket successfully created\n"); 
    }

    bzero(&SERVADDR, sizeof(SERVADDR)); 
   
    SERVADDR.sin_family = AF_INET; 
    SERVADDR.sin_addr.s_addr = htonl(INADDR_ANY); 
    SERVADDR.sin_port = htons(PORT_FRONT); 

    if ((bind(FRONT_FD, (struct sockaddr*)&SERVADDR, sizeof(SERVADDR))) != 0) { 
        printf("socket bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    
    if(make_socket_non_blocking(FRONT_FD) < 0){
        printf("non-blocking failed\n");
        exit(EXIT_FAILURE);
    }
    

    if ((listen(FRONT_FD, MAX_CONN)) != 0) { 
        printf("listen failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else{
        FRONT_SERVLEN = sizeof(SERVADDR); 
    }
   


    FRONT_EPLFD = epoll_create1(0);

    if(FRONT_EPLFD == -1){
        printf("epoll creation failed \n");
        exit(EXIT_FAILURE);
    }

    FRONT_EVENT.data.fd = FRONT_FD;
    FRONT_EVENT.events = EPOLLIN | EPOLLET;
    
    if (epoll_ctl(FRONT_EPLFD, EPOLL_CTL_ADD, FRONT_FD, &FRONT_EVENT) < 0){
        printf("epoll add failed\n");
        exit(EXIT_FAILURE);
    }    

    FRONT_EVENTARRAY = calloc(MAX_CONN, sizeof(FRONT_EVENT));


    while(TRUE){

        int n, i ;

        n = epoll_wait(FRONT_EPLFD, FRONT_EVENTARRAY, MAX_CONN, -1);

        for (i = 0 ; i < n; i ++){

            if (
                (FRONT_EVENTARRAY[i].events & EPOLLERR) ||
                (FRONT_EVENTARRAY[i].events & EPOLLHUP) ||
                (!(FRONT_EVENTARRAY[i].events & EPOLLIN))
            ){

                printf("epoll wait error \n");
                close(FRONT_EVENTARRAY[i].data.fd);
                continue;

            } else if (FRONT_FD == FRONT_EVENTARRAY[i].data.fd){

                front_handle_conn();

                printf("new front conn successfully handled\n");

            } else{

                front_handle_client(FRONT_EVENTARRAY[i].data.fd);

                printf("front data successfully handled\n");


            }

        }


    }


    free(FRONT_EVENTARRAY);

    close(FRONT_FD);

    close(FRONT_EPLFD);



}


void front_handle_conn(){


    while(TRUE){

        struct sockaddr in_addr;
        socklen_t in_len;
        int infd;

        in_len = sizeof(in_addr);
   
        infd = accept(FRONT_FD, &in_addr, &in_len);

        if(infd == -1){

            if(
                (errno == EAGAIN) ||
                (errno == EWOULDBLOCK)
            ){
                printf("all incoming front connections handled\n");
                break;

            } else{

                printf("error handling incoming front connection\n");
                break;
            }
        }


        if(make_socket_non_blocking(infd) < 0){
            printf("failed new conn non block\n");
            exit(EXIT_FAILURE);
        }

        int front_idx = set_frontctx_by_fd(infd);


        if(front_idx < 0){

            printf("failed new conn sockctx\n");
            exit(EXIT_FAILURE);

        }


        FRONT_EVENT.data.fd = infd;
        FRONT_EVENT.events = EPOLLIN | EPOLLET;

        if (epoll_ctl(FRONT_EPLFD, EPOLL_CTL_ADD, infd, &FRONT_EVENT) < 0){

            printf("handle epoll add failed\n");
            exit(EXIT_FAILURE);

        }  else {

            printf("handle epoll add success\n");

        }


    }

}


void front_handle_client(int cfd){

    int chan_idx = get_chanctx_by_fd(cfd, ISFRONT);

    if(chan_idx < 0){
        
        front_authenticate(cfd);

        return;
    }

    front_communicate(chan_idx);

    return;


}


void front_authenticate(int cfd){


    int valread;
    int valwrite;

    struct HUB_PACKET hp;


    int front_idx = get_frontctx_by_fd(cfd);

    printf("not registered to chan ctx, auth\n");

    if(front_idx < 0){

        printf("failed to get front idx\n");

        return;
    }


    hp.ctx_type = ISFRONT;
    hp.fd = FRONT_CTX[front_idx].frontfd;
    
    ctx_read_packet(&hp);

    if(hp.flag <= 0){

        printf("failed to read front\n");

        free_frontctx(front_idx);

        return;

    }

    uint8_t id[MAX_ID_LEN] = {0};

    int verified_idx = idpw_verify(hp.rbuff);

    if(verified_idx < 0){

        printf("invalid idpw\n");

        free_frontctx(front_idx);

        free(hp.rbuff);

        return;

    }

    strcpy(id, CHAN_CTX[verified_idx].id);


    printf("front id: %s\n", id);

    free(hp.rbuff);

    int chan_idx = set_chanctx_from_frontctx(cfd, id);

    if (chan_idx < 0){

        printf("failed to update chanctx\n");

        free_frontctx(front_idx);

        return;

    }


    uint64_t body_len = (uint64_t)(strlen("SUCCESS") + 1);

    memset(hp.header, 0, HUB_HEADER_BYTELEN);

    memset(hp.wbuff, 0, MAX_BUFF);

    hp.ctx_type = CHAN_ISFRONT;

    strcpy(hp.header, HUB_HEADER_AUTHFRONT);

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



void front_communicate(int chan_idx){


    printf("incoming front communication to sock\n");

    int sockfd = CHAN_CTX[chan_idx].sockfd;

    if(sockfd == 0){

        printf("no sock exists for communication\n");

        return;

    }
    
    printf("sock exists\n");

    struct HUB_PACKET hp;

    hp.ctx_type = CHAN_ISFRONT;

    strcpy(hp.id, CHAN_CTX[chan_idx].id);

    ctx_read_packet(&hp);

    if(hp.flag <= 0){

        printf("failed to communicate front read\n");

        return;

    }

    memset(hp.header, 0, HUB_HEADER_BYTELEN);

    memset(hp.wbuff, 0, MAX_BUFF);

    hp.ctx_type = CHAN_ISSOCK;

    strcpy(hp.header, HUB_HEADER_SENDSOCK);

    strncpy(hp.wbuff, hp.rbuff, hp.body_len);

    hp.flag = 0;

    free(hp.rbuff);

    ctx_write_packet(&hp);

    if(hp.flag <= 0){

        printf("failed to send to sock\n");

        return;
    } 

    printf("sent to sock\n");

    return;


}