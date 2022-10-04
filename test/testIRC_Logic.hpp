#ifndef TEST_TESTIRC_LOGIC_HPP_
#define TEST_TESTIRC_LOGIC_HPP_
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_Logic, zeroUsersAfterInstantiation){
	IRC_Logic logic;

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_Logic, bullshitMessageDoesNotAddUser){
	IRC_Logic logic;

	logic.receive(0, "Bullshit One Two Three Four\r\n");

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_Logic, userCreationMessageFollowedByOtherContentStopsAtDelimeter){
	IRC_Logic logic;

	logic.receive(0, "PASS password\r\nalkdfaslkfy");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, sendingPassMultipleTimeShouldNotCreateMultipleUser){
    IRC_Logic logic;

    authenticate(&logic, 0, "password");
    authenticate(&logic, 0, "password");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, nickCannotStartWithReservedPrefix){
    IRC_Logic logic;

    connectAndSetNick(&logic, 0, "password", "#nick");
    connectAndSetNick(&logic, 1, "password", "$nick");
    connectAndSetNick(&logic, 2, "password", ":nick");

    assertAllNicksEmpty(&logic);
}


TEST(IRC_Logic, nickCannotContainReservedCharacter){
    IRC_Logic logic;

    connectAndSetNick(&logic, 0, "password", "hello*world");
    connectAndSetNick(&logic, 1, "password", "hello,world");
    connectAndSetNick(&logic, 2, "password", "hello?world");
    connectAndSetNick(&logic, 3, "password", "hello!world");
    connectAndSetNick(&logic, 4, "password", "hello@world");
    connectAndSetNick(&logic, 5, "password", "hello world");

    assertAllNicksEmpty(&logic);
}

TEST(IRC_Logic, sendingTwoCommandAtOnceShouldExecuteBoth){
    IRC_Logic logic;

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
    createUser(&logic, 0, "nick", "Full Name");

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

/*
TEST(IRC_Logic, successfulRegistrationAddsUserToList){
    IRC_Logic logic;

   createUser(&logic, 0, "nick", "Full Name");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
    ASSERT_STREQ("nick", logic.getRegisteredUsers().at(0).nick.c_str());
    ASSERT_STREQ("Full Name", logic.getRegisteredUsers().at(0).name.c_str());
}
TEST(IRC_Logic, doubleRegistrationDoesNotCreateUser){
	IRC_Logic logic;

	createUser(&logic, "nick", "Full Name");
	createUser(&logic, "nick", "Full Name");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_STREQ("nick", logic.getRegisteredUsers().at(0).nick.c_str());
	ASSERT_STREQ("Full Name", logic.getRegisteredUsers().at(0).name.c_str());
}

TEST(IRC_Logic, weIgnoreAllCapabilityNegociation){
    IRC_Logic logic;

    logic.receive("CAP LS 302\r\n");
    createUser(&logic, "camembert", "De Meaux");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, userRegistrationSendsAcknowledgement){
    IRC_Logic logic;

    createUser(&logic, "camembert", "De Meaux");

    ASSERT_STRNE("", logic.popMessage().c_str());
    ASSERT_STREQ("", logic.popMessage().c_str());
}

*/

#endif //TEST_TESTIRC_LOGIC_HPP_
