#ifndef TEST_TESTIRC_CHANNELOPERATIONS_HPP_
#define TEST_TESTIRC_CHANNELOPERATIONS_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_ChannelOperations, joiningAChannelRequiresName) {
	IRC_Logic logic("password");
	std::string result;
    registerDummyUser(&logic, 0, 1);

	result = logic.processRequest(0, "JOIN\r\n");

	ASSERT_TRUE(countSubStrings(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_ChannelOperations,
		joiningANonExistingChannelCreatesAChannelAccordingToTheGivenName) {
	IRC_Logic logic("password");
	std::string result;
    registerDummyUser(&logic, 0, 1);

	result = logic.processRequest(0, "JOIN mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
	ASSERT_STREQ("mychan", logic.getChannels().front().name.c_str());
}

TEST(IRC_ChannelOperations,
		joiningANonExistingCreatesAChannelAndIgnoreHashtagsign) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);

	logic.processRequest(0, "JOIN #mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
	ASSERT_STREQ("mychan", logic.getChannels().front().name.c_str());
}

TEST(IRC_ChannelOperations, JoiningAnExistingChannelDoesNotCreateNewChannel) {
	IRC_Logic logic("password");

    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");

	ASSERT_EQ(1, logic.getChannels().size());
}

TEST(IRC_ChannelOperations, allMembersOfAChannelButTheSenderReceiveTheMessage) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#mychan");

	logic.processRequest(1, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"PRIVMSG #mychan messageContent"));
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
}

TEST(IRC_ChannelOperations, tryingToSendToChannelYouAreNotAPartOfReturnsError) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);
	logic.processRequest(0, "JOIN #mychan\r\n");

	logic.processRequest(1, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_CANNOTSENDTOCHAN));
}

TEST(IRC_ChannelOperations, ifNoRecipientInChannelDoesNotSendError) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 1, "#mychan");
    int before = logic.getMessageQueue().size();

	logic.processRequest(0, "PRIVMSG #mychan messageContent\r\n");

	int after = logic.getMessageQueue().size();
	ASSERT_EQ(before, after);
}

TEST(IRC_ChannelOperations,
		disonnectedMemberGetsRemovedFromChannelAndTheirFdIsReused) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 3, "#chan");
	logic.disconnectUser(0, "leaving");

    registerDummyUser(&logic, 0, 1);
	logic.processRequest(1, "PRIVMSG #chan messageContent\r\n");

	ASSERT_FALSE(logic.getMessageQueue().empty());
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
	ASSERT_EQ(2, logic.getMessageQueue().back().recipients.front());
}

TEST(IRC_ChannelOperations, joiningAChannelNotifiesOther) {
	IRC_Logic logic("password");

    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");

	ASSERT_EQ(2, countMessageContaining(logic.getMessageQueue(), "JOIN"));
	ASSERT_EQ(2, countMessageContaining(logic.getMessageQueue(), RPL_NAMREPLY));
}

TEST(IRC_ChannelOperations, leavingChannelDoesNotInvalidateOtherUsers) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");

	logic.processRequest(1, "PRIVMSG #chan messageContent\r\n");
	ASSERT_FALSE(responseContains(logic.getMessageQueue().back().content,
				ERR_CANNOTSENDTOCHAN));
}

TEST(IRC_ChannelOperations, disconnectedMemberNotifiesOtherMembers) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");
    emptyQueue(&logic.getMessageQueue());

	logic.processRequest(0, "QUIT :leaving\r\n");

	ASSERT_FALSE(logic.getMessageQueue().empty());
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content,
                                 "QUIT"));
    ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
                                 ERR_CLOSINGLINK));
}

TEST(IRC_ChannelOperations, channelNamesAreCaseInsensitive) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

	logic.processRequest(1, "JOIN #ChAn\r\n");
	logic.processRequest(0, "JOIN #chan\r\n");
	ASSERT_EQ(1, logic.getChannels().size());
}

TEST(IRC_ChannelOperations, channelNamesCanContainDifferentPrefixes) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 4);

	logic.processRequest(0, "JOIN #chan\r\n");
	logic.processRequest(1, "JOIN &chan\r\n");
	logic.processRequest(2, "JOIN +chan\r\n");
	logic.processRequest(3, "JOIN !chan\r\n");
	ASSERT_EQ(1, logic.getChannels().size());
}

TEST(IRC_ChannelOperations, bracketAndMoreHaveLowercases) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

	logic.processRequest(1, "JOIN #ChAn\r\n");
	logic.processRequest(0, "JOIN #chan\r\n");
	ASSERT_EQ(1, logic.getChannels().size());
}

TEST(IRC_ChannelOperations, channelNamesAreMax50CharsLong) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 2);

	logic.processRequest(1, "JOIN #CHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAN\r\n");
	ASSERT_EQ(49, logic.getChannels().front().name.size());
}

TEST(IRC_ChannelOperations, sendingPartChannelLeavesSaidChannel) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");

    logic.processRequest(0, "PART #chan\r\n");

	ASSERT_EQ(1, logic.getChannels().front().members.size());
}

TEST(IRC_ChannelOperations, partNotifiesOtherMembers) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");

	logic.processRequest(0, "PART #chan\r\n");

	ASSERT_EQ(2, logic.getMessageQueue().back().recipients.size());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content, "PART"));
}

TEST(IRC_ChannelOperations, partWithoutChannelShouldReturnError) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#chan");
	logic.processRequest(0, "PART\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_ChannelOperations, partOnNonExistingChannelShouldReturnError) {
	IRC_Logic logic("password");
    registerDummyUser(&logic, 0, 1);
	logic.processRequest(0, "PART test\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NOSUCHCHANNEL));
}

TEST(IRC_ChannelOperations, partNotJoinedChannelShouldReturnError) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 1, "#chan");
	registerUser(&logic, 1, "password", "nick", "username", "full name");

	logic.processRequest(1, "PART chan\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_NOTONCHANNEL));
}

TEST(IRC_ChannelOperations, EmptyChannelsShouldBeDeleted) {
	IRC_Logic logic("password");
	registerMembersAndJoinToChannel(&logic, 0, 1, "#chan");

	logic.processRequest(0, "PART chan\r\n");

	ASSERT_TRUE(logic.getChannels().empty());
}

TEST(IRC_ChannelOperations, recipientNotFoundSearchInChannels) {
	IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 0, 2, "#mychan");
    int before = logic.getMessageQueue().size();

	emptyQueue(&logic.getMessageQueue());
	logic.processRequest(0, "PRIVMSG mychan messageContent\r\n");

	ASSERT_EQ(1, logic.getMessageQueue().size());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"messageContent"));
}


#endif  // TEST_TESTIRC_CHANNELOPERATIONS_HPP_
