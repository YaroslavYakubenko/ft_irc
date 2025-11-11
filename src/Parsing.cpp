#include "../include/Parsing.hpp"

Command parse(const std::string &msg, Client *client){
	std::cout << "PARSE" << std::endl;
    std::istringstream iss(msg);
    std::string cmd;
	std::vector<std::string>args;
    std::string arg;

	iss >> cmd;
    while(iss >> arg){
		if(arg[0] == ':'){
			std::string rest;
			std::getline(iss, rest);
			arg = arg.substr(1) + rest;
		if (!arg.empty() && (arg[arg.size() - 1] == '\r' || arg[arg.size() - 1] == '\n'))
    		arg.erase(arg.size() - 1);
			args.push_back(arg);
			break;
		}
    	args.push_back(arg);
    }
	Command command(cmd, args, client);
	return command;
	//execute
 }