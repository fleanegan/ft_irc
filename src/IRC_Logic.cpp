#include <vector>
#include <algorithm>
#include <iostream>
#include "../inc/IRC_Logic.hpp"

IRC_Logic::IRC_Logic(const std::string &password) : _password(password),
	_operName("oper"), _operPassword("operPassword"),
	_fdToDisconnect(0) {
	}

IRC_Logic::~IRC_Logic() {}

std::string IRC_Logic::processRequest(int fd, const std::string &input) {
	std::vector<std::string> splitMessageVector;
	IRC_User::iterator currentUser = getUserByFd(fd);
	std::string result;

	currentUser->receivedCharacters += input;
	while (currentUser->receivedCharacters.find("\r\n") != std::string::npos) {
		splitMessageVector = extractFirstMessage(&(*currentUser));
		processIncomingMessage(&(*currentUser), splitMessageVector);
	}
	result.swap(_returnMessage);
	return result;
}

void IRC_Logic::processIncomingMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	std::string command = stringToLower(splitMessageVector[0]);

	if (command == "pass") {
		processPassMessage(user, splitMessageVector);
	} else if (command == "cap") {
	} else if (command == "ping") {
		processPingMessage(user, splitMessageVector);
	} else if (user->isAuthenticated == false) {
		_returnMessage += generateResponse(
				ERR_CONNECTWITHOUTPWD, "This server is password protected.");
	} else if (command == "mode") {
		processModeMessage(user, splitMessageVector);
	} else if (command == "nick") {
		processNickMessage(user, splitMessageVector);
	} else if (command == "user") {
		processUserMessage(user, splitMessageVector);
	} else if (command == "privmsg") {
		processPrivMsgMessage(user, splitMessageVector);
	} else if (command == "whois") {
		processWhoIsMessage(user, splitMessageVector);
	} else if (command == "whowas") {
		processWhoWasMessage(user, splitMessageVector);
	} else if (command == "join") {
		processJoinMessage(user, splitMessageVector);
	} else if (command == "quit") {
		processQuitMessage(user, splitMessageVector);
	} else if (command == "part") {
		processPartMessage(user, splitMessageVector);
	} else if (command == "oper") {
		processOperMessage(user, splitMessageVector);
	} else if (command == "kill") {
		processKillMessage(user, splitMessageVector);
	} else if (command == "notice") {
		processNoticeMessage(user, splitMessageVector);
	}
}

void IRC_Logic::processModeMessage(const IRC_User *user,
		const std::vector<std::string> &splitMessageVector) {
	std::string replyContent;
	if (splitMessageVector.size() < 2)
		replyContent = generateResponse(ERR_NEEDMOREPARAMS,
				"Please provide a nickname");
	else
		replyContent = concatenateContentFromIndex(0, splitMessageVector);
	appendMessage(IRC_Message(user->fd, replyContent, ""));
}

void IRC_Logic::processPrivMsgMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	sendMsg(user, splitMessageVector, true);
}

void IRC_Logic::processNoticeMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	sendMsg(user, splitMessageVector, false);
}

void IRC_Logic::sendMsg(
		const IRC_User *user,
		const std::vector<std::string> &splitMessageVector,
		bool replyToErrors) {
	std::queue<int> recipients;
	IRC_Message reply(user->fd, "", "");

	if (splitMessageVector.size() < 3) {
		if (replyToErrors == false)
			return;
		reply.content = generateResponse(
				ERR_NOTEXTTOSEND, "Missing message text");
		appendMessage(reply);
	} else {
		appendRecipients(*user, splitMessageVector, &recipients, replyToErrors);
	}
	if (!recipients.empty())
		appendMessage(IRC_Message(recipients, splitMessageVector, user));
}


