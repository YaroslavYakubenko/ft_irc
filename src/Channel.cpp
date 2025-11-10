#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include <algorithm>

Channel::Channel(const std::string &name, Server* server) : _name(name), _topic(""), _key(""),
	_userLimit(0), _inviteOnly(false), _topicLock(false), _server(server) {}

Channel::~Channel() {}

const std::string &Channel::getName() const {return _name;}

const std::string &Channel::getTopic() const {return _topic;}

bool Channel::isInviteOnly() const {return _inviteOnly;}

bool Channel::isTopicLock() const {return _topicLock;}

bool Channel::checkKey(const std::string &key) const {return (_key.empty() || _key == key);}

int Channel::getUserLimit() const {return _userLimit;}

void Channel::addClient(Client* client) {
	if (!hasClient(client))
		_clients.push_back(client);
}

void Channel::removeClient(Client* client) {
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it == client) {
			_clients.erase(it);
			break;
		}
	}
	_operators.erase(client);
	_invited.erase(client);
}

bool Channel::hasClient(const Client* client) const {
	for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it == client)
			return true;
	}
	return false;
}

const std::vector<Client*>& Channel::getClients() const {return _clients;}

void Channel::addOperator(Client* client) {_operators.insert(client);}

void Channel::removeOperator(Client* client) {_operators.erase(client);}

bool Channel::isOperator(Client* client) const {return (_operators.find(client) != _operators.end());}

bool Channel::isInvited(Client* client) const {return (_invited.find(client) != _invited.end());}

void Channel::removeInvite(Client* client) {_invited.erase(client);}

void Channel::setTopic(const std::string &topic) {_topic = topic;}

void Channel::setInviteOnly(bool flag) {_inviteOnly = flag;}

void Channel::setTopicLock(bool flag) {_topicLock = flag;}

void Channel::setKey(const std::string &key) {_key = key;}

void Channel::clearKey() {_key.clear();}

void Channel::setUserLimit(int limit) {_userLimit = limit;}

void Channel::clearUserLimit() {_userLimit = 0;}

bool Channel::kick(Client* operatorClient, Client* targetClient, const std::string &comment) {
	if (!isOperator(operatorClient)) {
		_server->sendError(operatorClient, "482", _name, "You're not channel operator");
		return false;
	}
	if (!hasClient(targetClient)) {
		_server->sendError(operatorClient, "441", _name, "They aren't on that channel");
		return false;
	}
	std::string msg = ":" + operatorClient->getNickname() + " KICK " + _name + " " +
		targetClient->getNickname() + " :" + (comment.empty() ? "kicked" : comment) + "\r\n";
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0);
	removeClient(targetClient);
	return true;
}

bool Channel::inviteCommand(Client* operatorClient, Client* targetClient) {
	std::cout << "Operator: " << operatorClient->getUsername() << ", target: " << targetClient->getUsername() << std::endl;
	if (!isOperator(operatorClient)) {
		_server->sendError(operatorClient, "482", _name, "You don't have operator's rights");
		return false;
	}
	_invited.insert(targetClient);
	std::string msg = ":" + operatorClient->getNickname() + " INVITE " + targetClient->getNickname() + " :" + _name + "\r\n";
	send(targetClient->getFd(), msg.c_str(), msg.size(), 0);
	return true;
}

bool Channel::topicCommand(Client* client, const std::string &newTopic) {
	if (newTopic.empty()) {
		std::string msg = ":server 332 " + client->getNickname() + " " + _name + " :" + _topic + "\r\n";
		send(client->getFd(), msg.c_str(), msg.size(), 0);
		return true;
	}
	if (_topicLock && !isOperator(client)) {
		_server->sendError(client, "482", _name, "You don't have operator's rights");
		return false;
	}
	_topic = newTopic;
	std::string msg = ":" + client->getNickname() + " TOPIC " + _name + " :" + _topic + "\r\n";
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0);
	return true;
}

bool Channel::modeCommand(Client* operatorClient, char mode, bool enable, const std::string &param) {
	if (!isOperator(operatorClient)) {
		_server->sendError(operatorClient, "482", _name, "You don't have operator's rights");
		return false;
	}
	// Client* target = findClientByNick(param);
	// 	if (param != "" && !target) {
	// 		_server->sendError(operatorClient, "441", _name, "They aren't on that channel");
	// 		return false;
	// 	}
	switch (mode) {
		case 'i':
			if (!param.empty()) {
				_server->sendError(operatorClient, "472", _name, "Invite-only (+i) does noe require a parameter");
				return false;
			}
			_inviteOnly = enable;
			break;
		case 't':
			if (!param.empty()) {
				_server->sendError(operatorClient, "472", _name, "Topic lock (+t) does noe require a parameter");
				return false;
			}
			_topicLock = enable;
			break;
		case 'k':
			if (enable) {
				if (!param.empty()) {
					_server->sendError(operatorClient, "461", _name, "Not enough parameters for +k");
					return false;
				}
				_key = param;
			}
			else
				_key.clear();
			break;
		case 'o':
			if (param.empty()) {
				_server->sendError(operatorClient, "461", _name, "Not enough parameters for +o/-o");
				return false;
			}
			{
				Client* target = findClientByNick(param);
				if (!target) {
					_server->sendError(operatorClient, "441", _name, "They aren't on that channel");
					return false;
				}
				if (enable)
					addOperator(target);
				else {
					if (_operators.size() > 1)
						removeOperator(target);
					else
						return false;
				}
			}
			break;
		case 'l':
			if (enable) {
				int lim = std::atoi(param.c_str());
				if (lim <= 0) {
					_server->sendError(operatorClient, "472", _name, "Limit (+l) must be positive");
                    return false;
				}
				_userLimit = lim;
				std::cout << "Limit for channel " << _name << ": " << _userLimit << std::endl;
			}
			else {
				if (!param.empty()) {
					_server->sendError(operatorClient, "472", _name, "Removing limit (-l) does not require a parameter");
                    return false;
				}
				_userLimit = 0;
			}
			break;
		default:
			_server->sendError(operatorClient, "472", _name, "Unknown mode");
			return false;
	}
	std::string msg = ":" + operatorClient->getNickname() + " MODE " + _name + " " +
		(std::string(enable ? "+" : "-") + mode + (param.empty() ? "" : " " + param)) + "\r\n";
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0);
	return true;
}

Client* Channel::findClientByNick(const std::string& nickname) {
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getNickname() == nickname)
			return _clients[i];
		}
	return NULL;
}

std::set<Client*> Channel::getOperators(){return _operators;}

void Channel::printOperators(){
	std::cout << "Operators in channel:\n";
for (std::set<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
    Client* client = *it;
    if (client)
        std::cout << " - " << client->getNickname() << std::endl;
}
}