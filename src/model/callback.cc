#ifndef PREPROCESSOR_HEADER 
#define PREPROCESSOR_HEADER 
#include "../../include/httplib/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <sqlite3.h> 
std::map<std::string,std::string> RecordText;

struct DB_Result {

    int Status;
    int RecordCount = 0;
    std::vector<std::map<std::string,std::string>> Result; 

};

#endif


static int GetRecordByCmd_callback(void* data, int argc, char** argv, char** azColName)
{
    int i; 

    DB_Result* data_callback;

    data_callback = (DB_Result*)data;

    for (i =0;i<argc;i++){


        if ((std::string)azColName[i] == "first"){

            RecordText["first"] = argv[i]? argv[i]:"NULL";

        } else if ((std::string)azColName[i] == "second"){

            RecordText["second"] = argv[i]? argv[i]:"NULL";
            
        } else if ((std::string)azColName[i] == "third"){

            RecordText["third"] = argv[i]? argv[i]:"NULL";

        }

    }

    data_callback->RecordCount += 1;
    data_callback->Result.push_back(RecordText);
    
    
    return 0;
}
