#include "../../lib/httplib/httplib.h"
#include <sqlite3.h> 

using namespace httplib;





struct RecordText {

    std::string first, second, third;

};


static int callback(void* data, int argc, char** argv, char** azColName)
{
    int i;
    
    RecordText* data_callback;

    data_callback = (RecordText*)data;

  
    data_callback->first = argv[0];
    data_callback->second = argv[1];
    data_callback->third = argv[2];

    
    return 0;
}



class Model {


public:


    RecordText GetRecordByCmd(){


        sqlite3* DB;
        int exit = 0;
        exit = sqlite3_open("./model/rec.db", &DB);
        RecordText data;
    
        std::string sql("SELECT * FROM rec;");
        if (exit) {
            std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
            sqlite3_close(DB);
            return data;
            
        }
        else
            std::cout << "Opened Database Successfully!" << std::endl;
    
        int rc = sqlite3_exec(DB, sql.c_str(), callback, (void*)&data, NULL);
    
        if (rc != SQLITE_OK)
            std::cerr << "Error SELECT" << std::endl;
        else {
            std::cout << "Operation OK!" << std::endl;

        }
    
        sqlite3_close(DB);

        return data;


    }





};