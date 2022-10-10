#include "../inc/IRC_Channel.hpp"

IRC_Channel::IRC_Channel(const std::string &channelName): name(getChannelName(channelName)) {
}

bool IRC_Channel::operator==(const IRC_Channel &rhs) const {
	return name == rhs.name;
}

bool IRC_Channel::operator!=(const IRC_Channel &rhs) const {
	return !(rhs == *this);
}

std::string  IRC_Channel::getChannelName( const std::string &name) {
	if (name[0] == '#')
		return name.substr(1, name.size());
	return name;
}
