
#ifndef PREPROCESSOR_HEADER_SOCK
#define PREPROCESSOR_HEADER_SOCK

#include "./sock_resource.h"

#endif

#include "./path_handle.cc"

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


int main(int argc, char **argv) {

  int INTERACTIVE_FLAG = 0;

  for (int i =0; i < argc; i++){

    if(strcmp(argv[i], "--interactive") == 0){
      INTERACTIVE_FLAG = 1;
    }

  }

  boost::asio::io_service io_serve;

  server_plain hub_plain;


  server hub;
  std::vector<connection_hdl> connections;

  //turn_off_logging(hub);


  hub_plain.init_asio(&io_serve);
  set_message_handler_plain(hub_plain, connections);
  set_open_handler_plain(hub_plain, connections);
  set_close_handler_plain(hub_plain, connections);
  std::string raw_ip_address_plain = "0.0.0.0";
  unsigned short port_num_plain = 3000;


  boost::system::error_code ec;

  boost::asio::ip::address ip_address_plain = boost::asio::ip::address::from_string(raw_ip_address_plain, ec);

  const boost::asio::ip::tcp::endpoint ep_plain(ip_address_plain,port_num_plain); 
 
  hub_plain.listen(port_num_plain);
  hub_plain.start_accept();


  hub.init_asio(&io_serve);

  set_message_handler(hub, connections);
  set_tls_init_handler(hub);
  set_open_handler(hub, connections);
  set_close_handler(hub, connections);


  std::string raw_ip_address = "0.0.0.0";
  unsigned short port_num = 3001;



  boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(raw_ip_address, ec);

  const boost::asio::ip::tcp::endpoint ep(ip_address,port_num);



  hub.listen(ep);
  hub.start_accept();

  SOCK_TO_FRONT.insert({&hub, &hub_plain});

  FRONT_TO_SOCK.insert({&hub_plain, &hub});

  if(INTERACTIVE_FLAG == 0){

    io_serve.run();

  }else{

    websocketpp::lib::thread t1(&server::run, &hub);
    websocketpp::lib::thread t2(&server_plain::run, &hub_plain);

    std::string name;
    std::string input;
    bool done = false;
    std::cout << "owner: ";
    std::getline(std::cin, name);

    while (!done) {
      std::getline(std::cin, input);
      if (input == "close") {
        done = true;
      } else if (input == "help") {
        std::cout << "\nCommand List:\n"
                  << "close\n"
                  << "<message>\n"
                  << "help: Display this help text\n"
                  << std::endl;
      } else {
        std::string msg{name + ": " + input};

        void* conn_addr =  VERIFIED_SOCK_CONNS[name];
        connection_hdl* connection = &VERIFIED_ADDRCONN_MAPPER[conn_addr];
        send_message(&hub, connection, msg);
      }
    }
    t1.join();
    t2.join();

  }

} 
