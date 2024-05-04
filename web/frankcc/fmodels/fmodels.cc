#include "frank_cc/server.h"

std::string DB_ADDRESS= "tcp://frankdb:3306";


DBResult<FrankRecord> get_record_by_fsession(std::string fsession){

    std::cout << "sql get record" << std::endl;

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

        prep_stmt = con->prepareStatement("SELECT email, f_session, p_key FROM his_onlyfriends WHERE f_session = ?");

        prep_stmt->setString(1,fsession);

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


DBResult<FrankRecord> set_record_by_email(std::string user_email, std::string fsession, std::string pkey){

    std::cout << "sql set record" << std::endl;

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

        prep_stmt->setString(1, user_email);

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

            delete res;
            delete prep_stmt;
            delete con;

            return db_res;

        } 
        prep_stmt = con->prepareStatement("UPDATE his_onlyfriends SET f_session = ?, p_key = ? WHERE email = ?");

        prep_stmt->setString(1,fsession);

        prep_stmt->setString(2,pkey);

        prep_stmt->setString(3,user_email);

        prep_stmt->execute();

        res = prep_stmt->getResultSet();

        db_res.status = "SUCCESS"; 

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

DBResult<FrankRecord> unset_record_by_email(std::string user_email){

    std::cout << "sql unset record" << std::endl;

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

        prep_stmt = con->prepareStatement("UPDATE his_onlyfriends SET f_session = 'N', p_key = 'N' WHERE email = ?");

        prep_stmt->setString(1, user_email);

        prep_stmt->execute();

        res = prep_stmt->getResultSet();

        db_res.status = "SUCCESS"; 

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

