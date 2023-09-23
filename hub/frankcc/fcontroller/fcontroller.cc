#ifndef FRANK_PREPROCESSOR_HEADER 
#define FRANK_PREPROCESSOR_HEADER 
#include <httplib/httplib.h>
#include <jsonlib/json.hpp>
#endif


#include <time.h>
#include <curl/curl.h>
#include "../fmodels/fmodels.h"

using namespace httplib;

using json = nlohmann::json;

std::ifstream ctrl_f("./config.json");
json ctrl_config_data = json::parse(ctrl_f);

std::ifstream ctrl_o("./ogapi.json");
json ctrl_oauth_data = json::parse(ctrl_o);

std::string DEBUG_MODE = ctrl_config_data["DEBUG_MODE"];
std::string VIEW_ROOT = ctrl_config_data["VIEW_ROOT"];
std::string LOCAL_URL = ctrl_config_data["LOCAL_URL"];
std::string REMOTE_URL = ctrl_config_data["REMOTE_URL"];

std::string GOOGLE_CLIENT_ID = ctrl_oauth_data["web"]["client_id"];
std::string GOOGLE_CLIENT_SECRET = ctrl_oauth_data["web"]["client_secret"];
std::string GOOGLE_AUTH_ENDPOINT = ctrl_oauth_data["web"]["auth_uri"];
std::string GOOGLE_TOKEN_ENDPOINT = ctrl_oauth_data["web"]["token_uri"];
std::string OAUTH_REDIRECT_URI = "";


std::string USER_INFO_API = "https://www.googleapis.com/oauth2/v2/userinfo?access_token=";



std::string hex_gen(int key_size) {

    std::random_device rd;
    std::mt19937 gen(rd());
    auto curr_clock = std::chrono::high_resolution_clock::now();
    std::seed_seq sseq {
        std::chrono::duration_cast<std::chrono::hours> (curr_clock.time_since_epoch()).count(), 
        std::chrono::duration_cast<std::chrono::seconds> (curr_clock.time_since_epoch()).count()
        };
    
    gen.seed(sseq);
    
    std::uniform_int_distribution<int> dis(0x0, 0xF);

    std::string hex_code = "";
    
    for (std::size_t i = 0; i < key_size; ++i) {
	
        std::stringstream stream;
        stream << std::hex << dis(gen);
        std::string result( stream.str() );

        hex_code += result;

    }

    
    return hex_code;
}

bool template_replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


std::string getCookieVal(std::string line, std::string delimiter){

    
    std::string key = line.substr(0, line.find(delimiter));

    std::string val = line.substr(line.find(delimiter)+1, line.length()-1);

    if(key != "SID"){
        return "N";
    } 

    if(val == ""){
        return "N";
    }

    std::cout << "cookie val: " << val << std::endl;

    return val;

}


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

void logCurrentTime(std::string identity){

    std::stringstream time_stream;
    
    auto now = std::chrono::system_clock::now();

    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    time_stream << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S");

    std::string std_time_now = time_stream.str();

    std::cout << std_time_now + ": " + identity << std::endl;

};


void setSessionStatus(Response &res,std::string key, std::string val){

    std::string cookie = key + "=" + val;

    res.set_header("Set-Cookie",cookie);

    std::cout<< "set cookie:" << val << std::endl;
}


