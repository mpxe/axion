#include "user.h"


#include <regex>


matrix::User::User(std::string&& user_id, std::string&& display_name)
    : id_{std::move(user_id)}, display_name_{display_name}
{
  // Extract account name from user id, e.g. user from @user:example.com
  std::regex rx{R"(@(.+):)"};
  std::smatch m;
  if (std::regex_search(id_, m, rx))
    account_name_ = m[1];
}


void matrix::User::set_avatar_url(std::string&& url)
{
  avatar_url_ = std::move(url);
  std::regex rx{R"(mxc://.+/(.+))"};
  std::smatch m;
  if (std::regex_search(avatar_url_, m, rx))
    avatar_id_ = m[1];
}
