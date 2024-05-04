
#ifndef _FRANK_HUB_SOCK_H_
#define _FRANK_HUB_SOCK_H_


#include "frank_hub.h"




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






extern std::map<std::string, void*> VERIFIED_SOCK_CONNS;

extern std::map<std::string, void*> VERIFIED_FRONT_CONNS;

extern std::map<void*, std::string> VERIFIED_SOCK_CONNS_OWNER;

extern std::map<void*, std::string> VERIFIED_FRONT_CONNS_OWNER;

extern std::map<void*, connection_hdl> VERIFIED_ADDRCONN_MAPPER;

extern std::map<server*, server_plain*> SOCK_TO_FRONT;

extern std::map<server_plain*, server*> FRONT_TO_SOCK;

extern std::future<std::string> AWAIT;

extern std::string DB_ADDRESS;




void on_message_plain(server_plain* hub_plain, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg);




void on_message(server* hub, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg);



void on_open(std::vector<connection_hdl>* connections, connection_hdl hdl);


void on_close_plain(server_plain* hub_plain, std::vector<connection_hdl>* connections,
              connection_hdl hdl);

void turn_off_logging(server& hub);


void set_message_handler(server& hub,
                         std::vector<connection_hdl>& connections);




void set_message_handler_plain(server_plain& hub_plain,
                         std::vector<connection_hdl>& connections);



void set_open_handler(server& hub, std::vector<connection_hdl>& connections);



void set_open_handler_plain(server_plain& hub_plain, std::vector<connection_hdl>& connections);


void set_close_handler(server& hub,
                       std::vector<connection_hdl>& connections);


void set_close_handler_plain(server_plain& hub_plain,
                       std::vector<connection_hdl>& connections);


websocketpp::lib::shared_ptr<ssl_context> on_tls_init(connection_hdl hdl);


void set_tls_init_handler(server& hub);




void on_close(server* hub, std::vector<connection_hdl>* connections,
              connection_hdl hdl);




bool equal_connection_hdl(connection_hdl& a, connection_hdl& b);

void unregister_verified_connections(connection_hdl& hdl);

void remove_connection(server* hub, std::vector<connection_hdl>* connections,
                       connection_hdl& hdl);

void remove_connection_plain(server_plain* hub_plain, std::vector<connection_hdl>* connections,
                       connection_hdl& hdl);


void remove_closing_connection(std::vector<connection_hdl>* connections,
                       connection_hdl& hdl);

void register_verified_connections_sock(server* hub,
                                   std::vector<connection_hdl>* connections,
                                   connection_hdl& hdl, 
                                   std::string owner);


void register_verified_connections_front(server_plain* hub_plain,
                                   std::vector<connection_hdl>* connections,
                                   connection_hdl& hdl, 
                                   std::string owner);

void send_message(server* hub, connection_hdl* connection, std::string msg);


void send_message_plain(server_plain* hub_plain, connection_hdl* connection, std::string msg);


void sock_to_front_handler(server* hub, connection_hdl* connection, std::string msg);


void front_to_sock_handler(server_plain* hub_plain, connection_hdl* connection, std::string msg);

std::string extract_common_name(const char* cert);


void ph_sock_client(server* hub, 
                    std::vector<connection_hdl>* connections,
                    connection_hdl hdl,
                    websocketpp::config::asio::message_type::ptr msg);




std::string async_callback_test(std::string words);


void ph_sock_client_test(server* hub, 
                         std::vector<connection_hdl>* connections,
                         connection_hdl hdl,
                         websocketpp::config::asio::message_type::ptr msg);



void ph_front_client(server_plain* hub_plain, 
                     std::vector<connection_hdl>* connections,
                     connection_hdl hdl,
                     websocketpp::config::asio::message_type::ptr msg);


void ph_front_client_test (server_plain* hub_plain, 
                           std::vector<connection_hdl>* connections,
                           connection_hdl hdl,
                           websocketpp::config::asio::message_type::ptr msg);




int sig_verify(const char* cert_pem, const char* intermediate_pem);


void cert_info(const char* cert_pem);


std::string check_if_verified_conn(std::map<void*, std::string>* connections,
                            connection_hdl hdl);


std::string authenticate_request(websocketpp::config::asio::message_type::ptr msg);

int test_verify();

int test_get_sn();


DBResult<FrankRecord> get_record_by_email(std::string email);


DBResult<FrankRecord> get_record_by_pkey(std::string pkey);




#endif