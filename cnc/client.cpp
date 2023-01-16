//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../controller/httplib.h"
#include <bits/stdc++.h>

using namespace std;

int main(){
    // HTTP
    httplib::Client cli("http://localhost:8080");

    // HTTPS
    //httplib::Client cli("https://cpp-httplib-server.yhirose.repl.co");

    auto res = cli.Get("/hi");
    cout<<res->status<<endl;
    cout<<res->body<<endl;
    
    return 0;

}
