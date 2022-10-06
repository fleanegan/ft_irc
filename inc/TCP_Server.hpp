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


class TCP_Server {
	protected:
		std::vector<struct pollfd> _fds;
		struct sockaddr_in	_servAddr;
		int	_port;

	public:
		TCP_Server( void );
		TCP_Server( int port);
		TCP_Server( const TCP_Server& other );

		TCP_Server& operator = ( const TCP_Server& other);

		~TCP_Server( void );

		virtual std::string	processMessage( int fd, const std::string& buffer ) = 0;

		void	host( void );

		class socketFailedException: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		class couldNotBindException: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		class brokenConnectionException: public std::exception {
			public:
				virtual const char* what() const throw();
		};
};

#endif // INC_TCP_SERVER_HPP_