//wrap pattern function to take const std::string &
void IRC_Logic::appendRecipients(
		const IRC_User &user,
		const std::vector<std::string> &splitMessageVector,
		std::queue<int> *recipients,
		bool replyToErrors) {
	std::string target = splitMessageVector[1];
	bool entityFound;

	entityFound = appendMatchingSingleRecipients(user, recipients, target)\
				  || appendMatchingChannelRecipients(user, recipients, target);
	if (!replyToErrors || entityFound)
		return;
	else if (IRC_Channel::isChannelPrefix(target))
		appendMessage(IRC_Message(user.fd,
					generateResponse(ERR_NOSUCHCHANNEL,
						"No such channel on this server"), ""));
	else
		appendMessage(IRC_Message(user.fd,
					generateResponse(ERR_NOSUCHNICK,
						"This nick is not found!"), ""));
}

bool IRC_Logic::appendMatchingChannelRecipients(const IRC_User &user,
		std::queue<int> *recipients, const std::string &target) {
	bool entityFound = false;

	for (IRC_Channel::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (isMatchingWildcardExpression(it->name,
					IRC_Channel::toChannelName(target))) {
			entityFound = true;
			if (it->isUserInChannel(user))
				it->appendRecipientFdsForSender(user, recipients);
			else
				appendMessage(IRC_Message(user.fd,
							generateResponse(ERR_CANNOTSENDTOCHAN,
								"You're not part of this channel"), ""));
		}
	}
	return entityFound;
}

bool IRC_Logic::appendMatchingSingleRecipients(const IRC_User &user, std::queue<int> *recipients, const std::string &target) {
	bool entityFound = false;
    bool isMask = target.find('@') != std::string::npos;

    if (isMask && !isValidRecipientMask(user, target))
        return true;
    for (IRC_User::iterator it = _users.begin(); it != _users.end(); ++it) {
		if ((user.nick != it->nick &&
					isMatchingWildcardExpression(it->nick, target))) {
			entityFound = true;
			recipients->push(it->fd);
		}
		else if ((isMask &&
                  isMatchingWildcardExpression(it->userName, target.substr(0, target.find('@')))) &&
			isMatchingWildcardExpression(it->hostIp, target.substr(target.find('@') + 1))) {
				entityFound = true;
				recipients->push(it->fd);
			}
	}
	return entityFound;
}

bool IRC_Logic::isValidRecipientMask(const IRC_User &user, const std::string &target) {
    unsigned long lastMultiWildcardPosition = target.rfind('*');
    unsigned long lastPointPosition = target.rfind('.');
    unsigned long lastSingleWildcardPosition = target.rfind('?');
    bool multiWildAfterLastPoint = lastMultiWildcardPosition != std::string::npos && lastMultiWildcardPosition > lastPointPosition;
    bool singleWildAfterLastPoint = lastSingleWildcardPosition != std::string::npos && lastSingleWildcardPosition > lastPointPosition;
    bool maskHasNoPoint = target.substr(target.find('@') + 1).find('.') == std::string::npos;

    if (maskHasNoPoint || multiWildAfterLastPoint || singleWildAfterLastPoint) {
		appendMessage(IRC_Message(user.fd,
                                  generateResponse(ERR_WILDTOPLEVEL,
						"Mask Invalid"), ""));
        return false;
	}
    return true;
}

void IRC_Logic::processPassMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", "");

	if (user->isAuthenticated)
		reply.content = generateResponse(ERR_ALREADYREGISTERED,
				"You are already registered");
	else if (splitMessageVector.size() == 2
			&& splitMessageVector[1] == _password)
		user->isAuthenticated = true;
	else if (splitMessageVector.size() == 1)
		reply.content = generateResponse(ERR_NEEDMOREPARAMS,
				"Please enter a password");
	else
		reply.content = generateResponse(ERR_PASSWDMISMATCH,
				"Incorrect password");
	appendMessage(reply);
}

void IRC_Logic::appendMessage(const IRC_Message &reply) {
	if (!reply.content.empty())
		_messageQueue.push(reply);
}

