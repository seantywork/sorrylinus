#ifndef FRANK_PREPROCESSOR_HEADER 
#define FRANK_PREPROCESSOR_HEADER 
#include <httplib/httplib.h>
#include <jsonlib/json.hpp>
#endif

#include "./fcontroller_mod.h"
#include "../fmodels/fmodels.h"



class Controller {

public:


    static void GetIndex(const Request &req, Response &res){

        std::string sid = get_session_status(req);

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
            set_session_status(res,"SID",sid);
        }

        std::string index_path = VIEW_ROOT;

        if (auth == 0){

            index_path += "index.html";

        } else if (auth == 1){   

            index_path += "frank.html";

        }

        std::string content = get_file_content(index_path);
        template_replace(content, "{{%.REQ_KEY%}}",req_key);
        std::string type = "text/html";


        res.set_content(content,type);

        log_current_time("[ / ]");


    
    }

    static void TestFrankHealth(const Request &req, Response &res){

        std::string ret = "{\"message\":\"NotFine\"}";

        std::string sid = get_session_status(req);

        int auth = 0;

        if (sid != "N"){

            DBResult<FrankRecord> q_res = get_record_by_fsession(sid);

            if (q_res.status == "SUCCESS"){

                auth = 1;
            }
        }


        if (auth != 1){

            res.set_content(ret,"application/json");

        } else {

            ret = "{\"message\":\"Fine\"}";

            res.set_content(ret,"application/json");

        }

        log_current_time("[ /frank test ]");

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

        log_current_time("[ /oauth login ]");
    }

    static void GoogleOAuthCallback(const Request &req, Response &res){

        std::string sid = get_session_status(req);

        if (sid == "N"){
            res.set_redirect("/");
        }

        try{

            std::string code_string = req.get_param_value("code");

            std::string content = request_post_code(GOOGLE_TOKEN_ENDPOINT,code_string);
            
            json OADATA = json::parse(content);

            std::string access_token = OADATA["access_token"];

            std::string get_userinfo = USER_INFO_API + access_token;

            std::string result = request_get_url(get_userinfo);

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


        log_current_time("[ /oauth callback ]");
    }

    static void SignOut(const Request &req, Response &res){


        std::string ret = "{\"message\":\"FAIL\"}";

        std::string sid = get_session_status(req);

        int auth = 0;

        DBResult<FrankRecord> q_res;

        if (sid != "N"){

            q_res = get_record_by_fsession(sid);

            if (q_res.status == "SUCCESS"){

                auth = 1;
            }
        }


        if (auth != 1){

            res.set_content(ret,"application/json");

        } else {

            DBResult<FrankRecord> q_res_unset = unset_record_by_email(q_res.results[0].email);            

            ret = "{\"message\":\""+ q_res_unset.status +"\"}";

            res.set_content(ret,"application/json");

        }

        log_current_time("[ /frank signout ]");


    }

};