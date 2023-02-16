#include "../../lib/httplib/httplib.h"
#include "../../lib/nlohmann/json.hpp"


#include "../model/model.cpp"

using namespace httplib;

using json = nlohmann::json;

std::ifstream ctrl_f("./config.json");
json ctrl_config_data = json::parse(ctrl_f);

std::string VIEW_ROOT = ctrl_config_data["VIEW_ROOT"];


void logCurrentTime(std::string identity){

    std::stringstream time_stream;
    
    auto now = std::chrono::system_clock::now();

    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    time_stream << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S");

    std::string std_time_now = time_stream.str();

    std::cout << std_time_now + ": " + identity << std::endl;

};



std::string getFileContent(std::string file_path){


    std::ifstream in_file(file_path);
    std::stringstream buffer;
    buffer << in_file.rdbuf();


    if (!in_file){

        return "Path Not Available";

    }

    std::string content = buffer.str();

    return content;
}




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

        
        RecordText record_data = mdl.GetRecordByCmd();

        std::string ret_data = "";

        ret_data += "| " + record_data.first + " | " + record_data.second + " | " +record_data.third + " |";


        res.set_content(ret_data,"text/plain");


        logCurrentTime("[ /get-records ]");




    }



};