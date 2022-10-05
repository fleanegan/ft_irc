#ifndef TEST_TESTUTILS_HPP_
#define TEST_TESTUTILS_HPP_
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "../inc/IRC_Logic.hpp"
#include "../inc/IRC_User.hpp"
#include "../inc/return_code.hpp"

void authenticate(IRC_Logic *logic, int fd, const std::string &password ){
    logic->receive(fd, "PASS " + password + "\r\n");
}

void setUser(IRC_Logic *logic, int fd, const std::string &userName, const std::string &fullName) {
    logic->receive(fd, "USER " + userName + " " + userName + " * :" + fullName + "\r\n");
}

void setNick(IRC_Logic *logic, int fd, const std::string &nick) {
    logic->receive(fd, "NICK " + nick + "\r\n");
}

void connectAndSetNick(IRC_Logic *logic, int fd, const std::string &password, const std::string &nick){
    authenticate(logic, fd, password);
    setNick(logic, fd, nick);
}

void registerUser(IRC_Logic *logic, int fd, const std::string &password, const std::string &nick, const std::string &userName, const std::string &fullName){
	authenticate(logic, fd, password);
	setNick(logic, fd, nick);
	setUser(logic, fd, userName, fullName);
}

void assertAllNicksEmpty(IRC_Logic *logic) {
    std::vector<IRC_User> users = logic->getRegisteredUsers();
    for (std::vector<IRC_User>::iterator it = users.begin(); it != users.end(); ++it)
        ASSERT_STREQ("", it->nick.c_str());
}

#endif     // TEST_TESTUTILS_HPP_
