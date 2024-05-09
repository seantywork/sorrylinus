#include "frank/controller/client.h"



int client_connect(int uidx){


    int cfd = client_get_socket(HOST_FULL_ADDRESS_CLIENT);

    if(cfd < 0){

        printf("failed to get socket for: %d\n", uidx);

        return -1;
    }

    printf("connected for: %d\n", uidx);

    user_array[uidx].cfd = cfd;

    return cfd;

}

int client_get_socket(char* url_str){


    int cfd;
    char hostname[256] = "";
    char    portnum[6] = "";
    char      proto[6] = "";
    char      *tmp_ptr = NULL;
    int           port;
    struct hostent *host;
    struct sockaddr_in dest_addr;


    if(url_str[strlen(url_str)] == '/')
    url_str[strlen(url_str)] = '\0';


    strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

    strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));


    if(strchr(hostname, ':')) {
        tmp_ptr = strchr(hostname, ':');

        strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
        *tmp_ptr = '\0';
    }

    port = atoi(portnum);

    if ( (host = gethostbyname(hostname)) == NULL ) {
        printf("failed to get host\n");
        return -1;
    }


    cfd = socket(AF_INET, SOCK_STREAM, 0);

    if(cfd < 0){

        printf("failed to get socket\n");

        return -1;
    }

    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);


    memset(&(dest_addr.sin_zero), '\0', 8);

    tmp_ptr = inet_ntoa(dest_addr.sin_addr);

    struct timeval u_timeout;      
    u_timeout.tv_sec = TIMEOUT_SEC;
    u_timeout.tv_usec = 0;
    

    if (setsockopt (cfd, SOL_SOCKET, SO_RCVTIMEO, &u_timeout, sizeof(u_timeout)) < 0){

        printf("set recv timeout\n");

        return -3;
    }
    

    if (setsockopt (cfd, SOL_SOCKET, SO_SNDTIMEO, &u_timeout, sizeof(u_timeout)) < 0) {

        printf("set send timeout\n");
    
        return -4;
    }
    

    int rc = 0;
    
    int sockfd_flags_before;

    sockfd_flags_before = fcntl(cfd,F_GETFL,0);
    
    if(sockfd_flags_before < 0) {
    
        printf("failed to getfl\n");

        return -5;
    
    }
    
    

    if(fcntl(cfd, F_SETFL, sockfd_flags_before | O_NONBLOCK)<0){


        printf("failed to setfl");

        return -6;
    
    } 



    if (connect(cfd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {

        if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS)) {

            rc = -11;

        } else {
            
            struct timespec now;

            clock_gettime(CLOCK_MONOTONIC_RAW, &now);

            struct timespec deadline;


            do{

                clock_gettime(CLOCK_MONOTONIC_RAW, &deadline);

                int ms_until_deadline = ((deadline.tv_sec - now.tv_sec) * 1000 + (deadline.tv_nsec - now.tv_nsec) / 1000000);
 
                if(ms_until_deadline > TIMEOUT_MS) { 
                    rc = -10; 
                    break; 
                }
                
                struct pollfd pfds[] = { { .fd = cfd, .events = POLLOUT } };
                rc = poll(pfds, 1, ms_until_deadline);
                
                if(rc > 0) {
                    int error = 0; 
                    socklen_t len = sizeof(error);
                    int retval = getsockopt(cfd, SOL_SOCKET, SO_ERROR, &error, &len);
                    if(retval == 0) {
                        errno = error;
                    }
                    if(error != 0) {
                        rc = -11;
                    }
                }

            } while(rc == -11 && errno == EINTR);


            if(rc == -10) {
                
                errno = ETIMEDOUT;
                
            }

        }

    } 

    if(fcntl(cfd,F_SETFL,sockfd_flags_before ) < 0) {

        return -20;
    }


    return cfd;




}




int client_write(int cfd, int write_len, uint8_t* write_bytes){

    int valwrite = 0;


    valwrite = write(cfd, write_bytes, write_len * sizeof(uint8_t));

    if(valwrite <= 0){

        printf("write: server gone: %d\n", valwrite);

        return -2;
    }

    return valwrite;

}