std::string getSessionStatus(const Request &req){

    std::string sess_stat = "N";


    size_t cookie_count = req.get_header_value_count("Cookie");

    if(cookie_count != 1){
        std::cout<< "wrong cookie count:" << cookie_count << std::endl;

        return sess_stat;
    }

    std::string cookie_line = req.get_header_value("Cookie");

    if(cookie_line.find("SID") != std::string::npos){
        
        std::string cookie_val = getCookieVal(cookie_line,"=");

        sess_stat = cookie_val;
    } else {

        std::cout<< "cookie not found:" << cookie_line << std::endl;
    }



    return sess_stat;
}


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string RequestPostCode(std::string req_url, std::string code){

    //const char* c_req_url = req_url.c_str();

    CURL *curl;
    CURLcode res;

    std::string post_fields = "";
    std::string redirect_uri = "";
    std::string readBuffer;


    post_fields += "code=" + code + "&";
    post_fields += "client_id=" + GOOGLE_CLIENT_ID + "&";
    post_fields += "client_secret=" + GOOGLE_CLIENT_SECRET + "&";
    post_fields += "redirect_uri=" + OAUTH_REDIRECT_URI + "&";
    post_fields += "grant_type=authorization_code";

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, req_url.c_str());
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_fields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);

    } else {
        readBuffer = "curl init failed";
    }


    return readBuffer;
}

std::string RequestGetUrl(std::string req_url){

    //const char* c_req_url = req_url.c_str();

    CURL *curl;
    CURLcode res;

    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, req_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);

    } else {
        readBuffer = "curl init failed";
    }


    return readBuffer;
}



class Controller {

public:


    static void GetIndex(const Request &req, Response &res){

        std::string sid = getSessionStatus(req);

        int auth = 0;

        std::string req_key = "";

        if (sid != "N"){

            DBResult<FrankRecord> q_res = get_record_by_fsession(sid);

            if (q_res.status == "SUCCESS"){

                req_key = q_res.results[0].p_key;

                auth = 1;
            }
        } else {
            std::string sid = hex_gen(32);
            setSessionStatus(res,"SID",sid);
        }

        std::string index_path = VIEW_ROOT;

        if (auth == 0){

            index_path += "index.html";

        } else if (auth == 1){   

            index_path += "frank.html";

        }

        std::string content = getFileContent(index_path);
        template_replace(content, "{{%.REQ_KEY%}}",req_key);
        std::string type = "text/html";


        res.set_content(content,type);

        logCurrentTime("[ / ]");


    
    }

    static void TestFrankHealth(const Request &req, Response &res){



    }

    static void GoogleOAuthLogin(const Request &req, Response &res){

        std::string request_uri = "";

        if(DEBUG_MODE == "TRUE"){
            OAUTH_REDIRECT_URI = ctrl_oauth_data["web"]["redirect_uris"][0];
        }else{
            OAUTH_REDIRECT_URI = ctrl_oauth_data["web"]["redirect_uris"][1];
        }

        request_uri += GOOGLE_AUTH_ENDPOINT + "?";
        request_uri += "client_id=" + GOOGLE_CLIENT_ID + "&";
        request_uri += "redirect_uri=" + OAUTH_REDIRECT_URI + "&";
        request_uri += "scope=https://www.googleapis.com/auth/userinfo.email&";
        request_uri += "response_type=code";

        res.set_redirect(request_uri);

        logCurrentTime("[ /oauth login ]");
    }

    static void GoogleOAuthCallback(const Request &req, Response &res){

        std::string sid = getSessionStatus(req);

        if (sid == "N"){
            res.set_redirect("/");
        }

        try{

            std::string code_string = req.get_param_value("code");

            std::string content = RequestPostCode(GOOGLE_TOKEN_ENDPOINT,code_string);
            
            json OADATA = json::parse(content);

            std::string access_token = OADATA["access_token"];

            std::string get_userinfo = USER_INFO_API + access_token;

            std::string result = RequestGetUrl(get_userinfo);

            json UIDATA = json::parse(result);

            std::string email = UIDATA["email"];

            std::cout << email << std::endl;

            std::string pkey = hex_gen(32);

            DBResult<FrankRecord> q_res = set_record_by_email(email, sid, pkey);

            if (q_res.status == "SUCCESS"){

                res.set_redirect("/");


            } else{

                std::cout << q_res.status << std::endl;

                res.set_content("failed setting record","text/plain");

            }


        } catch(...){
            
            std::cout << "error callback" << std::endl;

            res.set_redirect("/");

            return;
        }


        logCurrentTime("[ /oauth callback ]");
    }

/*
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

*/

};