#include "../include/Server.hpp"
#include "../include/Client.hpp"

bool running = true;

Server::Server(int port, const std::string &password) : _port(port), _password(password), _listener(-1) {
	initSocket();
}

Server::~Server() {
	for (size_t i = 0; i < _clients.size(); ++i)
		close(_clients[i]->getFd());
	if (_listener >= 0)
		close(_listener);
	for (size_t i = 0; i < _channels.size(); ++i)
		delete _channels[i];
	_channels.clear();
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
	while (running) {
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

void Server::printClients(){
	std::cout << "Clients list:" << std::endl;
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		std::cout << (*it)->getUsername() << std::endl;
	}
}

void Server::handleNewConnection() {
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(_listener, (sockaddr*)&client_addr, &client_len); // FIXME: should we not check if accept returns an error
	setNonBlocking(client_fd);
	if (client_fd >= 0) {
		Client* new_client = new Client(client_fd);
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
		printClients();
	} // TODO: add error msg if accept returns -1
}

bool Server::checkUniqueClient(const std::string& nickname, const std::string& username) {
	for (size_t i = 0; i < _clients.size(); ++i)
		if (_clients[i]->getNickname() == nickname || _clients[i]->getUsername() == username)
			return 1;
	return 0;
}

bool Server::checkUniqueNick(const std::string& nickname) {
	for (size_t i = 0; i < _clients.size(); ++i)
		if (_clients[i]->getNickname() == nickname)
			return 1;
	return 0;
}

bool Server::checkUniqueUser(const std::string& username) {
	for (size_t i = 0; i < _clients.size(); ++i)
		if (_clients[i]->getUsername() == username)
			return 1;
	return 0;
}

void Server::removeClient(Client* client) {
	std::string msg = "Your nick or user is already taken. Please change it and try to connect again!";
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->getFd() == client->getFd()) {
			send((*it)->getFd(), msg.c_str(), msg.size(), 0);
			//usleep(100000);
			close((*it)->getFd());
			_clients.erase(it);
			break;
		}
	}
}

/*void Server::Pass(Command *cmd){
	std::cout << "INSIDE PASS" << std::endl;
	Client *client = cmd->getClient();
	std::string nick, user;
	std::vector<std::string>args = cmd->getArgs();
	std::vector<std::string>::iterator it = args.begin();
    while(it != args.end())
    {
        if(*it == "NICK")
			nick = *(++it);
		if(*it == "USER")
			user = *(++it);
		it++;
    }
	if(checkUniqueClient(nick, user)){
		std::cout << "***CLIENT IS NOT UNIQUE, YOU WILL BE DISCONNECTED!***" << std::endl;
		removeClient(client);
		//disconnect client and tell him so <----------------------------------------here
	} else{
		client->setNickname(nick);
		client->setUsername(user);
		std::cout << "New client info:" << std::endl;
		std::cout << "NICK: " << client->getNickname() << std::endl;
		std::cout << "User: " << client->getUsername() << std::endl;
	}
	printClients();
}*/

void Server::Nick(Command *cmd){
	std::cout << "INSIDE NICK" << std::endl;
	Client *client = cmd->getClient();
	std::vector<std::string>args = cmd->getArgs();
	std::string nick = args[0];
	std::cout << "NICK IS " << nick << std::endl;

	if(checkUniqueNick(nick)){
		std::cout << "***NICK IS NOT UNIQUE, YOU WILL BE DISCONNECTED!***" << std::endl;
		removeClient(client);
		//disconnect client and tell him so <----------------------------------------here
	} else{
		client->setNickname(nick);
		std::cout << "New client info:" << std::endl;
		std::cout << "NICK: " << client->getNickname() << std::endl;

	}
}

void Server::User(Command *cmd){
	std::cout << "INSIDE USER" << std::endl;
	Client *client = cmd->getClient();
	std::vector<std::string>args = cmd->getArgs();
	std::string user = args[0];
	std::cout << "USER IS " << user << std::endl;

	if(checkUniqueUser(user)){
		std::cout << "***USER IS NOT UNIQUE, YOU WILL BE DISCONNECTED!***" << std::endl;
		removeClient(client);
		//disconnect client and tell him so <----------------------------------------here
	} else{
		client->setUsername(user);
		std::cout << "USER: " << client->getUsername() << std::endl;

	}
}

