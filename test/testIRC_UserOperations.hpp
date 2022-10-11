#ifndef TEST_TESTIRC_USEROPERATIONS_HPP_
#define TEST_TESTIRC_USEROPERATIONS_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"


TEST(IRC_UserOperations, callingNickWhenSetChangesNick) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nick", "username", "Full Name");
	setNick(&logic, 0, "newNick");

	ASSERT_STREQ("newNick", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_UserOperations, receivingPingReturnsPong) {
	IRC_Logic logic("password");

	logic.processInput(0, "PING nonEmptyToken\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"nonEmptyToken"));
}

TEST(IRC_UserOperations, receivingPingWithoutArgumentsReturnsError) {
	IRC_Logic logic("password");

    logic.processInput(0, "PING\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_UserOperations, receivingWhoIsShouldSendInformationsAboutUser) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);

	logic.processInput(1, "WHOIS nick0\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"nick0"));
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				RPL_WHOISUSER));
}

TEST(IRC_UserOperations, whoIsOnUnregisteredNicknameReturnsError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);

	logic.processInput(1, "WHOIS notExistingNick\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NOSUCHNICK));
}

TEST(IRC_UserOperations, whoIsWithoutNicknameReturnsError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);

	logic.processInput(1, "WHOIS\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NONICKNAMEGIVEN));
}

TEST(IRC_UserOperations, WhoWasReturnsInfoOnPreviouslyRegisteredUser) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 3);

	logic.disconnectUser(0);
	logic.disconnectUser(1);
	logic.processInput(2, "WHOWAS nick1\r\n");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"nick1"));
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				RPL_WHOWASUSER));
}

TEST(IRC_UserOperations, whoWasWithoutNicknameReturnsError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);

	logic.processInput(1, "WHOWAS\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NONICKNAMEGIVEN));
}

TEST(IRC_UserOperations, whoWasOnUnregisteredNicknameReturnsError) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);

	logic.processInput(1, "WHOWAS notExistingNick\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_WASNOSUCHNICK));
}

TEST(IRC_UserOperations, changedNickAppearsOnWhoWas) {
	IRC_Logic logic("password");
	registerDummyUser(&logic, 2);
	setNick(&logic, 0, "setNick");
	setNick(&logic, 0, "unsetNick");
	setNick(&logic, 1, "setNick");
	setNick(&logic, 1, "unsetNick1");

	logic.processInput(1, "WHOWAS setNick\r\n");

	ASSERT_EQ(2,
			countSubStrings(logic.getMessageQueue().back().content, "setNick"));
}

#endif  // TEST_TESTIRC_USEROPERATIONS_HPP_

