

#include <sorrylinusmod/mod/solimod.h>
#include <sorrylinusmod/test/solimod_test.h>


#ifndef SOLI_HEADER_TEST
#define SOLI_HEADER_TEST
#include <string>
#include <iostream>
#endif

std::string mod_soli_test_uname(){

    char result[MAX_TEST_UNAME_LEN] = {0};

    soli_test_uname(result);

    std::string ret_str = result;

    std::cout << ret_str;

    return ret_str;

}


