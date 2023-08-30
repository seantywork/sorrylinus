
#include <csignal>
#include <iostream>
//#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>

#include <websocketpp/config/asio.hpp>

#include <fstream>

#include <future>
#include <unistd.h>     

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

using connection_hdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using server = websocketpp::server<websocketpp::config::asio_tls>;
using ssl_context = websocketpp::lib::asio::ssl::context;

using server_plain =  websocketpp::server<websocketpp::config::asio>;




std::map<std::string, void*> VERIFIED_SOCK_CONNS;

std::map<std::string, void*> VERIFIED_FRONT_CONNS;

std::map<void*, std::string> VERIFIED_SOCK_CONNS_OWNER;

std::map<void*, std::string> VERIFIED_FRONT_CONNS_OWNER;

std::map<void*, connection_hdl> VERIFIED_ADDRCONN_MAPPER;




bool equal_connection_hdl(connection_hdl& a, connection_hdl& b) {

  return a.lock() == b.lock();

}

void unregister_verified_connections(connection_hdl& hdl){

    std::string owner_key = "";
    std::string sokey = "";
    std::string fokey = "";

    void* conn_addr = hdl.lock().get();


    if(VERIFIED_SOCK_CONNS_OWNER.find(conn_addr) != VERIFIED_SOCK_CONNS_OWNER.end()){

        sokey = VERIFIED_SOCK_CONNS_OWNER[conn_addr];

    }

    if(VERIFIED_FRONT_CONNS_OWNER.find(conn_addr) != VERIFIED_FRONT_CONNS_OWNER.end()){

        fokey = VERIFIED_FRONT_CONNS_OWNER[conn_addr];

    }

    // tight check
    /*
    if (sokey == fokey && sokey != "" && fokey != ""){
        owner_key = sokey;
    }

    if (owner_key == ""){

        return;

    }
    */

    owner_key = sokey;

    int t_sco = VERIFIED_SOCK_CONNS_OWNER.size();
    int t_fco = VERIFIED_FRONT_CONNS_OWNER.size();
    int t_sc = VERIFIED_SOCK_CONNS.size();
    int t_fc = VERIFIED_FRONT_CONNS.size();
    int t_am = VERIFIED_ADDRCONN_MAPPER.size();

    printf("registry: %d %d %d %d %d\n",t_sco, t_fco, t_sc, t_fc, t_am);


    VERIFIED_SOCK_CONNS_OWNER.erase(conn_addr);
    VERIFIED_FRONT_CONNS_OWNER.erase(conn_addr);
    VERIFIED_SOCK_CONNS.erase(owner_key);
    VERIFIED_FRONT_CONNS.erase(owner_key);
    VERIFIED_ADDRCONN_MAPPER.erase(conn_addr);

    t_sco = VERIFIED_SOCK_CONNS_OWNER.size();
    t_fco = VERIFIED_FRONT_CONNS_OWNER.size();
    t_sc = VERIFIED_SOCK_CONNS.size();
    t_fc = VERIFIED_FRONT_CONNS.size();
    t_am = VERIFIED_ADDRCONN_MAPPER.size();

    printf("registry: %d %d %d %d %d\n",t_sco, t_fco, t_sc, t_fc, t_am);

    return;


}




void remove_connection(server* hub, std::vector<connection_hdl>* connections,
                       connection_hdl& hdl) {
  auto equal_connection_hdl_predicate =
      std::bind(&equal_connection_hdl, hdl, ::_1);

  
  connections->erase(
      std::remove_if(std::begin(*connections), std::end(*connections),
                     equal_connection_hdl_predicate),
      std::end(*connections));


  unregister_verified_connections(hdl);


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

  
  unregister_verified_connections(hdl);



  std::cout << "connection evicted for closing"  << std::endl;
}



void register_verified_connections_sock(server* hub,
                                   std::vector<connection_hdl>* connections,
                                   connection_hdl& hdl, 
                                   std::string owner){

    void* conn_addr = hdl.lock().get();

    if(VERIFIED_SOCK_CONNS_OWNER.find(conn_addr) != VERIFIED_SOCK_CONNS_OWNER.end()){

        std::cout << "existing sock connection detected" << std::endl;
        remove_connection(hub,connections, hdl);
        std::cout << "existing sock connection removed" << std::endl;

    }
    

    VERIFIED_SOCK_CONNS.insert({owner, hdl.lock().get()});
    VERIFIED_SOCK_CONNS_OWNER.insert({hdl.lock().get(), owner}); 
    VERIFIED_ADDRCONN_MAPPER.insert({hdl.lock().get(),hdl});

    std::cout << "sock connection pool updated" << std::endl;


}

void register_verified_connections_front(server* hub,
                                   std::vector<connection_hdl>* connections,
                                   connection_hdl& hdl, 
                                   std::string owner){

    void* conn_addr = hdl.lock().get();

    if(VERIFIED_FRONT_CONNS_OWNER.find(conn_addr) != VERIFIED_FRONT_CONNS_OWNER.end()){

        std::cout << "existing front connection detected" << std::endl;
        remove_connection(hub,connections, hdl);
        std::cout << "existing front connection removed" << std::endl;

    }

    VERIFIED_FRONT_CONNS.insert({owner, hdl.lock().get()});
    VERIFIED_FRONT_CONNS_OWNER.insert({hdl.lock().get(), owner}); 
    VERIFIED_ADDRCONN_MAPPER.insert({hdl.lock().get(),hdl});
    std::cout << "front connection pool updated" << std::endl;

}


void send_message(server* hub, connection_hdl* connection, std::string msg) {
  hub->send(*connection, msg, websocketpp::frame::opcode::text);
}

void send_message_plain(server_plain* hub_plain, connection_hdl* connection, std::string msg) {
  hub_plain->send(*connection, msg, websocketpp::frame::opcode::text);
}

std::string extract_common_name(const char* cert) {

    BIO *b = BIO_new(BIO_s_mem());
    BIO_puts(b, cert);
    X509 * x509 = PEM_read_bio_X509(b, NULL, NULL, NULL);

    std::string ret_cn = "FAIL";

    int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(x509), NID_commonName, -1);
    if (common_name_loc < 0) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }
    
    X509_NAME_ENTRY * common_name_entry = X509_NAME_get_entry(X509_get_subject_name(x509), common_name_loc);
    if (common_name_entry == NULL) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }
    
    ASN1_STRING * common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (common_name_asn1 == NULL) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }
    
    char const * common_name_str = (char const *) ASN1_STRING_get0_data(common_name_asn1);
    
    if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
        BIO_free(b);
        X509_free(x509);
        return ret_cn;
    }

    ret_cn = common_name_str;
    
    BIO_free(b);
    X509_free(x509);

    return ret_cn;
}


