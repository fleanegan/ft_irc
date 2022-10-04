#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>
#include "./IRC_User.hpp"

class IRC_Logic {
private:
    std::string _remain;
    std::string _response;
    std::string _password;
    std::vector<IRC_User> _users;
public:
    IRC_Logic();

    explicit IRC_Logic(const std::string &password);

    IRC_Logic(const IRC_Logic &other);

    IRC_Logic &operator=(const IRC_Logic &);

    ~IRC_Logic();

    void cleanupName(std::string *name);

    void removeMessageTermination(std::string *message);

    void receive(int fd, const std::string &string);

    std::vector<IRC_User> getRegisteredUsers();

    std::vector<std::string> splitMessage(std::string string) const;

    std::string buildFullName(const std::vector<std::string> &splitMessageVector);

    bool isUserMessage(const std::vector<std::string> &splitMessageVector) const;

    bool isNickAlreadyPresent(const std::string &nick);

    IRC_User *getUserByFd(const int &fd);

    std::string popMessage();

    bool isNickMessage(const std::vector<std::string> &splitMessageVector) const;

    std::string stringToLower(const std::string &input) const;
};
#endif //INC_IRC_LOGIC_HPP_
