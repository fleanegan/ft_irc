#ifndef TEST_TESTIRC_LOGICUSERREGISTRATION_HPP_
#define TEST_TESTIRC_LOGICUSERREGISTRATION_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_LogicUserRegistration, zeroUsersAfterInstantiation) {
	IRC_Logic logic("password");

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

TEST(IRC_LogicUserRegistration, firstConnectionOfFdAddsUserToList) {
	IRC_Logic logic("password");

	logic.processInput(0, "Bullshit One Two Three Four\r\n");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_LogicUserRegistration, userCreationMessageFollowedByOtherContentStopsAtDelimeter) {
	IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\nalkdfaslkfy");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_LogicUserRegistration, sendingPassMultipleTimeShouldNotCreateMultipleUser) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	authenticate(&logic, 0, "password");

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
	std::string result;

	authenticateAndSetNick(&logic, 0, "password", "hello*world");
	authenticateAndSetNick(&logic, 1, "password", "hello,world");
	authenticateAndSetNick(&logic, 2, "password", "hello?world");
	authenticateAndSetNick(&logic, 3, "password", "hello!world");
	authenticateAndSetNick(&logic, 4, "password", "hello@world");
	result = authenticateAndSetNick(&logic, 5, "password", "hello world");

	assertAllNicksEmpty(&logic);
	ASSERT_TRUE(responseContainsCode(result, ERR_ERRONEOUSNICK));
}

TEST(IRC_LogicUserRegistration, sendingTwoCommandAtOnceShouldExecuteBoth) {
	IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\nNICK JD\r\n");
	ASSERT_EQ(1, logic.getRegisteredUsers().size());
	ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_LogicUserRegistration, cannotSetNickWithoutSuccessfulPass) {
	IRC_Logic logic("password");

	setNick(&logic, 0, "JD");

	ASSERT_STREQ("", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_LogicUserRegistration, SuccessfulPassCommandAddsUser) {
	IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\n");
	setUser(&logic, 0, "nick", "Full Name");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_LogicUserRegistration, nickRegistersANickName) {
	IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\n");
	logic.processInput(0, "NICK nickname\r\n");

	ASSERT_STREQ("nickname", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_LogicUserRegistration, passFromDifferentFdDoesNotUnlockNick) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setNick(&logic, 1, "JD");

	ASSERT_STREQ("", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_LogicUserRegistration, wrongPasswordDoesNotEnableRegistration) {
	IRC_Logic logic("password");

	std::string result = registerUser(&logic, 0, "wrong password", "nick", "username", "Full Name");

	ASSERT_FALSE(logic.getRegisteredUsers().front().isAuthenticated);
	ASSERT_TRUE(responseContainsCode(result, ERR_PASSWDMISMATCH));
}

TEST(IRC_LogicUserRegistration, noPasswordSendReturnsError) {
	IRC_Logic logic("password");

	std::string result = logic.processInput(0, "PASS\r\n");

	ASSERT_FALSE(logic.getRegisteredUsers().front().isAuthenticated);
	ASSERT_TRUE(responseContainsCode(result, ERR_NEEDMOREPARAMS));
}

TEST(IRC_LogicUserRegistration, duplicatePassReturnsError) {
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	std::string result = authenticate(&logic, 0, "password");

	ASSERT_TRUE(responseContainsCode(result, ERR_ALREADYREGISTERED));
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
	std::string result = authenticateAndSetNick(&logic, 1, "password", "jD");

	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
	ASSERT_TRUE(responseContainsCode(result, ERR_NICKNAMEINUSE));
}

TEST(IRC_LogicUserRegistration, nickWithoutParameterReturnsError) {
	IRC_Logic logic("password");
	std::string result;

	authenticate(&logic, 0, "password");
	result = logic.processInput(0, "NICK\r\n");
	ASSERT_TRUE(responseContainsCode(result, ERR_NONICKNAMEGIVEN));
}

TEST(IRC_LogicUserRegistration, userRegistrationWorksNoMatterTheUserNickOrder) {
	IRC_Logic logic("password");
	std::string rep;

	authenticate(&logic, 0, "password");
	logic.processInput(0, "USER JD JD * John Doe\r\n");
	rep += setNick(&logic, 0, "JayDee");

	ASSERT_STREQ("JayDee", logic.getRegisteredUsers()[0].nick.c_str());
	ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].name.c_str());
	ASSERT_STREQ("John Doe", logic.getRegisteredUsers()[0].fullName.c_str());
	ASSERT_TRUE(isValidUserRegistrationResponse(rep));
}

