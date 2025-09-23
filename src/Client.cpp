#include "Client.hpp"

Client::Client() : _fd(-1), _registered(false) {}

Client::Client(int fd) : _fd(fd), _registered(false) {}

Client::Client(const Client& other) {
	*this = other;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		_fd = other._fd;
		_nickname = other._nickname;
		_username = other._username;
		_hostname = other._hostname;
		_realname = other._realname;
		_registered = other._registered;
	}
	return *this;
}

Client::~Client() {}

int Client::getFd() const {return _fd;}
std::string Client::getNickname() const {return _nickname;}
std::string Client::getUsername() const {return _username;}
std::string Client::getHostname() const {return _hostname;}
std::string Client::getRealname() const {return _realname;}
bool Client::isRegistered() const {return _registered;}

void Client::setFd(int fd) {_fd = fd;}
void Client::setNickname(const std::string& nickname) {_nickname = nickname;}
void Client::setUsername(const std::string& username) {_username = username;}
void Client::setHostname(const std::string& hostname) {_hostname = hostname;}
void Client::setRealname(const std::string& realname) {_realname = realname;}
void Client::setRegistered(bool registered) {_registered = registered;}