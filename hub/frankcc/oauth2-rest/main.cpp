/* Original Work Copyright (c) 2020 Giuseppe Baccini - giuseppe.baccini@live.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <ctime>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "properties_file_reader.h"
#include "json/json.h"

#define GET_PROPERTY(from, p) from(p)[0]



#include <sstream>

 
std::string urlencode(const std::string &s)
{
    static const char lookup[]= "0123456789abcdef";
    std::stringstream e;
    for(int i=0, ix=s.length(); i<ix; i++)
    {
        const char& c = s[i];
        if ( (48 <= c && c <= 57) ||//0-9
             (65 <= c && c <= 90) ||//abc...xyz
             (97 <= c && c <= 122) || //ABC...XYZ
             (c=='-' || c=='_' || c=='.' || c=='~') 
        )
        {
            e << c;
        }
        else
        {
            e << '%';
            e << lookup[ (c&0xF0)>>4 ];
            e << lookup[ (c&0x0F) ];
        }
    }
    return e.str();
}
/** rest_client
 *
 *  rest_client provides basic client functionalities to call REST API(s)
 *  based on HTTP protocol using OAuth2 like authentication.
 *  Please note that OAuth2 support in this example is severely limited and do not cover
 *  the full specification of the protocol.
 *  rest_client is intended to be a didactical example and should not be used as production code.
 *  For reasons of brevity and clarity only get() - performing HTTP GET - method
 *  has been implemented in rest_client.
 *  However, you can easily add missing HTTP methods along the lines of get() method.
 *  rest_client has been developed for the usage with Google API.
 *  You can start hacking with Google API at: https://console.developers.google.com/.
 */
struct rest_client {

        //ctor
        rest_client(std::string &&properties_file_name)
            : cfg_(std::move(properties_file_name)) {
            std::ifstream refresh_token_ifs("refresh_token");
            if(!refresh_token_ifs.bad()) {
                refresh_token_ifs >> refresh_token_;
            }
        }

        const utils::PropertiesFileReader &get_cfg() {
            return cfg_;
        }

        int init() {

            //we create authentication related connection object

            auth_conn_.reset(new RestClient::Connection(GET_PROPERTY(cfg_, "token_server")));
            if(auth_conn_) {
                std::cout << "auth_conn_ object created" << std::endl;
            } else {
                std::cerr << "failed creating auth_conn_ object" << std::endl;
                return -1;
            }

            //we setup authentication related connection object
            auth_conn_->SetTimeout(30);
            RestClient::HeaderFields authHeaderFields;
            authHeaderFields["Content-Type"] = std::string("application/x-www-form-urlencoded");
            auth_conn_->SetHeaders(authHeaderFields);


            //we create resource related connection object
            resource_conn_.reset(new RestClient::Connection(GET_PROPERTY(cfg_, "rest_api_url")));
            if(resource_conn_) {
                std::cout << "resource_conn_ object created" << std::endl;
            } else {
                std::cerr << "failed creating resource_conn_ object" << std::endl;
                return -1;
            }

            //we setup resource related connection object
            resource_conn_->SetTimeout(30);

            //all ok
            return 0;
        }

        /** get
         *
         *  Performs an HTTP GET call over resource connection using an optional parameter: query_string.
         *  You can pass to the second parameter: cb, an argument denoting a lambda that takes the response
         *  of the HTTP GET call.
         */
        template<typename callback>
        int get(const std::string &query_string, callback cb) {
            if(refresh_access_token_if_expired()) {
                std::cerr << "failed to get authentication token, aborting GET call..." << std::endl;
                return -1;
            }

            std::string uri("/");
            uri += GET_PROPERTY(cfg_, "rest_api_uri");
            if(query_string != "") {
                uri += '?';
                uri += query_string;
            }

            RestClient::HeaderFields reqHF;
            reqHF["Authorization"] = access_token_type_ + ' ' +  access_token_;
            resource_conn_->SetHeaders(reqHF);

            RestClient::Response res;
            res = resource_conn_->get(uri);

            if(res.code != 200) {
                std::cout << "warning: call result code was:" << res.code << std::endl;
            }

            cb(res);
            return res.code;
        }

    private:

        int refresh_access_token_if_expired() {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            int res = 0;

            //if the authentication token has been expired or if it will expire in less than 30 seconds we request a brand new.
            if((now > access_token_expire_tp_) || ((access_token_expire_tp_ - now) < std::chrono::duration<int>(30))) {
                if(refresh_token_ == "") {
                    //we must request the refresh_token; this can be done only once for a given code.
                    std::cout << "sending confirm permissions request for the given code:" << GET_PROPERTY(cfg_, "code") << std::endl;
                    res = post_confirm_permissions_request();
                } else {
                    //we already have got a refresh_token, we must use that in order to refresh the authentication token.
                    std::cout << "sending refresh access token request" << std::endl;
                    res = post_refresh_access_token_request();
                }
            } else {
                std::cout << "current access token is valid" << std::endl;
            }

            return res;
        }

