#ifndef _FRANK_CC_CORE_H_
#define _FRANK_CC_CORE_H_

#include "frank_cc.h"

struct FrankRecord {

    std::string email;
    std::string f_session;
    std::string p_key;
};


template <typename T>
struct DBResult{

    std::string status;
    
    std::vector<T> results; 

};




extern std::ifstream app_f;
extern std::ifstream app_o;
extern json app_config_data;
extern json app_oauth_data;
extern std::string ADDRESS;
extern int PORT;

extern std::string PUBLIC_ROOT;

extern std::string DEBUG_MODE;
extern std::string VIEW_ROOT;
extern std::string LOCAL_URL;
extern std::string REMOTE_URL;

extern std::string GOOGLE_CLIENT_ID;
extern std::string GOOGLE_CLIENT_SECRET;
extern std::string GOOGLE_AUTH_ENDPOINT;
extern std::string GOOGLE_TOKEN_ENDPOINT;
extern std::string OAUTH_REDIRECT_URI;


extern std::string USER_INFO_API;


extern std::string DB_ADDRESS;




class Controller {

public:


    static void GetIndex(const Request &req, Response &res);

    static void TestFrankHealth(const Request &req, Response &res);

    static void GoogleOAuthLogin(const Request &req, Response &res);

    static void GoogleOAuthCallback(const Request &req, Response &res);

    static void SignOut(const Request &req, Response &res);

    

};


class Router {


public:

    Server *srv;
    Controller ctrl;
    Router(Server *app);
    void Init();

};




std::string hex_gen(int key_size);
bool template_replace(std::string& str, const std::string& from, const std::string& to);

std::string get_cookie_val(std::string line, std::string delimiter);

std::string get_file_content(std::string file_path);

void log_current_time(std::string identity);

void set_session_status(Response &res,std::string key, std::string val);

std::string get_session_status(const Request &req);

size_t req_write_callback(void *contents, size_t size, size_t nmemb, void *userp);

std::string request_post_code(std::string req_url, std::string code);

std::string request_get_url(std::string req_url);


DBResult<FrankRecord> get_record_by_fsession(std::string fsession);


DBResult<FrankRecord> set_record_by_email(std::string user_email, std::string fsession, std::string pkey);

DBResult<FrankRecord> unset_record_by_email(std::string user_email);


#endif