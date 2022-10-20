#include <iostream>
#include "../inc/IRC_Channel.hpp"

IRC_Channel::IRC_Channel(const std::string &channelName) :
	name(toChannelName(channelName)) {
	}

bool IRC_Channel::operator==(const IRC_Channel &rhs) const {
	return name == rhs.name;
}

bool IRC_Channel::operator!=(const IRC_Channel &rhs) const {
	return !(rhs == *this);
}

bool IRC_Channel::isUserInChannel(const IRC_User &user) const {
	for (std::vector<IRC_User>::const_iterator it = members.begin();
			it != members.end(); ++it)
		if (user.nick == it->nick)
			return true;
	return false;
}

std::string IRC_Channel::toChannelName(const std::string &name) {
	std::string result = stringToLower(name);

	if(isChannelPrefix(result))
		return result.substr(1, 49);
	return result.substr(0, 49);
}

bool IRC_Channel::isChannelPrefix(const std::string &result) {
	std::string prefix = "#&+!";

	if (prefix.find(result[0]) != std::string::npos)
		return true;
	return false;
}

void IRC_Channel::appendJoinMessages(
		std::queue<IRC_Message> *messageQueue, const IRC_User &newMember) {
	appendJoinNotificationToAllMembers(messageQueue, newMember);
	appendJoinReplyToNewMember(messageQueue, newMember);
}

void IRC_Channel::appendJoinReplyToNewMember(
		std::queue<IRC_Message> *messageQueue, const IRC_User &newMember) {
	IRC_Message reply(newMember.fd, "", "");

	reply.content = std::string(RPL_NAMREPLY) + " " + newMember.nick +
		" = " + name + " :";
	for (std::vector<IRC_User>::reverse_iterator
			it = members.rbegin(); it != members.rend(); ++it)
		reply.content += " " + it->nick;
	reply.content += std::string("\r\n") + RPL_ENDOFNAMES + " "
		+ newMember.nick + " #" + name +" :End of NAMES list.\r\n";
	messageQueue->push(reply);
}

void IRC_Channel::appendJoinNotificationToAllMembers(
		std::queue<IRC_Message> *messageQueue, const IRC_User &newMember) {
	std::string notifyText;
    std::queue<int> recipients = getMemberFds();

    notifyText = " JOIN :#" + name + "\r\n";
    IRC_Message notify(recipients, notifyText, &newMember);
	messageQueue->push(notify);
}

std::queue<int> IRC_Channel::getMemberFds() {
    std::queue<int> recipients;
    for (std::vector<IRC_User>::reverse_iterator
			it = members.rbegin(); it != members.rend(); ++it)
		recipients.push(it->fd);
    return recipients;
}

void IRC_Channel::appendRecipientFdsForSender(
        const IRC_User &user, std::queue<int> *recipients) const {
	for (std::vector<IRC_User>::const_iterator
			it = members.begin();
			it != members.end(); it++)
		if (user.nick != it->nick) {
			recipients->push((it)->fd);
		}
}

void IRC_Channel::removeMember(const IRC_User &user) {
	if (!isUserInChannel(user))
		return;
	for (std::vector<IRC_User>::iterator
			it = members.begin(); it != members.end(); ++it)
		if (it->nick == user.nick)
			it = members.erase(it) - 1;
}

void IRC_Channel::broadCastToAllMembers(
        const std::string &message, const IRC_User &sender,
        std::queue<IRC_Message> *messages) {
    messages->push(IRC_Message(getMemberFds(), message, &sender));
}

void IRC_Channel::broadCastToOtherMembers(
        const std::string &message, const IRC_User &sender,
        std::queue<IRC_Message> *messages) {
    std::queue<int> recipients;

    appendRecipientFdsForSender(sender, &recipients);
    messages->push(IRC_Message(recipients, message, &sender));
}

void IRC_Channel::updateNick(
		const std::string &oldNick, const std::string &newNick) {
    for (IRC_User::iterator it = members.begin(); it != members.end(); ++it)
        if (oldNick == it->nick) {
            it->nick = newNick;
            return;
        }
}

IRC_Channel::iterator IRC_Channel::findChannelByNameInVector(
		std::vector<IRC_Channel> *channels, const std::string &name) {
    std::string normedChannelName = toChannelName(name);

    for (iterator
                 it = channels->begin(); it != channels->end(); ++it)
        if (normedChannelName == it->name)
            return it;
    return channels->end();
}
