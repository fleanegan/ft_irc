#ifndef TEST_TESTIRC_LOGIC_HPP_
#define TEST_TESTIRC_LOGIC_HPP_
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_Logic, zeroUsersAfterInstantiation){
	IRC_Logic logic("password");

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_Logic, noActionMeansNoReturnCode){
	IRC_Logic logic("password");

	ASSERT_TRUE(logic.getReturnCodes().empty());
}

TEST(IRC_Logic, bullshitMessageDoesNotAddUser){
	IRC_Logic logic("password");

	logic.receive(0, "Bullshit One Two Three Four\r\n");

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_Logic, userCreationMessageFollowedByOtherContentStopsAtDelimeter){
	IRC_Logic logic("password");

	logic.receive(0, "PASS password\r\nalkdfaslkfy");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, sendingPassMultipleTimeShouldNotCreateMultipleUser){
    IRC_Logic logic("password");

    authenticate(&logic, 0, "password");
    authenticate(&logic, 0, "password");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, nickCannotStartWithReservedPrefix){
    IRC_Logic logic("password");

    connectAndSetNick(&logic, 0, "password", "#nick");
    connectAndSetNick(&logic, 1, "password", "$nick");
    connectAndSetNick(&logic, 2, "password", ":nick");

    assertAllNicksEmpty(&logic);
}

TEST(IRC_Logic, nickCannotContainReservedCharacter){
    IRC_Logic logic("password");

    connectAndSetNick(&logic, 0, "password", "hello*world");
    connectAndSetNick(&logic, 1, "password", "hello,world");
    connectAndSetNick(&logic, 2, "password", "hello?world");
    connectAndSetNick(&logic, 3, "password", "hello!world");
    connectAndSetNick(&logic, 4, "password", "hello@world");
    connectAndSetNick(&logic, 5, "password", "hello world");

    assertAllNicksEmpty(&logic);
}

TEST(IRC_Logic, sendingTwoCommandAtOnceShouldExecuteBoth){
    IRC_Logic logic("password");

	logic.receive(0, "PASS password\r\nNICK JD\r\n");
    ASSERT_EQ(1, logic.getRegisteredUsers().size());
    ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_Logic, cannotSetNickWithoutSuccessfulPass){
    IRC_Logic logic("password");

	setNick(&logic, 0, "JD");

    ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_Logic, SuccessfulPassCommandAddsUser){
	IRC_Logic logic("password");

    logic.receive(0, "PASS password\r\n");
	setUser(&logic, 0, "nick", "Full Name");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, nickRegistersANickName){
    IRC_Logic logic("password");

    logic.receive(0, "PASS password\r\n");
    logic.receive(0, "NICK nickname\r\n");

    ASSERT_STREQ("nickname", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_Logic, passFromDifferentFdDoesNotUnlockNick){
    IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setNick(&logic, 1, "JD");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
    ASSERT_STREQ("", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_Logic, wrongPasswordShouldNotGenerateUser){
	IRC_Logic logic("password");

	authenticate(&logic, 0, "wrongPassword");

    ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_Logic, duplicateNickRequestMustNotBeSet){
	IRC_Logic logic("password");

    connectAndSetNick(&logic, 0, "password", "JD");
    connectAndSetNick(&logic, 1, "password", "JD");
	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
}

TEST(IRC_Logic, nickApprovalShouldBeCaseInsensitive){
	IRC_Logic logic("password");

    connectAndSetNick(&logic, 0, "password", "JD");
    connectAndSetNick(&logic, 1, "password", "jD");
	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
}

TEST(IRC_Logic, userMessageSetsUserNameAndFullName){
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	logic.receive(0, "USER JD JD * :John Doe\r\n");
	setNick(&logic, 0, "JayDee");
	ASSERT_STREQ("JayDee", logic.getRegisteredUsers()[0].nick.c_str());
	ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].name.c_str());
	ASSERT_STREQ("John Doe", logic.getRegisteredUsers()[0].fullName.c_str());
}

TEST(IRC_Logic, completedUserRegistrationSendsAcknowledgement){
    IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nick", "userName", "Full Name");

    ASSERT_EQ(RPL_WELCOME, logic.getReturnCodes()[0]);
    ASSERT_EQ(RPL_YOURHOST, logic.getReturnCodes()[1]);
    ASSERT_EQ(RPL_CREATED, logic.getReturnCodes()[2]);
    ASSERT_EQ(RPL_MYINFO, logic.getReturnCodes()[3]);
    ASSERT_EQ(RPL_ISUPPORT, logic.getReturnCodes()[4]);
}

TEST(IRC_Logic, incompletedUserRegistrationDoesNotSendAcknowledgement){
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setUser(&logic, 0, "userName", "Full Name");

	ASSERT_TRUE(logic.getReturnCodes().empty());
}

TEST(IRC_Logic, incompletedNickRegistrationDoesNotSendAcknowledgement){
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setNick(&logic, 0, "nick");

	ASSERT_TRUE(logic.getReturnCodes().empty());
}

/*
TEST(IRC_Logic, SendingNickMultipleTimeShouldSendAnError){
	IRC_Logic logic;

	createUser(&logic, "nick", "Full Name");
	createUser(&logic, "nick", "Full Name");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_STREQ("nick", logic.getRegisteredUsers().at(0).nick.c_str());
	ASSERT_STREQ("Full Name", logic.getRegisteredUsers().at(0).name.c_str());
}

TEST(IRC_Logic, weIgnoreAllCapabilityNegociation){
    IRC_Logic logic("password");

    logic.receive("CAP LS 302\r\n");
    setUser(&logic, "camembert", "De Meaux");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

*/

#endif //TEST_TESTIRC_LOGIC_HPP_
