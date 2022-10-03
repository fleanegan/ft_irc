#include <stdlib.h>

#include "../inc/IRC_Server.hpp"
#include <iostream>

// MAIN_CPP
int	main(int ac, char **av)
{
	if (ac != 3) {
		std::cout << "Usage: ./ircerv <port> <password>"
			<< "\n\tport: The server listening port"
			<< "\n\tpassword: The password to access the server" << std::endl;
		return 1;
	}
	try {
		IRC_Server server(atoi(av[1]), 1);
		server.host();
	} catch (const std::exception& e) {
		std::cout << "ircserv: " << e.what() << std::endl;
		return (1);
	}
}
