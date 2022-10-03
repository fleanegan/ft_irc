#include <vector>
#include "../inc/IRC_Logic.hpp"

IRC_Logic::IRC_Logic() {

}

IRC_Logic::IRC_Logic(const IRC_Logic &other) {
	(void) other;
}

IRC_Logic &IRC_Logic::operator=(const IRC_Logic &) {
	return *this;
}

IRC_Logic::~IRC_Logic() {

}

void IRC_Logic::receive(const std::string &string) {
	std::vector<std::string> splitMessageVector;
	User result;

	_remain += string;
	splitMessageVector = splitMessage(string);
	if (splitMessageVector.size() >= 5 && splitMessageVector[0] == "USER")
		_users.push_back(User());
}

std::vector<std::string> IRC_Logic::splitMessage(std::string string) const {
	size_t pos;

	std::vector<std::string> result;
	while ((pos = string.find(" ")) != std::string::npos) {
		result.push_back (string.substr(0, pos));
		string.erase(0, pos + 1);
	}
	result.push_back(string);
	return result;
}

std::string IRC_Logic::getNextCommand() {
	if (_remain.find("\r\n") != std::string::npos)
		return "\r\n";
	return "";
}

std::vector<User> IRC_Logic::getUsers() {
	return _users;
}
