#ifndef INC_RETURN_CODE_HPP_
#define INC_RETURN_CODE_HPP_

# define RPL_WELCOME "001"
# define RPL_YOURHOST "002"
# define RPL_CREATED "003"
# define RPL_MYINFO "004"
# define RPL_MYINFO "004"
# define RPL_ISUPPORT "005"

#include <string>
#include "./IRC_User.hpp"

std::string rplWelcome(const IRC_User& user);
std::string rplYourHost();
std::string rplCreated();
std::string rplMyInfo();
std::string rplISupport();
#endif //INC_RETURN_CODE_HPP_
