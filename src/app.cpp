#include "../lib/httplib/httplib.h"

#include "./router/router.cpp"

using namespace httplib;



int main(){



  Router app;

  app.InitController();

  app.Start();


  return 0;

}
