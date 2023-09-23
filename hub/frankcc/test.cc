#include <chrono>
#include <iostream>
#include <array>
#include <random>
#include <cstddef>
#include <iomanip>

#include <curl/curl.h>


std::string hex_gen(int key_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    auto curr_clock = std::chrono::high_resolution_clock::now();
    std::seed_seq sseq {
        std::chrono::duration_cast<std::chrono::hours> (curr_clock.time_since_epoch()).count(), 
        std::chrono::duration_cast<std::chrono::seconds> (curr_clock.time_since_epoch()).count()
        };
    
    gen.seed(sseq);
    
    std::uniform_int_distribution<int> dis(0x0, 0xF);

    
    std::string hex_code = "";
    
    for (std::size_t i = 0; i < key_size; ++i) {
	
        std::stringstream stream;
        stream << std::hex << dis(gen);
        std::string result( stream.str() );

        hex_code += result;

    }

    
    return hex_code;
}

std::vector<std::string> cookie_split(std::string line, std::string delimiter){

    std::vector<std::string> ret;

    std::string key = line.substr(0, line.find(delimiter));

    if (key.length() == line.length()){
        std::cout << "error" << std::endl;
    }

    std::string val = line.substr(line.find(delimiter)+1, line.length()-1);

    std::cout << key << " "<< val << std::endl;



    return ret;

}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string curl_request_post_code(){

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    }


    return readBuffer;
}


int main() {

    // std::cout << hex_gen(32) << std::endl;

    //std::vector<std::string> vs = cookie_split("test-shit","=");

    std::cout << curl_request_post_code() << std::endl;

    
    return 0;
}