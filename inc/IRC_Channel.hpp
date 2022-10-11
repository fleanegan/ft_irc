#ifndef INC_IRC_CHANNEL_HPP_
#define INC_IRC_CHANNEL_HPP_
#include <string>
#include <vector>
#include "./utils.hpp"

struct IRC_Channel{
	typedef std::vector<IRC_Channel>::iterator ChannelIterator;

	std::string name;
	std::vector<int> membersFd;

	explicit IRC_Channel(const std::string& channelName);
	bool operator==(const IRC_Channel &rhs) const;
	bool operator!=(const IRC_Channel &rhs) const;

	static std::string  getChannelName(const std::string &name);

	private:
	IRC_Channel();
};



#endif  // INC_IRC_CHANNEL_HPP_
