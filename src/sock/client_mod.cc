#ifndef SOCK_CLIENT_HEADER
#define SOCK_CLIENT_HEADER
#include <string>
#include <iostream>
#endif

#include "../module/module.cc"

std::string mod_query(std::string in_message){

    std::string ret = "";

    if(in_message == "test-uname"){

        ret = mod_soli_test_uname();
        

    } else{

        ret = "ErrNoSuchOps";

    }

    return ret;


}
