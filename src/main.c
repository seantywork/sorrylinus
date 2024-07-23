#include "sorrylinus/soli.h"
#include "sorrylinus/v1/cmd/core.h"
#include "sorrylinus/v1/sock/core.h"
#include "sorrylinus/utils/conf.h"
#include "sorrylinus/modules/v1/core.h"


int main(int argc, char** argv){


    printf("initializing sorrylinus with conf....\n");


    int result = soli_conf_init();


    if(result < 0){

        printf("failed to init: %d\n", result);

        return -1;

    }

    printf("initializing mod with conf....\n");

    result = solimod_set_cfg(&SOLICFG);

    if(result < 0){

        printf("failed to set mod cfg: %d\n", result);

        return -1;

    }

    result = solimod_set_logfp(LOGFP);


    if(result < 0){

        printf("failed to set mod logfp: %d\n", result);

        return -1;

    }

    if(argc > 1){

        result = cmd_communicate(argv[1]);

    } else {

        printf("connecting to hubc...\n");



        result = hubc_connect();

        if(result < 0){

            printf("failed to connect: %d\n", result);

            return -1;

        }

        result = hubc_communicate();
        

        if(result < 0){

            printf("failed to communicate: %d\n", result);

            return -1;
        }

        printf("comminication terminated\n");

    }

    return 0;
}
