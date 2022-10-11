#include "../inc/IRC_User.hpp"

IRC_User::IRC_User(int fd):
	hostName("127.0.0.1"), isAuthenticated(false), fd(fd) {}

IRC_User::IRC_User(
		const std::string &nick, const std::string &name, const int &fd):
	nick(nick), userName(name),
		hostName("127.0.0.1"), isAuthenticated(false), fd(fd) {}

bool IRC_User::isNickValid(const std::string &nick) {
    std::string forbiddenChars = "*,!?@";

    for (std::string::iterator
			it = forbiddenChars.begin(); it != forbiddenChars.end(); ++it)
        if (nick.find(*it) != std::string::npos)
            return false;
    if (nick[0] == '#' || nick[0] == '$' || nick[0] == ':')
        return false;
    return true;
}

std::string IRC_User::buildFullName(
		const std::vector<std::string> &splitMessageVector) {
	std::string name = concatenateContentFromIndex(4, splitMessageVector);

	removeLeadingColon(&name);
	return name;
}

IRC_User::UserIterator IRC_User::findUserByFdInVector(
		std::vector<IRC_User> *users, int fd) {
	for (UserIterator it = users->begin();
		 it != users->end(); it++) {
		if (it->fd == fd)
			return it;
	}
	return users->end();
}

IRC_User::UserIterator IRC_User::findUserByNickInVector(
		std::vector<IRC_User> *users, const std::string &nick) {
	for (UserIterator it = users->begin();
		 it != users->end(); it++) {
		if (it->nick == nick)
			return it;
	}
	return users->end();
}

std::string IRC_User::toString() const {
	return nick + " " + userName + " " + hostName + " * :" + fullName + "\r\n";
}

std::string IRC_User::toPrefixString() const {
    return ":" + nick + "!~" + userName + "@" + hostName + " ";
}

