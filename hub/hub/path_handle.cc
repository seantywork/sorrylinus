
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

    sock_to_front_handler(hub, &hdl, msg->get_payload()); 


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

    sock_to_front_handler(hub, &hdl, msg->get_payload()); 

    return;
    
}


void ph_front_client(server_plain* hub_plain, 
                     std::vector<connection_hdl>* connections,
                     connection_hdl hdl,
                     websocketpp::config::asio::message_type::ptr msg){


    std::string verification_status = check_if_verified_conn(&VERIFIED_FRONT_CONNS_OWNER, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    
    DBResult<FrankRecord> result;
    
    if (verification_status == "FAIL"){

      result = get_record_by_pkey(msg->get_payload());
      
      if (result.status == "FAIL"){
        std::cout << "auth failed: *: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection_plain(hub_plain, connections, hdl);
        return;
      }

      std::string email = result.results[0].email;

      std::string f_session = result.results[0].f_session;

      std::string p_key = result.results[0].p_key;      


      if (f_session == "N"){
        std::cout << "auth failed: no session: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection_plain(hub_plain, connections, hdl);
        return;        
      }

      if (p_key == "N"){
        std::cout << "auth failed: no key: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection_plain(hub_plain, connections, hdl);
        return;        
      }


      std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;

      register_verified_connections_front(hub_plain, connections, hdl, email);      
    

      return;
    }

    front_to_sock_handler(hub_plain, &hdl, msg->get_payload());  

    return;
}

void ph_front_client_test (server_plain* hub_plain, 
                           std::vector<connection_hdl>* connections,
                           connection_hdl hdl,
                           websocketpp::config::asio::message_type::ptr msg){


    std::string verification_status = check_if_verified_conn(&VERIFIED_FRONT_CONNS_OWNER, hdl);
    std::cout << "on_message: v_stat:" << verification_status << std::endl;
    DBResult<FrankRecord> result;


    if (verification_status == "FAIL"){

      result = get_record_by_pkey(msg->get_payload());
      
      if (result.status == "FAIL"){
        std::cout << "auth failed: remove connection:" << hdl.lock().get() << std::endl;
        remove_connection_plain(hub_plain, connections, hdl);
    
      }else {
        std::cout << "auth success: add verified connection:" << hdl.lock().get() << std::endl;
        std::string email = result.results[0].email;
        register_verified_connections_front(hub_plain, connections, hdl, email);  
      }  

      return;
    }

    std::cout << "on_message: " << msg->get_payload() << std::endl;  

    front_to_sock_handler(hub_plain, &hdl, msg->get_payload());  

    return;
}