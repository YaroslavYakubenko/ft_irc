#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "Client.hpp"

class Channel {
private:
	std::string				_name;
	std::string				_topic;
	std::string				_key;
	int						_userLimit;
	bool					_inviteOnly;
	bool					_topicLock;
	std::vector<Client*>	_clients;
	std::vector<Channel*>	_channels;
	std::set<Client*>		_operators;
	std::set<Client*>		_invited;
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
	bool hasClient(const Client* client) const;
	const std::vector<Client*>& getClients() const;
	const std::vector<Channel*>& getChannel() const; //?

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

	bool kick(Client* operatorClient, Client* targetClient, const std::string &comment);
	bool inviteCommand(Client* operatorClient, Client* targetClient);
	bool topicCommand(Client* client, const std::string &newTopic);
	bool modeCommand(Client* operatorClient, char mode, bool enable, const std::string &param);
	void privmsg(const Client& sender, const std::string& target, const std::string& message);

	Client* findClientByNick(const std::string& nickname);
	Channel* findChannelByName(const std::string& channelname);
};