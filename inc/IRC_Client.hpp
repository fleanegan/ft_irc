#ifndef FT_IRC_IRC_CLIENT_HPP
#define FT_IRC_IRC_CLIENT_HPP
#include <string>
#include <vector>

struct IRC_Client{
    std::string nick;
    std::string name;

    IRC_Client();

    IRC_Client(const std::string &nick, const std::string &name);

    static bool isValidCreationString(const std::vector<std::string>& splitMessageVector);
};



#endif //FT_IRC_IRC_CLIENT_HPP
