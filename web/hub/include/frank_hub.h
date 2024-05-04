#ifndef _FRANK_HUB_H_
#define _FRANK_HUB_H_


#include <csignal>
#include <iostream>

#include <websocketpp/server.hpp>
#include <boost/asio.hpp>

#include <websocketpp/config/asio.hpp>

#include <fstream>

#include <future>
#include <unistd.h>     

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>


#include <stdlib.h>
#include <iostream>
#include <vector>

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>


using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using server = websocketpp::server<websocketpp::config::asio_tls>;
using ssl_context = websocketpp::lib::asio::ssl::context;

using server_plain =  websocketpp::server<websocketpp::config::asio>;




#endif