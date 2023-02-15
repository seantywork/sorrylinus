#include "../../lib/httplib/httplib.h"
#include "../../lib/nlohmann/json.hpp"

#include "../controller/controller.cpp"

using namespace httplib;

using json = nlohmann::json;

std::ifstream rt_f("./config.json");
json rt_config_data = json::parse(rt_f);

std::string ADDRESS = rt_config_data["ADDRESS"];
int PORT =  rt_config_data["PORT"];



class Router {

public:

    Server srv;

    Controller ctrl;

    void InitController(){

        srv.Get("/", ctrl.GetIndex);


    }


    void Start(){

        std::cout << "Server is up and runnning..." << std::endl;

        std::cout << "-----> ADDRESS : " << ADDRESS << " " << "PORT : " << PORT << std::endl;

        srv.listen(ADDRESS, PORT);

    } 


};

