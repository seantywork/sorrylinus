
//#include "frankhub/sock/v1.h"
#include "frankhub/admin.h"

char CA_CERT[MAX_PW_LEN] = {0};


int main(){


    srand((unsigned int)time(NULL));


    admin_listen_and_serve();

    front_listen_and_server();

    // sock_listen_and_serve();

}