#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include <algorithm>

Channel::Channel(const std::string &name) : _name(name), _topic(""), _key(""),
	_userLimit(0), _inviteOnly(false), _topicLock(false) {}

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

bool Channel::hasClient(Client* client) const {
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

void Channel::invite(Client* client) {_invited.insert(client);}

bool Channel::isInvited(Client* client) const {return (_invited.find(client) != _invited.end());}

void Channel::setTopic(const std::string &topic) {_topic = topic;}

void Channel::setInviteOnly(bool flag) {_inviteOnly = flag;}

void Channel::setTopicLock(bool flag) {_topicLock = flag;}

void Channel::setKey(const std::string &key) {_key = key;}

void Channel::clearKey() {_key.clear();}

void Channel::setUserLimit(int limit) {_userLimit = limit;}

void Channel::clearUserLimit() {_userLimit = 0;}

bool Channel::kick(Client* operatorClient, Client* targetClient, const std::string &comment) {
	if (!isOperator(operatorClient))
		return false;
	if (!hasClient(targetClient))
		return false;
	std::string msg = ":" + operatorClient->getNickname() + " KICK " + _name + " " +
		targetClient->getNickname() + " :" + (comment.empty() ? "kicked" : comment) + "\r\n";
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0);
	removeClient(targetClient);
	return true;
}

bool Channel::inviteCommand(Client* operatorClient, Client* targetClient) {
	if (!isOperator(operatorClient))
		return false;
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
	if (_topicLock && !isOperator(client))
		return false;
	_topic = newTopic;
	std::string msg = ":" + client->getNickname() + " TOPIC " + _name + " :" + _topic + "\r\n";
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0);
	return true;
}

bool Channel::modeCommand(Client* operatorClient, char mode, bool enable, const std::string &param) {
	if (!isOperator(operatorClient))
		return false;
	switch (mode) {
		case 'i': _inviteOnly = enable; break;
		case 't': _topicLock = enable; break;
		case 'k':
			if (enable)
				_key = param;
			else
				_key.clear();
			break;
		case 'o':
			if (enable)
				addOperator((Client*)param.c_str());
			else
				removeOperator((Client*)param.c_str());
			break;
		case 'l':
			if (enable)
				_userLimit = std::atoi(param.c_str());
			else
				_userLimit = 0;
			break;
		default: return false;
	}
	std::string msg = ":" + operatorClient->getNickname() + " MODE " + _name + " " +
		(std::string(enable ? "+" : "-") + mode + (param.empty() ? "" : " " + param)) + "\r\n";
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0);
	return true;
}