void IRC_Logic::processPingMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", "");

	if (splitMessageVector.size() == 1)
		reply.content = generateResponse(ERR_NEEDMOREPARAMS,
				"Please provide a token to ping");
	else
		reply.content = "PONG " + splitMessageVector[1] + "\r\n";
	appendMessage(reply);
}

void IRC_Logic::processNickMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", "");

	if (splitMessageVector.size() == 1) {
		reply.content = generateResponse(ERR_NONICKNAMEGIVEN,
				"No nickname was given");
	} else if (IRC_User::isNickValid(splitMessageVector[1]) == false
			|| splitMessageVector.size() != 2) {
		reply.content = generateResponse(ERR_ERRONEOUSNICK,
				"Your nickname shouldn't contain forbidden characters");
	} else if (isNickAlreadyPresent(splitMessageVector[1])) {
		reply.content = generateResponse(ERR_NICKNAMEINUSE,
				"nickname " + splitMessageVector[1] + " is already taken.");
	} else {
		if (!user->nick.empty() ||
				(user->userName.empty() && user->fullName.empty()))
			reply = *updateNick(user, splitMessageVector, &reply);
		else
			reply = *initNick(user, splitMessageVector, &reply);
	}
	appendMessage(reply);
}

IRC_Message *
IRC_Logic::initNick(IRC_User *user, const std::vector<std::string> &splitMessageVector, IRC_Message *reply) {
	user->nick = splitMessageVector[1].substr(0, 9);
	reply->content = welcomeNewUser(user);
	return reply;
}

IRC_Message *
IRC_Logic::updateNick(IRC_User *user, const std::vector<std::string> &splitMessageVector, IRC_Message *reply) {
	updateNickInAllChannels(user, splitMessageVector);
	_prevUsers.push_back(*user);
	user->nick = splitMessageVector[1].substr(0, 9);
	return reply;
}

void IRC_Logic::updateNickInAllChannels(
		const IRC_User *user,
		const std::vector<std::string> &splitMessageVector) {
	for (IRC_Channel::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (it->isUserInChannel(*user)) {
			it->broadCastToOtherMembers(concatenateContentFromIndex(0, splitMessageVector), *user, &_messageQueue);
			it->updateNick(user->nick, splitMessageVector[1]);
		}
	}
}

void IRC_Logic::processUserMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", "");

	if (user->userName != "" || user->fullName != "") {
		reply.content = generateResponse(ERR_ALREADYREGISTERED,
				"You already sent USER!");
	} else if (splitMessageVector.size() < 5) {
		reply.content = generateResponse(ERR_NEEDMOREPARAMS,
				"You need to send username, username, unused and fullname.");
	} else {
		user->userName = splitMessageVector[1];
		user->fullName = IRC_User::buildFullName(splitMessageVector);
		reply.content = welcomeNewUser(user);
	}
	appendMessage(reply);
}

std::string IRC_Logic::welcomeNewUser(IRC_User *user) {
	if (user->isValid()) {
		return generateResponse(RPL_WELCOME,
				user->nick + " :Welcome to the land of fromage "
				+ user->nick + "!~" + user->userName + "@127.0.0.1")
			+ generateResponse(RPL_YOURHOST,
					user->nick + " :This is our ft_irc server.")
			+ generateResponse(RPL_CREATED,
					user->nick + " :It was created in 2022.")
			+ generateResponse(RPL_MYINFO,
					user->nick + " :It is dedicated to cheese and "
					"somehow tomatoes")
			+ generateResponse(RPL_ISUPPORT,
					user->nick + " :It is able to create channels "
					"with operators, privmsg "
					"(also weirdly a bit of whois and whowas)");
	}
	return "";
}

bool IRC_Logic::isNickAlreadyPresent(const std::string &nick) {
	for (std::vector<IRC_User>::iterator
			user = _users.begin(); user != _users.end(); ++user)
		if (stringToLower(user->nick) == stringToLower(nick))
			return true;
	return false;
}

