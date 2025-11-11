#pragma once

#include <vector>
#include <string>
#include "Client.hpp"

class Client;

class Command{
private:
    std::string cmd;
    std::vector<std::string>args;
    Client *client;
public:
    Command(){};
    Command(std::string cmd, std::vector<std::string>args, Client *client) : cmd(cmd), args(args), client(client){};
	~Command(){};

    std::string getCmd(){return this->cmd;}
    std::vector<std::string> getArgs(){return this->args;}

    Client *getClient(){return this->client;}
    void printCmd(){
        std::cout << "Command: " << this->cmd << std::endl;
        std::cout << "Args: " << std::endl;
        std::vector<std::string>::iterator it = this->args.begin();
        while(it != this->args.end())
        {
            std::cout << "Arg: " << *it << std::endl;
            it++;
        }
    }

};