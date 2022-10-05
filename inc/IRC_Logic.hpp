#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>
#include <queue>
#include "./IRC_User.hpp"
#include "./return_code.hpp"

class IRC_Logic {
private:
    std::string _remain;
    std::vector<int> _returnCodes;
    std::string _password;
    std::vector<IRC_User> _users;

	IRC_Logic();
public:
    explicit IRC_Logic(const std::string &password);
    IRC_Logic(const IRC_Logic &other);
    IRC_Logic &operator=(const IRC_Logic &);
    ~IRC_Logic();
	void receive(int fd, const std::string &string);
	std::vector<IRC_User> getRegisteredUsers();
	IRC_User *getUserByFd(const int &fd);
	std::vector<int> getReturnCodes();

private:
    void cleanupName(std::string *name);
	void removeMessageTermination(std::string *message);
    std::vector<std::string> extractFirstMessage(std::string string);
    std::string buildFullName(const std::vector<std::string> &splitMessageVector);
    bool isUserMessage(const std::vector<std::string> &splitMessageVector) const;
    bool isNickAlreadyPresent(const std::string &nick);
    bool isNickMessage(const std::vector<std::string> &splitMessageVector) const;
    std::string stringToLower(const std::string &input) const;
	void processIncommingMessage(int fd, const std::vector<std::string> &splitMessageVector);
	void processNickMessage(int fd, const std::vector<std::string> &splitMessageVector);
	void processUserMessage(const int& fd, const std::vector<std::string> &splitMessageVector);
	void processPassMessage(int fd, const std::vector<std::string> &splitMessageVector);
	bool userIsRegistered(IRC_User* user);
};
#endif //INC_IRC_LOGIC_HPP_
