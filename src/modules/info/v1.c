#include "sorrylinus/modules/info/v1.h"





void info_uname(char* result){


    struct utsname uname_ptr;
    uname(&uname_ptr);

    strcat(result, "****\n");

    strcat(result, "system name - ");

    strcat(result, uname_ptr.sysname);

    strcat(result, "\n");

    strcat(result, "node name - ");

    strcat(result, uname_ptr.nodename);

    strcat(result, "\n");

    strcat(result, "release - ");

    strcat(result, uname_ptr.release);

    strcat(result, "\n");

    strcat(result, "version - ");

    strcat(result, uname_ptr.version);

    strcat(result, "\n");

    strcat(result, "machine - ");

    strcat(result, uname_ptr.machine);

    strcat(result, "\n");

    strcat(result, "****\n");

}