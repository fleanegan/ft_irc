#include "../inc/utils.hpp"
#include "../inc/return_code.hpp"

std::string generateResponse(
		const std::string &returnCode, const std::string &explanation) {
	std::string result;

	result += returnCode + " ";
	result += explanation;
	result += "\r\n";
	return result;
}

char toScandinavianLower(const char &c) {
	if (c >= 'A' && c <= '^')
		return 32 + c;
	return c;
}

std::string stringToLower(const std::string &input) {
	std::string result;

	for (std::string::const_iterator it = input.begin();
			it != input.end(); ++it) {
		result += toScandinavianLower(*it);
	}
	return result;
}

void removeLeadingColon(std::string *input) {
	if ((*input)[0] == ':')
		*input = input->substr(1, input->size());
}

std::string concatenateContentFromIndex(
		int startIndex, const std::vector<std::string> &splitMessageVector) {
	std::string name;

	for (size_t i = startIndex; i < splitMessageVector.size(); i++) {
		name += splitMessageVector[i];
		if (i < splitMessageVector.size() - 1)
			name += " ";
	}
	return name;
}

bool isMatchingWildcardExpression(const std::string &input, const std::string &expression) {
	unsigned long untilWildcardPosition;
	unsigned long rest;
	unsigned long wildcardPosition;

	wildcardPosition = expression.find("*");
	if (wildcardPosition != std::string::npos) {
		untilWildcardPosition = input.find(expression.substr(0, wildcardPosition));
		rest = input.find(expression.substr(wildcardPosition + 1));

		if (untilWildcardPosition != std::string::npos && rest != std::string::npos)
			return true;
	}
	return expression == input;
}

	// find first wildcard position in expression
	// if contains wildcard
		// check if equal until wildcard
		// check if equal until end
		// if both check are passing it is a match
