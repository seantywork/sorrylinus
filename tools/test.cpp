
#include <iostream>
#include <sqlite3.h>
  
using namespace std;

struct RecordText {

    string first;
    string second;
    string third;

};
  
static int callback(void* data, int argc, char** argv, char** azColName)
{
    int i;
    
    RecordText* data_callback;

    data_callback = (RecordText*)data;

  
    data_callback->first = argv[0];
    data_callback->second = argv[1];
    data_callback->third = argv[2];

    
    return 0;
}
  
int main(int argc, char** argv)
{
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open("rec.db", &DB);
    RecordText data;
  
    string sql("SELECT * FROM rec;");
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        return (-1);
    }
    else
        std::cout << "Opened Database Successfully!" << std::endl;
  
    int rc = sqlite3_exec(DB, sql.c_str(), callback, (void*)&data, NULL);
  
    if (rc != SQLITE_OK)
        cerr << "Error SELECT" << endl;
    else {
        cout << "Operation OK!" << endl;
        cout << data.first << endl;
        cout << data.second << endl;
        cout << data.third << endl;
    }
  
    sqlite3_close(DB);
    return (0);
}