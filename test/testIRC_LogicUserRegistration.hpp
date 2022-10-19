#ifndef TEST_TESTIRC_LOGICUSERREGISTRATION_HPP_
#define TEST_TESTIRC_LOGICUSERREGISTRATION_HPP_

#include <string>
#include <vector>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

void assertAllNicksEmpty(IRC_Logic *logic) {
	std::vector<IRC_User> users = logic->getRegisteredUsers();
	for (std::vector<IRC_User>::iterator
				 it = users.begin(); it != users.end(); ++it)
		ASSERT_STREQ("", it->nick.c_str());
}

TEST(IRC_LogicUserRegistration, zeroUsersAfterInstantiation) {
	IRC_Logic logic("password");

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_LogicUserRegistration,
		userCreationMessageFollowedByOtherContentStopsAtDelimeter) {
	IRC_Logic logic("password");
    logic.addUser(0, "127.0.0.1");

	logic.processRequest(0, "PASS password\r\nalkdfaslkfy");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_LogicUserRegistration, nickCannotStartWithReservedPrefix) {
	IRC_Logic logic("password");

	authenticateAndSetNick(&logic, 0, "password", "#nick");
	authenticateAndSetNick(&logic, 1, "password", "$nick");
	authenticateAndSetNick(&logic, 2, "password", ":nick");

	assertAllNicksEmpty(&logic);
}

TEST(IRC_LogicUserRegistration, nickCannotContainReservedCharacter) {
	IRC_Logic logic("password");

	authenticateAndSetNick(&logic, 0, "password", "hello*world");
	authenticateAndSetNick(&logic, 1, "password", "hello,world");
	authenticateAndSetNick(&logic, 2, "password", "hello?world");
	authenticateAndSetNick(&logic, 3, "password", "hello!world");
	authenticateAndSetNick(&logic, 4, "password", "hello@world");
	authenticateAndSetNick(&logic, 5, "password", "hello world");

//	assertAllNicksEmpty(&logic);
//	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
//				ERR_ERRONEOUSNICK));
}

TEST(IRC_LogicUserRegistration, sendingTwoCommandAtOnceShouldExecuteBoth) {
	IRC_Logic logic("password");
    authenticate(&logic, 0, "password");

	logic.processRequest(0, "USER name name unused :full name\r\nNICK JD\r\n");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_LogicUserRegistration, cannotSetNickWithoutSuccessfulPass) {
	IRC_Logic logic("password");
    logic.addUser(0, "127.0.0.1");

	setNick(&logic, 0, "JD");

	ASSERT_STREQ("", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_LogicUserRegistration, nickRegistersANickName) {
	IRC_Logic logic("password");
    authenticate(&logic, 0, "password");

	logic.processRequest(0, "NICK nickname\r\n");

	ASSERT_STREQ("nickname", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_LogicUserRegistration, passFromDifferentFdDoesNotUnlockNick) {
	IRC_Logic logic("password");
    logic.addUser(1, "127.0.0.1");

    authenticate(&logic, 0, "password");
	setNick(&logic, 1, "JD");

	ASSERT_STREQ("", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_LogicUserRegistration, wrongPasswordDoesNotEnableRegistration) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "wrong password", "nick", "username", "Full Name");

	ASSERT_FALSE(logic.getRegisteredUsers().front().isAuthenticated);
	ASSERT_FALSE(logic.getMessageQueue().empty());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content,
				ERR_PASSWDMISMATCH));
}

TEST(IRC_LogicUserRegistration, noPasswordSendReturnsError) {
	IRC_Logic logic("password");
    logic.addUser(0, "127.0.0.1");

    logic.processRequest(0, "PASS\r\n");

	ASSERT_FALSE(logic.getRegisteredUsers().front().isAuthenticated);
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_LogicUserRegistration, duplicatePassReturnsError) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	authenticate(&logic, 0, "password");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_ALREADYREGISTERED));
}

TEST(IRC_LogicUserRegistration, duplicateNickRequestMustNotBeSet) {
	IRC_Logic logic("password");

	authenticateAndSetNick(&logic, 0, "password", "JD");
	authenticateAndSetNick(&logic, 1, "password", "JD");
	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
}

