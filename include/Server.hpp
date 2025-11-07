#pragma once

#include <iostream>
#include <vector>
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
	int					_port;
	std::string			_password;
	int					_listener;
	std::vector<pollfd>	_fds;
	std::vector<Client> _clients;
	std::vector<Channel*>	_channels;

	void	initSocket();
	void	handleNewConnection();
	void	handleClient(size_t i);
public:
	Server(int port, const std::string &password);
	~Server();

	void	run();

	Client* findClientByNick(const std::string& nickname);
	const Client* findClientByNick(const std::string& nickname) const;
	void process_msg(int fd, char *buffer, size_t len);
	void Pass(Command *cmd);
	void execCmd(Command *cmd);

	Channel* findChannelByName(const std::string& channelName);
	void addChannel(Channel* channel);
	void joinChannel(Client* client, const std::string& channelName, const std::string& key);
	void sendError(Client* client, const std::string& code, const std::string& channel, const std::string& msg);
};