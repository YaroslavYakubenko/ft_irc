#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

int main() {
	// Client operatorClient;
	// operatorClient.setNickname("Oper");
	// Client targetClient;
	// targetClient.setNickname("Target");
	// Channel channel("#test");
	// channel.addClient(&operatorClient);
	// channel.addClient(&targetClient);
	// channel.addOperator(&operatorClient);


	// if (channel.inviteCommand(&operatorClient, &targetClient))
	// 	std::cout << "inviteCommand works OK\n" << std::endl;
	// else
	// 	std::cout << "inviteCommand doesn't work\n" << std::endl;


	// if (channel.topicCommand(&operatorClient, "New topic"))
	// 	std::cout << "topicCommand (set, operator) was changed\n" << std::endl;
	// else {
	// 	std::cout << "topicCommand (set, operator) wasn't changed\n" << std::endl;
	// 	std::cout << "topicCommand (get) successful" << std::endl;
	// }


	// if (channel.modeCommand(&operatorClient, 'i', true, ""))
	// 	std::cout << "+i modeCommand done\n" << std::endl;
	// if (channel.modeCommand(&operatorClient, 't', true, "")) {
	// 	if (channel.topicCommand(&targetClient, "Hacked"))
	// 		std::cout << "+t topicCommand(set, not operator) was hacked!!!" << std::endl;
	// 	else
	// 		std::cout << "+t topicCommand(set, not operator) was blocked" << std::endl;
	// }
	// if (channel.modeCommand(&operatorClient, 't', false, "")) {
	// 	if (channel.topicCommand(&targetClient, "Hacked"))
	// 		std::cout << "-t topicCommand (set, not operator) was hacked!!!\n" << std::endl;
	// 	else
	// 		std::cout << "-t topicCommand (set, not operator) was blocked\n" << std::endl;
	// }
	// if (channel.modeCommand(&operatorClient, 'k', true, "key123"))
	// 	std::cout << "+k modeCommand done" << std::endl;
	// if (channel.modeCommand(&operatorClient, 'k', false, ""))
	// 	std::cout << "-k modeCommand done\n" << std::endl;


	// Client user;
	// user.setNickname("User");
	// channel.addClient(&user);
	// if (channel.modeCommand(&operatorClient, 'o', true, user.getNickname()))
	// 	std::cout << "+o modeCommand done: " << user.getNickname() << " got operator" << std::endl;
	// else
	// 	std::cout << "+o modeCommand didn't work" << std::endl;
	// if (!channel.isOperator(&user))
	// 	std::cout << user.getNickname() << " now is operator" << std::endl;
	// else
	// 	std::cout << user.getNickname() << " still not operator" << std::endl;
	// if (channel.modeCommand(&operatorClient, 'o', false, user.getNickname()))
	// 	std::cout << "-o modeCommand done: " << user.getNickname() << " not operator anymore" << std::endl;
	// else
	// 	std::cout << "-o modeCommand didn't work" << std::endl;
	// if (!channel.isOperator(&user))
	// 	std::cout << user.getNickname() << " not operator anymore\n" << std::endl;
	// else
	// 	std::cout << user.getNickname() << " still operator\n" << std::endl;


	// if (channel.kick(&operatorClient, &targetClient, "Goodbye!"))
	// 	std::cout << "kick worked successful" << std::endl;
	// else
	// 	std::cout << "kick didn't work" << std::endl;
	// if (!channel.hasClient(&targetClient))
	// 	std::cout << "client was deleted" << std::endl;
	// else 
	// 	std::cout << "client didn't delete" << std::endl;



	std::cout << "Open channel" << std::endl;
	Server server;
	Client openUser;
	openUser.setNickname("OpenUser");
	server.joinChannel(&openUser, "#openChannel", "");
	Channel* openChannel = server.findChannelByName("#openChannel");
	if (openChannel && openChannel->hasClient(&openUser))
		std::cout << "OpenUser could join to channel\n" << std::endl;
	else
		std::cout << "OpenUser could not join to channel\n" << std::endl;
	

		
	std::cout << "Channel with key" << std::endl;
	Server serverPass;
	Client passUser;
	passUser.setNickname("passUser");
	serverPass.joinChannel(&passUser, "#passChannel", "key");
	Channel* passChannel = serverPass.findChannelByName("#passChannel");
	if (passChannel && passChannel->hasClient(&passUser))
		std::cout << "PassUser could create a channel" << std::endl;
	else
		std::cout << "PassUser could not create a channel" << std::endl;
	Client guest;
	guest.setNickname("Guest");
	std::cout << "Trying to enter with wrong pass" << std::endl;
	serverPass.joinChannel(&guest, "#passChannel", "wrongKey");
	if (!passChannel->hasClient(&guest))
		std::cout << "Guest could not join becouse of wrong pass" << std::endl;
	else
		std::cout << "Guest could join - ERROR" << std::endl;
	std::cout << "Trying to enter with correct pass" << std::endl;
	serverPass.joinChannel(&guest, "#passChannel", "key");
	if (passChannel->hasClient(&guest))
		std::cout << "Guest could join\n" << std::endl;
	else
		std::cout << "Guest could not join - ERROR\n" << std::endl;



	std::cout << "Invite only channel" << std::endl;
	Server serverInvite;
	Client oper;
	oper.setNickname("Oper");
	serverInvite.joinChannel(&oper, "#inviteChannel", "");
	Channel* inviteChannel = serverInvite.findChannelByName("#inviteChannel");
	inviteChannel->setInviteOnly(true);
	Client invitedUser;
	invitedUser.setNickname("InvitedUser");
	std::cout << "Trying to join without invitation" << std::endl;
	serverInvite.joinChannel(&invitedUser, "#inviteChannel", "");
	if (!inviteChannel->hasClient(&invitedUser))
		std::cout << "InvitedUser was blocked without invitation" << std::endl;
	else
		std::cout << "InvitedUser was joined without invitation - ERROR" << std::endl;
	std::cout << "Trying to join with invitation" << std::endl;
	inviteChannel->inviteCommand(&oper, &invitedUser);
	serverInvite.joinChannel(&invitedUser, "#inviteChannel", "");
	if (inviteChannel->hasClient(&invitedUser))
		std::cout << "InvitedUser was joined with invitation\n" << std::endl;
	else
		std::cout << "InvitedUser not added after invite - ERROR\n" << std::endl;
	



	// std::cout << "User limit channel" << std::endl;
	// Server serverLimit;
	// Client op, u1, u2, u3;
	// op.setNickname("Op");
	// u1.setNickname("U1");
	// u2.setNickname("U2");
	// u3.setNickname("U3");
	// serverLimit.joinChannel(&op, "#limitChannel", "");
	// Channel* limitChannel = serverLimit.findChannelByName("#limitChannel");
	// limitChannel->setUserLimit(3);
	// serverLimit.joinChannel(&u1, "#limitChannel", "");
	// serverLimit.joinChannel(&u2, "#limitChannel", "");
	// serverLimit.joinChannel(&u3, "#limitChannel", "");
	// if (limitChannel->hasClient(&op))
	// 	std::cout << "op was added to limitChannel" << std::endl;
	// else
	// 	std::cout << "op was not added to limitChannel - ERROR" << std::endl;
	// if (limitChannel->hasClient(&u1))
	// 	std::cout << "U1 was added to limitChannel" << std::endl;
	// else
	// 	std::cout << "U1 was not added to limitChannel - ERROR" << std::endl;
	// if (limitChannel->hasClient(&u2))
	// 	std::cout << "U2 was added to limitChannel" << std::endl;
	// else
	// 	std::cout << "U2 was not added to limitChannel - ERROR" << std::endl;
	// if (limitChannel->hasClient(&u3))
	// 	std::cout << "U3 was added to limitChannel - ERROR\n" << std::endl;
	// else
	// 	std::cout << "U3 was not added to limitChannel\n" << std::endl;




	// std::cout << "Operator rights test" << std::endl;
	// Server serverRight;
	// Client oper1, user11;
	// oper.setNickname("Oper");
	// user11.setNickname("User");
	// serverRight.joinChannel(&oper1, "#rightsChannel", "");
	// Channel* rightsChannel = serverRight.findChannelByName("#rightsChannel");
	// if (rightsChannel)
	// 	std::cout << "Oper create a channel" << std::endl;
	// else
	// 	std::cout << "Oper could not create a channel" << std::endl;
	// serverRight.joinChannel(&user11, "#rightsChannel", "");
	// if (rightsChannel->hasClient(&user11))
	// 	std::cout << "User joined to channel" << std::endl;
	// else
	// 	std::cout << "User could not join to channel" << std::endl;
	// if (rightsChannel->isOperator(&oper1))
	// 	std::cout << "Oper is operator" << std::endl;
	// else
	// 	std::cout << "Oper is not a operator" << std::endl;
	// if (rightsChannel->isOperator(&user11))
	// 	std::cout << "User is operator" << std::endl;
	// else
	// 	std::cout << "User is not operator" << std::endl;
	// std::cout << "Adding rights to User" << std::endl;
	// bool result = rightsChannel->modeCommand(&oper1, 'o', true, user11.getNickname());
	// if (result)
	// 	std::cout << "Oper could give rights to User" << std::endl;
	// else
	// 	std::cout << "Oper could nor give rights to User" << std::endl;
	// if (rightsChannel->isOperator(&user11))
	// 	std::cout << "User is operator\n" << std::endl;
	// else
	// 	std::cout << "User still is not aperator\n" << std::endl;
	



	std::cout << "kickChannel test" << std::endl;
	Server serverKick;
	Client oper2, user2;
	oper2.setNickname("Oper2");
	user2.setNickname("User2");
	serverKick.joinChannel(&oper2, "#kickChannel", "");
	serverKick.joinChannel(&user2, "#kickChannel", "");
	Channel* kickChannel = serverKick.findChannelByName("#kickChannel");
	if (kickChannel->hasClient(&oper2))
		std::cout << "Oper2 joined to a channel" << std::endl;
	else
		std::cout << "Oper2 could not join to a channel" << std::endl;
	if (kickChannel->hasClient(&user2))
		std::cout << "User2 joined to a channel" << std::endl;
	else
		std::cout << "User2 could not join to a channel" << std::endl;
	kickChannel->kick(&oper2, &user2, "kicked");
	if (kickChannel->hasClient(&user2))
		std::cout << "User2 is still in a channel\n" << std::endl;
	else
		std::cout << "User2 was kicked from channel\n" << std::endl;
	return 0;
}