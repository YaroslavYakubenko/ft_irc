#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

int main() {
	Client operatorClient;
	operatorClient.setNickname("Oper");
	Client targetClient;
	targetClient.setNickname("Target");
	Channel channel("#test");
	channel.addClient(&operatorClient);
	channel.addClient(&targetClient);
	channel.addOperator(&operatorClient);


	if (channel.inviteCommand(&operatorClient, &targetClient))
		std::cout << "inviteCommand works OK\n" << std::endl;
	else
		std::cout << "inviteCommand doesn't work\n" << std::endl;


	if (channel.topicCommand(&operatorClient, "New topic"))
		std::cout << "topicCommand (set, operator) was changed\n" << std::endl;
	else {
		std::cout << "topicCommand (set, operator) wasn't changed\n" << std::endl;
		std::cout << "topicCommand (get) successful" << std::endl;
	}


	if (channel.modeCommand(&operatorClient, 'i', true, ""))
		std::cout << "+i modeCommand done\n" << std::endl;
	if (channel.modeCommand(&operatorClient, 't', true, "")) {
		if (channel.topicCommand(&targetClient, "Hacked"))
			std::cout << "+t topicCommand(set, not operator) was hacked!!!" << std::endl;
		else
			std::cout << "+t topicCommand(set, not operator) was blocked" << std::endl;
	}
	if (channel.modeCommand(&operatorClient, 't', false, "")) {
		if (channel.topicCommand(&targetClient, "Hacked"))
			std::cout << "-t topicCommand (set, not operator) was hacked!!!\n" << std::endl;
		else
			std::cout << "-t topicCommand (set, not operator) was blocked\n" << std::endl;
	}
	if (channel.modeCommand(&operatorClient, 'k', true, "key123"))
		std::cout << "+k modeCommand done" << std::endl;
	if (channel.modeCommand(&operatorClient, 'k', false, ""))
		std::cout << "-k modeCommand done\n" << std::endl;


	Client user;
	user.setNickname("User");
	channel.addClient(&user);
	if (channel.modeCommand(&operatorClient, 'o', true, user.getNickname()))
		std::cout << "+o modeCommand done: " << user.getNickname() << " got operator" << std::endl;
	else
		std::cout << "+o modeCommand didn't work" << std::endl;
	if (!channel.isOperator(&user))
		std::cout << user.getNickname() << " now is operator" << std::endl;
	else
		std::cout << user.getNickname() << " still not operator" << std::endl;
	if (channel.modeCommand(&operatorClient, 'o', false, user.getNickname()))
		std::cout << "-o modeCommand done: " << user.getNickname() << " not operator anymore" << std::endl;
	else
		std::cout << "-o modeCommand didn't work" << std::endl;
	if (!channel.isOperator(&user))
		std::cout << user.getNickname() << " not operator anymore\n" << std::endl;
	else
		std::cout << user.getNickname() << " still operator\n" << std::endl;


	if (channel.kick(&operatorClient, &targetClient, "Goodbye!"))
		std::cout << "kick worked successful" << std::endl;
	else
		std::cout << "kick didn't work" << std::endl;
	if (!channel.hasClient(&targetClient))
		std::cout << "client was deleted" << std::endl;
	else 
		std::cout << "client didn't delete" << std::endl;
	return 0;
}