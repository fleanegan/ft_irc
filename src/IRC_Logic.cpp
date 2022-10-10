#include <vector>
#include "../inc/IRC_Logic.hpp"

std::string concatenateContentFromIndex(int startIndex, const std::vector<std::string> &splitMessageVector);

IRC_Logic::IRC_Logic(const std::string &password) : _password(password) {
}

IRC_Logic::IRC_Logic(const IRC_Logic &other) {
	(void) other;
}

IRC_Logic &IRC_Logic::operator=(const IRC_Logic &) {
	return *this;
}

IRC_Logic::~IRC_Logic() {

}

void IRC_Logic::removeMessageTermination(std::string *message) {
	message->erase(message->size() - 3, 3);
}

void IRC_Logic::cleanupName(std::string *name) {
	name->erase(0, 1);
	name->erase(name->size() - 1, 1);
}

std::string IRC_Logic::processInput(int fd, const std::string &input) {
	std::vector<std::string> splitMessageVector;
	IRC_User::UserIterator currentUser = getUserByFd(fd);
	std::string result;

	if (currentUser == _users.end()) {
		_users.push_back(IRC_User(fd));
		currentUser = _users.end() - 1;
	}
	currentUser->receivedCharacters += input;
	while (currentUser->receivedCharacters.find("\r\n") != std::string::npos) {
		splitMessageVector = extractFirstMessage(&(*currentUser));
		result += processIncomingMessage(&(*currentUser), splitMessageVector);
	}
	return result;
}

std::string IRC_Logic::processIncomingMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	const std::vector<std::string> &splitMessageVector1 = splitMessageVector;

	if (splitMessageVector1[0] == "PASS")
		return processPassMessage(user, splitMessageVector);
	else if (splitMessageVector[0] == "CAP")
		return "";
	else if (splitMessageVector[0] == "PING")
		return processPingMessage(splitMessageVector);
	else if (user->isAuthenticated == false)
		return generateResponse(ERR_CONNECTWITHOUTPWD,
								"This server is password protected. Have you forgotten to send PASS?");
	else if (splitMessageVector[0] == "NICK")
		return processNickMessage(user, splitMessageVector);
	else if (splitMessageVector[0] == "USER")
		return processUserMessage(user, splitMessageVector);
	else if (splitMessageVector[0] == "PRIVMSG")
		return processPrivMsgMessage(user, splitMessageVector);
	else if (splitMessageVector[0] == "WHOIS")
		return processWhoIsMessage(user, splitMessageVector);
	else if (splitMessageVector[0] == "WHOWAS")
		return processWhoWasMessage(user, splitMessageVector);
	else
		return "";
}

std::string IRC_Logic::processPrivMsgMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	std::queue<int> recipients;
	IRC_User::UserIterator recipient;

	if (splitMessageVector.size() < 3)
		return generateResponse(ERR_NOTEXTTOSEND, "Dont bother your friends if you have nothing to tell them! Add a message");
	recipient = getUserByNick(splitMessageVector[1]);
	if (recipient == _users.end())
		return generateResponse(ERR_NOSUCHNICK, "Get an addressbook! This nick does not exist!");
	recipients.push(recipient->fd);
	_messageQueue.push(IRC_Message(recipients, splitMessageVector, user));
	return "";
}

std::string IRC_Logic::processPassMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	if (user->isAuthenticated)
		return generateResponse(ERR_ALREADYREGISTERED, "Do you really like to type the pw that much?");
	if (splitMessageVector.size() == 2 && splitMessageVector[1] == _password) {
		user->isAuthenticated = true;
		return "";
	} else if (splitMessageVector.size() == 1){
		return generateResponse(ERR_NEEDMOREPARAMS, "You did not enter a password");
	}
	return generateResponse(ERR_PASSWDMISMATCH, "You did not enter the correct password");
}

std::string IRC_Logic::processPingMessage(const std::vector<std::string> &splitMessageVector) {
	if (splitMessageVector.size() == 1)
		return generateResponse(ERR_NEEDMOREPARAMS, "Ping must have a token");
	return ("PONG " + splitMessageVector[1] + "\r\n");
}

