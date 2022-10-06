#include "../inc/return_code.hpp"
#include <string>

std::string generateResponse(const std::string &returnCode, const std::string &explanation) {
	std::string result;

	result += returnCode + " :";
	result += explanation;
	result += "\r\n";
	return result;
}

std::string stringToLower(const std::string &input) {
	std::string result;

	for (std::string::const_iterator it = input.begin();
		 it != input.end(); ++it) {
		result += tolower(*it);
	}
	return result;
}
