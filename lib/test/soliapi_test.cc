#include <iostream>
#include "../../include/test/soliapi_test.h"
using namespace std;


int SoLiTest::SoLiTestMethod(){

    this->soli_test_value = 12;

    cout << "public value :" << this->soli_test_value << endl;


    this->soli_test_value_secret = 1;
    

    return this->soli_test_value_secret;

}