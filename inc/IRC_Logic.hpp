#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>
#include "IRC_Client.hpp"

class IRC_Logic {
private:
	std::string _remain;
	std::vector<IRC_Client> _clients;
public:
	IRC_Logic();
	IRC_Logic(const IRC_Logic &other);
	IRC_Logic &operator=(const IRC_Logic &);
	~IRC_Logic();
	void cleanupName(std::string *name);
	void removeMessageTermination(std::string *message);
	void receive(const std::string &string);

    std::vector<IRC_Client> getClients();
	std::vector<std::string> splitMessage(std::string string) const;
	std::string buildFullName(const std::vector<std::string> &splitMessageVector);
	bool isMessageClientCreation(const std::vector<std::string> &splitMessageVector) const;

    bool isClientAlreadyPresent(const std::string &nick);
};

#endif //INC_IRC_LOGIC_HPP_
