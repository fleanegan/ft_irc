#include "../inc/IRC_Server.hpp"
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

std::string IRC_Server::processMessage(int fd, const std::string& buffer) {
	std::string result = _logic.processInput(fd, buffer);

    distributeMessages();
    return result;
}

void IRC_Server::distributeMessages() {
    IRC_Message* currentMessage;
    int			currentFd;

    while (!_logic.getMessageQueue().empty()) {
        currentMessage = &_logic.getMessageQueue().front();
        while (!currentMessage->recipients.empty()) {
            currentFd = currentMessage->recipients.front();
            currentMessage->recipients.pop();
            _VERBOSE && std::cerr << "sending \n\t" << currentMessage->content
                << " to fd " << currentFd << std::endl;
            std::string sendString = (currentMessage->content + "\r\n");
            send(currentFd, sendString.c_str(), sendString.size(), 0);
        }
        _logic.getMessageQueue().pop();
    }
}

void IRC_Server::onDisconnect(int fd) {
	_logic.disconnectUser(fd, generateResponse(ERR_CLOSINGLINK, "Closing Link"));
    distributeMessages();
}
