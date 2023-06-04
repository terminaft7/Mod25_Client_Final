#include "chatpool.h"
#include <string>
#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <mysql.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#define MESSAGE_BUFFER 4096 // Максимальный размер буфера для приема и передачи
#define PORT 7777 // Номер порта, который будем использовать для приема и передачи 
char buffer[MESSAGE_BUFFER];
int socket_descriptor, message_size;
struct sockaddr_in serveraddress;
using namespace std;

string saveResultAsString(MYSQL_RES* result) { 
    string resultString; 
    if (result) { MYSQL_ROW row; unsigned int numFields = mysql_num_fields(result); 
    // Iterate over rows 
        while ((row = mysql_fetch_row(result))) { 
            // Iterate over columns 
            for (unsigned int i = 0; i < numFields; ++i) { 
                 if (row[i]) { 
                    // Append the column value to the result string 
                    resultString += row[i]; 
                     }
                } 
        } 
                mysql_free_result(result); 
    } 
    return resultString; 
} 

string saveResultAsString_from_Server(string mysql_request_to_server){
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

  

bool chatpool::showmap(string receiver, string nickname) {

        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;

        // Initialize the driver
        driver = sql::mysql::get_mysql_driver_instance();

        // Create a connection
        con = driver->connect("localhost:3306", "skillfactory", "SkillFactory7!");
   
    string check_receiver_id = "SELECT id FROM Users WHERE username LIKE  '" +  receiver +  "'";
    string receiver_id_str = saveResultAsString_from_Server(check_receiver_id);

    if (receiver_id_str != ""){
        cout << "Chat history with " << receiver << ":" << endl;
    }

    else {
        cout << "Receiver does not exist. Chose another receiver." << endl;
        return 0;
    }

    string check_nickname_id = "SELECT id FROM Users WHERE username LIKE  '" +  nickname +  "'";
    string nickname_id_str = saveResultAsString_from_Server(check_nickname_id);

  
    string chat_id_opt1 = nickname_id_str + "#" + receiver_id_str;
    string chat_id_opt2 = receiver_id_str + "#" + nickname_id_str;

    string query_chat_id_opt1 = "SELECT sender, message FROM Messages WHERE chat_id LIKE '" + chat_id_opt1 + "' OR chat_id LIKE '" + chat_id_opt2 + "' ORDER BY mes_datetime";

    sql::Statement *stmt = con->createStatement();
    stmt->execute("USE testdb");
    sql::ResultSet *res_from_server = stmt->executeQuery(query_chat_id_opt1.c_str());
    while (res_from_server ->next()) {
        std::cout << res_from_server ->getString(1) << "  " << res_from_server ->getString(2) << std::endl;
                }
    delete res_from_server ;
    delete stmt;
	return 1;
    
}



void chatpool::sendRequest(string nickname, string receiver) {
    
    cout << "SERVER IS LISTENING THROUGH THE PORT: " << PORT << " WITHIN A LOCAL SYSTEM" << endl;
    // Укажем адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Зададим номер порта для соединения с сервером
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Создадим сокет 
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    // Установим соединение с сервером
    if (connect(socket_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0) {
        cout << endl << " Something went wrong Connection Failed" << endl;
        exit(1);
    }
// Запускаем функцию обработки сообщений от клиентов и ответа на эти сообщения
    cout << "CLIENT IS ESTABLISHING A CONNECTION WITH SERVER THROUGH PORT: " << PORT << endl;
//Converting nickname from string to char   
    int k;
    for (k = 0; k < sizeof(nickname); k++) {
        buffer[k] = nickname[k];
    }

    
    sendto(socket_descriptor, buffer, MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));

    char answer[4096];

    recvfrom(socket_descriptor, answer, sizeof(buffer), 0, nullptr, nullptr);
 
    //Converting receiver from string to char for comparison in strcmp  
    int l;
    char receiver_char [4096];
    for (l = 0; l < sizeof(receiver); l++) {
        receiver_char[l] = receiver[l];
    }

      
    if (strcmp(answer, receiver_char) == 0)
    {    
       
    while (1) {
        cout << "Enter a message you want to send to the server: " << endl;
        cin >> buffer;

        if (strcmp(buffer, "end") == 0) {
            sendto(socket_descriptor, buffer, MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));
            cout << "Client work is done.!" << endl;
            close(socket_descriptor);
 
           return;
        }
        else {
            sendto(socket_descriptor, buffer, MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));
            cout << "Message sent successfully to the server: " << buffer << endl;
            cout << "Waiting for the Response from Server..." << endl;
        }
        
      
        cout << "Message Received From Server: " << endl;
        recvfrom(socket_descriptor, buffer, sizeof(buffer), 0, nullptr, nullptr);
        cout << buffer << endl;
       
    }
    // закрываем сокет, завершаем соединение
    close(socket_descriptor);
    return;
    
    }
    
    if (strcmp(answer, receiver_char) != 0) {
        cout << "Sorry, the user you want to chat it with is not online. Please try later." << endl;
        sendto(socket_descriptor, "end", MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));
        close(socket_descriptor);
        return;
    }

}
    

