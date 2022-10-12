#ifndef INC_IRC_MESSAGE_HPP_
#define INC_IRC_MESSAGE_HPP_

#include <queue>
#include <vector>
#include <string>
#include "./utils.hpp"
#include "./IRC_User.hpp"

class IRC_Message {
	private:
		IRC_Message();
	public:
		std::queue<int> recipients;
		IRC_User const* sender;
		std::string content;

		IRC_Message(const std::queue<int> &recipients,
					std::vector<std::string> splitMessageVector, const IRC_User *sender);
		IRC_Message(const std::queue<int> &recipients,
					const std::string& messageContent, const IRC_User *sender);
		IRC_Message(int recipientFd, const std::string& messageContent,
				const std::string& prefix);
		IRC_Message(const IRC_Message &other);
		IRC_Message &operator=(const IRC_Message &);
		~IRC_Message();
		std::string buildMessageContent(
			const std::vector<std::string> &splitMessageVector) const;
};

#endif  // INC_IRC_MESSAGE_HPP_
