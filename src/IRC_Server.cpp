#include "../inc/IRC_Server.hpp"
#include "../test/testUtils.hpp"
#include <iostream>

IRC_Server::IRC_Server(void): TCP_Server(), _logic("password") {
}

IRC_Server::IRC_Server(int port, const std::string& password):
	TCP_Server(port), _logic(password) {
}

IRC_Server::~IRC_Server() {
}

IRC_Server& IRC_Server::operator = (const IRC_Server& other) {
	(void)other;
	return *this;
}

void IRC_Server::processMessage(int fd, const std::string& buffer) {
	_logic.processRequest(fd, buffer);

    distributeMessages();
    handleDisconnectionsFromLogic();
}

void IRC_Server::handleDisconnectionsFromLogic() {
    int fdToDisconnect = _logic.popFdToDisconnect();
    if (fdToDisconnect)
        _fdsToCloseAfterUpdate.push(fdToDisconnect);
}
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

void IRC_Server::distributeMessages() {
    IRC_Message* currentMessage;
    int			currentFd;

    while (!_logic.getMessageQueue().empty()) {
        currentMessage = &_logic.getMessageQueue().front();
        while (!currentMessage->recipients.empty()) {
            currentFd = currentMessage->recipients.front();
            currentMessage->recipients.pop();

            std::string sendString = (currentMessage->content + "\r\n");
            if (errno){
                errno = 0;
                emptyQueue(&_logic.getMessageQueue());
                _VERBOSE && std::cerr << "ohoh, weve got some errors" << std::endl;
                return;
            }
            else{
                _VERBOSE && std::cerr << "sending \n\t" << currentMessage->content
                                      << " to fd " << currentFd << std::endl;
                send(currentFd, sendString.c_str(), sendString.size(), 0);
            }
        }
        _logic.getMessageQueue().pop();
    }
}

void IRC_Server::onDisconnect(int fd) {
	_VERBOSE && std::cerr << "connection lost from " << fd << std::endl;
	_logic.disconnectUser(fd, generateResponse(ERR_CLOSINGLINK, "Closing Link"));
    distributeMessages();
}

void IRC_Server::onConnect(int fd, const std::string &hostIp) {
    _logic.addUser(fd, hostIp);
}
