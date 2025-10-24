#pragma once

#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include <string>
#include <vector>
#include <sstream>

class Server;
class Client;

void parse(const std::string &msg, Server *server, Client *client);