#ifndef _SORRYLINUS_CONF_H_
#define _SORRYLINUS_CONF_H_



#include "sorrylinus/soli.h"
#include "sorrylinus/utils/core.h"


typedef enum SOLI_CONF_ENUM {

    SOLICFG_LOGFILE = 0,
    SOLICFG_MOD_IR_FREQFILE,
    SOLICFG_MOD_CCTV_DEVICE

} SOLI_CONF_ENUM;



typedef struct SOLI_CONF {

    char logfile[1024];
    char mod_ir_freqfile[1024];
    char mod_cctv_device[1024];

} SOLI_CONF;


typedef struct SOLI_CONF_TABLE{

    char* key;
    char* comment;

} SOLI_CONF_TABLE;

extern SOLI_CONF_TABLE conf_table[];


extern SOLI_CONF SOLICFG;

extern FILE* LOGFP;


int soli_conf_init();

int soli_conf_validate(char* val, char* raw);

int soli_conf_add(char* row);

int soli_conf_add_by_key(char* key, char* val);






#endif