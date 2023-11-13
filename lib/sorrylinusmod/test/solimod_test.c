
#include <sorrylinusmod/test/solimod_test.h>



std::string soli_test_uname(){


    struct utsname uname_ptr;
    uname(&uname_ptr);

    std::string str_ptr = "";

    str_ptr += "****\n"

    str_ptr += "system name - ";
    std::string sysname = uname_ptr.sysname;
    str_ptr += sysname + "\n";

    str_ptr += "node name - ";
    std::string nodename = uname_ptr.nodename;
    str_ptr += nodename + "\n";

    str_ptr += "release - ";
    std::string release = uname_ptr.release;
    str_ptr += release + "\n";
    
    str_ptr += "version - ";
    std::string version = uname_ptr.version;
    str_ptr += version + "\n";

    str_ptr += "machine - ";
    std::string machine = uname_ptr.machine;
    str_ptr += machine + "\n";

    str_ptr += "****\n"


    return str_ptr;  

}

