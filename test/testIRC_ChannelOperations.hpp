#ifndef TEST_TESTIRC_CHANNELOPERATIONS_HPP_
#define TEST_TESTIRC_CHANNELOPERATIONS_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_ChannelOperations, joiningAChannelRequiresName) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "JOIN\r\n");

	ASSERT_TRUE(countSubStrings(logic.getMessageQueue().back().content,
				ERR_NEEDMOREPARAMS));
}

TEST(IRC_ChannelOperations,
		joiningANonExistingChannelCreatesAChannelAccordingToTheGivenName) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "JOIN mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
	ASSERT_STREQ("mychan", logic.getChannels().front().name.c_str());
}

TEST(IRC_ChannelOperations,
		joiningANonExistingCreatesAChannelAndIgnoreHashtagsign) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "JOIN #mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
	ASSERT_STREQ("mychan", logic.getChannels().front().name.c_str());
}

TEST(IRC_ChannelOperations, JoiningAnExistingChannelDoesNotCreateNewChannel) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(0, "JOIN #mychan\r\n");
	result = logic.processInput(1, "JOIN #mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
}

TEST(IRC_ChannelOperations, allMembersOfAChannelButTheSenderReceiveTheMessage) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);
	logic.processInput(0, "JOIN #mychan\r\n");
	logic.processInput(1, "JOIN #mychan\r\n");

	logic.processInput(1, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"PRIVMSG #mychan messageContent"));
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
}

TEST(IRC_ChannelOperations, tryingToSendToChannelYouAreNotAPartOfReturnsError) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);
	logic.processInput(0, "JOIN #mychan\r\n");

	result = logic.processInput(1, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				ERR_CANNOTSENDTOCHAN));
}

TEST(IRC_ChannelOperations, ifNoRecipientInChannelDoesNotSendError) {
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);
	logic.processInput(0, "JOIN #mychan\r\n");
	int before = logic.getMessageQueue().size();

	logic.processInput(0, "PRIVMSG #mychan messageContent\r\n");

	int after = logic.getMessageQueue().size();
	ASSERT_EQ(before, after);
}

TEST(IRC_ChannelOperations,
		disonnectedMemberGetsRemovedFromChannelAndTheirFdIsReused) {
	IRC_Logic logic("password");
	registerMembersAndJoinToChannel(&logic, 3);
	logic.disconnectUser(0, "leaving");

	registerDummyUser(&logic, 1);
	logic.processInput(1, "PRIVMSG #chan messageContent\r\n");

	ASSERT_FALSE(logic.getMessageQueue().empty());
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
	ASSERT_EQ(2, logic.getMessageQueue().back().recipients.front());
}

TEST(IRC_ChannelOperations, joiningAChannelNotifiesOther) {
	IRC_Logic logic("password");

	registerMembersAndJoinToChannel(&logic, 2);

	ASSERT_EQ(2, countMessageContaining(logic.getMessageQueue(), "JOIN"));
	ASSERT_EQ(2, countMessageContaining(logic.getMessageQueue(), RPL_NAMREPLY));
}

TEST(IRC_ChannelOperations, disconnectedMemberNotifiesOtherMembers) {
	IRC_Logic logic("password");
	registerMembersAndJoinToChannel(&logic, 2);

	logic.processInput(0, "QUIT :leaving\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"QUIT"));
	ASSERT_FALSE(logic.getMessageQueue().empty());
	ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
	ASSERT_TRUE(responseContains(logic.getMessageQueue().back().content,
				"nick0"));
}

// TEST(IRC_ChannelOperations, channelNamesAreCaseInsensitive) {
// TEST(IRC_ChannelOperations, channelNamesCanContainDifferentPrefixes) {
// TEST(IRC_ChannelOperations, channelNamesAreMax50CharsLong) {

#endif  // TEST_TESTIRC_CHANNELOPERATIONS_HPP_
