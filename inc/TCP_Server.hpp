#ifndef INC_TCP_SERVER_HPP_
#define INC_TCP_SERVER_HPP_

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include <string>


class TCP_Server {
	protected:
		int	_fd;
		int	_opt;
		int	_port;
		int	_addrLen;
		struct sockaddr_in	_addr;

	public:
		TCP_Server( void );
		TCP_Server( int port, int opt);
		TCP_Server( const TCP_Server& other );

		TCP_Server& operator = ( const TCP_Server& other);

		~TCP_Server( void );

		virtual void	getMessage( char* buffer ) = 0;
		virtual std::string	sendMessage( void ) = 0;

		void	host( void );

		int	getFd( void ) const;
		int	getOpt( void ) const;
		int	getPort( void ) const;
		int	getAddrLen( void ) const;
		struct sockaddr_in	getAddr( void ) const;

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
