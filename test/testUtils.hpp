#ifndef TEST_TESTUTILS_HPP_
#define TEST_TESTUTILS_HPP_
#include <string>
#include <vector>
#include <sstream>
#include "gtest/gtest.h"
#include "../inc/IRC_Logic.hpp"
#include "../inc/IRC_User.hpp"
#include "../inc/return_code.hpp"

std::string authenticate(
		IRC_Logic *logic, int fd, const std::string &password ) {
	return logic->processInput(fd, "PASS " + password + "\r\n");
}

std::string setUser(IRC_Logic *logic,
		int fd, const std::string &userName, const std::string &fullName) {
	return logic->processInput(fd, "USER "
			+ userName + " " + userName + " * :" + fullName + "\r\n");
}

std::string setNick(IRC_Logic *logic, int fd, const std::string &nick) {
	return logic->processInput(fd, "NICK " + nick + "\r\n");
}

std::string authenticateAndSetNick(IRC_Logic *logic,
		int fd, const std::string &password, const std::string &nick) {
	std::string result;
    result += authenticate(logic, fd, password);
    result += setNick(logic, fd, nick);
	return result;
}

std::string registerUser(IRC_Logic *logic,
		int fd,
		const std::string &password,
		const std::string &nick,
		const std::string &userName,
		const std::string &fullName) {
	std::string result;

	result += authenticate(logic, fd, password);
	result += setNick(logic, fd, nick);
	result += setUser(logic, fd, userName, fullName);

	return result;
}

void assertAllNicksEmpty(IRC_Logic *logic) {
    std::vector<IRC_User> users = logic->getRegisteredUsers();
    for (std::vector<IRC_User>::iterator
			it = users.begin(); it != users.end(); ++it)
        ASSERT_STREQ("", it->nick.c_str());
}

bool isValidUserRegistrationResponse(const std::string &returnMessage) {
	return 	returnMessage.find(RPL_WELCOME) != std::string::npos &&
			returnMessage.find(RPL_YOURHOST) != std::string::npos &&
			returnMessage.find(RPL_CREATED) != std::string::npos &&
			returnMessage.find(RPL_MYINFO) != std::string::npos &&
			returnMessage.find(RPL_ISUPPORT) != std::string::npos;
}

bool responseContains(
		const std::string &returnMessage, const std::string &code) {
	return 	returnMessage.find(code) != std::string::npos;
}

void registerDummyUser(IRC_Logic *logic, int n) {
	std::string nick("nick");
	std::string userName("username");
	std::string fullName("Full Name");
	std::string indexString;

	while (--n >= 0) {
		std::stringstream ss;
		ss << n;
		ss >> indexString;
		registerUser(logic, n, "password",
				nick + indexString,
				userName + indexString,
				fullName + indexString);
	}
}

void registerMembersAndJoinToChannel(IRC_Logic *logic, int n) {
    registerDummyUser(logic, n);
    while (--n >= 0) {
        logic->processInput(n, "JOIN #chan\r\n");
    }
}

int countSubStrings(const std::string &string, const std::string &toFind) {
	int occurrences = 0;

	std::string::size_type pos = 0;
	while ((pos = string.find(toFind, pos)) != std::string::npos) {
		++occurrences;
		pos += toFind.length();
	}
	return occurrences;
}

#endif     // TEST_TESTUTILS_HPP_
