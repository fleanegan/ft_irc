#ifndef TEST_TESTUTILS_HPP_
#define TEST_TESTUTILS_HPP_
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "../inc/IRC_Logic.hpp"
#include "../inc/IRC_User.hpp"
#include "../inc/return_code.hpp"

std::string authenticate(IRC_Logic *logic, int fd, const std::string &password ){
	return logic->processInput(fd, "PASS " + password + "\r\n", "hostName");
}

std::string setUser(IRC_Logic *logic, int fd, const std::string &userName, const std::string &fullName) {
	return logic->processInput(fd, "USER " + userName + " " + userName + " * :" + fullName + "\r\n", "hostName");
}

std::string setNick(IRC_Logic *logic, int fd, const std::string &nick) {
	return logic->processInput(fd, "NICK " + nick + "\r\n", "hostName");
}

std::string connectAndSetNick(IRC_Logic *logic, int fd, const std::string &password, const std::string &nick){
	std::string result;
    result += authenticate(logic, fd, password);
    result += setNick(logic, fd, nick);
	return result;
}

std::string registerUser(IRC_Logic *logic, int fd, const std::string &password, const std::string &nick, const std::string &userName, const std::string &fullName){
	std::string result;

	result += authenticate(logic, fd, password);
	result += setNick(logic, fd, nick);
	result += setUser(logic, fd, userName, fullName);

	return result;
}

void assertAllNicksEmpty(IRC_Logic *logic) {
    std::vector<IRC_User> users = logic->getRegisteredUsers();
    for (std::vector<IRC_User>::iterator it = users.begin(); it != users.end(); ++it)
        ASSERT_STREQ("", it->nick.c_str());
}

bool isValidUserRegistrationResponse(const std::string &returnMessage){
	return 	returnMessage.find(RPL_WELCOME) != std::string::npos &&
			returnMessage.find(RPL_YOURHOST) != std::string::npos &&
			returnMessage.find(RPL_CREATED) != std::string::npos &&
			returnMessage.find(RPL_MYINFO) != std::string::npos &&
			returnMessage.find(RPL_ISUPPORT) != std::string::npos;
}

bool reponseContainsCode(const std::string &returnMessage, const std::string &code){
	return 	returnMessage.find(code) != std::string::npos;
}



#endif     // TEST_TESTUTILS_HPP_
