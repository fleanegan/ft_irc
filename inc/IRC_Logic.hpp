#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>
#include <queue>
#include "./IRC_User.hpp"
#include "./return_code.hpp"
#include "./utils.hpp"
#include "IRC_Message.hpp"

class IRC_Logic {
private:
    std::string _password;
    std::vector<IRC_User> _users;
	std::queue<IRC_Message> _messageQueue;

	IRC_Logic();
public:
	typedef std::vector<IRC_User>::iterator UserIterator;

    explicit IRC_Logic(const std::string &password);
    IRC_Logic(const IRC_Logic &other);
    IRC_Logic &operator=(const IRC_Logic &);
    ~IRC_Logic();
	std::string processInput(int fd, const std::string &input);
	std::vector<IRC_User> getRegisteredUsers();
	UserIterator getUserByFd(const int &fd);
	UserIterator getUserByNick(const std::string &nick);
	std::queue<IRC_Message> &getMessageQueue();
	void disconnectUser( int fd );

private:
    void cleanupName(std::string *name);
	void removeMessageTermination(std::string *message);
    std::vector<std::string> extractFirstMessage(IRC_User *user);
	bool isNickAlreadyPresent(const std::string &nick);
	bool isUserRegistered(IRC_User* user);
	std::string welcomeNewUser(IRC_User *user);
	std::string processIncomingMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processNickMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processUserMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processPassMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processPingMessage(const std::vector<std::string> &splitMessageVector);
	std::string processPrivMsgMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processWhoIsMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
};
#endif //INC_IRC_LOGIC_HPP_
