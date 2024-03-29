#ifndef PREPROCESSOR_HEADER 
#define PREPROCESSOR_HEADER 
#include <httplib/httplib.h>
#include <jsonlib/json.hpp>
#endif

#include "../controller/controller.cc"

using namespace httplib;


std::ifstream rt_f("./config.json");
json rt_config_data = json::parse(rt_f);

std::string PUBLIC_ROOT = rt_config_data["PUBLIC_ROOT"];

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

        srv->set_mount_point("/public",PUBLIC_ROOT);

        srv->Post("/get-records", ctrl.GetRecords);

    }



};

