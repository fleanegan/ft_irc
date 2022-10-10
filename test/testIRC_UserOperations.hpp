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
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "WHOIS nick0\r\n");

	//nick userName userName host * :fullName
	ASSERT_TRUE(responseContains(result, RPL_WHOISUSER));
}

TEST(IRC_LogicUserRegistration, removingUserFromServerMovesToWasList) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 3);

	logic.disconnectUser(0);
	logic.disconnectUser(1);
	result = logic.processInput(2, "WHOWAS nick1\r\n");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_TRUE(responseContains(result, "nick1"));
}

#endif //TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_

