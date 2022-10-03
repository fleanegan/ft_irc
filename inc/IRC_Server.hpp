#ifndef INC_IRC_SERVER_
#define INC_IRC_SERVER_

#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <queue>

#include "./TCP_Server.hpp"

class IRC_Server: public TCP_Server {
	private:
		std::queue<std::string>	_commands;
		std::string	_remain;

		std::string	retrieveMessage( char* buffer );
		std::string	processCommand( std::string command );
	public:
		IRC_Server( void );
		IRC_Server( int port, int opt);
		IRC_Server( const IRC_Server& other);

		~IRC_Server();

		IRC_Server& operator = (const IRC_Server& other);

		void	getMessage( char* buffer );
		std::string	sendMessage( void );
};

#endif // INC_IRC_SERVER_
