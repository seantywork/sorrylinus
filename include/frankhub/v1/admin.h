#ifndef _FRANK_HUB_ADMIN_H_
#define _FRANK_HUB_ADMIN_H_


#include "frankhub/v1/core.h"




#define PORT_ADMIN 2999
#define ADMIN_PW "letssharewiththewholeuniverse"
#define ADMIN_CONN 3

#define ADMIN_WORD 8
#define ADMIN_COMMAND ADMIN_WORD * 2
#define ADMIN_CONTENT ADMIN_WORD * 1280 // 10KB 

#define ADMIN_COMMAND_AUTH "ADMINAUTH"
#define ADMIN_COMMAND_INSERTUSER "INSERTUSER"
#define ADMIN_COMMAND_EJECTUSER "EJECTTUSER"

extern int ADMIN_FD;
extern int ADMIN_SERVLEN;
extern int ADMIN_ENTERED;

void admin_listen_and_serve();


int admin_handle(int infd);

int admin_authenticate(int infd, uint8_t* command, uint8_t* content);

int admin_insert_user(int infd, uint8_t* id);

int admin_eject_user(int infd, uint8_t* id);


#endif

