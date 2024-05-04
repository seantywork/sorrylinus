#include "sorrylinus/sock/v1.h"


int main(int argc, char** argv){



    printf("connecting to hubc...\n");


    int result = hubc_connect();

    if(result < 0){

        printf("failed to connect: %d\n", result);

        return -1;

    }


    





    return 0;
}