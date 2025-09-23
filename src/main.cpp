#include "../include/Server.hpp"
#include "../include/Client.hpp"

void signalHandler(int signum) {
	if (signum == SIGINT) {
		std::cout << "\nShutting down the server..." << std::endl;
		running = false;
	}
}

int main(int ac, char **av) {
	if (ac != 3) {
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	int port = std::atoi(av[1]);
	std::string password = av[2];
	std::signal(SIGINT, signalHandler);
	Server server(port, password);
	server.run();
	return 0;
}