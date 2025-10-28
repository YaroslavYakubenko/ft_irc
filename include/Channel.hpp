#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "Client.hpp"
#include "Command.hpp"

class Client;
class Command;

class Channel {
private:
	std::string				_name;
	std::string				_topic;
	std::string				_key;
	int						_userLimit;
	bool					_inviteOnly;
	bool					_topicLock;
	std::vector<Client*>	_clients;
	std::set<Client*>		_operators;
	std::set<Client*>		_invited;
	int						_last_cmd;
	Command					_cmd; // should it stay?

public:
	Channel(const std::string &name);
	~Channel();

	const std::string &getName() const;
	const std::string &getTopic() const;
	bool isInviteOnly() const;
	bool isTopicLock() const;
	bool checkKey(const std::string &key) const;
	int getUserLimit() const;

	void addClient(Client* client);
	void removeClient(Client* client);
	bool hasClient(Client* client) const;
	const std::vector<Client*>& getClients() const;

	void addOperator(Client* client);
	void removeOperator(Client* client);
	bool isOperator(Client* client) const;

	void invite(Client* client);
	bool isInvited(Client* client) const;

	void setTopic(const std::string &topic);
	void setInviteOnly(bool flag);
	void setTopicLock(bool flag);
	void setKey(const std::string &key);
	void clearKey();
	void setUserLimit(int limit);
	void clearUserLimit();

	//void execCmd();
	//void Pass();


	bool kick(Client* operatorClient, Client* targetClient, const std::string &comment);
	bool inviteCommand(Client* operatorClient, Client* targetClient);
	bool topicCommand(Client* client, const std::string &newTopic);
	bool modeCommand(Client* operatorClient, char mode, bool enable, const std::string &param);
};