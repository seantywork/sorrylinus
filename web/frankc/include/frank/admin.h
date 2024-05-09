#ifndef _FRANK_ADMIN_H_
#define _FRANK_ADMIN_H_



#include "frank/core.h"



#define ADMIN_PW "letssharewiththewholeuniverse"
#define ADMIN_CONN 3

#define ADMIN_WORD 8
#define ADMIN_COMMAND ADMIN_WORD * 2
#define ADMIN_CONTENT ADMIN_WORD * 1280 // 10KB 

#define ADMIN_COMMAND_AUTH "ADMINAUTH"
#define ADMIN_COMMAND_INSERTUSER "INSERTUSER"
#define ADMIN_COMMAND_EJECTUSER "EJECTTUSER"


extern int ADMIN_FD;

int admin_get_socket(char* url_str);

int admin_authenticate();

int admin_insert_user(uint8_t* id, uint8_t* pw);

int admin_eject_user(uint8_t* id);





#endif


