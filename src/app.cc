#include "sorrylinus/app/core.h"

std::ifstream app_f("./config.json");
json app_config_data = json::parse(app_f);

std::string SOCK_LOCAL_ADDR = app_config_data["SOCK_LOCAL_ADDR"];
std::string SOCK_FEEB_ADDR = app_config_data["SOCK_FEEB_ADDR"];



int main(int argc, char **argv){


  int SOCK_FLAG = 0;

  int SOCK_FEEB_FLAG = 0;

  int SOCK_TEST_FLAG = 0;

  int INTERACTIVE_FLAG = 0;

  std::string SOCK_ADDR = "";

  for (int i = 0; i < argc; i++){
      
      std::cout<< argv[i] << std::endl;

      if(strcmp(argv[i], "--sock") == 0){
        SOCK_FLAG = 1;
      }

      if(strcmp(argv[i], "--feeb") == 0){

        SOCK_FEEB_FLAG = 1;

      }

      if(strcmp(argv[i], "--sock-test") == 0){
        SOCK_TEST_FLAG = 1;
      }

      if(strcmp(argv[i], "--interactive") == 0){
        INTERACTIVE_FLAG = 1;
      }


  }

  if(SOCK_FLAG == 1 && SOCK_TEST_FLAG == 1){

    std::cout << "can't set both --sock and --sock-test at the same time" << std::endl;
    return 1;

  }

  if(SOCK_FLAG == 1){

    if(SOCK_FEEB_FLAG == 1){

      SOCK_ADDR = "wss://" + SOCK_FEEB_ADDR; 

    } else {

      SOCK_ADDR = "wss://" + SOCK_LOCAL_ADDR;

    }

    if(INTERACTIVE_FLAG == 0){
      run_sock_client(SOCK_ADDR);
    }else{
      run_sock_client_interactive(SOCK_ADDR);
    }


  } else if (SOCK_TEST_FLAG == 1){

    if(SOCK_FEEB_FLAG == 1){

      SOCK_ADDR = "wss://" + SOCK_FEEB_ADDR + "/test"; 

    } else {

      SOCK_ADDR = "wss://" + SOCK_LOCAL_ADDR + "/test";

    }

    if(INTERACTIVE_FLAG == 0){
      run_sock_client(SOCK_ADDR);
    }else{
      run_sock_client_interactive(SOCK_ADDR);
    }


  } else {


    std::cout << "invalid argument combination" << std::endl;

    return -1;
    
  }


  return 0;

}
