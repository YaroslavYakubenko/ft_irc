#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

void signalHandler(int signum) {
	if (signum == SIGINT) {
		std::cout << "\nShutting down the server..." << std::endl;
		running = false;
	}
}

int main(int ac, char **av) {
	int port = std::atoi(av[1]);
	std::string password = av[2];
	if (ac != 3) {
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	if (port != 6667) {
		std::cout << "Port is incorrect" << std::endl;
		exit(1);
	}
	if (password != "123") {
		std::cout << "464 Password is incorrect" << std::endl;
		exit(1);
	}
	std::signal(SIGINT, signalHandler);
	Server server(port, password);
	server.run();
	return 0;
}
