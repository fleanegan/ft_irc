#ifndef TEST_TESTIRC_LOGIC_HPP_
#define TEST_TESTIRC_LOGIC_HPP_
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_Logic, zeroUsersAfterInstantiation){
	IRC_Logic logic;

	ASSERT_TRUE(logic.getClients().empty());
}

TEST(IRC_Logic, bullshitMessageDoesNotAddUser){
	IRC_Logic logic;

	logic.receive("Bullshit One Two Three Four\r\n");

	ASSERT_TRUE(logic.getClients().empty());
}

TEST(IRC_Logic, successfulRegistrationAddsUserToList){
	IRC_Logic logic;

	createUser(logic, "nick", "Full Name");

	ASSERT_EQ(1, logic.getClients().size());
	ASSERT_STREQ("nick", logic.getClients().at(0).nick.c_str());
	ASSERT_STREQ("Full Name", logic.getClients().at(0).name.c_str());
}

TEST(IRC_Logic, userCreationMessageFollowedByOtherContentStopsAtDelimeter){
	IRC_Logic logic;

	logic.receive("USER nick nick * ");
	logic.receive(":Full Name\r\n awetihoaw");

	ASSERT_EQ(1, logic.getClients().size());
	ASSERT_STREQ("nick", logic.getClients().at(0).nick.c_str());
	ASSERT_STREQ("Full Name", logic.getClients().at(0).name.c_str());
}

TEST(IRC_Logic, doubleRegistrationDoesNotCreateUser){
	IRC_Logic logic;

	createUser(logic, "nick", "Full Name");
	createUser(logic, "nick", "Full Name");

	ASSERT_EQ(1, logic.getClients().size());
	ASSERT_STREQ("nick", logic.getClients().at(0).nick.c_str());
	ASSERT_STREQ("Full Name", logic.getClients().at(0).name.c_str());
}


TEST(IRC_Logic, nickCannotStartWithReservedPrefix){
    IRC_Logic logic;

    createUser(logic, "#nick", "Full Name");
    createUser(logic, "$nick", "Full Name");
    createUser(logic, ":nick", "Full Name");

    ASSERT_TRUE(logic.getClients().empty());
}

TEST(IRC_Logic, creatingAnInvalidUserFollowedByAValidUserCreatesOneUser){
    IRC_Logic logic;

    createUser(logic, "hello*world", "Full Name");
    createUser(logic, "nick", "Full Name");

    ASSERT_EQ(1, logic.getClients().size());
}

TEST(IRC_Logic, nickCannotContainReservedCharacter){
    IRC_Logic logic;

    createUser(logic, "hello*world", "Full Name");
    createUser(logic, "hello,world", "Full Name");
    createUser(logic, "hello?world", "Full Name");
    createUser(logic, "hello!world", "Full Name");
    createUser(logic, "hello@world", "Full Name");
    createUser(logic, "hello world", "Full Name");

    ASSERT_TRUE(logic.getClients().empty());
}

#endif //TEST_TESTIRC_LOGIC_HPP_
