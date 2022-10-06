#ifndef INC_RETURN_CODE_HPP_
#define INC_RETURN_CODE_HPP_

# define RPL_WELCOME "001"
# define RPL_YOURHOST "002"
# define RPL_CREATED "003"
# define RPL_MYINFO "004"
# define RPL_ISUPPORT "005"
# define ERR_NONICKNAMEGIVEN "431"
# define ERR_ERRONEOUSNICK "432"
# define ERR_NICKNAMEINUSE  "433"
# define ERR_NEEDMOREPARAMS "461"
# define ERR_ALREADYREGISTERED "462"
# define ERR_PASSWDMISMATCH "464"
# define ERR_CONNECTWITHOUTPWD "ERROR"

#include <string>
#include "./IRC_User.hpp"

std::string generateResponse(const std::string& returnCode, const std::string& explanation);
#endif //INC_RETURN_CODE_HPP_
