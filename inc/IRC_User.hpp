#ifndef INC_IRC_USER_HPP_
#define INC_IRC_USER_HPP_
#include <string>
#include <vector>

struct IRC_User{
	std::string nick;
    std::string name;
    std::string fullName;
	std::string receivedCharacters;
	std::string hostName;
	bool isAuthenticated;
    int fd;

	IRC_User(int fd, const std::string &hostName);
	IRC_User(const std::string &nick, const std::string &name, const int &fd);
	static bool isNickValid(const std::string &nick);
private:
	IRC_User();
	//todo: implement
//	IRC_User(const IRC_User &);
//	IRC_User &operator=(const IRC_User &);
};



#endif // INC_IRC_USER_HPP_
