#ifndef TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_
#define TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_

#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_LogicPrivateMessage, sendingToNonExistingNickGeneratesError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic);
	std::string result;

	result = logic.processInput(0, "PRIVMSG nonExisingNick content\r\n");

	ASSERT_TRUE(responseContainsCode(result, ERR_NOSUCHNICK));
}

TEST(IRC_LogicPrivateMessage, noTextIsAProblem) {
	IRC_Logic logic("password");
	registerDummyUser(&logic);
	std::string result;

	result = logic.processInput(0, "PRIVMSG nonExisingNick\r\n");

	ASSERT_TRUE(responseContainsCode(result, ERR_NOTEXTTOSEND));
}

TEST(IRC_LogicPrivateMessage, sendingValidPrivateMessageShouldNotGenerateError) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic);
	registerUser(&logic, 0, "password", "otherNick", "usernam", "Fuller Name");


	result = logic.processInput(0, "PRIVMSG otherNick content\r\n");

	ASSERT_STREQ("", result.c_str());
}

#endif //TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_
