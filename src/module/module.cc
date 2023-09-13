
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

    return soli_test_uname();
}


