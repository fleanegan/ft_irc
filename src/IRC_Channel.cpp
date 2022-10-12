#include "../inc/IRC_Channel.hpp"

IRC_Channel::IRC_Channel(const std::string &channelName):
	name(getChannelName(channelName)) {
}

bool IRC_Channel::operator==(const IRC_Channel &rhs) const {
	return name == rhs.name;
}

bool IRC_Channel::operator!=(const IRC_Channel &rhs) const {
	return !(rhs == *this);
}

std::string  IRC_Channel::getChannelName(const std::string &name) {
	if (name[0] == '#')
		return name.substr(1, name.size());
	return name;
}

void IRC_Channel::appendJoinMessages(
		std::queue<IRC_Message> *messageQueue, const IRC_User &newMember) {
	appendJoinNotificationToAllMembers(messageQueue, newMember);
	appendJoinReplyToNewMember(messageQueue, newMember);
}

void IRC_Channel::appendJoinReplyToNewMember(
		std::queue<IRC_Message> *messageQueue, const IRC_User &newMember) {
	IRC_Message reply(newMember.fd, "", "");

	reply.content += std::string(RPL_NAMREPLY) + " " + newMember.nick +
					 " = " + name + " :";
	for (std::vector<IRC_User *>::reverse_iterator
				 it = members.rbegin(); it != members.rend(); ++it)
		reply.content += " " + (*it)->nick;
	reply.content += "\r\n";
	reply.content += std::string(RPL_ENDOFNAMES) + " "
					 + newMember.nick + " #" + name +
					 " :End of NAMES list.\r\n";
	messageQueue->push(reply);
}

void IRC_Channel::appendJoinNotificationToAllMembers(
		std::queue<IRC_Message> *messageQueue, const IRC_User &newMember) {
	std::string notifyText;
	std::queue<int> recipients;

	notifyText = " JOIN :#" + name + "\r\n";
	for (std::vector<IRC_User *>::reverse_iterator
				 it = members.rbegin(); it != members.rend(); ++it)
		recipients.push((*it)->fd);
	IRC_Message notify(recipients, notifyText, &newMember);
	messageQueue->push(notify);
}