void Server::invite(Command *cmd){
	std::vector<std::string>args = cmd->getArgs();
	std::cout << "INVITE1" << std::endl;
	Channel *target_chan = findChannelByName(args[1]);
	std::cout << "INVITE2" << std::endl;
	Client *target_cli = findClientByNick(args[0]);
	std::cout << "INVITE3" << std::endl;
	if(!target_cli){
		sendError(cmd->getClient(), "401", args[0], "No such nick/channel");
		return;
	}
	if(!target_chan){
		sendError(cmd->getClient(), "401", args[1], "No such nick/channel");
		return;
	}
	std::cout << "INVITE4" << std::endl;
	target_chan->inviteCommand(cmd->getClient(), target_cli);
	std::cout << "INVITE5" << std::endl;
}

void Server::mode(Command *cmd){
  std::vector<std::string>args = cmd->getArgs();
  if(args.size() < 2)
    return;
  Channel *target_chan = findChannelByName(args[0]);
  bool enable = 1; // what if not -+, can you make int enable = -1 for default
  std::string opt = args[1];
  if(opt[0] == '-')
    enable = 0;
  char mode = opt[1];
  if(args.size() < 3){
    std::cout << "PARAM IS EMPTY" << std::endl;
    args.resize(3);
    args[2] = "";
    }
  target_chan->modeCommand(cmd->getClient(), mode, enable, args[2]);
}

void Server::kick(Command *cmd){
	std::vector<std::string>args = cmd->getArgs();
	Channel *target_chan = findChannelByName(args[0]);
	Client *target_cli = findClientByNick(args[2]);
	if(args.size() < 3){
		std::cout << "PARAM IS EMPTY" << std::endl;
		args.resize(3);
		args[3] = "";
    }

	target_chan->kick(cmd->getClient(), target_cli, args[2]);
}

void Server::execCmd(Command *cmd){ 
	std::string mycmd = cmd->getCmd();
	std::vector<std::string>args = cmd->getArgs();
	
	if(mycmd == "NICK")
		Nick(cmd);
	if(mycmd == "USER")
		User(cmd);
	if(mycmd == "PRIVMSG"){
		std::cout << "Arg[1] = " << args[0] << "Arg[2] = " << args[1] << std::endl;
		privmsg(*cmd->getClient(), args[0], args[1]);
	}
	if(mycmd == "JOIN"){
		std::cout << "SEG CHECK 1" << std::endl;
		if(args.size() < 2){
		std::cout << "KEY IS EMPTY" << std::endl;
		args.resize(2);
		args[1] = "";
		}

		joinChannel(cmd->getClient(), args[0], args[1]);
  }
	if(mycmd == "INVITE")
		invite(cmd);
	if(mycmd == "MODE")
		mode(cmd);
	if(mycmd == "KICK")
  		kick(cmd);	
}

void Server::process_msg(int fd, std::string msg){
	std::cout << "PROCCESS_MSG" << std::endl;
	//char ss[512];
	//strncpy(ss, buffer, len);
	//ss[len] = '\0';
	Client * client_ptr;
	for (size_t j = 0; j < _clients.size(); ++j) {
			if (_clients[j]->getFd() == fd) {
				client_ptr = _clients[j];
				break;
			}
	}
	Command cmd = parse(msg.c_str(), this, client_ptr);
	cmd.printCmd();
	execCmd(&cmd);
}

void Server::handleClient(size_t i) {
	int	fd = _fds[i].fd;
	std::string msg;
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	ssize_t bytes = recv(_fds[i].fd, buffer, BUFFER_SIZE - 1, 0);
	if (bytes <= 0) { // TODO: 0 means client disconnected, <0 means error and it has errno
		std::cout << "Client dissconnected: fd=" << _fds[i].fd << std::endl;
		_buffer.erase(fd);
		close(fd);
		_fds.erase(_fds.begin() + i);
		for (size_t j = 0; j < _clients.size(); ++j) {
			if (_clients[j]->getFd() == fd) {
				_clients.erase(_clients.begin() + j);
				break;
			}
		}
	} else {
		_buffer[fd].append(buffer, bytes);

		// Process all complete IRC messages
		size_t pos;
		while ((pos = _buffer[fd].find("\r\n")) != std::string::npos) {
			msg = _buffer[fd].substr(0, pos);
			_buffer[fd].erase(0, pos + 2);

			std::cout << "Received from fd=" << fd << ": " << msg << std::endl;
			process_msg(fd, msg);

		}
	}
}

Client* Server::findClientByNick(const std::string& nickname) {
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getNickname() == nickname)
			return _clients[i];
	}
	return NULL;
}

Channel* Server::findChannelByName(const std::string& chhanelName) {
	for (size_t i = 0; i < _channels.size(); ++i) {
		if (_channels[i]->getName() == chhanelName)
			return _channels[i];
		}
	return NULL;
}

