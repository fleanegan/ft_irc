#ifndef TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_
#define TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_LogicPrivateMessage, sendingToNonExistingNickGeneratesError) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);

	logic.processRequest(0, "PRIVMSG nonExisingNick content\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NOSUCHNICK));
}

TEST(IRC_LogicPrivateMessage, noTextIsAProblem) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);

	logic.processRequest(0, "PRIVMSG nonExisingNick\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NOTEXTTOSEND));
}

TEST(IRC_LogicPrivateMessage, sendingValidPrivateMessageRegistersMessage) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

	logic.processRequest(0, "PRIVMSG nick1 :content is cool\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.front());
}

TEST(IRC_LogicPrivateMessage, messagesSentWithPrefixContainingNick) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

	logic.processRequest(0, "PRIVMSG nick1 :content is cool\r\n");

	ASSERT_STREQ(":nick0!~username0@127.0.0.1 PRIVMSG nick1 :content is cool",
			logic.getMessageQueue().back().content.c_str());
}

#endif  // TEST_TESTIRC_LOGICPRIVATEMESSAGE_HPP_