std::vector<std::string> IRC_Logic::extractFirstMessage(IRC_User *user) {
	size_t pos;
	std::string message;
	std::vector<std::string> result;

	message = user->receivedCharacters.substr(
			0, user->receivedCharacters.find("\r\n"));
	while ((pos = message.find(" ")) != std::string::npos) {
		result.push_back(message.substr(0, pos));
		message.erase(0, pos + 1);
	}
	result.push_back(message);
	user->receivedCharacters.erase(
			0, user->receivedCharacters.find("\r\n") + 2);
	return result;
}

std::vector<IRC_User> &IRC_Logic::getRegisteredUsers() {
	return _users;
}

std::vector<IRC_Channel> &IRC_Logic::getChannels() {
	return _channels;
}

std::queue<IRC_Message> &IRC_Logic::getMessageQueue() {
	return _messageQueue;
}

IRC_User::iterator IRC_Logic::getUserByFd(const int &fd) {
	return IRC_User::findUserByFdInVector(&_users, fd);
}

IRC_User::iterator IRC_Logic::getUserByNick(const std::string &nick) {
	return IRC_User::findUserByNickInVector(&_users, nick);
}

IRC_Channel::iterator IRC_Logic::getChannelByName(const std::string &name) {
	return IRC_Channel::findChannelByNameInVector(&_channels, name);
}

void IRC_Logic::disconnectUser(int userFd, const std::string &reason) {
	IRC_User::iterator user = getUserByFd(userFd);

	if (user == _users.end())
		return;
	broadCastToOtherUsers(reason, *user);
	appendMessage(IRC_Message(userFd, generateResponse(ERR_CLOSINGLINK,
					"Closing link"), ""));
	for (std::vector<IRC_Channel>::iterator
			channel = _channels.begin();
			channel != _channels.end(); channel++) {
		channel->removeMember(*user);
		if (channel->members.empty())
			channel = _channels.erase(channel) - 1;
	}
	_prevUsers.push_back(*user);
	_users.erase(user);
}

void IRC_Logic::broadCastToAllUsers(
		const std::string &message, const IRC_User &user) {
	std::queue<int> recipients;

	for (IRC_User::iterator it = _users.begin(); it != _users.end(); ++it)
		recipients.push(it->fd);
	appendMessage(IRC_Message(recipients, message, &user));
}

void IRC_Logic::broadCastToOtherUsers(
		const std::string &message, const IRC_User &user) {
	std::queue<int> recipients;

	for (IRC_User::iterator it = _users.begin(); it != _users.end(); ++it)
		if (it->fd != user.fd)
			recipients.push(it->fd);
	appendMessage(IRC_Message(recipients, message, &user));
}

void IRC_Logic::processWhoIsMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_User::iterator result;
	IRC_Message reply(user->fd, "", "");

	if (splitMessageVector.size() == 1) {
		reply.content = generateResponse(
				ERR_NONICKNAMEGIVEN, "Please provide a nickname");
	} else {
		result = getUserByNick(splitMessageVector[1]);
		if (result == _users.end())
			reply.content = generateResponse(ERR_NOSUCHNICK,
					"This nickName is not yet registered");
		else
			reply.content = generateResponse(RPL_WHOISUSER, result->toString());
	}
	appendMessage(reply);
}

void IRC_Logic::processWhoWasMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	std::string result;
	IRC_Message reply(user->fd, "", "");

	if (splitMessageVector.size() == 1) {
		reply.content = generateResponse(
				ERR_NONICKNAMEGIVEN, "Please provide a nickname");
	} else {
		result = generateWhoWasAnswer(splitMessageVector);
		if (result.empty())
			reply.content = generateResponse(
					ERR_WASNOSUCHNICK, "This user was never registered");
		else
			reply.content = result + RPL_ENDOFWHOWAS + "\r\n";
	}
	appendMessage(reply);
}

std::string IRC_Logic::generateWhoWasAnswer(
		const std::vector<std::string> &splitMessageVector) const {
	std::string result;

	for (std::vector<IRC_User>::const_reverse_iterator
			rit = _prevUsers.rbegin(); rit != _prevUsers.rend(); ++rit) {
		if (rit->nick == splitMessageVector[1])
			result += generateResponse(RPL_WHOWASUSER, rit->toString());
	}
	return result;
}

