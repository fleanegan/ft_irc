#include "../inc/return_code.hpp"

std::string generateResponse(const std::string &returnCode, const std::string &explanation) {
	std::string result;

	result += returnCode + " :";
	result += explanation;
	result += "\r\n";
	return result;
}
