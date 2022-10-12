#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include "../inc/IRC_Server.hpp"
#include "../inc/IRC_Logic.hpp"

IRC_Server *serv;

void sighandler(int signum) {
	serv->~IRC_Server();
	exit(0);
    (void) signum;
}

// MAIN_CPP
int	main(int ac, char **av) {
	if (ac != 3) {
		std::cerr << "Usage: ./ircerv <port> <password>"
			<< "\n\tport: The server listening port"
			<< "\n\tpassword: The password to access the server" << std::endl;
		return 1;
	}
	signal(SIGINT, sighandler);
	try {
		IRC_Server server(atoi(av[1]), av[2]);
		serv = &server;
		server.host();
	} catch (const std::exception& e) {
		std::cerr << "ircserv: " << e.what() << std::endl;
		return (1);
	}
}
