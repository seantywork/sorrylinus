//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../src/httplib.h"


using namespace httplib;

std::string ROOT_PATH = "./templates/";

std::string getCurrentTime(){

    std::stringstream time_stream;
    
    auto now = std::chrono::system_clock::now();

    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    time_stream << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S");

    std::string std_time_now = time_stream.str();

    return std_time_now;

}

std::string getFileContent(std::string file_path){


    std::ifstream in_file(file_path);
    std::stringstream buffer;
    buffer << in_file.rdbuf();


    if (!in_file){

        return "Path Not Available";

    }

    std::string content = buffer.str();

    return content;
}

int main(){
    // HTTP
    Server svr;

    // HTTPS
    //httplib::SSLServer svr;


    svr.Get("/", [](const Request &, Response &res) {

      std::string index_path = ROOT_PATH + "html/index.html";

      res.set_content(getFileContent(index_path), "text/html");

      std::string current_time = getCurrentTime();


      std::cout << current_time + ": GET [ / ] " << std::endl;


    });

    svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
      
      std::cout << "[ /numbers ] has been hit" << std::endl;

      auto numbers = req.matches[1];
      res.set_content(numbers, "text/plain");
    });


    svr.Post("/body-header-param", [](const Request& req, Response& res) {
      if (req.has_header("Content-Length")) {
        auto val = req.get_header_value("Content-Length");
      }
      if (req.has_param("key")) {
        auto val = req.get_param_value("key");
        std::cout << val << std::endl;
      }
      res.set_content(req.body, "text/plain");
    });

    //svr.Get("/stop", [&](const Request& req, Response& res) {
    //  svr.stop();
    //});


    std::cout << "Server is up and runnning..." << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;

}
