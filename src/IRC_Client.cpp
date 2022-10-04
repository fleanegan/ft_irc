#include "inc/IRC_Client.hpp"

IRC_Client::IRC_Client() : nick("JD"), name("John Doe"){
}

IRC_Client::IRC_Client(const std::string &nick, const std::string &name) :
        nick(nick), name(name) {
}

bool IRC_Client::isValidCreationString(const std::vector<std::string> &splitMessageVector) {
    std::string forbiddenChars = "*,!?@";
    const std::string &nick = splitMessageVector[1];
    size_t	fullNameBegin = 0;

    while (fullNameBegin < splitMessageVector.size()
           && splitMessageVector[fullNameBegin][0] != ':')
        fullNameBegin++;
    if (fullNameBegin >= 5 || fullNameBegin == splitMessageVector.size())
        return false;
    if (nick[0] == '#' || nick[0] == '$' || nick[0] == ':')
        return false;
    for (std::string::iterator it = forbiddenChars.begin(); it != forbiddenChars.end(); ++it)
        if (nick.find(*it) != std::string::npos)
            return false;
    return true;
}