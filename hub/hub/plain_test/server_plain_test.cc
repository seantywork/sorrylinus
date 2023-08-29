
#ifndef PREPROCESSOR_HEADER_SOCK
#define PREPROCESSOR_HEADER_SOCK

#include "./sock_resource_plain_test.h"

#endif

std::future<std::string> AWAIT;


std::string async_callback_test(std::string words){

  sleep(5);

  words += " ...after a few seconds";

  return words;

}


void ph_sock_client_plain_test(server* hub, 
                         std::vector<connection_hdl>* connections,
                         connection_hdl hdl,
                         websocketpp::config::asio::message_type::ptr msg){

    std::string send_back_payload;
 

    std::string test_words = "hello from the hub";

    AWAIT = std::async(std::launch::async, async_callback_test, test_words);

    std::cout << "waiting a few seconds..." << std::endl;

    send_back_payload = AWAIT.get();

    send_message(hub, &hdl, send_back_payload);


    std::cout << "on_message:" << msg->get_payload() << std::endl;
    return;
    
}

void on_message(server* hub, std::vector<connection_hdl>* connections,
                connection_hdl hdl,
                websocketpp::config::asio::message_type::ptr msg) {

  server::connection_ptr con = hub->get_con_from_hdl(hdl);
  std::string url_path = con->get_resource();


  if (url_path == "/sock-client"){

    // ph_sock_client(hub, connections, hdl, msg);

  } else if (url_path == "/front-client"){

    // ph_front_client(hub, connections, hdl, msg);

  } else if (url_path == "/sock-client/test"){
    
    ph_sock_client_plain_test(hub, connections, hdl, msg);

  
  } else if (url_path == "/front-client/test"){

    // ph_front_client_test(hub, connections, hdl, msg);

  } else {

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






int main() {
  server hub;
  std::vector<connection_hdl> connections;

  //turn_off_logging(hub);


  hub.init_asio();

  set_message_handler(hub, connections);
  set_open_handler(hub, connections);
  set_close_handler(hub, connections);


  std::string raw_ip_address = "0.0.0.0";
  unsigned short port_num = 3001;



  boost::system::error_code ec;

  boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(raw_ip_address, ec);

  const boost::asio::ip::tcp::endpoint ep(ip_address,port_num);



  hub.listen(ep);
  hub.start_accept();
  //hub.run();
  

  websocketpp::lib::thread t1(&server::run, &hub);

  std::string name;
  std::string input;
   bool done = false;
  std::cout << "Name: ";
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

      connection_hdl* connection = &connections[0];

      send_message(&hub, connection, msg);
    }
  }
  t1.join();
}