void IRC_Logic::processJoinMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", "");
	std::vector<IRC_Channel>::iterator channel;
	IRC_Channel *channelCandidate;

	if (splitMessageVector.size() == 1) {
		reply.content = generateResponse(
				ERR_NEEDMOREPARAMS, "Join requires <channel_name> "
				"and <password>");
		appendMessage(reply);
		return;
	}
	channelCandidate = new IRC_Channel(splitMessageVector[1]);
	if (getChannelByName(channelCandidate->name) == _channels.end())
		_channels.push_back(*channelCandidate);
	channel = getChannelByName(channelCandidate->name);
	delete channelCandidate;
	channel->members.push_back(*user);
	channel->appendJoinMessages(&_messageQueue, *user);
}

void IRC_Logic::processQuitMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	if (splitMessageVector.size() < 2)
		disconnectUser(user->fd, "QUIT leaving");
	else
		disconnectUser(user->fd, concatenateContentFromIndex(0, splitMessageVector));
}

void IRC_Logic::processPartMessage(
		IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Channel::iterator channel;

	if (splitMessageVector.size() < 2) {
		appendMessage(IRC_Message(user->fd,
					generateResponse(ERR_NEEDMOREPARAMS,
						"Please provide a channel name"),
					user->toPrefixString()));
		return;
	}
	channel = getChannelByName(splitMessageVector[1]);
	if (channel == _channels.end()) {
		appendMessage(IRC_Message(user->fd,
					generateResponse(ERR_NOSUCHCHANNEL,
						"This channel does not exist"),
					user->toPrefixString()));
	} else if (!channel->isUserInChannel(*user)) {
		appendMessage(IRC_Message(user->fd,
					generateResponse(ERR_NOTONCHANNEL,
						"You are not on this channel"),
					user->toPrefixString()));
	} else {
		channel->broadCastToAllMembers(
				concatenateContentFromIndex(0, splitMessageVector),
				*user, &_messageQueue);
		channel->removeMember(*user);
		if (channel->members.empty())
			_channels.erase(channel);
	}
}

void IRC_Logic::processOperMessage(IRC_User *user,
		const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", user->toPrefixString());
	if (splitMessageVector.size() != 3) {
		reply.content = generateResponse(ERR_NEEDMOREPARAMS,
				"Please provide a channel name and and a password");
	} else if (splitMessageVector[1] != _operName || splitMessageVector[2] != _operPassword) {
		reply.content = generateResponse(ERR_PASSWDMISMATCH,
				"Password incorrect");
	} else {
		reply.content = generateResponse(RPL_YOUREOPER, "You are now oper");
		user->isOper = true;
	}
	appendMessage(reply);
}

void IRC_Logic::addUser(int fd, const std::string &hostIp) {
	_users.push_back(IRC_User(fd, hostIp));
}

void IRC_Logic::processKillMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	IRC_Message reply(user->fd, "", "");

	if (splitMessageVector.size() < 2) {
		reply.content = generateResponse(ERR_NEEDMOREPARAMS,
				"Please provide a nickname to kill");
	} else if (getUserByNick(splitMessageVector[1]) == _users.end()) {
		reply.content = generateResponse(ERR_NOSUCHNICK,
				"This user does not exist");
	} else if (user->isOper == false) {
		reply.content = generateResponse(ERR_NOPRIVILEGES,
				"You need to be oper in order to kill");
	} else {
		_fdToDisconnect = getUserByNick(splitMessageVector[1])->fd;
		disconnectUser(_fdToDisconnect,
				"QUIT " + splitMessageVector[1] + " killed by oper\r\n");
	}
	appendMessage(reply);
}

int IRC_Logic::popFdToDisconnect() {
	int result = _fdToDisconnect;

	_fdToDisconnect = 0;
	return result;
}

