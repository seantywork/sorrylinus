#include "frank/admin.h"

int ADMIN_FD;




int admin_get_socket(char* url_str){

    int sockfd;
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


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);


    memset(&(dest_addr.sin_zero), '\0', 8);

    tmp_ptr = inet_ntoa(dest_addr.sin_addr);


    if ( connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1 ) {

        printf("cannot connect to host\n");

        return -2;
    }

    return sockfd;
}


int admin_authenticate(){

    int retval = 0;

    uint8_t command[ADMIN_COMMAND] = {0};
    uint8_t content[ADMIN_CONTENT] = {0};

    strcpy(command, ADMIN_COMMAND_AUTH);
    strcpy(content, ADMIN_PW);

    retval = admin_write(ADMIN_COMMAND, command);

    if(retval < 0){

        printf("failed to send command\n");

        return retval;

    }


    retval = admin_write(ADMIN_CONTENT, content);

    if(retval < 0){

        printf("failed to send content\n");

        return retval;

    }

    memset(command, 0, ADMIN_COMMAND);
    memset(content, 0, ADMIN_CONTENT);

    retval = admin_read(ADMIN_COMMAND, command);

    if(retval < 0 ){

        printf("failed to read command\n");

        return retval;
    }

    retval = admin_read(ADMIN_CONTENT, content);

    if(retval < 0 ){

        printf("failed to read content\n");

        return retval;
    }

    printf("auth:\n");
    printf("cmd: %s\n", command);
    printf("content: %s\n", content);


    return retval;
}

int admin_insert_user(uint8_t* id, uint8_t* pw){

    int retval = 0;

    uint8_t command[ADMIN_COMMAND] = {0};
    uint8_t content[ADMIN_CONTENT] = {0};

    strcpy(command, ADMIN_COMMAND_INSERTUSER);
    strcpy(content, id);


    retval = admin_write(ADMIN_COMMAND, command);

    if(retval < 0){

        printf("failed to send command\n");

        return retval;

    }

    retval = admin_write(ADMIN_CONTENT, content);

    if(retval < 0){

        printf("failed to send content\n");

        return retval;

    }


    memset(command, 0, ADMIN_COMMAND);
    memset(content, 0, ADMIN_CONTENT);

    retval = admin_read(ADMIN_COMMAND, command);

    if(retval < 0 ){

        printf("failed to read command\n");

        return retval;
    }

    retval = admin_read(ADMIN_CONTENT, content);

    if(retval < 0 ){

        printf("failed to read content\n");

        return retval;
    }

    printf("got: %s\n", content);

    strcpy(pw, content);

    return retval;
}

int admin_eject_user(uint8_t* id){

    int retval = 0;


    uint8_t command[ADMIN_COMMAND] = {0};
    uint8_t content[ADMIN_CONTENT] = {0};

    strcpy(command, ADMIN_COMMAND_EJECTUSER);
    strcpy(content, id);


    retval = admin_write(ADMIN_COMMAND, command);

    if(retval < 0){

        printf("failed to send command\n");

        return retval;

    }

    retval = admin_write(ADMIN_CONTENT, content);

    if(retval < 0){

        printf("failed to send content\n");

        return retval;

    }


    memset(command, 0, ADMIN_COMMAND);
    memset(content, 0, ADMIN_CONTENT);

    retval = admin_read(ADMIN_COMMAND, command);

    if(retval < 0 ){

        printf("failed to read command\n");

        return retval;
    }

    retval = admin_read(ADMIN_CONTENT, content);

    if(retval < 0 ){

        printf("failed to read content\n");

        return retval;
    }

    printf("got: %s\n", content);


    return retval;
}


int admin_read(int read_len, uint8_t* rbuff){


    int valread = 0;

    int front_idx = 0;

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

        if(ms_until_deadline > HUB_TIMEOUT_MS){
            
            printf("time limit exceeded\n");

            free(rbuff_tmp);

            return -10;
        }

        valread_tmp = read(ADMIN_FD, (void*)rbuff_tmp, read_len);

        if(valread_tmp <= 0){

            if(errno == EAGAIN){

                memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

                valread_tmp = 0;

                continue;
            }

            printf("read: admin server gone: %d\n", valread);

            free(rbuff_tmp);

            return -1;
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


int admin_write(int write_len, uint8_t* wbuff){



    int valwrite = 0;

    int front_idx = 0;

    valwrite = write(ADMIN_FD, (void*)wbuff, write_len);

    if (valwrite <= 0){

        printf("write: admin server gone: %d\n", valwrite);

        return -1;

    }

    return valwrite;





}
