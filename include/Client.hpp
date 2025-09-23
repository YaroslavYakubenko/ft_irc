#pragma once

#include "Server.hpp"

class Client {
private:
	int			_fd;
	std::string	_nickname;
	std::string	_username;
	std::string	_hostname;
	std::string	_realname;
	bool		_registered;
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
	
	// std::vector<Client> _clients;
};