//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../controller/httplib.h"
#include <bits/stdc++.h>

using namespace httplib;
using namespace std;

int main(){
    // HTTP
    Server svr;

    // HTTPS
    //httplib::SSLServer svr;

    svr.Get("/hi", [](const Request &, Response &res) {
      cout << "[ /hi ] has been hit" << endl;
      res.set_content("Hello World!", "text/plain");
    });

    svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
      
      cout << "[ /numbers ] has been hit" << endl;

      auto numbers = req.matches[1];
      res.set_content(numbers, "text/plain");
    });

    cout << "CNC is up and runnning..." <<endl;
    svr.listen("0.0.0.0", 8080);

    return 0;

}
