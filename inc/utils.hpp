#ifndef FT_IRC_UTILS_HPP
#define FT_IRC_UTILS_HPP
#include <string>


std::string stringToLower(const std::string &input);

std::string generateResponse(const std::string& returnCode, const std::string& explanation);

#endif //FT_IRC_UTILS_HPP
