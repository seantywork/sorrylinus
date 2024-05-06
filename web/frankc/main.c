#include "frank/admin.h"






int main(){



    int sockfd = admin_get_socket(HOST_FULL_ADDRESS);

    if(sockfd < 0){

        printf("failed to connect to hub\n");

        return -1;

    }

    ADMIN_FD = sockfd;


    int result = admin_authenticate();

    if(result < 0){

        printf("failed auth\n");

        return -1;
    }

    uint8_t test_send[ADMIN_CONTENT] = {0};
    uint8_t test_read[ADMIN_CONTENT] = {0};

    strcpy(test_send, "seantywork@gmail.com");

    result = admin_insert_user(test_send, test_read);

    result = admin_eject_user(test_send);





    return 0;
}