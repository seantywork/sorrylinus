#ifndef PREPROCESSOR_HEADER 
#define PREPROCESSOR_HEADER 
#include <httplib/httplib.h>
#include <jsonlib/json.hpp>
#endif



#include "../model/model.cc"

#include "./build_ret_json.cc"
#include "./controller_utils.cc"

using namespace httplib;

using json = nlohmann::json;

std::ifstream ctrl_f("./config.json");
json ctrl_config_data = json::parse(ctrl_f);

std::string VIEW_ROOT = ctrl_config_data["VIEW_ROOT"];



class Controller {

public:


    static void GetIndex(const Request &req, Response &res){

        std::string index_path = VIEW_ROOT + "html/index.html";

        std::string content = getFileContent(index_path);
        std::string type = "text/html";


        res.set_content(content,type);

        logCurrentTime("[ / ]");


    
    }

    static void GetRecords(const Request &req, Response &res){

        Model mdl;

        std::string body_string = req.body;

        json body_json = json::parse(body_string);

        std::string val = body_json["CMD"];


        
        DB_Result record_data = mdl.GetRecordByCmd();

        std::string ret_data = "";

        if (record_data.Status != 0){

            ret_data += "[]";

            res.set_content(ret_data,"text/plain");
            logCurrentTime("[ /get-records ] : SUCCESS");
            return;

        }


        std::vector<std::string> keys;

        keys.push_back("first");
        keys.push_back("second");
        keys.push_back("third");

        ret_data = BuildRetJSON(keys, record_data);

        try{

            json ret_json = json::parse(ret_data);
        
        } catch (...){

            ret_data = "[]";

            res.set_content(ret_data,"text/plain");
            logCurrentTime("[ /get-records ] : ERROR");
            return;

        }
        
        
        res.set_content(ret_data,"text/plain");


        logCurrentTime("[ /get-records ] : SUCCESS");




    }



};