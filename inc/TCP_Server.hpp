#ifndef INC_TCP_SERVER_HPP_
#define INC_TCP_SERVER_HPP_

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <cstring>
#include <exception>
#include <string>
#include <vector>
#include <queue>

#define BUFFER_LEN 1024
#define _VERBOSE true

class TCP_Server {
	private:
		TCP_Server(const TCP_Server &other);

		TCP_Server &operator=(const TCP_Server &other);

	protected:
		std::vector<struct pollfd> _fds;
		struct sockaddr_in _servAddr;
		int _port;
        std::queue<int> _fdsToCloseAfterUpdate;

		virtual void onDisconnect(int fd) = 0;

	public:
		TCP_Server(void);

		explicit TCP_Server(int port);

		virtual ~TCP_Server(void);

		virtual void processMessage(int fd, const std::string &buffer) = 0;

		void host(void);

		class socketFailedException : public std::exception {
			public: // NOLINT :Not respecting space indentation in this project
				virtual const char *what() const throw();
		};

		class couldNotBindException : public std::exception {
			public: // NOLINT :Not respecting space indentation in this project
				virtual const char *what() const throw();
		};

		class brokenConnectionException : public std::exception {
			public: // NOLINT :Not respecting space indentation in this project
				virtual const char *what() const throw();
		};

		void setUpTcpSocket(int port);

		void saveConnectionInfo(int fd, const std::string &hostIp);

		virtual void onConnect(int fd, const std::string &basicString) = 0;

    void closeFdHook();
};

#endif  // INC_TCP_SERVER_HPP_
