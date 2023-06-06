#ifndef SERVER_PREPROCESSOR_HEADER 
#define SERVER_PREPROCESSOR_HEADER y
#include "../lib/httplib/httplib.h"
#include "../lib/nlohmann/json.hpp"
#endif


#include <sqlite3.h> 

using namespace httplib;


std::map<std::string,std::string> RecordText;

struct DB_Result {

    int Status;
    int RecordCount = 0;
    std::vector<std::map<std::string,std::string>> Result; 

};

#include "./callback.cc"

class Model {


public:


    DB_Result GetRecordByCmd(){


        sqlite3* DB;
        int exit = 0;
        exit = sqlite3_open("./model/rec.db", &DB);
        DB_Result data;
    
        std::string sql("SELECT * FROM rec;");
        if (exit) {
            std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
            sqlite3_close(DB);
            data.Status = -1;
            return data;
            
        }
        int rc = sqlite3_exec(DB, sql.c_str(), GetRecordByCmd_callback, (void*)&data, NULL);
    
        if (rc != SQLITE_OK){
            data.Status = 1;

        }else {
            data.Status = 0;
        }
    
        sqlite3_close(DB);


        return data;


    }





};