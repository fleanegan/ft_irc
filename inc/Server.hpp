#ifndef INC_SERVER_HPP_
#define INC_SERVER_HPP_

# include "ircserv.hpp"

class Server {
	private:
		int	_fd;
		int	_opt;
		int	_port;
		int	_addrLen;
		struct sockaddr_in	_addr;
	public:
		Server( void );
		Server( int port, int opt);
		Server( const Server& other );

		Server& operator = ( const Server& other);

		~Server( void );

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

#endif // INC_SERVER_HPP_
