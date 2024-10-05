#include "frankhub/v1/admin.h"


int ADMIN_FD;
int ADMIN_SERVLEN;
int ADMIN_ENTERED;

void admin_listen_and_serve(){


    struct sockaddr_in SERVADDR;


    ADMIN_FD = socket(AF_INET, SOCK_STREAM, 0); 

    if (ADMIN_FD == -1) { 
        printf("socket creation failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else {
        printf("socket successfully created\n"); 
    }


    bzero(&SERVADDR, sizeof(SERVADDR)); 

    SERVADDR.sin_family = AF_INET; 
    SERVADDR.sin_addr.s_addr = htonl(INADDR_ANY); 
    SERVADDR.sin_port = htons(PORT_ADMIN); 

    if ((bind(ADMIN_FD, (struct sockaddr*)&SERVADDR, sizeof(SERVADDR))) != 0) { 
        printf("socket bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 


    if ((listen(ADMIN_FD, ADMIN_CONN)) != 0) { 
        printf("listen failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else{
        FRONT_SERVLEN = sizeof(SERVADDR); 
    }



    while(1){
        struct sockaddr in_addr;
        socklen_t in_len;
        int infd;

        int result = 0;

        infd = accept(ADMIN_FD, &in_addr, &in_len);

        result = admin_handle(infd);

        if(result == 1){
            printf("quit serving admin\n");

            break;
        }

        close(infd);

        ADMIN_ENTERED = 0;


    }

    return;

}


int admin_handle(int infd){

    int retval = 0;


    while(1){

        int valread = 0;

        int valwrite = 0;

        uint8_t command[ADMIN_COMMAND] = {0};
        uint8_t content[ADMIN_CONTENT] = {0};

        uint8_t rbuff_tmp[ADMIN_CONTENT] = {0};

        valread = read(infd, command, ADMIN_COMMAND);

        if(valread <= 0){

            printf("client gone\n");

            return valread;
        }

        valread = 0;
        
        while(valread < ADMIN_CONTENT){

            int read_tmp = 0;

            read_tmp = read(infd, rbuff_tmp, ADMIN_CONTENT);

            if(read_tmp <= 0){

                printf("client gone while reading content\n");

                return valread;

            }


            for(int i = 0 ; i < read_tmp; i ++){

                int idx = valread + i;

                content[idx] = rbuff_tmp[i];

            }

            valread += read_tmp;

        }


        if(ADMIN_ENTERED != 1){

            int verified = admin_authenticate(infd, command, content);

            if(verified != 1){
                return verified;
            }

            continue;

        }


        if(strcmp(command, ADMIN_COMMAND_INSERTUSER) == 0){

            retval = admin_insert_user(infd, content);

            if(retval < 0){

                printf("failed to insert user\n");

                continue;
            }

            printf("insert user successful\n");


        } else if (strcmp(command, ADMIN_COMMAND_EJECTUSER) == 0){


            retval = admin_eject_user(infd, content);

            if(retval < 0){

                printf("failed to eject user\n");

                continue;
            }

            printf("eject user successful\n");


        } else {


            printf("invalid admin command\n");

            return -11;
        }  



    }



    return retval;

}


int admin_authenticate(int infd, uint8_t* command, uint8_t* content){

    int ret = 1 ; 

    int valwrite = 0;

    if(strcmp(command, ADMIN_COMMAND_AUTH) != 0){

        printf("invalid auth command\n");

        return -10;

    }

    if(strcmp(content, ADMIN_PW) != 0){

        printf("invalid auth content\n");

        return -10;

    }

    ADMIN_ENTERED = 1;

    valwrite = write(infd, command, ADMIN_COMMAND);

    if(valwrite <= 0){

        printf("client gone while writing command\n");

        ADMIN_ENTERED = 0;

        return valwrite;

    }

    memset(content, 0, ADMIN_CONTENT);

    strcat(content, "SUCCESS");

    valwrite = write(infd, content, ADMIN_CONTENT);

    if(valwrite <= 0){

        printf("client gone while writing content\n");

        ADMIN_ENTERED = 0;

        return valwrite;

    }


    return ret;
}



int admin_insert_user(int infd, uint8_t* id){

    int retval;

    int valwrite = 0;
    int chan_idx = get_chanctx_by_id(id);

    uint8_t new_command[ADMIN_COMMAND] = {0};

    uint8_t new_content[ADMIN_CONTENT] = {0};

    uint8_t pw[MAX_PW_LEN] = {0};

    uint8_t pw_bin[MAX_PW_LEN] = {0};

    strcpy(new_command, ADMIN_COMMAND_INSERTUSER);

    gen_random_bytestream(pw_bin, DEFAULT_RANDLEN);

    bin2hex(pw, DEFAULT_RANDLEN, pw_bin);

    if(chan_idx < 0){

        printf("creating new chanctx\n");

        retval = update_chanctx_from_userinfo(id, pw);        


    } else {

        printf("modifying existing chanctx\n");

        retval = update_chanctx_from_userinfo(id, pw);


    }
    
    if(retval < 0){

        printf("failed to update chanctx\n");

        strcpy(new_content, "FAIL");
    } else {

        printf("successfully added chanctx\n");

        strcpy(new_content, pw);

    }



    valwrite = write(infd, new_command, ADMIN_COMMAND);

    if(valwrite <= 0){

        printf("client gone while writing command\n");

        return valwrite;

    }

    valwrite = write(infd, new_content, ADMIN_CONTENT);

    if(valwrite <= 0){

        printf("client gone while writing content\n");

        return valwrite;

    }


    return retval;

}



int admin_eject_user(int infd, uint8_t* id){

    int retval;

    int valwrite = 0;

    int chan_idx = get_chanctx_by_id(id);

    uint8_t new_command[ADMIN_COMMAND] = {0};

    uint8_t new_content[ADMIN_CONTENT] = {0};

    strcpy(new_command, ADMIN_COMMAND_EJECTUSER);

    if(chan_idx < 0){

        printf("chanctx doesn't exist for %s\n", id);

        retval = -1;      


    } else {

        free_chanctx(chan_idx);

        printf("chanctx for %s deleted\n", id);

        retval = 0;

    }
    
    if(retval < 0){

        printf("failed to eject user from chanctx\n");

        strcpy(new_content, "FAIL");
    
    } else {

        printf("successfully ejected user from chanctx\n");

        strcpy(new_content, "SUCCESS");

    }



    valwrite = write(infd, new_command, ADMIN_COMMAND);

    if(valwrite <= 0){

        printf("client gone while writing command\n");

        return valwrite;

    }

    valwrite = write(infd, new_content, ADMIN_CONTENT);

    if(valwrite <= 0){

        printf("client gone while writing content\n");

        return valwrite;

    }

    return retval;
}