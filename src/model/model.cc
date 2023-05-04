#include "../../lib/httplib/httplib.h"
#include <sqlite3.h> 

using namespace httplib;





struct RecordText {

    std::string first, second, third;

};

struct DB_Result {

    int Status;
    int RecordCount = 0;
    RecordText Result[100]; 

};

static int GetRecordByCmd_callback(void* data, int argc, char** argv, char** azColName)
{
    int i; 

    DB_Result* data_callback;

    data_callback = (DB_Result*)data;

    int ridx = data_callback->RecordCount;

    RecordText row;

    for (i =0;i<argc;i++){


        if ((std::string)azColName[i] == "first"){

            row.first = argv[i]? argv[i]:"NULL";

        } else if ((std::string)azColName[i] == "second"){

            row.second = argv[i]? argv[i]:"NULL";
            
        } else if ((std::string)azColName[i] == "third"){

            row.third = argv[i]? argv[i]:"NULL";

        }

    }

    data_callback->RecordCount += 1;
    data_callback->Result[ridx] = row;
    
    
    return 0;
}



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
            
        }else{
            std::cout << "Opened Database Successfully!" << std::endl;
        }
        int rc = sqlite3_exec(DB, sql.c_str(), GetRecordByCmd_callback, (void*)&data, NULL);
    
        if (rc != SQLITE_OK){
            std::cerr << "Error SELECT" << std::endl;
            data.Status = 1;

        }else {
            std::cout << "Operation OK!" << std::endl;
            data.Status = 0;
        }
    
        sqlite3_close(DB);


        return data;


    }





};