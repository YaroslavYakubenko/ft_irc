#include "../include/Server.hpp"
#include "../include/Client.hpp"

bool running = true;

Server::Server(int port, const std::string &password) : _port(port), _password(password), _listener(-1) {
	initSocket();
}

Server::~Server() {
	for (size_t i = 0; i < _clients.size(); ++i)
		close(_clients[i].getFd());
	if (_listener >= 0)
		close(_listener);
}

static int setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::initSocket() {
	_listener = socket(AF_INET, SOCK_STREAM, 0);
	if (setNonBlocking(_listener) < 0) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	if (_listener < 0) { // TODO: shouldn't it be right after creation of the socket?
		perror("socket");
		exit(EXIT_FAILURE);
	}
	int yes = 1;
	if (setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
		perror("setsockopt");
		close(_listener);
		exit(EXIT_FAILURE);
	}
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);
	if (bind(_listener, (sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(_listener);
		exit(EXIT_FAILURE);
	}
	if (listen(_listener, BACKLOG) < 0) {
		perror("listen");
		close(_listener);
		exit(EXIT_FAILURE);
	}
	std::cout << "Server started on port " << _port << std::endl;
	pollfd server_fd;
	server_fd.fd = _listener;
	server_fd.events = POLLIN;
	server_fd.revents = 0;
	_fds.push_back(server_fd);
}

void Server::run() {
	// char buffer[BUFFER_SIZE];
	// bool running = true;
	while (running) { // TODO: can't it be while(1)?
		int ret = poll(_fds.data(), _fds.size(), -1);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			perror("poll");
			break;
		}
		// TODO: split listener and clients check (if (_fds[i].fd == _listener))
		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _listener) {
					handleNewConnection();
				} else {
					handleClient(i);
				}
			}
		}
	}
	for (size_t i = 0; i < _fds.size(); ++i)
		close(_fds[i].fd);
	_fds.clear();
}

void Server::handleNewConnection() {
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(_listener, (sockaddr*)&client_addr, &client_len); // should we not check if accept returns an error
	setNonBlocking(client_fd);
	if (client_fd >= 0) {
		Client new_client(client_fd);
		_clients.push_back(new_client);
		pollfd client_fd_struct;
		client_fd_struct.fd = client_fd;
		client_fd_struct.events = POLLIN;
		client_fd_struct.revents = 0;
		_fds.push_back(client_fd_struct);
		//char buffer[BUFFER_SIZE];
		//std::memset(buffer, 0, BUFFER_SIZE);
		//ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
		// BUFFER IS EMPTY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//std::cout << "handle NEW CONNECTION buffer = " << buffer << std::endl;
		//new_client.setInfo(buffer, bytes);
		std::cout << "New client connected: fd=" << client_fd << std::endl;
	} // TODO: add error msg if accept returns -1
}

void Server::Pass(Command *cmd){
	std::cout << "INSIDE PASS" << std::endl;
	Client *client = cmd->getClient();
	std::vector<std::string>args = cmd->getArgs();
	std::vector<std::string>::iterator it = args.begin();
    while(it != args.end())
    {
        if(*it == "NICK")
			client->setNickname(*(++it));
		if(*it == "USER")
			client->setUsername(*(++it));
		it++;
    }
	std::cout << "New client info:" << std::endl;
		std::cout << "NICK: " << client->getNickname() << std::endl;
		std::cout << "User: " << client->getUsername() << std::endl;
	
	/*std::cout << "CLIENT PARSE:" << std::endl;
	std::cout << "MSG came like : " << msg << std::endl;
	std::istringstream iss(msg);
    std::string token;
	iss >> token;
	if(token == "CAP"){
		std::cout << "INSIDE CLIENTPARSE" << std::endl;
		while(iss >> token){
		if(token == "NICK"){
			iss >> token;
			std::cout << "token for NICK = " << token << std::endl;
			setNickname(token);
		}
		if(token == "USER"){
			iss >> token;
			std::cout << "token for USER = " << token << std::endl;
			setUsername(token);
		}				
		}*/
		//}
	
}

void Server::execCmd(Command *cmd){
	std::string mycmd = cmd->getCmd();
	if(mycmd == "CAP")
		Pass(cmd);
	if(mycmd == "JOIN")
		
}

void Server::process_msg(int fd, char *buffer, size_t len){
	std::cout << "PROCCESS_MSG" << std::endl;
	char ss[512];
	strncpy(ss, buffer, len);
	ss[len] = '\0';
	Client * client_ptr;
	for (size_t j = 0; j < _clients.size(); ++j) {
			if (_clients[j].getFd() == fd) {
				client_ptr = &_clients[j];
				break;
			}
	}
	Command cmd = parse(ss, this, client_ptr);
	cmd.printCmd();
	execCmd(&cmd);
}

void Server::handleClient(size_t i) {
	int	fd = _fds[i].fd;
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	ssize_t bytes = recv(_fds[i].fd, buffer, BUFFER_SIZE - 1, 0);
	if (bytes <= 0) { // TODO: 0 means client disconnected, <0 means error and it has errno
		std::cout << "Client dissconnected: fd=" << _fds[i].fd << std::endl;
		close(fd);
		_fds.erase(_fds.begin() + i);
		for (size_t j = 0; j < _clients.size(); ++j) {
			if (_clients[j].getFd() == fd) {
				_clients.erase(_clients.begin() + j);
				break;
			}
		}
	} else {
		std::string msg(buffer, bytes);
		Client* client_ptr = NULL;
		for (size_t j = 0; j < _clients.size(); ++j) {
			if (_clients[j].getFd() == fd) {
				client_ptr = &_clients[j];
				break;
			}
		}
		if (client_ptr) {
			std::cout << "Recieved from fd=" << fd << ":" << msg;
			std::string reply = "Recieved: " + msg;
			process_msg(fd, buffer, bytes);
			send(fd, reply.c_str(), reply.size(), 0);
		} // TODO: add error msg
	}
}

Client* Server::findClientByNick(const std::string& nickname) {
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i].getNickname() == nickname)
			return &_clients[i];
	}
	return NULL;
}

const Client* Server::findClientByNick(const std::string& nickname) const {
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i].getNickname() == nickname)
			return &_clients[i];
	}
	return NULL;
}