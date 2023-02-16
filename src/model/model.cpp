#include "../../lib/httplib/httplib.h"
#include "../../lib/fastcsv/csv_parser.h"

using namespace httplib;



struct RecordText {

    std::string first, second, third;

};




class Model {


public:


    RecordText GetRecordByCmd(){


        io::CSVReader<3> in("./model/data.csv");
        in.read_header(io::ignore_extra_column,"first","second","third");
        
        RecordText ret;

        for(int i=0;i<1;i++){

            in.read_row(ret.first, ret.second, ret.third);

        }

        return ret;


    }





};