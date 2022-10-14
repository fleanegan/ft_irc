#ifndef INC_IRC_USER_HPP_
#define INC_IRC_USER_HPP_
#include <string>
#include <vector>
#include "./utils.hpp"

struct IRC_User{
    typedef std::vector<IRC_User>::const_iterator ConstUserIterator;
	typedef std::vector<IRC_User>::iterator UserIterator;

	std::string nick;
	std::string userName;
	std::string fullName;
	std::string hostIp;
	std::string receivedCharacters;
	bool isAuthenticated;
	int fd;
    bool isOper;

	IRC_User(int fd, const std::string& hostIP);
	IRC_User(const std::string &nick, const std::string &name,
			const int &fd, const std::string &hostIP);
	static bool isNickValid(const std::string &nick);
	static std::string buildFullName(
			const std::vector<std::string>& splitMessageVector);
	std::string toString() const;
	static UserIterator findUserByFdInVector(
			std::vector<IRC_User> *users, int fd);
	static UserIterator findUserByNickInVector(
			std::vector<IRC_User> *users, const std::string &nick);
    bool isValid(){
        if (nick.empty() || userName.empty() || fullName.empty())
            return false;
        return true;
    }

	std::string toPrefixString() const;

	private:
	IRC_User();

	// todo: implement
	// IRC_User(const IRC_User &);
	// IRC_User &operator=(const IRC_User &);
};



#endif  // INC_IRC_USER_HPP_
