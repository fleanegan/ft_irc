#ifndef TEST_TESTIRC_USEROPERATIONS_HPP_
#define TEST_TESTIRC_USEROPERATIONS_HPP_

#include <string>
#include <vector>
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
    logic.addUser(0, "127.0.0.1");

    logic.processRequest(0, "PING nonEmptyToken\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 "nonEmptyToken"));
}

TEST(IRC_UserOperations, receivingPingWithoutArgumentsReturnsError) {
    IRC_Logic logic("password");
    logic.addUser(0, "127.0.0.1");

    logic.processRequest(0, "PING\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_NEEDMOREPARAMS));
}

TEST(IRC_UserOperations, receivingWhoIsShouldSendInformationsAboutUser) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

    logic.processRequest(1, "WHOIS nick0\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 "nick0"));
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 RPL_WHOISUSER));
}

TEST(IRC_UserOperations, whoIsOnUnregisteredNicknameReturnsError) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

    logic.processRequest(1, "WHOIS notExistingNick\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_NOSUCHNICK));
}

TEST(IRC_UserOperations, whoIsWithoutNicknameReturnsError) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

    logic.processRequest(1, "WHOIS\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_NONICKNAMEGIVEN));
}

TEST(IRC_UserOperations, WhoWasReturnsInfoOnPreviouslyRegisteredUser) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 3);

    logic.disconnectUser(0, "leaving");
    logic.disconnectUser(1, "leaving");
    logic.processRequest(2, "WHOWAS nick1\r\n");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 "nick1"));
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 RPL_WHOWASUSER));
}

TEST(IRC_UserOperations, whoWasWithoutNicknameReturnsError) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

    logic.processRequest(1, "WHOWAS\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_NONICKNAMEGIVEN));
}

TEST(IRC_UserOperations, whoWasOnUnregisteredNicknameReturnsError) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

    logic.processRequest(1, "WHOWAS notExistingNick\r\n");

    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_WASNOSUCHNICK));
}

TEST(IRC_UserOperations, changedNickAppearsOnWhoWas) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);
    setNick(&logic, 0, "setNick");
    setNick(&logic, 0, "unsetNick");
    setNick(&logic, 1, "setNick");
    setNick(&logic, 1, "unsetNick1");

    logic.processRequest(1, "WHOWAS setNick\r\n");

    ASSERT_EQ(2,
              countSubStrings(logic.getMessageQueue().back().content, "setNick"));
}

TEST(IRC_UserOperations,
     quitRemovesUserFromServerAndNotifiesOtherChannelmembers) {
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");
    registerUser(&logic, 2, "password", "nick", "username", "full name");

    emptyQueue(&logic.getMessageQueue());
    logic.processRequest(0, "QUIT reason\r\n");

    ASSERT_EQ(2, logic.getRegisteredUsers().size());
    ASSERT_EQ(2, logic.getMessageQueue().front().recipients.size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content,
                                 "reason"));
}

TEST(IRC_UserOperations,
     callingQuitAndDisconnectDoesNotAtttemptToRemoveUserTwice) {
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");

    logic.processRequest(0, "QUIT reason\r\n");
    logic.disconnectUser(0, "Causes segfault when not handled with care");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

TEST(IRC_UserOperations, quitWithoutArgumentGeneratesLeaving) {
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 1, "#chan");
    emptyQueue(&logic.getMessageQueue());

    logic.processRequest(0, "QUIT\r\n");

    ASSERT_TRUE(logic.getRegisteredUsers().empty());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content,
                                 "leaving"));
}

TEST(IRC_UserOperations, quitRepliesErrorToSender) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);

    logic.processRequest(0, "QUIT reason\r\n");

    ASSERT_TRUE(logic.getRegisteredUsers().empty());
    ASSERT_TRUE(countMessageContaining(logic.getMessageQueue(),
                                       ERR_CLOSINGLINK));
}

TEST(IRC_UserOperations, changingNickNotifiesOnlyChannelMembers) {
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "chanA");
    registerMembersAndJoinToChannel(&logic, 2, 3, "chanB");
    emptyQueue(&logic.getMessageQueue());

    setNick(&logic, 0, "newNick");

    std::vector<int> recipientsOfNickChange = gatherAllRecipientsOfMessageQueue(&logic);
    ASSERT_EQ(1, recipientsOfNickChange.size());
}

TEST(IRC_UserOperations, changingNickDoesNotSendWelcomeMessage) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);

    setNick(&logic, 0, "newNick");

    ASSERT_EQ(1, countMessageContaining(logic.getMessageQueue(), RPL_WELCOME));
}

TEST(IRC_UserOperations, changingNickDoesNotBreakChannelMessages) {
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 1, "chan");

    setNick(&logic, 0, "newNick");

    ASSERT_TRUE(logic.getChannels().back().isUserInChannel(logic.getRegisteredUsers().back()));
}

TEST(IRC_UserOperations, operWithoutEnoughArgumentsReturnsError) {
    IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 1);
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "OPER nick0\r\n");
	logic.processRequest(0, "OPER\r\n");

    ASSERT_EQ(2, logic.getMessageQueue().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content,
				ERR_NEEDMOREPARAMS));
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_UserOperations, operWithoutCorrectPasswordReturnsError) {
    IRC_Logic logic("password");
	registerDummyUser(&logic, 0, 1);
	emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "OPER oper wrongPassword\r\n");
	logic.processRequest(0, "OPER wrongNick operPassword\r\n");

    ASSERT_EQ(2, logic.getMessageQueue().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_PASSWDMISMATCH));
    ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content,
				ERR_PASSWDMISMATCH));
}

TEST(IRC_UserOperations, operWithCorrectPasswordPromotesUser) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);
    emptyQueue(&logic.getMessageQueue());

    logic.processRequest(0, "OPER oper operPassword\r\n");

    ASSERT_EQ(1, logic.getMessageQueue().size());
    ASSERT_TRUE(logic.getUserByNick("nick0")->isOper);
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 RPL_YOUREOPER));
}

TEST(IRC_UserOperations, modeWithoutArgumentsReturnsError) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);
    emptyQueue(&logic.getMessageQueue());

    logic.processRequest(0, "MODE\r\n");

    ASSERT_EQ(1, logic.getMessageQueue().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_UserOperations, normalUserCannotUseKill) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);
    emptyQueue(&logic.getMessageQueue());

    logic.processRequest(0, "KILL nick1 reason\r\n");

    ASSERT_EQ(1, logic.getMessageQueue().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NOPRIVILEGES));
}

TEST(IRC_UserOperations, killNeedsOneParameter) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);
    emptyQueue(&logic.getMessageQueue());

    logic.processRequest(0, "KILL\r\n");

    ASSERT_EQ(1, logic.getMessageQueue().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_NEEDMOREPARAMS));
}

TEST(IRC_UserOperations, killingANonExistingNickNameReturnsError) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);
    emptyQueue(&logic.getMessageQueue());

    logic.processRequest(0, "KILL notExistingNick reason\r\n");

    ASSERT_EQ(1, logic.getMessageQueue().size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_NOSUCHNICK));
}

TEST(IRC_UserOperations, killingAUserDisconnectsIt) {
    IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);
    emptyQueue(&logic.getMessageQueue());
    logic.processRequest(0, "OPER oper operPassword\r\n");

    logic.processRequest(0, "KILL nick1 reason\r\n");

    ASSERT_EQ(1, logic.getRegisteredUsers().size());
}

#endif  // TEST_TESTIRC_USEROPERATIONS_HPP_

