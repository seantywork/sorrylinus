
#include "sorrylinus/app/core.h"
#include "sorrylinus/mod/core.h"

std::string mod_query(std::string in_message){

    std::string ret = "";

    if(in_message == "test-uname"){

        char test_uname[MAX_TEST_UNAME_LEN] = {0};

        SOLIMOD_test_uname(test_uname);
        
        ret = test_uname;


    } else{

        ret = "ErrNoSuchOps";

    }

    return ret;


}
