#ifndef TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_
#define TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_

#include <string>
#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_ChannelOperations, joiningAChannelRequiresName){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "JOIN\r\n");

	ASSERT_TRUE(countSubStrings(result, ERR_NEEDMOREPARAMS));
}

TEST(IRC_ChannelOperations, joiningANonExistingChannelCreatesAChannelAccordingToTheGivenName){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "JOIN mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
	ASSERT_STREQ("mychan", logic.getChannels().front().name.c_str());
}

TEST(IRC_ChannelOperations, joiningANonExistingCreatesAChannelAndIgnoreHashtagsign){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);

	result = logic.processInput(0, "JOIN #mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
	ASSERT_STREQ("mychan", logic.getChannels().front().name.c_str());
}

TEST(IRC_ChannelOperations, JoiningAnExistingChannelDoesNotCreateNewChannel){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);

	result = logic.processInput(0, "JOIN #mychan\r\n");
	result = logic.processInput(1, "JOIN #mychan\r\n");

	ASSERT_EQ(1, logic.getChannels().size());
}

TEST(IRC_ChannelOperations, allMembersOfAChannelButTheSenderReceiveTheMessage){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);
	logic.processInput(0, "JOIN #mychan\r\n");
	logic.processInput(1, "JOIN #mychan\r\n");

	logic.processInput(1, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content, "PRIVMSG #mychan messageContent"));
	ASSERT_EQ(1, logic.getMessageQueue().front().recipients.size());
}

TEST(IRC_ChannelOperations, tryingToSendToChannelYouAreNotAPartOfReturnsError){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 2);
	logic.processInput(0, "JOIN #mychan\r\n");

	result = logic.processInput(1, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(responseContains(result, ERR_CANNOTSENDTOCHAN));
}

TEST(IRC_ChannelOperations, ifNoRecipientInChannelDoesNotSendError){
	IRC_Logic logic("password");
	std::string result;
	registerDummyUser(&logic, 1);
	logic.processInput(0, "JOIN #mychan\r\n");

	logic.processInput(0, "PRIVMSG #mychan messageContent\r\n");

	ASSERT_TRUE(logic.getMessageQueue().empty());

}

TEST(IRC_ChannelOperations, disonnectedMemberGetsRemovedFromChannelAndTheirFdIsReused){
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 3);
    logic.disconnectUser(0);

    registerDummyUser(&logic, 1);
    logic.processInput(1, "PRIVMSG #chan messageContent\r\n");

    ASSERT_FALSE(logic.getMessageQueue().empty());
    ASSERT_EQ(1, logic.getMessageQueue().back().recipients.size());
    ASSERT_EQ(2, logic.getMessageQueue().back().recipients.front());

}

//TEST(IRC_ChannelOperations, joiningAChannelNotifiesOther){
//	IRC_Logic logic("password");
//	std::string result;
//    registerMembersAndJoinToChannel(&logic, 2);
//
//	ASSERT_FALSE(logic.getMessageQueue().empty());
//}

TEST(IRC_ChannelOperations, disonnectedMemberNotifiesOtherMembers){
    IRC_Logic logic("password");
    registerMembersAndJoinToChannel(&logic, 2);
	std::string result;

	result = logic.processInput(0, "QUIT :leaving\r\n");
    logic.disconnectUser(0);

    ASSERT_TRUE(responseContains(result, ERR_CLOSINGLINK));
    ASSERT_FALSE(logic.getMessageQueue().empty());
    ASSERT_EQ(1, logic.getMessageQueue().front().recipients.size());
    ASSERT_TRUE(responseContains(logic.getMessageQueue().front().content, "nick0"));

}

//TEST(IRC_ChannelOperations, channelNamesAreCaseInsensitive){
//TEST(IRC_ChannelOperations, channelNamesCanContainDifferentPrefixes){
//TEST(IRC_ChannelOperations, channelNamesAreMax50CharsLong){

#endif //TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_

