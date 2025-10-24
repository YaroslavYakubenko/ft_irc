#pragma once

#include <string>
#include "../include/Server.hpp"
#include <sstream>

class Server;

class Client {
private:
	int			_fd;
	std::string	_nickname;
	std::string	_username;
	std::string	_hostname;
	std::string	_realname;
	bool		_registered;
	std::string	_recv_buffer;
	std::string	_send_buffer;
	bool		_pass_ok;
public:
	Client();
	Client(int fd);
	Client(const Client& other);
	Client& operator=(const Client& other);
	~Client();
	
	int			getFd() const;
	std::string	getNickname() const;
	std::string	getUsername() const;
	std::string	getHostname() const;
	std::string	getRealname() const;
	bool		isRegistered() const;
	
	void	setFd(int fd);
	void	setNickname(const std::string& nickname);
	void	setUsername(const std::string& username);
	void	setHostname(const std::string& hostname);
	void	setRealname(const std::string& realname);
	void	setRegistered(bool registered);

	// void	setInfo(char *buffer, size_t bytes);
	void 	clientParse(const std::string &msg);

	bool	popLine(std::string &out);
};