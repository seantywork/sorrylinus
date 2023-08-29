
#include <csignal>
#include <iostream>
//#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>

#include <websocketpp/config/asio.hpp>

#include <fstream>

#include <future>
#include <unistd.h>     

using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using server = websocketpp::server<websocketpp::config::asio_tls>;
using ssl_context = websocketpp::lib::asio::ssl::context;


std::vector<connection_hdl> VERIFIED_SOCK_CONNS;

std::vector<connection_hdl> VERIFIED_FRONT_CONNS;


bool equal_connection_hdl(connection_hdl& a, connection_hdl& b) {

  return a.lock() == b.lock();
}

void remove_connection(server* hub, std::vector<connection_hdl>* connections,
                       connection_hdl& hdl) {
  auto equal_connection_hdl_predicate =
      std::bind(&equal_connection_hdl, hdl, ::_1);

  
  connections->erase(
      std::remove_if(std::begin(*connections), std::end(*connections),
                     equal_connection_hdl_predicate),
      std::end(*connections));

  VERIFIED_SOCK_CONNS.erase(
      std::remove_if(std::begin(VERIFIED_SOCK_CONNS), std::end(VERIFIED_SOCK_CONNS),
                     equal_connection_hdl_predicate),
      std::end(VERIFIED_SOCK_CONNS));

  VERIFIED_FRONT_CONNS.erase(
      std::remove_if(std::begin(VERIFIED_FRONT_CONNS), std::end(VERIFIED_FRONT_CONNS),
                     equal_connection_hdl_predicate),
      std::end(VERIFIED_FRONT_CONNS));


  std::cout << "connection evicted"  << std::endl;

  std::cout << "check:" << hdl.lock().get()  << std::endl;

  hub->close(hdl, websocketpp::close::status::normal, "done");


  std::cout << "connection destroyed"  << std::endl;
}

void remove_closing_connection(std::vector<connection_hdl>* connections,
                       connection_hdl& hdl) {
  auto equal_connection_hdl_predicate =
      std::bind(&equal_connection_hdl, hdl, ::_1);

  
  connections->erase(
      std::remove_if(std::begin(*connections), std::end(*connections),
                     equal_connection_hdl_predicate),
      std::end(*connections));

  VERIFIED_SOCK_CONNS.erase(
      std::remove_if(std::begin(VERIFIED_SOCK_CONNS), std::end(VERIFIED_SOCK_CONNS),
                     equal_connection_hdl_predicate),
      std::end(VERIFIED_SOCK_CONNS));

  VERIFIED_FRONT_CONNS.erase(
      std::remove_if(std::begin(VERIFIED_FRONT_CONNS), std::end(VERIFIED_FRONT_CONNS),
                     equal_connection_hdl_predicate),
      std::end(VERIFIED_FRONT_CONNS));



  std::cout << "connection evicted for closing"  << std::endl;
}


void send_message(server* hub, connection_hdl* connection, std::string msg) {
  hub->send(*connection, msg, websocketpp::frame::opcode::text);
}




