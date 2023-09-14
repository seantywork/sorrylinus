
extern "C"{
#include <sorrylinusmod/mod/solimod.h>
#include <sorrylinusmod/test/solimod_test.h>
}

#ifndef SOCK_CLIENT_HEADER
#define SOCK_CLIENT_HEADER
#include <string>
#include <iostream>
#endif

std::string mod_soli_test_uname(){

    char* ret_ptr = soli_test_uname();

    std::string ret_str = ret_ptr;

    free(ret_ptr);

    std::cout << ret_str;

    return ret_str;

}


