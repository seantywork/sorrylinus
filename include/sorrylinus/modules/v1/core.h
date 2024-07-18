#ifndef _SORRYLINUS_MOD_H_
#define _SORRYLINUS_MOD_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "sorrylinus/soli.h"


#define SOLI_LEN 4
#define SOLI_DISCOVERY 0
#define SOLI_INFO_UNAME 1
#define SOLI_CCTV_STREAM 2
#define SOLI_IR_SEND   3

#define SOLI_MAX_CMD_LEN 64
#define SOLI_MAX_CMD_TABLE_LEN SOLI_MAX_CMD_LEN * 10 * 10

typedef struct SOLI_CMD {
  char cmd[SOLI_MAX_CMD_LEN];
  int argc;
  char **argv;
} SOLI_CMD;

typedef struct SOLI_CMD_TABLE {

    char* cmd;
    char* args;
    char* comment;

} SOLI_CMD_TABLE;

extern SOLI_CMD_TABLE cmd_table[];


uint8_t* solimod_handle(uint64_t command_len, uint8_t* command, int* flag);

SOLI_CMD* solimod_parse_cmd(char* raw);

void solimod_free_cmd(SOLI_CMD* soli_cmd);

void solimod_discovery(char* command_table);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif