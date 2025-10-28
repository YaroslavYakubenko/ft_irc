#pragma once

#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include <string>
#include <vector>
#include <sstream>
#include "Command.hpp"

class Server;
class Client;
class Command;

Command parse(const std::string &msg, Server *server, Client *client);