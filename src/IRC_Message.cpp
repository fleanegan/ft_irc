#include "../inc/IRC_Message.hpp"
#include "./IRC_User.hpp"

IRC_Message::IRC_Message(const std::queue<int> &recipients, std::vector<std::string> splitMessageVector, IRC_User *sender) :
		recipients(recipients), sender(sender), content(buildMessageContent(splitMessageVector)) {
}

IRC_Message::IRC_Message(const IRC_Message &other): 
	recipients(other.recipients), sender(other.sender), content(other.content){
}

IRC_Message::IRC_Message(const std::queue<int> &recipients, const std::string &messageContent, IRC_User *sender) : recipients(recipients), sender(sender), content(
        sender->toPrefixString() + messageContent){
}

IRC_Message::IRC_Message(int recipientFd, const std::string &messageContent,
                         const std::string &prefix) : recipients(), sender(NULL), content(prefix + messageContent){
    recipients.push(recipientFd);
}

IRC_Message &IRC_Message::operator=(const IRC_Message &other) {
	if (this != &other) {
		recipients = other.recipients;
		content = other.content;
		sender = other.sender;
	}
	return *this;
}

IRC_Message::~IRC_Message() {
}

std::string IRC_Message::buildMessageContent(const std::vector<std::string> &splitMessageVector) const{
	std::string name;

    name = sender->toPrefixString();
    name += concatenateContentFromIndex(0, splitMessageVector);

	return name;
}
