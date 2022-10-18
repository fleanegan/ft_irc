#ifndef TEST_TESTIRC_LOGICMESSAGE_HPP_
#define TEST_TESTIRC_LOGICMESSAGE_HPP_

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

TEST(IRC_LogicNotice, noticeToNonexistingNickRepliesNothing) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "NOTICE nonexistingNick content\r\n");

	ASSERT_TRUE(logic.getMessageQueue().empty());
}

TEST(IRC_LogicNotice, noticeWithoutTextRepliesNothing) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "NOTICE nick1\r\n");

	ASSERT_TRUE(logic.getMessageQueue().empty());
}

TEST(IRC_LogicNotice, noticeToExistingNickRepliesNothingButSendsTextToRecipient) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "NOTICE nick1 content\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().size());
}

TEST(IRC_ChannelOperations, sendingMessageToChannelWithAlternativePrefix) {
	IRC_Logic logic("password");
	registerMembersAndJoinToChannel(&logic, 0, 2, "&chan");
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "NOTICE &chan hello\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().size());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content, "hello"));
}

TEST(IRC_ChannelOperations,
		sendingAMessageToANickEndingWithWildcardShouldFindIt) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "PRIVMSG nick* hello\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().size());
	ASSERT_EQ(1, logic.getMessageQueue().front().recipients.size());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content, "hello"));
}

TEST(wildcardUtils, sendingAMessagetoChannelWithWildcardSendToAllMatching) {
	IRC_Logic logic("password");
	registerMembersAndJoinToChannel(&logic, 0, 1, "#chan1");
	registerMembersAndJoinToChannel(&logic, 1, 2, "#chan2");
	registerDummyUser(&logic, 2, 3);
	logic.processRequest(2, "JOIN #chan1\r\n");
	logic.processRequest(2, "JOIN #chan2\r\n");
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(2, "PRIVMSG #chan* hello\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().size());
	ASSERT_EQ(2, logic.getMessageQueue().back().recipients.size());
}

TEST(wildcardUtils, matchingSingleStarAtEnd) {
	std::string input = "nick0";
	std::string expression = "nick*";

	ASSERT_TRUE(isMatchingWildcardExpression(input, expression));
}

TEST(wildcardUtils, noWildCardInExpression) {
	std::string input = "nick0";
	std::string expression = "nick0";

	ASSERT_TRUE(isMatchingWildcardExpression(input, expression));
}

TEST(wildcardUtils, wildCardAtBeginning) {
	std::string input = "nick0";
	std::string expression = "*ick0";

	ASSERT_TRUE(isMatchingWildcardExpression(input, expression));
}

TEST(wildcardUtils, wildCardInMiddle) {
	std::string input = "nick0";
	std::string expression = "n*0";

	ASSERT_TRUE(isMatchingWildcardExpression(input, expression));
}

TEST(wildcardUtils, multipleWildcards) {
	std::string input = "nick0";
	std::string expression = "n*c*******0******";

	ASSERT_TRUE(isMatchingWildcardExpression(input, expression));
}

TEST(wildcardUtils, singleReplaceWildcards) {
	std::string input = "nick0";
	std::string expression = "n*?*******?******";

	ASSERT_TRUE(isMatchingWildcardExpression(input.c_str(), expression.c_str()));
}


TEST(IRC_LogicPrivateMessage, messageSentWithAUsernameAndMaskShouldSend) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);

	logic.processRequest(0, "PRIVMSG username1@127.0.0.1 :content is cool\r\n");

	ASSERT_STREQ(":nick0!~username0@127.0.0.1 PRIVMSG username1@127.0.0.1 :content is cool",
			logic.getMessageQueue().back().content.c_str());
}

TEST(IRC_LogicPrivateMessage, SendingAMessageWithMaskWithoutDotReturnsError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);

	logic.processRequest(0, "PRIVMSG username1@localhost :content is cool\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_WILDTOPLEVEL));
}

TEST(IRC_LogicPrivateMessage,
		SendingAMessageWithMaskContainingWildCardAfterLastDotReturnsError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 2);

	logic.processRequest(0, "PRIVMSG username1@127.0.0.* :content is cool\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_WILDTOPLEVEL));
}


#endif  // TEST_TESTIRC_LOGICMESSAGE_HPP_
