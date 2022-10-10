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

#endif //TEST_TESTIRC_LOGICUSEROPERATIONS_HPP_

