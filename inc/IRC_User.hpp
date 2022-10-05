#ifndef INC_IRC_USER_HPP_
#define INC_IRC_USER_HPP_
#include <string>
#include <vector>

struct IRC_User{
    std::string nick;
    std::string name;
    std::string fullName;
    int fd;

    explicit IRC_User(const int &fd);
    IRC_User(const std::string &nick, const std::string &name, const int &fd);
    static bool isValidCreationString(const std::vector<std::string>& splitMessageVector);
    static bool isNickValid(const std::string &nick);
private:
		IRC_User();

};



#endif // INC_IRC_USER_HPP_
