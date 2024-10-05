
#include "frankhub/v2/sock/core.h"
#include "frankhub/v2/front/core.h"
#include "frankhub/v2/utils.h"


FILE* LOGFP;

pthread_mutex_t G_MTX;


int s_sig_num = 0;

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_sig_num = sig_num;
}

int main(){


    srand((unsigned int)time(NULL));


    int result = load_config_json();

    LOGFP = fopen("log/hub.txt","a");

    pthread_t sock_tid;
    
    if (pthread_mutex_init(&G_MTX, NULL) != 0) { 
        printf("mutex init has failed\n"); 
        return -1; 
    } 

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    pthread_create(&sock_tid, NULL, (void*)sock_listen_and_serve, NULL);

    sleepms(500);

    front_listen_and_serve();

    fclose(LOGFP);


}