#include "frank_hub/core.h"


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
