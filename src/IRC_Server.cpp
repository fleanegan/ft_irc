#include "../inc/IRC_Server.hpp"
#include <iostream>

IRC_Server::IRC_Server( void ): TCP_Server() {
}

IRC_Server::IRC_Server( int port, int opt): TCP_Server(port, opt) {
}

IRC_Server::IRC_Server( const IRC_Server& other):TCP_Server(*this) {
	(void)other;
}

IRC_Server::~IRC_Server() {
}

IRC_Server& IRC_Server::operator = (const IRC_Server& other) {
	(void)other;
	return *this;
}

std::string	IRC_Server::retrieveMessage( char* buffer ) {
	std::string ret;

	while (strcmp(buffer, "\r\n") && *buffer) {
		ret += *buffer;
		buffer++;
	}
	return (ret);
}

void	IRC_Server::getMessage( char* buffer )
{
	std::string toPush;
	std::string tmp;

	if (!_remain.empty()) {
		toPush += _remain;
		_remain.clear();
	}
	tmp = retrieveMessage(buffer);
	while (tmp.size() > 2 && (&tmp[tmp.size() - 3]) == std::string("\r\n"))
	{
		toPush += tmp;
		_commands.push(toPush);
		toPush.clear();
		buffer += tmp.size();
		tmp = retrieveMessage(buffer);
	}
	if (strlen(buffer) != 0)
		_remain.append(buffer);
}

std::string	IRC_Server::processCommand( std::string command ) {
	return (command);
}

std::string	IRC_Server::sendMessage( void )
{
	std::string	rep;

	while (!_commands.empty()) {
		rep += processCommand(_commands.front());
		std::cout << rep << std::endl;
		_commands.pop();
	}
	return (rep);
}
