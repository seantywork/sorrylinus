#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
//#include <websocketpp/config/asio_client.hpp>
#include <fstream>


using client = websocketpp::client<websocketpp::config::asio_client>;
using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;


//using client = websocketpp::client<websocketpp::config::asio_tls_client>;
//using ssl_context = websocketpp::lib::asio::ssl::context;




void send_message(client* sock, connection_hdl* connection, std::string msg) {
  sock->send(*connection, msg, websocketpp::frame::opcode::text);
}

void close_connection(client* sock, connection_hdl* connection) {
  sock->close(*connection, websocketpp::close::status::normal, "done");
}

void on_message(client* client, connection_hdl hdl,
                websocketpp::config::asio_client::message_type::ptr msg) {
  std::cout << "on_message: " << msg->get_payload() << std::endl;
//  client->close(hdl, websocketpp::close::status::normal, "done");
}

void on_open(client* sock, connection_hdl* connection ,connection_hdl hdl) {
  *connection = hdl;
  std::cout << "on_open: send cert" << std::endl;

  std::ifstream in_cert("sock/tls/sub_client.crt");
  std::stringstream buffer;

  std::string msg;

  buffer << in_cert.rdbuf();

  msg = buffer.str();

  sock->send(hdl, msg, websocketpp::frame::opcode::text);
}

void turn_off_logging(client& sock) {
  sock.clear_access_channels(websocketpp::log::alevel::all);
  sock.clear_error_channels(websocketpp::log::elevel::all);
}

void set_message_handler(client& sock) {
  sock.set_message_handler(
      websocketpp::lib::bind(&on_message, &sock, ::_1, ::_2));
}

void set_open_handler(client& sock, connection_hdl* connection) {
  sock.set_open_handler(websocketpp::lib::bind(&on_open, &sock, connection, ::_1));
}

void set_url(client& sock, std::string url) {
  websocketpp::lib::error_code ec;
  auto connection = sock.get_connection(url, ec);
  sock.connect(connection);
}


int main() {


  client sock;

  connection_hdl connection;

  //turn_off_logging(sock);

  sock.init_asio();

  set_open_handler(sock, &connection);
  set_message_handler(sock);
  std::string remote_addr = "ws://localhost:3001/sock-client/test";
  set_url(sock, remote_addr);

  websocketpp::lib::thread t1(&client::run, &sock);

  
  bool done = false;
  std::string input;
  std::string name;

  std::cout << "Name: ";
  std::getline(std::cin, name);

  while (!done) {
    std::getline(std::cin, input);
    if (input == "close") {
      done = true;
      close_connection(&sock, &connection);
    } else if (input == "help") {
      std::cout << "\nCommand List:\n"
                << "close\n"
                << "<message>\n"
                << "help: Display this help text\n"
                << std::endl;
    } else {
      std::string msg{name + ": " + input};
      send_message(&sock, &connection, msg);
    }
  }

  t1.join();

  return 0;
}


/*
int main() {

  bool done = false;
  std::string input;
  std::string name;

  client sock;

  connection_hdl connection;

  //turn_off_logging(sock);

  sock.init_asio();

  set_tls_init_handler(&sock, &connection);
  set_open_handler(sock, &connection);
  set_message_handler(sock);

  set_url(sock, "wss://localhost:3001/sock-client");

  websocketpp::lib::thread t1(&client::run, &sock);


  std::cout << "Name: ";
  std::getline(std::cin, name);

  while (!done) {
    std::getline(std::cin, input);
    if (input == "close") {
      done = true;
      close_connection(&sock, &connection);
    } else if (input == "help") {
      std::cout << "\nCommand List:\n"
                << "close\n"
                << "<message>\n"
                << "help: Display this help text\n"
                << std::endl;
    } else {
      std::string msg{name + ": " + input};
      send_message(&sock, &connection, msg);
    }
  }

  t1.join();
}

*/