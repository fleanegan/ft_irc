#include "../inc/TCP_Server.hpp"

TCP_Server::TCP_Server( void ) {
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

TCP_Server::TCP_Server( int port, int opt) {
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

TCP_Server::TCP_Server( const TCP_Server& other ) {
	*this = other;
}

TCP_Server& TCP_Server::operator = ( const TCP_Server& other) {
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

TCP_Server::~TCP_Server( void ) {
	shutdown(_fd, SHUT_RDWR);
}

void	TCP_Server::host( void ) {
	int		new_socket;
	int		valread;
	char	buffer[1024];

	if (listen(_fd, 3) < 0)
		throw brokenConnectionException();
	if ((new_socket = accept(_fd, reinterpret_cast<struct sockaddr*>(&_addr),
				reinterpret_cast<socklen_t*>(&_addrLen))) < 0)
		throw brokenConnectionException();
	while ((valread = recv(new_socket, buffer, 1024, 0)) != 0)
	{
		if (valread < 0)
			throw brokenConnectionException();
		getMessage(buffer);
		std::string rep = sendMessage();
		send(new_socket, rep.c_str(), rep.size(), 0);
	}
    close(new_socket);
}

int	TCP_Server::getFd( void ) const {
	return _fd;
}

int	TCP_Server::getOpt( void ) const {
	return _opt;
}

int	TCP_Server::getPort( void ) const {
	return _port;
}

int	TCP_Server::getAddrLen( void ) const {
	return _addrLen;
}

struct sockaddr_in	TCP_Server::getAddr( void ) const {
	return _addr;
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
