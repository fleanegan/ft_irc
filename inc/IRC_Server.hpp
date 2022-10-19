#ifndef INC_IRC_SERVER_HPP_
#define INC_IRC_SERVER_HPP_

#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <queue>

#include "./IRC_Logic.hpp"
#include "./TCP_Server.hpp"

class IRC_Server : public TCP_Server {
	private:
		IRC_Logic _logic;

		IRC_Server(const IRC_Server &other);
	public:
		IRC_Server(void);
		IRC_Server(int port, const std::string &password);

		~IRC_Server();

		IRC_Server &operator=(const IRC_Server &other);

		void processMessage(int fd, const std::string &buffer);

		void onDisconnect(int fd);

		void distributeMessages();

        void onConnect(int fd, const std::string &hostIp);

    void handleDisconnectionsFromLogic();
};

#endif  // INC_IRC_SERVER_HPP_
