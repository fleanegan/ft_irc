#include "../inc/IRC_User.hpp"

IRC_User::IRC_User(int fd) : isAuthenticated(false), fd(fd){
}

IRC_User::IRC_User(const std::string &nick, const std::string &name, const int &fd):
        nick(nick), name(name), isAuthenticated(false), fd(fd) {
}

bool IRC_User::isNickValid(const std::string &nick) {
    std::string forbiddenChars = "*,!?@";

    for (std::string::iterator it = forbiddenChars.begin(); it != forbiddenChars.end(); ++it)
        if (nick.find(*it) != std::string::npos)
            return false;
    if (nick[0] == '#' || nick[0] == '$' || nick[0] == ':')
        return false;
    return true;
}

std::string IRC_User::buildFullName(const std::vector<std::string> &splitMessageVector) {
	std::string name = concatenateContentFromIndex(4, splitMessageVector);

	removeLeadingColon(&name);
	return name;
}

