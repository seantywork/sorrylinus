#include <csignal>
#include <iostream>
//#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>

#include <websocketpp/config/asio.hpp>

#include <fstream>

#include "./peer_verification.cc"


//using server = websocketpp::server<websocketpp::config::asio>;
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

bool check_if_verified_conn(std::vector<connection_hdl>* connections,
                            connection_hdl hdl){

  bool status = false;

  for (auto& connection : *connections) {
    if (connection.lock() == hdl.lock()) {
      status = true;
    }
  }

  std::cout << "verified connections: " << connections->size() << std::endl;
  std::cout << "verified status: " << status << std::endl;
  return status;
}

bool authenticate_request(websocketpp::config::asio::message_type::ptr msg){


  bool result = false;

  std::string payload = msg->get_payload();

  const char * cert_pem = payload.c_str();


  std::ifstream in_cert("tls/ca.crt");
  std::stringstream buffer;

  std::string ca_payload;

  buffer << in_cert.rdbuf();

  ca_payload = buffer.str();

  const char * intermediate_pem = ca_payload.c_str();

  int v = sig_verify(cert_pem, intermediate_pem);

  if(v >= 1){
    result = true;
  }
  return result;
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

void on_message(server* hub, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg) {

  server::connection_ptr con = hub->get_con_from_hdl(hdl);
  std::string path = con->get_resource();

  std::cout << "path: " << path << std::endl;

  if (path == "/sock-client"){

    bool verification_status = check_if_verified_conn(&VERIFIED_SOCK_CONNS, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    if (!verification_status){

      bool result = authenticate_request(msg);
      
      if (!result){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection(hub, connections, hdl);

      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        VERIFIED_SOCK_CONNS.push_back(hdl);    
      }  

      return;
    }

    std::cout << "on_message: " << msg->get_payload() << std::endl;  

  } else if (path == "/front-client"){

    bool verification_status = check_if_verified_conn(&VERIFIED_FRONT_CONNS, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    if (!verification_status){

      bool result = authenticate_request(msg);
      
      if (!result){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection(hub, connections, hdl);
    
      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        VERIFIED_FRONT_CONNS.push_back(hdl);    
      }  

      return;
    }

    std::cout << "on_message: " << msg->get_payload() << std::endl;  

  } else {

    remove_connection(hub, connections, hdl);
    return;

  }


}

void on_open(std::vector<connection_hdl>* connections, connection_hdl hdl) {
  std::cout << "on_open: add tmp connection" << std::endl;

  connections->push_back(hdl);
  std::cout << "connections: " << connections->size() << std::endl;
}

void on_close(server* hub, std::vector<connection_hdl>* connections,
              connection_hdl hdl) {
  std::cout << "on_close" << std::endl;
  remove_closing_connection(connections, hdl);
  std::cout << "connections: " << connections->size() << std::endl;
}

void turn_off_logging(server& hub) {
  hub.clear_access_channels(websocketpp::log::alevel::all);
  hub.clear_error_channels(websocketpp::log::elevel::all);
}

void set_message_handler(server& hub,
                         std::vector<connection_hdl>& connections) {
  hub.set_message_handler(
      websocketpp::lib::bind(&on_message, &hub, &connections, ::_1, ::_2));
}

void set_open_handler(server& hub, std::vector<connection_hdl>& connections) {
  hub.set_open_handler(websocketpp::lib::bind(&on_open, &connections, ::_1));
}

void set_close_handler(server& hub,
                       std::vector<connection_hdl>& connections) {
  hub.set_close_handler(
      websocketpp::lib::bind(&on_close, &hub, &connections, ::_1));
}


websocketpp::lib::shared_ptr<ssl_context> on_tls_init(connection_hdl hdl) {
  auto ctx = websocketpp::lib::make_shared<ssl_context>(ssl_context::sslv23);

  std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;

  ctx->use_certificate_chain_file("tls/sub.crt");
  ctx->use_private_key_file("tls/sub_priv.pem", ssl_context::pem);

  return ctx;
}


void set_tls_init_handler(server& hub) {
  hub.set_tls_init_handler(websocketpp::lib::bind(&on_tls_init, ::_1));
}


int main() {
  server hub;
  std::vector<connection_hdl> connections;

  //turn_off_logging(hub);

  hub.init_asio();

  set_message_handler(hub, connections);
  set_tls_init_handler(hub);
  set_open_handler(hub, connections);
  set_close_handler(hub, connections);


  std::string raw_ip_address = "0.0.0.0";
  unsigned short port_num = 3001;



  boost::system::error_code ec;

  boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(raw_ip_address, ec);

  const boost::asio::ip::tcp::endpoint ep(ip_address,port_num);



  hub.listen(ep);
  hub.start_accept();
  hub.run();
}