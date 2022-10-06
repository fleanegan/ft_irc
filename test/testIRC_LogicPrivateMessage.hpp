#ifndef TEST_TESTIRC_LOGIC_HPP_
#define TEST_TESTIRC_LOGIC_HPP_

#include "./testUtils.hpp"
#include "gtest/gtest.h"

TEST(IRC_LogicPrivateMessage, firstTestw) {
	IRC_Logic logic("password");

	ASSERT_TRUE(logic.getRegisteredUsers().empty());
}

#endif //TEST_TESTIRC_LOGIC_HPP_
