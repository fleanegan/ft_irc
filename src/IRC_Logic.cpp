#include <vector>
#include "../inc/IRC_Logic.hpp"

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

bool IRC_Logic::isUserMessage(const std::vector<std::string> &splitMessageVector) const {
	return splitMessageVector.size() >= 5 && splitMessageVector[0] == "USER";
}

void IRC_Logic::receive(int fd, const std::string &string) {
	std::vector<std::string> splitMessageVector;

	_remain += string;
	while (_remain.find("\r\n") != std::string::npos) {
		splitMessageVector = extractFirstMessage(_remain.substr(0, _remain.find("\r\n")));
		processIncommingMessage(fd, splitMessageVector);
	}
}

void IRC_Logic::processIncommingMessage(int fd, const std::vector<std::string> &splitMessageVector) {
	if (splitMessageVector[0] == "PASS") {
		processPassMessage(fd, splitMessageVector);
	} else if (!getUserByFd(fd))
		return;
	else if (isNickMessage(splitMessageVector))
		processNickMessage(fd, splitMessageVector);
	else if (isUserMessage(splitMessageVector))
		processUserMessage(fd, splitMessageVector);
}

void IRC_Logic::processPassMessage(int fd, const std::vector<std::string> &splitMessageVector) {
	if (getUserByFd(fd))
		return;
	if (splitMessageVector.size() == 2 && splitMessageVector[1] == _password) {
		_users.push_back(IRC_User(fd));
	}
}

void IRC_Logic::processUserMessage(const int &fd, const std::vector<std::string> &splitMessageVector) {
	if (!IRC_User::isValidCreationString(splitMessageVector))
		return;
	IRC_User *newUser = getUserByFd(fd);
	newUser->name = splitMessageVector[1];
	newUser->fullName = buildFullName(splitMessageVector);
	if (userIsRegistered(newUser)) {
		_returnCodes.push_back(RPL_WELCOME);
		_returnCodes.push_back(RPL_YOURHOST);
		_returnCodes.push_back(RPL_CREATED);
		_returnCodes.push_back(RPL_MYINFO);
		_returnCodes.push_back(RPL_ISUPPORT);
	}
}

void IRC_Logic::processNickMessage(int fd, const std::vector<std::string> &splitMessageVector) {
	if (splitMessageVector.size() == 2 && !isNickAlreadyPresent(splitMessageVector[1]))
		getUserByFd(fd)->nick = splitMessageVector[1];
}

bool IRC_Logic::isNickMessage(const std::vector<std::string> &splitMessageVector) const {
	return splitMessageVector[0] == "NICK"
		   && IRC_User::isNickValid(splitMessageVector[1]);
}

bool IRC_Logic::isNickAlreadyPresent(const std::string &nick) {
	for (std::vector<IRC_User>::iterator user = _users.begin(); user != _users.end(); ++user)
		if (stringToLower(user->nick) == stringToLower(nick))
			return true;
	return false;
}

std::string IRC_Logic::stringToLower(const std::string &input) const {
	std::string result;

	for (std::string::const_iterator it = input.begin();
		 it != input.end(); ++it) {
		result += tolower(*it);
	}
	return result;
}

std::string IRC_Logic::buildFullName(const std::vector<std::string> &splitMessageVector) {
	std::string name;

	for (size_t i = 4; i < splitMessageVector.size(); i++) {
		name += splitMessageVector[i] + " ";
	}
	cleanupName(&name);
	return name;
}

std::vector<std::string> IRC_Logic::extractFirstMessage(std::string string) {
	size_t pos;

	std::vector<std::string> result;
	while ((pos = string.find(" ")) != std::string::npos) {
		result.push_back(string.substr(0, pos));
		string.erase(0, pos + 1);
	}
	result.push_back(string);
	_remain.erase(0, _remain.find("\r\n") + 2);
	return result;
}

std::vector<IRC_User> IRC_Logic::getRegisteredUsers() {
	return _users;
}

std::vector<int> IRC_Logic::getReturnCodes() {
	return _returnCodes;
}

IRC_User *IRC_Logic::getUserByFd(const int &fd) {
	for (std::vector<IRC_User>::iterator it = _users.begin();
		 it != _users.end(); it++) {
		if (it->fd == fd)
			return &(*it);
	}
	return NULL;
}

bool IRC_Logic::userIsRegistered(IRC_User* user) {
	if (!user || user->nick == "" || user->name == "" || user->fullName == "")
		return false;
	return true;
}