int client_read(int cfd, int read_len, uint8_t* read_bytes){


    int valread = 0;

    int ms_until_deadline = 0;

    uint8_t* rbuff_tmp = (uint8_t*)malloc(read_len * sizeof(uint8_t));

    memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

    int valread_tmp = 0;

    struct timespec rnow;

    clock_gettime(CLOCK_MONOTONIC_RAW, &rnow);

    struct timespec rdeadline;

    while(valread < read_len){

        clock_gettime(CLOCK_MONOTONIC_RAW, &rdeadline);
        
        ms_until_deadline = ((rdeadline.tv_sec - rnow.tv_sec) * 1000 + (rdeadline.tv_nsec - rnow.tv_nsec) / 1000000);

        
        if(ms_until_deadline > TIMEOUT_MS){
            
            printf("time limit exceeded\n");

            free(rbuff_tmp);

            return -10;
        }


        valread_tmp = read(cfd, (void*)rbuff_tmp, read_len);

        if(valread_tmp <= 0){
            
            if(errno == EAGAIN){

                memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

                valread_tmp = 0;

                continue;
            }

            printf("read: server gone: %d\n", valread);

            free(rbuff_tmp);

            return -2;
        }

        for(int i = 0 ; i < valread_tmp; i++){

            int idx = valread + i;

            read_bytes[idx] = rbuff_tmp[i];

        }

        valread += valread_tmp;        

        memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

        valread_tmp = 0;

    }


    free(rbuff_tmp);

    return valread;


}


int client_write_packet(int uidx, uint8_t* header, uint64_t body_len, uint8_t* wbuff){

    int cfd = user_array[uidx].cfd;

    int valwrite = 0;

    valwrite = client_write(cfd, HUB_HEADER_BYTELEN, header);

    if(valwrite <= 0){

        printf("packet send header failed\n");

        return valwrite;

    }


    uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

    uint64_t body_len_new = 0; 

    body_len_new = htonll(body_len);

    memcpy(body_len_byte, &body_len_new, HUB_BODY_BYTELEN);

    valwrite = client_write(cfd, HUB_BODY_BYTELEN, body_len_byte);

    if(valwrite <= 0){

        printf("packet send body len failed\n");

        return valwrite;

    }

    valwrite = client_write(cfd, body_len, wbuff);

    if(valwrite <= 0){

        printf("packet send body failed\n");

        return valwrite;

    }

    return valwrite;


}

uint8_t* client_read_packet(int uidx, uint8_t* header, uint64_t* body_len, int* flag){


    int cfd = user_array[uidx].cfd;


    uint8_t* rbuff = NULL;

    int valread = 0;

    valread = client_read(cfd, HUB_HEADER_BYTELEN, header);

    if(valread <= 0){

        printf("packet read header failed \n");

        *flag = valread;

        return rbuff;
    }


    uint8_t body_len_byte[HUB_BODY_BYTELEN] = {0};

    uint64_t body_len_new = 0;

    valread = client_read(cfd, HUB_BODY_BYTELEN, body_len_byte);

    if(valread <= 0){

        printf("packet read body len failed \n");

        *flag = valread;

        return rbuff;

    }

    memcpy(&body_len_new, body_len_byte, HUB_BODY_BYTELEN);

    *body_len = ntohll(body_len_new);

    rbuff = (uint8_t*)malloc((*body_len) * sizeof(uint8_t));

    memset(rbuff, 0, (*body_len) * sizeof(uint8_t));

    valread = client_read(cfd, (*body_len), rbuff);

    if(valread <=0){

        printf("packet read failed \n");

        *flag = valread;

        free(rbuff);

        return rbuff;
    }

    *flag = valread;

    return rbuff;

}




int client_exchange(int uidx, int* recv_len, uint8_t* recv_buff, int send_len, uint8_t* send_buff){


    int result = 0;

    uint8_t header[HUB_HEADER_BYTELEN]= {0};
    uint64_t body_len = 0;
    int flag = 0;
    uint8_t* rbuff;
    uint8_t wbuff[MAX_BUFF] = {0};

    strcpy(header, HUB_HEADER_SENDFRONT);

    body_len = send_len;

    memcpy(wbuff, send_buff, send_len);

    result = client_write_packet(uidx, header, body_len, wbuff);

    if(result <= 0){

        printf("failed to exchange: send to front\n");

        return -1;
    
    }
    memset(header, 0, HUB_HEADER_BYTELEN);

    body_len = 0;

    rbuff = client_read_packet(uidx, header, &body_len, &flag);

    if(flag <= 0){

        printf("failed to exchange: recv from front: %d\n", flag);

        return -2;

    }

    *recv_len = flag;


    memcpy(recv_buff, rbuff, flag);

    free(rbuff);

    return 0;

}
