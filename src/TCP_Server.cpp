#include "../inc/TCP_Server.hpp"

TCP_Server::TCP_Server( void ) {
	socklen_t addrLen = sizeof(_servAddr);
	int opt = 0;

	_fds.push_back((pollfd){0, POLLIN, 0});
	if ((_fds.front().fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw socketFailedException();
	if (setsockopt(_fds.front().fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&opt, sizeof(opt)))
		throw socketFailedException();
	_servAddr.sin_family = AF_INET;
	_servAddr.sin_addr.s_addr = INADDR_ANY;
	_servAddr.sin_port = htons(6667);
	if (bind(_fds.front().fd, reinterpret_cast<struct sockaddr*>(&_servAddr),
				addrLen))
		throw couldNotBindException();
}

TCP_Server::TCP_Server( int port ) {
	socklen_t addrLen = sizeof(_servAddr);
	int opt = 0;

	_fds.push_back((pollfd){0, POLLIN, 0});
	if ((_fds.front().fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw socketFailedException();
	if (setsockopt(_fds.front().fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&opt, sizeof(opt)))
		throw socketFailedException();
	_servAddr.sin_family = AF_INET;
	_servAddr.sin_addr.s_addr = INADDR_ANY;
	_servAddr.sin_port = htons(port);
	if (bind(_fds.front().fd, reinterpret_cast<struct sockaddr*>(&_servAddr),
				addrLen))
		throw couldNotBindException();
}

TCP_Server::TCP_Server( const TCP_Server& other ) {
	*this = other;
}

TCP_Server& TCP_Server::operator = ( const TCP_Server& other) {
	if (this != &other)
	{
		_fds = other._fds;
		_port = other._port;
		_servAddr = other._servAddr;
	}
	return *this;
}

TCP_Server::~TCP_Server( void ) {
	shutdown(_fds.front().fd, SHUT_RDWR);
}

void	TCP_Server::host( void ) {
	sockaddr_in	cliaddr;
	socklen_t addrLen = sizeof(cliaddr);
	std::string response;
	char buffer[1024];
	int	bytesRead;
	int newClient;

	listen(_fds.front().fd, 3);
	while (1) {
		if (!poll(_fds.data(), _fds.size(), -1))
			break;
		if (_fds.front().revents & POLLIN) {
			std::cerr << "New client socket" << std::endl;
			newClient = accept(_fds.front().fd, reinterpret_cast<sockaddr*>(&cliaddr), &addrLen),
			_fds.push_back((pollfd){ newClient, POLLIN | POLLERR, 0});
		}
		for (std::vector<pollfd>::iterator it = ++_fds.begin(); it != _fds.end(); ++it) {
			memset(buffer, 0, 1024);
			if (it->fd != -1 && it->revents & POLLIN) {
				bytesRead = recv(it->fd, buffer, 1024, 0);
				if (bytesRead <= 0) {
					std::cerr << "Connection closed with " << it->fd << std::endl;
					close(it->fd);
					it->fd = -1;
				}
				else {
					std::cerr << "read "<< bytesRead << " bytes from fd " << it->fd << std::endl;
					std::cerr << "content:\n\t" << buffer << std::endl;
					response = processMessage(it->fd, buffer);
					std::cerr << "sending:\n\t" << response << std::endl;
					send(it->fd, response.c_str(), response.size(), 0);
				}
			}	
		}
	}
}

const char* TCP_Server::socketFailedException::what() const throw() {
	return ("Socket failed to initialize");
}

const char* TCP_Server::couldNotBindException::what() const throw() {
	return ("Could not bind port to socket");
}

const char* TCP_Server::brokenConnectionException::what() const throw() {
	return ("Connection with client broken");
}
