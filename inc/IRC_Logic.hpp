#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>
#include <queue>
#include "./IRC_User.hpp"
#include "./return_code.hpp"

class IRC_Logic {
private:
    std::vector<int> _returnCodes;
    std::string _password;
    std::vector<IRC_User> _users;

	IRC_Logic();
public:
    explicit IRC_Logic(const std::string &password);
    IRC_Logic(const IRC_Logic &other);
    IRC_Logic &operator=(const IRC_Logic &);
    ~IRC_Logic();
	std::string processInput(int fd, const std::string &input, const std::string &hostName);
	std::vector<IRC_User> getRegisteredUsers();
	IRC_User *getUserByFd(const int &fd);
	std::vector<int> getReturnCodes();

private:
    void cleanupName(std::string *name);
	void removeMessageTermination(std::string *message);
    std::vector<std::string> extractFirstMessage(IRC_User *user);
    std::string buildFullName(const std::vector<std::string> &splitMessageVector);
    bool isUserMessage(const std::vector<std::string> &splitMessageVector) const;
    bool isNickAlreadyPresent(const std::string &nick);
    bool isNickMessage(const std::vector<std::string> &splitMessageVector) const;
    std::string stringToLower(const std::string &input) const;
	std::string processIncomingMessage(const std::vector<std::string> &splitMessageVector, IRC_User *user);
	std::string processNickMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processUserMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	std::string processPassMessage(IRC_User *user, const std::vector<std::string> &splitMessageVector);
	bool userIsRegistered(IRC_User* user);
	bool isAuthenticationMessage(const std::vector<std::string> &splitMessageVector) const;
};
#endif //INC_IRC_LOGIC_HPP_
