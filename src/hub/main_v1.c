
#include "frankhub/v1/sock/core.h"
#include "frankhub/v1/front/core.h"
#include "frankhub/v1/admin.h"
#include "frankhub/v1/utils.h"
FILE* LOGFP;

pthread_mutex_t G_MTX;

int main(){


    srand((unsigned int)time(NULL));

    LOGFP = fopen("log/hub.txt","a");


    pthread_t sock_tid;
    pthread_t front_tid;
    
    if (pthread_mutex_init(&G_MTX, NULL) != 0) { 
        printf("mutex init has failed\n"); 
        return -1; 
    } 
  
    
    pthread_create(&front_tid, NULL, (void*)front_listen_and_serve, NULL);

    sleepms(500);

    pthread_create(&sock_tid, NULL, (void*)sock_listen_and_serve, NULL);

    sleepms(500);

    admin_listen_and_serve();

    fclose(LOGFP);


}