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

bool isMatchingWildcardExpressionRecursion(
        const char *stringToMatch, const char *expression) {
    if (*expression == '\0' && *stringToMatch == '\0')
        return true;
    if (*expression == '*') {
        while (*(expression + 1) == '*')
            expression++;
    }
    if (*expression == '*' && *(expression + 1) != '\0'
        && *stringToMatch == '\0')
        return false;
    if (*expression == '?' || *expression == *stringToMatch)
        return isMatchingWildcardExpressionRecursion(
                stringToMatch + 1, expression + 1);
    if (*expression == '*')
        return isMatchingWildcardExpressionRecursion(
                stringToMatch, expression + 1)
               || isMatchingWildcardExpressionRecursion(stringToMatch + 1,
                                                        expression);
    return false;
}

bool isMatchingWildcardExpression(
        const std::string &stringToMatch, const std::string &expression) {
    return isMatchingWildcardExpressionRecursion(
            stringToMatch.c_str(), expression.c_str());
}

std::vector<std::string> splitOnToken(std::string message, const std::string &splitPattern) {
    std::vector<std::string> result;
    size_t pos;

    while ((pos = message.find(splitPattern)) != std::string::npos) {
        result.push_back(message.substr(0, pos));
        message.erase(0, pos + 1);
    }
    if (message.empty() == false)
        result.push_back(message);
    return result;
}