TEST(IRC_LogicUserRegistration, incompletedUserRegistrationDoesNotSendAcknowledgement) {
	IRC_Logic logic("password");
	std::string result;

	result = authenticate(&logic, 0, "password");
	result += setUser(&logic, 0, "userName", "Full Name");

	ASSERT_STREQ("", result.c_str());
}

TEST(IRC_LogicUserRegistration, incompletedNickRegistrationDoesNotSendAcknowledgement) {
	IRC_Logic logic("password");
	std::string result;

	result += authenticate(&logic, 0, "password");
	result += setNick(&logic, 0, "nick");

	ASSERT_STREQ("", result.c_str());
}

TEST(IRC_LogicUserRegistration, aUserPlacesMessageBetweenIncompleteMessageOfOtherUser) {
	IRC_Logic logic("password");

	logic.processInput(0, "PASS passw");
	authenticate(&logic, 1, "password");
	logic.processInput(0, "ord\r\n");

	ASSERT_EQ(2, logic.getRegisteredUsers().size());
	ASSERT_TRUE(logic.getRegisteredUsers()[0].isAuthenticated);
	ASSERT_TRUE(logic.getRegisteredUsers()[1].isAuthenticated);
}

TEST(IRC_LogicUserRegistration, processInputReturnsMessageAccordingToUserRequest) {
	IRC_Logic logic("password");
	std::string rep;
	authenticate(&logic, 0, "password");
	setNick(&logic, 0, "nick");

	rep = setUser(&logic, 0, "username", "Full Name");

	ASSERT_TRUE(isValidUserRegistrationResponse(rep));
}

TEST(IRC_LogicUserRegistration, blankUserNameReturnsError) {
	IRC_Logic logic("password");
	std::string rep;
	authenticate(&logic, 0, "password");

	rep = logic.processInput(0, "USER\r\n");

	ASSERT_TRUE(responseContainsCode(rep, ERR_NEEDMOREPARAMS));
}

TEST(IRC_LogicUserRegistration, sendingUserMessageTwiceReturnsError) {
	IRC_Logic logic("password");
	std::string rep;
	authenticate(&logic, 0, "password");

	rep = setUser(&logic, 0, "user", "Full User");
	rep = setUser(&logic, 0, "user", "Full User");

	ASSERT_TRUE(responseContainsCode(rep, ERR_ALREADYREGISTERED));
}

TEST(IRC_LogicUserRegistration, irssiLoginSequenceRegistersAUser) {
	IRC_Logic logic("password");
	std::string rep;

	rep = setNick(&logic, 0, "nick");

	ASSERT_TRUE(responseContainsCode(rep, ERR_CONNECTWITHOUTPWD));
}

TEST(IRC_LogicUserRegistration, callingNickWhenSetChangesNick) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nick", "username", "Full Name");
	setNick(&logic, 0, "newNick");

	ASSERT_STREQ("newNick", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_LogicUserRegistration, truncateNickNameToNineCharacters) {
	IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nicknameisforwritingthelongestwordpossible", "username", "Full Name");

	ASSERT_STREQ("nicknamei", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_LogicUserRegistration, removingRealNameLeadingColon){
	IRC_Logic logic("password");
	std::string result;

	result = logic.processInput(0, "CAP LS 302\r\n");

	ASSERT_STREQ("", result.c_str());
}

TEST(IRC_LogicUserRegistration, capabilitiesNegociationRequestShouldBeignored){
    IRC_Logic logic("password");

	registerUser(&logic, 0, "password", "nick", "username", "Full name");

	ASSERT_STREQ("Full name", logic.getRegisteredUsers().front().fullName.c_str());
}

#endif //TEST_TESTIRC_LOGICUSERREGISTRATION_HPP_

