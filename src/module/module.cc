

#include <sorrylinusmod/mod/solimod.h>
#include <sorrylinusmod/test/solimod_test.h>


#ifndef SOLI_HEADER_TEST
#define SOLI_HEADER_TEST
#include <string>
#include <iostream>
#endif

std::string mod_soli_test_uname(){

    std::string ret_str = soli_test_uname();

    std::cout << ret_str;

    return ret_str;

}


