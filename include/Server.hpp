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

#define BACKLOG 10
#define BUFFER_SIZE 1024

extern bool running;

class Server {
private:
	int					_port;
	std::string			_password;
	int					_listener;
	std::vector<pollfd>	_fds;

	void	initSocket();
	void	handleNewConnection();
	void	handleClient(size_t i);
public:
	Server(int port, const std::string &password);
	~Server();

	void	run();
};