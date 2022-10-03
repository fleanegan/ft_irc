#ifndef TEST_TESTIRC_LOGIC_HPP_
#define TEST_TESTIRC_LOGIC_HPP_
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_Logic, unterminatedMessageDoesNotGenerateACommand){
	IRC_Logic logic;

	logic.receive("This is not a finished request");

	ASSERT_STREQ("", logic.getNextCommand().c_str());
}

TEST(IRC_Logic, commandGetsCreatedAfterTerminatingSequence){
	IRC_Logic logic;

	logic.receive("\r\n");

	ASSERT_STREQ("\r\n", logic.getNextCommand().c_str());
}

TEST(IRC_Logic, zeroUsersAfterInstantiation){
	IRC_Logic logic;

	ASSERT_TRUE(logic.getUsers().empty());
}

TEST(IRC_Logic, bullshitMessageDoesNotAddUser){
	IRC_Logic logic;

	logic.receive("Bullshit One Two Three Four\r\n");

	ASSERT_TRUE(logic.getUsers().empty());
}

TEST(IRC_Logic, successfulRegistrationAddsUserToList){
	IRC_Logic logic;

	logic.receive("USER nick nick * :Full Name\r\n");

	ASSERT_EQ(1, logic.getUsers().size());
	ASSERT_STREQ("nick", logic.getUsers().at(0).nick.c_str());
	ASSERT_STREQ("Full Name", logic.getUsers().at(0).name.c_str());
}

#endif //TEST_TESTIRC_LOGIC_HPP_
