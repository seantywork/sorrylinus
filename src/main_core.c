#include "sorrylinus/soli.h"


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