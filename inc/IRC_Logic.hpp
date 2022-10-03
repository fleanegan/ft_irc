#ifndef INC_IRC_LOGIC_HPP_
#define INC_IRC_LOGIC_HPP_
#include <string>
#include <vector>

struct User{
	std::string nick;
	std::string name;

	User(): nick("JD"), name("John Doe"){
	}

	User(const std::string &nick, const std::string &name):
		nick(nick), name(name) {
	}
};

class IRC_Logic {
private:
	std::string _remain;
	std::vector<User> _users;
public:
	IRC_Logic();
	IRC_Logic(const IRC_Logic &other);
	IRC_Logic &operator=(const IRC_Logic &);
	~IRC_Logic();
	void cleanupName(const std::string &name);
	void removeMessageTermination(const std::string &message);
	void receive(const std::string &string);
	std::string getNextCommand();
	std::vector<User> getUsers();
	std::vector<std::string> splitMessage(std::string string) const;
};

#endif //INC_IRC_LOGIC_HPP_
