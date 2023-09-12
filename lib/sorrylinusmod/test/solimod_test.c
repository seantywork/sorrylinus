
#include <sorrylinusmod/test/solimod_test.h>



char* soli_test_uname(){


    struct utsname uname_ptr;
    uname(&uname_ptr);

    char* str_ptr = (char*)malloc(500);
    strcat(str_ptr,"system name - ");
    strcat(str_ptr,uname_ptr.sysname);
    strcat(str_ptr,"\n");
    strcat(str_ptr,"node name   - ");
    strcat(str_ptr,uname_ptr.nodename);
    strcat(str_ptr,"\n");
    strcat(str_ptr,"release     - ");
    strcat(str_ptr,uname_ptr.release);
    strcat(str_ptr,"\n");
    strcat(str_ptr,"version     - ");
    strcat(str_ptr,uname_ptr.version);
    strcat(str_ptr,"\n");
    strcat(str_ptr,"machine     - ");
    strcat(str_ptr,uname_ptr.machine);
    strcat(str_ptr,"\n"); 


    return str_ptr;  

}

