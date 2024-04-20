#include "frank_hub/core.h"



std::string DB_ADDRESS= "tcp://frankdb:3306";


DBResult<FrankRecord> get_record_by_email(std::string email){

    DBResult<FrankRecord> db_res;

    FrankRecord fr;


    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(DB_ADDRESS, "seantywork", "letsshareitwiththewholeuniverse");

        con->setSchema("frank");

        prep_stmt = con->prepareStatement("SELECT email, f_session, p_key FROM his_onlyfriends WHERE email = ?");

        prep_stmt->setString(1,email);

        prep_stmt->execute();

        res = prep_stmt->getResultSet();


        while (res->next()) {
            
            fr.email = res->getString("email");
            fr.f_session = res->getString("f_session");
            fr.p_key = res->getString("p_key");
            
            db_res.results.push_back(fr);
            
        }

        if(db_res.results.size() != 1){
            
            db_res.status = "FAIL";

        } else {
            db_res.status = "SUCCESS";    
        }


        delete res;
        delete prep_stmt;
        delete con;

    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "  << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }



    return db_res;
}



DBResult<FrankRecord> get_record_by_pkey(std::string pkey){

    DBResult<FrankRecord> db_res;

    FrankRecord fr;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(DB_ADDRESS, "seantywork", "letsshareitwiththewholeuniverse");

        con->setSchema("frank");

        prep_stmt = con->prepareStatement("SELECT email, f_session, p_key FROM his_onlyfriends WHERE p_key = ?");

        prep_stmt->setString(1,pkey);

        prep_stmt->execute();

        res = prep_stmt->getResultSet();


        while (res->next()) {
            
            fr.email = res->getString("email");
            fr.f_session = res->getString("f_session");
            fr.p_key = res->getString("p_key");
            
            db_res.results.push_back(fr);
            
        }

        if(db_res.results.size() != 1){
            
            db_res.status = "FAIL";

        } else {
            db_res.status = "SUCCESS";    
        }


        delete res;
        delete prep_stmt;
        delete con;

    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "  << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }



    return db_res;
}
