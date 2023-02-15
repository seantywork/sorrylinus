#include "../../lib/httplib/httplib.h"
#include "../../lib/nlohmann/json.hpp"

#include "../controller/controller.cpp"

using namespace httplib;

/*
using json = nlohmann::json;

std::ifstream rt_f("./config.json");
json rt_config_data = json::parse(rt_f);
*/


class Router {

public:

    Server *srv;

    Controller ctrl;

    Router(Server *app){

        srv = app;


    }

    void Init(){

        srv->Get("/", ctrl.GetIndex);

    }



};

