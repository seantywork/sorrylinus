#include "sorrylinus/soli.h"
#include "sorrylinus/modules/core.h"


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


uint8_t* soli_handle(uint64_t command_len, uint8_t* command, int* flag){

    uint8_t* body;

    SOLI_CMD* soli_cmd = soli_parse_cmd(command);

    if(strcmp(soli_cmd->cmd, SOLI_INFO_UNAME) == 0){

        *flag = 1024;
        
        body = (uint8_t*)malloc((*flag) * sizeof(uint8_t));

        memset(body, 0, (*flag) * sizeof(uint8_t));

        info_uname(body);



    } else if (strcmp(soli_cmd->cmd, SOLI_CCTV_STREAM) == 0){

        *flag = 1024;

        body = (uint8_t*)malloc((*flag) * sizeof(uint8_t));

        memset(body, 0, (*flag) * sizeof(uint8_t));

    } else {

        *flag = strlen("no such command") + 1;

        body = (uint8_t*)malloc((*flag) * sizeof(uint8_t));

        memset(body, 0, (*flag) * sizeof(uint8_t));

        strcpy(body, "no such command");

    }


    return body;
}



SOLI_CMD* soli_parse_cmd(char* raw){

    char* token;

    char* cmd_delim = ":";

    char* arg_delim = ",";

    SOLI_CMD* soli_cmd = (SOLI_CMD*)malloc(sizeof(SOLI_CMD));

    memset(soli_cmd, 0, sizeof(SOLI_CMD));

    token = strtok(raw, cmd_delim);
    
    int idx = 0;
    
    while( token != NULL ) {

        if(idx == 0){

            strncpy(soli_cmd->cmd, token, SOLI_MAX_CMD_LEN);

        } else {

            int arglen = strlen(token) + 1;

            if (arglen < 2){

                break;
            }

            if(idx == 1){

                soli_cmd->argv = (char**)malloc(sizeof(char*) * idx);

                soli_cmd->argv[idx - 1] = (char*)malloc(sizeof(char) * arglen);

                memset(soli_cmd->argv[idx - 1], 0, sizeof(char) * arglen);

                strcpy(soli_cmd->argv[idx - 1], token);


            } else {

                soli_cmd->argv = (char**)realloc(soli_cmd->argv, sizeof(char*) * idx);

                soli_cmd->argv[idx - 1] = (char*)malloc(sizeof(char) * arglen);

                memset(soli_cmd->argv[idx - 1], 0, sizeof(char) * arglen);

                strcpy(soli_cmd->argv[idx - 1], token); 

            }


        }
        
        token = strtok(NULL, arg_delim);

        idx += 1;
    }

    if(idx == 0){

        free(soli_cmd);

        return (SOLI_CMD*)NULL;

    }

    soli_cmd->argc = idx - 1;
    

    return soli_cmd;
}