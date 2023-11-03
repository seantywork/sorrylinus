#ifndef FRANK_PREPROCESSOR_HEADER 
#define FRANK_PREPROCESSOR_HEADER 
#include <httplib/httplib.h>
#include <jsonlib/json.hpp>
#endif

#include "../fcontroller/fcontroller.cc"

using namespace httplib;

using json = nlohmann::json;

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

        srv->Get("/test/frank-health", ctrl.TestFrankHealth);

        srv->set_mount_point("/static",PUBLIC_ROOT);

        srv->Get("/oauth2/google/login", ctrl.GoogleOAuthLogin);

        srv->Get("/oauth2/google/callback", ctrl.GoogleOAuthCallback);

        srv->Post("/signout", ctrl.SignOut);

    }



};

