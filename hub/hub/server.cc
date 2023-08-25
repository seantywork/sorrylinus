#include <csignal>
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>

/*
  using server = websocketpp::server<websocketpp::config::asio_tls>;
  using ssl_context = websocketpp::lib::asio::ssl::context;
*/

using server = websocketpp::server<websocketpp::config::asio>;
using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

bool equal_connection_hdl(connection_hdl& a, connection_hdl& b) {
  return a.lock() == b.lock();
}

void remove_connection(std::vector<connection_hdl>* connections,
                       connection_hdl& hdl) {
  auto equal_connection_hdl_predicate =
      std::bind(&equal_connection_hdl, hdl, ::_1);
  connections->erase(
      std::remove_if(std::begin(*connections), std::end(*connections),
                     equal_connection_hdl_predicate),
      std::end(*connections));
}

void on_message(server* hub, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg) {
  std::cout << "on_message: " << msg->get_payload() << std::endl;
  for (auto& connection : *connections) {
    if (connection.lock() != hdl.lock()) {
      hub->send(connection, msg->get_payload(),
                   websocketpp::frame::opcode::text);
    }
  }
}

void on_open(std::vector<connection_hdl>* connections, connection_hdl hdl) {
  std::cout << "on_open" << std::endl;
  connections->push_back(hdl);
  std::cout << "connections: " << connections->size() << std::endl;
}

void on_close(server* hub, std::vector<connection_hdl>* connections,
              connection_hdl hdl) {
  std::cout << "on_close" << std::endl;
  remove_connection(connections, hdl);
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

/*
  websocketpp::lib::shared_ptr<ssl_context> on_tls_init(connection_hdl hdl) {
    auto ctx = websocketpp::lib::make_shared<ssl_context>(ssl_context::sslv23);

    ctx->use_certificate_chain_file("cert.pem");
    ctx->use_private_key_file("key.pem", ssl_context::pem);
    return ctx;
  }


  void set_tls_init_handler(server& hub) {
    hub.set_tls_init_handler(websocketpp::lib::bind(&on_tls_init, ::_1));
  }
*/

int main() {
  server hub;
  std::vector<connection_hdl> connections;

  turn_off_logging(hub);

  hub.init_asio();

  set_message_handler(hub, connections);
  //set_tls_init_handler(server);
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