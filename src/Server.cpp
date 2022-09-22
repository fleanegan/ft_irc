#include "../inc/Server.hpp"

Server::Server( void ) {
	_opt = 1;
	_addrLen = sizeof(_addr);
	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw socketFailedException();
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&_opt, sizeof(_opt)))
		throw socketFailedException();
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(6667);
	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_addr), _addrLen))
		throw couldNotBindException();
}

Server::Server( int port, int opt) {
	_opt = opt;
	_addrLen = sizeof(_addr);
	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw socketFailedException();
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&_opt, sizeof(_opt)))
		throw socketFailedException();
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(port);
	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_addr), _addrLen))
		throw couldNotBindException();
}

Server::Server( const Server& other ) {
	*this = other;
}

Server& Server::operator = ( const Server& other) {
	if (this != &other)
	{
		_fd = other.getFd();
		_opt = other.getOpt();
		_port = other.getPort();
		_addrLen = other.getAddrLen();
		_addr = other.getAddr();
	}
	return *this;
}

Server::~Server( void ) {
	shutdown(_fd, SHUT_RDWR);
}

void	Server::host( void ) {
	int	new_socket;
	int valread;
	char	buffer[4096];

	if (listen(_fd, 3) < 0)
		throw brokenConnectionException();
	if ((new_socket = accept(_fd, reinterpret_cast<struct sockaddr*>(&_addr),
				reinterpret_cast<socklen_t*>(&_addrLen))) < 0)
		throw brokenConnectionException();
	while (1) {
		while ((valread = read(new_socket, buffer, 4096)) > 0)
		{
			std::cout << buffer << std::endl;
			memset(&buffer, 0, 4096);
		}
		send(new_socket, "pong\n", strlen("pong\n"), 0);
	}
    close(new_socket);
}

int	Server::getFd( void ) const {
	return _fd;
}

int	Server::getOpt( void ) const {
	return _opt;
}

int	Server::getPort( void ) const {
	return _port;
}

int	Server::getAddrLen( void ) const {
	return _addrLen;
}

struct sockaddr_in	Server::getAddr( void ) const {
	return _addr;
}

const char* Server::socketFailedException::what() const throw() {
	return ("Socket failed to initialize");
}

const char* Server::couldNotBindException::what() const throw() {
	return ("Could not bind port to socket");
}

const char* Server::brokenConnectionException::what() const throw() {
	return ("Connection with client broken");
}
