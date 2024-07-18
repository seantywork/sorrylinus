
#include "sorrylinus/modules/v1/core.h"




SOLI_CMD_TABLE cmd_table[] = {

    { .cmd = "discovery",    .args="-",                .comment="-"},
    { .cmd = "info-uname",   .args="-",                .comment="-"},
    { .cmd = "cctv-stream",  .args="conf,address",     .comment="-"},
    { .cmd = "ir-send",      .args="conf",             .comment="-"},
};



uint8_t* solimod_handle(uint64_t command_len, uint8_t* command, int* flag){

    uint8_t* body;

    *flag = 10240;
    
    body = (uint8_t*)malloc((*flag) * sizeof(uint8_t));

    memset(body, 0, (*flag) * sizeof(uint8_t));

    SOLI_CMD* soli_cmd = solimod_parse_cmd(command);

    if((void*)soli_cmd == NULL){

        strcpy(body, "failed to handle soli: null cmd");

        return body;

    }

    if(strcmp(soli_cmd->cmd, cmd_table[SOLI_DISCOVERY].cmd) == 0){


        solimod_discovery(body);



    } else if (strcmp(soli_cmd->cmd, cmd_table[SOLI_INFO_UNAME].cmd) == 0){

        info_uname(body);

    } else {

        *flag = strlen("no such command") + 1;

        body = (uint8_t*)malloc((*flag) * sizeof(uint8_t));

        memset(body, 0, (*flag) * sizeof(uint8_t));

        strcpy(body, "no such command");

    }

    solimod_free_cmd(soli_cmd);


    return body;
}



SOLI_CMD* solimod_parse_cmd(char* raw){

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


void solimod_free_cmd(SOLI_CMD* soli_cmd){


    if(soli_cmd->argc != 0){

        for(int i = 0 ; i < soli_cmd->argc; i ++){


            free(soli_cmd->argv[i]);

        }

        free(soli_cmd->argv);

    }


    free(soli_cmd);


}


void solimod_discovery(char* command_table){


    for(int i =0; i < SOLI_LEN; i++){

        strcat(command_table, cmd_table[i].cmd);
        strcat(command_table, ":");
        strcat(command_table, cmd_table[i].args);
        strcat(command_table,"\n");

    }


}