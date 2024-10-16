#include "sorrylinus/utils/conf.h"


SOLI_CONF SOLICFG;

FILE* LOGFP = NULL;


SOLI_CONF_TABLE conf_table[] = {

    { .key = "logfile",              .comment = ""},
    { .key = "mod_ir_eps",      .comment = ""},
    { .key = "mod_ir_aeps",      .comment = ""},
    { .key = "mod_ir_gap",      .comment = ""},
    { .key = "mod_ir_outpin",      .comment = ""},
    { .key = "mod_ir_frequency",      .comment = ""},
    { .key = "mod_ir_opts_dir",      .comment = ""},
    { .key = "mod_cctv_source",      .comment = ""},
    { .key = "mod_cctv_device",      .comment = ""}

};

int soli_conf_init(){

    uint8_t wbuff[MAX_BUFF] = {0};

    int conflen = read_file_to_buffer(wbuff, MAX_BUFF, "conf/soli.conf");

    if(conflen <= 0){

        return conflen;

    }

    char** lines;

    int line_count = 0;

    char* token;

    char* delim = "\n";

    char row[SOLI_MAX_CONF_RAW_LEN] = {0};

    token = strtok(wbuff, delim);

    while(token != NULL){

        int linelen = 0;

        if (line_count == 0){

            lines = (char**)malloc(sizeof(char*) * (line_count + 1));

        } else {

            lines = (char**)realloc(lines, sizeof(char*) * (line_count + 1));

        }

        linelen = strlen(token) + 1;

        lines[line_count] = (char*)malloc(sizeof(char) * linelen);

        memset(lines[line_count], 0 , sizeof(char) * linelen);

        strcpy(lines[line_count], token);

        line_count += 1;

        token = strtok(NULL, delim);
    }


    for (int i = 0 ; i < line_count; i++) {

        memset(row, 0, SOLI_MAX_CONF_RAW_LEN);

        int flag = soli_conf_validate(row, lines[i]);

        if(flag < 0){

            return flag;
        }

        flag = soli_conf_add(row);

        if(flag < 0) {

            return flag;
        }

    }

    for(int i = 0 ; i < line_count; i++){

        free(lines[i]);

    }

    if (line_count != 0){

        free(lines);
    }


    return 0;
}


int soli_conf_validate(char* val, char* raw){


    int rawlen = strlen(raw);

    if (rawlen < 3){

        return -1;

    }

    int idx = 0;

    for(int i = 0 ; i < rawlen; i ++){

        if(raw[i] == ' '){

            continue;
        }

        val[idx] = raw[i];

        idx += 1;

        if(idx > SOLI_MAX_CONF_RAW_LEN){
            return -2;
        }

    }

    val[idx] = 0;

    return idx;

}


int soli_conf_add(char* row){

    char* token;

    char* delim = ":";

    token = strtok(row, delim);

    char key[SOLI_MAX_KEY_LEN] = {0};
    char val[SOLI_MAX_VAL_LEN] = {0};

    int idx = 0;

    while( token != NULL ) {

        if(idx == 0){

            strcpy(key, token);

        } else {

            strcpy(val, token);
            break;
        }

        token = strtok(NULL, delim);


        idx += 1;

    }

    if (idx != 1){

        return -11;

    }

    int flag = soli_conf_add_by_key(key, val);

    return flag;
}


int soli_conf_add_by_key(char* key, char* val){


    if(strcmp(key, conf_table[SOLICFG_LOGFILE].key) == 0){

        strcpy(SOLICFG.logfile, val);

        printf("config: logfile: %s\n", SOLICFG.logfile);

        LOGFP = fopen(SOLICFG.logfile,"a");

        printf("config: logfile: opened\n");

        return 0;

    } else if (strcmp(key, conf_table[SOLICFG_MOD_IR_EPS].key) == 0){

        sscanf(val, "%d", &SOLICFG.mod_ir_eps);

        printf("config: mod_ir_eps: %d\n", SOLICFG.mod_ir_eps);

        return 1;

    }else if (strcmp(key, conf_table[SOLICFG_MOD_IR_AEPS].key) == 0){


        sscanf(val, "%d", &SOLICFG.mod_ir_aeps);

        printf("config: mod_ir_aeps: %d\n", SOLICFG.mod_ir_aeps);

        return 2;

    }else if (strcmp(key, conf_table[SOLICFG_MOD_IR_GAP].key) == 0){


        sscanf(val, "%d", &SOLICFG.mod_ir_gap);

        printf("config: mod_ir_gap: %d\n", SOLICFG.mod_ir_gap);

        return 3;

    }else if (strcmp(key, conf_table[SOLICFG_MOD_IR_OUTPIN].key) == 0){

        sscanf(val, "%d", &SOLICFG.mod_ir_outpin);

        printf("config: mod_ir_outpin: %d\n", SOLICFG.mod_ir_outpin);

        return 4;

    }else if (strcmp(key, conf_table[SOLICFG_MOD_IR_FREQUENCY].key) == 0){

        sscanf(val, "%d", &SOLICFG.mod_ir_frequency);

        printf("config: mod_ir_frequency: %d\n", SOLICFG.mod_ir_frequency);

        return 5;

    }else if (strcmp(key, conf_table[SOLICFG_MOD_IR_OPTS_DIR].key) == 0){

        strcpy(SOLICFG.mod_ir_opts_dir, val);
        printf("config: mod_ir_opts_dir: %s\n", SOLICFG.mod_ir_opts_dir);

        return 6;

    } else if (strcmp(key, conf_table[SOLICFG_MOD_CCTV_SOURCE].key) == 0){

        strcpy(SOLICFG.mod_cctv_source, val);
        printf("config: mod_cctv_source: %s\n", SOLICFG.mod_cctv_source);

        return 7;

    } else if (strcmp(key, conf_table[SOLICFG_MOD_CCTV_DEVICE].key) == 0){

        strcpy(SOLICFG.mod_cctv_device, val);
        printf("config: mod_cctv_device: %s\n", SOLICFG.mod_cctv_device);

        return 8;

    } else {

        printf("config: invalid: key: %s\n", key);

        return -101;

    }


    return -100;

}


