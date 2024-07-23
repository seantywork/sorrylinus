
#include "sorrylinus/modules/v1/core.h"
#include "sorrylinus/modules/v1/info/core.h"
#include "sorrylinus/modules/v1/cctv/core.h"
#include "sorrylinus/utils/core.h"
#include "sorrylinus/utils/conf.h"




SOLI_CMD_TABLE cmd_table[] = {

    { .cmd = "discovery",    .args="-",                .comment="-"},
    { .cmd = "info-uname",   .args="-",                .comment="-"},
    { .cmd = "cctv-stream",  .args="location",         .comment="-"},
    { .cmd = "ir-get-opts",  .args="-",                .comment="-"},
    { .cmd = "ir-send",      .args="opt",              .comment="-"},
};


SOLI_CONF* SOLIMODCFG = NULL;

FILE* LOGFPMOD = NULL;


int solimod_set_cfg(SOLI_CONF* cfg){


    if(cfg == NULL){

        printf("null cfg\n");

        return -1;

    }

    SOLIMODCFG = cfg;


    return 0;
}

int solimod_set_logfp(FILE* logfp){

    if(logfp == NULL){

        printf("null logfp\n");

        return -1;
    }

    LOGFPMOD = logfp;


    return 0;
}

uint8_t* solimod_handle(uint64_t command_len, uint8_t* command, int* flag){

    uint8_t* body;

    *flag = 10240;
    
    body = (uint8_t*)malloc((*flag) * sizeof(uint8_t));

    memset(body, 0, (*flag) * sizeof(uint8_t));

    printf("raw cmd received: %s\n", command);

    SOLI_CMD* soli_cmd = solimod_parse_cmd(command);


    printf("cmd: %s\n", soli_cmd->cmd);

    printf("argc: %d\n", soli_cmd->argc);

    if((void*)soli_cmd == NULL){

        strcpy(body, "failed to handle soli: null cmd");

        return body;

    }

    if(strcmp(soli_cmd->cmd, cmd_table[SOLI_DISCOVERY].cmd) == 0){


        solimod_discovery(body);



    } else if (strcmp(soli_cmd->cmd, cmd_table[SOLI_INFO_UNAME].cmd) == 0){


        info_uname(body);


    } else if (strcmp(soli_cmd->cmd, cmd_table[SOLI_CCTV_STREAM].cmd) == 0){


        if(soli_cmd->argc < 1){

            sprintf(body, "failed to handle soli: too few arguments: %d for cmd: %s\n" , soli_cmd->argc, soli_cmd->cmd);

        } else {

            cctv_stream_toggle(body, soli_cmd->argv[0]);
        
        }



    } else {


        strcpy(body, "failed to handle soli: no such cmd");
    
    }



    solimod_free_cmd(soli_cmd);


    return body;
}



SOLI_CMD* solimod_parse_cmd(char* raw){

    char* token;

    char* arg_delim = ",";

    char raw_arg[SOLI_MAX_ARGBYTE_LEN] = {0};

    int got_cmd = 0;

    int idx = 0;

    SOLI_CMD* soli_cmd = (SOLI_CMD*)malloc(sizeof(SOLI_CMD));

    memset(soli_cmd, 0, sizeof(SOLI_CMD));
    
    while(1) {

        if(*raw == ':'){

            got_cmd = 1;
            break;

        }

        soli_cmd->cmd[idx] = *raw;

        idx += 1;
        raw += 1;

        if(idx > SOLI_MAX_CMDBYTE_LEN){

            break;

        }

    }

    if(got_cmd != 1){

        free(soli_cmd);

        return (SOLI_CMD*)NULL;

    }

    idx = 0;

    raw += 1;

    strcpy(raw_arg, raw);

    token = strtok(raw_arg, arg_delim);

    while(token != NULL){

        if(idx == 0){

            soli_cmd->argv = (char**)malloc(sizeof(char*) * (idx + 1));

        } else {

            soli_cmd->argv = (char**)realloc(soli_cmd->argv, sizeof(char*) * (idx + 1));
        }

        int arglen = strlen(token) + 1;

        soli_cmd->argv[idx] = (char*)malloc(sizeof(char) * arglen);

        memset(soli_cmd->argv[idx], 0, sizeof(char) * arglen);

        strcpy(soli_cmd->argv[idx], token);
        
        token = strtok(NULL, arg_delim);

        idx += 1;
    }


    soli_cmd->argc = idx;
    

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