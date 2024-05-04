#include "frank_hub/core.h"

void on_message_plain(server_plain* hub_plain, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg) {

  server_plain::connection_ptr con = hub_plain->get_con_from_hdl(hdl);
  std::string url_path = con->get_resource();

  if (url_path == "/front-client"){

    ph_front_client(hub_plain, connections, hdl, msg);

  } /*else if (url_path == "/front-client/test"){

    ph_front_client_test(hub_plain, connections, hdl, msg);

  } */else {

    std::cout << "illegal path: " << url_path << std::endl;  
    remove_connection_plain(hub_plain, connections, hdl);
    return;

  }



}

void on_message(server* hub, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg) {

  server::connection_ptr con = hub->get_con_from_hdl(hdl);
  std::string url_path = con->get_resource();


  if (url_path == "/sock-client"){

    ph_sock_client(hub, connections, hdl, msg);

  }/* else if (url_path == "/sock-client/test"){
    
    ph_sock_client_test(hub, connections, hdl, msg);

  
  } */else {

    std::cout << "illegal path: " << url_path << std::endl;  
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

void on_close_plain(server_plain* hub_plain, std::vector<connection_hdl>* connections,
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

void set_message_handler_plain(server_plain& hub_plain,
                         std::vector<connection_hdl>& connections) {
  hub_plain.set_message_handler(
      websocketpp::lib::bind(&on_message_plain, &hub_plain, &connections, ::_1, ::_2));
}


void set_open_handler(server& hub, std::vector<connection_hdl>& connections) {
  hub.set_open_handler(websocketpp::lib::bind(&on_open, &connections, ::_1));
}

void set_open_handler_plain(server_plain& hub_plain, std::vector<connection_hdl>& connections) {
  hub_plain.set_open_handler(websocketpp::lib::bind(&on_open, &connections, ::_1));
}


void set_close_handler(server& hub,
                       std::vector<connection_hdl>& connections) {
  hub.set_close_handler(
      websocketpp::lib::bind(&on_close, &hub, &connections, ::_1));
}

void set_close_handler_plain(server_plain& hub_plain,
                       std::vector<connection_hdl>& connections) {
  hub_plain.set_close_handler(
      websocketpp::lib::bind(&on_close_plain, &hub_plain, &connections, ::_1));
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

