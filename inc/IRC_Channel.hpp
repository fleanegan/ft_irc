#ifndef INC_IRC_CHANNEL_HPP_
#define INC_IRC_CHANNEL_HPP_

#include <string>
#include <vector>
#include <queue>
#include "./utils.hpp"
#include "./IRC_User.hpp"
#include "./IRC_Message.hpp"
#include "./return_code.hpp"

struct IRC_Channel {
	typedef std::vector<IRC_Channel>::iterator ChannelIterator;

	std::string name;
	std::vector<IRC_User *> members;

	explicit IRC_Channel(const std::string &channelName);
	bool operator==(const IRC_Channel &rhs) const;
	bool operator!=(const IRC_Channel &rhs) const;

	void appendJoinMessages(
			std::queue<IRC_Message> *messageQueue, const IRC_User &newMember);

	void appendJoinReplyToNewMember(
			std::queue<IRC_Message> *messageQueue, const IRC_User &newMember);

	void appendJoinNotificationToAllMembers(
			std::queue<IRC_Message> *messageQueue, const IRC_User &newMember);

	static std::string getChannelName(const std::string &name);

	private:
	IRC_Channel();
};

#endif  // INC_IRC_CHANNEL_HPP_
