#include "../inc/IRC_Server.hpp"
#include <iostream>

IRC_Server::IRC_Server( void ): TCP_Server(), _logic("password") {
}

IRC_Server::IRC_Server( int port, const std::string& password): TCP_Server(port), _logic(password) {
}

IRC_Server::~IRC_Server() {
}

IRC_Server& IRC_Server::operator = (const IRC_Server& other) {
	(void)other;
	return *this;
}

std::string IRC_Server::processMessage(int fd, const std::string& buffer) {
	return _logic.processInput(fd, buffer, "localhost");
}
