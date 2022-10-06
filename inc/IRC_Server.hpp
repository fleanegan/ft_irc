#ifndef INC_IRC_SERVER_HPP_
#define INC_IRC_SERVER_HPP_

#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <queue>

#include "./IRC_Logic.hpp"
#include "./TCP_Server.hpp"

class IRC_Server: public TCP_Server {
	private:
		IRC_Logic _logic;
	public:
		IRC_Server( void );
		IRC_Server( int port, const std::string& password);
		IRC_Server( const IRC_Server& other);

		~IRC_Server();

		IRC_Server& operator = (const IRC_Server& other);

		std::string processMessage(int fd, const std::string& buffer);

};

#endif // INC_IRC_SERVER_HPP_
