extern "C"{
#include <sorrylinusmod/test/solimod_test.h>    
}


#ifndef SOCK_CLIENT_HEADER
#define SOCK_CLIENT_HEADER
#include <string>
#include <iostream>
#include "../src/module/module.cc"
#endif

std::string mod_query(std::string in_message){

    std::string ret = "";

    if(in_message == "test-uname"){

        ret = mod_soli_test_uname();
        

    } else{

        ret = "ErrNoSuchOps";

    }

    return ret;


}


int main (){


    std::cout<< mod_query("test-uname") <<std::endl;

    return 0;
}





