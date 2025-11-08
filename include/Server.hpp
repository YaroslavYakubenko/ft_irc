#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <cerrno>
#include <fcntl.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "Parsing.hpp"
#include "Command.hpp"

#define BACKLOG 10
#define BUFFER_SIZE 1024

extern bool running;
class Client;
class Channel;
class Server {
private:
	int						_port;
	std::string				_password;
	int						_listener;
	std::vector<pollfd>		_fds;
	std::vector<Client> 	_clients;
	std::vector<Channel*>	_channels;
	std::map<int, std::string> _buffer;

	void	initSocket();
	void	handleNewConnection();
	void	handleClient(size_t i);
public:
	Server(int port, const std::string &password);
	~Server();

	void	run();

	Client* findClientByNick(const std::string& nickname);
	const Client* findClientByNick(const std::string& nickname) const;
	void process_msg(int fd, std::string msg);
	void Pass(Command *cmd);
	void execCmd(Command *cmd);
	Channel* findChannelByName(const std::string& channelName);
	void addChannel(Channel* channel);
	void removeChannel(Channel* channel);
	void joinChannel(Client* client, const std::string& channelName, const std::string& key);
	void sendError(Client* client, const std::string& code, const std::string& channel, const std::string& msg);
	void privmsg(const Client& sender, const std::string& target, const std::string& message);
	void printClients();
	bool checkUniqueClient(const std::string& nickname, const std::string& username); // delete
	bool checkUniqueNick(const std::string& nickname);
	bool checkUniqueUser(const std::string& username);
	void Nick(Command *cmd);
	void User(Command *cmd);
	void removeClient(Client* client);

};