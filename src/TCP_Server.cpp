#include <netdb.h>
#include <arpa/inet.h>
#include "../inc/TCP_Server.hpp"

TCP_Server::TCP_Server(void): _fdsToCloseAfterUpdate() {
	setUpTcpSocket(6667);
}

TCP_Server::TCP_Server(int port): _fdsToCloseAfterUpdate() {
	setUpTcpSocket(port);
}

void TCP_Server::setUpTcpSocket(int port) {
	socklen_t addrLen = sizeof(_servAddr);
	int opt = 0;

    saveConnectionInfo(socket(AF_INET, SOCK_STREAM, 0), "");
	if (_fds.front().fd == -1)
		throw socketFailedException();
	if (setsockopt(_fds.front().fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				   &opt, sizeof(opt)))
		throw socketFailedException();
	_servAddr.sin_family = AF_INET;
	_servAddr.sin_addr.s_addr = INADDR_ANY;
	_servAddr.sin_port = htons(port);
	if (bind(_fds.front().fd, reinterpret_cast<struct sockaddr *>(&_servAddr),
			 addrLen)) {
		close(_fds.front().fd);
		throw couldNotBindException();
	}
}

void TCP_Server::saveConnectionInfo(int fd, const std::string &hostIp) {
	_fds.push_back((pollfd) {fd, POLLIN | POLLERR, 0});
    if (!hostIp.empty())
        onConnect(fd, hostIp);
}

TCP_Server::~TCP_Server(void) {
	shutdown(_fds.front().fd, SHUT_RDWR);
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();
			++it)
		close(it->fd);
}

void TCP_Server::host(void) {
	sockaddr_in cliaddr;
	socklen_t addrLen = sizeof(cliaddr);
	char buffer[BUFFER_LEN];
	int bytesRead;
	int newClient;

	listen(_fds.front().fd, 3);
	while (poll(_fds.data(), _fds.size(), -1)) {
		if (_fds.front().revents & POLLIN) {
			_VERBOSE && std::cerr << "New client socket" << std::endl;
			newClient = accept(_fds.front().fd,
					reinterpret_cast<sockaddr *>(&cliaddr), &addrLen);
            saveConnectionInfo(newClient, inet_ntoa(cliaddr.sin_addr));
        }
		for(std::vector<pollfd>::iterator
				it = _fds.begin() + 1; it != _fds.end(); ++it) {
			memset(buffer, 0, BUFFER_LEN);
			_VERBOSE && std::cerr << "Connection " << it->fd << std::endl;
			if (it->revents & (POLLIN | POLLERR)) {
				bytesRead = recv(it->fd, buffer, BUFFER_LEN, 0);
				if (bytesRead <= 0) {
					_VERBOSE && std::cerr << "Connection closed with " << it->fd << std::endl;
					close(it->fd);
                    onDisconnect(it->fd);
					it = _fds.erase(it) - 1;
				} else {
                    _VERBOSE && std::cerr << "read " << bytesRead
						<< " bytes from fd " << it->fd << std::endl;
					_VERBOSE && std::cerr << "content:\n\t" << buffer << std::endl;
					processMessage(it->fd, buffer);
				}
			}
		}
        closeFdHook();
	}
}

void TCP_Server::closeFdHook() {
    while (_fdsToCloseAfterUpdate.empty() == false) {
        int currentFd = _fdsToCloseAfterUpdate.front();
        _fdsToCloseAfterUpdate.pop();
        close(currentFd);
        for (std::vector<pollfd>::iterator it = _fds.begin()
                ; it != _fds.end(); ++it) {
            if (it->fd == currentFd)
                it = _fds.erase(it) - 1;
        }
    }
}

const char *TCP_Server::socketFailedException::what() const throw() {
	return ("Socket failed to initialize");
}

const char *TCP_Server::couldNotBindException::what() const throw() {
	return ("Could not bind port to socket");
}

const char *TCP_Server::brokenConnectionException::what() const throw() {
	return ("Connection with client broken");
}
