
#ifndef PREPROCESSOR_HEADER_SOCK
#define PREPROCESSOR_HEADER_SOCK

#include "./sock_resource.h"

#endif

#include "./peer_verification.cc"

std::future<std::string> AWAIT;

void ph_sock_client(server* hub, 
                    std::vector<connection_hdl>* connections,
                    connection_hdl hdl,
                    websocketpp::config::asio::message_type::ptr msg){

    std::string verification_status = check_if_verified_conn(&VERIFIED_SOCK_CONNS_OWNER, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    std::string result;
    if (verification_status == "FAIL"){

      result = authenticate_request(msg);
      
      if (result == "FAIL"){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection(hub, connections, hdl);

      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        std::cout << "auth success: add verified connection:" << result << std::endl;
        register_verified_connections_sock(hub, connections, hdl, result);   
      }  

      return;
    }

    std::cout << "on_message: " << msg->get_payload() << std::endl;  


    return;

    
}


std::string async_callback_test(std::string words){


  for(int i =0;i < 5; i++){
    std::cout << "waiting..." <<std::endl;
    sleep(1);
  }

  words += " ...after a few seconds";

  return words;

}


void ph_sock_client_test(server* hub, 
                         std::vector<connection_hdl>* connections,
                         connection_hdl hdl,
                         websocketpp::config::asio::message_type::ptr msg){

    std::string send_back_payload;
    
    std::string verification_status = check_if_verified_conn(&VERIFIED_SOCK_CONNS_OWNER, hdl);
    
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    
    std::string result;

    if (verification_status == "FAIL"){

      result = authenticate_request(msg);
      
      if (result == "FAIL"){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection(hub, connections, hdl);

      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        std::cout << "auth success: add verified connection:" << result << std::endl;

        register_verified_connections_sock(hub, connections, hdl, result);

        std::string test_words = "hello from the hub";

        AWAIT = std::async(std::launch::async, async_callback_test, test_words);

        std::cout << "waiting a few seconds..." << std::endl;

        send_back_payload = AWAIT.get();

        send_message(hub, &hdl, send_back_payload);


      }  

      return;
    }


    std::cout << "on_message:" << msg->get_payload() << std::endl;
    return;
    
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

void ph_front_client(server* hub, 
                     std::vector<connection_hdl>* connections,
                     connection_hdl hdl,
                     websocketpp::config::asio::message_type::ptr msg){


    std::string verification_status = check_if_verified_conn(&VERIFIED_FRONT_CONNS_OWNER, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    std::string result;
    if (verification_status == "FAIL"){

      result = authenticate_request(msg);
      
      if (result == "FAIL"){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection(hub, connections, hdl);
    
      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        
        register_verified_connections_front(hub, connections, hdl, result);      
      }  

      return;
    }

    std::cout << "on_message: " << msg->get_payload() << std::endl;  

    return;
}

void ph_front_client_test (server* hub, 
                           std::vector<connection_hdl>* connections,
                           connection_hdl hdl,
                           websocketpp::config::asio::message_type::ptr msg){


    std::string verification_status = check_if_verified_conn(&VERIFIED_FRONT_CONNS_OWNER, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    std::string result;
    if (verification_status == "FAIL"){

      result = authenticate_request(msg);
      
      if (result == "FAIL"){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection(hub, connections, hdl);
    
      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        register_verified_connections_front(hub, connections, hdl, result);  
      }  

      return;
    }

    std::cout << "on_message: " << msg->get_payload() << std::endl;  

    return;
}