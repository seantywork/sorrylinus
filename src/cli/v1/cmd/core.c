#include "sorrylinus/v1/cmd/core.h"
#include "sorrylinus/modules/v1/core.h"
#include "sorrylinus/utils/core.h"




int cmd_communicate(char* arg){

    uint64_t body_len  = 0;
    int flag = 0;
    uint8_t wbuff[MAX_BUFF] = {0};

    body_len = strlen(arg);

    uint8_t* body = solimod_handle(body_len, arg, &flag);

    memcpy(wbuff, body, flag);

    free(body);

    printf("%s\n", wbuff);

    return 0;

}
