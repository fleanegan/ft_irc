#ifndef TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_
#define TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"


TEST(IRC_LogicUserRegistration, callingNickWhenSetChangesNick) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nick", "username", "Full Name");
	setNick(&logic, 0, "newNick");

	ASSERT_STREQ("newNick", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_LogicUserRegistration, receivingPingReturnsPong){
	IRC_Logic logic("password");
	std::string result;

	result = logic.processInput(0, "PING nonEmptyToken\r\n");

	ASSERT_TRUE(responseContains(result, "nonEmptyToken"));
}

TEST(IRC_LogicUserRegistration, receivingPingWithoutArgumentsReturnsError){
	IRC_Logic logic("password");
	std::string result;

	result = logic.processInput(0, "PING\r\n");

	ASSERT_TRUE(responseContains(result, ERR_NEEDMOREPARAMS));
}

TEST(IRC_LogicUserRegistration, receivingWhoIsShouldSendInformationsAboutUser){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(1, "WHOIS nick0\r\n");

	ASSERT_TRUE(responseContains(result, "nick0" ));
}

TEST(IRC_LogicUserRegistration, whoIsOnUnregisteredNicknameReturnsError){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(1, "WHOIS notExistingNick\r\n");

	ASSERT_TRUE(responseContains(result, ERR_NOSUCHNICK));
}

TEST(IRC_LogicUserRegistration, whoIsWithoutNicknameReturnsError){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(1, "WHOIS\r\n");

	ASSERT_TRUE(responseContains(result, ERR_NONICKNAMEGIVEN));
}

TEST(IRC_LogicUserRegistration, WhoWasReturnsInfoOnPreviouslyRegisteredUser) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 3);

	logic.disconnectUser(0);
	logic.disconnectUser(1);
	result = logic.processInput(2, "WHOWAS nick1\r\n");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_TRUE(responseContains(result, "nick1"));
}

TEST(IRC_LogicUserRegistration, whoWasWithoutNicknameReturnsError){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(1, "WHOWAS\r\n");

	ASSERT_TRUE(responseContains(result, ERR_NONICKNAMEGIVEN));
}

TEST(IRC_LogicUserRegistration, whoWasOnUnregisteredNicknameReturnsError){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(1, "WHOWAS notExistingNick\r\n");

	ASSERT_TRUE(responseContains(result, ERR_WASNOSUCHNICK));
}

TEST(IRC_LogicUserRegistration, changedNickAppearsOnWhoWas){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);
	setNick(&logic, 0, "setNick");
	setNick(&logic, 0, "unsetNick");
	setNick(&logic, 1, "setNick");
	setNick(&logic, 1, "unsetNick1");

	result = logic.processInput(1, "WHOWAS setNick\r\n");

	ASSERT_EQ(2, countSubStrings(result, "setNick"));
}

#endif //TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_

