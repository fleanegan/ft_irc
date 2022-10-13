#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>
#include <queue>
#include "./IRC_User.hpp"
#include "./return_code.hpp"
#include "./utils.hpp"
#include "./IRC_Message.hpp"
#include "./IRC_Channel.hpp"

class IRC_Logic {
	private:
		std::string _password;
		std::vector<IRC_User> _users;
		std::vector<IRC_Channel> _channels;
		std::vector<IRC_User> _prevUsers;
		std::queue<IRC_Message> _messageQueue;
		std::string _returnMessage;

	public:
		explicit IRC_Logic(const std::string &password);
		~IRC_Logic();
		std::string processRequest(int fd, const std::string &input);
		IRC_User::UserIterator getUserByFd(const int &fd);
		IRC_User::UserIterator getUserByNick(const std::string &nick);
		std::queue<IRC_Message> &getMessageQueue();
		void disconnectUser(int fd, const std::string &reason);
		std::vector<IRC_User>& getRegisteredUsers();
		std::vector<IRC_Channel>& getChannels();

		void addUser(int fd, const std::string &hostIp);

	private:
		IRC_Logic();
		IRC_Logic(const IRC_Logic &other);
		IRC_Logic &operator=(const IRC_Logic &);
		std::vector<std::string> extractFirstMessage(IRC_User *user);
		bool isNickAlreadyPresent(const std::string &nick);
		bool isUserRegistered(IRC_User* user);
		std::string welcomeNewUser(IRC_User *user);
		void processIncomingMessage(
				IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processNickMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processUserMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processPassMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processPingMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processPrivMsgMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processWhoIsMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processWhoWasMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		std::string generateWhoWasMessage(
				const std::vector<std::string> &splitMessageVector) const;
		void processJoinMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processQuitMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void processPartMessage(IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		IRC_Channel::ChannelIterator getChannelByName(const std::string &name);
		std::queue<int> fetchChannelRecipients(
				const IRC_User &user, const std::string &channelName);
		std::queue<int> fetchSingleRecipient(
				int fd, const std::vector<std::string> &splitMessageVector);
		void processModeMessage(const IRC_User *user,
				const std::vector<std::string> &splitMessageVector);
		void appendMessage(const IRC_Message &reply);

    void broadCastToAllUsers(
            const std::string &message, const IRC_User &user);
};
#endif  // INC_IRC_LOGIC_HPP_