std::string IRC_Logic::processNickMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	std::string result;

	if (splitMessageVector.size() == 1)
		return (generateResponse(ERR_NONICKNAMEGIVEN, "You should consider having a nickname"));
	else if (IRC_User::isNickValid(splitMessageVector[1]) == false || splitMessageVector.size() != 2)
		return generateResponse(ERR_ERRONEOUSNICK, "Sigh, think again. NO FORBIDDEN CHARACTERS!");
	else if (isNickAlreadyPresent(splitMessageVector[1]))
		return generateResponse(ERR_NICKNAMEINUSE, "Sorry, someone was just as creative as you are.");
	user->nick = splitMessageVector[1].substr(0, 9);
	return welcomeNewUser(user);
}

std::string IRC_Logic::processUserMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	if (user->userName != "" || user->fullName != "")
		return generateResponse(ERR_ALREADYREGISTERED, "Call USER once. Not twice.");
	if (splitMessageVector.size() < 5)
		return generateResponse(ERR_NEEDMOREPARAMS, "You need to send username, username, unused and fullname.");
	user->userName = splitMessageVector[1];
	user->fullName = IRC_User::buildFullName(splitMessageVector);
	return welcomeNewUser(user);
}

std::string IRC_Logic::welcomeNewUser(IRC_User *user) {
	if (isUserRegistered(user)) {
		return generateResponse(RPL_WELCOME, user->nick + " :Welcome to the land of fromage " + user->nick + "!~" + user->userName + "@127.0.0.1")
			   + generateResponse(RPL_YOURHOST, "This is a ft_irc server")
			   + generateResponse(RPL_CREATED, "The server was probably created in 2022")
			   + generateResponse(RPL_MYINFO, "This server has got channels, believe me")
			   + generateResponse(RPL_ISUPPORT, "Moderate demands");
	}
	return "";
}

bool IRC_Logic::isNickAlreadyPresent(const std::string &nick) {
	for (std::vector<IRC_User>::iterator user = _users.begin(); user != _users.end(); ++user)
		if (stringToLower(user->nick) == stringToLower(nick))
			return true;
	return false;
}

std::vector<std::string> IRC_Logic::extractFirstMessage(IRC_User *user) {
	size_t pos;
	std::string message;
	std::vector<std::string> result;

	message = user->receivedCharacters.substr(0, user->receivedCharacters.find("\r\n"));
	while ((pos = message.find(" ")) != std::string::npos) {
		result.push_back(message.substr(0, pos));
		message.erase(0, pos + 1);
	}
	result.push_back(message);
	user->receivedCharacters.erase(0, user->receivedCharacters.find("\r\n") + 2);
	return result;
}

std::vector<IRC_User> IRC_Logic::getRegisteredUsers() {
	return _users;
}

std::queue<IRC_Message> &IRC_Logic::getMessageQueue() {
	return _messageQueue;
}

IRC_User::UserIterator IRC_Logic::getUserByFd(const int &fd) {
	return IRC_User::findUserByFdInVector(&_users, fd);
}

IRC_User::UserIterator IRC_Logic::getUserByNick(const std::string &nick) {
	return IRC_User::findUserByNickInVector(&_users, nick);
}

bool IRC_Logic::isUserRegistered(IRC_User *user) {
	if (!user || user->nick == "" || user->userName == "" || user->fullName == "")
		return false;
	return true;
}

std::string IRC_Logic::processWhoIsMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	const IRC_User::UserIterator &result = getUserByNick(splitMessageVector[1]);
	(void)user;
	return std::string(RPL_WHOISUSER) + " " + result->nick + " " + result->userName + " " + " 127.0.0.1 * :" + result->fullName + "\r\n";
}

void IRC_Logic::disconnectUser( int fd ) {
	IRC_User::UserIterator userToBeDeleted = getUserByFd(fd);

	_prevUsers.push_back(*userToBeDeleted);
	_users.erase(userToBeDeleted);
}

std::string IRC_Logic::processWhoWasMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector) {
	const IRC_User::UserIterator &result = IRC_User::findUserByNickInVector(&_prevUsers, splitMessageVector[1]);
	(void)user;
	return std::string(RPL_WHOWASUSER) + " " + result->nick + " " + result->userName + " 127.0.0.1 * :" + result->fullName + "\r\n";
}
