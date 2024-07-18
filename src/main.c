#include "sorrylinus/soli.h"
#include "sorrylinus/v1/sock/core.h"


int main(int argc, char** argv){



    printf("connecting to hubc...\n");


    int result = hubc_connect();

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


    return 0;
}


int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path){


    int valread = 0 ;

    int c;

    FILE* fp;

    fp = fopen(file_path, "r");

    while(1) {

        c = fgetc(fp);
        if( feof(fp) ) {
            break;
        }


        buff[valread] = c;

        valread += 1;

        if(valread > max_buff_len){

            return -10;
        }
    
   }

    return valread;
}
