#include "../inc/return_code.hpp"

std::string rplWelcome(const IRC_User& user) {
	return "001 Welcome to the Internet Relay Network" + user.nick +
		"!" + user.name + "@" + user.hostName + "\r\n";
}

std::string rplYourHost(){
//	return "002 Your host is <servername>, running version " + "<ver>" + "\r\n";
	return "002 Your host is <servername>, running version <ver>\r\n";
}
std::string rplCreated(){
//	return "003 This server was created " + "<date>" + "\r\n";
	return "003 This server was created <date>\r\n";
};
std::string rplMyInfo(){
//	return "004 " + "<servername>" +  "<version>" + " " + "<available user modes>" + " " + "<available channel modes>" + "\r\n";
	return "004 <servername> <version> <available user modes> <available channel modes>\r\n";
}
std::string rplISupport(){
	return "005 <servername> *(<feature>(=<value>)) :are supported by this server\r\n";
}
