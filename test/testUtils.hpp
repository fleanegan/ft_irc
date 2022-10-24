#ifndef TEST_TESTUTILS_HPP_
#define TEST_TESTUTILS_HPP_
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include "../inc/IRC_Logic.hpp"
#include "../inc/IRC_User.hpp"
#include "../inc/return_code.hpp"

void authenticate(
		IRC_Logic *logic, int fd, const std::string &password ) {
    logic->addUser(fd, "127.0.0.1");
    logic->processRequest(fd, "PASS " + password + "\r\n");
}

void setUser(IRC_Logic *logic,
		int fd, const std::string &userName, const std::string &fullName) {
	logic->processRequest(fd, "USER "
			+ userName + " " + userName + " * :" + fullName + "\r\n");
}

void setNick(IRC_Logic *logic, int fd, const std::string &nick) {
	logic->processRequest(fd, "NICK " + nick + "\r\n");
}

void authenticateAndSetNick(IRC_Logic *logic,
		int fd, const std::string &password, const std::string &nick) {
    authenticate(logic, fd, password);
    setNick(logic, fd, nick);
}

void registerUser(IRC_Logic *logic,
		int fd,
		const std::string &password,
		const std::string &nick,
		const std::string &userName,
		const std::string &fullName) {
	authenticate(logic, fd, password);
	setNick(logic, fd, nick);
	setUser(logic, fd, userName, fullName);
}


bool isValidUserRegistrationResponse(const std::string &returnMessage) {
	return 	returnMessage.find(RPL_WELCOME) != std::string::npos &&
			returnMessage.find(RPL_YOURHOST) != std::string::npos &&
			returnMessage.find(RPL_CREATED) != std::string::npos &&
			returnMessage.find(RPL_MYINFO) != std::string::npos &&
			returnMessage.find(RPL_ISUPPORT) != std::string::npos;
}

bool responseContains(
		const std::string &returnMessage, const std::string &code) {
	return 	returnMessage.find(code) != std::string::npos;
}

void registerDummyUser(IRC_Logic *logic, int beginFd, int endFd) {
	std::string nick("nick");
	std::string userName("username");
	std::string fullName("Full Name");
	std::string indexString;

	while (beginFd < endFd) {
		std::stringstream ss;
		ss << beginFd;
		ss >> indexString;
		registerUser(logic, beginFd, "password",
				nick + indexString,
				userName + indexString,
				fullName + indexString);
        ++beginFd;
	}
}

void registerMembersAndJoinToChannel(
		IRC_Logic *logic, int beginFd, int endFd,
		const std::string &channelName) {
    registerDummyUser(logic, beginFd, endFd);
    while (beginFd < endFd) {
        logic->processRequest(beginFd, "JOIN " + channelName + "\r\n");
        ++beginFd;
    }
}

int countSubStrings(const std::string &string, const std::string &toFind) {
	int occurrences = 0;

	std::string::size_type pos = 0;
	while ((pos = string.find(toFind, pos)) != std::string::npos) {
		++occurrences;
		pos += toFind.length();
	}
	return occurrences;
}

int countMessageContaining(
		std::queue<IRC_Message> messageQueue, const std::string &toCount) {
	int count = 0;
	while (!messageQueue.empty()) {
		if (messageQueue.front().content.find(toCount) != std::string::npos)
			++count;
		messageQueue.pop();
	}
	return count;
}

std::vector<int> gatherAllRecipientsOfMessageQueue(IRC_Logic *logic) {
    std::vector<int>recipientsOfNickChange;
    while (logic->getMessageQueue().empty() == false) {
        IRC_Message tmp = logic->getMessageQueue().front();
        while (tmp.recipients.empty() == false) {
            recipientsOfNickChange.push_back(tmp.recipients.front());
            tmp.recipients.pop();
        }
        logic->getMessageQueue().pop();
    }
    return recipientsOfNickChange;
}

template <typename T>
void emptyQueue(std::queue<T>*queue) {
    std::queue<IRC_Message> test;
    queue->swap(test);
}


#endif     // TEST_TESTUTILS_HPP_
