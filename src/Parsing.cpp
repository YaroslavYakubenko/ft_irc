#include "../include/Parsing.hpp"

 void parse(const std::string &msg, Server *server, Client *client){
	std::cout << "PARSE" << std::endl;
     std::istringstream iss(msg);
     std::string cmd;
     std::vector<std::string>args;
     std::string arg;

     iss >> cmd;
     if(cmd == "CAP")
     client->clientParse(msg);
   // while(iss >> arg){
        
    //}
 }