void Server::addChannel(Channel* channel) {
	_channels.push_back(channel);
}

void Server::removeChannel(Channel* channel) {
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it == channel) {
			delete *it;
			_channels.erase(it);
			break;
		}
	}
}

void Server::joinChannel(Client* client, const std::string& channelName, const std::string& key) {
	std::cout << "INSIDE JOIN " << channelName << " " << key << std::endl;
	if(channelName[0] != '#' && channelName[0] != '!' && channelName[0] != '+' && channelName[0] != '&'){
		sendError(client, "479", channelName, "Illegal channel name");
		return;
	}

	Channel* channel = findChannelByName(channelName);
	if (!channel) {
		std::cout << "HERE1!" << std::endl;
		channel = new Channel(channelName, this);
		addChannel(channel);
		if (key != "")
			channel->setKey(key);
		channel->addClient(client);
		channel->addOperator(client);
		channel->printOperators();
		std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN :" + channelName + "\r\n";
		send(client->getFd(), joinMsg.c_str(), joinMsg.size(), 0);
		std::string names = ":server 353 " + client->getNickname() + " = " + channelName + " :" + client->getNickname() + "\r\n";
		send(client->getFd(), names.c_str(), names.size(), 0);
		std::string endMsg = ":server 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
		send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
	} else {
		std::cout << "HERE2!" << std::endl;
		if (!channel) {
			sendError(client, "403", channelName, "No such channel");
			return;
		}
		if (channel->isInviteOnly() && !channel->isInvited(client)) {
			sendError(client, "473", channelName, "Cannot join channel (+i)");
			return;
		}
		if (!channel->checkKey(key)) {
			sendError(client, "475", channelName, "Cannot join channel (+k)");
			return;
		}
		if (channel->getUserLimit() > 0 && channel->getClients().size() >=
			static_cast<size_t>(channel->getUserLimit())) {
			sendError(client, "471", channelName, "Cannot join channel (+l)");
			return;
		}
		if (channel->hasClient(client)){
			sendError(client, "443", channelName, "is already on channel");	
			return;
		}
		std::cout << "HERE3!" << std::endl;
		channel->addClient(client);
		if (channel->isInviteOnly() && channel->isInvited(client))
			channel->removeInvite(client);
		std::string joinMsg = ":" + client->getNickname() + " JOIN " + channelName + "\r\n";
		const std::vector<Client*>& members = channel->getClients();
		std::cout << "HERE4!" << std::endl;
		for (size_t i = 0; i < members.size(); ++i)
			send(members[i]->getFd(), joinMsg.c_str(), joinMsg.size(), 0);
		channel->topicCommand(client, "");
		std::string names = ":server 353 " + client->getNickname() + " = " + channelName + " :";
		std::cout << "HERE5!" << std::endl;
		for (size_t i = 0 ; i < members.size(); ++i)
			names += members[i]->getNickname() + " ";
		names += "\r\n";
		send(client->getFd(), names.c_str(), names.size(), 0);
		std::string endMsg = ":server 366 " + client->getNickname() + "  " + channelName + " :End of /name list\r\n";
		send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
		std::cout << "HERE6!" << std::endl;
	}
}

void Server::sendError(Client* client, const std::string& code, const std::string& channel, const std::string& msg) {
	std::string err = ":server " + code + " " + client->getNickname() + " " + channel + " :" + msg + "\r\n";
	send(client->getFd(), err.c_str(), err.size(), 0);
}

void Server::privmsg(const Client& sender, const std::string& target, const std::string& message) {
	std::cout << "INSIDE PRIVMSG 1!!!" << std::endl;
	if (target.empty() || message.empty())
		return;
	if (target[0] == '#') {
		Channel* channel = findChannelByName(target);
		if (!channel || !channel->hasClient(&sender))
			return;
		std::string msg = ":" + sender.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
		const std::vector<Client*>& clients = channel->getClients();
		for (size_t i = 0; i < clients.size(); ++i) {
			if (clients[i] != &sender)
				send(clients[i]->getFd(), msg.c_str(), msg.size(), 0);
		}
	} else {
		std::cout << "INSIDE PRIVMSG 2!!!" << std::endl;
		printClients();
		Client* recipient = findClientByNick(target);
		if (!recipient)
			return;
		std::cout << "INSIDE PRIVMSG 3!!!" << std::endl;
		std::string msg = ":" + sender.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
		std::cout << "fd = " << recipient->getFd() << ", msg = " << msg.c_str() << ", size = " << msg.size() << std::endl;
		send(recipient->getFd(), msg.c_str(), msg.size(), 0);
	}
}