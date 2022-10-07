#include "../inc/IRC_Message.hpp"

IRC_Message::IRC_Message() {

}

IRC_Message::IRC_Message(const std::queue<int>& recipients, const std::string& content):
	recipients(recipients), content(content) {
}

IRC_Message::IRC_Message(const IRC_Message &other): 
	recipients(other.recipients), content(other.content) {
}

IRC_Message &IRC_Message::operator=(const IRC_Message &other) {
	if (this != &other) {
		recipients = other.recipients;
		content = other.content;
	}
	return *this;
}

IRC_Message::~IRC_Message() {
}

std::string IRC_Message::buildMessageContent(const std::vector<std::string> &splitMessageVector) {
	std::string name = concatenateContentFromIndex(2, splitMessageVector);

	removeLeadingColon(&name);
	return name;
}
