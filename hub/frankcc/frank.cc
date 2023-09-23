#ifndef FRANK_PREPROCESSOR_HEADER 
#define FRANK_PREPROCESSOR_HEADER 
#include <httplib/httplib.h>
#include <jsonlib/json.hpp>
#endif

#include "./frouter/frouter.cc"


using namespace httplib;


using json = nlohmann::json;

std::ifstream app_f("./config.json");
json app_config_data = json::parse(app_f);

std::string ADDRESS = app_config_data["ADDRESS"];
int PORT =  app_config_data["PORT"];


int main(){

    std::cout << "entering AP mode..." << std::endl;

    Server app;

    Router rt(&app);

    rt.Init();


    std::cout << "server is up and runnning..." << std::endl;

    std::cout << "-----> HOST ADDRESS : " << ADDRESS << " " << "PORT : " << PORT << std::endl;

    app.listen(ADDRESS, PORT);



    return 0;
}