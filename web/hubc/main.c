
//#include "frankhub/sock/v1.h"
#include "frankhub/admin.h"

char CA_CERT[MAX_PW_LEN] = {0};


int main(){


    srand((unsigned int)time(NULL));

    // sock_listen_and_serve();

    // front_listen_adn_server();

    admin_listen_and_serve();


}