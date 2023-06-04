#include "User.h"
#include <string>
#include <iostream>
#include <vector>
#include "Userpool.h"
#include <mysql.h>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
using namespace std;

string saveResultAsString_for_Userpool(string mysql_request_to_server){
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;

        // Initialize the driver
        driver = sql::mysql::get_mysql_driver_instance();

        // Create a connection
        con = driver->connect("localhost:3306", "skillfactory", "SkillFactory7!");

        // Use the connection for executing queries or other database operations
        sql::Statement *stmt = con->createStatement();
        stmt->execute("USE testdb");
        sql::ResultSet *res_from_server = stmt->executeQuery(mysql_request_to_server.c_str());
        string resultString;
        // Fetch rows from the result set
    while (res_from_server->next()) {
            resultString = res_from_server->getString("id");
  }

    // Clean up
    delete res_from_server;
    delete stmt;
    delete con;
    return resultString;                  
 } 
void Userpool::addUser(const User& username) {

 	string query1 = "SELECT id FROM Users WHERE username LIKE  '" +  username.getnickname() +  "'";
	string res_from_server_toAddUser = saveResultAsString_for_Userpool(query1);
	if (res_from_server_toAddUser != "")  {
		cout << "User with name " << username.getnickname() << " already exists. Try another name!" << endl;
	}
	else {
	try{
		string query2 = "INSERT INTO Users(id, username, password) values(default,  '" +  username.getnickname() +  "', SHA1('" + username.getpassword() + "')) ";
		sql::mysql::MySQL_Driver *driver;
    	sql::Connection *con;

        // Initialize the driver
    	driver = sql::mysql::get_mysql_driver_instance();

        // Create a connection
    	con = driver->connect("localhost:3306", "skillfactory", "SkillFactory7!");
 
		sql::Statement *stmt = con->createStatement();
    	stmt->execute("USE testdb");
		sql::ResultSet *res_from_server = stmt->executeQuery(query2);
		cout << "we tried to add User2" << endl;
    	delete res_from_server;
    	delete stmt;
		cout << "we tried to add User" << endl;

			 } catch (const sql::SQLException &e) {
        // Handle any errors that occur
        cout << "SQL Exception: " << e.what() << endl;
		cout << username.getnickname() << " was created." << endl;
		        
    }
	
	}
}

bool Userpool::checkUser(const string& nickname, const string& password) {

	string query = "SELECT id FROM Users WHERE username LIKE  '" +  nickname +  "' AND password LIKE SHA1('" + password + "')";
    string res_from_server = saveResultAsString_for_Userpool(query);
  
	if (res_from_server != "") {
		cout << "Login successful. " << endl;
		return true;}
	else {
		cout << "Login failed. Try again." << endl;
	return false;
	}
 
}

void Userpool::showUsers() {
	
	sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;

        // Initialize the driver
    driver = sql::mysql::get_mysql_driver_instance();

        // Create a connection
    con = driver->connect("localhost:3306", "skillfactory", "SkillFactory7!");
 

	sql::Statement *stmt = con->createStatement();
    stmt->execute("USE testdb");
    sql::ResultSet *res_from_server = stmt->executeQuery("SELECT id, username FROM Users");
    while (res_from_server ->next()) {
        std::cout << res_from_server ->getString(1) << "  " << res_from_server ->getString(2) << std::endl;
    }
    delete res_from_server ;
    delete stmt;
}
