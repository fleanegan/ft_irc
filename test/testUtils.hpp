#ifndef TEST_TESTUTILS_HPP_
#define TEST_TESTUTILS_HPP_
#include "../inc/IRC_Logic.hpp"
#include "../inc/IRC_Client.hpp"

void createUser(IRC_Logic& logic, const std::string& nick,
		const std::string& name) {
	logic.receive("USER " + nick + " " + nick + " * :" + name + "\r\n");
}

#endif     // TEST_TESTUTILS_HPP_