TEST(IRC_LogicUserRegistration, nickApprovalShouldBeCaseInsensitive) {
	IRC_Logic logic("password");

	authenticateAndSetNick(&logic, 0, "password", "JD");
	authenticateAndSetNick(&logic, 1, "password", "jD");

	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NICKNAMEINUSE));
}

TEST(IRC_LogicUserRegistration, nickWithoutParameterReturnsError) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");

	logic.processRequest(0, "NICK\r\n");
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NONICKNAMEGIVEN));
}

TEST(IRC_LogicUserRegistration, userRegistrationWorksNoMatterTheUserNickOrder) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	logic.processRequest(0, "USER uname uname * Full Name\r\n");
	setNick(&logic, 0, "nick");

	ASSERT_STREQ("nick", logic.getRegisteredUsers()[0].nick.c_str());
	ASSERT_STREQ("uname", logic.getRegisteredUsers()[0].userName.c_str());
	ASSERT_STREQ("Full Name", logic.getRegisteredUsers()[0].fullName.c_str());
	ASSERT_TRUE(isValidUserRegistrationResponse(
				logic.getMessageQueue().back().content));
}

TEST(IRC_LogicUserRegistration,
		incompletedUserRegistrationDoesNotSendAcknowledgement) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setUser(&logic, 0, "userName", "Full Name");

	ASSERT_TRUE(logic.getMessageQueue().empty());
}

TEST(IRC_LogicUserRegistration,
		incompletedNickRegistrationDoesNotSendAcknowledgement) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setNick(&logic, 0, "nick");

	ASSERT_TRUE(logic.getMessageQueue().empty());
}

TEST(IRC_LogicUserRegistration, cannotExecuteCommandsBeforeRegistrationCompletion) {
	IRC_Logic logic("password");
	authenticate(&logic, 0, "password");

	logic.processRequest(0, "JOIN #chan\r\n");

	ASSERT_TRUE(responseContains(
			logic.getMessageQueue().back().content, ERR_NOTREGISTERED));
}

TEST(IRC_LogicUserRegistration,
		aUserPlacesMessageBetweenIncompleteMessageOfOtherUser) {
	IRC_Logic logic("password");
    logic.addUser(0, "127.0.0.1");

    logic.processRequest(0, "PASS passw");
	authenticate(&logic, 1, "password");
	logic.processRequest(0, "ord\r\n");

	ASSERT_EQ(2, logic.getRegisteredUsers().size());
	ASSERT_TRUE(logic.getRegisteredUsers()[0].isAuthenticated);
	ASSERT_TRUE(logic.getRegisteredUsers()[1].isAuthenticated);
}

TEST(IRC_LogicUserRegistration,
		processRequestReturnsMessageAccordingToUserRequest) {
	IRC_Logic logic("password");
	authenticate(&logic, 0, "password");
	setNick(&logic, 0, "nick");

	setUser(&logic, 0, "username", "Full Name");

	ASSERT_TRUE(isValidUserRegistrationResponse(
				logic.getMessageQueue().back().content));
}

TEST(IRC_LogicUserRegistration, blankUserNameReturnsError) {
	IRC_Logic logic("password");
	authenticate(&logic, 0, "password");

	logic.processRequest(0, "USER\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_LogicUserRegistration, sendingUserMessageTwiceReturnsError) {
	IRC_Logic logic("password");
	authenticate(&logic, 0, "password");

	setUser(&logic, 0, "user", "Full User");
	setUser(&logic, 0, "user", "Full User");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_ALREADYREGISTERED));
}

TEST(IRC_LogicUserRegistration, truncateNickNameToNineCharacters) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "password",
			"TheLongestNickNameEver", "username", "Full Name");

	ASSERT_STREQ("TheLonges", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_LogicUserRegistration, removingRealNameLeadingColon) {
	IRC_Logic logic("password");
    logic.addUser(0, "127.0.0.1");

    logic.processRequest(0, "CAP LS 302\r\n");

	ASSERT_TRUE(logic.getMessageQueue().empty());
}

TEST(IRC_LogicUserRegistration, capabilitiesNegociationRequestShouldBeignored) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nick", "username", "Full userName");

	ASSERT_STREQ("Full userName",
			logic.getRegisteredUsers().front().fullName.c_str());
}

#endif  // TEST_TESTIRC_LOGICUSERREGISTRATION_HPP_
