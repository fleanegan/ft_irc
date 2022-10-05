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

TEST(IRC_Logic, firstConnectionOfFdAddsUserToList){
	IRC_Logic logic("password");

	logic.processInput(0, "Bullshit One Two Three Four\r\n", "localhost");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, userCreationMessageFollowedByOtherContentStopsAtDelimeter){
	IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\nalkdfaslkfy", "localhost");

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

	authenticateAndSetNick(&logic, 0, "password", "#nick");
	authenticateAndSetNick(&logic, 1, "password", "$nick");
	authenticateAndSetNick(&logic, 2, "password", ":nick");

    assertAllNicksEmpty(&logic);
}

TEST(IRC_Logic, nickCannotContainReservedCharacter){
    IRC_Logic logic("password");
	std::string result;

	authenticateAndSetNick(&logic, 0, "password", "hello*world");
	authenticateAndSetNick(&logic, 1, "password", "hello,world");
	authenticateAndSetNick(&logic, 2, "password", "hello?world");
	authenticateAndSetNick(&logic, 3, "password", "hello!world");
	authenticateAndSetNick(&logic, 4, "password", "hello@world");
    result = authenticateAndSetNick(&logic, 5, "password", "hello world");

    assertAllNicksEmpty(&logic);
	ASSERT_TRUE(reponseContainsCode(result, ERR_ERRONEOUSNICK));
}

TEST(IRC_Logic, sendingTwoCommandAtOnceShouldExecuteBoth){
    IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\nNICK JD\r\n", "localhost");
    ASSERT_EQ(1, logic.getRegisteredUsers().size());
    ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_Logic, cannotSetNickWithoutSuccessfulPass){
    IRC_Logic logic("password");

	setNick(&logic, 0, "JD");

    ASSERT_STREQ("", logic.getRegisteredUsers().front().nick.c_str());
}

TEST(IRC_Logic, SuccessfulPassCommandAddsUser){
	IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\n", "localhost");
	setUser(&logic, 0, "nick", "Full Name");

	ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_Logic, nickRegistersANickName){
    IRC_Logic logic("password");

	logic.processInput(0, "PASS password\r\n", "localhost");
	logic.processInput(0, "NICK nickname\r\n", "localhost");

    ASSERT_STREQ("nickname", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_Logic, passFromDifferentFdDoesNotUnlockNick){
    IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	setNick(&logic, 1, "JD");

    ASSERT_STREQ("", logic.getRegisteredUsers()[0].nick.c_str());
}

TEST(IRC_Logic, wrongPasswordDoesNotEnableRegistration){
	IRC_Logic logic("password");

	std::string result = registerUser(&logic, 0, "wrong password", "nick", "username", "Full Name");

    ASSERT_FALSE(logic.getRegisteredUsers().front().isAuthenticated);
	ASSERT_TRUE(reponseContainsCode(result, ERR_PASSWDMISMATCH));
}

TEST(IRC_Logic, noPasswordSendReturnsError){
	IRC_Logic logic("password");

	std::string result = logic.processInput(0, "PASS\r\n", "localhost");

	ASSERT_FALSE(logic.getRegisteredUsers().front().isAuthenticated);
	ASSERT_TRUE(reponseContainsCode(result, ERR_NEEDMOREPARAMS));
}

TEST(IRC_Logic, duplicatePassReturnsError){
	IRC_Logic logic("password");

	authenticate(&logic, 0, "password");
	std::string result = authenticate(&logic, 0, "password");
	
	ASSERT_TRUE(reponseContainsCode(result, ERR_ALREADYREGISTERED));
}

TEST(IRC_Logic, duplicateNickRequestMustNotBeSet){
	IRC_Logic logic("password");

	authenticateAndSetNick(&logic, 0, "password", "JD");
	authenticateAndSetNick(&logic, 1, "password", "JD");
	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
}

TEST(IRC_Logic, nickApprovalShouldBeCaseInsensitive){
	IRC_Logic logic("password");

	authenticateAndSetNick(&logic, 0, "password", "JD");
    std::string result= authenticateAndSetNick(&logic, 1, "password", "jD");

	ASSERT_STREQ("", logic.getRegisteredUsers()[1].nick.c_str());
	ASSERT_TRUE(reponseContainsCode(result, ERR_NICKNAMEINUSE));
}

TEST(IRC_Logic, nickWithoutParameterReturnsError) {
	IRC_Logic logic("password");
	std::string result;

    authenticate(&logic, 0, "password");
	result = logic.processInput(0, "NICK\r\n", "hostname");
	ASSERT_TRUE(reponseContainsCode(result, ERR_NONICKNAMEGIVEN));
}

TEST(IRC_Logic, userRegistrationWorksNoMatterTheUserNickOrder){
	IRC_Logic logic("password");
	std::string rep;

	authenticate(&logic, 0, "password");
	logic.processInput(0, "USER JD JD * :John Doe\r\n", "localhost");
	rep += setNick(&logic, 0, "JayDee");

	ASSERT_STREQ("JayDee", logic.getRegisteredUsers()[0].nick.c_str());
	ASSERT_STREQ("JD", logic.getRegisteredUsers()[0].name.c_str());
	ASSERT_STREQ("John Doe", logic.getRegisteredUsers()[0].fullName.c_str());
	ASSERT_TRUE(isValidUserRegistrationResponse(rep));
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

TEST(IRC_Logic, aUserPlacesMessageBetweenIncompleteMessageOfOtherUser){
	IRC_Logic logic("password");

	logic.processInput(0, "PASS passw", "localhost");
	authenticate(&logic, 1, "password");
	logic.processInput(0, "ord\r\n", "localhost");

	ASSERT_EQ(2, logic.getRegisteredUsers().size());
	ASSERT_TRUE(logic.getRegisteredUsers()[0].isAuthenticated);
	ASSERT_TRUE(logic.getRegisteredUsers()[1].isAuthenticated);
}

TEST(IRC_Logic, processInputReturnsMessageAccordingToUserRequest){
	IRC_Logic logic("password");
	std::string rep;
	authenticate(&logic, 0, "password");
	setNick(&logic, 0, "nick");

	rep = setUser(&logic, 0, "username", "Full Name");

	ASSERT_TRUE(isValidUserRegistrationResponse(rep));
}

TEST(IRC_Logic, blankUserNameReturnsError){
	IRC_Logic logic("password");
	std::string rep;
	authenticate(&logic, 0, "password");

	rep = logic.processInput(0, "USER\r\n", "localhost");

	ASSERT_TRUE(reponseContainsCode(rep, ERR_NEEDMOREPARAMS));
}

TEST(IRC_Logic, sendingUserMessageTwiceReturnsError){
	IRC_Logic logic("password");
	std::string rep;
	authenticate(&logic, 0, "password");

	rep = setUser(&logic, 0, "user", "Full User");
	rep = setUser(&logic, 0, "user", "Full User");

	ASSERT_TRUE(reponseContainsCode(rep, ERR_ALREADYREGISTERED));
}


// TEST(IRC_Logic, callingNickWhenSetChangesNick){
// TEST(IRC_Logic, truncateNickNameToNineCharacters){

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

    logic.processInput("CAP LS 302\r\n");
    setUser(&logic, "camembert", "De Meaux");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

*/

#endif //TEST_TESTIRC_LOGIC_HPP_
