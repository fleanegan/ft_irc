#include <vector>
#include "../inc/IRC_Logic.hpp"

IRC_Logic::IRC_Logic(): _password("password") {
}

IRC_Logic::IRC_Logic( const std::string& password): _password(password) {
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
        splitMessageVector = splitMessage(_remain.substr(0, _remain.find("\r\n")));
        _remain.erase(0, _remain.find("\r\n") + 2);
        if (splitMessageVector[0] == "PASS" && !getUserByFd(fd)){
			if (splitMessageVector.size() == 2 && splitMessageVector[1] == _password){
				_users.push_back(IRC_User(fd));
			}
        }
		if (!getUserByFd(fd))
			return;
		if (isNickMessage(splitMessageVector)) {
            if (splitMessageVector.size() == 2 && !isNickAlreadyPresent(splitMessageVector[1]))
			    getUserByFd(fd)->nick = splitMessageVector[1];
		}
        if (isUserMessage(splitMessageVector)) {
            if (!IRC_User::isValidCreationString(splitMessageVector))
                return;
			_response += "Welcome\r\n";
        }
    }
}

bool IRC_Logic::isNickMessage(const std::vector<std::string> &splitMessageVector) const {
    return splitMessageVector[0] == "NICK"
               && IRC_User::isNickValid(splitMessageVector[1]);
}

bool IRC_Logic::isNickAlreadyPresent(const std::string &nick) {
    for (std::vector<IRC_User>::iterator user = _users.begin(); user != _users.end(); ++user)
        if (stringToLower(user->nick)== stringToLower(nick))
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

std::vector<std::string> IRC_Logic::splitMessage(std::string string) const {
	size_t pos;

	std::vector<std::string> result;
	while ((pos = string.find(" ")) != std::string::npos) {
		result.push_back (string.substr(0, pos));
		string.erase(0, pos + 1);
	}
	result.push_back(string);
	return result;
}

std::vector<IRC_User> IRC_Logic::getRegisteredUsers() {
	return _users;
}

std::string IRC_Logic::popMessage() {
	std::string ret;
	ret.swap(_response);
	return (ret);
}

IRC_User* IRC_Logic::getUserByFd(const int& fd) {
    for (std::vector<IRC_User>::iterator it = _users.begin();
            it  != _users.end(); it++){
        if (it->fd == fd)
            return &(*it);
    }
    return NULL;
}
