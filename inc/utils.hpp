#ifndef INC_UTILS_HPP_
#define INC_UTILS_HPP_
#include <string>
#include <vector>

std::string stringToLower(const std::string &input);
std::string generateResponse(
		const std::string& returnCode, const std::string& explanation);
void removeLeadingColon(std::string *input);
std::string concatenateContentFromIndex(
		int startIndex, const std::vector<std::string> &splitMessageVector);

bool isMatchingWildcardExpression(const std::string &stringToMatch, const std::string &expression);

std::vector<std::string> splitOnToken(std::string message, const std::string &splitPattern);

#endif  // INC_UTILS_HPP_
