#include <iostream>

extern "C"{
#include <sorrylinus/test/soliapi_test.h>    
}


using namespace std;



int main (){


    int check;


    check = soli_test_method();

    cout << "exit with: " << check << endl;


    return 0;
}





