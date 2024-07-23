#ifndef _SORRYLINUS_MOD_H_
#define _SORRYLINUS_MOD_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "sorrylinus/soli.h"
#include "sorrylinus/utils/conf.h"
#include "sorrylinus/utils/core.h"

typedef enum SOLI_CMD_TYPE {

  SOLI_DISCOVERY = 0,
  SOLI_INFO_UNAME,
  SOLI_CCTV_STREAM,
  SOLI_IR_GET_OPTS,
  SOLI_IR_SEND,
  SOLI_LEN
};

#define SOLI_MAX_CMDBYTE_LEN 128
#define SOLI_MAX_ARGBYTE_LEN 512
#define SOLI_MAX_CMD_TABLE_LEN SOLI_MAX_CMD_LEN * 10 * 10
#define SOLIMOD_TIMEOUT_MS 20000

typedef struct SOLI_CMD {
  char cmd[SOLI_MAX_CMDBYTE_LEN];
  int argc;
  char **argv;
} SOLI_CMD;

typedef struct SOLI_CMD_TABLE {

    char* cmd;
    char* args;
    char* comment;

} SOLI_CMD_TABLE;


extern SOLI_CMD_TABLE cmd_table[];

extern SOLI_CONF* SOLIMODCFG;

extern FILE* LOGFPMOD;

int solimod_set_cfg(SOLI_CONF* cfg);
int solimod_set_logfp(FILE* logfp);

uint8_t* solimod_handle(uint64_t command_len, uint8_t* command, int* flag);

SOLI_CMD* solimod_parse_cmd(char* raw);

void solimod_free_cmd(SOLI_CMD* soli_cmd);

void solimod_discovery(char* command_table);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif