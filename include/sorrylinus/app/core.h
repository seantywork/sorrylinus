#ifndef _SORRYLINUS_APP_H_
#define _SORRYLINUS_APP_H_


#include "sorrylinus/soli.h"

#include <sqlite3.h> 

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <jsonlib/json.hpp>

#include <fstream>
#include <string>
#include <iostream>



using json = nlohmann::json;

using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;


using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using ssl_context = websocketpp::lib::asio::ssl::context;



struct DB_Result {

    int Status;
    int RecordCount = 0;
    std::vector<std::map<std::string,std::string>> Result; 

};


extern std::ifstream app_f;


extern json app_config_data;


extern std::string SOCK_LOCAL_ADDR;

extern std::string SOCK_FEEB_ADDR;




void send_message(client* sock, connection_hdl* connection, std::string msg);


void close_connection(client* sock, connection_hdl* connection);


void on_message(client* client, connection_hdl hdl,
                websocketpp::config::asio_client::message_type::ptr msg);


void on_open(client* sock, connection_hdl* connection ,connection_hdl hdl);


void turn_off_logging(client& sock);


void set_message_handler(client& sock);

void set_open_handler(client& sock, connection_hdl* connection);


void set_url(client& sock, std::string url);

bool verify_certificate(client* sock, connection_hdl* connection,bool preverified, boost::asio::ssl::verify_context& ctx);

websocketpp::lib::shared_ptr<ssl_context> on_tls_init(client* sock, connection_hdl* connection);

void set_tls_init_handler(client* sock, connection_hdl* connection);


void run_sock_client(std::string remote_addr);

void run_sock_client_interactive(std::string remote_addr);



class Model {


public:


    DB_Result GetRecordByCmd();



};

int GetRecordByCmd_callback(void* data, int argc, char** argv, char** azColName);



std::string mod_query(std::string in_message);




#endif