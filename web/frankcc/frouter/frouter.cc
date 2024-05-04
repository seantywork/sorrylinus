#include "frank_cc/server.h"

std::string PUBLIC_ROOT = app_config_data["PUBLIC_ROOT"];



Router::Router(Server *app){

    srv = app;

}


void Router::Init(){

    srv->Get("/", ctrl.GetIndex);

    srv->Get("/test/frank-health", ctrl.TestFrankHealth);

    srv->set_mount_point("/static",PUBLIC_ROOT);

    srv->Get("/oauth2/google/login", ctrl.GoogleOAuthLogin);

    srv->Get("/oauth2/google/callback", ctrl.GoogleOAuthCallback);

    srv->Post("/signout", ctrl.SignOut);

}


