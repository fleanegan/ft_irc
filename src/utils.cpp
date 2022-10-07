#include "utils.hpp"
#include "../inc/return_code.hpp"

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

void removeLeadingColon(std::string *input) {
	if ((*input)[0] == ':')
		*input = input->substr(1, input->size());
}

std::string concatenateContentFromIndex(int startIndex, const std::vector<std::string> &splitMessageVector) {
	std::string name;

	for (size_t i = startIndex; i < splitMessageVector.size(); i++) {
		name += splitMessageVector[i];
		if (i < splitMessageVector.size() - 1)
			name += " ";
	}
	return name;
}