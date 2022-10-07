#ifndef TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_
#define TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_LogicPrivateMessage, sendingToNonExistingNickGeneratesError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 1);
	std::string result;

	result = logic.processInput(0, "PRIVMSG nonExisingNick content\r\n");

	ASSERT_TRUE(responseContains(result, ERR_NOSUCHNICK));
}

TEST(IRC_LogicPrivateMessage, noTextIsAProblem) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 1);
	std::string result;

	result = logic.processInput(0, "PRIVMSG nonExisingNick\r\n");

	ASSERT_TRUE(responseContains(result, ERR_NOTEXTTOSEND));
}

TEST(IRC_LogicPrivateMessage, sendingValidPrivateMessageRegistersMessage) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);

	logic.processInput(0, "PRIVMSG nick1 :content is cool\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().size());
	ASSERT_EQ(1, logic.getMessageQueue().front().recipients.size());
	ASSERT_EQ(1, logic.getMessageQueue().front().recipients.front());
	ASSERT_STREQ("content is cool", logic.getMessageQueue().front().content.c_str());
}

#endif //TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_
