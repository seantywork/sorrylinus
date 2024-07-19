
#include "sorrylinus/utils/core.h"



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

    fclose(fp);

    return valread;
}



void get_current_time_string(char* tstr){


    struct tm *info;
    
    int millisec;
    char msec_str[5] = {0};

    struct timeval tv;


    gettimeofday(&tv, NULL);


    millisec = tv.tv_usec / 1000;

    if (millisec >= 1000){

        millisec -= 1000;
        tv.tv_sec += 1;

    }




    info = localtime(&tv.tv_sec);

    strftime(tstr, MAX_TIMESTR_LEN, "%Y-%m-%d %H:%M:%S", info);

    sprintf(msec_str, ".%03d", millisec);

    strcat(tstr, msec_str);


}

void sleepms(long ms){


    struct timespec ts;
    int res;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    nanosleep(&ts, &ts);



}

void fmt_logln(FILE *fp, char* fmt_out, ...){

    va_list valist;

    char log_str[MAX_LOG_TXT_LEN] = {0};

    char log_fmt_str[MAX_LOG_TXT_LEN] = {0};

    char fmt_arg[MAX_FMT_ARG_LEN] = {0};

    char time_str[MAX_TIMESTR_LEN] = {0};



    int arg_len = 0;

    int str_len = strlen(fmt_out);

    if(str_len < 1){
        return;
    }

    for(int i = 0; i < str_len; i++){

        if(fmt_out[i] == '%' && i != 0 && fmt_out[i - 1] != '\\'){

            arg_len += 1;

        }

    }

    get_current_time_string(time_str);

    if(arg_len == 0){

        sprintf(log_str, "[ %s ] %s\n", time_str, fmt_out);

    } else {

        char fmt_flag[3] = {0};

        va_start(valist, arg_len);

        int i = 0;

        int fmt_i = 0;

        for(;;){
            
            if(fmt_out[i] == '%' && i != 0 && i != (str_len - 1) && fmt_out[i - 1] != '\\'){

                fmt_flag[0] = fmt_out[i];

                i += 1;

                fmt_flag[1] = fmt_out[i];

                fmt_flag[2] = 0;

                memset(fmt_arg, 0, MAX_FMT_ARG_LEN);

                if(fmt_flag[1] == 'd'){

                    int a = va_arg(valist, int);

                    sprintf(fmt_arg, fmt_flag, a);


                } else if (fmt_flag[1] == 's') {

                    char* a = va_arg(valist, char*);

                    sprintf(fmt_arg, fmt_flag, a);                    

                }

                strcat(log_fmt_str, fmt_arg);
                
                int added = strlen(fmt_arg);

                i += 1;
                fmt_i += added;


            } else {

                log_fmt_str[fmt_i] = fmt_out[i];

                i += 1;
                fmt_i += 1;

            }


            if(i >= str_len){
                break;
            }

        }

        va_end(valist);

        sprintf(log_str, "[ %s ] %s\n", time_str, log_fmt_str);
    }


    fputs(log_str, fp);

    fflush(fp);



}