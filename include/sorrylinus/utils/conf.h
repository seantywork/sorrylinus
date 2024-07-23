#ifndef _SORRYLINUS_CONF_H_
#define _SORRYLINUS_CONF_H_



#include "sorrylinus/soli.h"
#include "sorrylinus/utils/core.h"


typedef enum SOLI_CONF_ENUM {

    SOLICFG_LOGFILE = 0,
    SOLICFG_MOD_IR_EPS,
    SOLICFG_MOD_IR_AEPS,
    SOLICFG_MOD_IR_GAP,
    SOLICFG_MOD_IR_OUTPIN,
    SOLICFG_MOD_IR_FREQUENCY,
    SOLICFG_MOD_IR_OPTS_DIR,
    SOLICFG_MOD_CCTV_SOURCE,
    SOLICFG_MOD_CCTV_DEVICE

} SOLI_CONF_ENUM;



typedef struct SOLI_CONF {

    char logfile[1024];
    int mod_ir_eps;
    int mod_ir_aeps;
    int mod_ir_gap;
    int mod_ir_outpin;
    int mod_ir_frequency;
    char mod_ir_opts_dir[1024];
    char mod_cctv_source[1024];
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