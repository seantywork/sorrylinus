#include <sorrylinusmod/test/solimod_test.h>




char* soli_test_method(){

    char* str_ptr;
    struct utsname uname_ptr;
    uname(&uname_ptr);

    printf("system name - %s \n", uname_ptr.sysname);
    printf("node name   - %s \n", uname_ptr.nodename);
    printf("release     - %s \n", uname_ptr.release);
    printf("version     - %s \n", uname_ptr.version);
    printf("machine     - %s \n", uname_ptr.machine);
    

    return 0;  

}
