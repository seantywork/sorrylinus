#include "frank_cc/server.h"

std::ifstream app_o("./ogapi.json");
json app_oauth_data = json::parse(app_o);

std::string DEBUG_MODE = app_config_data["DEBUG_MODE"];
std::string VIEW_ROOT = app_config_data["VIEW_ROOT"];
std::string LOCAL_URL = app_config_data["LOCAL_URL"];
std::string REMOTE_URL = app_config_data["REMOTE_URL"];

std::string GOOGLE_CLIENT_ID = app_oauth_data["web"]["client_id"];
std::string GOOGLE_CLIENT_SECRET = app_oauth_data["web"]["client_secret"];
std::string GOOGLE_AUTH_ENDPOINT = app_oauth_data["web"]["auth_uri"];
std::string GOOGLE_TOKEN_ENDPOINT = app_oauth_data["web"]["token_uri"];
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


std::string get_cookie_val(std::string line, std::string delimiter){

    
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


std::string get_file_content(std::string file_path){


    std::ifstream in_file(file_path);
    std::stringstream buffer;
    buffer << in_file.rdbuf();


    if (!in_file){

        return "Path Not Available";

    }

    std::string content = buffer.str();

    return content;
}

void log_current_time(std::string identity){

    std::stringstream time_stream;
    
    auto now = std::chrono::system_clock::now();

    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    time_stream << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S");

    std::string std_time_now = time_stream.str();

    std::cout << std_time_now + ": " + identity << std::endl;

};


void set_session_status(Response &res,std::string key, std::string val){

    std::string cookie = key + "=" + val;

    res.set_header("Set-Cookie",cookie);

    std::cout<< "set cookie:" << val << std::endl;
}


std::string get_session_status(const Request &req){

    std::string sess_stat = "N";


    size_t cookie_count = req.get_header_value_count("Cookie");

    if(cookie_count != 1){
        std::cout<< "wrong cookie count:" << cookie_count << std::endl;

        return sess_stat;
    }

    std::string cookie_line = req.get_header_value("Cookie");

    if(cookie_line.find("SID") != std::string::npos){
        
        std::string cookie_val = get_cookie_val(cookie_line,"=");

        sess_stat = cookie_val;
    } else {

        std::cout<< "cookie not found:" << cookie_line << std::endl;
    }



    return sess_stat;
}


size_t req_write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string request_post_code(std::string req_url, std::string code){

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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_write_callback);
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

std::string request_get_url(std::string req_url){

    //const char* c_req_url = req_url.c_str();

    CURL *curl;
    CURLcode res;

    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, req_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_write_callback);
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