        int post_confirm_permissions_request() {
            std::string post_data = build_confirm_permissions_request();

            std::string post_data_enc = urlencode(post_data);

            RestClient::Response rest_res = auth_conn_->post("",post_data_enc);
            if(rest_res.code == 200) {
                return process_confirm_permissions_response(rest_res);
            } else {
                std::cerr << "error getting refresh token and access token, http-code:" << rest_res.code << std::endl;
                std::cerr << "http-body:" << rest_res.body << std::endl;
                return -1;
            }
        }

        int post_refresh_access_token_request() {
            std::string post_data = build_refresh_access_token_request();
            RestClient::Response rest_res = auth_conn_->post("", post_data);
            if(rest_res.code == 200) {
                return process_refresh_access_token_response(rest_res);
            } else {
                std::cerr << "error refreshing access token, http-code:" << rest_res.code << std::endl;
                std::cerr << "http-body:" << rest_res.body << std::endl;
                return -1;
            }
        }

        std::string build_confirm_permissions_request() {
            std::ostringstream os;
            os << "code="           << GET_PROPERTY(cfg_, "code")             << '&'
               << "client_id="      << GET_PROPERTY(cfg_, "client_id")        << '&'
               << "client_secret="  << GET_PROPERTY(cfg_, "client_secret")    << '&'
               << "redirect_uri=http://localhost:8000/oauth2/google/callback" << '&'           
               << "grant_type=authorization_code";
            return os.str();
        }

        std::string build_refresh_access_token_request() {
            std::ostringstream os;
            os << "client_id="      << GET_PROPERTY(cfg_, "client_id")        << '&'
               << "client_secret="  << GET_PROPERTY(cfg_, "client_secret")    << '&'
               << "refresh_token="  << refresh_token_ << '&'
               << "grant_type=refresh_token";
            return os.str();
        }

        int process_confirm_permissions_response(const RestClient::Response &response) {
            /*
                example of response:

                {
                    "access_token": "...",
                    "expires_in": 3599,
                    "refresh_token": "...",
                    "scope": "https://www.googleapis.com/auth/drive.readonly",
                    "token_type": "Bearer"
                }
            */

            Json::Value auth_res;
            std::istringstream istr(response.body);
            istr >> auth_res;
            refresh_token_ = auth_res.get("refresh_token", "").asString();
            access_token_ = auth_res.get("access_token", "").asString();
            access_token_type_ = auth_res.get("token_type", "").asString();
            access_token_expire_tp_ = std::chrono::system_clock::now() +
                                    std::chrono::duration<int>(auth_res.get("expires_in", 0U).asUInt());

            //flush refresh token to "refresh_token" file so it can be reused in future runs
            std::ofstream refresh_token_ofs("refresh_token", std::ofstream::trunc);
            refresh_token_ofs << refresh_token_;

            std::cout << "confirm permissions response:" << std::endl << auth_res;

            return 0;
        }

        int process_refresh_access_token_response(const RestClient::Response &response) {
            /*
                example of response:

                {
                    "access_token": "...",
                    "expires_in": 3599,
                    "scope": "https://www.googleapis.com/auth/drive.readonly",
                    "token_type": "Bearer"
                }
            */

            Json::Value auth_res;
            std::istringstream istr(response.body);
            istr >> auth_res;
            access_token_ = auth_res.get("access_token", "").asString();
            access_token_type_ = auth_res.get("token_type", "").asString();
            access_token_expire_tp_ = std::chrono::system_clock::now() +
                                    std::chrono::duration<int>(auth_res.get("expires_in", 0U).asUInt());

            std::cout << "refresh access token response:" << std::endl << auth_res << std::endl;
            std::time_t tt = std::chrono::system_clock::to_time_t(access_token_expire_tp_);
            std::cout << "access token will expire at:" << ctime(&tt);

            return 0;
        }

        utils::PropertiesFileReader cfg_;
        std::unique_ptr<RestClient::Connection> auth_conn_;
        std::unique_ptr<RestClient::Connection> resource_conn_;
        std::string refresh_token_;                                     //refresh token
        std::string access_token_;                                      //access token
        std::string access_token_type_;                                 //access token type (Bearer)
        std::chrono::system_clock::time_point access_token_expire_tp_;  //access token expiring time-point
};

int main()
{

    //we create a rest client
    rest_client rc("rc.properties");

    //we init it
    if(rc.init()) {
        std::cerr << "error init rest_client, exiting..." << std::endl;
        return 1;
    }

    //the optional query string to apply to the REST API call
    std::string query_string("key=");
    query_string += GET_PROPERTY(rc.get_cfg(), "api_key");

    //the response in json format from the REST API
    Json::Value json_response;

    //the HTTP GET call for the REST API
    int http_res = rc.get(query_string, [&](RestClient::Response &get_res) -> int {
        std::istringstream istr(get_res.body);
        istr >> json_response;
        return 0;
    });

    if(http_res != 200) {
        std::cerr << "call to REST API failed, exiting..." << std::endl;
        return 1;
    } else {
        std::cout << "call to REST API was successful, dumping response to file..." << std::endl;
    }

    //flush REST API response to file named: GET_response
    std::ofstream GET_response_ofs("GET_response", std::ofstream::trunc);
    GET_response_ofs << json_response;

    return 0;
}
