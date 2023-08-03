#ifndef PREPROCESSOR_HEADER 
#define PREPROCESSOR_HEADER 
#include <bits/stdc++.h>
#include "../model/model.cc"
#endif


std::string BuildRetJSON(std::vector<std::string> keys,DB_Result values){

    std::string ret_string = "";

    int row_iter = values.RecordCount;

    int row_last_idx = row_iter - 1;

    int key_iter = keys.size();

    int key_last_idx = key_iter - 1;


    for(int i=0;i<row_iter;i++){

        for (int j=0;j<key_iter;j++){

            std::string key_value_pair = "";

            key_value_pair = "\"" + keys[j] + "\" : \"" + values.Result[i][keys[j]] + "\"";

            if(j == 0){

                if(key_last_idx == 0){

                    ret_string += " { " + key_value_pair + " } ";

                } else {

                    ret_string += " { " + key_value_pair + " , ";

                }


            } else if (j == key_last_idx){

                ret_string += key_value_pair + " } ";


            } else {

                ret_string += key_value_pair + " , ";

            }


        }

        if(i == 0){

            if(row_last_idx == 0){

                ret_string = "[ " + ret_string + " ]";

            } else{

                ret_string = "[ " + ret_string + " , ";

            }

        } else if (i == row_last_idx){

            ret_string = ret_string + " ]";


        } else {

            ret_string = ret_string + " , ";
        }

       
    }


    return ret_string;

}
