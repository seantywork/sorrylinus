#include "sock_resource_sql.h"


int main(){

    std::cout << "Running 'SELECT 'Hello World!' » AS _message'..." << std::endl;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect("tcp://localhost:3306", "seantywork", "letsshareitwiththewholeuniverse");

        con->setSchema("frank");

        prep_stmt = con->prepareStatement("SELECT email FROM his_onlyfriends WHERE email = ?");

        prep_stmt->setString(1,"seantywork@gmail.com");

        prep_stmt->execute();

        res = prep_stmt->getResultSet();
  

        while (res->next()) {
            std::cout << "\t... MySQL replies: ";
            
            std::cout << res->getString("email") << std::endl;
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

    std::cout << std::endl;

    return EXIT_SUCCESS;
}