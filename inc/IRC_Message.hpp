//
// Created by fschlute on 10/7/22.
//

#ifndef FT_IRC_IRC_MESSAGE_HPP
#define FT_IRC_IRC_MESSAGE_HPP

#include <queue>
#include <string>
#include "utils.hpp"
#include "IRC_User.hpp"

class IRC_Message {
private:
	IRC_Message();
public:
	std::queue<int> recipients;
	IRC_User const* sender;
	std::string content;

    IRC_Message(const std::queue<int> &recipients, std::vector<std::string> splitMessageVector, IRC_User *sender);
    IRC_Message(const std::queue<int> &recipients, const std::string& messageContent, IRC_User *sender);
	IRC_Message(const IRC_Message &other);
	IRC_Message &operator=(const IRC_Message &);
	~IRC_Message();
	std::string buildMessageContent(const std::vector<std::string> &splitMessageVector) const;
};

#endif //FT_IRC_IRC_MESSAGE_HPP
