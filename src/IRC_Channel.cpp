#include "../inc/IRC_Channel.hpp"

IRC_Channel::IRC_Channel(const std::string &channelName) {
	if (channelName[0] == '#')
		name = channelName.substr(1, channelName.size());
	else
		name = channelName;
}

bool IRC_Channel::operator==(const IRC_Channel &rhs) const {
	return name == rhs.name;
}

bool IRC_Channel::operator!=(const IRC_Channel &rhs) const {
	return !(rhs == *this);
}