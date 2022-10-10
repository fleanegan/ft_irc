#ifndef INC_IRC_USER_HPP_
#define INC_IRC_USER_HPP_
#include <string>
#include <vector>
#include "utils.hpp"

struct IRC_User{
	typedef std::vector<IRC_User>::const_iterator ConstUserIterator;
	typedef std::vector<IRC_User>::iterator UserIterator;

	std::string nick;
    std::string userName;
    std::string fullName;
	std::string receivedCharacters;
	bool isAuthenticated;
    int fd;

	explicit IRC_User(int fd);
	IRC_User(const std::string &nick, const std::string &name, const int &fd);
	static bool isNickValid(const std::string &nick);
	static std::string buildFullName(const std::vector<std::string>& splitMessageVector);
	static UserIterator findUserByFdInVector(std::vector<IRC_User> *users, int fd) {
		for (UserIterator it = users->begin();
			 it != users->end(); it++) {
			if (it->fd == fd)
				return it;
		}
		return users->end();
	}
	static UserIterator findUserByNickInVector(std::vector<IRC_User> *users, const std::string &nick) {
		for (UserIterator it = users->begin();
			 it != users->end(); it++) {
			if (it->nick == nick)
				return it;
		}
		return users->end();
	}

private:
	IRC_User();
	//todo: implement
//	IRC_User(const IRC_User &);
//	IRC_User &operator=(const IRC_User &);
};



#endif // INC_IRC_USER_HPP_
